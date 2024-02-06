
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
#include "proc_SetupW.h"
#include "../../mycartsrc/MartCartop.h"
#include "SkinDirListFile.h"


extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}
//--------------------------
extern void LangInitAndLoadFontExt(void);

static bool isPressA=false;
static bool isPressB=false;


static u8 ItemIdx =0;
TProcState *CurProcState;


#define SYSColor1  ColorTable.ZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.ZV_SYSTEM.SYS_Color2

#define cpy_x (123+40)

const char *VerName={"V1.05"};

const TRect  menurect11={cpy_x,45+(16+4)+12,52,19};
const TRect  menurect12={cpy_x,45+(16+4)*2+8,52,19};
const TRect  menurect13={cpy_x,45+(16+4)*3+4,52,19};
const TRect  menurect14={cpy_x,45+(16+4)*4,52,19};

const TRect  menurect00={37+37*1,140,68,24};
const TRect  menurect01={37+37*3,140,68,24};

const TRect  menurectX={17,42,18,18};

static u8 LanNum=0;

static bool bFlag3;

static bool bKoreanFlg;
static bool bFrenchFlg;

TSetupRom SetupRom;

void Switch_skin()
{
	static UnicodeChar DefaultFilenameW[64];
	char pp[256];   
	memset(pp,0,256);
	snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);
	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
	Skin_SetFilename(DefaultFilenameW);
	
	////////保留皮肤的文件名
    const char* pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/fpath.bin");        
    FAT_FILE* FileHandle2 = FAT2_fopen_AliasForWrite(pfullalias);
    if(FileHandle2 != NULL)
    {
    	FAT2_fseek(FileHandle2,0,SEEK_SET);
    	FAT2_fwrite(pp,1,0x100,FileHandle2);
    	FAT2_fclose(FileHandle2);
    } 
}
///////////////////////////////////////////
void SetFlag3()
{
	bFlag3=true;
}

void ClearFlag3()
{
	bFlag3=false;
}
bool GetFlag3()
{
	return(bFlag3);
}

void SetKoreanFlg()
{
	bKoreanFlg=true;
}

void ClearKoreanFlg()
{
	bKoreanFlg=false;
}
bool GetKoreanFlg()
{
	return(bKoreanFlg);
}

void SetFrenchFlg()
{
	bFrenchFlg=true;
}

