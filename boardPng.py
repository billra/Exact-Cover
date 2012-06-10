#!/usr/bin/env python
# boardPng
# Bill Ola Rasmussen
# version 0.1

import sys
import png # https://github.com/drj11/pypng
# unable to get 2to3 working, so env is 2.x for now

lineWidth=3
cellWidth=20
image=[]

def pixel(x,y,s,xo,yo):
    global image
    for ix in range(x+xo,x+s+xo):
        for iy in range(y+yo,y+s+yo):
            image[iy][ix]=1

def addPoint(w,h,i,ids):
    x,y=i%w,i//w
    xb=x*(lineWidth+cellWidth)+lineWidth
    yb=y*(lineWidth+cellWidth)+lineWidth
    pixel(xb,yb,cellWidth,0,0)
    if x<w-1 and x+1+y*w in ids:
        pixel(xb,yb,cellWidth,cellWidth/2,0)
    if y<h-1 and x+(y+1)*w in ids:
        pixel(xb,yb,cellWidth,0,cellWidth/2)
        

def addTile(w,h,ids):
    for i in ids:
        assert -1<i<w*h
        addPoint(w,h,i,ids)

def parseInput(w,h):
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
            addTile(w,h,ids)

def writeImage():
    f=open('out.png','wb')
    palette=[(0xF0,0xF0,0xF0),(0x00,0x00,0xF0)]
    print len(image)
    print len(image[0]), len(image)
    w=png.Writer(len(image[0]),len(image),palette=palette,bitdepth=1)
    w.write(f,image)
    f.close()

def initImage(w,h):
    global image
    width=w*(lineWidth+cellWidth)+lineWidth
    height=h*(lineWidth+cellWidth)+lineWidth
    image=[z[:] for z in [[0]*width]*height] # copies, not references

def main(w,h):
    print('boardPng')
    initImage(w,h)
    parseInput(w,h)
    writeImage()
    print('done.')

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 3:
        main(int(sys.argv[1]),int(sys.argv[2]))
    else:
        print('usage: '+sys.argv[0]+' [width] [height]')
