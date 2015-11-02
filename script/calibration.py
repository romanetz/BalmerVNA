import cmath
import math
import bisect
import xml.etree.ElementTree as ET

from graph_u2 import calcUxZ, calcRxUI

Rmax = 1000 # 500 Om - этом максимальные цифры которые отображаем

def readXmlData(fileName):
	tree = ET.parse(fileName)
	root = tree.getroot()
	et_data = root.find('data')
	data = []
	for et_h in et_data.iter('h'):
		a = et_h.attrib
		d = {}
		d['F'] = float(a['F'])
		d['q_csin'] = float(a['q_csin'])
		d['q_ccos'] = float(a['q_ccos'])
		d['i_csin'] = float(a['i_csin'])
		d['i_ccos'] = float(a['i_ccos'])
		d['freq'] = float(a['freq'])
		data.append(d)
	return data

def getZFromData(data):
	amplitude = []

	Farr = []
	Zarr = []

	for d in data:
		Farr.append(d['F'])
		Zi = complex(d['i_ccos'], d['i_csin'])
		Zq = complex(d['q_ccos'], d['q_csin']) 
		Zarr.append(Zi/Zq)
	return (Farr, Zarr)

def readXmlZ(fileName):
	data = readXmlData(fileName)
	return getZFromData(data)

def ZtoAmplithude(Zarr):
	Aarr = []
	for z in Zarr:
		Aarr.append(abs(z))
	return Aarr

def ZtoPhase(Zarr):
	Aarr = []
	for z in Zarr:
		Aarr.append(cmath.phase(z))
	return Aarr

def checkEqualFreq(freq1, freq2):
	assert(len(freq1)==len(freq2))
	for i in range(len(freq1)):
		df = abs(freq1[i]-freq2[i])/freq1[i]
		assert(df<1e-4)
	pass

