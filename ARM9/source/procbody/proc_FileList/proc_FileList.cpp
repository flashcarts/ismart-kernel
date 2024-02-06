#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "../../ipc6.h"
#include "datetime.h"
#include "arm9tcm.h"
#include "strpcm.h"
#include "lang.h"

#include "glib/glib.h"

#include "fat2.h"
#include "shell.h"
#include "sndeff.h"
#include "splash.h"
#include "resume.h"
#include "procstate.h"
#include "launchstate.h"
#include "strtool.h"
#include "unicode.h"
#include "rect.h"
#include "skin.h"
#include "cfont.h"
#include "BootROM.h"
#include "ErrorDialog.h"
#include "extlink.h"

#include "dll.h"
#include "dllsound.h"

#include "NDSROMIcon.h"

#include "cipk_simple.h"
#include "ards.h"
#include "inifile.h"
#include "..\proc_romset\proc_romset_bin.h"
#include "..\proc_SetupW.h"
#include "..\proc_CpyCut\proc_CpyCut.h"
#include "..\proc_GBA\proc_GBA.h"
// -----------------------------
#define SYSColor1  ColorTable.EZV_SYSTEM.SYS_Color1
#define SYSColor2  ColorTable.EZV_SYSTEM.SYS_Color2

static bool IPKLoadFlag;
static FAT_FILE *pIPKFile;
static UnicodeChar *pIPKFilenameUncode;
static CIPK *pIPK;
static TIPKThumbnail *pIPKThumbnail;

static void ReloadIPK(s32 fileidx);
static bool DrawIPK(CglCanvas *pcan);

// -----------------------------

static bool RequestRefreshPlayCursorIndex;
static s32 PlayCursorIndex;

static bool ScreenRedrawFlag;
static bool ForceUpdateSubScreenFlag;

static u32 PanelClosePowerOffTimeOut;

// -----------------------------

static bool HPSwitch_ProcessLong,HPSwitch_ProcessSingleLong;
static u32 HPSwitch_ProcessWait;

// -----------------------------

static u32 BacklightTimeout;

static bool isPressSetUpDownButton = false;
static bool isPressCheatDownButton = false;  

//每一个图标对应
static bool isPress_LightControl = false;
static bool isPress_ToggleControl = false;
static bool isPress_HelpControl = false;
static bool isPress_SystemControl = false;
static bool isPress_ShutDownContol = false;
static bool isPress_ScrollUP = false;
static bool isPress_ScrollDOWN = false;
static bool isPress_Rumble = false;
static bool isPress_Launch = false;

static bool isMS2 = false;

static void Backlight_ResetTimer(void)
{
  if(BacklightTimeout==0){
    //2009.04.09 modify by mj 取消超时交换上下屏内容		
	if(ShellSet.SwapDisp==true) REG_POWERCNT|=POWER_SWAP_LCDS;
    IPC6->LCDPowerControl=LCDPC_ON_BOTH;
    ForceUpdateSubScreenFlag=true;
  }
  BacklightTimeout=ProcState.FileList.BacklightTimeoutSec*60;
}

static void Backlight_SetLast1vsync(void)
{
  BacklightTimeout=1;
}

static void Backlight_VsyncUpdate(u32 VsyncCount)
{
  if(BacklightTimeout==0) return;
  
  if(BacklightTimeout<=VsyncCount){
    BacklightTimeout=0;
    }else{
    BacklightTimeout-=VsyncCount;
  }
  
  if(BacklightTimeout==0){
    //2009.04.09 modify by mj 取消超时关主屏电源，以及交换上下屏内容
	if(ShellSet.SwapDisp==true) REG_POWERCNT&=~POWER_SWAP_LCDS;
    IPC6->LCDPowerControl=LCDPC_ON_TOP;
	//IPC6->LCDPowerControl=LCDPC_ON_BOTH;
  }
}

static bool Backlight_isStandby(void)
{
  if(BacklightTimeout==0) return(true);
  return(false);
}

// -----------------------------

static EProcStateFileListMode FileList_Mode;

static void MoveUpFolder(void);
static void MoveFolder(void);
static void StartApplication(void);

static bool BGM_isOpened(void);
static u32 BGM_GetFilesCount(void);
static u32 BGM_GetCurrentIndex(void);
static const UnicodeChar* BGM_GetCurrentFilename(void);
static const UnicodeChar* BGM_GetCurrentPath(void);

#include "proc_FileList_ScrollBar.h"
#include "proc_FileList_Clock.h"
#include "proc_FileList_NDSFiles_TextPool.h"
#include "proc_FileList_NDSFiles.h"
#include "proc_FileList_SJIS2Unicode.h"
#include "proc_FileList_FileList.h"
#include "proc_FileList_BGM.h"
#include "proc_FileList_Popup.h"
#include "proc_FileList_IPK.h"

// -----------------------------
// -----------------------------

// -----------------------------

static void ProcState_RefreshSave(void)
{
  if(ProcState_RequestSave==false) return;
  
  u32 fileidx=ScrollBar.SelectedIndex;
  TNDSFile *pndsf=&pNDSFiles[fileidx];
  
  Unicode_Copy(ProcState.FileList.SelectFilenameUnicode,pndsf->pFilenameUnicode);
  ProcState.FileList.SelectWindowTopOffset=(ScrollBar.ItemHeight*fileidx)-ScrollBar.TopPos;
  ProcState.FileList.Mode=FileList_Mode;
  
  ProcState_Save();
  
  ChangedCurrentPath=true;
}

// -----------------------------

static void FileListInit(void)
{
//  PrfStart();
    NDSFiles_RefreshCurrentFolder();
//  PrfEnd(0);
    _consolePrintf("\n nnn1");
  s32 ItemHeight=0;
  FileList_Mode = EPSFLM_Single;
  switch(FileList_Mode){
    case EPSFLM_Single: {
      ItemHeight=NDSROMIcon16Height+2;
    } break;
    case EPSFLM_Double: {
      ItemHeight=NDSROMIcon32Height+2;
    } break;
  }
  
  ScrollBar_Free(&ScrollBar);
  ScrollBar_Init(&ScrollBar,ItemHeight);
  
  ScrollBar.TopPos=0;
  ScrollBar.ShowPos=ScrollBar.TopPos;
  ScrollBar.MaxPos=ScrollBar.ItemHeight*NDSFilesCount;
  _consolePrintf("\n nnn2");
  for(u32 idx=0;idx<NDSFilesCount;idx++){
    TNDSFile *pndsf=&pNDSFiles[idx];
    if(GetFlag2()){
	    if(Unicode_isEqual(pndsf->pFilenameUnicode,RomSetInfo.FileName)==true){
	      ScrollBar_SetSelectedIndex(&ScrollBar,idx);
	      ScrollBar_SetDirectTopPos(&ScrollBar,(ScrollBar.ItemHeight*idx)-ProcState.FileList.SelectWindowTopOffset);
	      ScrollBar.ShowPos=ScrollBar.TopPos;
	    }
	    
    }
    else if(GetFlag3())
    {
    	if(Unicode_isEqual(pndsf->pFilenameUnicode,SetupRom.FileName)==true){
    		      ScrollBar_SetSelectedIndex(&ScrollBar,idx);
    		      ScrollBar_SetDirectTopPos(&ScrollBar,(ScrollBar.ItemHeight*idx)-ProcState.FileList.SelectWindowTopOffset);
    		      ScrollBar.ShowPos=ScrollBar.TopPos;
    		    }
    	
    }
    else
    {
    	if(Unicode_isEqual(pndsf->pFilenameUnicode,ProcState.FileList.SelectFilenameUnicode)==true){
    		      ScrollBar_SetSelectedIndex(&ScrollBar,idx);
    		      ScrollBar_SetDirectTopPos(&ScrollBar,(ScrollBar.ItemHeight*idx)-ProcState.FileList.SelectWindowTopOffset);
    		      ScrollBar.ShowPos=ScrollBar.TopPos;
    		    }
    }
  }
  ClearFlag2();
  ClearFlag3();
  
  _consolePrintf("\n nnn3");
}

