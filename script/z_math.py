import math
import cmath

R477 = 470+47
R16 = 18.
R22 = 270.
R14 = 49.9
R19 = 49.9
R21 = 1./(1/49.9+1/R477)
R15 = 82.

class Result:
	pass

def calcUZ(U_Z, U):
	C_R16 = 1/R16+1/R14+1/R19+1/R22
	C_R19 = 1/R19+1/R21+1/R15
	U_R16 = (U/R16 + U_Z*(1/R14+1/(R15*C_R19*R19)))/(C_R16-1/(R19*C_R19*R19))
	U_R19 = (U_Z/R15 + U_R16/R19)/C_R19
	#C_Z_1 = 1/C_Z
	C_Z_1 = U_Z/(U_R16/R14 + U_R19/R15)
	Z = C_Z_1/(1-C_Z_1/R14-C_Z_1/R15)
	print("U_R16=", U_R16)
	print("U_R19=", U_R19)
	print("U_R15=", U_Z - U_R19)
	print("Z=", Z)
	r = Result()
	r.Z = Z
	r.U = U
	r.U_Z = U_Z
	r.U_R16 = U_R16
	r.U_R19 = U_R19
	r.U_R15 = U_Z - U_R19
	return r

def calcU15(U_R15, U):
	C_R16 = 1/R16+1/R14+1/R22+1/R19
	C_R19 = 1/R19+1/R21
	U_R19 = ((U/R16 + U_R15/R14)/C_R16 + U_R15/R15*R19)/(C_R19*R19-(1/R14+1/R19)/C_R16)
	U_R16 = U_R19*C_R19*R19 - U_R15/R15*R19
	Z = (U_R19+U_R15)/((U_R16-U_R19-U_R15)/R14 - U_R15/R15)
	#Z = 1./(1/Z - 1/R477)
	Z = Z*R477/(R477 - Z)
	#print("U_R16=", U_R16)
	#print("U_R19=", U_R19)
	#print("Z=", Z)

	r = Result()
	r.Z = Z
	r.U = U
	r.U_Z = U_R19+U_R15
	r.U_R16 = U_R16
	r.U_R19 = U_R19
	return r

def calcZ(Z, U):
	Z = Z*R477/(Z+R477)
	C_R16 = (-1/R16-1/R14-1/R19-1/R22)
	C_R15 = (-1/R14-1/Z-1/R15)
	C00 = (1/R14-R15/R19*C_R15)
	C01 = (R15*(1/R21+1/R19)*C_R15-1/R14-1/Z)
	C10 = (C_R16-R15/R19/R14)
	C11 = (1/R14+1/R19+R15*(1/R21+1/R19)/R14)
	U_R16 = U/R16/(C00/C01*C11-C10)
	U_R19 = -U_R16*C00/C01
	U_R15 = -U_R16*R15/R19 + U_R19*R15*(1/R21+1/R19)
	#print("U_R16=",U_R16)
	#print("U_R19=",U_R19)
	#print("U_R15=",U_R15)

	return U_R15

def Check(r):
	Z = r.Z
	U = r.U
	U_Z = r.U_Z
	U_R16 = r.U_R16
	U_R19 = r.U_R19

	I_R16 = (U-U_R16)/R16
	I_R22 = U_R16/R22
	I_R14 = (U_R16-U_Z)/R14
	I_R19 = (U_R16-U_R19)/R19
	I_Z = U_Z/Z
	I_R15 = (U_Z - U_R19)/R15
	I_R21 = U_R19/R21
	print("I_R14=", I_R14)
	print("I_R15=", I_R15)
	print("I_R16=", I_R16)
	print("I_R21=", I_R21)
	print("I_R22=", I_R22)

	print("I_R16 - I_R14 - I_R19 - I_R22=", I_R16 - I_R14 - I_R19 - I_R22)
	print("I_R14 - I_Z - I_R15=", I_R14 - I_Z - I_R15)
	print("I_R19 - I_R21 + I_R15=", I_R19 - I_R21 + I_R15)
	pass


def main():
	#r = calcUZ(0.4, 1)
	#Check(r)
	'''
	r = calcUZ(0.4, 1)
	Check(r)
	r1 = calcU15(r.U_R15, r.U)
	Check(r1)
	'''
	'''
	r1 = calcU15(complex(-0.20, +0.001), 1)
	print("U_Z=", r1.U_Z)
	print("Z=", r1.Z)
	#Check(r1)
	'''
	Z = complex(1, -0.1)
	U_R15 = calcZ(Z, 1)
	print("U_R15=",U_R15)
	r = calcU15(U_R15, 1)
	print("Z=", r.Z)
	print("Z=", r.Z)
	print("U_R15=", r.U_Z - r.U_R19)

if __name__ == "__main__":
	main()

