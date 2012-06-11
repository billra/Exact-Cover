#!/usr/bin/env python
# boardPng
# Bill Ola Rasmussen
# version 1.0

import sys
import png # https://github.com/drj11/pypng
# unable to get 2to3 for pypng working, so env is 2.x for now

class BoardImage:
    def __init__(self,w,h,lineWidth,cellWidth):
        self.w,self.h=w,h
        self.lw,self.cw=lineWidth,cellWidth
        wBits,hBits=self.ToBitXy(w,h)
        self.bits=[z[:] for z in [[0]*wBits]*hBits] # copies, not references
    def ToBitXy(self,x,y):
        u=self.lw+self.cw
        return x*u+self.lw,y*u+self.lw
    def ToIdx(self,x,y):
        return x+y*self.w
    def ToXy(self,i):
        return i%self.w,i//self.w
    def Cell(self,xb,yb): # bit coordinate system
        for ix in range(xb,xb+self.cw):
            for iy in range(yb,yb+self.cw):
                self.bits[iy][ix]=1
    def TilePart(self,i,ids):
        'partial tile: draw cell and break wall'
        x,y=self.ToXy(i)
        xb,yb=self.ToBitXy(x,y)
        self.Cell(xb,yb)
        if x<self.w-1 and self.ToIdx(x+1,y)in ids:
            self.Cell(xb+self.cw/2,yb) # break E wall
        if y<self.h-1 and self.ToIdx(x,y+1) in ids:
            self.Cell(xb,yb+self.cw/2) # break S wall
    def Write(self,fileName):
        f=open(fileName,'wb')
        palette=[(0xF0,0xF0,0xF0),(0x00,0x00,0xF0)]
        w=png.Writer(len(self.bits[0]),len(self.bits),palette=palette,bitdepth=1)
        w.write(f,self.bits)
        f.close()
    
def addTile(image,ids):
    for i in ids:
        assert -1<i<image.w*image.h
        image.TilePart(i,ids)

def parseInput(image):
    parse=False
    for line in sys.stdin:
        words=line.split()
        if not len(words):
            continue
        if ']'==words[0]:
            break
        if '['==words[0]:
            parse=True
            continue
        if parse:
            ids=[int(a) for a in words]
            addTile(image,ids)

def main(w,h):
    print('boardPng')
    image=BoardImage(w,h,3,20)
    parseInput(image)
    image.Write('out.png')
    print('done.')

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 3:
        main(int(sys.argv[1]),int(sys.argv[2]))
    else:
        print('usage: '+sys.argv[0]+' [width] [height]')