static s32 TopPosStacks[16]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static void TopPosStacks_Set(UnicodeChar *pPathUnicode,s32 TopPos)
{
//  const char *papath=StrConvert_Unicode2Ank_Test(pPathUnicode);
  
  u32 pathdepth=0;
  if((pPathUnicode[0]==0)||(pPathUnicode[1]==0)){
    }else{
    while(*pPathUnicode!=(UnicodeChar)0){
      UnicodeChar uc=*pPathUnicode++;
      if(uc==(UnicodeChar)'/') pathdepth++;
    }
  }
  
  if(16<=pathdepth) return;
  
//  _consolePrintf("TopPosStacks_Set: pathdepth=%d TopPos=%d [%s]\n",pathdepth,TopPos,papath);
  
  TopPosStacks[pathdepth]=TopPos;
}

static s32 TopPosStacks_Get(UnicodeChar *pPathUnicode)
{
//  const char *papath=StrConvert_Unicode2Ank_Test(pPathUnicode);
  
  u32 pathdepth=0;
  if((pPathUnicode[0]==0)||(pPathUnicode[1]==0)){
    }else{
    while(*pPathUnicode!=(UnicodeChar)0){
      UnicodeChar uc=*pPathUnicode++;
      if(uc==(UnicodeChar)'/') pathdepth++;
    }
  }
  
  if(16<=pathdepth) return(-1);
  
//  _consolePrintf("TopPosStacks_Get: pathdepth=%d TopPos=%d [%s]\n",pathdepth,TopPosStacks[pathdepth],papath);
  
  return(TopPosStacks[pathdepth]);
}

static void MoveUpFolder(void)
{
  if((ProcState.FileList.CurrentPathUnicode[0]==0)||(ProcState.FileList.CurrentPathUnicode[1]==0)) return;
  
  if(ProcState.FileList.MoveFolderLocked==true){
    Sound_Start(WAVFN_Notify);
    return;
  }
  
  Sound_Start(WAVFN_MovePage);
  ChangedCurrentPath=true;
  
  TopPosStacks_Set(ProcState.FileList.CurrentPathUnicode,-1);
  
  UnicodeChar *ptaguni=ProcState.FileList.CurrentPathUnicode;
  
  u32 slashidx=0;
  u32 idx=0;
  while(ptaguni[idx]!=(UnicodeChar)0){
    if(ptaguni[idx]==(UnicodeChar)'/') slashidx=idx;
    idx++;
  }
  
  Unicode_Copy(ProcState.FileList.SelectFilenameUnicode,&ptaguni[slashidx+1]);
  ProcState.FileList.SelectWindowTopOffset=ScrollBar.ItemHeight;
  
  if(slashidx==0){
    ptaguni[0]=(UnicodeChar)'/';
    ptaguni[1]=(UnicodeChar)0;
    }else{
    ptaguni[slashidx]=(UnicodeChar)0;
  }
  
  FileListInit();
  
  s32 TopPos=TopPosStacks_Get(ProcState.FileList.CurrentPathUnicode);
  if(TopPos!=-1){
    ScrollBar_SetDirectTopPos(&ScrollBar,TopPos);
    ScrollBar.ShowPos=ScrollBar.TopPos;
  }
  
  ProcState_RequestSave=true;
  
  RequestRefreshPlayCursorIndex=true;
  
  SetProcFadeEffect(EPFE_CrossFade);
  ScreenRedrawFlag=true;
  ForceUpdateSubScreenFlag=true;
}

static void MoveFolder(void)
{
  if(ProcState.FileList.MoveFolderLocked==true){
    Sound_Start(WAVFN_Notify);
    return;
  }
  
  Sound_Start(WAVFN_MovePage);
  ChangedCurrentPath=true;
  
  TopPosStacks_Set(ProcState.FileList.CurrentPathUnicode,ScrollBar.TopPos);
  
  TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  
  UnicodeChar *ptaguni=ProcState.FileList.CurrentPathUnicode;
  
  if(ptaguni[1]!=(UnicodeChar)0){
    UnicodeChar uslash[2]={(UnicodeChar)'/',(UnicodeChar)0};
    Unicode_Add(ptaguni,uslash);
  }
  
  Unicode_Add(ptaguni,pndsf->pFilenameUnicode);
  
  ProcState.FileList.SelectFilenameUnicode[0]=(UnicodeChar)0;
  ProcState.FileList.SelectWindowTopOffset=0;
  
  FileListInit();
  
  s32 TopPos=TopPosStacks_Get(ProcState.FileList.CurrentPathUnicode);
  if(TopPos!=-1){
    ScrollBar_SetDirectTopPos(&ScrollBar,TopPos);
    ScrollBar.ShowPos=ScrollBar.TopPos;
  }
  
  ProcState_RequestSave=true;
  
  RequestRefreshPlayCursorIndex=true;
  
  SetProcFadeEffect(EPFE_CrossFade);
  ScreenRedrawFlag=true;
  ForceUpdateSubScreenFlag=true;
}

