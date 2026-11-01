// SID payload keeps its original load/init/play addresses.
#pragma region(main, 0x2400, 0xa000, , , {code,data,bss,heap,stack})
#pragma section(lowcode, 0)
#pragma region(lowcode, 0x0880, 0x1000, , , {lowcode})
#pragma section(music, 0)
#pragma region(music, 0x1000, 0x2130, , , {music})
#pragma data(music)
const u8 music[4400]={
 #embed "assets/runtime/title-music.bin"
};
#pragma data(data)
const u8 gameMusic[779]={
 #embed "assets/runtime/music.bin"
};
const char spriteArt[]={
 #embed 2176 0 lzo "assets/runtime/sprites.bin"
};
const u8 planetPacked[]={
 #embed "assets/runtime/planet-packed.bin"
};
const u8 planetIndex[384]={
 #embed "assets/runtime/planet-index.bin"
};
const u8 planetPalette[1040]={
 #embed "assets/runtime/planet-palette.bin"
};
u8 musicZP[12],gameZP[12];
#pragma optimize(push)
#pragma optimize(noasm)
__asm musicInit {
 ldx #11
save:
 lda $84,x
 sta gameZP,x
 dex
 bpl save
 lda #0
 tax
 tay
 jsr music
 ldx #11
restore:
 lda $84,x
 sta musicZP,x
 lda gameZP,x
 sta $84,x
 dex
 bpl restore
 rts
}
__asm musicPlay {
 lda mode
 bne ingame
 jsr music+3
 rts
ingame:
 ldx #11
swapin:
 lda $84,x
 sta gameZP,x
 lda musicZP,x
 sta $84,x
 dex
 bpl swapin
 jsr music+51
 ldx #11
swapout:
 lda $84,x
 sta musicZP,x
 lda gameZP,x
 sta $84,x
 dex
 bpl swapout
 rts
}
#pragma optimize(pop)
#include "background.h"
