import math
import numpy as np

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
		#(A,B) = normSin(arr, freq, step)
		#(c0, A, B) = calcSinCos(arr, freq, step)
		(c0, A, B) = calcSinCosMatrix(np.array(arr), freq, step)		
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

def fullCycles(N, freq, step):
	'''
	N - максимально возможное количество точек
	freq - частота сигнала
	step - шаг по времени между точками
	'''
	cycle = 1.0/(step*freq)
	#print("cycle=", cycle)

	return int(int(N/cycle)*cycle)

def calcSinCos(arr, freq, step):
	'''
		return (c0,csin, ccos)
		c0+csin*sin(f)+ccos*cos(f)
	'''
	N = len(arr)
	N = fullCycles(N, freq, step)
	print("N=", N)
	fstep = 2*math.pi*step*freq
	
	sum_c0 = 0
	for i in range(N):
		sum_c0 += arr[i]
	c0 = sum_c0/N

	for i in range(N):
		arr[i] -= c0

	sum_sin = 0
	sum_cos = 0
	for i in range(N):
		sum_sin += arr[i]*math.sin(fstep*i)
		sum_cos += arr[i]*math.cos(fstep*i)

	csin = sum_sin*2/N
	ccos = sum_cos*2/N

	#print "c0=", c0
	#print("csin=", csin)
	#print("ccos=", ccos)
	return (c0, csin, ccos)

def calcFi(csin, ccos):
	'''
		input 
		csin*sin(f)+ccos*cos(f) == amplitude*sin(f+fi)
		return (amplitude, fi)
	'''
	amplitude = math.sqrt(csin*csin+ccos*ccos)
	#print "amplitude=", "{:.2f}".format(amplitude)
	csin /= amplitude
	ccos /= amplitude

	#print "sin=", math.acos(csin)
	#print "cos=", math.asin(ccos)
	fi = math.atan2(ccos, csin)
	#print "fi=", fi
	return (amplitude, fi)


def calcSinCosMatrix(arr, freq, step):
	'''
		return (c0,csin, ccos)
		c0+csin*sin(f)+ccos*cos(f)
	'''
	N = len(arr)
	fcycle = 2*math.pi*freq
	y = np.array(arr)

	timeList = np.linspace(0, step*(N-1), num=N)
	fsin = np.sin(fcycle*timeList)
	fcos = np.cos(fcycle*timeList)

	c0 = np.sum(y)
	c1 = np.sum(y*fcos)
	c2 = np.sum(y*fsin)
	m00 = N
	m01 = np.sum(fsin)
	m02 = np.sum(fcos)
	m10 = np.sum(fcos)
	m11 = np.sum(fsin*fcos)
	m12 = np.sum(fcos*fcos)
	m20 = np.sum(fsin)
	m21 = np.sum(fsin*fsin)
	m22 = np.sum(fcos*fsin)

	M = np.array([[m00, m01, m02], [m10, m11, m12], [m20, m21, m22]])
	#print("M=", M)
	C = np.array([c0, c1, c2])
	x = np.linalg.solve(M, C)
	#print("const=", x[0], "csin=", x[1], "ccos=", x[2])
	return (x[0], x[1], x[2])

def main():
	N = 256
	freq = 1111
	timeList = np.linspace(0, STEP*(N-1), num=N)
	phase = 1.5*math.pi
	fcycle = 2*math.pi*freq
	arr = np.sin(2*math.pi*freq*timeList+phase)
	calcSinCosMatrix(arr, freq, STEP)
	pass

if __name__ == "__main__":
	main()