static void StartApplication(void)
{
  TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  
  switch(pndsf->FileType){
    case ENFFT_UnknownFile: break;
    case ENFFT_UpFolder: MoveUpFolder(); return; break;
    case ENFFT_Folder: MoveFolder(); return; break;
    case ENFFT_Sound: {
      if(DLLList_isSupportFormatExt32(pndsf->Ext32)==EPT_Sound){
        BGM_Start(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
        ProcState_RequestSave=true;
        ProcState_RefreshSave();
        ScreenRedrawFlag=true;
        ForceUpdateSubScreenFlag=true;
        return;
      }
    } break;
    case ENFFT_Image: {
      Sound_Start(WAVFN_Click);
      ProcState_RequestSave=true;
      Unicode_Copy(RelationalFilePathUnicode,ProcState.FileList.CurrentPathUnicode);
      Unicode_Copy(RelationalFileNameUnicode,pndsf->pFilenameUnicode);
      RelationalFilePos=0;
      SetNextProc(ENP_ImageView,EPFE_None);
      return;
    } break;
    case ENFFT_Text: {
      Sound_Start(WAVFN_Click);
      ProcState_RequestSave=true;
      Unicode_Copy(RelationalFilePathUnicode,ProcState.FileList.CurrentPathUnicode);
      Unicode_Copy(RelationalFileNameUnicode,pndsf->pFilenameUnicode);
      RelationalFilePos=0;
      ManualTextEncode=ETE_Auto;
      ManualTextEncode_OverrideFlag=false;
      SetNextProc(ENP_TextView,EPFE_CrossFade);
      return;
    } break;
    case ENFFT_Video: {
      Sound_Start(WAVFN_Click);
      ProcState_RequestSave=true;
      Unicode_Copy(RelationalFilePathUnicode,ProcState.FileList.CurrentPathUnicode);
      Unicode_Copy(RelationalFileNameUnicode,pndsf->pFilenameUnicode);
      RelationalFilePos=0;
      SetNextProc(ENP_DPGPlay,EPFE_None);
      return;
    } break;
    case ENFFT_NDSROM: {
      Sound_Start(WAVFN_Click);
      ProcState_RequestSave=true;
      BootROM_SetInfo(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
      return;
    } break;
    case ENFFT_GBA: {
         Sound_Start(WAVFN_Click);
         ProcState_RequestSave=true;
         //BootROM_SetInfo(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
         const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
		 Unicode_Copy(GBAInfo.FileName,pndsf->pFilenameUnicode);
		 strcpy(GBAInfo.FullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));	
		 SetNextProc(ENP_GBA,EPFE_CrossFade);
    } break;
    case ENFFT_Skin: {
      const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
      
      if(Skin_SetFilename(pFullPathUnicode)==true){
        Unicode_Copy(ProcState.System.SkinFilenameUnicode,pFullPathUnicode);
        ProcState_RequestSave=true;
      
        SetNextProc(ENP_FileList,EPFE_CrossFade);
        return;
      }
    } break;
  }
  
  Sound_Start(WAVFN_MovePage);
  ChangedCurrentPath=true;
  
  _consolePrintf("StartApplication: Not support function.\n");
  if(pndsf->FileType!=ENFFT_Sound) ProcState_RequestSave=true;
}

// -----------------------------

static void MP3Cnt_Exec_Prev(void)
{
  Sound_Start(WAVFN_Click);
  Popup_Show_Prev();
  BGM_Prev();
}

static void MP3Cnt_Exec_Next(bool RelationalPlayMode)
{
  if(RelationalPlayMode==false) Sound_Start(WAVFN_Click);
  Popup_Show_Next();
  
  if(RelationalPlayMode==false){
    BGM_Next();
    return;
  }
  
  switch(ProcState.FileList.PlayMode){
    case EPSFLPM_Repeat: {
      BGM_NextRepeat();
    } break;
    case EPSFLPM_AllRep: {
      BGM_Next();
    } break;
    case EPSFLPM_Random: {
      BGM_NextRandom();
    } break;
  }
}

static void MP3Cnt_Exec_ChangePause(void)
{
  if(GlobalPauseFlag==false){
    GlobalPauseFlag=true;
    Sound_Start(WAVFN_Click);
    }else{
    GlobalPauseFlag=false;
  }
  Popup_Show_Pause();
}

static void MP3Cnt_Exec_ChangePlayMode(void)
{
  TProcState_FileList *pfl=&ProcState.FileList;
  
  switch(pfl->PlayMode){
    case EPSFLPM_Repeat: pfl->PlayMode=EPSFLPM_AllRep; break;
    case EPSFLPM_AllRep: pfl->PlayMode=EPSFLPM_Random; break;
    case EPSFLPM_Random: pfl->PlayMode=EPSFLPM_Repeat; break;
    default: pfl->PlayMode=EPSFLPM_Repeat; break;
  }
  
  Popup_Show_PlayMode();
  
  ProcState_RequestSave=true;
  ProcState_Save();
}

//CHEAT WINDOWNS...added by mj  20090427------------------
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
//CHEAT WINDOWNS END...added by mj  20090427---------------------------
// -----------------------------

static void CB_ExternalPowerAttach(void)
{
  if(PanelClosePowerOffTimeOut!=0){
    if(BacklightTimeout==0){
      //2009.04.09 modify by mj 取消超时关主屏电源
      IPC6->LCDPowerControl=LCDPC_ON_BOTH;
      }else{
      IPC6->LCDPowerControl=LCDPC_ON_BOTH;
    }
    GlobalPauseFlag=false;
    PanelClosePowerOffTimeOut=0;
  }
}

static void CB_ExternalPowerDetach(void)
{
  IPC6->LCDPowerControl=LCDPC_OFF_BOTH;
  GlobalPauseFlag=true;
  PanelClosePowerOffTimeOut=10*60*60;
}

static void CB_PanelClose(void)
{
  if(ShellSet.CarSupply==true){
    IPC6->LCDPowerControl=LCDPC_OFF_BOTH;
    GlobalPauseFlag=true;
    PanelClosePowerOffTimeOut=1;
  }
  return;
  
  CB_ExternalPowerDetach();
}

static void CB_PanelOpen(void)
{
  CB_ExternalPowerAttach();
}

// -----------------------------
extern void TestPauseKey(void);
#include "../proc_cheat/proc_Cheat.h"
#include "../proc_romset/proc_romset_bin.h"

static bool isUserCheatDataExits()
{
	const char *pfn;	  
	pfn=DefaultDataPath "/" CheatDataFilenme;
	if(FullPath_FileExistsAnsi(pfn))
		return true;
	else
		return false;
}
static bool isSkinUserExits()
{
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
static void CB_KeyPress(u32 VsyncCount,u32 Keys)
{
  if(PanelClosePowerOffTimeOut!=0){
    CB_ExternalPowerAttach();
    return;
  }
  
  if((Keys&KEY_TOUCH)!=0) return;
  
  {
    if(Backlight_isStandby()==true){
      Backlight_ResetTimer();
      return;
    }
    Backlight_ResetTimer();
  }
  
  if(((Keys&KEY_R)!=0) && ((Keys&KEY_A)!=0)){//RunGBA();
	 
	  RunGBA(0,0x10);
	  /*
    s32 ofs=DLLSound_GetPosOffset(),max=DLLSound_GetPosMax();
    s32 diff=max/100;
    s32 val=0;
    if((Keys&KEY_LEFT)!=0) val=-1;
    if((Keys&KEY_RIGHT)!=0) val=1;
    if((Keys&KEY_UP)!=0) val=-5;
    if((Keys&KEY_DOWN)!=0) val=5;
    if(val!=0){
      Popup_Show_Seek(val);
      diff*=val;
      if(diff==0) diff=val;
      ofs+=diff;
      if(ofs<0) ofs=0;
      if(max<ofs) ofs=max;
      if(diff<=0){
        ofs&=~3;
        }else{
        ofs=(ofs+3)&~3;
        if(max<ofs) ofs=max;
      }
      DLLSound_SetPosOffset(ofs);
      ScreenRedrawFlag=true;
      ForceUpdateSubScreenFlag=true;
    }
    return;
  */}
  //static int mLevBrightness = 3;
  if((Keys&KEY_L)!=0){
	  /*if(mLevBrightness ==3)
		  mLevBrightness = 0;
	  else 
		  mLevBrightness++;
	  IPC6->Brightness=mLevBrightness;
	  ProcState_RequestSave=true;    
	  ProcState_Save();*/

	 ChangeNextBacklightLevel();
	 ProcState_Save();
	  /*
    if((IPC6->PanelOpened==false)&&(ProcState.FileList.DisableLRKeyOnPanelClosed==true)) return;
    if((Keys&KEY_LEFT)!=0){
      Sound_Start(WAVFN_Click);
      switch(MP3Cnt_WindowState){
        case EMCWS_Hide: MP3Cnt_WindowState=EMCWS_Show; break;
        case EMCWS_Show: MP3Cnt_WindowState=EMCWS_Hide; break;
      }
      ScreenRedrawFlag=true;
    }
    if((Keys&(KEY_RIGHT|KEY_A))!=0){
      MP3Cnt_Exec_Next(false);
      ScreenRedrawFlag=true;
    }
    if((Keys&KEY_B)!=0){
      MP3Cnt_Exec_Prev();
      ScreenRedrawFlag=true;
    }
    if((Keys&(KEY_X|KEY_Y))!=0){
      if((Keys&KEY_X)!=0) ChangeNextBacklightLevel();
      if((Keys&KEY_Y)!=0) ChangePrevBacklightLevel();
      Popup_Show_BacklightLevel();
    }
    if((Keys&KEY_UP)!=0){
      MP3Cnt_Exec_ChangePause();
      ScreenRedrawFlag=true;
    }
    if((Keys&KEY_DOWN)!=0){
      MP3Cnt_Exec_ChangePlayMode();
      ScreenRedrawFlag=true;
    }
    return;
  */}
  
  if((Keys&KEY_SELECT)!=0){
	  

	  /*TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
		if(pndsf->FileType==ENFFT_NDSROM)
		{
			const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
			Unicode_Copy(RomSetInfo.FileName,pndsf->pFilenameUnicode);
			strcpy(RomSetInfo.FullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));		
			_consolePrintf("\n FileName=%s", StrConvert_Unicode2Ank_Test(RomSetInfo.FileName));
			_consolePrintf("\n FullPath=%s",RomSetInfo.FullPathAlias);  			
			strcpy(pNDSAlis,RomSetInfo.FullPathAlias);
			if(isHomeBrew())
				return;				
			SetFlag2();
			SetNextProc(ENP_ROMSET,EPFE_CrossFade);
			
			//TestPauseKey();
			//TestPauseKey();
			
		}*/	

	  SetNextProc(ENP_ReMenu,EPFE_CrossFade); 
      //Sound_Start(WAVFN_Click);
      
    //ProcState.System.LastState=ELS_Launch;
    
    //ProcState_RequestSave=true;
    
    //SetNextProc(ENP_Launch,EPFE_CrossFade);
    //Sound_Start(WAVFN_Click);
  }
/*
  if((Keys&KEY_SELECT)!=0){
    IPC6->SoundChannels++;
    if(IPC6->SoundChannels==3) IPC6->SoundChannels=0;
  }
*/
  
  TScrollBar *psb=&ScrollBar;
  
  s32 pagesize=((psb->ClientSize+(psb->ItemHeight-1))/psb->ItemHeight)/2;
  
  if((Keys&KEY_UP)!=0){
//    Sound_Start(WAVFN_CurDown);
//    ChangedCurrentPath=true;
    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex-1);
    ScreenRedrawFlag=true;
    SetClearAllFlg();
  }
  
  if((Keys&KEY_LEFT)!=0){
//    Sound_Start(WAVFN_CurDown);
//    ChangedCurrentPath=true;
    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex-pagesize);
    ScreenRedrawFlag=true;
    SetClearAllFlg();
  }
  
  if((Keys&KEY_DOWN)!=0){
//    Sound_Start(WAVFN_CurDown);
//    ChangedCurrentPath=true;
    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+1);
    ScreenRedrawFlag=true;
    SetClearAllFlg();
  }
  
  if((Keys&KEY_RIGHT)!=0){
//    Sound_Start(WAVFN_CurDown);
//    ChangedCurrentPath=true;
    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+pagesize);
    ScreenRedrawFlag=true;
    SetClearAllFlg();
  }
  
  if((Keys&KEY_A)!=0){
    StartApplication();
    ScreenRedrawFlag=true;
    TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  }
  
  if((Keys&KEY_B)!=0){
    if(ShellSet.BButtonToStopFunc==true){
      if(BGM_isOpened()==false){
        MoveUpFolder();
        }else{
        BGM_Stop(false);
        Sound_Start(WAVFN_Click);
      }
      }else{
      if(ProcState.FileList.MoveFolderLocked==false){
        MoveUpFolder();
        }else{
        BGM_Stop(false);
        Sound_Start(WAVFN_Click);
      }
    }
  }
  
  if((Keys&KEY_START)!=0){
  
	/*
    ProcState_RequestSave=true;
    TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
    Unicode_Copy(RelationalFilePathUnicode,ProcState.FileList.CurrentPathUnicode);
    Unicode_Copy(RelationalFileNameUnicode,pndsf->pFilenameUnicode);
    RelationalFilePos=0;
    SetNextProc(ENP_SysMenu,EPFE_CrossFade);
  */}
  TProcState *CurProcState;
  CurProcState = &ProcState; 
  if( ((Keys & KEY_Y)!=0) && ((Keys&KEY_L)!=0) )
  {
	  if(!GetY1Flag())
	  {
		  TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  		  if((pndsf->FileType!=ENFFT_UpFolder) && (pndsf->FileType!=ENFFT_Folder)  )
  		  {
  			  //记录文件别名
  			  const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
   			  Unicode_Copy(CpyCutFileInfo.FileName,pndsf->pFilenameUnicode);
  			  Unicode_Copy(CpyCutFileInfo.SrcPath,ProcState.FileList.CurrentPathUnicode);
   			  strcpy(CpyCutFileInfo.SrcFullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));	
  		  }
		  SetY1Flag(); 
	  }  
	  else
	  {
		  //开始复制文件到目的位置
		 //TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
		  //const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,CpyCutFileInfo.FileName);
		  //strcpy(CpyCutFileInfo.DesFullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));	
		  Unicode_Copy(CpyCutFileInfo.DesPath,ProcState.FileList.CurrentPathUnicode);
		  //不是同一目录
  		  if(!Unicode_isEqual_NoCaseSensitive(CpyCutFileInfo.DesPath,CpyCutFileInfo.SrcPath))
  		  {
  			  SetY2Flag();
  			  //弹出删除菜单
  			  SetNextProc(ENP_CpyCut,EPFE_CrossFade);
  		  }		
	  }
  }
  if( ((Keys & KEY_X)!=0) && ((Keys&KEY_L)!=0) )
  {
	  //SetNextProc(ENP_SetupW,EPFE_CrossFade); 
		/*{
			TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
			if(pndsf->FileType==ENFFT_NDSROM)
			{
				const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
				Unicode_Copy(SetupRom.FileName,pndsf->pFilenameUnicode);
				strcpy(SetupRom.FullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));		
				_consolePrintf("\n FileName=%s", StrConvert_Unicode2Ank_Test(SetupRom.FileName));
				_consolePrintf("\n FullPath=%s",SetupRom.FullPathAlias);  
				SetFlag3();	  						
			}	
		}
		SetNextProc(ENP_SetupW,EPFE_CrossFade);*/
	  if(!GetY1Flag())
	  {
		  ////开始删除文件到目的位置
		  TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
		  if((pndsf->FileType!=ENFFT_UpFolder) && (pndsf->FileType!=ENFFT_Folder)  )
		  {
			  //记录文件别名
			  const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
		      Unicode_Copy(CpyCutFileInfo.FileName,pndsf->pFilenameUnicode);
		      strcpy(CpyCutFileInfo.SrcFullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));		
		      SetX1Flag();
			  //弹出删除菜单
			  SetNextProc(ENP_CpyCut,EPFE_CrossFade);
		  }
	  }
	  else
	  {
		  //开始剪切文件到目的位置
		  //TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  		  //const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,CpyCutFileInfo.FileName);
  		 // strcpy(CpyCutFileInfo.DesFullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));	
  		  Unicode_Copy(CpyCutFileInfo.DesPath,ProcState.FileList.CurrentPathUnicode);			  
		  //不是同一目录
		  if(!Unicode_isEqual_NoCaseSensitive(CpyCutFileInfo.DesPath,CpyCutFileInfo.SrcPath))
		  {
			  SetX2Flag();
			  //弹出删除菜单
			  SetNextProc(ENP_CpyCut,EPFE_CrossFade);
		  }
	  }

  }
  
  if((Keys&(KEY_Y|KEY_X))!=0){
	  /*
    s32 Volume=ProcState.System.Volume64;    
    if(Keys==(KEY_Y|KEY_X)){
      Volume=64;
      }else{
      if((Keys&KEY_Y)!=0) Volume-=2;
      if((Keys&KEY_X)!=0) Volume+=2;
      
      if(Volume<0) Volume=0;
      if(Volume>strpcmVolumeMax) Volume=strpcmVolumeMax;
    }    
    Volume&=~1;
    strpcmSetVolume64(Volume);
    ProcState.System.Volume64=Volume;
    ProcState_RequestSave=true;    
    Popup_Show_Volume();    
    ScreenRedrawFlag=true;
    ForceUpdateSubScreenFlag=true;
  */}
}

