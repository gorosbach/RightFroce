from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import numpy as np
from itertools import combinations
root=Path(__file__).resolve().parent.parent
runtime=root/'assets'/'runtime'
preview_dir=root/'docs'/'images'
runtime.mkdir(parents=True,exist_ok=True)
preview_dir.mkdir(parents=True,exist_ok=True)
im=Image.open(root/'assets'/'source-art'/'splash-art.png').convert('RGB').resize((320,200),Image.Resampling.LANCZOS)
d=ImageDraw.Draw(im)
font=ImageFont.truetype('C:/Windows/Fonts/arialbd.ttf',38)
title='RightForce';w=d.textbbox((0,0),title,font=font)[2]
d.text(((320-w)//2+2,5),title,font=font,fill=(45,70,150))
d.text(((320-w)//2,2),title,font=font,fill=(245,224,105))
for text,y,size in [('FIRE TO LAUNCH',178,12),('JOY 2  /  FIRE: LASER  /  SPACE: WARP',192,9)]:
 f=ImageFont.truetype('C:/Windows/Fonts/arialbd.ttf',size);w=d.textbbox((0,0),text,font=f)[2]
 d.text(((320-w)//2,y),text,font=f,fill='white')
im=im.resize((160,200),Image.Resampling.LANCZOS)
pal=np.array([(0,0,0),(255,255,255),(136,57,50),(103,182,189),(139,63,150),(85,160,73),(64,49,141),(191,206,114),(139,84,41),(87,66,0),(184,105,98),(80,80,80),(120,120,120),(148,224,137),(120,105,196),(159,159,159)],dtype=np.int32)
a=np.array(im,dtype=np.int32);bitmap=bytearray(8000);screen=bytearray(1000);color=bytearray(1000);out=np.zeros((200,160,3),dtype=np.uint8)
for cy in range(25):
 for cx in range(40):
  block=a[cy*8:cy*8+8,cx*4:cx*4+4];v=block.reshape(32,3)
  dist=((v[:,None,:]-pal[None,:,:])**2).sum(2)
  nearest=dist.argmin(1);candidates=np.argsort(np.bincount(nearest,minlength=16))[::-1]
  candidates=[int(c) for c in candidates if c][:6]
  opts=min(combinations(candidates,3),key=lambda cs:dist[:,[0,*cs]].min(1).sum())
  cols=[0,*opts];ids=dist[:,cols].argmin(1).reshape(8,4)
  cell=cy*40+cx;screen[cell]=(cols[1]<<4)|cols[2];color[cell]=cols[3]
  for r in range(8):
   bitmap[cell*8+r]=sum(int(ids[r,x])<<(6-2*x) for x in range(4))
  out[cy*8:cy*8+8,cx*4:cx*4+4]=pal[np.array(cols)[ids]]

# Draw the instructions AFTER quantization: hard-edged 5x7 pixels, no resampling.
glyphs={
'F':[31,16,16,30,16,16,16],'I':[14,4,4,4,4,4,14],'R':[30,17,17,30,20,18,17],
'E':[31,16,16,30,16,16,31],'T':[31,4,4,4,4,4,4],'O':[14,17,17,17,17,17,14],
'S':[15,16,16,14,1,1,30],'A':[14,17,17,31,17,17,17],'J':[7,2,2,2,18,18,12],
'Y':[17,17,10,4,4,4,4],'2':[14,17,1,2,4,8,31],'L':[16,16,16,16,16,16,31],
'P':[30,17,17,30,16,16,16],'C':[14,17,16,16,16,17,14],'W':[17,17,17,21,21,21,10],
' ':[0]*7,':':[0,4,4,0,4,4,0]}
for cy in range(21,25):
 for cx in range(40):
  cell=cy*40+cx;bitmap[cell*8:cell*8+8]=bytes(8);screen[cell]=0x10;color[cell]=1
out[168:]=0
for text,y in [('FIRE TO START',170),('JOY2 FIRE:LASER',180),('SPACE:WARP',190)]:
 x0=(160-(len(text)*6-1))//2
 for i,ch in enumerate(text):
  for dy,bits in enumerate(glyphs[ch]):
   for dx in range(5):
    if bits&(16>>dx):
     x=x0+i*6+dx;yy=y+dy;cell=(yy//8)*40+x//4
     bitmap[cell*8+(yy&7)]|=1<<(6-2*(x&3));out[yy,x]=pal[1]
(runtime/'splash.bin').write_bytes(bitmap+screen+color)
Image.fromarray(out).resize((640,400),Image.Resampling.NEAREST).save(preview_dir/'splash-c64.png')
