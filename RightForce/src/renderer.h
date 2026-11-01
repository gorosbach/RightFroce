// 22 virtual sprites, two command banks, eight physical VIC-II channels.
#define VS_MAX 22
volatile u8 queueID;u8 vObject[22],rObject[44];
u8 lastObjectX[31],lastObjectHi[31],lastObjectValid[31];
u8 tweenFrame,actualMSB,drawLo,drawHi,drawY,targetLo,targetHi,drawSlot,drawID,drawBit,tweenDelta;
u8 vxLo[VS_MAX],vxHi[VS_MAX];
u8 vy[VS_MAX],vi[VS_MAX],vc[VS_MAX],vf[VS_MAX],vp[VS_MAX],order[VS_MAX],accepted[VS_MAX],vcount;
u8 rx[44],ry[44],ri[44],rc[44],rs[44],rm[44],re[44],rxc[44],ryc[44],rmu[44],rline[44];
volatile u8 spriteReady,readyBase,readyCount,irqBase,irqCount;
u8 irqIndex,irqLimit,irqFirst,irqPhase,irqSame;
volatile u8 ticks;
u8 lastTick;u16 missedFrames,spriteDrops;
volatile u8 emitIndex,emitObject,emitSlot,hwMSB,hwEnable,hwXX,hwYY,hwMulti;
u8 emitMask,emitInverse,emitFlags;
const u8 flagXX[64]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128};
const u8 flagYY[64]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128};
const u8 flagMulti[64]={0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128,0,0,0,0,0,0,0,0,1,2,4,8,16,32,64,128};

const u8 inverseMasks[8]={254,253,251,247,239,223,191,127};
u8 freeAt[8],owner[8],maskTable[8]={1,2,4,8,16,32,64,128};

