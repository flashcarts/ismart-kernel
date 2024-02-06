
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

#include "proc_CpyCut.h"


#define SYSColor1  ColorTable.ZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.ZV_SYSTEM.SYS_Color2


extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}


static bool bPressX1Flag;
static bool bPressX2Flag;
static bool bPressY1Flag;
static bool bPressY2Flag;
static bool bDisableABFlag;

static bool bClearALLFlgTrans;

void SetClearAllFlgTrans(void)
{
	bClearALLFlgTrans = true;
}
void ClearClearAllFlgTrans(void)
{
	bClearALLFlgTrans = false;
}
bool GetClearAllFlgTrans(void)
{
	return bClearALLFlgTrans ;
}

static bool bFinishedOPFlag;

const u32 x=32,y=54,w=194,h=54;
u32 ProgressVal=175;
static u32 ProgressValCur;

TCpyCutFileInfo CpyCutFileInfo;

static void SetFinishedOPFlag(void)
{
	bFinishedOPFlag = true;
}
static void ClearFinishedOPFlag(void)
{
	bFinishedOPFlag = false;
}
static bool GetFinishedOPFlag(void)
{
	return bFinishedOPFlag;
}

u32 SetProgressVal(u32 Val)
{
	if(Val>=ProgressVal)
		ProgressValCur=ProgressVal;
	else
		ProgressValCur=Val;
}

void SetX1Flag(void)
{
	bPressX1Flag = true;
}
void ClearX1Flag(void)
{
	bPressX1Flag = false;
}
bool GetX1Flag(void)
{
	return bPressX1Flag;
}

void SetX2Flag(void)
{
	bPressX2Flag = true;
}
void ClearX2Flag(void)
{
	bPressX2Flag = false;
}
bool GetX2Flag(void)
{
	return bPressX2Flag;
}

void SetY1Flag(void)
{
	bPressY1Flag = true;
}
void ClearY1Flag(void)
{
	bPressY1Flag = false;
}
bool GetY1Flag(void)
{
	return bPressY1Flag;
}

void SetY2Flag(void)
{
	bPressY2Flag = true;
}
void ClearY2Flag(void)
{
	bPressY2Flag = false;
}
bool GetY2Flag(void)
{
	return bPressY2Flag;
}

void SetbDisableABFlag(void)
{
	bDisableABFlag = true;
}
void ClearbDisableABFlag(void)
{
	bDisableABFlag = false;
}
bool GetbDisableABFlag(void)
{
	return bDisableABFlag;
}
static void ClearAllFlag(void)
{
	ClearX1Flag();
	ClearX2Flag();
	ClearY1Flag();
	ClearY2Flag();
}
static void CB_CancelBtn_Click(void *pComponentButton)
{
	ClearAllFlag();
	SetNextProc(ENP_FileList,EPFE_CrossFade);
}

// -----------------------------
static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
 
 if((Keys&KEY_A)!=0){
	 if(!bPressY1Flag)
	 {
		 if((bPressX1Flag) && (!bPressX2Flag))
		 {
			 //删除选中文件
			 FAT2_remove(CpyCutFileInfo.SrcFullPathAlias);
			 //ClearX1Flag();
			 SetFinishedOPFlag();
			 SetProgressVal(175);
			 SetClearAllFlgTrans();
		 }
	 }
	 else
	 {
		 if((!bPressX1Flag) && (bPressX2Flag))
		 {
			 //开始剪切文件到目的位置
			// FAT2_move(CpyCutFileInfo.SrcFullPathAlias,ConvertFull_Unicode2Alias(CpyCutFileInfo.DesPath,NULL),CpyCutFileInfo.DesPath,CpyCutFileInfo.FileName);
			 FAT2_move(CpyCutFileInfo.SrcFullPathAlias,ConvertFull_Unicode2Alias(CpyCutFileInfo.DesPath,NULL),CpyCutFileInfo.DesPath,CpyCutFileInfo.FileName);
			// ClearX2Flag();
			 SetFinishedOPFlag();
			 SetProgressVal(175);
		 }
		 if(bPressY2Flag)
		 {
			 SetbDisableABFlag();
			 //开始复制文件到目的位置
			 FAT2_Copy( CpyCutFileInfo.SrcFullPathAlias,CpyCutFileInfo.DesFullPathAlias,ConvertFull_Unicode2Alias(CpyCutFileInfo.DesPath,NULL));
			 //ClearY2Flag();
			 SetFinishedOPFlag();
			 ClearbDisableABFlag();
			 
			 
		 }
		 //ClearY1Flag();
	 }
	
	 
	// CB_CancelBtn_Click(NULL);
  }
  
  if((Keys&KEY_B)!=0){

	  CB_CancelBtn_Click(NULL);
  }
   if((Keys&(KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_DOWN))!=0){
    if((Keys&KEY_LEFT)!=0) 
    {}
    if((Keys&KEY_RIGHT)!=0) 
    {}
    if((Keys&KEY_DOWN)!=0)
    {}
    if((Keys&KEY_UP)!=0)
    {}
  }
   if((Keys&KEY_Y)!=0)
   {
	   
   }
   if((Keys&KEY_X)!=0)
   {
	   
   }
}

