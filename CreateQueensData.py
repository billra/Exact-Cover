#!/usr/bin/env python3
# Generate Queens Exact Cover Data
# Bill Ola Rasmussen
# version 1.0

# Description extracted from http://en.wikipedia.org/wiki/Exact_cover
#	"The N queens problem is an example of a generalized exact cover problem.
#	The problem involves four kinds of constraints:
#		Rank: For each of the N ranks, there must be exactly one queen.
#		File: For each of the N files, there must be exactly one queen.
#		Diagonals: For each of the 2N − 1 diagonals, there must be at most one queen.
#		Reverse diagonals: For each of the 2N − 1 reverse diagonals, there must be 
#			at most one queen.
#	Note that the 2N rank and file constraints form the primary constraints, while
#	the 4N − 2 diagonal and reverse diagonals form the secondary constraints. Further,
#	because the each of first and last diagonal and reverse diagonals involve only
#	one square on the chessboard, these can be omitted and thus we can reduce the
#	number of secondary constraints to 4N − 6. The matrix for the N queens problem
#	then have N^2 rows and 6N − 6 columns, each row for a possible queen placement on
#	each square on the chessboard, and each column for each constraint."
#
# Creating an exact cover representation for solving is one of many ways
# to solve the N queens problem. A good link for N queens is:
# http://queens.inf.tu-dresden.de/?n=f&l=en

class Board(object):
	'chess board'
	def __init__(self,n):
		'create a chess board of size n'
		self.n=n		# number of rows (also number of columns)
		self.t=2*n-1	# ... diagonals (single direction)
		self.pc=2*n		# ... primary constraints (rows + columns)
		self.dc=2*n-3	# ... (secondary) constraints for a single diagonal dimension
	def __str__(self):
		return 'board size: '+str(self.n)+' diagonals: '+str(self.t)
	def xy_qr(self,x,y):
		'convert cartesian to diagonal coordinates'
		return x+self.n-1-y,x+y
	def constraintCounts(self):
		'number of primary and secondary constraints'
		return self.pc,self.dc*2
	def constraints(self,x,y):
		'column constraints for queen at position x,y'
		r=[x,y+self.n] # row and column constraints
		d=self.xy_qr(x,y)
		if 0<d[0]<self.t-1:
			r.append(d[0]-1+self.pc)
		if 0<d[1]<self.t-1:
			r.append(d[1]-1+self.pc+self.dc)
		return r
	
def showCoordConvert(n):
	'enumerate all coordinate conversions'
	b=Board(n)
	print(b)
	for x in range(n):
		for y in range(n):
			q,r=b.xy_qr(x,y)
			print(str(x)+','+str(y),'->',str(q)+','+str(r))

def explain(b):
	'describe file data'
	s='A generalized exact cover specification for the n-Queens problem.\n'\
	  +str(b)+'\n'\
	  'First digit: number of primary constraints "exactly one".\n'\
	  'Second digit: number of secondary constraints "at most one".\n'\
	  'Each line after the first two digits describes a unique queen position.\n'\
	  'For more information, see\n'\
	  'http://en.wikipedia.org/wiki/Exact_cover#N_queens_problem'
	print(s)

def exactCoverSpec(n):
	b=Board(n)
	explain(b)
	print('[')
	print(' '.join(map(str,b.constraintCounts())))
	for x in range(n):
		for y in range(n):
			print(' '.join(map(str,b.constraints(x,y))))
	print(']')
	
def runTests():
	print('usage: '+sys.argv[0]+' [board size]')
	import platform
	print('python version '+platform.python_version())
	print('running unit tests...')
	import doctest
	doctest.testfile('CreateQueensTests.txt')
	print('done.')

if __name__ == "__main__":
	import sys
	if len(sys.argv) == 2:
		exactCoverSpec(int(sys.argv[1]))
	else:
		runTests()

