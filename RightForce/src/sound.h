volatile u8 sfxRequest,sfxKind,sfxTime;
#pragma optimize(push)
#pragma optimize(noasm)
__asm soundPlay {
 lda sfxRequest
 beq running
 sta sfxKind
 tax
 lda #0
 sta sfxRequest
 sta $d412
 lda #$08
 sta $d413
 lda #$88
 sta $d414
 lda #12
 cpx #2
 bne notwarp
 lda #60
notwarp:
 cpx #3
 bne length
 lda #28
length:
 sta sfxTime
running:
 lda sfxTime
 beq done
 dec sfxTime
 lda sfxKind
 cmp #4
 beq pickupTone
 cmp #3
 beq noise
 cmp #2
 beq warpTone
 lda sfxTime
 asl
 asl
 adc #12
 sta $d40f
 lda #$21
 bne control
warpTone:
 lda sfxTime
 eor #63
 asl
 sta $d40e
 lda sfxTime
 adc #4
 sta $d40f
 lda #$11
 bne control
pickupTone:
 lda sfxTime
 eor #15
 asl
 adc #20
 sta $d40f
 lda #$11
 bne control
noise:
 lda sfxTime
 lsr
 adc #2
 sta $d40f
 lda #$81
control:
 sta $d412
 lda sfxTime
 bne done
 lda #0
 sta sfxKind
 sta $d412
done:
 rts
}
#pragma optimize(pop)
void sound(u8 kind) {if(kind>=sfxKind)sfxRequest=kind;}