#pragma optimize(push)
#pragma optimize(noasm)
__asm emitSprite {
 ldx emitSlot
 lda maskTable,x
 sta emitMask
 lda inverseMasks,x
 sta emitInverse
 ldy emitObject
 lda vf,y
 sta emitFlags
 lda hwMSB
 and emitInverse
 sta hwMSB
 lda vxHi,y
 beq nohigh
 lda hwMSB
 ora emitMask
 sta hwMSB
nohigh:
 lda emitFlags
 asl
 asl
 asl
 ora emitSlot
 tax
 lda hwXX
 and emitInverse
 ora flagXX,x
 sta hwXX
 lda hwYY
 and emitInverse
 ora flagYY,x
 sta hwYY
 lda hwMulti
 and emitInverse
 ora flagMulti,x
 sta hwMulti
 lda hwEnable
 ora emitMask
 sta hwEnable
 ldx emitIndex
 lda vxLo,y
 sta rx,x
 lda vy,y
 sta ry,x
 lda vi,y
 cmp #41
 bcc E_normalLead
 cmp #48
 bcs E_normalLead
 // Seven caption pieces can reuse three channels on the same line. Allow
 // the complete IRQ batch to finish before their common Y comparator.
 lda vy,y
 sec
 sbc #24
 jmp E_storeLine
E_normalLead:
 lda vy,y
 sec
 sbc #8
E_storeLine:
 sta rline,x
 lda vObject,y
 sta rObject,x
 lda vi,y
 sta ri,x
 lda vc,y
 sta rc,x
 lda emitSlot
 sta rs,x
 lda hwMSB
 sta rm,x
 lda hwEnable
 sta re,x
 lda hwXX
 sta rxc,x
 lda hwYY
 sta ryc,x
 lda hwMulti
 sta rmu,x
 rts
}
// Interpolate X only: Y stays identical to the sorted raster schedule,
// so a reused channel can never be moved into its previous sprite.
__asm applySprite {
 lda tweenFrame
 bne T_interpolate
 lda rs,x
 tay
 asl
 tay
 lda rx,x
 sta $d000,y
 lda ry,x
 sta $d001,y
 lda rs,x
 tay
 lda ri,x
 sta $c3f8,y
 lda rc,x
 sta $d027,y
 lda rm,x
 sta actualMSB
 sta $d010
 lda rxc,x
 sta $d01d
 lda ryc,x
 sta $d017
 lda rmu,x
 sta $d01c
 lda re,x
 sta $d015
 rts
T_interpolate:
 lda rs,x
 sta drawSlot
 tay
 lda maskTable,y
 sta drawBit
 lda rm,x
 and drawBit
 beq T_low
 lda #1
T_low:
 sta targetHi
 sta drawHi
 lda rx,x
 sta targetLo
 sta drawLo
 lda ry,x
 sta drawY
 lda rObject,x
 sta drawID
 tay
 lda tweenFrame
 beq T_store
 lda lastObjectValid,y
 beq T_store
 sec
 lda targetLo
 sbc lastObjectX,y
 sta tweenDelta
 lda targetHi
 sbc lastObjectHi,y
 beq T_positive
 cmp #255
 bne T_store
 lda tweenDelta
 cmp #224
 bcc T_store
 bcs T_midpoint
T_positive:
 lda tweenDelta
 cmp #33
 bcs T_store
T_midpoint:
 clc
 lda targetLo
 adc lastObjectX,y
 sta drawLo
 lda targetHi
 adc lastObjectHi,y
 lsr
 sta drawHi
 ror drawLo
T_store:
 lda targetLo
 sta lastObjectX,y
 lda targetHi
 sta lastObjectHi,y
 lda #1
 sta lastObjectValid,y
 lda drawSlot
 asl
 tay
 lda drawLo
 sta $d000,y
 lda drawY
 sta $d001,y
 lda drawSlot
 tay
 lda ri,x
 sta $c3f8,y
 lda rc,x
 sta $d027,y
 lda actualMSB
 and inverseMasks,y
 sta actualMSB
 lda drawHi
 beq T_msbready
 lda actualMSB
 ora drawBit
 sta actualMSB
T_msbready:
 lda actualMSB
 sta $d010
 lda rxc,x
 sta $d01d
 lda ryc,x
 sta $d017
 lda rmu,x
 sta $d01c
 lda re,x
 sta $d015
 rts
}
__asm rasterIRQ {
 pha
 txa
 pha
 tya
 pha
 lda #1
 sta $d019
 lda irqPhase
 beq frameIRQ
 jmp events
frameIRQ:
 lda #0
 sta tweenFrame
 sta actualMSB
 lda spriteReady
 beq oldframe
 lda #1
 sta tweenFrame
 lda readyBase
 sta irqBase
 lda readyCount
 sta irqCount
 lda #0
 sta spriteReady
oldframe:
 lda #0
 sta $d015
 lda irqBase
 sta irqIndex
 clc
 adc irqCount
 sta irqLimit
 lda irqCount
 cmp #8
 bcc firstcount
 lda #8
firstcount:
 clc
 adc irqBase
 sta irqFirst
firstloop:
 ldx irqIndex
 cpx irqFirst
 beq musicframe
 jsr applySprite
 inc irqIndex
 jmp firstloop
musicframe:
 jsr musicPlay
 jsr soundPlay
 inc ticks
 // Real PAL seconds, independent of occasional delays in the main loop.
 lda mode
 cmp #1
 bne clockDone
 lda boss
 ora bossDying
 bne clockDone
 lda waveSeconds
 cmp #180
 bcs clockDone
 inc waveSubTicks
 lda waveSubTicks
 cmp #50
 bne clockDone
 lda #0
 sta waveSubTicks
 inc waveSeconds
 lda #1
 sta hudDirty
clockDone:
 jmp schedule
events:
 ldx irqIndex
 cpx irqLimit
 beq schedule
 lda rline,x
 sta irqSame
 jsr applySprite
 inc irqIndex
 ldx irqIndex
 cpx irqLimit
 beq schedule
 lda rline,x
 cmp irqSame
 beq events
 // Catch close events while already inside the handler.
 cmp $d012
 bcc events
 beq events
schedule:
 ldx irqIndex
 cpx irqLimit
 bcc more
 lda #0
 sta irqPhase
 lda #250
 bne rasterset
more:
 lda #1
 sta irqPhase
 lda rline,x
rasterset:
 sta $d012
 pla
 tay
 pla
 tax
 pla
 rti
}
__asm nmiReturn { rti }
#pragma optimize(pop)

