import struct
import time
import array
import json
import math

import serial_protocol
from serial_protocol import send,receive

COMMAND_NONE = 0
COMMAND_BIG_DATA = 1
COMMAND_SET_FREQ = 2
COMMAND_START_SAMPLING = 3
COMMAND_SAMPLING_COMPLETE = 4
COMMAND_SAMPLING_BUFFER_SIZE = 5
COMMAND_GET_SAMPLES = 6


SAMPLING_BUFFER_SIZE = None

def getFreq():
	freq = []
	for f in range(1, 10):
		freq.append(f*100e3)
	for f in range(1, 31):
		freq.append(f*1e6)
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

def setFreq(freq):
	freq = int(freq)
	send(COMMAND_SET_FREQ, struct.pack("=I", freq))
	data = receive()
	assert(data[0]==COMMAND_SET_FREQ)
	retFreq = struct.unpack_from('I', data, 1)[0]
	print("ret=", retFreq)
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
		print("offset=", offset)
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

	return math.sqrt(sum)/len(arr)

def getSqrByFreq(freq):
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
	return (sqrMean(ISampes), sqrMean(QSampes))


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
	for f in freq:
		print("f=", f)
		(I,Q) = getSqrByFreq(f)
		print("getSqrByFreq=", I, Q)
		IArray.append(I)
		QArray.append(Q)

	jout = {}
	jout['freq'] = freq
	jout['I'] = IArray
	jout['Q'] = QArray
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
	pass

def main():
	global SAMPLING_BUFFER_SIZE
	if not serial_protocol.connect():
		print("Cannot connect device")
	time.sleep(0.01)
	sendNone()
	#sendBigData(0,100)
	setFreq(120000)
	#startSampling()
	print("samplingCompleted=",samplingCompleted())
	SAMPLING_BUFFER_SIZE = samplingBufferSize()
	#print("bufSize=", SAMPLING_BUFFER_SIZE)
	#getSamples(False, 0, 5)
	#samplesI = getAllSamples(sampleQ=False)
	#writeSamples(samplesI)
	#print(len(samplesI))

	samplingOne(100e3)

	#print("getSqrByFreq=", getSqrByFreq(100000))
	#scanFreq()

	pass
def test():
	data = struct.pack("H", 1234)
	print(len(data))
	print(bool(0))
	print(getFreq())
	pass

if __name__ == "__main__":
	main()
	#test()
