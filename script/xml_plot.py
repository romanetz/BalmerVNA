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
	ax.set_xscale('log')
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
	ax.set_ylabel('Y')
	ax.plot(xdata, ydata1, color='red')
	ax.plot(xdata, ydata2, color='blue')
	plt.show()
	pass

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
	data = readXmlData('calibration/rx_49_9Om.xml')
	(arr_freq, arr_z) = getZFromData(data)
	arr_amplitude = ZtoAmplithude(arr_z)
	arr_phase = ZtoPhase(arr_z)
	plotRaw(arr_freq, arr_amplitude)
	#plotRaw(arr_freq, arr_phase)
	pass

def twoGraph():
	data1 = readXmlData('xml/10_7_no_res_tx.xml')
	data2 = readXmlData('xml/10_7_out300om_tx.xml')
	arr_freq = getFreqFromData(data1)
	(arr_amplitude1, arr_phase1) = getOutFromData(data1)
	(arr_amplitude2, arr_phase2) = getOutFromData(data2)

	for i in range(len(arr_amplitude2)):
		arr_amplitude2[i] *= 4.5

	plotRaw2(arr_freq, arr_amplitude1, arr_amplitude2)
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
	#(freq, Ux) = readXmlZ('calibration/rx_open.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_short.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_10Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_49_9Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_100Om.xml')
	(freq, Ux) = readXmlZ('calibration/rx_470pF.xml')
	#(freq, Ux) = readXmlZ('hard_rx.xml')
	Zarr = []
	#for i in range(len(freq)):
	#	Z = cal.correct(Ux[i], freq[i])
	#	Zarr.append(Z)
	for i in range(len(freq)):
		(Zs, Yo) = cal.ZxYoCalibration(freq[i])
		Zarr.append(Yo)
	#plotZ(freq, Ux)
	plotZ(freq, Zarr, "10 Om")
	pass

def ZGraph4():
	cal = Calibration()
	(freq, Ux) = readXmlZ('calibration/rx_open.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_short.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_10Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_49_9Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_100Om.xml')
	#(freq, Ux) = readXmlZ('calibration/rx_470pF.xml')
	#(freq, Ux) = readXmlZ('hard_rx.xml')
	Zarr = []
	Zdarr = []
	for i in range(len(freq)):
		Z = cal.correct(Ux[i], freq[i])
		Zm = cal.calculateZ(Ux[i])
		(U,I) = cal.calculateUI(Ux[i])
		Zarr.append(U)
		(K, Zs, Yo) = cal.ZxYoCalibration(freq[i])
		Zdut = K*(Zm-Zs)/(1-(Zm-Zs)*Yo)
		Zdarr.append(K)

	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(freq)):
		freq[i] *= 1e-6
	yreal = [abs(y) for y in Zarr]
	yimag = [cmath.phase(y) for y in Zarr]

	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('R (Om)')
	ax.plot(freq, yreal, color='red')
	ax.plot(freq, yimag, color='blue')
	plt.show()


def UGraph():
	(freq, Ux) = readXmlZ('calibration/tx_transmission.xml')
	plotZ(freq, Ux)

#stdGraph()
#twoGraph()
#twoGraph100grad()

#ZGraph()
#UGraph()
ZGraph4()

