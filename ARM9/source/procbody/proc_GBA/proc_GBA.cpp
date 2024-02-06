
#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "lang.h"
#include "BootROM.h"

#include "procstate.h"
#include "launchstate.h"
#include "datetime.h"

#include "glib/glib.h"

#include "fat2.h"
#include "shell.h"
#include "ErrorDialog.h"

#include "skin.h"
#include "component.h"
#include "sndeff.h"
#include "lang.h"
#include "mediatype.h"
#include "gba_nds_fat.h"

#include "disc_io.h"
#include "proc_GBA.h"
#include "../../mycartsrc/MartCartop.h"
#include "proc_GBA_RomOP.h"

extern LPIO_INTERFACE active_interface;


#define SYSColor1  ColorTable.ZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.ZV_SYSTEM.SYS_Color2

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}

TGBAInfo GBAInfo;
uint32 gl_ShakeID;
const u32 x=32,y=54,w=194,h=54;
const u32 ProgressVal=1765;
static u32 ProgressValCur;

static bool InitFlag;
static bool ForPFlag;
static bool RunGBAFlag;
static bool DisableABFlag;


static bool isPressRect11=false;
static bool isPressRect12=false;

const TRect  menurect00={x+8,y+2+20,40,16};
const TRect  menurect01={x+8+65,y+2+20,40,16};
const TRect  menurect02={x+8+130,y+2+20,40,16};

void SetInitFlag(void)
{
	InitFlag = true;
}
void ClearInitFlag(void)
{
	InitFlag = false;
}
bool GetInitFlag(void)
{
	return InitFlag;
}
void SetForPFlag(void)
{
	ForPFlag = true;
}
void ClearForPFlag(void)
{
	ForPFlag = false;
}
bool GetForPFlag(void)
{
	return ForPFlag;
}

void SetRunGBAFlag(void)
{
	RunGBAFlag = true;
}
void ClearRunGBAFlag(void)
{
	RunGBAFlag = false;
}
bool GetRunGBAFlag(void)
{
	return RunGBAFlag;
}
void SetDisableABFlag(void)
{
	DisableABFlag = true;
}
void ClearDisableABFlag(void)
{
	DisableABFlag = false;
}
bool GetDisableABFlag(void)
{
	return DisableABFlag;
}
static void RunGBA(u16 RomPage,u16 RamPage)
{
	
	
	register int k  ;
	for(k=0;k<256*256;k++)
		(*(uint16*)(0x6200000+k*2)) = 0;
	for(k=0;k<256*256;k++)
		(*(uint16*)(0x6000000+k*2)) = 0;
	
	*(uint16*)0x2000004 = RomPage;//0 ; //rompage
	*(uint16*)0x2000006 = RamPage ;//16 ; //rampage
	*(vuint32*)0x027ffe04 = 0xe59ff018 ;
	*(vuint32*)0x027ffe24 = 0x027ffe04 ;
	
	//POWER_CR  = 0x8001;
	 (*(vuint16*)0x04000304)= 0x8001;
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE); //sub bg 0 will be used to print text
	vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_SUB_BG , VRAM_D_LCD);
	videoSetMode (MODE_FB0);
	for(k=0;k<256*256;k++)
		(*(vuint16*)(0x06800000+k*2)) = BIT(15) ;
	for(k=0;k<256*256;k++)
		(*(vuint16*)(0x06820000+k*2)) = BIT(15);
	videoSetMode(MODE_0_2D);
	
	for(k=0;k<256*256;k++)
		(*(vuint16*)(0x6200000+k*2)) = 0;
	for(k=0;k<256*256;k++)
		(*(vuint16*)(0x6000000+k*2)) = 0;
	(*(uint16*)0x04000204) |= 0x0880; ;
	
	*(vuint32*)0x2000000 = 0xFDEf54C8 ;	
	while(1);  
}


static void CB_CancelBtn_Click(void *pComponentButton)
{
  SetNextProc(ENP_FileList,EPFE_CrossFade);
}

// -----------------------------

static void PathNameCopy()
{
	 Unicode_Copy(GBAOPInfo.FileName,GBAInfo.FileName);
	 strcpy(GBAOPInfo.FullPathAlias,GBAInfo.FullPathAlias);
}

static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
 
 if((Keys&KEY_A)!=0){
		// IPC6->LCDPowerControl=LCDPC_SOFT_POWEROFF;
	 PathNameCopy();
	 if((gl_ShakeID==0x227E2218) || (gl_ShakeID==0x227E2202) || (gl_ShakeID==0x89168916))
	 {
		 if(GetInitFlag())
		 {
			 SetDisableABFlag();
			 UpdataWindows();
			 if(!GetForPFlag())
			 {				 
				 //写入游戏到NorFlash
				 WriteGbaRomToNorFlash();
				 //SetRunGBAFlag();
				 Enable_Arm9DS();
			 	 OpenNorWrite();			    
			 	 SetSerialMode();
			 	 chip_reset();
			 	 CloseNorWrite();
			 	 
				 RunGBA(0,16);	
			 }
			 else
			 {
				 //写入游戏到PsRam
				 
				 WriteGbaRomToPsRam();
				 RunGBA(384,0x70);				 
			 }
		 }
	 }
	 else
	 {
		 CB_CancelBtn_Click(NULL);
	 }
  }
  
  if((Keys&KEY_B)!=0){
	  if(GetInitFlag())
	  {
		  ClearInitFlag();
	  }
	  else
	  {
		  CB_CancelBtn_Click(NULL);
	  }
  }
  if((Keys&KEY_X)!=0){
		if(!GetInitFlag())
		{
		    SetInitFlag();
			ClearForPFlag();
		}
  }
  if((Keys&KEY_Y)!=0){
	  if(!GetInitFlag())
	  {
		  SetInitFlag();	
		  SetForPFlag();
	  }
  }
  if((Keys&(KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_DOWN))!=0){}
}

