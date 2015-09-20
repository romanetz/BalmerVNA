import array
import math
import cmath
import json

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import smath as sm
import z_math as zm

mul = 77./327107250.


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

def andData(data):
	sub = 1
	if sub==2:
		for i in range(len(data)):
			d = data[i]&0xFFFF
			if d>0x7FFF:
				d = d-0xFFFF-1
			data[i] = d
	if sub==1:
		for i in range(len(data)):
			d = data[i]&0xFFFFFF
			if d>0x7FFFFF:
				d = d-0xFFFFFF-1
			data[i] = d


def plotOut():
	data = readJson("out.json")
	fig, ax = plt.subplots()
	#ax.set_xlabel("Time ")
	#ax.set_xscale('log')
	#ax.set_ylabel('Y')

	I = data['I']
	Q = data['Q']
	timeList = makeTimeList(I, 0, sm.STEP)

	L =[]
	for i in range(10):
		L.append(hex(I[i]))
	print(L)
	#andData(I),andData(Q)
	for i in range(len(I)):
		I[i] *= mul
		Q[i] *= mul
		#Q[i] *= 3e-4

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

def plotIQ(filename="freq.json"):
	#filename = "json/f0_100uH.json"
	#filename = "json/f0_7_5nF.json"
	#filename = "json/f0_short.json"
	#filename = "json/f0_1Om.json"
	#filename = "json/f0_50Om.json"
	#filename = "json/f0_100Om.json"
	#filename = "json/f0_open.json"
	jout = readJson(filename)
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (Hz)')
	freq = jout['freq']
	I = jout['I']
	Q = jout['Q']
	A = []
	#KU = 0.5 #примерно на глаз прикинули
	KU = 0.5148
	for i in range(len(Q)):
		A.append(I[i]/Q[i] * KU)
		I[i] *= 1e-8
		Q[i] *= 1e-8
		Q[i] *= 0.12
	fsin = jout['fsin']
	fi = jout['fi']

	addToFiArr(fi, math.pi)

	if True:
		ax.plot (freq, fi, color='blue')
	elif False:
		ax.plot (freq, I, color='red')
		ax.plot (freq, Q, color='blue')
		ax.set_ylim(0, plt.ylim()[1])
		ax.set_xscale('log')
		#ax.plot (freq, fsin)
	else:
		ax.plot (freq, fi, color='blue')
		ax.set_ylabel('Phase')
		#ax.set_ylim(-math.pi, +math.pi)
		for tl in ax.get_yticklabels():
		    tl.set_color('b')

		ax2 = ax.twinx()
		ax2.set_ylabel('Amplithude')
		ax2.plot (freq, A, color='red')
		for tl in ax2.get_yticklabels():
		    tl.set_color('r')
		#ax2.set_yscale('log')
		#ax.set_xscale('log')
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

def plotU_Z(filename="freq.json"):
	#filename = "json/f0_100uH.json"
	#filename = "json/f0_7_5nF.json"
	#filename = "json/f0_short.json"
	#filename = "json/f0_1Om.json"
	#filename = "json/f0_50Om.json"
	#filename = "json/f0_100Om.json"
	filename = "json/f0_open.json"
	jout = readJson(filename)
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (Hz)')
	freq = jout['freq']
	I = jout['I']
	Q = jout['Q']
	Zre = []
	Zim = []
	U_Zre = []
	U_Zim = []
	U_Zphase = []
	KU = 0.5148
	fi = jout['fi']
	addToFiArr(fi, math.pi)

	for i in range(len(Q)):
		amp = I[i]/Q[i] * KU
		phase = fi[i]
		if phase < -(math.pi*0.5+0.1):
			phase += math.pi
			amp = -amp

		U15 = cmath.rect(amp, phase)
		r = zm.calcU15(U15)
		U_Zre.append(r.U_Z.real)
		U_Zim.append(r.U_Z.imag)
		Zre.append(r.Z.real)
		Zim.append(r.Z.imag)
		U_Zphase.append(cmath.phase(r.U_Z)) #*180./math.pi)

	if True:
		ax.plot (freq, U_Zphase, color='blue')
		ax.set_xscale('log')
		#ax.set_ylabel('Phase (Grad)')
	elif True:
		ax.plot (freq, U_Zre, color='red')
		ax.plot (freq, U_Zim, color='blue')
		#ax.set_ylim(0, plt.ylim()[1])
		ax.set_xscale('log')
		#ax.plot (freq, fsin)
	else:
		ax.set_ylabel('Resistance (Om)')
		ax.plot (freq, Zre, color='red')
		ax.plot (freq, Zim, color='blue')
		ax.set_xscale('log')
	plt.show()
	pass

def main():
	#plotOut()
	#plotIQ()
	#plotFreq()
	#plotTestPhase()
	#plotFi2()

	plotU_Z()
	pass

if __name__ == "__main__":
	main()
