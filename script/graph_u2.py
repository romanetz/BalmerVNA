'''
Нарисуем график зависимости напряжения на U2 в
зависимости от подключенного сопротивления.
'''

import matplotlib.pyplot as plt
from z_math import *
import numpy as np

def formatComplex(c):
	if abs(c.imag)<1e-5:
		return "%.4f"%c.real
	if c.imag>0:
		return "%.4f+%.4fj"%(c.real,c.imag)
	return "%.4f%.4fj"%(c.real,c.imag)

def calcUx(R1, R2, R3, R4, R5, vcc=1.):
	'''
	Расчитываем напряжение и суммарное сопротивление элементов
	Схема такая.
	*----[R1]---*----[R4]---*
	|			|           |
	gnd			R3          vcc
	|			|           |
	*----[R2]---*----[R5]---*
	'''
	# U1 = R1*I1
	# U2 = R2*I2
	# U3 = (U2-U1)
	# U3 = R3*I3
	# U4 = R4*I4
	# U5 = R5*I5
	# I4 = I1-I3
	# I5 = I2+I3
	# U1+U4 = vcc
	# U2+U5 = vcc
	
	# приведем к каноническому виду
	# U1 - R1*I1 = 0
	# U2 - R2*I2 = 0
	# U3 - R3*I3 = 0
	# U4 - R4*I4 = 0
	# U5 - R5*I5 = 0
	# U1 - U2 + U3 = 0
	# -I1 + I3 + I4 = 0 
	# I2 + I3 - I5 = 0
	# U1+U4 = vcc
	# U2+U5 = vcc
	a = np.array([
	#U1, U2, U3, U4, U5, I1, I2, I3, I4, I5
	[ 1,  0,  0,  0,  0,-R1,  0,  0,  0,  0], # U1 - R1*I1 = 0
	[ 0,  1,  0,  0,  0,  0,-R2,  0,  0,  0], # U2 - R2*I2 = 0
	[ 0,  0,  1,  0,  0,  0,  0,-R3,  0,  0], # U3 - R3*I3 = 0
	[ 0,  0,  0,  1,  0,  0,  0,  0,-R4,  0], # U4 - R4*I4 = 0
	[ 0,  0,  0,  0,  1,  0,  0,  0,  0,-R5], # U5 - R5*I5 = 0
	[ 1, -1,  1,  0,  0,  0,  0,  0,  0,  0], # U1 - U2 + U3 = 0
	[ 0,  0,  0,  0,  0, -1,  0,  1,  1,  0], # -I1 + I3 + I4 = 0
	[ 0,  0,  0,  0,  0,  0,  1,  1,  0, -1], # I2 + I3 - I5 = 0
	[ 1,  0,  0,  1,  0,  0,  0,  0,  0,  0], # U1+U4 = vcc
	[ 0,  1,  0,  0,  1,  0,  0,  0,  0,  0], # U2+U5 = vcc
	])
	
	b = np.array([0,0,0,0,0,0,0,0,vcc,vcc])
	x = np.linalg.solve(a, b)
	#print(x, np.allclose(np.dot(a, x), b))
	U1 = x[0]
	U2 = x[1]
	U3 = x[2]
	U4 = x[3]
	U5 = x[4]
	I1 = x[5]
	I2 = x[6]
	I3 = x[7]
	I4 = x[8]
	I5 = x[9]
	Rsum = vcc/(I4+I5)
	if False:
		print("U1="+formatComplex(U1),
			"U2="+formatComplex(U2),
			"U3="+formatComplex(U3),
			"U4="+formatComplex(U4),
			"U5="+formatComplex(U5))
		print("I1="+formatComplex(I1),
			"I2="+formatComplex(I2),
			"I3="+formatComplex(I3),
			"I4="+formatComplex(I4),
			"I5="+formatComplex(I5))
		print("Rsum=", Rsum)
	return {"Rsum": Rsum, "U1": U1, "U3": U3}
	

def plotRaw(xdata, ydata):
	fig, ax = plt.subplots()
	ax.set_xlabel('Frequency (MHz)')
	for i in range(len(xdata)):
		xdata[i] *= 1e-6
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_ylabel('Y')
	ax.plot(xdata, ydata)
	plt.show()
	pass

	
R1 = 5j
R2 = 50
R3 = 50
R4 = 50
R5 = 50
out = calcUx(R1, R2, R3, R4, R5)
print(out["Rsum"])
