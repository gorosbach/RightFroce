void clearfoes(void) {
 u8 i;for(i=0;i<ENEMIES;i++){enemy[i].active=0;boom[i]=0;}
 for(i=0;i<BOLTS;i++)bolt[i].active=0;formationLeft=0;
}
void clearPlayerShots(void) {
 u8 i;for(i=0;i<SHOTS;i++){shot[i].active=0;shotMask[i]=0;}
}
void reset(void) {
 u8 i;kills=0;level=wave=1;lives=3;warp=3;charge=warptime=0;invul=60;
 mode=1;waveKills=spawnCount=boss=bossDying=playerBoom=0;
 px=50;py=shipY=95;timer=90;cooldown=0;clearfoes();
 waveSeconds=waveSubTicks=0;warpTrips=tributeActive=0;
 aiPhase=0;sideCount=sideCooldown=0;clearPlayerShots();weaponLevel=0;shownWeapon=255;loot.active=0;lootCounter=0;
 for(i=0;i<3;i++){rock[i].active=0;rockWait[i]=70+i*90;}
 hudDirty=1;
}
void weaponHud(void) {
 u8 k,r;if(shownWeapon==weaponLevel)return;
 for(k=0;k<4;k++)for(r=0;r<5;r++)BMP[320+48+k*8+r]=k<=weaponLevel?0xf0:0x90;
 shownWeapon=weaponLevel;
}
void damage(void) {
 if(invul||warptime||playerBoom||!lives)return;
 --lives;invul=150;playerBoom=20;
 if(sideCount)--sideCount;else if(weaponLevel)--weaponLevel;
 clearPlayerShots();sound(3);hudDirty=1;
}
void award(void) {
 ++kills;++waveKills;sound(3);hudDirty=1;
 if(++charge==3){charge=0;if(warp<3)++warp;}
}
void giveLoot(u8 type) {
 if(!type&&weaponLevel<3)++weaponLevel;
 else if(!type&&sideCount<2)++sideCount;
 else if(lives<9)++lives;
 hudDirty=1;sound(4);
}
void collectLoot(void) {giveLoot(lootType);loot.active=0;}
void dropLoot(int x,int y) {
 u8 type=(++lootCounter%3)==0;
 // A new reward is granted immediately if the previous pickup is still flying.
 if(loot.active){giveLoot(type);return;}
 lootType=type;loot.active=1;loot.x=x;loot.y=y;lootBase=y;lootTime=400;
}
__forceinline u8 rockHit(int x,int y,u8 w,u8 h) {
 return rock[2].active&&x<rock[2].x+21&&x+w>rock[2].x+3&&y<rock[2].y+19&&y+h>rock[2].y+2;
}
void updateRocks(void) {
 u8 i,s;
 for(i=0;i<3;i++) {
  if(!rock[i].active) {
   if(rockWait[i])--rockWait[i];
   else {rock[i].active=1;rock[i].x=328;rock[i].y=28+random16()%142;}
  } else {
   s=i==0?((frame&1)==0):(i==1?1:2+(wave>4));
   rock[i].x-=s;
   if(rock[i].x< -24){rock[i].active=0;rockWait[i]=70+random16()%150;}
  }
 }
 if(rockHit(px+6,shipY+7,18,8))damage();
}
void spawnFormation(void) {
 u8 i,j,reds=0;
 for(i=0;i<ENEMIES;i++)if(enemy[i].active&&elite[i])++reds;
 if(!formationLeft) {
  formationLeft=5+(random16()&1);formationIndex=0;
  formationBase=40+random16()%20;formationSeed=random16();formationSide=random16()&1;
  formationKind=((waveSeconds<60?0:(waveSeconds<120?1:2))+wave-1)%3;
 }
 for(i=0;i<ENEMIES;i++)if(!enemy[i].active&&!boom[i]) {
  enemy[i].active=1;enemy[i].x=formationSide?295:0;enemy[i].dx=formationSide?-1:1;
  enemyBaseY[i]=formationBase+(formationIndex%3)*36;
  enemyPhase[i]=formationSeed;enemy[i].y=enemyBaseY[i];enemySeen[i]=0;
  elite[i]=(++spawnCount%5)==0&&reds<(wave<3?1:2);hp[i]=elite[i]?5:1;
  enemyKind[i]=formationKind;
  if(!elite[i]){enemy[i].x=320;enemy[i].dx=-1;if(formationKind==2)hp[i]=2;}
  fireTimer[i]=elite[i]?100+random16()%41:0;
  for(j=0;j<SHOTS;j++)shotMask[j]&=~(1<<i);
  --formationLeft;++formationIndex;timer=formationLeft?16:(wave<8?100-wave*6:52);return;
 }
 timer=4;
}
void gameUpdate(u8 pressed) {
 u8 i,j,b,s,damagePoints,width;signed char wobble;
 if(!lives)return;
 if(pressed&&!spaceold&&warp&&!warptime&&!boss&&!bossDying&&!playerBoom) {
  --warp;tributeActive=(++warpTrips&3)==0;warptime=tributeActive?230:75;
  tributeX=-72;tributeY=shipY>76?shipY-52:24;if(tributeY>120)tributeY=120;
  clearfoes();clearPlayerShots();timer=140;sound(2);hudDirty=1;
 }
 if(invul)--invul;
 if(!playerBoom){
  px+=joyx[0]*2;py+=joyy[0]*2;
  if(px<0)px=0;if(px>296)px=296;if(py<24)py=24;if(py>178)py=178;
  shipY=py+sine[(frame>>1)&31]/5;
 }
 sideY[0]=shipY-18;if(sideY[0]<6)sideY[0]=6;
 sideY[1]=shipY+18;if(sideY[1]>188)sideY[1]=188;
 if(sideCooldown)--sideCooldown;
 if(cooldown)--cooldown;
 if(joyb[0]&&!cooldown&&!warptime&&!playerBoom)for(i=0;i<(sideCount==2?2:3);i++)if(!shot[i].active){
  shot[i].active=1;shot[i].x=px+20;shot[i].y=shipY+1;shotMask[i]=0;
  cooldown=weaponLevel?8:12;sound(1);break;
 }
 if(sideCount&&joyb[0]&&!sideCooldown&&!warptime&&!playerBoom) {
  for(b=0;b<sideCount;b++){i=3+b;if(!shot[i].active){
   shot[i].active=1;shot[i].x=px+12;shot[i].y=sideY[b];shotMask[i]=0;sideCooldown=64;sound(1);
  }}
 }
 if(tributeActive&&warptime<210)tributeX+=3;
 updateRocks();
 if(timer)--timer;
 if(!timer&&!warptime&&!boss&&!bossDying&&waveSeconds<180)spawnFormation();
 aiPhase^=1;
 for(i=aiPhase;i<ENEMIES;i+=2)if(enemy[i].active) {
  if(elite[i]){enemyPhase[i]+=6;enemy[i].y=enemyBaseY[i]+sine[(enemyPhase[i]>>1)&31]+sine[((enemyPhase[i]*3)>>3)&31];}
  else {wobble=gentleSine[((enemy[i].x>>4)+enemyPhase[i])&31];if(enemyKind[i]==1)wobble/=2;enemy[i].y=enemyBaseY[i]+wobble;}
  s=2+(wave-1)/3;if(s>5)s=5;if(elite[i]||enemyKind[i]==1)++s;
  enemy[i].x+=enemy[i].dx*(s*2);
  if(elite[i]){
   if(enemy[i].x<8){enemy[i].x=8;enemy[i].dx=1;}
   if(enemy[i].x>288){enemy[i].x=288;enemy[i].dx=-1;}
  }else if(enemy[i].x< -24||enemy[i].x>319)enemy[i].active=0;
  if(rockHit(enemy[i].x+3-(enemy[i].dx>0?s*2:0),enemy[i].y+4,18+s*2,13)){
   enemy[i].active=0;hp[i]=0;boom[i]=24;sound(3);continue;
  }
  // Ordinary formation members never shoot. Elites retain their own cadence.
  if(elite[i]) {
  if(fireTimer[i]>1)fireTimer[i]-=2;else fireTimer[i]=0;
  if(!fireTimer[i])for(b=0;b<BOLTS;b++)if(!bolt[b].active){
   fireTimer[i]=50-(wave<8?wave*4:28)+random16()%41;
   bolt[b].active=1;bolt[b].x=enemy[i].x+10;bolt[b].y=enemy[i].y;
   boltSeen[b]=0;bolt[b].dx=px>enemy[i].x?4:-4;
   bolt[b].dy=(signed char)(random16()%5)-2;
   break;
  }
  }
  if(enemySeen[i]&&enemy[i].active&&px+24>enemy[i].x&&px+6<enemy[i].x+24&&shipY+14>enemy[i].y+4&&shipY+7<enemy[i].y+15)damage();
 }
 for(i=0;i<SHOTS;i++)if(shot[i].active){
  if(i<3){width=weaponLevel?48:24;damagePoints=weaponLevel>=2?2:1;shot[i].x+=6+weaponLevel*2+(sideCount==2?4:0);}
  else {width=4;damagePoints=1;shot[i].x+=6;shot[i].y+=i==3?-1:1;}
  if(shot[i].x>319||shot[i].y<16||shot[i].y>188)shot[i].active=0;
  s=i<3&&weaponLevel>=2;
  if(rockHit(shot[i].x,shot[i].y+(s?7:9),width,s?5:2))shot[i].active=0;
  // Alternate main-beam and side-projectile collision passes, keeping motion 50 Hz.
  if(!shot[i].active||((frame&1)?i<3:i>=3))continue;
  hitX=shot[i].x;hitY=shot[i].y;hitWidth=width;
  hitBias=s?7:5;hitSpan=s?16:13;hitExclude=shotMask[i];
  __asm { jsr scanShotTargets }
  if(hitTargets)for(j=0;j<ENEMIES;j++)if(hitTargets&maskTable[j]){
   shotMask[i]|=maskTable[j];flash[j]=5;
   if(hp[j]<=damagePoints){enemy[j].active=0;hp[j]=0;boom[j]=24;if(elite[j])dropLoot(enemy[j].x,enemy[j].y);award();}
   else {hp[j]-=damagePoints;sound(1);}
   if(weaponLevel<3||i>=3){shot[i].active=0;break;}
  }
 }
 for(i=0;i<BOLTS;i++)if(bolt[i].active){
  bolt[i].x+=bolt[i].dx;bolt[i].y+=bolt[i].dy;
  if(bolt[i].x<0||bolt[i].x>315||bolt[i].y<24||bolt[i].y>185||rockHit(bolt[i].x,bolt[i].y+9,4,3))bolt[i].active=0;
  if(boltSeen[i]&&bolt[i].active&&bolt[i].x<px+24&&bolt[i].x+4>px+6&&bolt[i].y+9<shipY+15&&bolt[i].y+11>shipY+6){damage();bolt[i].active=0;}
 }
 if(loot.active){
  if(lootTime)--lootTime;if(!(frame&1))--loot.x;
  loot.y=lootBase+sine[(frame>>1)&31]/4;
  if(!lootTime||loot.x< -24)loot.active=0;
  if(!playerBoom&&loot.x<px+24&&loot.x+20>px+6&&loot.y+17>shipY+6&&loot.y+3<shipY+15)collectLoot();
 }
}
void publishGame(void) {
 u8 i;spritePublish();
 for(i=0;i<ENEMIES;i++)enemySeen[i]=enemyDraw[i]!=255&&accepted[enemyDraw[i]];
 for(i=0;i<BOLTS;i++)boltSeen[i]=boltDraw[i]!=255&&accepted[boltDraw[i]];
}
void renderGame(void) {
 u8 i,shape,color,baseColor;int drawX;u8 drawY;
 spriteBegin();shape=shipFrames[(frame>>2)&3];color=warptime?3:7;
 if(playerBoom){shape=23+((20-playerBoom)>>3);color=7;}
 SPR(0,playerBoom||(lives&&(!invul||(frame&4))),px,shipY,shape,color,1,0);
 if(!tributeActive&&rock[2].active)SPR(15,true,rock[2].x,rock[2].y,33,15,1,0);
 if(boss||bossDying){
  for(i=0;i<2;i++)SPR(17+i,true,bossX+i*48,bossY,bossDying?23+((frame>>2)&3):27+i+((frame>>3)&1)*2,bossDying?7:10,7,0);
 }else {baseColor=waveColors[(wave-1)%6];for(i=0;i<ENEMIES;i++){
  if(!enemy[i].active&&!boom[i]){enemyDraw[i]=255;continue;}
  shape=elite[i]||!enemyKind[i]?17:35+enemyKind[i];
  color=flash[i]?1:(elite[i]?10:(enemyKind[i]==1?3:(enemyKind[i]==2?4:baseColor)));
  if(boom[i]){shape=23+((23-boom[i])>>3);color=7;}
  drawX=enemy[i].x;drawY=enemy[i].y;
   enemyDraw[i]=SPR(1+i,true,drawX,drawY,shape,color,1,1);
 }
 }
 if(sideCount&&!warptime&&!playerBoom&&lives)for(i=0;i<sideCount;i++)SPR(27+i,true,px-8,sideY[i],49,3,1,1);
 for(i=0;i<SHOTS;i++)if(shot[i].active){
  if(i<3)SPR(7+i,true,shot[i].x,shot[i].y,weaponLevel>=2?48:18,weaponLevel>=2?10:1,weaponLevel>=2?3:(weaponLevel?2:0),2);
  else SPR(26+i,true,shot[i].x,shot[i].y,19,1,0,2);
 }
 for(i=0;i<BOLTS;i++){boltDraw[i]=255;if(bolt[i].active)boltDraw[i]=SPR(10+i,true,bolt[i].x,bolt[i].y,19,7,0,0);}
 if(loot.active)SPR(16,true,loot.x,loot.y,lootType?35:34,lootType?10:3,1,2);
 if(tributeActive){
  for(i=0;i<3;i++)SPR(17+i,true,tributeX+i*24,tributeY,38+i,12,5,0);
  for(i=0;i<7;i++)SPR(20+i,true,76+i*24,176,41+i,1,0,0);
 }
 for(i=0;i<2;i++)if(!tributeActive&&rock[i].active&&(!sideCount||vcount<12))SPR(13+i,true,rock[i].x,rock[i].y,31+i,i?12:11,1,3);
}
