// Fast bitmap stars. Every star owns a distinct scanline, so erasing a
// byte cannot damage another star. $fb-$fe are reserved for this routine.
u8 starcol[12], starphase[12], stargroup[12];
u8 rowlo[12], rowhi[12], colorlo[12], colorhi[12];
u8 oldlo[12], oldhi[12], oldcol[12];
u8 speed[3];
u8 mask0[32], mask1[32], maskbase[12], starcolor[12];
u8 longstars,starFirst,starStride;
// One bounded pass: clear old bytes, move, then plot from lookup tables.
// No division, modulo, pixel loops or variable shifts in this hot path.
#pragma optimize(push)
#pragma optimize(noasm)
__asm starsStep {
 ldx starFirst
loop:
 ldy stargroup,x
 lda speed,y
 bne moving
 jmp next
moving:
 lda oldlo,x
 sta $fb
 lda oldhi,x
 sta $fc
 sta $fe
 sec
 sbc #$40
 sta $fe
 lda $fb
 sta $fd
 ldy #0
 lda ($fd),y
 sta ($fb),y
 lda oldcol,x
 cmp #39
 beq cleared
 ldy #8
 lda ($fd),y
 sta ($fb),y
cleared:
 ldy stargroup,x
 lda starphase,x
 sec
 sbc speed,y
 bcs phase
 adc #8
 dec starcol,x
 bpl phase
 pha
 lda #39
 sta starcol,x
 pla
phase:
 sta starphase,x
 ldy starcol,x
 sty $fd
 clc
 lda rowlo,x
 adc colofflo,y
 sta $fb
 sta oldlo,x
 lda rowhi,x
 adc coloffhi,y
 sta $fc
 sta oldhi,x
 lda $fb
 sta $f9
 lda $fc
 sec
 sbc #$40
 sta $fa
 lda $fd
 cmp oldcol,x
 php
 sta oldcol,x
 lda longstars
 beq normal
 lda #24
 bne mask
normal:
 lda maskbase,x
mask:
 clc
 adc starphase,x
 tay
 lda mask0,y
 sta $fe
 lda mask1,y
 pha
 ldy #0
 lda $fe
 ora ($f9),y
 sta ($fb),y
 pla
 ldy $fd
 cpy #39
 beq color
 ldy #8
 ora ($f9),y
 sta ($fb),y
color:
 plp
 beq next
 clc
 lda colorlo,x
 adc $fd
 sta $fb
 lda colorhi,x
 adc #0
 sta $fc
 clc
 lda $fb
 adc #<backColors
 sta $f9
 lda $fc
 and #3
 adc #>backColors
 sta $fa
 ldy #0
 lda ($f9),y
 bne firstcolor
 lda starcolor,x
firstcolor:
 sta ($fb),y
 ldy $fd
 cpy #39
 beq next
 ldy #1
 lda ($f9),y
 bne secondcolor
 lda starcolor,x
secondcolor:
 sta ($fb),y
next:
 lda starStride
 cmp #2
 bne singleStar
 dex
singleStar:
 dex
 bmi done
 jmp loop
done:
 rts
}


#pragma optimize(pop)
void starsInit(void) {
 u8 i,p,g,y,n; u16 a,b,m;
 for(i=0;i<40;i++) {colofflo[i]=(u16)i*8;coloffhi[i]=((u16)i*8)>>8;}
 for(g=0;g<4;g++) for(p=0;p<8;p++) {
  n=g==3?8:g+1;
  m=((u16)0xffff<<(16-n))>>p;
  mask0[g*8+p]=m>>8;mask1[g*8+p]=m;
 }
 for(i=0;i<12;i++) {
  // A permutation of 48 distinct rows, spread over the playfield.
  y=26+((u16)i*19%48)*3;
  a=0xe000+(u16)(y>>3)*320+(y&7);
  b=0xc000+(u16)(y>>3)*40;
  rowlo[i]=a;rowhi[i]=a>>8;colorlo[i]=b;colorhi[i]=b>>8;
  starcol[i]=random16()%40;starphase[i]=random16()&7;
  oldcol[i]=starcol[i];a+=(u16)starcol[i]*8;
  oldlo[i]=a;oldhi[i]=a>>8;
  g=i%3;stargroup[i]=g;maskbase[i]=g*8;
  starcolor[i]=g==0?0xb0:(g==1?0xc0:0x10);
  SCREEN[(u16)(y>>3)*40+starcol[i]]=starcolor[i];
  if(starcol[i]<39) SCREEN[(u16)(y>>3)*40+starcol[i]+1]=starcolor[i];
 }
}
void irqInit(void) {
 REG(0xdc0d)=127;REG(0xdd0d)=127;
 // Actual reads are required to release a pending CIA interrupt line.
 __asm { lda $dc0d
         lda $dd0d }
 *(void **)0xfffe=rasterIRQ;
 *(void **)0xfffa=nmiReturn;
 REG(0xd011)=0x3b;REG(0xd012)=250;
 REG(0xd019)=15;REG(0xd01a)=1;
 lastTick=ticks;
 __asm { cli }
}
void framewait(void) {
 u8 now;
 do {now=ticks;} while(now==lastTick);
 if((u8)(now-lastTick)>1) ++missedFrames;
 lastTick=now;
}
void starsUpdate(void) {
 longstars=warptime!=0;
 if(longstars) {starFirst=11;starStride=1;speed[0]=speed[1]=speed[2]=8;}
 else {starFirst=10+(frame&1);starStride=2;speed[0]=(frame&2)==0;speed[1]=2;speed[2]=(frame&2)==0?3:4;}
 __asm { jsr starsStep }
}