void spritesOff(void) {
 memset(lastObjectValid,0,31);vcount=0;spriteReady=0;irqCount=0;irqPhase=0;irqIndex=irqLimit=0;
 REG(0xd015)=0;REG(0xd012)=250;
}
void spriteBegin(void) {vcount=0;}
// flags: 1 multicolor, 2 double width, 4 double height.
// Values shared with hand-written ASM must remain volatile; otherwise Oscar64
// may move identical flag/show assignments past an entire sprite loop.
volatile u16 queueX;
volatile u8 queueY,queueShow,queueImage,queueColor,queueFlags,queuePriority;
volatile u8 queueResult;
#pragma optimize(push)
#pragma optimize(noasm)
__asm queueSprite {
 pha
 txa
 pha
 tya
 pha
 lda queueShow
 beq Q_bad
 ldx vcount
 cpx #22
 beq Q_bad
 lda queueY
 cmp #200
 bcs Q_bad
 clc
 adc #50
 sta vy,x
 clc
 lda queueX
 adc #24
 sta vxLo,x
 lda queueX+1
 adc #0
 sta vxHi,x
 cmp #2
 bcs Q_bad
 cmp #1
 bne Q_inrange
 lda vxLo,x
 cmp #88
 bcs Q_bad
Q_inrange:
 lda queueID
 sta vObject,x
 lda queueImage
 sta vi,x
 lda queueColor
 sta vc,x
 lda queueFlags
 sta vf,x
 lda queuePriority
 sta vp,x
 lda #1
 sta accepted,x
 stx queueResult
 inc vcount
 jmp Q_exit
Q_bad:
 lda #255
 sta queueResult
Q_exit:
 pla
 tay
 pla
 tax
 pla
 rts
}
#pragma optimize(pop)
__forceinline u8 spriteAdd(u8 show,int x,u8 y,u8 image,u8 color,u8 flags,u8 priority) {
 queueShow=show;queueX=x;queueY=y;queueImage=image;queueColor=color;queueFlags=flags;queuePriority=priority;
 __asm { jsr queueSprite }
 return queueResult;
}
u8 buildSearchY,buildI,buildJ,buildObject,buildY,buildCount,buildBase,buildSlot,buildBest,buildVictim;
#pragma optimize(push)
#pragma optimize(noasm)
__asm buildSprites {
 lda #0
 sta buildI
S_sort:
 ldx buildI
 cpx vcount
 beq S_sorted
 stx buildObject
 stx buildJ
 lda vy,x
 sta buildY
S_insert:
 ldy buildJ
 beq S_place
 lda order-1,y
 tax
 lda vy,x
 cmp buildY
 bcc S_place
 beq S_place
 txa
 sta order,y
 dec buildJ
 jmp S_insert
S_place:
 lda buildObject
 sta order,y
 inc buildI
 jmp S_sort
S_sorted:
 lda irqBase
 beq S_bank1
 lda #0
 beq S_bankset
S_bank1:
 lda #22
S_bankset:
 sta buildBase
S_again:
 lda #0
 sta hwMSB
 sta hwEnable
 sta hwXX
 sta hwYY
 sta hwMulti
 sta buildCount
 sta buildI
 ldx #7
S_clear:
 sta freeAt,x
 dex
 bpl S_clear
 lda #255
 ldx #7
S_clearowner:
 sta owner,x
 dex
 bpl S_clearowner
S_object:
 ldx buildI
 cpx vcount
 bcc S_get
 jmp S_finished
S_get:
 lda order,x
 sta buildObject
 tax
 lda accepted,x
 bne S_visible
 jmp S_next
S_visible:
 lda vy,x
 sta buildY
 sta buildSearchY
 lda vi,x
 cmp #41
 bcc S_searchReady
 cmp #48
 bcs S_searchReady
 lda buildY
 sec
 sbc #16
 sta buildSearchY
S_searchReady:
 lda buildCount
 cmp #8
 bcs S_reuse
 sta buildSlot
 jmp S_allocate
S_reuse:
 lda #255
 sta buildSlot
 sta buildBest
 ldx #7
S_find:
 lda freeAt,x
 cmp buildSearchY
 bcc S_available
 beq S_available
 jmp S_findnext
S_available:
 cmp buildBest
 bcs S_findnext
 sta buildBest
 stx buildSlot
S_findnext:
 dex
 bpl S_find
 lda buildSlot
 cmp #255
 bne S_allocate
 // Drop the least important overlapping object, then rebuild the masks.
 lda buildObject
 sta buildVictim
 ldx #7
S_victim:
 lda owner,x
 cmp #255
 beq S_victimnext
 tay
 lda vp,y
 ldy buildVictim
 cmp vp,y
 bcc S_victimnext
 beq S_victimnext
 lda owner,x
 sta buildVictim
S_victimnext:
 dex
 bpl S_victim
 ldx buildVictim
 lda #0
 sta accepted,x
 inc spriteDrops
 bne S_dropdone
 inc spriteDrops+1
S_dropdone:
 cpx buildObject
 beq S_next
 jmp S_again
S_allocate:
 ldx buildSlot
 lda buildObject
 sta owner,x
 tay
 lda vf,y
 and #4
 beq S_short
 lda #50
 bne S_lifetime
S_short:
 lda #29
S_lifetime:
 clc
 adc buildY
 bcc S_lifetimeok
 lda #255
S_lifetimeok:
 sta freeAt,x
 stx emitSlot
 lda buildObject
 sta emitObject
 clc
 lda buildBase
 adc buildCount
 sta emitIndex
 jsr emitSprite
 inc buildCount
S_next:
 inc buildI
 jmp S_object
S_finished:
 lda buildBase
 sta readyBase
 lda buildCount
 sta readyCount
 lda #1
 sta spriteReady
 rts
}
#pragma optimize(pop)
void spritePublish(void) { __asm { jsr buildSprites } }


#define SPR(id,show,x,y,image,color,flags,priority) (queueID=(id),spriteAdd(show,x,y,image,color,flags,priority))
