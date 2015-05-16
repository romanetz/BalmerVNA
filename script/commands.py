import struct
import time
import array
import json
import math
import smath as sm

import serial_protocol
from serial_protocol import send,receive

COMMAND_NONE = 0
COMMAND_BIG_DATA = 1
COMMAND_SET_FREQ = 2
COMMAND_START_SAMPLING = 3
COMMAND_SAMPLING_COMPLETE = 4
COMMAND_SAMPLING_BUFFER_SIZE = 5
COMMAND_GET_SAMPLES = 6
COMMAND_SET_TX = 7
COMMAND_GET_CALCULATED = 8
COMMAND_START_SAMPLING_AND_CALCULATE = 9
COMMAND_CS4272_READ_REG = 10

JOB_NONE = 0
JOB_SAMPLING = 2
JOB_CALCULATING = 3
JOB_CALCULATING_COMPLETE = 4

SAMPLING_BUFFER_SIZE = None

def getLogArray(fmin, fmax, count):
	freq = []
	flmin = math.log(fmin)
	flmax = math.log(fmax)
	for i in range(count):
		fl = (flmax-flmin)*i/(count-1.0)+flmin
		freq.append(math.exp(fl))
	return freq

def getFreq():
	freq = []
	'''
	for f in range(1, 10):
		freq.append(f*1e4)
	for f in range(1, 10):
		freq.append(f*1e5)
	for f in range(1, 101, 1):
		freq.append(f*1e6)
	'''
	#for f in range(30, 101, 1):
	#	freq.append(f*1e6)
	freq = getLogArray(10e3, 10e6, 100)
	#freq = getLogArray(7.99e6, 8.02e6, 150)
	return freq

def sendNone():
	send(COMMAND_NONE)
	ret = receive()
	print("none receive=", ret)
	pass

def sendBigData(amin, amax):
	send(COMMAND_BIG_DATA, struct.pack("HH", amin, amax))
	ret = receive()
	assert(ret[0]==COMMAND_BIG_DATA)
	if len(ret)!=(1+(amax-amin)*2):
		print("bad size :", ret)

	bad = False
	offset = 1
	for x in range(amin, amax):
		data = struct.unpack_from("H", ret, offset)[0]
		if data!=x:
			bad = True
			break
		offset += 2

	if bad:
		print("bad value offset=", offset, ":",ret)
	else:
		print("range received ok ", amin, ",", amax, " size=", len(ret))
	pass

def setFreq(freq, level=200):
	freq = int(freq)
	#print("level=", level)
	send(COMMAND_SET_FREQ, struct.pack("=Ii", freq, int(level)))
	data = receive()
	assert(data[0]==COMMAND_SET_FREQ)
	retFreq = struct.unpack_from('I', data, 1)[0]
	#print("ret=", retFreq)
	pass

def startSampling():
	send(COMMAND_START_SAMPLING)
	pass

def samplingCompleted():
	send(COMMAND_SAMPLING_COMPLETE)
	data = receive()
	assert(data[0]==COMMAND_SAMPLING_COMPLETE)
	return bool(data[1])

def samplingBufferSize():
	send(COMMAND_SAMPLING_BUFFER_SIZE)
	data = receive()
	assert(data[0]==COMMAND_SAMPLING_BUFFER_SIZE)
	return struct.unpack_from('H', data, 1)[0]	

def setTX(tx):
	if tx:
		tx = 1
	else:
		tx = 0
	send(COMMAND_SET_TX, struct.pack("=B", tx))
	data = receive()
	assert(data[0]==COMMAND_SET_TX)
	assert(data[1]==tx)
	pass


def getSamples(sampleQ, offset, count):
	send(COMMAND_GET_SAMPLES, struct.pack("=BHH", sampleQ, offset, count))
	data = receive()
	if data==None:
		send(COMMAND_NONE)
		data = receive()
		dataNone = receive()
		assert(data!=None)
		assert(len(dataNone)==2)
		print("dataNone=", dataNone, "lenData=", len(data))

	#print("len(data)=", len(data))
	out = array.array('i', data)
	#print(out)
	return out.tolist()