void ClearFrenchFlg()
{
	bFrenchFlg=false;
}
bool GetFrenchFlg()
{
	return(bFrenchFlg);
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

static void CB_CancelBtn_Click(void *pComponentButton)
{
	extern bool SetupWisComefromMenu;
	if(SetupWisComefromMenu) 
	{
		SetNextProc(ENP_ReMenu,EPFE_CrossFade);
	}
	else
		SetNextProc(ENP_FileList,EPFE_CrossFade);
}

// -----------------------------

static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
 
 if((Keys&KEY_A)!=0){
	isPressA=true;
	ProcState_RequestSave	= true;
	ProcState_Save();
	CB_CancelBtn_Click(NULL);
  }
  
  if((Keys&KEY_B)!=0){
	isPressB=true;
	CB_CancelBtn_Click(NULL);
  }
  LanNum = MulLangListFileNum-1;
  if((Keys&(KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_DOWN))!=0){
    if((Keys&KEY_LEFT)!=0) 
    {
    	switch(ItemIdx)
    	{
    		case 0:
    			{
    				if(CurProcState->SetupW.WhichL>0)
					{
						CurProcState->SetupW.WhichL--;
						Lang_Free();
						Lang_Load();						    						
					}
    				else
    				{
    					CurProcState->SetupW.WhichL=LanNum;
						Lang_Free();
						Lang_Load();
    				}
    			}break;
    		case 1:
    			{
    				if(CurProcState->SetupW.Rumble>0)
    				{
    					CurProcState->SetupW.Rumble --;
    					Enable_Arm9DS();
    					switch(CurProcState->SetupW.Rumble)
						{
							case 1:
								SetShake(0xF0);
								_consolePrintf("Set Shake 0xF0\n");
								break;
							case 2:
								SetShake(0xF1);
								_consolePrintf("Set Shake 0xF1\n");
								break;
							case 3:
								SetShake(0xF2);
								_consolePrintf("Set Shake 0xF2\n");
								break;
							default:
								SetShake(0xF0);
								_consolePrintf("Set Shake 0xF0\n");
								break;
						}

    				}
    				else
    				{
    					CurProcState->SetupW.Rumble =2;
						Enable_Arm9DS();
						SetShake(0xf2);
    				}
    			}break;
    		case 2:
    			{
					if(CurProcState->SetupW.M3in1Mode==true)
					{
						Enable_Arm9DS();
						SetShake(0x08);
						CurProcState->SetupW.M3in1Mode =false;
					}
					else
					{	
						Enable_Arm9DS();
						SetShake(0xF0);
						CurProcState->SetupW.M3in1Mode =true;					
					}
				}break;
    		case 3:
    			{
    				if(CurProcState->SetupW.SkinSelect>0)
    				{
    					CurProcState->SetupW.SkinSelect --;
    					/*static UnicodeChar DefaultFilenameW[64];
    					char pp[256];    					        	
			        	snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);
			        	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
			        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
    					Skin_SetFilename(DefaultFilenameW);*/
    					Switch_skin();
    				}
    				else
    				{
    					CurProcState->SetupW.SkinSelect = ListFileNum -1 ;
    					/*static UnicodeChar DefaultFilenameW[64];
    					char pp[256];    					        	
    					snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);
    					StrConvert_Ank2Unicode(pp,DefaultFilenameW);
			        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
    					Skin_SetFilename(DefaultFilenameW);*/
    					Switch_skin();
    				}
				}break;
    	}
    }
    if((Keys&KEY_RIGHT)!=0) 
    {
    	switch(ItemIdx)
    	{
    		case 0:
    			{
    				if(CurProcState->SetupW.WhichL<LanNum)
					{
						CurProcState->SetupW.WhichL++;
						Lang_Free();
						Lang_Load();
					}
    				else
    				{
						CurProcState->SetupW.WhichL=0;
						Lang_Free();
						Lang_Load();
    				}
    			}break;
    		case 1:
    			{
    				if(CurProcState->SetupW.Rumble<2)
    				{
    					CurProcState->SetupW.Rumble ++;
    					Enable_Arm9DS();
    					switch(CurProcState->SetupW.Rumble)
						{
							case 1:
								SetShake(0xF0);
								_consolePrintf("Set Shake 0xF0\n");
								break;
							case 2:
								SetShake(0xF1);
								_consolePrintf("Set Shake 0xF1\n");
								break;
							case 3:
								SetShake(0xF2);
								_consolePrintf("Set Shake 0xF2\n");
								break;
							default:
								SetShake(0xF0);
								_consolePrintf("Set Shake 0xF0\n");
								break;
						}

    				}
    				else
    				{
    					CurProcState->SetupW.Rumble =0;
    					Enable_Arm9DS();
    					SetShake(0xF0);
    				}
    				
    			}break;
    		case 2:
    			{
					if(CurProcState->SetupW.M3in1Mode==false)
					{	
						Enable_Arm9DS();
						SetShake(0xF0);
						CurProcState->SetupW.M3in1Mode =true;
						
					}
					else
					{
						Enable_Arm9DS();
						SetShake(0x08);
						CurProcState->SetupW.M3in1Mode =false;
					}
    				
				}break;
    		case 3:
    			{
    				if(CurProcState->SetupW.SkinSelect< ListFileNum-1)
    				{
    					CurProcState->SetupW.SkinSelect ++;
    					/*static UnicodeChar DefaultFilenameW[64];
    					char pp[256];    					        	
    					snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);	

			        	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
			        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
    					Skin_SetFilename(DefaultFilenameW);*/
    					Switch_skin();
    				}
    				else
    				{
    					CurProcState->SetupW.SkinSelect =0;
    					/*static UnicodeChar DefaultFilenameW[64];
    					char pp[256];    					        	
    					snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);	

			        	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
			        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
    					Skin_SetFilename(DefaultFilenameW);*/
    					Switch_skin();
    				}
    				
    			}break;	
    	}
    }
    if((Keys&KEY_DOWN)!=0){
      if(ItemIdx==2)
  	      ItemIdx=3;
      if(ItemIdx==1)
    	  ItemIdx=2;
      if(ItemIdx==0)
          ItemIdx=1;           
    }
    if((Keys&KEY_UP)!=0){
        if(ItemIdx==1)
      	  ItemIdx=0;
        if(ItemIdx==2)
            ItemIdx=1;  
        if(ItemIdx==3)
            ItemIdx=2;
    }
  }
}

