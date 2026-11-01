#include <oscar.h>
#pragma code(lowcode)
// The original title tune is kept beneath I/O while the game tune runs.
// $cd00-$de2f is outside the screen, sprite set and bitmap allocations.
void backupTitleMusic(void) {
 *((volatile __memmap u8 *)1)=0x34;
 memcpy((void *)0xcd00,(void *)0x1000,4400);
 *((volatile __memmap u8 *)1)=0x35;
}
const char splashBits[]={
 #embed 8000 0 lzo "assets/runtime/splash.bin"
};
const char splashScreen[]={
 #embed 1000 8000 lzo "assets/runtime/splash.bin"
};
const char splashColors[]={
 #embed 1000 9000 lzo "assets/runtime/splash.bin"
};
void titleScreen(void) {
 REG(0xd015)=0;REG(0xd016)=0x18;REG(0xd020)=0;
 oscar_expand_lzo((char *)BMP,splashBits);oscar_expand_lzo((char *)SCREEN,splashScreen);
 oscar_expand_lzo_buf((char *)0xd800,splashColors);mode=0;
}
void startGame(void) {
 u8 i;
 __asm { sei }
 spritesOff();
 if(!mode) {memcpy((void *)0x1000,gameMusic,779);
  __asm { jsr musicInit }
 }
 REG(0xd016)=8;REG(0xd020)=0;
 memset(BMP,0,8000);memset(SCREEN,16,1000);
 starsInit();planetInit();reset();shownLevel=shownLives=shownWarp=shownClock=255;shownKills=65535;
 spritesOff();lastTick=ticks;
 __asm { cli }
}
void returnToTitle(void) {
 __asm { sei }
 spritesOff();
 *((volatile __memmap u8 *)1)=0x34;
 memcpy((void *)0x1000,(void *)0xcd00,4400);
 *((volatile __memmap u8 *)1)=0x35;
 sfxRequest=sfxKind=sfxTime=0;
 __asm { jsr musicInit }
 titleScreen();titleBlocked=1;lastTick=ticks;
 __asm { cli }
}
const u8 overLetters[8][7]={
 {14,17,16,23,17,17,14},{14,17,17,31,17,17,17},
 {17,27,21,21,17,17,17},{31,16,16,30,16,16,31},
 {14,17,17,17,17,17,14},{17,17,17,17,17,10,4},
 {31,16,16,30,16,16,31},{30,17,17,30,20,18,17}
};
void showGameOver(void) {
 u8 i,r,c,sx,sy,x,y;u16 off;
 __asm { sei }
 spritesOff();
 __asm { cli }
 mode=2;gameOverTime=150;
 // Four black character rows protect the message from the frozen scenery.
 for(r=10;r<14;r++) {
  memset(BMP+(u16)r*320+96,0,128);
  memset(SCREEN+(u16)r*40+12,16,16);
 }
 for(i=0;i<8;i++)for(r=0;r<7;r++)for(c=0;c<5;c++)
  if(overLetters[i][r]&(16>>c))for(sy=0;sy<2;sy++)for(sx=0;sx<2;sx++) {
   x=106+i*12+(i>=4?12:0)+c*2+sx;y=88+r*2+sy;
   off=(u16)(y>>3)*320+(x&0xf8)+(y&7);
   BMP[off]|=128>>(x&7);
  }
}

#pragma code(code)
