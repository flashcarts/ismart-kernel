
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

#include "proc_Cheat_Scorll.h"
#include "ards.h"
#include "proc_Cheat_Local2Unicode.h"
#include "proc_Cheat_SaveCheatData.h"

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}
#include "proc_Cheat.h"
TCheatRomInfo CheatRomInfo;
static char NdsRomSign[5];
u32 DSIdx;
//--------------------------

#define SYSColor1  ColorTable.ZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.ZV_SYSTEM.SYS_Color2

static bool isPressRect11=false;
static bool isPressRect12=false;
static bool isPressRect13=false;
static bool isPressRect14=false;

//TProcState *CurProcState;
const TRect  menurect11={7,163,56,24};
const TRect  menurect12={69,163,56,24};
const TRect  menurect13={131,163,56,24};
const TRect  menurect14={193,163,56,24};

const TRect  menurect21={0,18,100,17};
const TRect  menurect22={0,36,100,17};
const TRect  menurect23={0,54,100,17};
const TRect  menurect24={0,72,100,17};
const TRect  menurect25={0,90,100,17};
const TRect  menurect26={0,106,100,17};
const TRect  menurect27={0,123,100,17};
const TRect  menurect28={0,141,100,17};
static bool isPressRect21=false;
static bool isPressRect22=false;
static bool isPressRect23=false;
static bool isPressRect24=false;
static bool isPressRect25=false;
static bool isPressRect26=false;
static bool isPressRect27=false;
static bool isPressRect28=false;

static bool bgoLoop = false;
static bool bgoLoop2 = false;
static bool bCleanALLflg =false;

static bool bDispMenu =false;


static bool bend = false;
static int curChar = 0;
static UnicodeChar dststrTmp[256];

static bool bend2 = false;
static int curChar2 = 0;
static UnicodeChar dststr2Tmp[256];

static u8 isItemEnable[1024];//0x00=禁止 0x01=启用 0xff=文件夹
static u16 dI2ARD[1024];
static u32 LastSelectItem;
static s32 LastTopPos;
static bool ChangFlg;
static u8 toSaveCheatData[0x100];
static NDSCheatInfo *FindSelectNDSCheatHeadInfo()
{//在库文件中寻找游戏金手指数据是否存在
	u32 gl_RomCRC;
	uint8 *bm=(uint8*)safemalloc(512);
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
	{
		NdsRomSign[0]=sign[0];
		NdsRomSign[1]=sign[1];
		NdsRomSign[2]=sign[2];
		NdsRomSign[3]=sign[3];
		NdsRomSign[4]=0x00;
	}	
	if(bm)
	{
		safefree(bm);bm=NULL;
	}
	_consolePrintf("sign=%s \n",sign);	
	_consolePrintf("gl_RomCRC=%x \n",gl_RomCRC);	
	NDSCheatInfo *pInfo=FindNDSCheatInfo(sign,gl_RomCRC);	
	return pInfo;
}
static void StartNDSCheatWindows()
{	
    NDSCheatInfo *pInfo=FindSelectNDSCheatHeadInfo();		    
    if(pInfo)
    {
    	GetCheatData(pInfo);
    }
    _consolePrintf("\n GetCheatData End.");
}

static void CheatItemScrollBarInit()
{
	 
	 u32 ItemHeight=16+2; 
	 
	 ScrollBar_Free(&ScrollBar);
	 ScrollBar_Init(&ScrollBar,ItemHeight);
	  
	 ScrollBar.TopPos=0;
	 ScrollBar.ShowPos=ScrollBar.TopPos;
	 ScrollBar.MaxPos=ScrollBar.ItemHeight*m_FirstLevelCIC;
	 ScrollBar_SetSelectedIndex(&ScrollBar,0);

}

static void CB_CancelBtn_Click(void *pComponentButton)
{
  SetNextProc(ENP_ROMSET,EPFE_CrossFade);
}

// -----------------------------
void CleanbHotItem(AR_DATA *pARDataCurItem,s32 SelectIndex)
{
	AR_DATA *pPre=NULL;
	AR_DATA *pCur=NULL;
	AR_DATA *pNext=NULL;
	u32 index=0;	
	_consolePrintf("\n this");
	_consolePrintf("\n pARDataCurItem->bUser=%d",pARDataCurItem->bUse);
	_consolePrintf("\n pARDataCurItem->bOneHot=%d",pARDataCurItem->bOneHot);
	if(pARDataCurItem->bUse==true && pARDataCurItem->bOneHot==true)
	{
		
		//向前查找
		index=SelectIndex;
		pCur=pPre=pNext=pARDataCurItem;
		pPre=pCur->pARPreData;
		index--;
		if(pPre)
		{	
			_consolePrintf("\n pPre->bFolder=%d",pPre->bFolder);
			while(pPre->bFolder!=true)
			{
				if(pPre->bUse==true)
				{
					pPre->bUse=false;					
				}
				isItemEnable[index--]=0x00;
				pPre=pPre->pARPreData;
				if(pPre==NULL)
					break;
			}
		}
		//向后查找
		index=SelectIndex;
		pCur=pPre=pNext=pARDataCurItem;
		pNext=pCur->pARNextData;
		index++;
		if(pNext)
		{
			_consolePrintf("\n pNext->bFolder=%d",pNext->bFolder);
			while(pNext->bFolder!=true)
			{
				if(pNext->bUse==true)
				{
					pNext->bUse=false;
					
				}
				isItemEnable[index++]=0x00;
				pNext=pNext->pARNextData;
				if(pNext==NULL)
					break;
			}
		}
	}
}

static void EnableARData()
{
	AR_DATA	*pARDataCur=m_pARDataHead;
	pARDataCur=pARDataCur->pARNextData;	
	for(s32 kk=0;kk<m_CheatItemCounter;kk++)
	{
		if(pARDataCur)
		{
			if(isItemEnable[kk]==0x01)
				pARDataCur->bUse=true;			
			if(pARDataCur->bFolder==true)
			{
				pARDataCur->bUse=false;
			}			
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
			else
				break;
		}
	}	
}