class Calibration:
	'''
	Пока сделаем простейшую калибровку.
	Open-Short
	'''
	def __init__(self):
		self.Zstd = 49.9
		(self.freq, self.U_open) = readXmlZ('calibration/rx_open.xml')

		(freq, self.U_short) = readXmlZ('calibration/rx_short.xml')
		checkEqualFreq(self.freq, freq)

		(freq, self.U_50Om) = readXmlZ('calibration/rx_49_9Om.xml')
		checkEqualFreq(self.freq, freq)

		self.U_max = calcUxZ(0)
		self.U_min = calcUxZ(1e6)
		#print(self.U_short[0], self.U_open[0])

		(freq, self.U_tx_trans) = readXmlZ('calibration/tx_transmission.xml')
		checkEqualFreq(self.freq, freq)
		(freq, self.U_tx_open) = readXmlZ('calibration/tx_open.xml')
		checkEqualFreq(self.freq, freq)

		self.checkSortFreq()
		pass

	def checkSortFreq(self):

		for i in range(1, len(self.freq)):
			assert(self.freq[i-1]<self.freq[i])
		pass

	def calculateUI(self, U):
		'''
		Расчитывает напряжение на RX входе без корректировки.
		U - измеренное напряжение на резистре R15.
		return (U1,I1) - напряжение на RX входе.
		'''
		U = (U-self.U_open[0])/(self.U_short[0]-self.U_open[0])*(self.U_max-self.U_min)+self.U_min
		#print("U=",U)
		return calcRxUI(U)

	def calculateZ(self, U):
		'''
		Без корректировки.
		'''
		(U1, I1) = self.calculateUI(U)
		Z = U1/I1
		return Z

	def calculateY(self, U):
		'''
		Без корректировки.
		'''
		(U1, I1) = self.calculateUI(U)
		Y = I1/U1
		return Y

	def correct(self, U, freq):
		Z = self.calculateZ(U)

		#Z = complex(min(max(Z.real, -Rmax), Rmax), min(max(Z.imag, -Rmax), Rmax))
		Z = self.clamp(Z)

		Z = self.OSLCalibration(Z, freq)

		#print("Z=",Z)
		return Z

	def clamp(self, Z):
		(r, phi) = cmath.polar(Z)
		if r>Rmax:
			return cmath.rect(Rmax, 0) 
		#r = min(max(r, -Rmax), Rmax)
		return cmath.rect(r, phi)

	def OSLCalibration(self, Zxm, freq):
		'''
		Open-Short-Load калибровка.
		На входе Z измеренное не очень точно.
		На выходе Z откорректированное.
		Блин, не подумали. Напряжение неоткалиброванно, поэтому неизвестно (только в попугаях) :(
		Zx = Zstd*(1-Zstdm/Zom)*(Zxm-Zsm)/((Zstdm-Zsm)*(1-Zxm/Zom))
		Т.к. Zom очень большое может быть, заменим его на Yom = 1/Zom
		Zx = Zstd*(1-Zstdm*Yom)*(Zxm-Zsm)/((Zstdm-Zsm)*(1-Zxm*Yom))
		'''
		(U_open, U_short, U_load) = self.FindAndInterpolate(freq)

		Yom = self.calculateY(U_open)
		Zsm = self.calculateZ(U_short)
		Zstdm = self.calculateZ(U_load)
		Zstd = self.Zstd

		Zx = Zstd*(1-Zstdm*Yom)*(Zxm-Zsm)/((Zstdm-Zsm)*(1-Zxm*Yom))
		return Zx

	def ZxYoCalibration(self, freq):
		'''
		Пускай щупы будут представленны как 
		Xs - последовательное сопротивление щупов.
		Yo - параллельная проводимость щупов (в конце).

		Zx = Zstd*(1-Zstdm*Yom)*(Zxm-Zsm)/((Zstdm-Zsm)*(1-Zxm*Yom))

		Подставим Zxm <- Zsm , Zsm = 0
		Zs = Zstd*(1-Zstdm*Yom)*(Zsm)/((Zstdm)*(1-Zsm*Yom))
		Получаем довольно точно Zs

		Подставим Zxm <- 1/Yom , Yom = 0
		Zo = Zstd/Yom/(Zstdm-Zsm)

		Пускай 
		 K = Zstd/(Zstdm-Zsm) - коэффициент передачи щупов?
		'''
		(U_open, U_short, U_load) = self.FindAndInterpolate(freq)

		Yom = self.calculateY(U_open)
		Zsm = self.calculateZ(U_short)
		Zstdm = self.calculateZ(U_load)
		Zstd = self.Zstd

		K = Zstd/(Zstdm) 
		Zs = (1-Zstdm*Yom)*(Zsm)/(1-Zsm*Yom)
		Yo = Yom
		return (K, Zs, Yo)

	def FindAndInterpolate(self, freq):
		'''
		Предполагем, что массив freq отсотирован.
		Ищем калибровочные напряжения и интерполируем их.
		'''
		idx = bisect.bisect_left(self.freq, freq)
		assert(idx>=0 and idx<len(self.freq))

		#пока интерполяции нет, возвращаем ближайшее нижнее.
		return (self.U_open[idx], self.U_short[idx], self.U_50Om[idx])

	def txFindAndInterpolate(self, freq):
		'''
		Предполагем, что массив freq отсотирован.
		Ищем калибровочные напряжения и интерполируем их.
		'''
		idx = bisect.bisect_left(self.freq, freq)
		assert(idx>=0 and idx<len(self.freq))

		#пока интерполяции нет, возвращаем ближайшее нижнее.
		return (self.U_tx_open[idx], self.U_tx_trans[idx])
		
	def txCalculateUI(self, U):
		return (U-self.U_tx_open[0])/(self.U_tx_trans[0]-self.U_tx_open[0])
		
	def txCalculateUIf(self, U, freq):
		(U_tx_open, U_tx_trans) = self.txFindAndInterpolate(freq)
		#return (U-U_tx_open)/(U_tx_trans-U_tx_open)
		return U/U_tx_trans

def main():
	cal = Calibration()
	#data_x = readXmlData('calibration/rx_short.xml')
	data_x = readXmlData('calibration/rx_10Om.xml')
	#data_x = readXmlData('calibration/rx_100Om.xml')
	(freq, Ux) = getZFromData(data_x)
	for i in range(len(freq)):
	#for i in range(10):
		Z = cal.correct(Ux[i], freq[i])
		print("F=", freq[i], "Z=", Z)

	pass

if __name__ == "__main__":
	main()
