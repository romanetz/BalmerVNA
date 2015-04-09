import array
import math
import json

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import smath as sm

def readJson(filename):
	with open(filename, "rt") as file:
		data = json.load(file)
	return data

def makeTimeList(readableData, xmin, xstep):
	xlist = []
	for i in range(0, len(readableData)):
		xlist.append(xmin+i*xstep)
	return xlist

def plotRaw(data):
	fig, ax = plt.subplots()
	ax.set_xlabel("Time")
	#ax.set_xscale('log')
	ax.set_ylabel('Y')

	timeList = makeTimeList(data, 0, sm.STEP)

	ax.plot (timeList, data)

	plt.show()
	pass

def plotOut():
	data = readJson("out.json")
	fig, ax = plt.subplots()
	#ax.set_xlabel("Time ")
	#ax.set_xscale('log')
	#ax.set_ylabel('Y')

	I = data['I']
	Q = data['Q']
	timeList = makeTimeList(I, 0, sm.STEP)

	#freq = 1000
	#I = 1e6*np.sin(2*math.pi*freq*np.array(timeList))

	ax.plot (timeList, I, color='red')
	ax.plot (timeList, Q, color='blue')

	plt.show()

def plotFreq():
	data = readJson("out.json")
	fig, ax = plt.subplots()

	Q = data['Q']

	freqDelta = 10
	freqCenter = 1000
	count = 201
	(freqArr, Fmath) = sm.arrayFreq(Q, freqCenter-freqDelta, freqCenter+freqDelta, sm.STEP, count)
	fmax = sm.findFreqMax(freqArr, Fmath)
	print("fmax=", fmax)

	ax.plot (freqArr, Fmath, color='blue')

	plt.show()

def addToFi(fi, add):
	fi += add
	if fi > math.pi:
		fi -= 2*math.pi
	if fi < -math.pi:
		fi += 2*math.pi
	return fi

def addToFiArr(fi, add):
	for i in range(len(fi)):
		fi[i] = addToFi(fi[i], add)

def plotIQ():
	jout = readJson("freq.json")
	fig, ax = plt.subplots()
	freq = jout['freq']
	I = jout['I']
	Q = jout['Q']
	fsin = jout['fsin']
	fi = jout['fi']

	addToFiArr(fi, math.pi)
	#ax.plot (freq, I, color='red')
	#ax.plot (freq, Q, color='blue')
	#ax.set_ylim(999.0, 1000.0)
	#ax.plot (freq, fsin)
	ax.set_ylim(-math.pi, +math.pi)
	ax.plot (freq, fi)
	plt.show()
	pass

def plotTestPhase():
	fig, ax = plt.subplots()

	N = 256
	freq = 1111
	I = [0]*N
	timeList = makeTimeList(I, 0, sm.STEP)
	phase = 1
	amp = 1.2
	I = amp*np.sin(2*math.pi*freq*np.array(timeList)+phase)

	(c0,csin, ccos) = sm.calcSinCosMatrix(I, freq, sm.STEP)
	(amplitude, fi) = sm.calcFi(csin, ccos)
	print("c0=", c0)
	print("phase=", phase, "delta=", fi-phase)
	print("amplitude=", amplitude)

	Q = csin*np.sin(2*math.pi*freq*np.array(timeList))+ccos*np.cos(2*math.pi*freq*np.array(timeList))+c0

	#freq = 1000

	ax.plot(timeList, I, color='red')
	ax.plot(timeList, Q, color='blue')

	plt.show()

def plotFi2():
	fig, ax = plt.subplots()
	ax.set_xlabel("MHz")
	ax.set_ylabel('Phase')

	jout1 = readJson("freq_short.json")
	freq1 = jout1['freq']
	fi1 = jout1['fi']
	addToFiArr(fi1, math.pi)

	jout2 = readJson("freq_long.json")
	freq2 = jout2['freq']
	fi2 = jout2['fi']
	addToFiArr(fi2, math.pi)

	for i in range(len(freq1)):
		freq1[i] *= 1e-6
	for i in range(len(freq2)):
		freq2[i] *= 1e-6

	#ax.set_ylim(-math.pi, +math.pi)
	ax.plot(freq1, fi1, color='red')
	ax.plot(freq2, fi2, color='blue')
	plt.show()
	pass

def main():
	#plotOut()
	#plotIQ()
	#plotFreq()
	#plotTestPhase()
	plotFi2()
	pass

if __name__ == "__main__":
	main()
