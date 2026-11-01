"""Three green cratered worlds; no rings. Exact two-color C64 cells."""
from pathlib import Path
import math,random
import numpy as np
from PIL import Image
from c64_art import hires

root=Path(__file__).resolve().parent.parent
p=root/'assets'/'runtime'
preview_dir=root/'docs'/'images'
p.mkdir(parents=True,exist_ok=True)
preview_dir.mkdir(parents=True,exist_ok=True)
data=bytearray();palettes=bytearray();previews=[]
for variant,(r,height) in enumerate([(19,40),(15,32),(12,32)]):
 rng=random.Random(32064+variant)
 surface=np.zeros((height,64,3),dtype=np.int32);cover=np.zeros((height,64),dtype=bool)
 craters=[(-.38,-.4,.25),(.34,-.12,.22),(-.4,.28,.2),(.12,.5,.17),(.56,.4,.13),(.08,-.68,.12)]
 craters += [(rng.uniform(-.75,.75),rng.uniform(-.75,.75),rng.uniform(.045,.095)) for _ in range(25)]
 for y in range(height):
  for x in range(64):
   nx=(x-32)/(r*1.1);ny=(y-height/2)/r;rr=nx*nx+ny*ny
   if rr>1:continue
   z=math.sqrt(1-rr);light=max(0,-nx*.48-ny*.55+z*.69)
   shade=.22+.78*light
   shade+=.06*math.sin(nx*14+math.sin(ny*9))+.045*math.sin(ny*22+nx*9)
   for cx,cy,rad in craters:
    d=math.hypot((nx-cx)/(rad*.95),(ny-cy)/(rad*.78))
    if d<.76:shade*=.21+.36*d
    elif d<1.18:
     facing=(-(nx-cx)*.6-(ny-cy)*.8)/rad
     shade+=facing*.5*(1-abs(d-.93)*2)
   shade=max(.03,min(1.12,shade))
   # Pale green sunlit rock and deep green crater floors.
   color=np.array([145,221,125])*shade
   color+=np.array([1,2,1])*rng.uniform(-9,9)
   surface[y,x]=np.clip(color,0,255);cover[y,x]=True
 for phase in range(8):
  canvas=np.zeros((height,80,3),dtype=np.int32);mask=np.zeros((height,80),dtype=bool)
  canvas[:,phase:phase+64]=surface;mask[:,phase:phase+64]=cover
  bits,colors,preview=hires(canvas,mask,[0,5,13])
  data.extend(bits);palettes.extend(colors)
  if not phase:previews.append(Image.fromarray(preview[:,:64]))
(p/'planet.bin').write_bytes(data)
(p/'planet-palette.bin').write_bytes(palettes)
gallery=Image.new('RGB',(64*3,40))
for i,im in enumerate(previews):gallery.paste(im,(i*64,40-im.height))
gallery.resize((768,160),Image.Resampling.NEAREST).save(preview_dir/'planet-reference-preview.png')