static void CB_MouseDown(s32 x,s32 y)
{
	
}

static void CB_MouseMove(s32 x,s32 y)
{
	
}

static void CB_MouseUp(s32 x,s32 y)
{
	if(isInsideRect(menurect00,x,y)==true)
	{
		// IPC6->LCDPowerControl=LCDPC_SOFT_POWEROFF;
		SetInitFlag();
		ClearForPFlag();
	}
	if(isInsideRect(menurect01,x,y)==true)
	{
		ClearInitFlag();
		CB_CancelBtn_Click(NULL);
	}	
	if(isInsideRect(menurect02,x,y)==true)
	{
		SetInitFlag();	
		SetForPFlag();
	}	
}

static void gausscolor50bk(CglCanvas *pcan)
{
  CglCanvas *psrccan=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
  pcan->BitBltFullBeta(psrccan);
  
  CglCanvas *pdstcan=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
  pdstcan->FillFull(RGB15(0,0,0)|BIT15);
  
  for(u32 y=1;y<(ScreenHeight-1);y++){
    u16 *psrcbufm1=psrccan->GetScanLine(y-1);
    u16 *psrcbuf=psrccan->GetScanLine(y);
    u16 *psrcbufp1=psrccan->GetScanLine(y+1);
    u16 *ptagbuf=pdstcan->GetScanLine(y);
    for(u32 x=1;x<(ScreenWidth-1);x++){
      u32 c0=psrcbuf[x];
      u32 c1=psrcbufm1[x];
      u32 c2=psrcbuf[x-1];
      u32 c3=psrcbuf[x+1];
      u32 c4=psrcbufp1[x];
      u32 c5=psrcbufm1[x-1];
      u32 c6=psrcbufm1[x+1];
      u32 c7=psrcbufp1[x-1];
      u32 c8=psrcbufp1[x+1];
      u32 r0=(c0&(0x1f<<0))>>0;
      u32 r1=(c1&(0x1f<<0))>>0;
      u32 r2=(c2&(0x1f<<0))>>0;
      u32 r3=(c3&(0x1f<<0))>>0;
      u32 r4=(c4&(0x1f<<0))>>0;
      u32 r5=(c5&(0x1f<<0))>>0;
      u32 r6=(c6&(0x1f<<0))>>0;
      u32 r7=(c7&(0x1f<<0))>>0;
      u32 r8=(c8&(0x1f<<0))>>0;
      u32 g0=(c0&(0x1f<<5))>>5;
      u32 g1=(c1&(0x1f<<5))>>5;
      u32 g2=(c2&(0x1f<<5))>>5;
      u32 g3=(c3&(0x1f<<5))>>5;
      u32 g4=(c4&(0x1f<<5))>>5;
      u32 g5=(c5&(0x1f<<5))>>5;
      u32 g6=(c6&(0x1f<<5))>>5;
      u32 g7=(c7&(0x1f<<5))>>5;
      u32 g8=(c8&(0x1f<<5))>>5;
      u32 b0=(c0&(0x1f<<10))>>10;
      u32 b1=(c1&(0x1f<<10))>>10;
      u32 b2=(c2&(0x1f<<10))>>10;
      u32 b3=(c3&(0x1f<<10))>>10;
      u32 b4=(c4&(0x1f<<10))>>10;
      u32 b5=(c5&(0x1f<<10))>>10;
      u32 b6=(c6&(0x1f<<10))>>10;
      u32 b7=(c7&(0x1f<<10))>>10;
      u32 b8=(c8&(0x1f<<10))>>10;
      u32 r=((r0*4)+((r1+r2+r3+r4)*2)+((r5+r6+r7+r8)*1))/(4+8+4);
      u32 g=((g0*4)+((g1+g2+g3+g4)*2)+((g5+g6+g7+g8)*1))/(4+8+4);
      u32 b=((b0*4)+((b1+b2+b3+b4)*2)+((b5+b6+b7+b8)*1))/(4+8+4);
      r=r/2;
      g=g/2;
      b=b/2;
      ptagbuf[x]=RGB15(r,g,b)|BIT15;
    }
  }
  
  pdstcan->BitBltFullBeta(pcan);
  
  delete psrccan; psrccan=NULL;
  delete pdstcan; pdstcan=NULL;
}

