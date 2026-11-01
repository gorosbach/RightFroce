from pathlib import Path
import math,struct
script_dir=Path(__file__).resolve().parent
root=script_dir.parent
out=root/'assets'/'runtime'
music_dir=root/'assets'/'music'
out.mkdir(parents=True,exist_ok=True)
sid=(music_dir/'Lightforce_2006.sid').read_bytes()
off=struct.unpack('>H',sid[6:8])[0]
assert struct.unpack('<H',sid[off:off+2])[0]==0x1000
(out/'music.bin').write_bytes(sid[off+2:])
# Multicolor sprite pixels: white highlight, individual body, blue window.
rocket=['............']*5+['...11.......','...211......','...22211....','...222221...','..222322221.','222223322221','..222222221.','...222221...','...22211....','...211......','...11.......']+['............']*5
ufo=['............']*4+['....1111....','...122221...','...133331...','..12222221..','.1111111111.','122222222221','222222222222','122222222221','.1222222221.','...21.12....','...1...1....']+['............']*6
def enc(rows):
 b=bytearray(64)
 for y,row in enumerate(rows):
  assert len(row)==12,(y,row)
  for x,c in enumerate(row):
   if c!='.':b[y*3+x//4]|=int(c)<<(6-2*(x%4))
 return b
sprites=bytearray(448);sprites[:64]=enc(rocket);sprites[64:128]=enc(ufo)
sprites[128+27:128+30]=bytes([255]*3)
sprites[192+27]=sprites[192+30]=0xf0
for idx,flame in zip([4,5,6],['.2.','122','..2']):
 rows=rocket[:]
 rows[9]=flame+'222322221'
 rows[10]=('122' if idx==5 else '.22')+'223322221'
 rows[11]=flame+'222222221'
 sprites[idx*64:(idx+1)*64]=enc(rows)
(out/'sprites.bin').write_bytes(sprites)
# 64x32 half-lit Saturn; diagonal elliptical rings, stippled cloud bands.
pixels=set()
for y in range(32):
 for x in range(64):
  dx=x-32;dy=y-16
  r=dx*dx+dy*dy
  ring=(dx/31)**2+((dy+dx*.28)/5)**2
  disk=r<=14**2
  on=False
  if .70<ring<1.12 and (not disk or dy+dx*.28>1):on=(x+y)%3!=0
  if disk:
   light=dx+.42*dy<1
   if light:on=not ((y+int(dx*.18))%5==0 and x%3==0)
   else:on=r>=13.1**2 and dx<8
  if on:pixels.add((x,y))
data=bytearray(8*32*10)
for phase in range(8):
 for x,y in pixels:
  x+=phase
  data[phase*320+(y//8)*80+(x//8)*8+(y&7)]|=128>>(x&7)
(out/'planet.bin').write_bytes(data)

from pathlib import Path
import math
p=out;b=bytearray((p/'sprites.bin').read_bytes());b.extend(bytes(6*64))
def put(idx,x,y,v):
 if 0<=x<12 and 0<=y<21:b[idx*64+y*3+x//4]|=v<<(6-2*(x%4))
for f in range(4):
 for y in range(21):
  for x in range(12):
   dx=(x-5.5)*2;dy=y-10;r=(dx*dx+dy*dy)**.5
   if abs(r-(3+f*2))<2 or (r<8+f and (x*7+y*11+f)%9<2):put(7+f,x,y,1 if (x+y)%3==0 else 2)
# Two poses, each split into two expanded multicolor sprites.
b.extend(bytes(2*64))
for pose in range(2):
 for y in range(21):
  for x in range(24):
   dy=y-10;v=0
   if abs(dy)<=9:
    front=5-4*math.sqrt(max(0,1-(dy/9)**2))
    if front<=x<=10:
     v=1 if x<front+1.2 or abs(dy)>=8 else 2
     if 4<=x<=8 and abs(dy)<6:v=3 if (x+y)%4 else 2
     if x in (3,4) and y in (8,12):v=1
   if x>=10:
    for root in [3,7,12,17]:
     tentacle=root+math.sin((x-10)*.58+pose*1.3+root*.35)*(1+(x-10)/7)
     if abs(y-tentacle)<.7:v=3 if (x+root)%3 else 1
   if v:put(11+pose*2+x//12,x%12,y,v)
# Three asteroid depths and two rewards. All reuse hardware sprite channels.
b.extend(bytes(5*64))
for kind,r in enumerate([3.5,6.5,9.5]):
 for y in range(21):
  for x in range(12):
   dx=(x-5.5)*2;dy=y-10;angle=math.atan2(dy,dx)
   edge=r*(1+.10*math.sin(angle*5+kind)+.07*math.cos(angle*3))
   if dx*dx+dy*dy<edge*edge:
    v=1 if dx+dy< -r*.55 else 2
    if ((x-7)**2+(y-9)**2<5) or (x==4 and y in (13,14)):v=0
    if v:put(15+kind,x,y,v)
for y in range(4,18):
 for x in range(2,10):
  if y in (4,17) or x in (2,9):put(18,x,y,1)
for y in range(7,14):put(18,4,y,2)
for x in range(4,8):put(18,x,13,2)
heart=['.22..22.','22222222','22222222','.222222.','..2222..','...22...']
for y,row in enumerate(heart):
 for x,c in enumerate(row):
  if c!='.':put(19,x+2,y+7,1 if y==0 else 2)
(p/'sprites.bin').write_bytes(b)

import runpy
runpy.run_path(str(script_dir/"generate_planets.py"))

# Original Task Force payload; both tunes retain their native load address.
sid=(music_dir/'Task_Force.sid').read_bytes()
off=int.from_bytes(sid[6:8],'big')
assert int.from_bytes(sid[off:off+2],'little')==0x1000
(out/'title-music.bin').write_bytes(sid[off+2:])

runpy.run_path(str(script_dir/"generate_nebula.py"))

runpy.run_path(str(script_dir/"pack_planets.py"))

# Sector craft, Enterprise-D silhouette and a stationary three-line tribute.
b=bytearray((out/'sprites.bin').read_bytes());b.extend(bytes(12*64))
for y in range(21):
 for x in range(12):
  dy=abs(y-10)
  if dy<=4 and dy*2<=x<=10:
   put(20,x,y,1 if x==dy*2 or dy==4 else (3 if x>7 and dy<2 else 2))
  edge=5+int(abs(x-5.5)*.65)
  if edge<=y<=18-edge//2:
   put(21,x,y,1 if y==edge else (3 if 4<=x<=7 and y<11 else 2))
# Plan view: broad forward saucer, secondary hull, swept pylons, twin nacelles.
for y in range(21):
 for x in range(36):
  v=0
  if 1<=x<=14 and (2<=y<=4 or 16<=y<=18):v=1 if y in (2,18) else 3
  if 12<=x<=21 and (abs(y-(3+(x-12)*.7))<1 or abs(y-(17-(x-12)*.7))<1):v=2
  if ((x-18)/7)**2+((y-10)/3)**2<=1:v=1 if y in (7,13) else 2
  r=((x-27)/8)**2+((y-10)/7)**2
  if r<=1:
   v=1 if r>.72 else 2
   if .16<r<.24:v=3
   if (x,y) in [(27,9),(26,10),(27,10),(28,10),(27,11)]:v=1
   if y in (6,14) and x%3==0 and r<.7:v=3
  if v:put(22+x//12,x%12,y,v)
glyphs={
'A':[14,17,17,31,17,17,17],'C':[14,17,16,16,16,17,14],
'E':[31,16,16,30,16,16,31],'F':[31,16,16,30,16,16,16],
'H':[17,17,17,31,17,17,17],'I':[14,4,4,4,4,4,14],
'K':[17,18,20,24,20,18,17],'L':[16,16,16,16,16,16,31],
'N':[17,25,25,21,19,19,17],'O':[14,17,17,17,17,17,14],
'P':[30,17,17,30,16,16,16],'R':[30,17,17,30,20,18,17],
'S':[15,16,16,14,1,1,30],'T':[31,4,4,4,4,4,4],
'W':[17,17,17,21,21,21,10],' ':[0]*7,',':[0,0,0,0,0,4,8]}
for text,y0 in [('SPACE, THE FINAL FRONTIER',0),('SIR PATRICK STEWART',8)]:
 x0=(168-(len(text)*6-1))//2
 for i,ch in enumerate(text):
  for y,row in enumerate(glyphs[ch]):
   for x in range(5):
    if row&(16>>x):
     xx=x0+i*6+x;part=xx//24;col=xx%24
     b[(25+part)*64+(y0+y)*3+col//8]|=128>>(col&7)
assert len(b)==2048
(out/'sprites.bin').write_bytes(b)

# A short third line fits the seven existing caption sprites: five-pixel digits.
dates={'1':[4,12,4,4,14],'9':[14,17,15,1,14],'4':[18,18,31,2,2],
 '0':[14,17,17,17,14],'2':[14,17,2,4,31],'6':[14,16,30,17,14],'-':[0,0,31,0,0]}
text='1940-2026';x0=(168-(len(text)*6-1))//2
for i,ch in enumerate(text):
 for y,row in enumerate(dates[ch]):
  for x in range(5):
   if row&(16>>x):
    xx=x0+i*6+x;b[(25+xx//24)*64+(16+y)*3+(xx%24)//8]|=128>>(xx&7)
# Five-pixel red beam with a white core; a compact allied Sidewinder.
b.extend(bytes(2*64))
for y in (7,8,10,11):b[32*64+y*3:32*64+y*3+3]=bytes([170]*3)
b[32*64+27:32*64+30]=bytes([85]*3)
for y in range(7,14):
 for x in range(2,10):
  if abs(y-10)*2<=10-x:
   put(33,x,y,1 if x==9 or abs(y-10)*2==10-x else (3 if x==2 else 2))
assert len(b)==2176
(out/'sprites.bin').write_bytes(b)