static void FindEnableData()
{
	AR_DATA	*pARDataCur=m_pARDataHead;
	pARDataCur=pARDataCur->pARNextData;
	memset(isItemEnable,0x00,1024);
	for(s32 kk=0;kk<m_CheatItemCounter;kk++)
	{
		if(pARDataCur)
		{
			if(pARDataCur->bFolder==true)
			{
				pARDataCur->bUse=false;
			}
			if(pARDataCur->bUse)
			{	
				isItemEnable[kk]=0x01;				
			}
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
			else
				break;
		}
	}	
}


static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
  TScrollBar *psb=&ScrollBar;
 if((Keys&KEY_A)!=0)
 {
	FindEnableData(); 
	memset(toSaveCheatData,0x00,128);
	for(u32 idx=0;idx<1024;idx++)
	{
		if(isItemEnable[idx]==0x01)
		{
			u8 data=idx & 0x07;
			switch(data)
			{
				case 0x00: toSaveCheatData[idx>>3]|=0x01;break;
				case 0x01: toSaveCheatData[idx>>3]|=0x02;break;	
				case 0x02: toSaveCheatData[idx>>3]|=0x04;break;
				case 0x03: toSaveCheatData[idx>>3]|=0x08;break;
				case 0x04: toSaveCheatData[idx>>3]|=0x10;break;
				case 0x05: toSaveCheatData[idx>>3]|=0x20;break;	
				case 0x06: toSaveCheatData[idx>>3]|=0x40;break;
				case 0x07: toSaveCheatData[idx>>3]|=0x80;break;
				default:
					break;
			}
		}			
	}
	{
		toSaveCheatData[0xA0]=NdsRomSign[0];
		toSaveCheatData[0xA1]=NdsRomSign[1];
		toSaveCheatData[0xA2]=NdsRomSign[2];
		toSaveCheatData[0xA3]=NdsRomSign[3];
	}
	CheatDataSave(toSaveCheatData);
	//freeItMem();
	CB_CancelBtn_Click(NULL);
	//ProcState_RequestSave	= true;
	//ProcState_Save();
  }

  
  if((Keys&KEY_B)!=0){
      CB_CancelBtn_Click(NULL);
  }
  
  if((Keys&KEY_Y)!=0)
  {
	  //if((isItemEnable[psb->SelectedIndex]==0x01))
		//  isItemEnable[psb->SelectedIndex]=0x00;	  
	    AR_DATA	*pARDataCur=m_pARDataHead;
		pARDataCur=pARDataCur->pARNextData;
		s32 index=dI2ARD[psb->SelectedIndex];
		_consolePrintf("index=%d\n\n",index);
		for(s32 kk=0;kk<index;kk++)
		{
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
		}
		_consolePrintf("\n\npARDataCur->pFunction=%s\n\n",pARDataCur->pFunction);
		for(s32 ii=0;ii<pARDataCur->ARCheat.datasize;ii++)
			_consolePrintf("%08x\n\n",*(pARDataCur->ARCheat.pData+ii));	
		if(pARDataCur->bFolder==true)
		{
			pARDataCur->bUse=false;
			if(pARDataCur->bExpd)
			{
				pARDataCur->bExpd=false;
				m_FirstLevelCIC-=pARDataCur->SubCnt;
				LastSelectItem=psb->SelectedIndex;
				if(m_FirstLevelCIC<8)
					LastTopPos=0;
				else					
				{
					if(m_FirstLevelCIC-psb->TopPos/18<8)
						LastTopPos=(m_FirstLevelCIC-8)*18;
					else
						LastTopPos=psb->TopPos;
				}
				if(LastTopPos < 0)
					LastTopPos=0;
				ChangFlg=true;
				CheatItemScrollBarInit();
			}
		}
		else
		{
			if(pARDataCur->bUse==true)
			{
				pARDataCur->bUse=false;				
			}
			//isItemEnable[psb->SelectedIndex]=0x00;
			CleanbHotItem(pARDataCur,index); 
		}
  }
  if((Keys&KEY_X)!=0)
  {
	 // if(isItemEnable[psb->SelectedIndex]==0x00)
		//  isItemEnable[psb->SelectedIndex]=0x01;  
	  
	   AR_DATA	*pARDataCur=m_pARDataHead;
		pARDataCur=pARDataCur->pARNextData;
		s32 index=dI2ARD[psb->SelectedIndex];
		_consolePrintf("index=%d\n\n",index);
		for(s32 kk=0;kk<index;kk++)
		{
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
		}
		_consolePrintf("\n\npARDataCur->pFunction=%s\n\n",pARDataCur->pFunction);
		for(s32 ii=0;ii<pARDataCur->ARCheat.datasize;ii++)
			_consolePrintf("%08x\n\n",*(pARDataCur->ARCheat.pData+ii));		
		if(pARDataCur->bFolder==true)
		{
			pARDataCur->bUse=false;
			if(!pARDataCur->bExpd)
			{
				pARDataCur->bExpd=true;
				m_FirstLevelCIC+=pARDataCur->SubCnt;
				LastSelectItem=psb->SelectedIndex;
				LastTopPos=psb->TopPos;
				ChangFlg=true;
				CheatItemScrollBarInit();
			}
		}
		else
		{
			if(pARDataCur->bUse==false)
			{
				pARDataCur->bUse=true;				
			}
			//isItemEnable[psb->SelectedIndex]=0x01;
			CleanbHotItem(pARDataCur,index); 
		}
		_consolePrintf("\n pARDataCur->bUser=%d",pARDataCur->bUse);
		_consolePrintf("\n pARDataCur->bOneHot=%d",pARDataCur->bOneHot);
  }

  if((Keys&(KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_DOWN))!=0){
    if((Keys&KEY_LEFT)!=0) 
    {
    	
    	if(psb->SelectedIndex-4<0)
    		ScrollBar_SetSelectedIndex(psb,0); 
    	else
    		ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex-4);
    	
    	bCleanALLflg = true;
    }
    if((Keys&KEY_RIGHT)!=0) 
    {
    	if(psb->SelectedIndex+4>m_CheatItemCounter-1)
    	    ScrollBar_SetSelectedIndex(psb,m_CheatItemCounter-1); 
    	else
    		ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+4);
    	bCleanALLflg = true;

    }
    if((Keys&KEY_DOWN)!=0)
    {
    	ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+1);   
    	bCleanALLflg = true;
    }
    if((Keys&KEY_UP)!=0)
    {
    	ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex-1);
    	bCleanALLflg = true;
    }
  }
}

