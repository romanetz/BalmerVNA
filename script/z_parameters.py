'''
Решим еще одну систему линейных уавнений.
Z параметры по входному/выходному току и напряжению.

(U1)  (z11  z12) (I1)
	= (        )*
(U2)  (z21  z22) (I2)

U1 = z11*I1 + z12*I2
U2 = z21*I1 + z22*I2


http://www.rfcafe.com/references/electrical/s-h-y-z.htm

Для щупы1 имеем калибровку по ABCD параметрам (известны 3 коэффициэнта из четырех)
https://en.wikipedia.org/wiki/Two-port_network

Попробуем пользуясь имеющийся информацией нарисовать графики зависимости 
S11 и S21 от R

S параметры - это нормированные на Z0 (некое характерное сопротивление. например 50 Ом) напряжения прямой и отраженной волны.

Пускай у нас сразу будут нормированные ABCD параметры.
	V1 = A*V2 - B*I2
	I1 = C*V2 - D*I2
	
	B -> B/Z0 C -> C*Z0
	V1 = A*V2 - B*Z0*I2
	I1 = C/Z0*V2 - D*I2

Немного про S параметры.	
	U1p, U2p - напряжение прямой волны
	U1m, U2m - напряжение отраженной волны
	Z1p - волновое сопротивление, Z1p` - complex conjugate Z1p
	Но обычно заменяют Z1p, Z1p` на Z0 = 50 Ом, и получают некие коэффициэнтты.
	U1p = 0.5*(U1+I1*Z1p)
	U1m = 0.5*(U1-I1*Z1p`)
	
	U2p = 0.5*(U1+I1*Z2p)
	U2m = 0.5*(U1-I1*Z2p`)
	
	U1m = s11*U1p + s12*U2p
	U2m = s21*U1p + s22*U2p
	

rx ---(щупы1) -- Z -- (щупы2) -- tx (50 Ом)

На перед Z есть определенное напряжение и ток, предположим что они известны V1, I1.
На месте tx есть сопротивление R0 = 50 Ом.
Соответственно ток будет R0*I2 = V2
V2 мы также измерили.

Transmision калибровка.
	Мы можем узнать Vrx, Z с точки зрения rx канала. (Это будет сдвиг и ослабление линии + 50 Ом).
	Мы можем узнать Vtx с точки зрения tx канала. Vtx - это уже относительное напряжение, деленное на Vgen.
	Vrx - это число около нуля. Поэтому фазу надо калибровать по Vgen?

	Как посчитать сдвиг фазы до измеряемого элемента?
	Возможно это не нужно, и достаточно полного сдвига фаз.
	Vtx_tr = напряжение измеренное для transmission калибровки (т.е замыкаем щупы RX/TX)
	
	Элемент посередке вносит сдвиг фазы и амплитуды. Ktx = Vtx_measured / Vtx_tr
	Можно еще разделить на V1 - входное напряжение (которое известно).
	Ktx = 1 Z = 0
	Ktx = 0 Z = inf
	
	Для крайне низкой частоты.
	(Rm+R0)/V1 = R0/Vtx
	Rm = R0*(V1/Vtx-1)
	
	(Rm+R0)*Vtx = R0*V1
	Rm*Vtx = R0*(V1-Vtx)
	
	Rm = R0*(V1-Vtx)/Vtx = R0*(V1/Vtx-1)
	
	если V1 будет изменяться от 0 до 1 и V2 будет изменяться от 0 до 1, то может и прокатит.
	
Пускай у нас будет
	Vrx, Vtx и ABCD матрица, которая включает в себя всю информацию.
	V1 = A*V2 - B*I2
	I1 = C*V2 - D*I2
	
	V1 = Vrx, I1 = Irx
	V2 = Vtx, I2 = Vtx/R0

	Vrx = A*Vtx - B*Vtx/R0
	Irx = C*Vtx - D*Vtx/R0
	
	Vrx = Vtx*(A - B/R0)
	Irx = Vtx*(C - D/R0)
	
	Пускай Vtx = Vrx*K
	K = коэффициэнт передачи от rx к tx.
	
Матрица передачи кабеля.
	[ch    Z0*sh]
	[sh/Z0 ch   ]
	
	ch = cos(fi)*a, sh = sin(fi)*a
	fi - сдвиги фазы по всей длинне кабеля.

	Zrx = ABCDdut*ABCDtx*Z0	
'''

import cmath
import math
import numpy as np
import matplotlib.pyplot as plt

class ABCD:
	'''
		a11 = A, a12 = B
		a21 = C, a22 = D
		V1 = A*V2 - B*I2
		I1 = C*V2 - D*I2
		Для взаимных четырехполюсников A*D-B*C = 1
		Для симметричных четырехполюсников A = D
	'''
	def __init__(self, A = 1., B = 0., C = 0., D = 1.):
		self.a = np.array(
		[ [A, B],
		  [C, D]
		]);
		pass
		
	def setSeriesResistor(self, Z):
		# Z - резистор подключенный последовательно.
		self.a = np.array(
		[ [1., Z],
		  [0., 1.]
		]);
		pass
		
	def setShuntConductor(self, Y):
		#Y = 1/Z - проводимость подключенная параллельно
		self.a = np.array(
		[ [1., 0.],
		  [ Y, 1.]
		]);
		pass

	def setTransmission(self, gl, Z0):
		'''
		gl = l*gamma
		l = length
		gamma = a + j*b
		a - attenuation constant
		b - phase constant
		Z0 - characteristic impedance
		'''
		ch = cmath.cosh(gl)
		sh = cmath.sinh(gl)
		self.a = np.array(
		[ [ch, sh*Z0],
		  [sh/Z0, ch]
		]);
		pass
	def dot(self, v):
		return np.dot(self.a, v)
		
	def dotAbcd(self, abcd2):
		out = ABCD()
		out.a = np.dot(self.a, abcd2.a)
		return out


def testR():
	v = np.array([1, 1])
	abcd = ABCD()
	#abcd.setSeriesResistor(1)
	abcd.setShuntConductor(1)
	v1 = abcd.dot(v)
	print(v1)

def testTr():
	Z0 = 50.
	v = np.array([1, 1/(Z0+10)])
	abcd = ABCD()
	#abcd.setSeriesResistor(1)

	xdata = []
	ydata = []
	count = 100
	for i in range(count):
		fi = i*math.pi/(count-1)
		abcd.setTransmission(fi*1j, Z0)
		v1 = abcd.dot(v)
		z1 = v1[0]/v1[1]
		#print(v1, 'z=', abs(z1))
		xdata.append(z1.real)
		ydata.append(z1.imag)


	fig, ax = plt.subplots()
	ax.set_aspect('equal')
	ax.plot(xdata, ydata)
	plt.show()
	pass

def testZser():
	Z0 = 50.
	abcdTx = ABCD()
	abcdTx.setTransmission(1j, Z0)
	abcdRx = ABCD()
	abcdRx.setTransmission(.5j, Z0)

	abcdDut = ABCD()
	abcdDut.setSeriesResistor(10)

	abcdDutTx = abcdDut.dotAbcd(abcdTx)
	abcdRxDutTx = abcdRx.dotAbcd(abcdDutTx)
	print(abcdRxDutTx.a)

	abcdRxTx = abcdRx.dotAbcd(abcdTx)
	abcdDutRxTx = abcdDut.dotAbcd(abcdRxTx)
	print(abcdDutRxTx.a)

	pass

#testTr()
testZser()