static void CB_MouseDown(s32 x,s32 y)
{
}

static void CB_MouseMove(s32 x,s32 y)
{
	
}

static void CB_MouseUp(s32 x,s32 y)
{
}

static CglCanvas *pMasterBG;
#include "mycartsrc/CBMPResource.h"
static void DrawOnlineHelp(void)
{
	
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	CglTGF * pbm2= CpyCutAlpha_GetSkin(ECCSA_CpyCutBG);
	pbm2->BitBlt(pTmpBM,0,0);
	ClearFinishedOPFlag();
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
}

static void DrawButtonBG(CglCanvas *pTmpBM,int x,int y,int w,int h)
{
	int _x;
	int _y;
	int _w;
	int _h;
	_x=x-2;
	_y=y-1;
	_w=w+4;
	_h=h+2;
	
	pTmpBM->SetColor(RGB15(28,28,28)|BIT(15));
	pTmpBM->DrawBox(_x,_y,_w-1,_h-1);
	//pTmpBM->DrawBox(_x+1,_y+1,_w-2,_h-2);
	
	pTmpBM->SetColor(RGB15(8,8,8)|BIT(15));
	pTmpBM->DrawLine(_x,_y+_h-2,_x+_w-1,_y+_h-2);
	//pTmpBM->DrawLine(_x+1,_y+_h-1,_x+_w,_y+_h-1);
	
	pTmpBM->DrawLine(_x+_w-2,_y,_x+_w-2,_y+_h-1);
	//pTmpBM->DrawLine(_x+_w-1,_y+1,_x+_w-1,_y+_h-1);	
	
	pTmpBM->SetFontTextColor(RGB15(0xa,0xa,0xa)|BIT(15));
}

static void UpdataCpyCut(void)
{
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(SYSColor1);//设置字体颜色黑色  
	
	CglTGF * pbm2= CpyCutAlpha_GetSkin(ECCSA_CpyCutBG);
	pbm2->BitBlt(pTmpBM,0,0);

	
	//文件
	 if((bPressX1Flag) && (!bPressX2Flag) && (!bPressY1Flag) &&  (!bPressY2Flag))
	 {
		 pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("FileOP_Del")); 
	 }
	 if((bPressY1Flag) && (bPressY2Flag) && (!bPressX1Flag) && (!bPressX2Flag) )
	 {
		 pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("FileOP_Cpy")); 
	 }
	 if((bPressY1Flag) && (bPressX2Flag) && (!bPressX1Flag) && (!bPressY2Flag))
	 {
		 pTmpBM->TextOutUTF8(x+2,y+2,Lang_GetUTF8("FileOP_Cut")); 
	 }
	 
	 pbm2= CpyCutAlpha_GetSkin(ECCSA_progrss_bar_bg);
	 pbm2->BitBlt(pTmpBM,x+8,y+2+20);
	 
	 pbm2= CpyCutAlpha_GetSkin(ECCSA_progress_bar);
	 u32 idxPro=0;
	 while(idxPro<ProgressValCur)
	 {
		 pbm2->BitBlt(pTmpBM,x+9+idxPro,y+2+20+1);
		 idxPro++;	  
	 }	 
	 if(!GetbDisableABFlag())
	 {
		 pTmpBM->TextOutUTF8(x+70,y+54-16,Lang_GetUTF8("FileOP_ButtonOK")); 	
		 pTmpBM->TextOutUTF8(x+70+38*2,y+54-16,Lang_GetUTF8("FileOP_ButtonCancel")); 		 
	 }
	 else
	 {
		 pTmpBM->SetFontTextColor(SYSColor2);
		 pTmpBM->TextOutUTF8(x+70,y+54-16,Lang_GetUTF8("FileOP_ButtonOK")); 	
		 pTmpBM->TextOutUTF8(x+70+38*2,y+54-16,Lang_GetUTF8("FileOP_ButtonCancel")); 		 
	 }
	 
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
	
	
	
	
	//操作完成退出
	if( GetFinishedOPFlag())
	{
		ClearFinishedOPFlag();
		ProgressValCur=0;
		CB_CancelBtn_Click(NULL);
	}
}

static void CB_Start(void)
{
   lcdMainOnTop();
   DrawOnlineHelp();
}

static void CB_VsyncUpdate(u32 VsyncCount)
{

	UpdataCpyCut();
}

static void CB_End(void)
{
  ProcState_RequestSave=true;
  ProcState_Save();
  
}
void UpdataProgress(void)
{
	UpdataCpyCut();
}
void ProcCpyCut_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}