static void CB_MouseDown(s32 x,s32 y)
{
	//TScrollBar *psb=&ScrollBar;
	if(isInsideRect(menurect11,x,y)==true)	
	{
		if(isPressRect11 ==false)
			isPressRect11 = true;
	}
	if(isInsideRect(menurect12,x,y)==true)	
	{
		if(isPressRect12 ==false)
			isPressRect12 = true; 
	}
	if(isInsideRect(menurect13,x,y)==true)	
	{
		if(isPressRect13 ==false)
			isPressRect13 = true;
	}
	if(isInsideRect(menurect14,x,y)==true)	
	{
		if(isPressRect14 ==false)
			isPressRect14 = true;
	}
	
	//
	if(isInsideRect(menurect21,x,y)==true)	
	{
		if(isPressRect21 ==false)
			isPressRect21 = true;
	}
	if(isInsideRect(menurect22,x,y)==true)	
	{
		if(isPressRect22 ==false)
			isPressRect22 = true;
	}
	if(isInsideRect(menurect23,x,y)==true)	
	{
		if(isPressRect23 ==false)
			isPressRect23 = true;
	}
	if(isInsideRect(menurect24,x,y)==true)	
	{
		if(isPressRect24 ==false)
			isPressRect24 = true;
	}
	if(isInsideRect(menurect25,x,y)==true)	
	{
		if(isPressRect25 ==false)
			isPressRect25 = true;
	}
	if(isInsideRect(menurect26,x,y)==true)	
	{
		if(isPressRect26 ==false)
			isPressRect26 = true;
	}
	if(isInsideRect(menurect27,x,y)==true)	
	{
		if(isPressRect27 ==false)
			isPressRect27 = true;
	}
	if(isInsideRect(menurect28,x,y)==true)	
	{
		if(isPressRect28 ==false)
			isPressRect28 = true;
	}
	if(ScrollBar_MouseDown(&ScrollBar,x,y)==true) return;
}

