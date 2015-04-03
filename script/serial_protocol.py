# coding=UTF-8
import time
import serial

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
	print("Send:" , full_data)
	ser.write(full_data)
	pass

def receive():
	global lastData
	time.sleep(0.01)
	command = None
	while ser.inWaiting() > 0:
		c = ser.read(1)
		print(c)
		if ord(c)==0xFF:
			command = decode(lastData)
			lastData = b''
			break
		else:
			lastData += c
	return command

def printBin(s):
	for c in s:
		print(hex(ord(c)))
	print
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
	send(2, b"DDDxyz")
	ret = receive()
	if ret:
		print(ret)

	send(3, b"abX")
	ret = receive()
	if ret:
		print(ret)

	close()
	pass

def test3():
	b = bytearray(2)
	b = (222).to_bytes(2, byteorder='little')
	for x in b:
		print(x)
	pass

test2()
