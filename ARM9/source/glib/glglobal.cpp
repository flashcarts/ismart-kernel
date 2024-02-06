
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "glglobal.h"

CglScreenMain *pScreenMain;
CglScreenMainOverlay *pScreenMainOverlay;
CglScreenSub *pScreenSub;

void glDefaultMemorySetting(void)
{
  videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256);
  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256);
//  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256);
  
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_SPRITE_0x06400000, VRAM_C_MAIN_BG_0x06020000,VRAM_D_SUB_SPRITE);
  vramSetBankE(VRAM_E_LCD);
  vramSetBankF(VRAM_F_LCD);
  vramSetBankG(VRAM_G_LCD);
  vramSetBankH(VRAM_H_SUB_BG);
  vramSetBankI(VRAM_I_LCD);
}

void glDefaultClassCreate(void)
{
    //创建主屏的显示模式，双显示模式，BG3 ,BG2 , 缓冲模式可以进行切换显示
    pScreenMain=new CglScreenMain();
    //创建主屏的OVERLay ，sprite 的设置和显示,一共12个sprite
    pScreenMainOverlay=new CglScreenMainOverlay();
    //创建子屏的显示,同OVERLAY一样也是Sprite
    pScreenSub=new CglScreenSub();
}

void glDefaultClassFree(void)
{
  delete pScreenMain; pScreenMain=NULL;
  delete pScreenMainOverlay; pScreenMainOverlay=NULL;
  delete pScreenSub; pScreenSub=NULL;
}

//static void (*glDebugPrint)(const char* s)=NULL;

void glSetFuncDebugPrint(void (*_DebugPrint)(const char* s))
{
//  glDebugPrint=_DebugPrint;
}

/*
void glDebugPrintf(const char* format, ...)
{
  if(glDebugPrint==NULL) return;
  
  char strbuf[256];
  
  va_list args;
  
  va_start( args, format );
  vsprintf( strbuf, format, args );
  
  glDebugPrint(strbuf);
}
*/