static void CB_MouseMove(s32 x,s32 y)
{
	if(ScrollBar_MouseMove(&ScrollBar,x,y)==true) return;
}
///////////////////////////////////////////////////
static void Fun_global(u32 num)
{
	TScrollBar *psb=&ScrollBar;
	// if(isItemEnable[psb->SelectedIndex]==0x00)
	//  isItemEnable[psb->SelectedIndex]=0x01; 
	//extern void videoSub_SetShowLog(bool e);
	//videoSub_SetShowLog(true);
	_consolePrintf(" psb->SelectedIndex= %d \n",psb->SelectedIndex);
	_consolePrintf(" psb->TopPos= %d \n",psb->TopPos);
	_consolePrintf(" psb->ShowPos= %d \n",psb->ShowPos);
	_consolePrintf(" psb->MaxPos= %d \n",psb->MaxPos);
	_consolePrintf(" psb->LastPos= %d \n",psb->LastPos);
	
	//while(*(vuint16*)0x04000130 == 0x3ff);
	//while(*(vuint16*)0x04000130 != 0x3ff);
	//videoSub_SetShowLog(false);	
	ScrollBar_SetSelectedIndex(psb,psb->TopPos/18); 
	ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+num-1);
	bCleanALLflg = true;
		
	   AR_DATA	*pARDataCur=m_pARDataHead;
		pARDataCur=pARDataCur->pARNextData;
		s32 index=dI2ARD[psb->SelectedIndex];
		_consolePrintf("psb->SelectedIndex=%d\n\n",psb->SelectedIndex);
		_consolePrintf("index=%d\n\n",index);
		for(s32 kk=0;kk<index;kk++)
		{
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
		}
		_consolePrintf("\n\npARDataCur->pFunction=%s\n\n",pARDataCur->pFunction);
		for(s32 ii=0;ii<pARDataCur->ARCheat.datasize;ii++)
			_consolePrintf("%08x\n\n",*(pARDataCur->ARCheat.pData+ii));		
		if(pARDataCur->bFolder==true)
		{
			_consolePrintf(" is Folder\n");
			pARDataCur->bUse=false;
			if(!pARDataCur->bExpd)
			{
				_consolePrintf(" open Folder\n");
				pARDataCur->bExpd=true;
				m_FirstLevelCIC+=pARDataCur->SubCnt;
				LastSelectItem=psb->SelectedIndex;
				LastTopPos=psb->TopPos;
				ChangFlg=true;
				CheatItemScrollBarInit();
			}
			else//收缩文件夹
			{
				pARDataCur->bExpd=false;
				m_FirstLevelCIC-=pARDataCur->SubCnt;
				LastSelectItem=psb->SelectedIndex;
				if(m_FirstLevelCIC<8)
					LastTopPos=0;
				else					
				{
					if(m_FirstLevelCIC-psb->TopPos/18<8)
						LastTopPos=(m_FirstLevelCIC-8)*18;
					else
						LastTopPos=psb->TopPos;
				}
				if(LastTopPos < 0)
					LastTopPos=0;
				ChangFlg=true;
				CheatItemScrollBarInit();
			}
		}
		else
		{
			pARDataCur->bUse = !pARDataCur->bUse;
			CleanbHotItem(pARDataCur,index); 
		}
		_consolePrintf("\n pARDataCur->bUser=%d",pARDataCur->bUse);
		_consolePrintf("\n pARDataCur->bOneHot=%d",pARDataCur->bOneHot);
		_consolePrintf(" LastTopPos=%d\n",LastTopPos);
	    //while(*(vuint16*)0x04000130 == 0x3ff);
	    //while(*(vuint16*)0x04000130 != 0x3ff);
	    //videoSub_SetShowLog(false);
}
static void CB_MouseUp(s32 x,s32 y)
{
	TScrollBar *psb=&ScrollBar;
	if(psb->TopPos<0)
	{ 
		psb->TopPos=0;
	}
	else if((psb->TopPos%psb->ItemHeight)!=0) 
	{
		if(y>=psb->LastMouseY)
		{
			psb->TopPos=((psb->TopPos+(psb->ItemHeight-1))/psb->ItemHeight)*psb->ItemHeight;
		}
		else
		{
			psb->TopPos=((psb->TopPos-(psb->ItemHeight-1))/psb->ItemHeight)*psb->ItemHeight;
		}
	}		
	if(isInsideRect(menurect11,x,y)==true)	
	{
		if(isPressRect11 ==true)
			isPressRect11 = false;

		   AR_DATA	*pARDataCur=m_pARDataHead;
			pARDataCur=pARDataCur->pARNextData;
			s32 index=dI2ARD[psb->SelectedIndex];
			_consolePrintf("index=%d\n\n",index);
			for(s32 kk=0;kk<index;kk++)
			{
				if(pARDataCur->pARNextData)
					pARDataCur=pARDataCur->pARNextData;
			}
			_consolePrintf("\n\npARDataCur->pFunction=%s\n\n",pARDataCur->pFunction);
			for(s32 ii=0;ii<pARDataCur->ARCheat.datasize;ii++)
				_consolePrintf("%08x\n\n",*(pARDataCur->ARCheat.pData+ii));		
			if(pARDataCur->bFolder==true)
			{
				pARDataCur->bUse=false;
				if(!pARDataCur->bExpd)
				{
					pARDataCur->bExpd=true;
					m_FirstLevelCIC+=pARDataCur->SubCnt;
					LastSelectItem=psb->SelectedIndex;
					LastTopPos=psb->TopPos;
					ChangFlg=true;
					CheatItemScrollBarInit();
				}
			}
			else
			{
				if(pARDataCur->bUse==false)
				{
					pARDataCur->bUse=true;				
				}
				//isItemEnable[psb->SelectedIndex]=0x01;
				CleanbHotItem(pARDataCur,index); 
			}
			_consolePrintf("\n pARDataCur->bUser=%d",pARDataCur->bUse);
			_consolePrintf("\n pARDataCur->bOneHot=%d",pARDataCur->bOneHot);
	}
	if(isInsideRect(menurect12,x,y)==true)	
	{
		if(isPressRect12 ==true)
			isPressRect12 = false;
	    AR_DATA	*pARDataCur=m_pARDataHead;
		pARDataCur=pARDataCur->pARNextData;
		s32 index=dI2ARD[psb->SelectedIndex];
		_consolePrintf("index=%d\n\n",index);
		for(s32 kk=0;kk<index;kk++)
		{
			if(pARDataCur->pARNextData)
				pARDataCur=pARDataCur->pARNextData;
		}
		_consolePrintf("\n\npARDataCur->pFunction=%s\n\n",pARDataCur->pFunction);
		for(s32 ii=0;ii<pARDataCur->ARCheat.datasize;ii++)
			_consolePrintf("%08x\n\n",*(pARDataCur->ARCheat.pData+ii));	
		if(pARDataCur->bFolder==true)
		{
			pARDataCur->bUse=false;
			if(pARDataCur->bExpd)
			{
				pARDataCur->bExpd=false;
				m_FirstLevelCIC-=pARDataCur->SubCnt;
				LastSelectItem=psb->SelectedIndex;
				if(m_FirstLevelCIC<8)
					LastTopPos=0;
				else					
				{
					if(m_FirstLevelCIC-psb->TopPos/18<8)
						LastTopPos=(m_FirstLevelCIC-8)*18;
					else
						LastTopPos=psb->TopPos;
				}
				if(LastTopPos < 0)
					LastTopPos=0;
				ChangFlg=true;
				CheatItemScrollBarInit();
			}
		}
		else
		{
			if(pARDataCur->bUse==true)
			{
				pARDataCur->bUse=false;				
			}
			//isItemEnable[psb->SelectedIndex]=0x00;
			CleanbHotItem(pARDataCur,index); 
		}
	}
	if(isInsideRect(menurect13,x,y)==true)	
	{
		if(isPressRect13 ==true)
			isPressRect13 = false;
		//保存代码
		FindEnableData(); 
		memset(toSaveCheatData,0x00,128);
		for(u32 idx=0;idx<1024;idx++)
		{
			if(isItemEnable[idx]==0x01)
			{
				u8 data=idx & 0x07;
				switch(data)
				{
					case 0x00: toSaveCheatData[idx>>3]|=0x01;break;
					case 0x01: toSaveCheatData[idx>>3]|=0x02;break;	
					case 0x02: toSaveCheatData[idx>>3]|=0x04;break;
					case 0x03: toSaveCheatData[idx>>3]|=0x08;break;
					case 0x04: toSaveCheatData[idx>>3]|=0x10;break;
					case 0x05: toSaveCheatData[idx>>3]|=0x20;break;	
					case 0x06: toSaveCheatData[idx>>3]|=0x40;break;
					case 0x07: toSaveCheatData[idx>>3]|=0x80;break;
					default:
						break;
				}
			}			
		}
		{
			toSaveCheatData[0xA0]=NdsRomSign[0];
			toSaveCheatData[0xA1]=NdsRomSign[1];
			toSaveCheatData[0xA2]=NdsRomSign[2];
			toSaveCheatData[0xA3]=NdsRomSign[3];
		}
		CheatDataSave(toSaveCheatData);
    	CB_CancelBtn_Click(NULL);
	}
	if(isInsideRect(menurect14,x,y)==true)	
	{
		if(isPressRect14 ==true)
			isPressRect14 = false;
	    CB_CancelBtn_Click(NULL);
	}
	//触摸选项
	if(isInsideRect(menurect21,x,y)==true)	
	{
		if(isPressRect21 ==true)
			isPressRect21 = false;
	    Fun_global(1);
	}
	if(isInsideRect(menurect22,x,y)==true)	
	{
		if(isPressRect22 ==true)
			isPressRect22 = false;
	    Fun_global(2);
	}
	if(isInsideRect(menurect23,x,y)==true)	
	{
		if(isPressRect23 ==true)
			isPressRect23 = false;
	    Fun_global(3);
	}
	if(isInsideRect(menurect24,x,y)==true)	
	{
		if(isPressRect24 ==true)
			isPressRect24 = false;
	    Fun_global(4);
	}
	if(isInsideRect(menurect25,x,y)==true)	
	{
		if(isPressRect25 ==true)
			isPressRect25 = false;
	    Fun_global(5);
	}
	if(isInsideRect(menurect26,x,y)==true)	
	{
		if(isPressRect26 ==true)
			isPressRect26 = false;
	    Fun_global(6);
	}
	if(isInsideRect(menurect27,x,y)==true)	
	{
		if(isPressRect27 ==true)
			isPressRect27 = false;
	    Fun_global(7);
	}
	if(isInsideRect(menurect28,x,y)==true)	
	{
		if(isPressRect28 ==true)
			isPressRect28 = false;
	    Fun_global(8);
	}
	
	if(ScrollBar_MouseUp(&ScrollBar,x,y)==true) return;
}

