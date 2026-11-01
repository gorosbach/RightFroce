"""Exact VIC-II hires conversion: two colors in each 8x8 cell."""
from itertools import combinations
import numpy as np

PALETTE=np.array([
 (0,0,0),(255,255,255),(136,57,50),(103,182,189),
 (139,63,150),(85,160,73),(64,49,141),(191,206,114),
 (139,84,41),(87,66,0),(184,105,98),(80,80,80),
 (120,120,120),(148,224,137),(120,105,196),(159,159,159)
],dtype=np.int32)

def hires(rgb,coverage,allowed):
 height,width=coverage.shape
 bits=bytearray(width*height//8);colors=bytearray(width*height//64)
 preview=np.zeros((height,width,3),dtype=np.uint8)
 for cy in range(height//8):
  for cx in range(width//8):
   cell=cy*(width//8)+cx
   block=rgb[cy*8:cy*8+8,cx*8:cx*8+8].reshape(64,3)
   mask=coverage[cy*8:cy*8+8,cx*8:cx*8+8].reshape(64)
   if not mask.any():continue
   dist=((block[:,None,:]-PALETTE[None,:,:])**2).sum(2)
   choices=[pair for pair in combinations(allowed,2) if mask.all() or 0 in pair]
   lo,hi=min(choices,key=lambda pair:dist[:,pair].min(1).sum())
   if PALETTE[lo].sum()>PALETTE[hi].sum():lo,hi=hi,lo
   # Ordered shading retains texture and curved lighting at native resolution.
   delta=PALETTE[hi]-PALETTE[lo]
   factor=np.clip(((block-PALETTE[lo])*delta).sum(1)/max(1,(delta*delta).sum()),0,1).reshape(8,8)
   bayer=np.array([[0,8,2,10],[12,4,14,6],[3,11,1,9],[15,7,13,5]])
   threshold=(np.tile(bayer,(2,2))+.5)/16
   ids=factor>threshold
   colors[cell]=(hi<<4)|lo
   for y in range(8):bits[cell*8+y]=sum(int(ids[y,x])<<(7-x) for x in range(8))
   preview[cy*8:cy*8+8,cx*8:cx*8+8]=PALETTE[np.where(ids,hi,lo)]
 return bits,colors,preview
