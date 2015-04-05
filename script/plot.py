import array
import math
import json

import matplotlib
import matplotlib.pyplot as plt
import numpy as np

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

	timeList = makeTimeList(data, 0, 1.0/44800)

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

	timeList = makeTimeList(I, 0, 1.0/44800)

	ax.plot (timeList, I, color='red')
	ax.plot (timeList, Q, color='blue')

	plt.show()

def plotIQ():
	jout = readJson("freq.json")
	fig, ax = plt.subplots()
	freq = jout['freq']
	I = jout['I']
	Q = jout['Q']
	ax.plot (freq, I, color='red')
	ax.plot (freq, Q, color='blue')
	plt.show()
	pass


def main():
	plotOut()
	#plotIQ()
	pass

main()