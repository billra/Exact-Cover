#!/usr/bin/env python3
# Generate Y Pentomino Exact Cover Data
# Bill Ola Rasmussen
# version 0.1

# see, e.g. http://www.math.ucf.edu/~reid/Polyomino/y5_rect.html

class Board(object):
    'chess board'
    def __init__(self,w,h):
        'create a chess board of size w * h'
        self.w=w    # width
        self.h=h    # height
    def __str__(self):
        return 'board size: '+str(self.w)+' x '+str(self.h)
    #def xy_qr(self,x,y):
    #    'convert cartesian to diagonal coordinates'
    #    return x+self.n-1-y,x+y
    def constraintCounts(self):
        'number of primary and secondary constraints'
        return self.w*self.h,0
    #def constraints(self,x,y):
    #    'column constraints for queen at position x,y'
    #    r=[x,y+self.n] # row and column constraints
    #    d=self.xy_qr(x,y)
    #    if 0<d[0]<self.t-1:
    #        r.append(d[0]-1+self.pc)
    #    if 0<d[1]<self.t-1:
    #        r.append(d[1]-1+self.pc+self.dc)
    #    return r
    def constraints(self):
        for x in range(self.w):
            for y in range(self.h):
                for t in tileOrientations():
                    yield [2,3,4]
    
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
                yield zip(x,y)

def exactCoverSpec(w,h):
    b=Board(w,h)
    explain(b)
    print('[')
    print(' '.join(map(str,b.constraintCounts())))
    
    for a in tileOrientations():
        print(list(a))

    for z in b.constraints():
        print(' '.join(map(str,z)))

    #for x in range(n):
    #    for y in range(n):
    #        print(' '.join(map(str,b.constraints(x,y))))
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
