"""Pack each frame's bounded 4/5-row planet chunk; runtime uses a 50-byte buffer."""
from pathlib import Path
p=Path(__file__).resolve().parent.parent/'assets'/'runtime'
data=(p/'planet.bin').read_bytes();packed=bytearray();index=bytearray()
for base,height in [(0,40),(3200,32),(5760,32)]:
 for phase in range(8):
  for chunk in range(8):
   index.extend(len(packed).to_bytes(2,'little'))
   raw=bytearray()
   for y in range(chunk*(height//8),(chunk+1)*(height//8)):
    for x in range(10):raw.append(data[base+phase*height*10+(y//8)*80+x*8+(y&7)])
   pos=0
   while pos<len(raw):
    end=pos+1;zero=raw[pos]==0
    while end<len(raw) and (raw[end]==0)==zero and end-pos<128:end+=1
    packed.append((end-pos-1)|(0 if zero else 128))
    if not zero:packed.extend(raw[pos:end])
    pos=end
(p/'planet-packed.bin').write_bytes(packed)
(p/'planet-index.bin').write_bytes(index)
