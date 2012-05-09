#
# python module for konoha python module test
#

# ------------------------------------------------------ #
# [module test] #

def printer(x):
	print x

# ------------------------------------------------------ #
# [return value test] #

def retInt():
	return 1

def retFloat():
	return 1.0

def retString():
	return 'hello, world.'

def retTrue():
	return True

def retFalse():
	return False

def retByteArray():
	return bytearray('hello, world.')

def retComp():
	return 1+2j

def retComp():
	return 1+2j

def retTuple():
	return (1, 2, "hello", 1.0, (1, 2))

def retList():
	return [1, 2, "hello", 1.0, (1, 2)]

def retDict():
	return {1:'hello', 2: 1.0, 'foo': 'bar'}

# ------------------------------------------------------ #
# [arguments test] #

def args1(a):
	print a
	return a

def args2(a, b):
	print a, b
	return a

def args3(a, b, c):
	print a, b, c
	return a

# ------------------------------------------------------ #
# [class test] #

class Person:
	def __init__(self, name, age):
		self.name = name;
		self.age = age;

	def getName(self):
		return self.name;

	def getAge(self):
		return self.age;

	def setName(self, name):
		self.name = name;

	def setAge(self, age):
		self.age = age;

	def hello(self):
		print self.name + "(" + str(self.age) + ") says hello!!"
