import matplotlib.pyplot as plt
import math
import smath
from calibration import *

def fixFi(fi):
	if fi > math.pi:
		fi -= 2*math.pi
	if fi < -math.pi:
		fi += 2*math.pi
	return fi


def plotRaw(xdata, ydata):
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(xdata)):
		xdata[i] *= 1e-6
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('Y')
	ax.plot(xdata, ydata)
	plt.show()
	pass

def plotRaw2(xdata, ydata1, ydata2):
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(xdata)):
		xdata[i] *= 1e-6
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	#ax.set_ylabel('dB')
	ax.plot(xdata, ydata1, color='red')
	ax.plot(xdata, ydata2, color='blue')
	plt.show()
	pass

def to_dB(x):
	return 20*math.log(x)/math.log(10)
	
def splitComplexArray(Z_arr):
	re = [z.real for z in Z_arr]
	im = [z.imag for z in Z_arr]
	return (re, im)

def plotZ(xdata, ydata, title = None):
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(xdata)):
		xdata[i] *= 1e-6
	yreal = [y.real for y in ydata]
	yimag = [y.imag for y in ydata]

	if title:
		ax.set_title(title)

	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('R (Om)')
	ax.plot(xdata, yreal, color='red')
	ax.plot(xdata, yimag, color='blue')
	plt.show()
	pass
	
def plotZxy(zdata, zdata1 = None):

	fig = plt.figure()
	ax = fig.add_subplot(111, aspect='equal')
	ax.set_xlabel('Real')
	ax.set_ylabel('Imag')
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	(xdata, ydata) = splitComplexArray(zdata)
	ax.plot(xdata, ydata, color='#FF8000')
	
	if zdata1:
		(xdata1, ydata1) = splitComplexArray(zdata1)
		ax.plot(xdata1, ydata1, color='green')
	
	plt.show()
	pass
	

def getFreqFromData(data):
	freq = []
	for d in data:
		freq.append(d['F'])
	return freq

def getOutFromData(data):
	amplitude = []
	fi = []
	for d in data:
		(aI,fI) = smath.calcFi(d['i_csin'], d['i_ccos'])
		(aQ,fQ) = smath.calcFi(d['q_csin'], d['q_ccos'])
		amplitude.append(aI/aQ)
		fi.append(fixFi(fI-fQ))
	return (amplitude, fi)


def stdGraph():
	#data = readXmlData('hard_tx.xml')
	#data = readXmlData('calibration/rx_short.xml')
	#data = readXmlData('calibration/rx_open.xml')
	#data = readXmlData('calibration/rx_49_9Om.xml')
	#data = readXmlData('calibration/rx_49_9Om.xml')
	data = readXmlData('calibration/tx_transmission.xml')
	
	(arr_freq, arr_z) = getZFromData(data)
	arr_amplitude = ZtoAmplithude(arr_z)
	arr_phase = ZtoPhase(arr_z)
	plotRaw(arr_freq, arr_amplitude)
	#plotRaw(arr_freq, arr_phase)
	pass

def twoGraph():
	(arr_freq, arr_z1) = readXmlZ('calibration/rx_470pF.xml')
	(arr_freq, arr_z2) = readXmlZ('calibration/tx_open_disconnect.xml')
	
	arr_data1 = [0]*len(arr_freq)
	arr_data2 = [0]*len(arr_freq)
	offset = arr_z2[0]
	for i in range(len(arr_freq)):
		m = 1/0.42
		z1 = (arr_z1[i]-offset)*m
		z2 = (arr_z2[i]-offset)*m
		#arr_data1[i] = abs(z1)
		#arr_data2[i] = abs(z2)
		arr_data1[i] = z1
		arr_data2[i] = z2

	#for i in range(len(arr_amplitude2)):
	#	m = 1/0.42
	#	arr_amplitude1[i] = to_dB(arr_amplitude1[i]*m)
	#	arr_amplitude2[i] = to_dB(arr_amplitude2[i]*m)
	#plotRaw(arr_freq, arr_amplitude1)
	#plotRaw2(arr_freq, arr_data1, arr_data2)
	
	(xdata1, ydata1) = splitComplexArray(arr_data1)
	(xdata2, ydata2) = splitComplexArray(arr_data2)

	fig = plt.figure()
	ax = fig.add_subplot(111, aspect='equal')
	ax.set_xlabel('Real')
	ax.set_ylabel('Imag')
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.plot(xdata1, ydata1, color='red')
	ax.plot(xdata2, ydata2, color='blue')
	plt.show()
	
	plotZxy(arr_data1)
	pass

def twoGraph100grad():
	data1 = readXmlData('xml/10_7_out300om_tx.xml')
	data2 = readXmlData('xml/10_7_out300om_100grad_tx.xml')
	arr_freq = getFreqFromData(data1)
	(arr_amplitude1, arr_phase1) = getOutFromData(data1)
	(arr_amplitude2, arr_phase2) = getOutFromData(data2)

	delta_phase = []
	for i in range(len(arr_amplitude2)):
		df = (arr_phase1[i]-arr_phase2[i])
		if df > math.pi:
			df -= 2*math.pi
		delta_phase.append(df*180./math.pi)

	plotRaw2(arr_freq, arr_amplitude1, arr_amplitude2)
	#plotRaw(arr_freq, delta_phase)
	pass