static CglCanvas *pMasterBG;
#include "mycartsrc/CBMPResource.h"
extern void videoSub_SetShowLog(bool);
extern void TestPauseKey(void);
static void DrawOnlineHelp(void)
{

	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	ClearInitFlag();
	ClearForPFlag();
	ClearRunGBAFlag();
//	gausscolor50bk(pTmpBM);
 	//while(1);
	Enable_Arm9DS();
	OpenNorWrite();
    chip_reset();
	SetSerialMode();
	u32 idf=ReadNorFlashID();			
	CloseNorWrite();
	gl_ShakeID=idf;
	// _consolePrintf("\n idf=%x",idf);*/		 
	//TestPauseKey();
//	videoSub_SetShowLog(true);
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	}  
}

static void UpdataGBA(void)
{
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(SYSColor1);//设置字体颜色黑色  
		
	CglTGF * pbm2= GBAAlpha_GetSkin(EGBASA_GBABG);
	pbm2->BitBlt(pTmpBM,0,0);
   
	if(!GetInitFlag())
	{
		pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_Menu")); 	
		pTmpBM->TextOutUTF8(x+8,y+2+20,Lang_GetUTF8("GBA_NorFlash")); 
		pTmpBM->TextOutUTF8(x+8+65,y+2+20,Lang_GetUTF8("GBA_Cancel")); 
		pTmpBM->TextOutUTF8(x+8+130,y+2+20,Lang_GetUTF8("GBA_PsRam"));
	}
	else
	{
		if(!GetForPFlag())
		{
			if(GetbEraseFlag())
			{
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_Erase"));
			}
			else if(GetbSaveFlag())
			{
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_SavePatch"));
			}
			else if(GetbSaveMagFlag())
			{
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_Save_Load_Backup"));
			}
			else
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_WriteNorFlash")); 	
		}
		else
		{
			if(GetbSaveFlag())
			{
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_SavePatch")); 	
			}
			else if(GetbSaveMagFlag())
			{
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_Save_Load_Backup"));
			}
			else
				pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("GBA_WritePsRam")); 	
		}
		
		pbm2= GBAAlpha_GetSkin(EGBASA_progrss_bar_bg);
		pbm2->BitBlt(pTmpBM,x+8,y+2+20);
		 
		pbm2= GBAAlpha_GetSkin(EGBASA_progress_bar);
		
		u32 idxPro=0;
		if(GetbEraseFlag())
		{
			while(idxPro<=GetEraseProgressVal())
			{
				pbm2->BitBlt(pTmpBM,x+9+idxPro,y+2+20+1);
				idxPro++;	  
			}
		}
		idxPro=0;
		if(GetbWriteFlag())
		{
			while(idxPro<=GetEraseProgressVal())
			{
				pbm2->BitBlt(pTmpBM,x+9+idxPro,y+2+20+1);
				idxPro++;	  
			}
		}
		idxPro=0;
		if(GetbSaveFlag())
		{
			while(idxPro<=GetEraseProgressVal())
			{
				pbm2->BitBlt(pTmpBM,x+9+idxPro,y+2+20+1);
				idxPro++;	  
			}
		}
		else
		{
			//pbm2->BitBlt(pTmpBM,x+8+idxPro,y+2+20+1);
		}
		if(!GetDisableABFlag())
		{
			pTmpBM->TextOutUTF8(x+70,y+54-16,Lang_GetUTF8("GBA_OK")); 	
			pTmpBM->TextOutUTF8(x+70+38*2,y+54-16,Lang_GetUTF8("GBA_Cancel")); 
		}
		else
		{
			pTmpBM->SetFontTextColor(SYSColor2);
			pTmpBM->TextOutUTF8(x+70,y+54-16,Lang_GetUTF8("GBA_OK")); 	
			pTmpBM->TextOutUTF8(x+70+38*2,y+54-16,Lang_GetUTF8("GBA_Cancel")); 
		}
	}
	 /*
	if(0)
	{
		 pbm2= GBAAlpha_GetSkin(EGBASA_progrss_bar_bg);
		 pbm2->BitBlt(pTmpBM,x+8,y+2+20);
		 
		 pbm2= GBAAlpha_GetSkin(EGBASA_progress_bar);
		 u32 idxPro=0;
		 while(idxPro<ProgressValCur)
		 {
			 pbm2->BitBlt(pTmpBM,x+8+idxPro,y+2+20+1);
			 idxPro++;	  
		 }
		 pTmpBM->TextOutUTF8(x+70+38*2,y+54-16,Lang_GetUTF8("GBA_Cancel")); 
	}
		
	*/
	
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	}   
	/*if(GetRunGBAFlag())
	{
		//运行GBA
		ClearRunGBAFlag();
		RunGBA();		
	}*/


}
void UpdataWindows()
{
	UpdataGBA();
}
static void CB_Start(void)
{
	Enable_Arm9DS();
	SetShake(0x08);//关闭震动
	lcdMainOnTop();
	DrawOnlineHelp(); 
}

static void CB_VsyncUpdate(u32 VsyncCount)
{

	UpdataGBA();
}	

static void CB_End(void)
{
	
}

void ProcGBA_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}