static void DrawOnlineHelp(void)
{
	//CurProcState = &ProcState;
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	//gausscolor50bk(pTmpBM);
	CglTGF * pbm2= CheatAlpha_GetSkin(ECWSA_CheatBG);
	pbm2->BitBlt(pTmpBM,0,0);

	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(0x8000);//设置字体颜色黑色  
     
   // pTmpBM->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu"));   
    
    pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
    pbm2->BitBlt(pTmpBM,7,160);
    pbm2->BitBlt(pTmpBM,69,160); 
    pbm2->BitBlt(pTmpBM,131,160);
    pbm2->BitBlt(pTmpBM,193,160);                     
    pTmpBM->TextOutUTF8_StartMid(7,160+4,64,Lang_GetUTF8("CHEAT_Enable"));
    pTmpBM->TextOutUTF8_StartMid(69,160+4,56,Lang_GetUTF8("CHEAT_Disable")); 
    pTmpBM->TextOutUTF8_StartMid(131,160+4,56,Lang_GetUTF8("CHEAT_Save"));
    pTmpBM->TextOutUTF8_StartMid(193,160+4,56,Lang_GetUTF8("CHEAT_Cancel")); 
    
    CheatItemScrollBarInit();
    memset(isItemEnable,0x00,1024);
    //CheatData_Init();
    Set_DSIdx(DSIdx);
    CheatDataLoad(toSaveCheatData);
    char sign[5];
    {
		sign[0]=toSaveCheatData[0xA0];
		sign[1]=toSaveCheatData[0xA1];
		sign[2]=toSaveCheatData[0xA2];
		sign[3]=toSaveCheatData[0xA3];
		sign[4]=0;
	}
	if(!memcmp(sign,NdsRomSign,5))
	{   for(u32 idx=0;idx<128;idx++)
		{
			if((toSaveCheatData[idx] & 0x01))
				isItemEnable[idx*8+0x00]=0x01;
			if((toSaveCheatData[idx] & 0x02))
				isItemEnable[idx*8+0x01]=0x01;
			if((toSaveCheatData[idx] & 0x04))
				isItemEnable[idx*8+0x02]=0x01;
			if((toSaveCheatData[idx] & 0x08))
				isItemEnable[idx*8+0x03]=0x01;
			if((toSaveCheatData[idx] & 0x10))
				isItemEnable[idx*8+0x04]=0x01;
			if((toSaveCheatData[idx] & 0x20))
				isItemEnable[idx*8+0x05]=0x01;
			if((toSaveCheatData[idx] & 0x40))
				isItemEnable[idx*8+0x06]=0x01;
			if((toSaveCheatData[idx] & 0x80))
				isItemEnable[idx*8+0x07]=0x01; 			
		} 
		EnableARData();
	}   
	
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
}
/*
static void UpdataCheat(void)
{
	
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	
	CglCanvas *pTmpBM2=new CglCanvas(NULL,ScreenWidth,16,pf15bit);
	CglCanvas *pTmpBM3=new CglCanvas(NULL,ScreenWidth,16,pf15bit);
	
	CglTGF * pbm2= CheatAlpha_GetSkin(ECWSA_CheatBG);
	pbm2->BitBlt(pTmpBM,0,0);
	pbm2->BitBltLimitY(pTmpBM2,0,0,16,0);
	pbm2->BitBltLimitY(pTmpBM3,0,0,16,163);
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(0x8000);//设置字体颜色黑色  
	
	//--------------------------------------------------------------//
	pTmpBM->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu"));   
	    
	if(isPressRect11 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
	pbm2->BitBlt(pTmpBM,7,163);
	if(isPressRect12 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,69,163); 
	if(isPressRect13 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,131,163);
	if(isPressRect14 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,193,163);  
    
    pTmpBM->TextOutUTF8_StartMid(7,163+4,64,Lang_GetUTF8("CHEAT_Enable"));
    pTmpBM->TextOutUTF8_StartMid(69,163+4,64,Lang_GetUTF8("CHEAT_Disable")); 
    pTmpBM->TextOutUTF8_StartMid(131,163+4,64,Lang_GetUTF8("CHEAT_Save"));
    pTmpBM->TextOutUTF8_StartMid(193,163+4,64,Lang_GetUTF8("CHEAT_Cancel"));
	//------------------------------------------------------------------//
    
    TScrollBar *psb=&ScrollBar;
    //画金手指条目
    u32 dx=0;
    u32 dy=16;//pSB->TopPos;
    u32 ddistance=16+2;
    u32 dfontoffset=2;
    u32 wlimited=256-14-10;    
    u32 idx=0;
    AR_DATA *pARCheatCur;

	pARCheatCur=m_pARDataHead;
	if(pARCheatCur->pARNextData!=NULL)
		pARCheatCur=pARCheatCur->pARNextData;
	do
	{	

		if(pARCheatCur->bFolder==true)
		{
			isItemEnable[idx]=0xff;
		}
		s32 DrawHeight=ddistance;
		s32 DrawTop=(idx*DrawHeight)-psb->TopPos;;
		if((-(psb->ItemHeight-8)<=DrawTop)&&(DrawTop<psb->ClientSize))
		{
			if(psb->SelectedIndex==idx)
			{
				CglTGF * pbm3=CheatAlpha_GetSkin(ECWSA_ItemBG_Select);
				pbm3->BitBlt(pTmpBM,dx,DrawTop+dy);
				pTmpBM->SetFontTextColor(RGB15(31,31,31) | BIT(15));
			}
			else
			{
				pTmpBM->SetFontTextColor(RGB15(0,0,0) | BIT(15));
			}
			
			if(pARCheatCur->bFolder==true)
			{
				pbm2 =  CheatAlpha_GetSkin(ECWSA_Sub);
			}
			if(isItemEnable[idx]==0x00)
				pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_NO);    
			if(isItemEnable[idx]==0x01)
				pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_YES);			
			pbm2->BitBlt(pTmpBM,dx+2,DrawTop+dy);
			
			UnicodeChar dststr[256];			
			SJIS2Unicode_Convert(pARCheatCur->pFunction,dststr);
			pTmpBM->TextOutW_WidthOther(dx+22,DrawTop+dy+dfontoffset,wlimited,dststr);						
		}		
		pARCheatCur=pARCheatCur->pARNextData;
		idx++;
	}while(pARCheatCur!=NULL);    

	pTmpBM2->SetCglFont(pCglFontDefault);
	pTmpBM2->SetFontTextColor(0x8000);//设置字体颜色黑色  	
	pTmpBM2->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu")); 
	pTmpBM2->BitBlt(pTmpBM,0,0,ScreenWidth,16,0,0,0);
	if(pTmpBM2!=NULL)
	{
	    delete pTmpBM2; pTmpBM2=NULL;
	} 	
	pTmpBM3->BitBlt(pTmpBM,0,163,ScreenWidth,16,0,0,0);	
	if(pTmpBM3!=NULL)
	{
	    delete pTmpBM3; pTmpBM3=NULL;
	}
	if(isPressRect11 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
	pbm2->BitBlt(pTmpBM,7,163);
	if(isPressRect12 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,69,163); 
	if(isPressRect13 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,131,163);
	if(isPressRect14 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,193,163);  
    
    pTmpBM->TextOutUTF8_StartMid(7,163+4,64,Lang_GetUTF8("CHEAT_Enable"));
    pTmpBM->TextOutUTF8_StartMid(69,163+4,64,Lang_GetUTF8("CHEAT_Disable")); 
    pTmpBM->TextOutUTF8_StartMid(131,163+4,64,Lang_GetUTF8("CHEAT_Save"));
    pTmpBM->TextOutUTF8_StartMid(193,163+4,64,Lang_GetUTF8("CHEAT_Cancel"));
    
    ScrollBar_Draw(&ScrollBar,pTmpBM);    
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	    
	} 
}*/