static void CB_MouseDown(s32 x,s32 y)
{
	if(isInsideRect(menurect11,x,y)==true)
	{
		ItemIdx=0;
	}
	if(isInsideRect(menurect12,x,y)==true)
	{
		ItemIdx=1;		
	}
	if(isInsideRect(menurect13,x,y)==true)
	{
		ItemIdx=2;
	}
	if(isInsideRect(menurect14,x,y)==true)
	{
		ItemIdx=3;
	}
	if(isInsideRect(menurect00,x,y)==true)
	{
		 isPressA=true;
		 ProcState_RequestSave	= true;
		 ProcState_Save();
		   
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
	LanNum = MulLangListFileNum-1;
	CurProcState = &ProcState;
	if(isInsideRect(menurect11,x,y)==true)
	{
		if(CurProcState->SetupW.WhichL>0)
		{
			CurProcState->SetupW.WhichL--;
			ItemIdx=0;
			Lang_Free();
			Lang_Load();
		}
		else
		{
			CurProcState->SetupW.WhichL=LanNum;
			ItemIdx=0;
			Lang_Free();
			Lang_Load();
		}
	}
	if(isInsideRect(menurect12,x,y)==true)
	{

		if(CurProcState->SetupW.Rumble<2)
		{
			CurProcState->SetupW.Rumble ++;
			ItemIdx=1;
			Enable_Arm9DS();
			switch(CurProcState->SetupW.Rumble)
			{
				case 1:
					SetShake(0xF0);
					_consolePrintf("Set Shake 0xF0\n");
					break;
				case 2:
					SetShake(0xF1);
					_consolePrintf("Set Shake 0xF1\n");
					break;
				case 3:
					SetShake(0xF2);
					_consolePrintf("Set Shake 0xF2\n");
					break;
				default:
					SetShake(0xF0);
					_consolePrintf("Set Shake 0xF0\n");
					break;
			}
		}
		else
		{
			CurProcState->SetupW.Rumble =0;
			ItemIdx=1;
			Enable_Arm9DS();
			SetShake(0xF0);
		}
	}
	if(isInsideRect(menurect13,x,y)==true)
	{
		if(CurProcState->SetupW.M3in1Mode==false)
		{
			CurProcState->SetupW.M3in1Mode =true;
			Enable_Arm9DS();
			SetShake(0xf0);
			ItemIdx=2;
		}
		else
		{
			CurProcState->SetupW.M3in1Mode =false;
			Enable_Arm9DS();
			SetShake(0x08);
			ItemIdx=2;
		}
		//CurProcState->SetupW.CheatOnOff=false;
	}
	if(isInsideRect(menurect14,x,y)==true)
	{

		if(CurProcState->SetupW.SkinSelect<ListFileNum-1)
		{
			CurProcState->SetupW.SkinSelect ++;
			ItemIdx=3;
			/*static UnicodeChar DefaultFilenameW[64];
			char pp[256];    					        	
			snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);	

        	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
			Skin_SetFilename(DefaultFilenameW);*/
			Switch_skin();
		}
		else
		{
			CurProcState->SetupW.SkinSelect =0;
			ItemIdx=3;
			/*static UnicodeChar DefaultFilenameW[64];
			char pp[256];    					        	
			snprintf(pp,256,"/system/skin/%s",ListFile[CurProcState->SetupW.SkinSelect].Filename);	

        	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
        	//StrConvert_Ank2Unicode(DefaultDataPath "/" SkinFileName1,DefaultFilenameW);
			Skin_SetFilename(DefaultFilenameW);*/
			Switch_skin();
		}
	}
	/*if(isInsideRect(menurect22,x,y)==true)
	{	
		if(CurProcState->SetupW.CheatOnOff==false)
		{
			CurProcState->SetupW.CheatOnOff=true;
			isPressRect22=false;
		}
		else
		{
			CurProcState->SetupW.CheatOnOff=false;
			isPressRect22=false;
		}
		//CurProcState->SetupW.CheatOnOff=false;
	}
	if(isInsideRect(menurect31,x,y)==true)
	{
		if(CurProcState->SetupW.SoftReset==false)
		{
			CurProcState->SetupW.SoftReset=true;
			isPressRect31=false;
		}
		else
		{
			CurProcState->SetupW.SoftReset=false;
			isPressRect31=false;			
		}
		//CurProcState->SetupW.SoftReset==false;
	}
	if(isInsideRect(menurect32,x,y)==true)
	{
		if(CurProcState->SetupW.SoftReset==true)
		{
			CurProcState->SetupW.SoftReset=false;
			isPressRect32=false;
		}
		else
		{
			CurProcState->SetupW.SoftReset=true;
			isPressRect32=false;
		}
		//CurProcState->SetupW.SoftReset==false;
	}*/
	if(isInsideRect(menurect00,x,y)==true)
	{
		 isPressA=false;
		 CB_CancelBtn_Click(NULL);
	}
	if(isInsideRect(menurect01,x,y)==true)
	{
	 	isPressB=false;
	 	CB_CancelBtn_Click(NULL);
	}	
	if(isInsideRect(menurectX,x,y)==true)
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
extern bool SetupWisComefromMenu;
static void DrawOnlineHelp(void)
{

	CurProcState = &ProcState;
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	//gausscolor50bk(pTmpBM);
	CBMPResource *pbmpres=NULL;
	if(SetupWisComefromMenu) 
	{
		pbmpres = new CBMPResource(DefaultDataPath"/boot/load_setting.bmp");
	    u16 *pbmp = pbmpres->Get16bitData();    
	    u16 *pv = pTmpBM->GetVRAMBuf();
	    dmaCopy(pbmp,pv,ScreenWidth*ScreenHeight*2);
	}
	else
	{
		CglTGF * pbm2= SetupWAlpha_GetSkin(ESWSA_SetupWBG);
		pbm2->BitBlt(pTmpBM,0,0);
	}
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(SYSColor1);//设置字体颜色黑色  
    
	//pbm2= ScrollBarAlpha_GetSkin(EFLSA_Icon_sun);
   // pbm2->BitBlt(pTmpBM,0,0);
    
   // pbm2= ScrollBarAlpha_GetSkin(EFLSA_Icon_reset0);
  //  pbm2->BitBlt(pTmpBM,240,0); 
    
    pTmpBM->TextOutUTF8(37+2,45+16,Lang_GetUTF8("SET_Menu"));
   
    pTmpBM->TextOutUTF8(37+2,45+(16+4)+2+12,Lang_GetUTF8("SET_Language")); 
    /*pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    pbm2->BitBlt(pTmpBM,123,45+(16+4));    
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    pbm2->BitBlt(pTmpBM,202,45+(16+4));   
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4)); */

   /* switch(CurProcState->SetupW.WhichL)
    {
    	case 0: pTmpBM->TextOutA(150+16+2,45+2,"SChinese");break;
    	case 1: pTmpBM->TextOutA(150+16+2,45+2,"English");break;
    	case 2: pTmpBM->TextOutA(150+16+2,45+2,"Japan");break;
    	default:
    		break;
    }*/
 

    pTmpBM->TextOutUTF8(37+2,45+(16+4)*2+2+8,Lang_GetUTF8("SET_Rumble"));
  /*  pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    pbm2->BitBlt(pTmpBM,123,45+(16+4)*2);    
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    pbm2->BitBlt(pTmpBM,202,45+(16+4)*2);   
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4)*2); 
    if(CurProcState->SetupW.CheatOnOff==false)
    	//pTmpBM->TextOutA(cpy_x+2,45+(16+4)*2+2,"Disable");
   	 pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*2+2,Lang_GetUTF8("SET_RumbleV1"));
    else
       // pTmpBM->TextOutA(cpy_x+2,45+(16+4)*2+2,"Enable");
   	 pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*2+2,Lang_GetUTF8("SET_RumbleV2"));*/
   
    pTmpBM->TextOutUTF8(37+2,45+(16+4)*3+2+4,Lang_GetUTF8("SET_3in1Mode"));
  /*  pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    pbm2->BitBlt(pTmpBM,123,45+(16+4)*3);    
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    pbm2->BitBlt(pTmpBM,202,45+(16+4)*3);   
    pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4)*3); 	
    if(CurProcState->SetupW.SoftReset==false)
   // 	pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Disable");
    	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*3+2,Lang_GetUTF8("SET_3in1ModeV1"));
    else
    //    pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Enable");
    	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*3+2,Lang_GetUTF8("SET_3in1ModeV1"));
   
    /*pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
    pbm2->BitBlt(pTmpBM,37+37*1,140);
    pbm2->BitBlt(pTmpBM,37+37*3,140); */    
    pTmpBM->TextOutUTF8(37+2,45+(16+4)*4+2,Lang_GetUTF8("SET_Skin"));
    
    pTmpBM->TextOutUTF8_StartMid(37+37*1,140,68,Lang_GetUTF8("SET_ButtonOK"));
    pTmpBM->TextOutUTF8_StartMid(37+37*3,140,68,Lang_GetUTF8("SET_ButtonCancel")); 
    isPressA=false;
    isPressB=false;
     
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
	if(pbmpres!=NULL)
	{
	    delete pbmpres;pbmpres=NULL;
	}
}
bool is_read = false;
uint32 num=0;
static void UpdataSetupW(void)
{
	CurProcState = &ProcState;
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	CBMPResource *pbmpres=NULL;
	if(SetupWisComefromMenu) 
	{
		pbmpres = new CBMPResource(DefaultDataPath"/boot/load_setting.bmp");
	    u16 *pbmp = pbmpres->Get16bitData();    
	    u16 *pv = pTmpBM->GetVRAMBuf();
	    dmaCopy(pbmp,pv,ScreenWidth*ScreenHeight*2);
	}
	else
	{
		CglTGF * pbm2= SetupWAlpha_GetSkin(ESWSA_SetupWBG);
		pbm2->BitBlt(pTmpBM,0,0);	
	}
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(SYSColor1);//设置字体颜色黑色  
    
	//pbm2= ScrollBarAlpha_GetSkin(EFLSA_Icon_sun);
   // pbm2->BitBlt(pTmpBM,0,0);
    
  //  pbm2= ScrollBarAlpha_GetSkin(EFLSA_Icon_reset0);
  //  pbm2->BitBlt(pTmpBM,240,0); 
    pTmpBM->TextOutUTF8(37+2,45+16,Lang_GetUTF8("SET_Menu"));
    pTmpBM->TextOutA(35+100+40,45,VerName);
	//uint32 num;
    if(!is_read)
    {
    	num = Mart_ReadFirmwareVer();
    	is_read = true;
    }
	
	char HardWare[50];
	char FirmWare[50];
	memset(HardWare,0x00,50);
	memset(FirmWare,0x00,50);
	
	if(num==0x11111111) 
	{
		strcpy(FirmWare,"F[1111]");
		strcpy(HardWare,"H[1111]");
	}
	else if((num&0x000000FF)!=0x00000005)
	{
		strcpy(HardWare,"mat");
	}
	else
	{
		sprintf(FirmWare,"F[%02d]",(num & 0xff000000)>>24);
		sprintf(HardWare,"H[%03x]",(num & 0x0000ffff));
	}	
	pTmpBM->TextOutA(37,45,"VER:");
	pTmpBM->TextOutA(68,45,HardWare);
	pTmpBM->TextOutA(37+90,45,FirmWare);
   
    pTmpBM->TextOutUTF8(37+2,45+(16+4)+2+12,Lang_GetUTF8("SET_Language")); 
    /*  
        
    if(!isPressRect11)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    else 
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET1);
    pbm2->BitBlt(pTmpBM,123,45+(16+4));     
    if(!isPressRect12)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    else
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT1);    	
    pbm2->BitBlt(pTmpBM,202,45+(16+4));  
    if(ItemIdx != 0)
	{
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
	}
    else
	{
		pbm2 = SetupWAlpha_GetSkin(ESWSA_SelectOptionBG);
		    pTmpBM->SetFontTextColor(RGB15(31,31,31) | BIT(15));
	}
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4));  
*/
   /* switch(CurProcState->SetupW.WhichL)
    {
    	case 0: pTmpBM->TextOutA(150+16+2,45+2,"SChinese");break;
    	case 1: pTmpBM->TextOutA(150+16+2,45+2,"English");break;
    	case 2: pTmpBM->TextOutA(150+16+2,45+2,"Japan");break;
    	default:
    		break;
    }*/
    if(ItemIdx == 0)
       	DrawButtonBG(pTmpBM,cpy_x,45+(16+4)+12,52,17);
	pTmpBM->TextOutA(cpy_x+2,45+(16+4)+2+12,MulLangListFile[CurProcState->SetupW.WhichL].DispName);

    pTmpBM->SetFontTextColor(SYSColor1);
    pTmpBM->TextOutUTF8(37+2,45+(16+4)*2+2+8,Lang_GetUTF8("SET_Rumble"));
   /* if(!isPressRect21)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    else
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET1);
    pbm2->BitBlt(pTmpBM,123,45+(16+4)*2);    
    if(!isPressRect22)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    else
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT1);
    pbm2->BitBlt(pTmpBM,202,45+(16+4)*2);   
    if(ItemIdx != 1)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
    else
    	{
    		pbm2 = SetupWAlpha_GetSkin(ESWSA_SelectOptionBG);
    		pTmpBM->SetFontTextColor(RGB15(31,31,31) | BIT(15));
    	}
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4)*2); */
    if(ItemIdx == 1)
       	DrawButtonBG(pTmpBM,cpy_x,45+(16+4)*2+8,52,17);
    if(CurProcState->SetupW.Rumble==0)
    	//pTmpBM->TextOutA(cpy_x+2,45+(16+4)*2+2,"Disable");
    	 pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*2+2+8,Lang_GetUTF8("SET_RumbleV3"));
    else if(CurProcState->SetupW.Rumble==1)
   		//pTmpBM->TextOutA(cpy_x+2,45+(16+4)*2+2,"Enable");
   	 	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*2+2+8,Lang_GetUTF8("SET_RumbleV2"));    	
    else
  	 	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*2+2+8,Lang_GetUTF8("SET_RumbleV1"));    	
 
    pTmpBM->SetFontTextColor(SYSColor1);
    pTmpBM->TextOutUTF8(37+2,45+(16+4)*3+2+4,Lang_GetUTF8("SET_3in1Mode"));
    /*if(!isPressRect31)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET0);
    else
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_LET1);
    pbm2->BitBlt(pTmpBM,123,45+(16+4)*3);    
    if(!isPressRect32)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT0);
    else
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_RIT1);
    pbm2->BitBlt(pTmpBM,202,45+(16+4)*3);   
    if(ItemIdx != 2)
    	pbm2 = SetupWAlpha_GetSkin(ESWSA_Opt_BG);
    else
    	{
    		pbm2 = SetupWAlpha_GetSkin(ESWSA_SelectOptionBG); 
    		pTmpBM->SetFontTextColor(RGB15(31,31,31) | BIT(15));
    	}
    pbm2->BitBlt(pTmpBM,cpy_x,45+(16+4)*3); 	*/
    if(ItemIdx == 2)
    	DrawButtonBG(pTmpBM,cpy_x,45+(16+4)*3+4,52,17);
    if(CurProcState->SetupW.M3in1Mode==true)
    	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*3+2+4,Lang_GetUTF8("SET_3in1ModeV1"));
    	//pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Disable");
    else 
       // pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Enable");
       	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*3+2+4,Lang_GetUTF8("SET_3in1ModeV2"));
   
    pTmpBM->SetFontTextColor(SYSColor1);
    
   //
    pTmpBM->TextOutUTF8(37+2,45+(16+4)*4+2,Lang_GetUTF8("SET_Skin"));
    if(ItemIdx == 3)
        	DrawButtonBG(pTmpBM,cpy_x,45+(16+4)*4,52,17);
       // if(CurProcState->SetupW.SkinSelect==0)
        //	pTmpBM->TextOutUTF8(cpy_x+2,45+(16+4)*4+2,SkinData[0].pValue);
        	//pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Disable");
      ///  else 
           // pTmpBM->TextOutA(cpy_x+2,45+(16+4)*3+2,"Enable");
        pTmpBM->TextOutA(cpy_x+2,45+(16+4)*4+2,ListFile[CurProcState->SetupW.SkinSelect].DispName);
       
        pTmpBM->SetFontTextColor(SYSColor1);
    
    
    
    if(!isPressA)
    	//pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);
    	{
    		pTmpBM->SetFontTextColor(SYSColor1);    	
    	}
    else
    	//pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);  
    	{
    		pTmpBM->SetFontTextColor(SYSColor2);
    		DrawButtonBG(pTmpBM,37+37*1+5,140+2,68,17);
    	}
    //pbm2->BitBlt(pTmpBM,37+37*1,140);
    pTmpBM->TextOutUTF8_StartMid(37+37*1+5,140+2+2,68,Lang_GetUTF8("SET_ButtonOK"));
    pTmpBM->SetFontTextColor(SYSColor1);
    if(!isPressB)
    	//pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);
    	{
    		pTmpBM->SetFontTextColor(SYSColor1);    	
    	}
    else
    	{
    		pTmpBM->SetFontTextColor(SYSColor2);
    		DrawButtonBG(pTmpBM,37+37*3+5,140+2,68,17);
    	}
    	//  	pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1); 
   // pbm2->BitBlt(pTmpBM,37+37*3,140); */      
    pTmpBM->TextOutUTF8_StartMid(37+37*3+5,140+2+2,68,Lang_GetUTF8("SET_ButtonCancel")); 
    pTmpBM->SetFontTextColor(SYSColor1);
     
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	}   
	if(pbmpres!=NULL)
	{
	    delete pbmpres;pbmpres=NULL;
	}

}

static void CB_Start(void)
{
   lcdMainOnTop();
   extern bool SetupWisComefromMenu;
	if(SetupWisComefromMenu)
		dmaCopy(pScreenSub->pCanvas->GetVRAMBuf(),pScreenMain->pViewCanvas->GetVRAMBuf(),ScreenWidth*ScreenHeight*2);

   DrawOnlineHelp(); 
}

static void CB_VsyncUpdate(u32 VsyncCount)
{

	UpdataSetupW();
}

static void CB_End(void)
{
  ProcState_RequestSave=true;
  ProcState_Save();
  
}

void ProcSetupW_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}

