#include <c64/sprites.h>
#include <c64/joystick.h>
#include <string.h>
#include <oscar.h>
#pragma stacksize(1024)
#pragma heapsize(0)

// VIC bank $c000: screen $c000, sprites $c400, bitmap $e000.
#define REG(a) (*(volatile unsigned char *)(a))
#define BMP ((unsigned char *)0xe000)
#define SCREEN ((unsigned char *)0xc000)
#define BACK ((unsigned char *)0xa000)
#define SHAPES ((unsigned char *)0xc400)
typedef unsigned char u8;
typedef unsigned int u16;
typedef struct { int x,y; signed char dx,dy; u8 active; } Actor;
#define ENEMIES 6
#define BOLTS 3
#define SHOTS 5
Actor enemy[ENEMIES],shot[SHOTS],bolt[BOLTS],rock[3],loot;
u16 rockWait[3],lootTime;
u8 weaponLevel,shownWeapon=255,shotMask[SHOTS],lootType,lootCounter,lootBase;
u8 formationLeft,formationIndex,formationBase,formationSeed,formationSide;
u8 sideCount,sideCooldown,sideY[2];
u8 aiPhase,enemyKind[ENEMIES],formationKind,warpTrips,tributeActive,tributeY;
int tributeX;
volatile u8 waveSeconds,waveSubTicks;
u8 enemySeen[ENEMIES],boltSeen[BOLTS],enemyDraw[ENEMIES],boltDraw[BOLTS];
u16 rng=3711, kills, timer;
u8 hudDirty;
u8 frame, warp, warptime, charge, lives, level, cooldown, invul, spaceold;
int px,py,shipY;
u8 enemyBaseY[ENEMIES],enemyPhase[ENEMIES];
const signed char sine[32]={0,3,6,9,11,13,15,16,16,16,15,13,11,9,6,3,0,-3,-6,-9,-11,-13,-15,-16,-16,-16,-15,-13,-11,-9,-6,-3};
// 512-pixel wavelength: about 0.6 periods per screen, eight-pixel amplitude.
const signed char gentleSine[32]={0,1,3,4,5,6,7,8,8,8,7,6,5,4,3,1,0,-1,-3,-4,-5,-6,-7,-8,-8,-8,-7,-6,-5,-4,-3,-1};
const u8 shipFrames[4]={16,20,21,22};
volatile u8 keySample;
u8 titleBlocked;u16 gameOverTime;
u8 mode,wave=1,waveKills,spawnCount,elite[ENEMIES],hp[ENEMIES],fireTimer[ENEMIES],boom[ENEMIES],flash[ENEMIES],playerBoom;
u8 boss,bossHP,bossMax,bossDying,wavePause;
int bossX,bossY;u16 bossClock;
const u8 waveColors[6]={5,3,14,7,10,13};

u16 random16(void) { rng ^= rng<<7; rng ^= rng>>9; rng ^= rng<<8; return rng; }
u8 colofflo[40],coloffhi[40];
#include "sound.h"
#include "ambience.h"
#include "renderer.h"
#include "hit_test.h"
#include "engine.h"
u8 space(void) {
 // Scan SPACE in the reverse direction: PB4 -> PA7. Joystick port 2
 // shares PA0..4, so it cannot interfere with this sensed bit.
 // Explicit settling time and reads also work with older compilers.
 __asm {
  lda #$ff
  sta $dc00
  lda #0
  sta $dc02
  lda #$ef
  sta $dc01
  lda #$ff
  sta $dc03
  nop
  nop
  lda $dc00
  and #$80
  sta keySample
  lda #0
  sta $dc03
  lda #$ff
  sta $dc01
  sta $dc00
  sta $dc02
 }
 return keySample==0;
}
// Tiny numeric HUD: level, lives, WARP, kills (four groups).
const u8 digits[10][5]={{7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,1,1,1},{7,5,7,5,7},{7,5,7,1,7}};
void number(u8 x,u16 n) {
 u8 d[3],r,k,t; unsigned char *p;
 if(n>999) n=999;
 d[0]=0;while(n>=100) {n-=100;++d[0];}
 t=n;d[1]=0;while(t>=10) {t-=10;++d[1];}d[2]=t;
 p=BMP+320+(x&0xf8);
 for(k=0;k<3;k++) {
 for(r=0;r<5;r++) p[r]=digits[d[k]][r]<<4;
 p+=8;
 }
}
u8 shownLevel=255,shownLives=255,shownWarp=255,shownClock=255;
u16 shownKills=65535;
// Update at most one numeric group per frame, keeping SID/graphics time bounded.
void hud(void) {
 if(lives!=shownLives) {number(92,lives);shownLives=lives;hudDirty=1;return;}
 if(warp!=shownWarp) {number(172,warp);shownWarp=warp;hudDirty=1;return;}
 if(level!=shownLevel) {number(12,wave);shownLevel=level;hudDirty=1;return;}
 if((boss?0:180-waveSeconds)!=shownClock){shownClock=boss?0:180-waveSeconds;number(212,shownClock);hudDirty=1;return;}
 if((boss?bossHP:kills)!=shownKills) {number(252,boss?bossHP:kills);shownKills=boss?bossHP:kills;}
}
#include "gameplay.h"
#include "encounter.h"
#include "title.h"
int main(void) {
 u8 pressed;
 __asm { sei }
 REG(0x01)=0x35;backupTitleMusic();
 REG(0xdd00)&=252;REG(0xd011)=0x3b;REG(0xd016)=8;REG(0xd018)=8;
 REG(0xd020)=REG(0xd021)=0;
 memset(BMP,0,8000);memset(SCREEN,16,1000);
 oscar_expand_lzo((char *)SHAPES,spriteArt);REG(0xd025)=1;REG(0xd026)=14;
 starsInit();planetInit();reset();
 __asm { jsr musicInit }
 titleScreen();irqInit();
 for(;;) {
  framewait();joy_poll(0);pressed=space();++frame;
  if(!mode) {
   if(!joyb[0])titleBlocked=0;
   if(joyb[0]&&!titleBlocked)startGame();else continue;
  }
  if(mode==2) {if(gameOverTime)--gameOverTime;else returnToTitle();continue;}
  if(!lives&&!playerBoom) {showGameOver();continue;}
  gameUpdate(pressed);encounter();
  planetUpdate();starsUpdate();if(warptime){if(!--warptime){tributeActive=0;invul=50;}}
  if(playerBoom)--playerBoom;
  for(u8 fx=0;fx<ENEMIES;fx++){if(boom[fx])--boom[fx];if(flash[fx])--flash[fx];}
  // Split queue preparation and raster scheduling over alternating logic ticks.
  // The IRQ interpolates the committed positions on the intervening PAL frame.
  if(!(frame&1))renderGame();else publishGame();
  if(hudDirty) {hudDirty=0;hud();}
  weaponHud();spaceold=pressed;
 }
 return 0;
}

