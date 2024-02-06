
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
#include "proc_romset_bin.h"
#include "..\proc_cheat\proc_Cheat.h"
#include "..\proc_FileList\ards.h"
extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}
//--------------------------

#define SYSColor1  ColorTable.ZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.ZV_SYSTEM.SYS_Color2

bool isPressA=false;
bool isPressB=false;
bool isPressRect11=false;
bool isPressRect12=false;
bool isPressRect13=false;
bool isPressRect14=false;
bool isPressRect15=false;
bool isPressRect16=false;


static u8 ItemIdx =0;
//TProcState *CurProcState;

const TRect  menurect11={190,18+(16+0)*1,30,19};//softreset
const TRect  menurect12={190,18+(16+0)*2,30,19};//realtimesave
const TRect  menurect13={190,18+(16+0)*3,30,19};//gameguide
const TRect  menurect14={190,18+(16+0)*4,30,19};//cheat

const TRect  menurect15={190,18+(16+0)*5,30,19};//downloadplay
const TRect  menurect17={190,18+(16+0)*6,30,19};//cleanmode
const TRect  menurect18={150,18+(16+0)*6,30,19};//speed

const TRect  menurect16={45,18+(16+0)*4,140,19};//open cheatwindows

const TRect  menurect00={60,18+(16+0)*7+2,68,24};//save
const TRect  menurect01={140,18+(16+0)*7+2,68,24};//cancel


static void CB_CancelBtn_Click(void *pComponentButton)
{
  SetNextProc(ENP_FileList,EPFE_CrossFade);
  ClearFlag();
}

// -----------------------------
static char pNDSAlis[256];
static bool isHomeBrew()
{

	  u8 header[16];
	  {
	    FAT_FILE *pf=FAT2_fopen_AliasForRead(pNDSAlis);
	    if(pf==NULL){
	      _consolePrintf("Fatal error: Can not open NDS file. [%s]\n");
	      ShowLogHalt();
	    }
	    FAT2_fread(header,1,16,pf);
	    FAT2_fclose(pf);
	  }
	  
	  char ID[5];
	  ID[0]=header[12+0];
	  ID[1]=header[12+1];
	  ID[2]=header[12+2];
	  ID[3]=header[12+3];
	  ID[4]=0;
	  
	  _consolePrintf("Detected ROMID: %s\n",ID);
	  
	  
	  if(strcmp("####",ID)==0) return true;
	  if(strcmp("PASS",ID)==0) return true;
	  if((ID[0]==0x3d)&&(ID[1]==0x84)&&(ID[2]==0x82)&&(ID[3]==0x0a)) return true;
	  return false;

}