extern void TestPauseKey(void);
extern void videoSub_SetShowLog(bool);
static void UpdataCheat(void)
{
	
	CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	
	
	CglCanvas *pTmpBM2=new CglCanvas(NULL,ScreenWidth,16,pf15bit);
	CglCanvas *pTmpBM3=new CglCanvas(NULL,ScreenWidth,16,pf15bit);
	
	CglTGF * pbm2= CheatAlpha_GetSkin(ECWSA_CheatBG);
	pbm2->BitBlt(pTmpBM,0,0);
	pbm2->BitBltLimitY(pTmpBM2,0,0,16,0);
	pbm2->BitBltLimitY(pTmpBM3,0,0,16,163);
	
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor( SYSColor1 );//设置字体颜色黑色  	
	
	pTmpBM2->SetCglFont(pCglFontDefault);
	pTmpBM2->SetFontTextColor( SYSColor1 );//设置字体颜色黑色  	
    
    TScrollBar *psb=&ScrollBar;
    //画金手指条目
    u32 dx=0;
    u32 dy=16;//pSB->TopPos;
    u32 ddistance=16+2;
    u32 dfontoffset=2;
    u32 wlimited=256-14-15;    
    u32 idx=0,dI2ARDC=0;;
    AR_DATA *pARCheatCur;
    
    UnicodeChar dststr2[256];
    memset(dststr2,0x00,sizeof(UnicodeChar)*256);

	pARCheatCur=m_pARDataHead;
	if(pARCheatCur->pARNextData!=NULL)
		pARCheatCur=pARCheatCur->pARNextData;
	bool bDisSub=false;
	u32 SubCnt=0;
	u32 cpy = 0;
	bool bdirflag=0;
	
	if(ChangFlg==true)
	{
		psb->SelectedIndex=LastSelectItem;
		psb->TopPos=LastTopPos;
		ChangFlg=false;
	}	
	do
	{	
		if(pARCheatCur->bFolder)
		{
			bdirflag=true;
			if(!pARCheatCur->bExpd)
			{				
				if(bDisSub==true)
				{
					bDisSub=false;
				}
			}
			else
			{				
				if(bDisSub==false)
				{	
					bDisSub=true;
				}
			}			
		}		
		s32 DrawHeight=ddistance;
		s32 DrawTop=(idx*DrawHeight)-psb->TopPos;
		if((-(psb->ItemHeight-8)<=DrawTop)&&(DrawTop<psb->ClientSize))
		{
			if(psb->SelectedIndex==idx)
			{
				CglTGF * pbm3=CheatAlpha_GetSkin(ECWSA_ItemBG_Select);
				pbm3->BitBlt(pTmpBM,dx,DrawTop+dy);
				pTmpBM->SetFontTextColor( SYSColor2 );
			}
			else
			{
				pTmpBM->SetFontTextColor( SYSColor1 );
			}
			UnicodeChar dststr3[256];	
			UnicodeChar dststr23[256];	
			if(pARCheatCur->bFolder)
			{
				bDispMenu = false;
				if(!pARCheatCur->bExpd)
				{
					pbm2 =  CheatAlpha_GetSkin(ECWSA_Mul);
					if(bDisSub==true)
						bDisSub=false;
				}
				else
				{
					pbm2 =  CheatAlpha_GetSkin(ECWSA_Sub);
					if(bDisSub==false)
						bDisSub=true;
				}
				pbm2->BitBlt(pTmpBM,dx+2,DrawTop+dy);
				UnicodeChar dststr[256];			
				SJIS2Unicode_Convert(pARCheatCur->pFunction,dststr);
				Unicode_Copy(dststr3,dststr);
			    if(bCleanALLflg)
			    {
			    	bCleanALLflg = false;
			    	bend = false;
			    	curChar=0;
			    	memset(dststrTmp,0x00,sizeof(UnicodeChar)*256);
			    	Unicode_Copy(dststrTmp,dststr);
			    	
			    	bend2 = false;
			    	curChar2=0;
			    	memset(dststr2Tmp,0x00,sizeof(UnicodeChar)*256);
			    }
				//过长循环显示名字					
				{
					 int wStr=0;
					 wStr = pTmpBM->GetTextWidthW(dststr);
					 if(wStr > wlimited-20)
					 {
						 //长度过长，是否是选中的项目，选中的项目循环显示文件名
						 if(psb->SelectedIndex==idx)
						 {							 
							 if(bgoLoop)
							 {
								 if(!bend)
								 {
									 if(pTmpBM->GetTextWidthW(dststr+curChar) > wlimited-20)
									 {
										 //循环没到末尾
										 curChar++;
									 }
									 else
									 {											
										 bend = true;
									 }
								 }
								 else
								 {
									 if(curChar>0)
									 {
										 curChar--;
									 }
									 else
									 {
										 bend = false;
									 }
								 }
								 bgoLoop = false;									 
								 Unicode_Copy(dststrTmp,dststr+curChar);
							 }
							 Unicode_Copy(dststr3,dststrTmp);
						 }
					 }
				}	
				
				if(psb->SelectedIndex==idx)
					pTmpBM2->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu")); 
				
				pTmpBM->TextOutW_WidthOther(dx+22,DrawTop+dy+dfontoffset,wlimited,dststr3);
				dI2ARD[idx]=dI2ARDC;
				idx++;
				
			}//end if(pARCheatCur->bFolder)
			else
			{
				if(!pARCheatCur->bSub)
				{
					if(pARCheatCur->bUse)
						pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_YES);
					else
						pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_NO); 
					pbm2->BitBlt(pTmpBM,dx+2,DrawTop+dy);
					UnicodeChar dststr[256];			
					SJIS2Unicode_Convert(pARCheatCur->pFunction,dststr);
					Unicode_Copy(dststr3,dststr);

					if(bCleanALLflg)
				    {
				    	bCleanALLflg = false;
				    	bend = false;
				    	curChar=0;
				    	memset(dststrTmp,0x00,sizeof(UnicodeChar)*256);
				    	Unicode_Copy(dststrTmp,dststr);
				    	
				    	bend2 = false;
				    	curChar2=0;
				    	memset(dststr2Tmp,0x00,sizeof(UnicodeChar)*256);
				    }	
				   
					//过长循环显示名字					
					{
						 int wStr=0;
						 wStr = pTmpBM->GetTextWidthW(dststr);
						 if(wStr > wlimited-20)
						 {
							 //长度过长，是否是选中的项目，选中的项目循环显示文件名
							 if(psb->SelectedIndex==idx)
							 {							 
								 if(bgoLoop)
								 {						
									 bgoLoop = false;									 
									 Unicode_Copy(dststrTmp,dststr+curChar);
									 
									 if(!bend)
									 {
										 if(pTmpBM->GetTextWidthW(dststr+curChar) > wlimited-20)
										 {
											 //循环没到末尾
											 curChar++;
										 }
										 else
										 {											
											 bend = true;
										 }
									 }
									 else
									 {
										 if(curChar>0)
										 {
											 curChar--;
										 }
										 else
										 {
											 bend = false;
										 }
									 }
									 
									 
								 }
								 Unicode_Copy(dststr3,dststrTmp);
							 }
						 }
					}	
					
					pTmpBM->TextOutW_WidthOther(dx+22,DrawTop+dy+dfontoffset,wlimited,dststr3);
					dI2ARD[idx]=dI2ARDC;
					//bDispMenu = true;
					if(psb->SelectedIndex==idx)
					{					
						if(pARCheatCur->pFunction2 != NULL)
						{
							bDispMenu = false;
							SJIS2Unicode_Convert(pARCheatCur->pFunction2,dststr2);
							Unicode_Copy(dststr23,dststr2);
							
							//过长循环显示注释					
							{
								 int wStr=0;
								 wStr = pTmpBM->GetTextWidthW(dststr2);
								 if(wStr > 220)
								 {
									 //长度过长，是否是选中的项目，选中的项目循环显示文件名
									 if(psb->SelectedIndex==idx)
									 {							 
										 if(bgoLoop2)
										 {
											 bgoLoop2 = false;									 
											 Unicode_Copy(dststr2Tmp,dststr2+curChar2);							
											 if(!bend2)
											 {
												 if(pTmpBM->GetTextWidthW(dststr2+curChar2) > 220)
												 {
													 //循环没到末尾
													 curChar2++;
												 }
												 else
												 {											
													 bend2 = true;
												 }
											 }
											 else
											 {
												 if(curChar2>0)
												 {
													 curChar2--;
												 }
												 else
												 {
													 bend2 = false;
												 }
											 }
											 
										 }
										 Unicode_Copy(dststr23,dststr2Tmp);
									 }
								 }
							}	
							
							pTmpBM2->TextOutW_WidthOther(5,2,250,dststr23);	
							
						}	
						else
						{
							if(psb->SelectedIndex==idx)
								pTmpBM2->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu")); 
						}
					}	
					
					idx++;
				}//end if(!pARCheatCur->bSub)
				else
				{
					if(bDisSub)//展开
					{
						
						
						if(pARCheatCur->bUse)
							pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_YES);
						else
							pbm2 =  CheatAlpha_GetSkin(ECWSA_Opt_NO); 
						pbm2->BitBlt(pTmpBM,dx+2+5,DrawTop+dy);
						UnicodeChar dststr[256];			
						SJIS2Unicode_Convert(pARCheatCur->pFunction,dststr);
						Unicode_Copy(dststr3,dststr);
						
						
					    if(bCleanALLflg)
					    {
					    	bCleanALLflg = false;
					    	bend = false;
					    	curChar=0;
					    	memset(dststrTmp,0x00,sizeof(UnicodeChar)*256);
					    	Unicode_Copy(dststrTmp,dststr);
					    	
					    	bend2 = false;
					    	curChar2=0;
					    	memset(dststr2Tmp,0x00,sizeof(UnicodeChar)*256);					    	
					    	
					    }
					    
						//过长循环显示名字					
						{
							 int wStr=0;
							 wStr = pTmpBM->GetTextWidthW(dststr);
							 if(wStr > wlimited-20)
							 {
								 //长度过长，是否是选中的项目，选中的项目循环显示文件名
								 if(psb->SelectedIndex==idx)
								 {							 
									 if(bgoLoop)
									 {
										 if(!bend)
										 {
											 if(pTmpBM->GetTextWidthW(dststr+curChar) > wlimited-20)
											 {
												 //循环没到末尾
												 curChar++;
											 }
											 else
											 {											
												 bend = true;
											 }
										 }
										 else
										 {
											 if(curChar>0)
											 {
												 curChar--;
											 }
											 else
											 {
												 bend = false;
											 }
										 }
										 bgoLoop = false;									 
										 Unicode_Copy(dststrTmp,dststr+curChar);
									 }
									 Unicode_Copy(dststr3,dststrTmp);
								 }
							 }
						}	
						pTmpBM->TextOutW_WidthOther(dx+22+5,DrawTop+dy+dfontoffset,wlimited,dststr3);
						dI2ARD[idx]=dI2ARDC;
						//bDispMenu = true;
						if(psb->SelectedIndex==idx)
						{					
							if(pARCheatCur->pFunction2 != NULL)
							{
								bDispMenu = false;
								SJIS2Unicode_Convert(pARCheatCur->pFunction2,dststr2);
								Unicode_Copy(dststr23,dststr2);
								
								//过长循环显示注释					
								{
									 int wStr=0;
									 wStr = pTmpBM->GetTextWidthW(dststr2);
									 if(wStr > 220)
									 {
										 //长度过长，是否是选中的项目，选中的项目循环显示文件名
										 if(psb->SelectedIndex==idx)
										 {							 
											 if(bgoLoop2)
											 {
												 bgoLoop2 = false;									 
												 Unicode_Copy(dststr2Tmp,dststr2+curChar2);							
												 if(!bend2)
												 {
													 if(pTmpBM->GetTextWidthW(dststr2+curChar2) > 220)
													 {
														 //循环没到末尾
														 curChar2++;
													 }
													 else
													 {											
														 bend2 = true;
													 }
												 }
												 else
												 {
													 if(curChar2>0)
													 {
														 curChar2--;
													 }
													 else
													 {
														 bend2 = false;
													 }
												 }
												 
											 }
											 Unicode_Copy(dststr23,dststr2Tmp);
										 }
									 }
								}	
								
								pTmpBM2->TextOutW_WidthOther(5,2,250,dststr23);
								
							}	
							else
							{
								if(psb->SelectedIndex==idx)
									pTmpBM2->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu")); 
							}
						}
						idx++;
					}//end if(bDisSub)	
				}//end else	
			
			}			
			/*UnicodeChar dststr[256];			
			SJIS2Unicode_Convert(pARCheatCur->pFunction,dststr);
			if(!bDisSub)
				pTmpBM->TextOutW_WidthOther(dx+22,DrawTop+dy+dfontoffset,wlimited,dststr);
			else
				pTmpBM->TextOutW_WidthOther(dx+22+5,DrawTop+dy+dfontoffset,wlimited,dststr);*/
		}//end if((-(psb->ItemHeight-8)<=DrawTop)&&(DrawTop<psb->ClientSize))
		else
		{
			if(bdirflag)
			{							
				if(bDisSub==true)
				{
					idx++;
				}	
				if(pARCheatCur->bFolder)
					idx++;
			}
			else
				idx++;
		}
		dI2ARDC++;
		pARCheatCur=pARCheatCur->pARNextData;		
	}while(pARCheatCur!=NULL);    