static void CB_KeySameLRDown(void)
{
  MP3Cnt_Exec_ChangePause();
  ScreenRedrawFlag=true;
}

static bool isPressMouseButton;
static bool isShowErrorDialog;
extern void TestPauseKey(void);
static void mainloop_autoboot(const char *pFilename)
{
  const UnicodeChar PathUnicode[2]={(UnicodeChar)'/',0};
  UnicodeChar FilenameUnicode[16];
  StrConvert_Ank2Unicode(pFilename,FilenameUnicode);
  if(FileExistsUnicode(PathUnicode,FilenameUnicode)==true){
    _consolePrintf("Auto booting... [/%s]\n",pFilename);
    BootROM_SetInfo_NoLaunch(PathUnicode,FilenameUnicode);
  }
}
static void CB_MouseDown(s32 x,s32 y)
{
  if(Backlight_isStandby()==true) return;
  
  isPressMouseButton=true;
  
  if(isShowErrorDialog==true){
    isShowErrorDialog=false;
    pScreenMainOverlay->pCanvas->FillFull(0);
    return;
  }
  
  if(MP3Cnt_WindowState==EMCWS_Show){
    if((ScreenWidth-MP3Cnt_Width)<=x){
      u32 hlst[5];
      hlst[0]=MP3CntAlpha_GetSkin(EMP3SA_p0)->GetHeight();
      hlst[1]=MP3CntAlpha_GetSkin(EMP3SA_p1_prev)->GetHeight();
      hlst[2]=MP3CntAlpha_GetSkin(EMP3SA_p2_stop)->GetHeight();
      hlst[3]=MP3CntAlpha_GetSkin(EMP3SA_p3_next)->GetHeight();
      hlst[4]=MP3CntAlpha_GetSkin(EMP3SA_p4_repeat)->GetHeight();
      s32 exec=-1;
      u32 h=0;
      for(s32 idx=0;idx<5;idx++){
        h+=hlst[idx];
        if(y<h){
          exec=idx;
          break;
        }
      }
      if(exec!=-1){
        switch(exec){
          case 0: /* MP3Cnt_Exec_ChangeAuto(); */ break;
          case 1: MP3Cnt_Exec_Prev(); break;
          case 2: MP3Cnt_Exec_ChangePause(); break;
          case 3: MP3Cnt_Exec_Next(false); break;
          case 4: MP3Cnt_Exec_ChangePlayMode(); break;
        }
        ScreenRedrawFlag=true;
        return;
      }
    }
  }

  //-----------------------------------------------------------------
   	TiniSkinUser *SkinUser=&GlobalUserINI; 
   	if(SkinUser->LightControl.x != -1)
   	{
   		if((x>SkinUser->LightControl.x)&&(x<(SkinUser->LightControl.x+SkinUser->LightControl.width)))
   		{
   	 		if((y>SkinUser->LightControl.y)&&(y<(SkinUser->LightControl.y+SkinUser->LightControl.height)))
   	 		{
   	 			if(isPress_LightControl ==false)
  				{
  					isPress_LightControl = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ToggleControl.x != -1)
   	{
   		if((x>SkinUser->ToggleControl.x)&&(x<(SkinUser->ToggleControl.x+SkinUser->ToggleControl.width)))
   		{
   	 		if((y>SkinUser->ToggleControl.y)&&(y<(SkinUser->ToggleControl.y+SkinUser->ToggleControl.height)))
   	 		{
   	 			if(isPress_ToggleControl ==false)
  				{
  					isPress_ToggleControl = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->HelpControl.x != -1)
   	{
   		if((x>SkinUser->HelpControl.x)&&(x<(SkinUser->HelpControl.x+SkinUser->HelpControl.width)))
   		{
   	 		if((y>SkinUser->HelpControl.y)&&(y<(SkinUser->HelpControl.y+SkinUser->HelpControl.height)))
   	 		{
   	 			if(isPress_HelpControl ==false)
  				{
  					isPress_HelpControl = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->SystemControl.x != -1)
   	{
   		if((x>SkinUser->SystemControl.x)&&(x<(SkinUser->SystemControl.x+SkinUser->SystemControl.width)))
   		{
   	 		if((y>SkinUser->SystemControl.y)&&(y<(SkinUser->SystemControl.y+SkinUser->SystemControl.height)))
   	 		{
   	 			if(isPress_SystemControl ==false)
  				{
  					isPress_SystemControl = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ShutDownContol.x != -1)
   	{
   		if((x>SkinUser->ShutDownContol.x)&&(x<(SkinUser->ShutDownContol.x+SkinUser->ShutDownContol.width)))
   		{
   	 		if((y>SkinUser->ShutDownContol.y)&&(y<(SkinUser->ShutDownContol.y+SkinUser->ShutDownContol.height)))
   	 		{
   	 			if(isPress_ShutDownContol ==false)
  				{
  					isPress_ShutDownContol = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ScrollUP.x != -1)
   	{
   		if((x>SkinUser->ScrollUP.x)&&(x<(SkinUser->ScrollUP.x+SkinUser->ScrollUP.width)))
   		{
   	 		if((y>SkinUser->ScrollUP.y)&&(y<(SkinUser->ScrollUP.y+SkinUser->ScrollUP.height)))
   	 		{
   	 			if(isPress_ScrollUP ==false)
  				{
  					isPress_ScrollUP = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ScrollDOWN.x != -1)
   	{
   		if((x>SkinUser->ScrollDOWN.x)&&(x<(SkinUser->ScrollDOWN.x+SkinUser->ScrollDOWN.width)))
   		{
   	 		if((y>SkinUser->ScrollDOWN.y)&&(y<(SkinUser->ScrollDOWN.y+SkinUser->ScrollDOWN.height)))
   	 		{
   	 			if(isPress_ScrollDOWN ==false)
  				{
  					isPress_ScrollDOWN = true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->Launch.x != -1)
   	{
   		if((x>SkinUser->Launch.x)&&(x<(SkinUser->Launch.x+SkinUser->Launch.width)))
   		{
   	 		if((y>SkinUser->Launch.y)&&(y<(SkinUser->Launch.y+SkinUser->Launch.height)))
   	 		{
   	 			if(isPress_Launch ==false)
  				{
  					isPress_Launch = true;
  				}
   	 		}
   		} 		
   	}
   /*	if(SkinUser->Rumble.x != -1)
   	{
   		if((x>SkinUser->Rumble.x)&&(x<(SkinUser->Rumble.x+SkinUser->Rumble.width)))
   		{
   	 		if((y>SkinUser->Rumble.y)&&(y<(SkinUser->Rumble.y+SkinUser->Rumble.height)))
   	 		{
   	 			if(isPress_Rumble ==false)
  				{
  					isPress_Rumble = true;
  				}
   	 		}
   		} 		
   	}*/
   	//-----------------------------------------------------------------  
 

  if(ScrollBar_MouseDown(&ScrollBar,x,y)==true) return;
  if(FileList_MouseDown(&ScrollBar,x,y)==true) return;
}

static void CB_MouseMove(s32 x,s32 y)
{
  if(Backlight_isStandby()==true) return;
  
  if(ScrollBar_MouseMove(&ScrollBar,x,y)==true) return;
  if(FileList_MouseMove(&ScrollBar,x,y)==true) return;
}

static void CB_MouseUp(s32 x,s32 y)
{
	TScrollBar *psb=&ScrollBar;
	if(Backlight_isStandby()==true){
		Backlight_ResetTimer();
		return;
	}
  
 	isPressMouseButton=false;

//-----------------------------------------------------------------
   	TiniSkinUser *SkinUser=&GlobalUserINI; 
   	if(SkinUser->LightControl.x != -1)
   	{
   		if((x>SkinUser->LightControl.x)&&(x<(SkinUser->LightControl.x+SkinUser->LightControl.width)))
   		{
   	 		if((y>SkinUser->LightControl.y)&&(y<(SkinUser->LightControl.y+SkinUser->LightControl.height)))
   	 		{
   	 			if(isPress_LightControl ==true)
  				{
  					isPress_LightControl = false;
  					ChangeNextBacklightLevel();
  					ProcState_Save();
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ToggleControl.x != -1)
   	{
   		if((x>SkinUser->ToggleControl.x)&&(x<(SkinUser->ToggleControl.x+SkinUser->ToggleControl.width)))
   		{
   	 		if((y>SkinUser->ToggleControl.y)&&(y<(SkinUser->ToggleControl.y+SkinUser->ToggleControl.height)))
   	 		{
   	 			if(isPress_ToggleControl ==true)
  				{
  					
  					{
  						isPress_ToggleControl = false;
	  					TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
	  					if(pndsf->FileType==ENFFT_NDSROM)
						{
	  						const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
	  						Unicode_Copy(RomSetInfo.FileName,pndsf->pFilenameUnicode);
	  						strcpy(RomSetInfo.FullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));		
	  						_consolePrintf("\n FileName=%s", StrConvert_Unicode2Ank_Test(RomSetInfo.FileName));
	  						_consolePrintf("\n FullPath=%s",RomSetInfo.FullPathAlias);  
	  						strcpy(pNDSAlis,RomSetInfo.FullPathAlias);
	  						if(isHomeBrew())
	  							return;	
	  						SetFlag2();
	  						SetNextProc(ENP_ROMSET,EPFE_CrossFade);
						}	
  					}
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->HelpControl.x != -1)
   	{
   		if((x>SkinUser->HelpControl.x)&&(x<(SkinUser->HelpControl.x+SkinUser->HelpControl.width)))
   		{
   	 		if((y>SkinUser->HelpControl.y)&&(y<(SkinUser->HelpControl.y+SkinUser->HelpControl.height)))
   	 		{
   	 			if(isPress_HelpControl ==true)
  				{
  					isPress_HelpControl = false;
  					{
  						if(FullPath_FileExistsAnsi("/moonshl2.nds"))
  						{
  				            mainloop_autoboot("/moonshl2.nds");
  				            BootROM_SetInfo((UnicodeChar *)L"/",(UnicodeChar *)L"moonshl2.nds");
  						}
  						else
  						{
  							//提示去MoonShell网站下 MoonShell2 包
  							isMS2 = true ;
  						}
  					}
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->SystemControl.x != -1)
   	{
   		if((x>SkinUser->SystemControl.x)&&(x<(SkinUser->SystemControl.x+SkinUser->SystemControl.width)))
   		{
   	 		if((y>SkinUser->SystemControl.y)&&(y<(SkinUser->SystemControl.y+SkinUser->SystemControl.height)))
   	 		{
   	 			if(isPress_SystemControl ==true)
  				{
  					isPress_SystemControl = false;
  					{
  						TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  						if(pndsf->FileType==ENFFT_NDSROM)
						{
	  						const UnicodeChar *pFullPathUnicode=ConvertFull_MargeFromSplit(ProcState.FileList.CurrentPathUnicode,pndsf->pFilenameUnicode);
	  						Unicode_Copy(SetupRom.FileName,pndsf->pFilenameUnicode);
	  						strcpy(SetupRom.FullPathAlias,ConvertFullPath_Unicode2Alias(pFullPathUnicode));		
	  						_consolePrintf("\n FileName=%s", StrConvert_Unicode2Ank_Test(SetupRom.FileName));
	  						_consolePrintf("\n FullPath=%s",SetupRom.FullPathAlias);  
	  						SetFlag3();	  						
						}	
  					}
  					SetNextProc(ENP_SetupW,EPFE_CrossFade); 
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ShutDownContol.x != -1)
   	{
   		if((x>SkinUser->ShutDownContol.x)&&(x<(SkinUser->ShutDownContol.x+SkinUser->ShutDownContol.width)))
   		{
   	 		if((y>SkinUser->ShutDownContol.y)&&(y<(SkinUser->ShutDownContol.y+SkinUser->ShutDownContol.height)))
   	 		{
   	 			if(isPress_ShutDownContol ==true)
  				{
  					isPress_ShutDownContol = false;
  					SetNextProc(ENP_ShutDown,EPFE_CrossFade);
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ScrollUP.x != -1)
   	{
   		if((x>SkinUser->ScrollUP.x)&&(x<(SkinUser->ScrollUP.x+SkinUser->ScrollUP.width)))
   		{
   	 		if((y>SkinUser->ScrollUP.y)&&(y<(SkinUser->ScrollUP.y+SkinUser->ScrollUP.height)))
   	 		{
   	 			if(isPress_ScrollUP ==true)
  				{
  					isPress_ScrollUP = false;
  				    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex-1);
  				    ScreenRedrawFlag=true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->ScrollDOWN.x != -1)
   	{
   		if((x>SkinUser->ScrollDOWN.x)&&(x<(SkinUser->ScrollDOWN.x+SkinUser->ScrollDOWN.width)))
   		{
   	 		if((y>SkinUser->ScrollDOWN.y)&&(y<(SkinUser->ScrollDOWN.y+SkinUser->ScrollDOWN.height)))
   	 		{
   	 			if(isPress_ScrollDOWN ==true)
  				{
  					isPress_ScrollDOWN = false;
  				    ScrollBar_SetSelectedIndex(psb,psb->SelectedIndex+1);
  				    ScreenRedrawFlag=true;
  				}
   	 		}
   		} 		
   	}
   	if(SkinUser->Launch.x != -1)
   	{
   		if((x>SkinUser->Launch.x)&&(x<(SkinUser->Launch.x+SkinUser->Launch.width)))
   		{
   	 		if((y>SkinUser->Launch.y)&&(y<(SkinUser->Launch.y+SkinUser->Launch.height)))
   	 		{
   	 			if(isPress_Launch ==true)
  				{
  					isPress_Launch = false;
  				    StartApplication();
  				    ScreenRedrawFlag=true;
  				    TNDSFile *pndsf=&pNDSFiles[ScrollBar.SelectedIndex];
  				}
   	 		}
   		} 		
   	}
   /*	if(SkinUser->Rumble.x != -1)
   	{
   		if((x>SkinUser->Rumble.x)&&(x<(SkinUser->Rumble.x+SkinUser->Rumble.width)))
   		{
   	 		if((y>SkinUser->Rumble.y)&&(y<(SkinUser->Rumble.y+SkinUser->Rumble.height)))
   	 		{
   	 			if(isPress_Rumble ==true)
  				{
  					isPress_Rumble = false;
  				}
   	 		}
   		} 		
   	}*/
   	//-----------------------------------------------------------------  
   	isPress_LightControl = false;
   	isPress_ToggleControl = false;
   	isPress_HelpControl = false;
   	isPress_SystemControl = false;
   	isPress_ShutDownContol = false;
   	isPress_ScrollUP = false;
   	isPress_ScrollDOWN = false;
   	isPress_Rumble = false;
    isPress_Launch = false;

  if(ScrollBar_MouseUp(&ScrollBar,x,y)==true) return;
  if(FileList_MouseUp(&ScrollBar,x,y)==true) return;
}

static bool Process_SeekNext,Process_SeekPrev;
static u32 Process_WaitCount;

static void DrawOnlineHelp(void)
{
    if(Skin_OwnerDrawText.FileList_Top==true) return;
  
    CglB15 *pb15=FileList_GetSkin(EFLS_BG_TopMsg);
    
	if(GetClearAllFlgTrans())
	{
		SetClearAllFlg();
		ClearClearAllFlgTrans();
	}
    
   // CglTGF *pbm = ScrollBarAlpha_GetSkin(EFLSA_Icon_FILEINFO);
    //pbm->BitBlt(pb15->pCanvas,0,36);
    
    //显示加载数据
   // pb15->pCanvas->SetCglFont(pCglFontDefault);
   // pb15->pCanvas->SetFontTextColor(0x8000);//设置字体颜色黑色
    //pb15->pCanvas->TextOutUTF8(93,46,Lang_GetUTF8("IV_LoadMsg0"));
   // pb15->pCanvas->TextOutUTF8(93,46+20,Lang_GetUTF8("IV_LoadMsg1"));
    
   // pbm= ScrollBarAlpha_GetSkin(EFLSA_Icon_sun);
   // pbm->BitBlt(pb15->pCanvas,0,0);
    
  //  pbm= ScrollBarAlpha_GetSkin(EFLSA_Icon_reset0);
   // pbm->BitBlt(pb15->pCanvas,240,0);
    
   // pbm= ScrollBarAlpha_GetSkin(EFLSA_Btn_set0);
  //  pbm->BitBlt(pb15->pCanvas,56,154);
  //  pbm->BitBlt(pb15->pCanvas,140,154);
    
  //  pb15->pCanvas->SetFontTextColor(0x8000);
  //  pb15->pCanvas->TextOutUTF8_StartMid(56,154+6,68,Lang_GetUTF8("FL_Button_Setup"));
  //  pb15->pCanvas->TextOutUTF8_StartMid(140,154+6,68,Lang_GetUTF8("FL_Button_Cheat")); 
          

    /*//将Online显示去掉
    pb15->pCanvas->SetCglFont(pCglFontDefault);
  
    u32 x=8;
    u32 y=8;
    u32 h=glCanvasTextHeight+3;
  
    for(u32 idx=0;idx<12;idx++)
    {
      const char *pmsg=NULL;
      switch(idx){
#define Prefix "FL_"
      case 0: pmsg=Lang_GetUTF8(Prefix "Help0"); break;
      case 1: pmsg=Lang_GetUTF8(Prefix "Help1"); break;
      case 2: pmsg=Lang_GetUTF8(Prefix "Help2"); break;
      case 3: pmsg=Lang_GetUTF8(Prefix "Help3"); break;
      case 4: pmsg=Lang_GetUTF8(Prefix "Help4"); break;
      case 5: pmsg=Lang_GetUTF8(Prefix "Help5"); break;
      case 6: pmsg=Lang_GetUTF8(Prefix "Help6"); break;
      case 7: pmsg=Lang_GetUTF8(Prefix "Help7"); break;
      case 8: pmsg=Lang_GetUTF8(Prefix "Help8"); break;
      case 9: pmsg=Lang_GetUTF8(Prefix "Help9"); break;
      case 10: pmsg=Lang_GetUTF8(Prefix "Help10"); break;
      case 11: pmsg=Lang_GetUTF8(Prefix "Help11"); break;
#undef Prefix
    }
    if(pmsg!=NULL){
      if(idx==0){
        pb15->pCanvas->SetFontTextColor(ColorTable.FileList.HelpTop_Text);
        }else{
        pb15->pCanvas->SetFontTextColor(ColorTable.FileList.HelpBody_Text);
      }
      pb15->pCanvas->TextOutUTF8(x,y,pmsg);
    }
    y+=h;
  }
    */
}

extern void videoSub_SetShowLog(bool e);
bool first = true;
static void CB_Start(void)
{
	//videoSub_SetShowLog(true);  
	if(first)
	{//清空下屏切换到上屏的时候残像   
		vuint16*pv_main = pScreenMain->pViewCanvas->GetVRAMBuf(); 
		vuint16*pv_sub = pScreenSub->GetVRAMBuf();
		MemSet16CPU(0x8000,(void*)pv_main,0x18000);
		MemSet16CPU(0x8000,(void*)pv_sub,0x18000);
		//swiCopy((void*)pv_sub,(void*)pv_main,COPY_MODE_WORD|COPY_MODE_COPY|(0x18000>>2)); 
		first = false;
	} 
	ShellSet.SwapDisp = true ;    
		if(ShellSet.SwapDisp==true) REG_POWERCNT|=POWER_SWAP_LCDS;		    
	
    if(ErrorDialog_isExists()==false) Sound_Start(WAVFN_Open);
    ChangedCurrentPath=true;
  
    SJIS2Unicode_Init();
    if(Shell_isJPmode()==true) SJIS2Unicode_Load();
    
    if(1)//(isSkinUserExits())
    {
        InitINI_SkinUser(true);
        LoadINI_SkinUser();
     }
    else
    {
    	InitINI_SkinUser(false);
    }
    _consolePrintf("\n LightControl.x=%d",GlobalUserINI.LightControl.x);
    _consolePrintf("\n LightControl.y=%d",GlobalUserINI.LightControl.y);
    _consolePrintf("\n LightControl.png2=%s",GlobalUserINI.LightControl.png2);
    
    _consolePrintf("\n ShutDownContol.x=%d",GlobalUserINI.ShutDownContol.x);
    _consolePrintf("\n ShutDownContol.y=%d",GlobalUserINI.ShutDownContol.y);
    _consolePrintf("\n ShutDownContol.png2=%s",GlobalUserINI.ShutDownContol.png2);

    DrawOnlineHelp();
  
    RequestRefreshPlayCursorIndex=false;
    PlayCursorIndex=-1;

    Process_SeekNext=false;
    Process_SeekPrev=false;
    Process_WaitCount=0;

    PanelClosePowerOffTimeOut=0;
  
    InitIPK();
    BGM_Init();
  
    MP3Cnt_WindowState=EMCWS_Hide;
    MP3Cnt_PosX=0;
    MP3Cnt_PosForVSync=1;

    pScreenMainOverlay->pCanvas->FillFull(0);
    pScreenMainOverlay->SetPosition_for_Right64x192(0,0);
    pScreenMainOverlay->SetVisible_for_LeftTop128x64(false);
  
    if(ErrorDialog_isExists()==false)
    {
        isShowErrorDialog=false;
    }else
    {//
        Sound_Start(WAVFN_Notify);
        ChangedCurrentPath=true;
        isShowErrorDialog=true;
        ErrorDialog_Draw(pScreenMainOverlay->pCanvas);
        ErrorDialog_Clear();
    }
    Popup_Init();
    Clock_Init();
    Clock_Refresh();
    Backlight_ResetTimer();
  
    //文件列表初始化
    FileList_Mode=ProcState.FileList.Mode;
    FileListInit();
    isPressMouseButton=false;
  
    
    _consolePrintf("\n nnn4");
  if((Unicode_isEmpty(RelationalFilePathUnicode)==false)&&(Unicode_isEmpty(RelationalFileNameUnicode)==false)){
    BGM_Start(RelationalFilePathUnicode,RelationalFileNameUnicode);
    if(RelationalFilePos!=DLLSound_GetPosOffset()) DLLSound_SetPosOffset(RelationalFilePos);
    Resume_SetPos(RelationalFilePos);
    Resume_Save();
    RequestRefreshPlayCursorIndex=true;
    Backlight_SetLast1vsync();
  }
  RelationalFile_Clear();
  
  ChangedCurrentPath=true;
  
  ScreenRedrawFlag=true;
  ForceUpdateSubScreenFlag=true;
  
  
  _consolePrintf("\n nnn5");
  
  if(ScreenRedrawFlag==true){
    ScreenRedrawFlag=false;
    ForceUpdateSubScreenFlag=false;
    FileList_SubDrawBG(&ScrollBar);
    _consolePrintf("\n nnn6");
    FileList_MainDrawBG(&ScrollBar);
    _consolePrintf("\n nnn7");
    FileList_MainDrawBG(&ScrollBar);
    _consolePrintf("\n nnn8");
  }
}

static void CB_VsyncUpdate(u32 VsyncCount)
{
	//videoSub_SetShowLog(true);

	
	//用于字体滚动
	static int cntFont=0;	
	if(cntFont == 5)
	{
		//设置滚动标志
		cntFont = 0;
		SetloopFlg();
		_consolePrintf("\n loopFlg=%d",GetloopFlg());
		//TestPauseKey();
	}	
	cntFont++;	
	_consolePrintf("\n cnt=%d",cntFont);
	
		
	_consolePrintf("\n nnn9");
	if(VsyncCount==1) ProcState_Save();
  
  if(isPressMouseButton==true){
    Backlight_ResetTimer();
    }else{
    Backlight_VsyncUpdate(VsyncCount);
  }
  
  Popup_VsyncUpdate(VsyncCount);

  if(PanelClosePowerOffTimeOut!=0){
    if(PanelClosePowerOffTimeOut<VsyncCount){
      PanelClosePowerOffTimeOut=0;
      }else{
      PanelClosePowerOffTimeOut-=VsyncCount;
    }
    if(PanelClosePowerOffTimeOut==0){
      _consolePrintf("Panel closed timeout. Auto power off.\n");
      Sound_Start(WAVFN_PowerOff);
      u32 vsync=Sound_GetCurrentPlayTimePerVsync();
      _consolePrintf("Wait for terminate. (%d)\n",vsync);
      for(u32 w=0;w<vsync;w++){
        swiWaitForVBlank();
      }
      IPC6->LCDPowerControl=LCDPC_SOFT_POWEROFF;
      while(1);
    }
  }
  
  if(BGMResumeSaveTimeVSync!=0){
    BGMResumeSaveTimeVSync+=VsyncCount;
    if((60*5)<BGMResumeSaveTimeVSync){
      BGMResumeSaveTimeVSync=1;
      u32 pos=DLLSound_GetPosOffset();
      if(pos!=Resume_GetPos()){
        Resume_SetPos(pos);
        Resume_Save();
      }
    }
  }
  
  if((Process_SeekNext==true)||(Process_SeekPrev==true)){
    if(Process_WaitCount!=0){
      Process_WaitCount=10;
      }else{
      s32 ofs=DLLSound_GetPosOffset(),max=DLLSound_GetPosMax();
      s32 diff=max/100;
      s32 val=0;
      if(Process_SeekPrev==true) val=-1;
      if(Process_SeekNext==true) val=+1;
      if(val!=0){
        Popup_Show_Seek(val);
        diff*=val;
        if(diff==0) diff=val;
        ofs+=diff;
        if(ofs<0) ofs=0;
        if(max<ofs) ofs=max;
        if(diff<=0){
          ofs&=~3;
          }else{
          ofs=(ofs+3)&~3;
          if(max<ofs) ofs=max;
        }
        DLLSound_SetPosOffset(ofs);
        ScreenRedrawFlag=true;
        ForceUpdateSubScreenFlag=true;
      }
    }
  }
  
  if(PanelClosePowerOffTimeOut==0){
    for(u32 idx=0;idx<VsyncCount;idx++){
      ScrollBar_MouseIdle(&ScrollBar);
      FileList_MouseIdle(&ScrollBar);
    }
    
    if((BGM_isOpened()==true)&&(GlobalPauseFlag==false)){
      ScrollBar.ShowPos=ScrollBar.TopPos;
    }
    
    if(RequestRefreshPlayCursorIndex==true){
      RequestRefreshPlayCursorIndex=false;
      PlayCursorIndex=-1;
      if(BGM_isOpened()==true){
        if(Unicode_isEqual(ProcState.FileList.CurrentPathUnicode,BGM_GetCurrentPath())==true){
          for(s32 idx=0;idx<NDSFilesCount;idx++){
            TNDSFile *pNDSFile=&pNDSFiles[idx];
            if(Unicode_isEqual(pNDSFile->pFilenameUnicode,BGM_GetCurrentFilename())==true){
              PlayCursorIndex=idx;
              break;
            }
          }
        }
      }
    }
    
    {
      static u32 r=0;
      r+=VsyncCount;
      if(60<r){
        Clock_Refresh();
        ScreenRedrawFlag=true;
      }
    }
    
    static u32 redrawsubscrvsync=0;
    
    if(redrawsubscrvsync!=0){
      redrawsubscrvsync++;
      u32 mask;
      if((BGM_isOpened()==false)||(GlobalPauseFlag==true)){
        mask=1;
        }else{
        mask=3;
      }
      if((redrawsubscrvsync&mask)==0) FileList_SubDrawBG(&ScrollBar);
    }
    
    if(ScreenRedrawFlag==true){
      ScreenRedrawFlag=false;
      if(redrawsubscrvsync==0) redrawsubscrvsync=1;
      if(ForceUpdateSubScreenFlag==true){
        ForceUpdateSubScreenFlag=false;
        FileList_SubDrawBG(&ScrollBar);
      }
  //    PrfStart();
      
      _consolePrintf("\n jiejiejie");
      FileList_MainDrawBG(&ScrollBar);
  //    PrfEnd(VsyncCount);
      }else{
      if(redrawsubscrvsync!=0){
        redrawsubscrvsync=0;
        FileList_SubDrawBG(&ScrollBar);
      }
      ProcState_RefreshSave();
    }
  }
  
  if(NDSIconLoaded==false){
    TScrollBar *psb=&ScrollBar;
    u32 idx=psb->ShowPos/psb->ItemHeight;
    u32 tag=idx;
    idx++;
    if(idx==NDSFilesCount) idx=0;
    while(VBlankPassedFlag==false){
      NDSFiles_LoadNDSIcon(&pNDSFiles[idx]);
      idx++;
      if(idx==NDSFilesCount) idx=0;
      if(idx==tag){
        NDSIconLoaded=true;
        _consolePrint("All NDS icon loaded.\n");
        break;
      }
    }
  }
  
  if(HPSwitch_ProcessLong==true){
    if(VsyncCount<HPSwitch_ProcessWait){
      HPSwitch_ProcessWait-=VsyncCount;
      }else{
      HPSwitch_ProcessWait=20;
      s32 ofs=DLLSound_GetPosOffset(),max=DLLSound_GetPosMax();
      s32 diff=max/100;
      s32 val=+5;
      if(val!=0){
        Popup_Show_Seek(val);
        ofs+=diff*val;
        if(ofs<0) ofs=0;
        if(max<ofs) ofs=max;
        ofs&=~3;
        DLLSound_SetPosOffset(ofs);
        ScreenRedrawFlag=true;
        ForceUpdateSubScreenFlag=true;
      }
    }
  }
  
  if(HPSwitch_ProcessSingleLong==true){
    if(VsyncCount<HPSwitch_ProcessWait){
      HPSwitch_ProcessWait-=VsyncCount;
      }else{
      HPSwitch_ProcessWait=20;
      s32 ofs=DLLSound_GetPosOffset(),max=DLLSound_GetPosMax();
      s32 diff=max/100;
      s32 val=-5;
      if(val!=0){
        Popup_Show_Seek(val);
        ofs+=diff*val;
        if(ofs<0) ofs=0;
        if(max<ofs) ofs=max;
        ofs&=~3;
        DLLSound_SetPosOffset(ofs);
        ScreenRedrawFlag=true;
        ForceUpdateSubScreenFlag=true;
      }
    }
  }
  
  if(strpcmRequestStop==true){
    _consolePrint("Wait for terminate.");
    DLLSound_WaitForStreamPCM();
    BGM_Stop(true);
    
    if(ShellSet.PowerOffMusicEnd==true){
      if(BGMListIndex==(BGMListCount-1)){
        Resume_Clear();
        Resume_Save();
        IPC6->LCDPowerControl=LCDPC_SOFT_POWEROFF;
        while(1);
      }
    }

    MP3Cnt_Exec_Next(true);
    ProcState_RequestSave=true;
    ProcState_RefreshSave();
    ScreenRedrawFlag=true;
    ForceUpdateSubScreenFlag=true;
  }
  
  _consolePrintf("\n nnn10");
}

static void CB_End(void)
{
  TCallBack *pCallBack=CallBack_GetPointer();
  pCallBack->VBlankHandler=NULL;
  
  Popup_Free();
  
  MP3Cnt_WindowState=EMCWS_Hide;
  MP3Cnt_PosX=0;
  MP3Cnt_PosForVSync=0;
  
  pScreenMainOverlay->pCanvas->FillFull(0);
  pScreenMainOverlay->SetPosition_for_Right64x192(0,0);
  pScreenMainOverlay->SetVisible_for_LeftTop128x64(false);
  
  ProcState_RefreshSave();
  
  Clock_Free();
  BGM_Free();
  NDSFiles_Free();
  ScrollBar_Free(&ScrollBar);
  SJIS2Unicode_Free();
  
  FreeIPK();
  
  Resume_Clear();
  
    //这里被强制修改回来
    //if(ShellSet.SwapDisp==true) REG_POWERCNT&=~POWER_SWAP_LCDS;
}

static void CB_VBlankHandler(void)
{
  s32 posx=MP3Cnt_PosX;
  
  if(MP3Cnt_WindowState==EMCWS_Hide){
    if(0<posx){
      posx=(posx*15)/16;
    }
  }
  
  if(MP3Cnt_WindowState==EMCWS_Show){
    if(posx<MP3Cnt_Width){
      s32 x=MP3Cnt_Width-posx;
      x=(x*15)/16;
      posx=MP3Cnt_Width-x;
    }
  }
  
  MP3Cnt_PosX=posx;
  
  if(posx!=MP3Cnt_PosForVSync){
    MP3Cnt_PosForVSync=posx;
    if(posx<0) posx=0;
    if(MP3Cnt_Width<posx) posx=MP3Cnt_Width;
    pScreenMainOverlay->SetPosition_for_Right64x192(MP3Cnt_Width-posx,0);
  }
}

#include "proc_FileList_DeleteFileDialog.h"

#include "proc_FileList_Trigger_CallBack.h"

void ProcFileList_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->KeyLongPress=CB_KeyLongPress;
  pCallBack->KeySameLRDown=CB_KeySameLRDown;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
  pCallBack->VBlankHandler=CB_VBlankHandler;
  pCallBack->PanelClose=CB_PanelClose;
  pCallBack->PanelOpen=CB_PanelOpen;
  
  pCallBack->Trigger_ProcStart=CB_Trigger_ProcStart;
  pCallBack->Trigger_ProcEnd=CB_Trigger_ProcEnd;
  pCallBack->Trigger_Down=CB_Trigger_Down;
  pCallBack->Trigger_Up=CB_Trigger_Up;
  pCallBack->Trigger_LongStart=CB_Trigger_LongStart;
  pCallBack->Trigger_LongEnd=CB_Trigger_LongEnd;
  pCallBack->Trigger_SingleClick=CB_Trigger_SingleClick;
  pCallBack->Trigger_SingleLongStart=CB_Trigger_SingleLongStart;
  pCallBack->Trigger_SingleLongEnd=CB_Trigger_SingleLongEnd;
  pCallBack->Trigger_DoubleClick=CB_Trigger_DoubleClick;
  pCallBack->Trigger_DoubleLongStart=CB_Trigger_DoubleLongStart;
  pCallBack->Trigger_DoubleLongEnd=CB_Trigger_DoubleLongEnd;
  pCallBack->Trigger_TripleClick=CB_Trigger_TripleClick;
  
  pCallBack->ExternalPowerAttach=CB_ExternalPowerAttach;
  pCallBack->ExternalPowerDetach=CB_ExternalPowerDetach;
}

/*
    case ENFFT_Text: {
      // 偁偲偱儘儞僌僞僢僾偟偨偲偒偺僒僽儊僯儏乕偱慖戰偱偒傞傛偆偵偡傞丅
      ManualTextEncode=ETE_Auto;
      ManualTextEncode_OverrideFlag=false;
      return;
    } break;
*/