static bool isUserCheatDataExits()
{
	const char *pfn;	  
	pfn=DefaultDataPath "/" CheatDataFilenme;
	if(FullPath_FileExistsAnsi(pfn))
		return true;
	else
		return false;
}
static NDSCheatInfo *FindSelectNDSCheatHeadInfo(void)
{//在库文件中寻找游戏金手指数据是否存在
	_consolePrintf("\n this");
	u32 gl_RomCRC;
	uint8 *bm=(uint8*)safemalloc(512);
	_consolePrintf("\n name=%s",CheatRomInfo.FullPathAlias);
	FAT_FILE *fh=FAT2_fopen_AliasForRead(CheatRomInfo.FullPathAlias);
	if(!fh)
	{
		FAT2_fclose(fh);
		if(bm)
		{
			safefree(bm);bm=NULL;
		}
		return NULL;
	}
	FAT2_fread(bm,512,1,fh);
	FAT2_fclose(fh);
	{
		 	//计算下CRC32
	 	gl_RomCRC=crc32(0,bm,512);
		gl_RomCRC = ~gl_RomCRC;
	}
	char sign[5];
	memset(sign,0,5);
	sign[0]=bm[12];
	sign[1]=bm[13];		
	sign[2]=bm[14];
	sign[3]=bm[15];
	if(bm)
	{
		safefree(bm);bm=NULL;
	}
	_consolePrintf("sign=%s \n",sign);	
	_consolePrintf("gl_RomCRC=%x \n",gl_RomCRC);	
	NDSCheatInfo *pInfo=FindNDSCheatInfo(sign,gl_RomCRC);	
	return pInfo;
}
static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
	TRomSetState *pCurRomSetState=&RomSetState;
 
 if((Keys&KEY_A)!=0){
	 isPressA=true;
	 CloseRomData();	 
	 CB_CancelBtn_Click(NULL);
  }
  
  if((Keys&KEY_B)!=0){
	  isPressB=true;	  
	  CB_CancelBtn_Click(NULL);
  }
   if((Keys&(KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_DOWN))!=0){
    if((Keys&KEY_LEFT)!=0) 
    {
    	switch(ItemIdx)
    	{
    		case 0:
    			{
    				if(pCurRomSetState->SoftReset)
    					pCurRomSetState->SoftReset=false;
    				else
    					pCurRomSetState->SoftReset=true;
    			}break;
    		case 1:
    			{
    				if(pCurRomSetState->RealtimeSave)
    					pCurRomSetState->RealtimeSave=false;
    				else
    					pCurRomSetState->RealtimeSave=true;    				
    			}break;
    		case 2:
    			{
    				if(pCurRomSetState->GameGuide)
    					pCurRomSetState->GameGuide=false;
    				else
    					pCurRomSetState->GameGuide=true;					
				}break;
    		case 3:
    			{
    				if(pCurRomSetState->CheatState)
    					pCurRomSetState->CheatState=false;
    				else
    					pCurRomSetState->CheatState=true;					
				}break;	
    		case 4:
    			{
    				if(pCurRomSetState->DownloadPlayState)
    					pCurRomSetState->DownloadPlayState=false;
    				else
    					pCurRomSetState->DownloadPlayState=true;					
				}break;	
    		case 5:
    			{
    				if(pCurRomSetState->SpeciaMode)
    					pCurRomSetState->SpeciaMode=false;
    				else
    					pCurRomSetState->SpeciaMode=true;					
				}break;	
		    default: 
    			break;
    	}    	
    }
    if((Keys&KEY_RIGHT)!=0) 
    {
    	switch(ItemIdx)
    	{
		case 0:
			{
				if(pCurRomSetState->SoftReset)
					pCurRomSetState->SoftReset=false;
				else
					pCurRomSetState->SoftReset=true;
			}break;
		case 1:
			{
				if(pCurRomSetState->RealtimeSave)
					pCurRomSetState->RealtimeSave=false;
				else
					pCurRomSetState->RealtimeSave=true;    				
			}break;
		case 2:
			{
				if(pCurRomSetState->GameGuide)
					pCurRomSetState->GameGuide=false;
				else
					pCurRomSetState->GameGuide=true;					
			}break;
		case 3:
			{
				if(pCurRomSetState->CheatState)
					pCurRomSetState->CheatState=false;
				else
					pCurRomSetState->CheatState=true;					
			}break;	
		case 4:
			{
				if(pCurRomSetState->DownloadPlayState)
					pCurRomSetState->DownloadPlayState=false;
				else
					pCurRomSetState->DownloadPlayState=true;					
			}break;	
		case 5:
			{
				if(pCurRomSetState->SpeciaMode)
					pCurRomSetState->SpeciaMode=false;
				else
					pCurRomSetState->SpeciaMode=true;	
			}break;	
		default: 
			break;
    	}     	
    }
    if((Keys&KEY_DOWN)!=0)
    {
    	if(ItemIdx==4)
    		ItemIdx=5;
    	if(ItemIdx==3)
    		ItemIdx=4;
    	if(ItemIdx==2)
			ItemIdx=3;
		if(ItemIdx==1)
			ItemIdx=2;
		if(ItemIdx==0)
			ItemIdx=1;     
    }
    if((Keys&KEY_UP)!=0)
    {
		if(ItemIdx==1)
			ItemIdx=0;
		if(ItemIdx==2)
			ItemIdx=1;
		if(ItemIdx==3)
			ItemIdx=2;
		if(ItemIdx==4)
			ItemIdx=3;
		if(ItemIdx==5)
			ItemIdx=4;
		
    }
  }
   if((Keys&KEY_Y)!=0){
	   if(pCurRomSetState->CheatState)
	   		{
	   			isPressRect16=false;
	   			DSIdx=dsi();
	   			strcpy(CheatRomInfo.FullPathAlias,RomSetInfo.FullPathAlias);
	   			memcpy(&RomSetStateTmp,&RomSetState,sizeof(TRomSetState));
	   			SetFlag();				 
				 
			 {
				strcpy(pNDSAlis,CheatRomInfo.FullPathAlias);		
				if(isHomeBrew() == true)
				{
				   //do nothing
				}
				else
				{//商业ROM启动代码	  
				   	//打开金手指窗口					
  
					 if(isUserCheatDataExits())
					 {
						 NDSCheatInfo *pInfo=FindSelectNDSCheatHeadInfo();	
						 if(pInfo)
						 {
							 safefree(pInfo);
							 SetNextProc(ENP_Cheat,EPFE_CrossFade);
						 	
						 }
					 }				
				}	     
			 }
	   		}
   }
}

