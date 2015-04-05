# coding=UTF-8
import time
import serial
import struct

# ls /dev/cu.* <--- find device in Mac OS X

ser = None
lastData = b''

def connect():
	global ser
	ser = serial.Serial(
		#port='/dev/cu.usbmodem621', #left USB
		port='/dev/cu.usbmodem411', #right USB
		baudrate=115200,
		parity=serial.PARITY_ODD,
		stopbits=serial.STOPBITS_TWO,
		bytesize=serial.SEVENBITS
	)

	# ls /dev/cu.* <--- find device
	return ser.isOpen()

def close():
	ser.close()

def encode(data):
	ret = bytearray()
	if data is None:
		return ret

	for c in data:
		oc = c
		if oc>=0xFE:
			ret.append(0xFE)
			ret.append(oc-0xFE)
		else:
			ret.append(c)
	return ret

def decode(data):
	ret = bytearray()
	last_is_FE = False
	for c in data:
		oc = c
		assert(oc!=0xFF)
		if oc==0xFE:
			last_is_FE = True
			continue

		if last_is_FE:
			if oc==0:
				c=0xFE
			elif oc==1:
				c=0xFF
			else:
				assert(0)
			last_is_FE = False
		ret.append(c)

	return ret

def send(command, data=None):
	#в начало добавить command 
	#добавить data, преобразовать FE, FF как надо
	#в конец добавить FF
	assert(command>=0 and command<0xFF)
	full_data = bytearray()
	full_data.append(command)
	full_data += encode(data)
	full_data.append(0xFF)
	#print("Send:" , full_data)
	ser.write(full_data)
	ser.flush()
	pass

def receive():
	global lastData
	time.sleep(0.01)

	command = None
	for i in range(10):
		while ser.inWaiting() > 0:
			c = ser.read(1)
			#print(c)
			if ord(c)==0xFF:
				command = decode(lastData)
				lastData = b''
				break
			else:
				lastData += c
		if command!=None:
			break
		else:
			time.sleep(0.01)
			print("receive null", i)
		pass

	if command==None:
		if len(lastData)>20:
			print(command, len(lastData))
		else:
			print(command, lastData)
	elif len(command)>20:
		print(len(command), lastData)
	else:
		print(command, lastData)
	return command

def printBin(s):
	for c in s:
		print(hex(ord(c)))
	print
	pass

def clearQueue():
	ser.flushInput()
	ser.flushOutput()
	pass

def test():
	s = b'\xFF'
	s += b'abc'
	s += b'\xFE'

	se = encode(s)
	print(se)
	s1 = decode(se)
	print(s1)
	pass

def test2():
	if not connect():
		print("Cannot connect device")
	if False:
		send(2, b"DDDxyz")
		ret = receive()
		if ret:
			print(ret)

		send(3, b"abX")
		ret = receive()
		if ret:
			print(ret)

	sendRange(3,8)
	sendRange(1,30)
	sendRange(0,100)
	sendRange(0,300)
	sendRange(0,600)
	#sendRange(0,1200)

	close()
	pass

def test3():
	b = bytearray(2)
	b = (222).to_bytes(2, byteorder='little')
	for x in b:
		print(x)
	pass

if __name__ == "__main__":
	test2()
	pass