def getAllSamples(sampleQ):
	offset = 0
	samples = []
	count = 200
	while offset<SAMPLING_BUFFER_SIZE:
		if offset+count>SAMPLING_BUFFER_SIZE:
			count = SAMPLING_BUFFER_SIZE-offset
		#print("offset=", offset)
		tmpSamples = getSamples(sampleQ, offset, count)
		
		if tmpSamples==None:
			print("Return null. Restarting.")
			serial_protocol.clearQueue()
			continue
		
		samples += tmpSamples
		offset += count

	return samples

def sqrMean(arr):
	'''
	Простейшее среднеквадратическое отклонение
	'''
	mid = 0.0
	for x in arr:
		mid += x

	mid /= len(arr) 
	sum = 0.0
	for x in arr:
		s = x-mid
		sum += s*s

	return math.sqrt(sum/len(arr))

def getSamplesByFreq(freq):
	setFreq(freq)
	time.sleep(0.01)
	startSampling()
	ok = False
	for i in range(10):
		time.sleep(0.01)
		ok = samplingCompleted()
		if ok:
			break
	assert(ok)

	ISampes = getAllSamples(False)
	QSampes = getAllSamples(True)
	return (ISampes, QSampes)


def writeSamples(samples):
	jout = {}
	jout['data'] = samples
	f = open('out.json', 'wt')
	f.write(json.dumps(jout))
	f.close()
	pass

def scanFreq():
	freq = getFreq()
	IArray = []
	QArray = []
	FIarray = []
	fsin = []
	for f in freq:
		print("f=", f)
		(ISampes, QSampes) = getSamplesByFreq(f)
		I = sqrMean(ISampes)
		Q = sqrMean(QSampes)
		print("getSqrByFreq=", I, Q)
		IArray.append(I)
		QArray.append(Q)

		#freqDelta = 1
		#freqCenter = 1000
		freqDelta = 0.5
		freqCenter = 999.6
		count = 101
		(freqArr, Fmath) = sm.arrayFreq(QSampes, freqCenter-freqDelta, freqCenter+freqDelta, sm.STEP, count)
		fmax = sm.findFreqMax(freqArr, Fmath)
		fsin.append(fmax)
		(Ic0, Isin, Icos) = sm.calcSinCosMatrix(ISampes, fmax, sm.STEP)
		(Qc0, Qsin, Qcos) = sm.calcSinCosMatrix(QSampes, fmax, sm.STEP)
		(Iamplitude, Ifi) = sm.calcFi(Isin, Icos)
		(Qamplitude, Qfi) = sm.calcFi(Qsin, Qcos)
		FIarray.append(Ifi-Qfi)
		print("fmax=", fmax, "fi=", Ifi-Qfi)
		pass

	jout = {}
	jout['freq'] = freq
	jout['I'] = IArray
	jout['Q'] = QArray
	jout['fsin'] = fsin
	jout['fi'] = FIarray
	f = open('freq.json', 'wt')
	f.write(json.dumps(jout))
	f.close()
	pass

def levelFreq(f):
	fmin = 37
	fmax = 100
	a = (f*1e-6-fmin)/(fmax-fmin)
	b0 = 200
	b1 = 237
	if a<=0:
		return b0
	if a>1:
		return b1
	return b0*(1-a)+b1*a

