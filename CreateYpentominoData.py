#!/usr/bin/env python3
# Generate Y Pentomino Exact Cover Data
# Bill Ola Rasmussen
# version 1.0

# see, e.g. http://www.math.ucf.edu/~reid/Polyomino/y5_rect.html

class Board(object):
    'chess board'
    def __init__(self,w,h):
        'create a chess board of size w * h'
        self.w=w    # width
        self.h=h    # height
    def __str__(self):
        return 'board size: '+str(self.w)+' x '+str(self.h)
    def constraintCounts(self):
        'number of primary and secondary constraints'
        return self.w*self.h,0
    def validPlacement(self,x,y,t):
        return all(-1<i[0]+x<self.w and -1<i[1]+y<self.h for i in t)
    def makeConstraint(self,x,y,t):
        return [i[0]+x + self.w*(i[1]+y) for i in t]
    def constraints(self):
        for x in range(self.w):
            for y in range(self.h):
                for t in tileOrientations():
                    if self.validPlacement(x,y,t):
                        yield self.makeConstraint(x,y,t)

def tileOrientations():
    'generate the 8 unique Y pentomino orientations'
    x=[0,1,2,3,1]
    y=[0,0,0,0,1]
    for sw in range(2):
        x,y=y,x # swap axes
        for xs in range(2):
            x=[-n for n in x] # reflect across Y axis
            for ys in range(2):
                y=[-n for n in y] # reflect across X axis
                yield list(zip(x,y))

def explain(b):
    'describe file data'
    s='A generalized exact cover specification for the Y Pentomino cover problem.\n'\
      +str(b)+'\n'\
      'First digit: number of primary constraints "exactly one".\n'\
      'Second digit: number of secondary constraints "at most one".\n'\
      'Each line after the first two digits describes a unique Y Pentomino position.\n'\
      'For more information, see\n'\
      'http://www.math.ucf.edu/~reid/Polyomino/y5_rect.html'
    print(s)

def exactCoverSpec(w,h):
    b=Board(w,h)
    explain(b)
    print('[')
    print(' '.join(map(str,b.constraintCounts())))
    for z in b.constraints():
        print(' '.join(map(str,z)))
    print(']')

def runTests():
    print('usage: '+sys.argv[0]+' [board size] or')
    print('       '+sys.argv[0]+' [width] [height]')
    import platform
    print('python version '+platform.python_version())
    #print('running unit tests...')
    #import doctest
    #doctest.testfile('CreateYpentominoTests.txt')
    print('done.')

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 2:
        exactCoverSpec(int(sys.argv[1]),int(sys.argv[1])) # square
    elif len(sys.argv) == 3:
        exactCoverSpec(int(sys.argv[1]),int(sys.argv[2]))
    else:
        runTests()
