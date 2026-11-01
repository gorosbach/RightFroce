// 6502 broad-phase for all five player projectiles. No zero-page scratch.
volatile u16 hitX;
volatile u8 hitY,hitWidth,hitBias,hitSpan,hitExclude,hitTargets;
u8 hitDelta;
#pragma code(lowcode)
#pragma optimize(push)
#pragma optimize(noasm)
__asm scanShotTargets {
 php
 pha
 txa
 pha
 tya
 pha
 lda #0
 sta hitTargets
 ldx #0
 ldy #0
HT_loop:
 lda enemy+6,x
 beq HT_next
 lda maskTable,y
 and hitExclude
 bne HT_next
 sec
 lda hitY
 sbc enemy+2,x
 clc
 adc hitBias
 cmp hitSpan
 bcs HT_next
 // Signed 16-bit X difference: -width < projectileX-targetX < 24.
 sec
 lda hitX
 sbc enemy,x
 sta hitDelta
 lda hitX+1
 sbc enemy+1,x
 beq HT_positive
 cmp #255
 bne HT_next
 clc
 lda hitDelta
 adc hitWidth
 bcc HT_next
 beq HT_next
 jmp HT_hit
HT_positive:
 lda hitDelta
 cmp #24
 bcs HT_next
HT_hit:
 lda hitTargets
 ora maskTable,y
 sta hitTargets
HT_next:
 txa
 clc
 adc #7
 tax
 iny
 cpy #6
 beq HT_done
 jmp HT_loop
HT_done:
 pla
 tay
 pla
 tax
 pla
 plp
 rts
}
#pragma optimize(pop)
#pragma code(code)
