"""A tilted spiral galaxy, inspired by the user's bright-core reference."""
from pathlib import Path
import math
import numpy as np
from PIL import Image
from c64_art import hires

root=Path(__file__).resolve().parent.parent
p=root/'assets'/'runtime'
preview_dir=root/'docs'/'images'
p.mkdir(parents=True,exist_ok=True)
preview_dir.mkdir(parents=True,exist_ok=True)
rgb=np.zeros((64,128,3),dtype=np.int32);cover=np.zeros((64,128),dtype=bool)
for y in range(64):
 for x in range(128):
  dx=x-79;dy=y-25+dx*.12
  u=dx/47;v=dy/13
  r=math.hypot(u,v);theta=math.atan2(v,u)
  # Logarithmic arms leave broad black lanes between the pink/blue ribbons.
  phase=theta-2.6*math.log(r+.15)
  crest=(.5+.5*math.cos(phase*3))**7
  rim=math.exp(-((r-1.0)/.5)**2)
  haze=math.exp(-(r/1.34)**3)
  arm=crest*rim
  fine=(math.sin(x*2.37+y*3.13)+math.sin(x*.43-y*.91))*.045
  intensity=max(0,.11*haze+.84*arm+fine*haze)
  if r>1.6:intensity=0
  core=(dx/18)**2+(dy/6.8)**2
  if core<1.45:
   corelight=max(0,1-core/1.45)
   if corelight>.65:color=np.array([255,255,255])
   elif corelight>.45:color=np.array([255,255,205])*(.88+.12*corelight)
   else:color=np.array([208,133,146])*(.5+corelight)
   intensity=max(intensity,corelight)
  elif intensity>.4:
   color=np.array([184,105,151])*(.55+intensity*.55)
  else:
   color=np.array([91,60,169])*min(1,intensity*2.8)
  if intensity>.12:
   cover[y,x]=True;rgb[y,x]=np.clip(color,0,255)
bits,colors,preview=hires(rgb,cover,[0,6,4,14,10,7,1])
(p/'nebula.bin').write_bytes(bits+colors)
Image.fromarray(preview).resize((640,320),Image.Resampling.NEAREST).save(preview_dir/'nebula-reference-preview.png')
