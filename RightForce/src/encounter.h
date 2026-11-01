void encounter(void) {
 u8 i,j,phase,points,beamWidth,beamTop,beamBottom;int d;
 if(!lives)return;
 if(bossDying) {
  if(!--bossDying) {
   waveSeconds=waveSubTicks=0;boss=0;if(wave<99)++wave;level=wave;waveKills=0;timer=120;
   invul=100;hudDirty=1;clearfoes();
  }
  return;
 }
 if(!boss&&waveSeconds>=180&&!warptime) {
  clearfoes();boom[0]=boom[1]=0;boss=1;bossClock=0;
  bossHP=32+(wave<12?wave*4:48);bossMax=bossHP;
  bossX=212;bossY=70;warptime=0;if(invul<100)invul=100;hudDirty=1;
 }
 if(!boss)return;
 bossClock+=1+(wave>3);phase=(bossClock>>6)%3;
 bossX=202+sine[(bossClock>>2)&31];
 bossY=76+sine[(bossClock>>1)&31]*2;
 if(!warptime)for(i=0;i<2;i++) {
  if(fireTimer[i])--fireTimer[i];
  if(!fireTimer[i]&&!bolt[i].active) {
   fireTimer[i]=phase==2?8+random16()%12:20+random16()%35;
   bolt[i].active=1;bolt[i].x=bossX;bolt[i].y=bossY+4+i*20;
   bolt[i].dx=-(3+(wave>2));d=shipY-bolt[i].y;
   bolt[i].dy=phase==1?(signed char)(random16()%5)-2:(d>8?2:(d< -8?-2:0));
  }
 }
 if(rockHit(bossX+2,bossY+3,38,36)) {bossHP=0;bossDying=90;clearfoes();sound(3);hudDirty=1;return;}
 for(j=0;j<SHOTS;j++)if(shot[j].active&&!(shotMask[j]&64)) {
  beamWidth=j>=3?4:(weaponLevel?48:24);
  beamTop=j<3&&weaponLevel>=2?7:9;
  beamBottom=j<3&&weaponLevel>=2?12:(j>=3?11:10);
  if(shot[j].x+beamWidth>bossX&&shot[j].x<bossX+96&&shot[j].y+beamBottom>bossY&&shot[j].y+beamTop<bossY+42) {
   shotMask[j]|=64;if(weaponLevel<3||j>=3)shot[j].active=0;points=j<3&&weaponLevel>=2?2:1;
   bossHP=bossHP>points?bossHP-points:0;sound(1);hudDirty=1;
   if(!bossHP) {bossDying=90;clearfoes();award();break;}
  }
 }
 if(px+24>bossX&&px+6<bossX+96&&shipY+14>bossY&&shipY+7<bossY+42)damage();
}
