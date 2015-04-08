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

	freqDelta = 1
	freqCenter = 1000
	count = 201
	(freqArr, Fmath) = sm.arrayFreq(Q, freqCenter-freqDelta, freqCenter+freqDelta, sm.STEP, count)
	fmax = sm.findFreqMax(freqArr, Fmath)
	print("fmax=", fmax)

	ax.plot (freqArr, Fmath, color='blue')

	plt.show()

def plotIQ():
	jout = readJson("freq.json")
	fig, ax = plt.subplots()
	freq = jout['freq']
	I = jout['I']
	Q = jout['Q']
	fsin = jout['fsin']
	#ax.plot (freq, I, color='red')
	#ax.plot (freq, Q, color='blue')
	ax.set_ylim(999.0, 1000.0)
	ax.plot (freq, fsin)
	plt.show()
	pass


def main():
	#plotOut()
	plotIQ()
	#plotFreq()
	pass

main()