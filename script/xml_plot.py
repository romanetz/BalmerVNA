import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
import math
import smath

def fixFi(fi):
	if fi > math.pi:
		fi -= 2*math.pi
	if fi < -math.pi:
		fi += 2*math.pi
	return fi


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
	ax.set_ylabel('Y')
	ax.plot(xdata, ydata1, color='red')
	ax.plot(xdata, ydata2, color='blue')
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
	data = readXmlData('hard_tx.xml')
	arr_freq = getFreqFromData(data)
	(arr_amplitude, arr_phase) = getOutFromData(data)
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

#stdGraph()
twoGraph()
#twoGraph100grad()