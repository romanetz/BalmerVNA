import cmath
import math
import bisect
import xml.etree.ElementTree as ET

from graph_u2 import calcUxZ, calcRxUI

Rmax = 1000 # 500 Om - этом максимальные цифры которые отображаем

class Params:
	pass

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
		(freq, self.U_rx_trans) = readXmlZ('calibration/rx_transmission.xml')
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
		return (self.U_tx_open[idx], self.U_tx_trans[idx], self.U_rx_trans[idx])
		
	def txCalculateUI(self, U):
		#return (U-self.U_tx_open[0])/(self.U_tx_trans[0]-self.U_tx_open[0])
		(Urx, Irx) = self.calculateUI(self.U_rx_trans[0])
		return (U-self.U_tx_open[0])/(self.U_tx_trans[0]-self.U_tx_open[0])*Urx
		
		
	def txCalculateUIf(self, U, freq):
		(U_tx_open, U_tx_trans, U_rx_trans) = self.txFindAndInterpolate(freq)
		(Urx,Irx) = self.calculateUI(U_rx_trans)
		#return (U-U_tx_open)/(U_tx_trans-U_tx_open)
		return U/U_tx_trans*Urx

class CalibrationSOLT:
	'''
		Калибровка в терминах S параметров
		Обозначения взят из Rytting_NAModels.pdf
		G - измеренная отраженная волна.
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
		(freq, self.U_rx_trans) = readXmlZ('calibration/rx_transmission.xml')
		checkEqualFreq(self.freq, freq)

		self.checkSortFreq()
		pass

	def checkSortFreq(self):
		for i in range(1, len(self.freq)):
			assert(self.freq[i-1]<self.freq[i])
		pass

	def FindAndInterpolateRx(self, freq):
		'''
		Предполагем, что массив freq отсотирован.
		Ищем калибровочные напряжения и интерполируем их.
		'''
		idx = bisect.bisect_left(self.freq, freq)
		assert(idx>=0 and idx<len(self.freq))

		#пока интерполяции нет, возвращаем ближайшее нижнее.
		return (self.U_open[idx], self.U_short[idx], self.U_50Om[idx])

	def FindAndInterpolateTx(self, freq):
		'''
		Предполагем, что массив freq отсотирован.
		Ищем калибровочные напряжения и интерполируем их.
		'''
		idx = bisect.bisect_left(self.freq, freq)
		assert(idx>=0 and idx<len(self.freq))

		#пока интерполяции нет, возвращаем ближайшее нижнее.
		return (self.U_tx_trans[idx], self.U_tx_open[idx], self.U_rx_trans[idx])

	def calcErx(self, freq):
		'''
		Вычисляем выражение.
		Gm - измеренное отраженное.
		G - реальное отраженное.
		Связь такая:
		Gm = (e00-De*G)/(1-e11*G)
		De = e00*e11-(e10*e01)
		(e10*e01) - это один параметр, т.к. они всегда умноженной парой встречаются.
		Вычисляем для трех независимых уравнений.
		e00 + G*Gm*e11 - G*De = Gm

		Open G=1
			e00 + Gm_open*e11 - De = Gm_open
		Short G=-1
			e00 - Gm_short*e11 + De = Gm_sort
		Load Z=50 G=0
			e00 = Gm_load
		------------
			e00 + Gm_open*e11 - De = Gm_open
			e00 - Gm_short*e11 + De = Gm_sort
			Складываем
			2*e00 + (Gm_open-Gm_short)*e11 = Gm_open+Gm_sort

			e11 = (Gm_open+Gm_sort - 2*e00)/(Gm_open-Gm_short)

			De = Gm_sort-e00+Gm_short*e11
		'''

		(Gm_open, Gm_short, Gm_load) = self.FindAndInterpolateRx(freq)

		e00 = Gm_load
		e11 = (Gm_open+Gm_short-2*e00)/(Gm_open-Gm_short)
		De = Gm_short-e00+Gm_short*e11
		e10_01 = e00*e11 - De

		params = Params()
		params.e00 = e00
		params.e11 = e11
		params.De = De
		params.e10_01 = e10_01
		return params

	def calcEtx(self, freq):
		'''
			S21m_trans - напряжение когда вход и выход замкнуты через щупы (все напряжение от входа поступает в выход)
			S21m_open - напряжение, когда на щупы ничего не подавется (нужно вход и выход замкнуть 50 ом заглушками)
			Gm_tans - напряжение на соответствующее U_tx_trans
 		'''
		p = self.calcErx(freq)
		(S21m_trans, S21m_open, Gm_tans) = self.FindAndInterpolateTx(freq)

		G_trans = (Gm_tans - p.e00)/(Gm_tans*p.e11-p.De)
		p.e30 = S21m_open
		p.e22 = G_trans
		p.e10_32 = (S21m_trans-p.e30)*(1-p.e11*p.e22)

		#print("e30=", p.e30, "e22=", p.e22, "e10_32=",p.e10_32)
		#print("e11=", abs(p.e11), "e22=", abs(p.e22), "e10_32=", abs(p.e10_32))
		return p

	def calculateG(self, Gm, freq):
		'''
		Вычисляем коэффициэнт отражения для rx порта 
		учитывая щупы и коэффициэнт усиления усилителя.
		'''
		p = self.calcErx(freq)
		G = (Gm - p.e00)/(Gm*p.e11-p.De)
		return G

	def GtoZ(self, G):
		'''
		G = (Z-Z0)/(Z+Z0)
		G*Z+G*Z0 = Z-Z0
		G*Z-Z = -G*Z0-Z0
		(G-1)*Z = -(G+1)*Z0
		Z = Z0 * (1+G)/(1-G)
		'''
		if abs(1-G)<1e-5:
			G = 0

		return self.Zstd * (1+G)/(1-G)

	def iterateS11_S21(self, p, S11m, S21m):
		S11 = 1
		S21 = 0
		e00 = p.e00
		e11 = p.e11
		e22 = p.e22
		e30 = p.e30
		e10_32 = p.e10_32
		e10_01 = p.e10_01


		for i in range(5):
			S11 = ((S11m-e00)*(1-e11*e22*S21*S21)-(e10_01)*(e22*S21*S21))/((e10_01)*(1-e22*S11)+(S11m-e00)*(e11+e22+e11*e22*S11))
			S21 = (S21m-e30)*(1-(e11+e22+e11*e22*S11)*S11)/((e10_32) + (S21m-e30)*e11*e22*S21)
			print("S11=", S11, "S21=", S21)

		return (S11, S21)


	def calculateS11_S21(self, Urx, Utx, freq):
		'''
		Исходные уравнения.
		Ds = S11*S22 - S21*S12
		S11m = e00 + (e10*e01)*(S11 - e22*Ds)/(1-e11*S11-e22*S22+e11*e22*Ds)
		S21m = e30 + (e10*e32)*S21/(1-e11*S11-e22*S22+e11*e22*Ds)

		предположим, что у нас reciprocal четырехполюсник S12 = S21
		предположим, что у нас симметричный четырехполюсник S11 = S22
		Ds = S11*S11 - S21*S21
		S11m = e00 + (e10*e01)*(S11 - e22*Ds)/(1-e11*S11-e22*S11+e11*e22*Ds)
		S21m = e30 + (e10*e32)*S21/(1-e11*S11-e22*S11+e11*e22*Ds)

		Попробуем решить "методом простых итераций"
		DV = (1-e11*S11-e22*S11+e11*e22*Ds)
		DV = (1-(e11+e22)*S11+e11*e22*Ds)
		DV = (1-(e11+e22)*S11+e11*e22*(S11*S11-S21*S21))
		DV = (1-(e11+e22+e11*e22*S11)*S11-e11*e22*S21*S21)

		(S11m-e00) =  (e10*e01)*(S11 - e22*Ds)/DV
		(S21m-e30) =  (e10*e32)*S21/DV

		(S11m-e00)*DV =  (e10*e01)*(S11 - e22*Ds)
		(S21m-e30)*DV =  (e10*e32)*S21

		(S11m-e00)*DV =  (e10*e01)*(S11 - e22*Ds)
		(S21m-e30)*DV =  (e10*e32)*S21

		(S11m-e00)*(1-(e11+e22+e11*e22*S11)*S11-e11*e22*S21*S21) =  (e10*e01)*(S11 - e22*S11*S11 + e22S21*S21)
		(S21m-e30)*(1-(e11+e22+e11*e22*S11)*S11-e11*e22*S21*S21) =  (e10*e32)*S21

		(S11m-e00)*(1-e11*e22*S21*S21) =  (e10*e01)*((1-e22*S11)*S11 + e22*S21*S21)+(S11m-e00)*(e11+e22+e11*e22*S11)*S11
		(S21m-e30)*(1-(e11+e22+e11*e22*S11)*S11) =  (e10*e32)*S21 + (S21m-e30)*(e11*e22*S21*S21)

		(S11m-e00)*(1-e11*e22*S21*S21)-(e10*e01)*(e22*S21*S21) =  (e10*e01)*((1-e22*S11)*S11)+(S11m-e00)*(e11+e22+e11*e22*S11)*S11
		(S21m-e30)*(1-(e11+e22+e11*e22*S11)*S11) =  ((e10*e32) + (S21m-e30)*e11*e22*S21)*S21

		(S11m-e00)*(1-e11*e22*S21*S21)-(e10*e01)*(e22*S21*S21) =  ((e10*e01)*(1-e22*S11)+(S11m-e00)*(e11+e22+e11*e22*S11))*S11

		(S11m-e00)*(1-e11*e22*S21*S21)-(e10*e01)*(e22*S21*S21) =  ((e10*e01)*(1-e22*S11)+(S11m-e00)*(e11+e22+e11*e22*S11))*S11		
		(S21m-e30)*(1-(e11+e22+e11*e22*S11)*S11)/((e10*e32) + (S21m-e30)*e11*e22*S21) = S21
		'''

		p = self.calcEtx(freq)
		return self.iterateS11_S21(p, Urx, Utx)


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
