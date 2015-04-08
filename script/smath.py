import math

#шаг с которым идут сэмплы в данных
STEP = 1.0/48000

def normSin(arr, freq, step):
	'''
	Вычисляет перемноженное с синусом и косинусом значение
	'''
	fstep = 2*math.pi*step*freq
	asum = 0.
	bsum = 0.
	count = len(arr)
	for i in range(count):
		x = arr[i]
		asum += x*math.cos(fstep*i)
		bsum += x*math.sin(fstep*i)
	return (asum/count, bsum/count)

def arrayFreq(arr, freqMin, freqMax, step, count):
	out = []
	farr = []
	fdelta = (freqMax-freqMin)/(count-1)
	for i in range(count):
		freq = fdelta*i+freqMin
		(A,B) = normSin(arr, freq, step)
		out.append(math.sqrt(A*A+B*B))
		farr.append(freq)
		#print("freq=", freq, A, B)
	return (farr, out)

def findFreqMax(farr, out):
	smax = out[0]
	imax = 0
	for i in range(len(farr)):
		if smax<out[i]:
			smax = out[i]
			imax = i
	return farr[imax]

def freqWord(freq):
	PLL_MUL = 20
	REFCLK = 25000000.0
	MASTER_CLOCK = (REFCLK*PLL_MUL)
	return freq*(0xFFFFFFFF/MASTER_CLOCK+1.0/MASTER_CLOCK)
