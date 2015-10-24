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
	if False:
		#print("U1+U4=", U1+U4)
		#print("U2+U5=", U2+U5)
		#print("U3=", U3, "U2-U1", U2-U1)
		print("I2=", I2)

	return {"Rsum": Rsum, "U1": U1, "U3": U3, "I1": I1, "I2": I2, "I3" : I3}

def calcRx(U3, R2, R3, R4, R5, vcc=1.):
	'''
	Все параметры как в calcUx, но R1 - неизвестно, вместо него известно U3
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
	
	#избавимся от U1, U2
	# U3 = (R2*I2-R1*I1)
	# U3 = R3*I3
	# U4 = R4*I4
	# U5 = R5*I5
	# I4 = I1-I3
	# I5 = I2+I3
	# R1*I1+U4 = vcc
	# R2*I2+U5 = vcc
	
	#избавимся от I3, U4, U5
	# U3 = (R2*I2-R1*I1)
	# I4 = I1-U3/R3
	# I5 = I2+U3/R3
	# R1*I1+R4*I4 = vcc
	# R2*I2+R5*I5 = vcc
	
	#избавимся от I4, I5
	#остались неизвестные I1, I2, R1
	# U3 = (R2*I2-R1*I1)
	# R1*I1+R4*(I1-U3/R3) = vcc
	# R2*I2+R5*(I2+U3/R3) = vcc
	
	# перегруппируем I1, I2
	# -R1*I1 + R2*I2 = U3
	# (R1+R4)*I1 - R4/R3*U3 = vcc
	# (R2+R5)*I2 + R5/R3*U3 = vcc
	
	#I2 оказалось независимой??????
	I3 = U3/R3
	I2 = (vcc - R5*I3) / (R2+R5)
	# I1*R1 = I1R1 = U1 новая переменная (для линейности)
	U1 = R2*I2 - U3
	I1 = I3 + (vcc - U1)/R4
	R1 = U1/I1

	return {'R1' : R1, 'U1' : U1}
	
def calcUxBig(R1, R2, R3, R4, R5, R6, R7, vcc=1.):
	'''
	Расчитываем напряжение и суммарное сопротивление элементов
	Схема такая.
	*----[R1]---*----[R4]---*---*---[R7]---vcc
	|			|           |   |           
	gnd			R3          |   R6
	|			|           |   |
	*----[R2]---*----[R5]---*  gnd
	'''
	Rsum = calcUx(R1, R2, R3, R4, R5)["Rsum"]
	Rs6 = 1./(1/Rsum+1/R6)
	vcc1 = vcc*Rs6/(Rs6+R7)
	out = calcUx(R1, R2, R3, R4, R5, vcc1)
	out['vcc1'] = vcc1
	return out

def calcRxBig(U3, R2, R3, R4, R5, R6, R7, vcc=1.):
	'''
	Все параметры как в calcUx, но R1 - неизвестно, вместо него известно U3
	calcRx vcc переименовываем в vcc1
	'''
	# (I4+I5)+vcc1/R6 = (vcc-vcc1)/R7
	#(I4+I5) = (I1+I2)
	# I2 = (vcc1 - R5*I3) / (R2+R5) = vcc1/(R2+R5)+C0
	I3 = U3/R3
	C0 = - R5*I3 / (R2+R5)
	# I1 = I3 + (vcc1 - (R2*I2 - U3))/R4 = I3 + (vcc1 + U3 - R2*I2)/R4
	# I1 = I3 + (vcc1 + U3 - R2*(vcc1/(R2+R5)+C0))/R4 
	# I1 = vcc1*(1-R2/(R2+R5))/R4 + I3 + (U3 - R2*C0)/R4 
	C1 = (1-R2/(R2+R5))/R4
	C2 = I3 + (U3 - R2*C0)/R4
	# I1 = vcc1*C1+C2
	# I1+I2 = vcc1*(C1+1/(R2+R5)) + C2 + C0
	C3 = (C1+1/(R2+R5))
	# I1+I2 = vcc1*C3 + C2 + C0
	# I1+I2 + vcc1*(C3 + 1/R6 + 1/R7) + C2 + C0 -vcc/R7

	a = (C3 + 1/R6 + 1/R7)
	b = C2 + C0 -vcc/R7
	vcc1 = -b / a

	out = calcRx(U3, R2, R3, R4, R5, vcc1)
	out['vcc1'] = vcc1
	return out


R1 = 0
R2 = R21 #50
R3 = 82
R4 = 50
R5 = 50
R6 = 270
R7 = 18
out = calcUx(R1, R2, R3, R4, R5)
Rsum = out['Rsum']
Rs = 1/(1/Rsum + 1/R6) 
print(Rs/(Rs+R7))

def plot():
	R1arr = []
	for i in range(0, 100):
		R1arr.append(i*0.01)
	for i in range(1, 300):
		R1arr.append(i*1.)
		
	Uxarr = []
	U15arr = []
	for i in range(len(R1arr)):
		R1 = R1arr[i]
		#out = calcUx(R1, R2, R3, R4, R5)
		vcc = 1.
		out = calcUxBig(R1, R2, R3, R4, R5, R6, R7, vcc)
		U3 = out['U3']
		out1 = calcRxBig(U3, R2, R3, R4, R5, R6, R7, vcc)
		Uxarr.append(out['U3'])
		#Uxarr.append(out['U3'])
		#out1 = calcRx(U3, R2, R3, R4, R5)
		#U15arr.append(out['U1'])
		U15arr.append(out1['U1'])
		pass
		
	fig, ax = plt.subplots()
	#ax.set_xlabel('Frequency (MHz)')
	#for i in range(len(xdata)):
	#	xdata[i] *= 1e-6
	#ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_xlabel('R (Om)')
	ax.set_ylabel('U (Volts)')
	ax.plot(R1arr, Uxarr, color='blue')
	ax.plot(R1arr, U15arr, color='red')
	
	#ax.plot([0,100], [0,0], color='black')
	#ax.plot([50, 50], [-0.1, +0.35], '--', color="#FF8000")
	plt.show()
	pass

plot()