static void CB_MouseDown(s32 x,s32 y)
{
	TRomSetState *pCurRomSetState=&RomSetState;
	if(isInsideRect(menurect11,x,y)==true)
	{
		{
			if(pCurRomSetState->SoftReset)
				pCurRomSetState->SoftReset=false;
			else
				pCurRomSetState->SoftReset=true;
			ItemIdx=0;
		}
	}
	if(isInsideRect(menurect12,x,y)==true)
	{
		{
			if(pCurRomSetState->RealtimeSave)
				pCurRomSetState->RealtimeSave=false;
			else
				pCurRomSetState->RealtimeSave=true; 
			ItemIdx=1;
		}	
	}
	if(isInsideRect(menurect13,x,y)==true)
	{
		{
			if(pCurRomSetState->GameGuide)
				pCurRomSetState->GameGuide=false;
			else
				pCurRomSetState->GameGuide=true;					
			ItemIdx=2;
		}
		
	}
	if(isInsideRect(menurect14,x,y)==true)
	{	
		{
			if(pCurRomSetState->CheatState)
				pCurRomSetState->CheatState=false;
			else
				pCurRomSetState->CheatState=true;					
			ItemIdx=3;
		}
		
	}
	if(isInsideRect(menurect15,x,y)==true)
	{	
		{
			if(pCurRomSetState->DownloadPlayState)
				pCurRomSetState->DownloadPlayState=false;
			else
				pCurRomSetState->DownloadPlayState=true;					
			ItemIdx=4;
		}
		
	}
	if(isInsideRect(menurect16,x,y)==true)
	{	
		if(pCurRomSetState->CheatState)
		{
			isPressRect16=false;
			
		}		
	}
	if(isInsideRect(menurect17,x,y)==true)
	{	
		{
			if(pCurRomSetState->SpeciaMode)
				pCurRomSetState->SpeciaMode=false;
			else
				pCurRomSetState->SpeciaMode=true;					
			ItemIdx=5;
		}
		
	}
	if(isInsideRect(menurect18,x,y)==true)
	{	
		{
			if(pCurRomSetState->SpeciaMode)
			{
				pCurRomSetState->Speed += 1;
				if(pCurRomSetState->Speed == 11)
					pCurRomSetState->Speed = 0;
			}					
		}
		
	}
	if(isInsideRect(menurect00,x,y)==true)
	{
		 isPressA=true; 
	}
	if(isInsideRect(menurect01,x,y)==true)
	{
	 	isPressB=true;
	}	
}

static void CB_MouseMove(s32 x,s32 y)
{
	
}