def scanFreqHard():
	freq = getFreq()
	IArray = []
	QArray = []
	FIarray = []
	fsin = []


	for i in range(2):	
		setFreq(freq[0]) #, levelFreq(f))
		time.sleep(0.2)
		data = samplingAndCalculate()

	for f in freq:		
		setFreq(f) #, levelFreq(f))
		time.sleep(0.02)
		data = samplingAndCalculate()
		result_freq = data[0];
		result_q_cconst = data[1];
		result_q_csin = data[2];
		result_q_ccos = data[3];
		result_q_sqr = data[4];
		result_i_cconst = data[5];
		result_i_csin = data[6];
		result_i_ccos = data[7];
		result_i_sqr = data[8];
		result_time = data[9];

		print("f=", f, "fmax=", result_freq);


		#IArray.append(result_i_sqr)
		#QArray.append(result_q_sqr)

		(Iamplitude, Ifi) = sm.calcFi(result_i_csin, result_i_ccos)
		(Qamplitude, Qfi) = sm.calcFi(result_q_csin, result_q_ccos)
		IArray.append(Iamplitude)
		QArray.append(Qamplitude)
		FIarray.append(Ifi-Qfi)
		print("Iamplitude=", Iamplitude, "Qamplitude=", Qamplitude)
		print("fi=", Ifi-Qfi)
		pass

	jout = {}
	jout['freq'] = freq
	jout['I'] = IArray
	jout['Q'] = QArray
	jout['fsin'] = fsin
	jout['fi'] = FIarray
	f = open('freq.json', 'wt')
	f.write(json.dumps(jout))
	f.close()
	pass

def samplingOne(freq):
	setFreq(freq)
	time.sleep(0.01)
	startSampling()
	for i in range(10):
		time.sleep(0.01)
		ok = samplingCompleted()
		print("ok=",ok)
		if ok:
			break
	assert(ok)

	samplesI = getAllSamples(sampleQ=False)
	samplesQ = getAllSamples(sampleQ=True)
	jout = {}
	jout['freq'] = freq
	jout['I'] = samplesI
	jout['Q'] = samplesQ
	f = open('out.json', 'wt')
	f.write(json.dumps(jout))
	f.close()

	print( "sqrI=", sqrMean(samplesI))
	print( "sqrQ=", sqrMean(samplesQ))
	pass

def startSampling():
	send(COMMAND_START_SAMPLING_AND_CALCULATE)
	ret = receive()
	assert(ret[0]==COMMAND_START_SAMPLING_AND_CALCULATE)
	pass

def getCalculated():
	send(COMMAND_GET_CALCULATED)
	ret = receive()
	assert(ret[0]==COMMAND_GET_CALCULATED)
	if ret[1]!=JOB_CALCULATING_COMPLETE:
		#print("ret[1]=", ret[1])
		return [ret[1],]

	data = struct.unpack_from("fffffffffH", ret, 2)
	#print("data=", data)
	return data

def samplingAndCalculate():
	startSampling()

	data = None

	for x in range(10):
		data = getCalculated()
		if len(data)>1:
			break
		time.sleep(0.05)
		pass
	return data

def readCs4272Reg(reg):
	send(COMMAND_CS4272_READ_REG, struct.pack("=B", reg))
	ret = receive()
	assert(ret[0]==COMMAND_CS4272_READ_REG)
	assert(ret[1]==reg)
	val = ret[2]
	print("reg=", reg, "val=", hex(val), bin(val))
	return val


def main():
	global SAMPLING_BUFFER_SIZE
	if not serial_protocol.connect():
		print("Cannot connect device")
	time.sleep(0.01)
	sendNone()
	#sendBigData(0,100)
	setFreq(100001)
	#startSampling()
	#print("samplingCompleted=",samplingCompleted())
	SAMPLING_BUFFER_SIZE = samplingBufferSize()
	setTX(0)

	#readCs4272Reg(0x1)
	#samplingOne(100000)
	scanFreqHard()
	#print(samplingAndCalculate())

	pass
def test():
	#data = struct.pack("H", 1234)
	#print(len(data))
	#print(bool(0))
	#print(getFreq())
	print(getLogArray(100e3, 100e6, 100))
	pass

if __name__ == "__main__":
	main()
	#test()
