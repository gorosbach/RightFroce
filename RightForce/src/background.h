u8 backColors[1000];
volatile u8 planetLine;
u8 planetVariant,planetY,planetState,planetRows,planetColumns,planetStart,planetCount,planetSkip;
u8 planetHeight,planetChunkRows,planetSlice,planetPaletteRow;
volatile u8 planetRowsPerFrame,planetBufferOffset;
u16 planetPaletteBase;
u16 planetWait;
int planetX;
u8 planetLo[40],planetHi[40];
u8 planetChunk[50],planetReadIndex,planetChunkNumber;
const u8 *planetDecodeSrc;
u8 decodeRun,decodeTag,decodeLimit;

#pragma optimize(push)
#pragma optimize(noasm)
__asm planetDecode {
 lda planetDecodeSrc
 sta $fd
 lda planetDecodeSrc+1
 sta $fe
 ldx #0
 ldy #0
newrun:
 lda ($fd),y
 sta decodeTag
 and #127
 clc
 adc #1
 sta decodeRun
 inc $fd
 bne ptrready
 inc $fe
ptrready:
 lda decodeTag
 bmi literal
 lda #0
zeros:
 sta planetChunk,x
 inx
 dec decodeRun
 bne zeros
 jmp checkend
literal:
 lda ($fd),y
 sta planetChunk,x
 inc $fd
 bne ptrnext
 inc $fe
ptrnext:
 inx
 dec decodeRun
 bne literal
checkend:
 cpx decodeLimit
 bcc newrun
 rts
}
__asm planetDraw {
 clc
 lda planetSkip
 adc planetBufferOffset
 sta planetReadIndex
 lda planetRowsPerFrame
 sta planetRows
row:
 ldx planetLine
 ldy planetStart
 clc
 lda planetLo,x
 adc colofflo,y
 sta $fb
 sta $f9
 lda planetHi,x
 adc coloffhi,y
 sta $fc
 sec
 sbc #$40
 sta $fa
 lda planetCount
 sta planetColumns
 ldx planetReadIndex
 ldy #0
column:
 lda planetChunk,x
 sta ($fb),y
 sta ($f9),y
 inx
 tya
 clc
 adc #8
 tay
 dec planetColumns
 bne column
 inc planetLine
 clc
 lda planetReadIndex
 adc #10
 sta planetReadIndex
 dec planetRows
 bne row
 rts
}
#pragma optimize(pop)

const char nebulaArt[]={
 #embed 1152 0 lzo "assets/runtime/nebula.bin"
};
void backgroundInit(void) {
 u8 r,c;u16 src,dst,cell;
 memset(BACK,0,8000);memset(backColors,0,1000);
 oscar_expand_lzo((char *)BMP,nebulaArt);
 for(r=0;r<8;r++)for(c=0;c<16;c++) {
  src=(u16)r*128+c*8;dst=(u16)(r+4)*320+16+c*8;
  memcpy(BACK+dst,BMP+src,8);memcpy(BMP+dst,BMP+src,8);
  cell=(u16)(r+4)*40+2+c;
  backColors[cell]=BMP[1024+(u16)r*16+c];SCREEN[cell]=backColors[cell];
 }
 memset(BMP,0,1152);
}
void planetSpawn(void) {
 u8 y;u16 a;
 const u8 heights[3]={40,32,32};
 const u16 paletteBases[3]={0,400,720};
 planetVariant=random16()%3;planetHeight=heights[planetVariant];
 planetChunkRows=planetHeight>>3;planetSlice=0;
 planetPaletteBase=paletteBases[planetVariant];
 planetY=104+(random16()%((200-planetHeight-104)/8+1))*8;
 planetX=320;planetLine=0;planetChunkNumber=0;planetState=1;
 for(y=0;y<planetHeight;y++) {
  a=0xe000+(u16)((y+planetY)>>3)*320+((y+planetY)&7);
  planetLo[y]=a;planetHi[y]=a>>8;
 }
}
void planetInit(void) {
 backgroundInit();planetLine=0;planetSlice=0;planetState=0;planetWait=75;
}
void planetUpdate(void) {
 u8 col,n,y,c,ink,tag,pos,len;int coarse;u16 cell,k;
 const u8 *src;
 const u8 *palette;
 if(!planetState) {if(planetWait)--planetWait;else planetSpawn();return;}
 if(!planetLine) {
  planetPaletteRow=255;planetX-=2;
  if(planetX<=-80) {
   memset(backColors+(u16)(planetY>>3)*40,0,(u16)(planetHeight>>3)*40);
   memset(SCREEN+(u16)(planetY>>3)*40,16,(u16)(planetHeight>>3)*40);
   planetState=0;planetWait=150+random16()%151;return;
  }
 }
 planetRowsPerFrame=planetSlice?planetChunkRows-2:2;
 planetBufferOffset=planetSlice?20:0;
 coarse=planetX>>3;planetSkip=coarse<0?-coarse:0;
 planetStart=coarse<0?0:coarse;
 n=10-planetSkip;if(planetStart+n>40)n=40-planetStart;planetCount=n;
 if(n) {
  // Restore the previous palette and install this row's planet palette.
  for(y=planetLine>>3;y<=((planetLine+planetRowsPerFrame-1)>>3);y++)if(y!=planetPaletteRow) {
   planetPaletteRow=y;
   cell=(u16)((planetY>>3)+y)*40+planetStart;
   palette=planetPalette+planetPaletteBase+(u16)(planetX&7)*(planetHeight*10/8)+y*10+planetSkip;
   for(c=0;c<n;c++) {
    ink=palette[c];backColors[cell+c]=ink;SCREEN[cell+c]=ink;
   }
   if(planetStart+n<40) {backColors[cell+n]=0;SCREEN[cell+n]=0x10;}
  }
  if(!planetSlice){
  k=((u16)planetVariant*64+(planetX&7)*8+planetChunkNumber)*2;
  planetDecodeSrc=planetPacked+(planetIndex[k]|((u16)planetIndex[k+1]<<8));
  decodeLimit=planetChunkRows*10;
  __asm { jsr planetDecode }
  }
  __asm { jsr planetDraw }
 } else planetLine+=planetRowsPerFrame;
 planetSlice^=1;
 if(!planetSlice&&++planetChunkNumber==8){planetChunkNumber=0;planetLine=0;}
}