static void CB_MouseUp(s32 x,s32 y)
{
	TRomSetState *pCurRomSetState=&RomSetState;
	if(isInsideRect(menurect11,x,y)==true)
	{

	}
	if(isInsideRect(menurect12,x,y)==true)
	{

	}
	if(isInsideRect(menurect13,x,y)==true)
	{

	}
	if(isInsideRect(menurect14,x,y)==true)
	{

	}
	if(isInsideRect(menurect16,x,y)==true)
	{
		if(pCurRomSetState->CheatState)
		{
			isPressRect16=false;
			DSIdx=dsi();
			strcpy(CheatRomInfo.FullPathAlias,RomSetInfo.FullPathAlias);
   			memcpy(&RomSetStateTmp,&RomSetState,sizeof(TRomSetState));
   			SetFlag();
			{

				strcpy(pNDSAlis,CheatRomInfo.FullPathAlias);		
				if(isHomeBrew() == true)
				{
				   //do nothing
				}
				else
				{//商业ROM启动代码	  
				   	//打开金手指窗口					
					 if(isUserCheatDataExits())
					 {
						 NDSCheatInfo *pInfo=FindSelectNDSCheatHeadInfo();	
						 if(pInfo)
						 {
							 safefree(pInfo);
							 isPressRect16=false;
							 SetNextProc(ENP_Cheat,EPFE_CrossFade);
						 	
						 }
					 }	
					 else isPressRect16=true;
				}	     
			 
			}
		}
	}
	if(isInsideRect(menurect00,x,y)==true)
	{
		 isPressA=false; 
		 CloseRomData();
		 CB_CancelBtn_Click(NULL);
	}
	if(isInsideRect(menurect01,x,y)==true)
	{
	 	isPressB=false;
	 	CB_CancelBtn_Click(NULL);
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
extern void TestPauseKey(void);
static void DrawOnlineHelp(void)
{
	
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
   //gausscolor50bk(pTmpBM);
	CglTGF * pbm2= RomSetAlpha_GetSkin(ERSSA_RomSetBG);
	pbm2->BitBlt(pTmpBM,0,0);
	OpenRomData();	
	_consolePrintf("\n Here8!");
	VRAMWriteCache_Enable();
	_consolePrintf("\n Here9!");
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	_consolePrintf("\n Here10!");
	VRAMWriteCache_Disable();
	_consolePrintf("\n Here11!");
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
	isPressA=false;
	isPressB=false;
	isPressRect16=false;
	
	_consolePrintf("\n Here12!");
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
	
	pTmpBM->SetFontTextColor(SYSColor2);
}

static void UpdataRomSet(void)
{
	TRomSetState *pCurRomSetState=&RomSetState;
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(SYSColor1);//设置字体颜色黑色  
	
	CglTGF * pbm2= RomSetAlpha_GetSkin(ERSSA_RomSetBG);
	pbm2->BitBlt(pTmpBM,0,0);
	
	pTmpBM->TextOutUTF8(45,18+2,Lang_GetUTF8("ROMSET_Menu"));	
	

	pTmpBM->TextOutUTF8(45,18+(16+0)*1+2,Lang_GetUTF8("ROMSET_SoftReset"));
	if(ItemIdx == 0)
	{
		/*pTmpBM->SetColor(RGB15(31,31,31)|BIT(15));
		pTmpBM->DrawBox(190,18+(16+0)*1,28,17);
		pTmpBM->DrawBox(190+1,18+(16+0)*1+1,28-2,17-2);
		pTmpBM->SetColor(RGB15(0,0,0)|BIT(15));
		pTmpBM->DrawLine(190,18+(16+0)*1+17,190+28,18+(16+0)*1+17);
		pTmpBM->DrawLine(190,18+(16+0)*1+17-1,190+28,18+(16+0)*1+17-1);
		pTmpBM->DrawLine(190+28,18+(16+0)*1,190+28,18+(16+0)*1+17);
		pTmpBM->DrawLine(190+28-1,18+(16+0)*1,190+28-1,18+(16+0)*1+17);			
*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*1,26,17);
		//pTmpBM->SetFontTextColor(0xFFFF);
	}
	if(pCurRomSetState->SoftReset)
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*1+2,Lang_GetUTF8("ROMSET_SoftResetV1"));	
	}
	else
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*1+2,Lang_GetUTF8("ROMSET_SoftResetV2"));	
	}
	pTmpBM->SetFontTextColor(SYSColor1);
	
	pTmpBM->TextOutUTF8(45,18+(16+0)*2+2,Lang_GetUTF8("ROMSET_RealtimeSave"));
	if(ItemIdx == 1)
	{
		/*pTmpBM->SetColor(RGB15(0x00,0x00,0x1f)|BIT(15));
		pTmpBM->FillBox(190,18+(16+0)*2,25,17);
		pTmpBM->SetFontTextColor(0xFFFF);	*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*2,26,17);
	}
	if(pCurRomSetState->RealtimeSave)
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*2+2,Lang_GetUTF8("ROMSET_RealtimeSaveV1"));	
	}
	else
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*2+2,Lang_GetUTF8("ROMSET_RealtimeSaveV2"));	
	}
	pTmpBM->SetFontTextColor(SYSColor1);
	
	
	pTmpBM->TextOutUTF8(45,18+(16+0)*3+2,Lang_GetUTF8("ROMSET_GameGuide"));
	if(ItemIdx == 2)
	{
		/*pTmpBM->SetColor(RGB15(0x00,0x00,0x1f)|BIT(15));
		pTmpBM->FillBox(190,18+(16+0)*3,25,17);
		pTmpBM->SetFontTextColor(0xFFFF);*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*3,26,17);
	}
	if(pCurRomSetState->GameGuide)
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*3+2,Lang_GetUTF8("ROMSET_GameGuideV1"));	
	}
	else
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*3+2,Lang_GetUTF8("ROMSET_GameGuideV2"));	
	}
	pTmpBM->SetFontTextColor(SYSColor1);
	
	//pTmpBM->TextOutUTF8(45,18+(16+0)*4+2,Lang_GetUTF8("ROMSET_Cheat"));
	if(ItemIdx == 3)
	{
		/*pTmpBM->SetColor(RGB15(0x00,0x00,0x1f)|BIT(15));
		pTmpBM->FillBox(190,18+(16+0)*4,25,17);
		pTmpBM->SetFontTextColor(0xFFFF);*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*4,26,17);
	}
	if(pCurRomSetState->CheatState)
	{
		if(!isPressRect16)
		{
			DrawButtonBG(pTmpBM,45,18+(16+0)*4,137,17);
			pTmpBM->TextOutUTF8(45,18+(16+0)*4+2,Lang_GetUTF8("ROMSET_OpenCheatWin"));
		}
		pTmpBM->TextOutUTF8(190,18+(16+0)*4+2,Lang_GetUTF8("ROMSET_CheatV1"));	
	}
	else
	{
		isPressRect16=false;
		pTmpBM->TextOutUTF8(45,18+(16+0)*4+2,Lang_GetUTF8("ROMSET_Cheat"));
		pTmpBM->TextOutUTF8(190,18+(16+0)*4+2,Lang_GetUTF8("ROMSET_CheatV2"));	
	}	
	pTmpBM->SetFontTextColor(SYSColor1);
	
	pTmpBM->TextOutUTF8(45,18+(16+0)*5+2,Lang_GetUTF8("ROMSET_DownloadPlay"));
	if(ItemIdx == 4)
	{
		/*pTmpBM->SetColor(RGB15(0x00,0x00,0x1f)|BIT(15));
		pTmpBM->FillBox(190,18+(16+0)*4,25,17);
		pTmpBM->SetFontTextColor(0xFFFF);*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*5,26,17);
	}
	if(pCurRomSetState->DownloadPlayState)
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*5+2,Lang_GetUTF8("ROMSET_DownloadPlayV1"));	
	}
	else
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*5+2,Lang_GetUTF8("ROMSET_DownloadPlayV2"));	
	}	
	pTmpBM->SetFontTextColor(SYSColor1);
	pTmpBM->TextOutUTF8(45,18+(16+0)*6+2,Lang_GetUTF8("ROMSET_CleanMode"));
	if(ItemIdx == 5)
	{
		/*pTmpBM->SetColor(RGB15(0x00,0x00,0x1f)|BIT(15));
		pTmpBM->FillBox(190,18+(16+0)*4,25,17);
		pTmpBM->SetFontTextColor(0xFFFF);*/
		DrawButtonBG(pTmpBM,190,18+(16+0)*6,26,17);
	}
	if(pCurRomSetState->SpeciaMode)
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*6+2,Lang_GetUTF8("ROMSET_CleanModeV1"));
		char string[5]={0};
		sprintf(string, "%d ",pCurRomSetState->Speed); 
		pTmpBM->TextOutUTF8(160,18+(16+0)*6+2,string);
		DrawButtonBG(pTmpBM,150,18+(16+0)*6,26,17);
	}
	else
	{
		pTmpBM->TextOutUTF8(190,18+(16+0)*6+2,Lang_GetUTF8("ROMSET_CleanModeV2"));	
	}
	pTmpBM->SetFontTextColor(SYSColor1);	
	
	if(!pCurRomSetState->CheatState)
	{
		pTmpBM->SetFontTextColor(SYSColor2);	
	}
	if(isPressRect16)
	{
		pTmpBM->SetFontTextColor(SYSColor2);
		//DrawButtonBG(pTmpBM,45,18+(16+0)*4,137,19);
		pTmpBM->TextOutUTF8(45,18+(16+0)*4,"no cheat");	
	}	
	//pTmpBM->TextOutUTF8(45,18+(16+0)*6+2,Lang_GetUTF8("ROMSET_OpenCheatWin"));	
	pTmpBM->SetFontTextColor(SYSColor1);
	if(isPressA)
	{
		pTmpBM->SetFontTextColor(SYSColor2);
		DrawButtonBG(pTmpBM,60,18+(16+0)*7,60,19);
	}
	pTmpBM->TextOutUTF8(60,18+(16+0)*7+2,Lang_GetUTF8("ROMSET_Save"));	
	pTmpBM->SetFontTextColor(SYSColor1);
	
	if(isPressB)
	{
		pTmpBM->SetFontTextColor(SYSColor2);	
		DrawButtonBG(pTmpBM,140,18+(16+0)*7,60,19);
	}
	pTmpBM->TextOutUTF8(140,18+(16+0)*7+2,Lang_GetUTF8("ROMSET_Cancel"));	
	pTmpBM->SetFontTextColor(SYSColor1);	
	
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
	
}

static void CB_Start(void)
{
   lcdMainOnTop();
   DrawOnlineHelp(); 
}

static void CB_VsyncUpdate(u32 VsyncCount)
{

	UpdataRomSet();
}

static void CB_End(void)
{
  ProcState_RequestSave=true;
  ProcState_Save();
  
}

void ProcRomSet_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}

