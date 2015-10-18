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
	ax.set_xlabel('Frequency (Hz)')
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('Y')
	ax.plot(xdata, ydata)
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


data = readXmlData('hard.xml')

arr_freq = getFreqFromData(data)
(arr_amplitude, arr_phase) = getOutFromData(data)

#plotRaw(arr_freq, arr_amplitude)
plotRaw(arr_freq, arr_phase)