//	if(bDispMenu)
//	{
//		pTmpBM2->TextOutUTF8(5,2,Lang_GetUTF8("CHEAT_Menu")); 
//		bDispMenu = false;
//	}
	
	
	pTmpBM2->BitBlt(pTmpBM,0,0,ScreenWidth,16,0,0,0);
	if(pTmpBM2!=NULL)
	{
	    delete pTmpBM2; pTmpBM2=NULL;
	} 	
	pTmpBM3->BitBlt(pTmpBM,0,163,ScreenWidth,16,0,0,0);	
	if(pTmpBM3!=NULL)
	{
	    delete pTmpBM3; pTmpBM3=NULL;
	}
	if(isPressRect11 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
	pbm2->BitBlt(pTmpBM,7,163);
	if(isPressRect12 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,69,163); 
	if(isPressRect13 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,131,163);
	if(isPressRect14 ==false)
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);        
	else
		pbm2 =  ScrollBarAlpha_GetSkin(EFLSA_Btn_set1);
    pbm2->BitBlt(pTmpBM,193,163);  
    
    pTmpBM->TextOutUTF8_StartMid(7,163+4,64,Lang_GetUTF8("CHEAT_Enable"));
    pTmpBM->TextOutUTF8_StartMid(69,163+4,64,Lang_GetUTF8("CHEAT_Disable")); 
    pTmpBM->TextOutUTF8_StartMid(131,163+4,64,Lang_GetUTF8("CHEAT_Save"));
    pTmpBM->TextOutUTF8_StartMid(193,163+4,64,Lang_GetUTF8("CHEAT_Cancel"));
    
    ScrollBar_Draw(&ScrollBar,pTmpBM);    
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
   ShellSet.SwapDisp = true ;    
	if(ShellSet.SwapDisp==true) REG_POWERCNT|=POWER_SWAP_LCDS;		    
   StartNDSCheatWindows();
   SJIS2Unicode_Init();
   SJIS2Unicode_Load();
   DrawOnlineHelp();
   

}

static void CB_VsyncUpdate(u32 VsyncCount)
{
	static int cntFont=0;	
	if(cntFont == 20)
	{
		//设置滚动标志
		cntFont = 0;	
		bgoLoop = true;
		bgoLoop2 = true;
	}	
	cntFont++;	
	UpdataCheat();
}

static void CB_End(void)
{
  ScrollBar_Free(&ScrollBar);
  SJIS2Unicode_Free();
  DeleteAR_DATA();
}

void ProcCheat_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}