def ZGraph():
	cal = Calibration()
	Ux1 = None
	#(freq, Ux) = readXmlZ('calibration/rx_open.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_short.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_10Om.xml')
	#(freq, Ux1) = readXmlZ('calibration/rx_49_9Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_100Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_470pF.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_transmission.xml')
	(freq, Ux) = readXmlZ('calibration/rx_ser_10_Om.xml')
	Zarr = []
	Uarr = []
	
	if Ux1:
		Uarr1 = []
	else:
		Uarr1 = None
	for i in range(len(freq)):
		Z = cal.correct(Ux[i], freq[i])
		Zarr.append(Z)
		(U,I) = cal.calculateUI(Ux[i])
		Uarr.append(U)
		if Ux1:
			Uarr1.append(cal.calculateUI(Ux1[i])[0])
			
		
	#for i in range(len(freq)):
	#	(Zs, Yo) = cal.ZxYoCalibration(freq[i])
	#	Zarr.append(Yo)
	#plotZ(freq, Ux)
	plotZ(freq, Zarr, "10 Om")
	#print(Zarr)
	#plotZxy(Uarr, Uarr1)
	pass

def ZGraph4():
	cal = Calibration()
	(x, Urx_t) = readXmlZ('calibration/rx_transmission.xml')
	(x, Utx_t) = readXmlZ('calibration/tx_transmission.xml')
	(Urx_0,I) = cal.calculateUI(Utx_t[0])
	K = cal.txCalculateUI(Utx_t[0]) / cal.calculateUI(Urx_t[0])[0]
	print(cal.txCalculateUI(Utx_t[0]),  cal.calculateUI(Urx_t[0])[0])
	print("K=", K)

	#(freq, Urx) = readXmlZ('calibration/rx_transmission.xml')
	#(freq, Utx) = readXmlZ('calibration/tx_transmission.xml')
	#(freq, Urx) = readXmlZ('calibration/rx_ser_49_9Om.xml')
	#(freq, Utx) = readXmlZ('calibration/tx_ser_49_9Om.xml')
	#(freq, Urx) = readXmlZ('calibration/rx_ser_10_Om.xml')
	#(freq, Utx) = readXmlZ('calibration/tx_ser_10_Om.xml')
	(freq, Urx) = readXmlZ('calibration/rx_ser_100_Om.xml')
	(freq, Utx) = readXmlZ('calibration/tx_ser_100_Om.xml')
	Urx_arr = []
	Utx_arr = []
	for i in range(len(freq)):
		K = cal.txCalculateUI(Utx_t[i]) / cal.calculateUI(Urx_t[i])[0]
		(U,I) = cal.calculateUI(Urx[i])
		U *= K
		Ut = cal.txCalculateUI(Utx[i])
		Rm = 50.*(U/Ut-1)
		#Urx_arr.append(U)
		#Utx_arr.append(Ut)
		Urx_arr.append(Rm)
		Utx_arr.append(Rm)

	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(freq)):
		freq[i] *= 1e-6
	#y1 = [cmath.phase(y) for y in Urx_arr]
	#y2 = [cmath.phase(y) for y in Utx_arr]
	y1 = [abs(y) for y in Urx_arr]
	y2 = [abs(y) for y in Utx_arr]

	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('R (Om)')
	ax.plot(freq, y1, color='red')
	ax.plot(freq, y2, color='blue')
	plt.show()


def UGraph():
	(freq, Ux) = readXmlZ('calibration/tx_transmission.xml')
	plotZ(freq, Ux)

def ZGraphTx():
	cal = Calibration()
	#(freq, Utx) = readXmlZ('calibration/tx_ser_10_Om.xml')
	#(freq, Urx) = readXmlZ('calibration/rx_ser_10_Om.xml')
	
	(freq, Utx) = readXmlZ('calibration/tx_ser_49_9Om.xml')
	(freq, Urx) = readXmlZ('calibration/rx_ser_49_9Om.xml')
	
	#(freq, Utx) = readXmlZ('calibration/tx_ser_100_Om.xml')
	#(freq, Urx) = readXmlZ('calibration/rx_ser_100_Om.xml')
	
	#(freq, Utx) = readXmlZ('calibration/tx_ser_470_pF.xml')
	
	#(freq, Utx) = readXmlZ('calibration/tx_transmission.xml')
	#(freq, Urx) = readXmlZ('calibration/rx_transmission.xml')

	Zarr = []
	for i in range(len(freq)):
	#for i in range(5):
		U = cal.txCalculateUI(Utx[i])
		Uf = cal.txCalculateUIf(Utx[i], freq[i])
		Ur = cal.calculateUI(Urx[i])[0]
		Rm = 50*(Ur-U)/U
		#Rm = 50*(Ur-Uf)/Uf
		#print(Ur, Uf, "Rm="+str(Rm))
		#Zarr.append(U)
		#Zarr.append(Ur)
		Zarr.append(Rm)
		
	#plotZ(freq, Zarr, "10 Om")
	#Rarr = [ abs(z) for z in Zarr]
	Rarr = [ cmath.phase(z)*180/math.pi for z in Zarr]
	plotRaw(freq, Rarr)
	pass

def graphG():
	cal = CalibrationSOLT()
	Ux1 = None
	#(freq, Ux) = readXmlZ('calibration/rx_open.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_short.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_10Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_49_9Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_100Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_470pF.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_transmission.xml')
	(freq, Ux) = readXmlZ('calibration/rx_ser_10_Om.xml')
	Garr = []
	Zarr = []

	for i in range(len(freq)):
		G = cal.calculateG(Ux[i], freq[i])
		Z = cal.GtoZ(G)
		Garr.append(G)
		Zarr.append(Z)
			
		
	#plotZ(freq, Garr, "10 Om")
	plotZ(freq, Zarr, "Z 10 Om")
	pass


#stdGraph()
#twoGraph()
#twoGraph100grad()

#ZGraph()
#ZGraphTx()
#UGraph()
#ZGraph4()

graphG()

