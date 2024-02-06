
#include <NDS.h>

#include <stdio.h>
#include <stdlib.h>

#include "_console.h"
#include "_consoleWriteLog.h"
#include "_const.h"

#include "skin.h"

#include "maindef.h"
#include "memtool.h"
#include "shell.h"
#include "splash.h"
#include "strtool.h"
#include "procstate.h"

#include "cstream_fs.h"

#include "zlibhelp.h"

#include "extmem.h"

// ---------------------------------------------------------------------------------

#include "skin_SkinFile.h"
#include "skin_CustomBG.h"

// ---------------------------------------------------------------------------------

typedef struct {
  CglB15 *pbm;
} TstructB15;

typedef struct {
  CglTGF *pbm;
} TstructTGF;

#define ComponentSkinAlphaCount (ECSACount)
static TstructTGF ComponentSkinAlpha[ComponentSkinAlphaCount];

#define FileListSkinCount (EFLSCount)
static TstructB15 FileListSkin[FileListSkinCount];
#define FileListSkinAlphaCount (EFLSACount)
static TstructTGF FileListSkinAlpha[FileListSkinAlphaCount];
#define FileListClockSkinAlphaCount (EFLCLKSACount)
static TstructTGF FileListClockSkinAlpha[FileListClockSkinAlphaCount];

#define ScrollBarSkinAlphaCount (ESBSACount)
static TstructTGF ScrollBarSkinAlpha[ScrollBarSkinAlphaCount];

#define SetupSkinCount (ESSCount)
static TstructB15 SetupSkin[SetupSkinCount];
#define SetupSkinAlphaCount (ESSACount)
static TstructTGF SetupSkinAlpha[SetupSkinAlphaCount];

#define SysMenuSkinAlphaCount (ESMSACount)
static TstructTGF SysMenuSkinAlpha[SysMenuSkinAlphaCount];

#define MoviePlayerSkinCount (EMPSCount)
static TstructB15 MoviePlayerSkin[MoviePlayerSkinCount];
#define MoviePlayerSkinAlphaCount (EMPSACount)
static TstructTGF MoviePlayerSkinAlpha[MoviePlayerSkinAlphaCount];

#define ImageViewSkinCount (EIVSCount)
static TstructB15 ImageViewSkin[ImageViewSkinCount];

#define LaunchSkinCount (ELSCount)
static TstructB15 LaunchSkin[LaunchSkinCount];
#define LaunchSkinAlphaCount (ELSACount)
static TstructTGF LaunchSkinAlpha[LaunchSkinAlphaCount];

#define LongTapSkinAlphaCount (ELTSACount)
static TstructTGF LongTapSkinAlpha[LongTapSkinAlphaCount];

#define MP3CntSkinAlphaCount (EMP3SACount)
static TstructTGF MP3CntSkinAlpha[MP3CntSkinAlphaCount];

#define CustomSkinCount (ECSCount)
static TstructB15 CustomSkin[CustomSkinCount];

#define StandbyClockSkinCount (ESCCount)
static TstructB15 StandbyClockSkin[StandbyClockSkinCount];
#define StandbyClockSkinAlphaCount (ESCACount)
static TstructTGF StandbyClockSkinAlpha[StandbyClockSkinAlphaCount];

#define TextViewSkinCount (ETVCount)
static TstructB15 TextViewSkin[TextViewSkinCount];
#define TextViewSkinAlphaCount (ETVACount)
static TstructTGF TextViewSkinAlpha[TextViewSkinAlphaCount];

//-added by mj 20090414----------------

#define SetupWSkinAlphaCount (ESWSACount)
static TstructTGF SetupWSkinAlpha[SetupWSkinAlphaCount];

#define CheatSkinAlphaCount (ECWSACount)
static TstructTGF CheatSkinAlpha[CheatSkinAlphaCount];

#define RomSetSkinAlphaCount (ERSSACount)
static TstructTGF RomSetSkinAlpha[RomSetSkinAlphaCount];

#define ShutDownSkinAlphaCount (ESDSACount)
static TstructTGF ShutDownSkinAlpha[ShutDownSkinAlphaCount];

#define CpyCutSkinAlphaCount (ECCSACount)
static TstructTGF CpyCutSkinAlpha[CpyCutSkinAlphaCount];

#define GBASkinAlphaCount (EGBASACount)
static TstructTGF GBASkinAlpha[GBASkinAlphaCount];

#define BootRomSkinAlphaCount (EBRSACount)
static TstructTGF BootRomSkinAlpha[BootRomSkinAlphaCount];

// ---------------------------------------------------------------------------------

static void ComponentAlpha_Init(void)
{
  for(u32 idx=0;idx<ComponentSkinAlphaCount;idx++){
    TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EComponentSkinAlpha)idx){
      case ECSA_BoxMin: pfn="cmps_BoxMin.tgf"; break;
      case ECSA_BoxPlus: pfn="cmps_BoxPlus.tgf"; break;
      case ECSA_CheckOff: pfn="cmps_ChkOff.tgf"; break;
      case ECSA_CheckOn: pfn="cmps_ChkOn.tgf"; break;
      case ECSA_RadioOff: pfn="cmps_RadioOff.tgf"; break;
      case ECSA_RadioOn: pfn="cmps_RadioOn.tgf"; break;
      case ECSA_Ok: pfn="cmps_Ok.tgf"; break;
      case ECSA_Cancel: pfn="cmps_Cancel.tgf"; break;
      case ECSACount: default: {
        _consolePrintf("Fatal error: ComponentAlpha: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void ComponentAlpha_Free(void)
{
  for(u32 idx=0;idx<ComponentSkinAlphaCount;idx++){
    TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* ComponentAlpha_GetSkin(EComponentSkinAlpha idx)
{
  TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------
#include "../mycartsrc/CBMPResource.h"
extern void videoSub_SetShowLog(bool e);
#include "lang.h"
static void FileList_Init(void)
{
    LoadCustomBG();
  
    if(pCustomBG==NULL)
    {
        for(u32 idx=0;idx<FileListSkinCount;idx++)
        {
           TstructB15 *ptag=&FileListSkin[(u32)idx];
           const char *pfn=NULL;
           switch((EFileListSkin)idx)
           {
           case EFLS_BG_TopMsg: 
                //pfn="FL_BG_TopMsg.b15"; 
                //SkinFile_LoadB15(pfn,&ptag->pbm);
                //break;
           case EFLS_BG_Top:
                {
                    pfn="ez5_desktop.b15";
                    SkinFile_LoadB15(pfn,&ptag->pbm);
                    
                } break;
           case EFLS_BG_Bottom: 
                {//加载下屏信息 ， 选择文件信息
                  /*  pfn="FL_BG_Bottom.b15"; 
                    SkinFile_LoadB15(pfn,&ptag->pbm);
                    
                    u32 fidx=SkinFile_GetFileIndexFromFilename(pfn);
                    
                    if(fidx==(u32)-1){
                      _consolePrintf("Fatal error: Skin file '%s' not found.\n",pfn);
                      ShowLogHalt();
                    }
                    TSkinFileCache *psfc=&pSkinFileCache[fidx];
                       
                    //psfc->pData 应该是空
                    //这里需要加载上屏信息 文件列表背景
                    char pp[256];   
                   if((ProcState.SetupW.SkinSelect>(SkinDataCount-1)) && ((ProcState.SetupW.SkinSelect<0)))
                   {
                	   snprintf(pp,256,"/system/skin0/c_file.bmp");
                   }
                   else
                   {                                   	
                	   snprintf(pp,256,"/system/skin%d/c_file.bmp",ProcState.SetupW.SkinSelect);
                   }
                                        
                    CBMPResource *pbmpres = new CBMPResource(pp);
                    u16 *pbmp = pbmpres->Get16bitData();
                    MemCopy16CPU(pbmp,ptag->pbm->pCanvas->GetVRAMBuf(),ScreenWidth*ScreenHeight*2);                     
                    if(psfc->pData)
                    {
                        psfc->DataSize = ScreenWidth*ScreenHeight*2;
                        MemCopy16CPU(pbmp,psfc->pData,ScreenWidth*ScreenHeight*2);                                         
                    }
                    delete pbmpres;
                    pbmpres = NULL ;
                    break;*/
                	 pfn="c_file.b15";
                	 SkinFile_LoadB15(pfn,&ptag->pbm);
                } break;
                case EFLS_DeleteFileDialog: 
                    //pfn="FL_DeleteFileDialog.b15"; 
                   // SkinFile_LoadB15(pfn,&ptag->pbm);
                    break;
                case EFLSCount: default: {
                  _consolePrintf("Fatal error: FileList: Unknown type. %d\n",idx);
                  ShowLogHalt();
                } break;
            }
      
        }
    }
    else
    {
        /* 此段是自定义的背景图显示
        Splash_Update();
        {
            TstructB15 *ptag=&FileListSkin[(u32)EFLS_BG_TopMsg];
            ptag->pbm=new CglB15(NULL,ScreenWidth | (ScreenHeight<<16));
            CglCanvas *pcan=ptag->pbm->pCanvas;
            pCustomBG->BitBlt(pcan,0,0,ScreenWidth,ScreenHeight,0,0,false);
        }
        Splash_Update();
        {
            TstructB15 *ptag=&FileListSkin[(u32)EFLS_BG_Top];
            ptag->pbm=new CglB15(NULL,ScreenWidth | (ScreenHeight<<16));
            CglCanvas *pcan=ptag->pbm->pCanvas;

            CBMPResource *pbmpres = new CBMPResource(DefaultDataPath"/themes/default/bckgrd_1.bmp");
            u16 *pbmp = pbmpres->Get16bitData();
            MemCopy32CPU(pbmp,pCustomBG->GetVRAMBuf(),ScreenWidth*ScreenHeight*2);                   
            delete pbmpres;
            pbmpres = NULL ;           
                        
            pCustomBG->BitBlt(pcan,0,0,ScreenWidth,ScreenHeight,0,0,false);
        }
        Splash_Update();
        {
            TstructB15 *ptag=&FileListSkin[(u32)EFLS_BG_Bottom];
            ptag->pbm=new CglB15(NULL,ScreenWidth | (ScreenHeight<<16));
            CglCanvas *pcan=ptag->pbm->pCanvas;

            CBMPResource *pbmpres = new CBMPResource(DefaultDataPath"/themes/default/bckgrd_1.bmp");
            u16 *pbmp = pbmpres->Get16bitData();
            MemCopy32CPU(pbmp,pCustomBG->GetVRAMBuf(),ScreenWidth*ScreenHeight*2);                   
            delete pbmpres;
            pbmpres = NULL ;           
            
            pCustomBG->BitBlt(pcan,0,0,ScreenWidth,ScreenHeight,0,ScreenHeight,false);
        }*/
    }
}

static void FileList_Free(void)
{
  for(u32 idx=0;idx<FileListSkinCount;idx++){
    TstructB15 *ptag=&FileListSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* FileList_GetSkin(EFileListSkin idx)
{
  TstructB15 *ptag=&FileListSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void FileListAlpha_Init(EProcStateFileListMode Mode)
{
  const char *pPrefix;
  Mode = EPSFLM_Single;
  switch(Mode){
    case EPSFLM_Single: pPrefix="FL_Single_%s"; break;
    case EPSFLM_Double: pPrefix="FL_Double_%s"; break;
    default: pPrefix="%s"; break;
  }
  
  for(u32 idx=0;idx<FileListSkinAlphaCount;idx++){
    TstructTGF *ptag=&FileListSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EFileListSkinAlpha)idx){
      case EFLSA_ItemBG_Clear: pfn="ItemBG_Clear.tgf"; break;
      case EFLSA_ItemBG_Select: pfn="ItemBG_Select.tgf"; break;
      case EFLSA_ItemBG_PlayIcon: pfn="ItemBG_PlayIcon.tgf"; break;
      case EFLSA_Icon_UnknownFile: pfn="Icon_UnknownFile.tgf"; break;
      case EFLSA_Icon_UpFolder: pfn="Icon_UpFolder.tgf"; break;
      case EFLSA_Icon_Folder: pfn="Icon_Folder.tgf"; break;
      //case EFLSA_Icon_Sound: pfn="Icon_Sound.tgf"; break;
    //  case EFLSA_Icon_Image: pfn="Icon_Image.tgf"; break;
      //case EFLSA_Icon_Video: pfn="Icon_Video.tgf"; break;
      case EFLSA_Icon_NDSROM: pfn="Icon_NDSROM.tgf"; break;
    //  case EFLSA_Icon_Skin: pfn="Icon_Skin.tgf"; break;
    //  case EFLSA_Icon_Text: pfn="Icon_Text.tgf"; break;
      case EFLSA_Icon_GBA: pfn="Icon_GBA.tgf"; break;
      
      case EFLSA_Icon_UnknownFile_EXT: pfn="Icon_UF_EXT.tgf"; break;
      case EFLSA_Icon_UpFolder_EXT: pfn="Icon_UpF_EXT.tgf"; break;
      case EFLSA_Icon_Folder_EXT: pfn="Icon_F_EXT.tgf"; break;
      case EFLSA_Icon_GBA_EXT: pfn="Icon_GBA_EXT.tgf"; break;
      
      
      case EFLSACount: default: {
        _consolePrintf("Fatal error: FileListAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    char fn[32];
    snprintf(fn,32,pPrefix,pfn);
    SkinFile_LoadTGF(fn,&ptag->pbm);
  }
}

static void FileListAlpha_Free(void)
{
  for(u32 idx=0;idx<FileListSkinAlphaCount;idx++){
    TstructTGF *ptag=&FileListSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* FileListAlpha_GetSkin(EFileListSkinAlpha idx)
{
  TstructTGF *ptag=&FileListSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void FileListClockAlpha_Init(void)
{
  for(u32 idx=0;idx<FileListClockSkinAlphaCount;idx++){
    TstructTGF *ptag=&FileListClockSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EFileListClockSkinAlpha)idx){
      case EFLCLKSA_BG: pfn="FLCLK_BG.tgf"; break;
      case EFLCLKSA_Digits: pfn="FLCLK_digits.tgf"; break;
      case EFLCLKSACount: default: {
        _consolePrintf("Fatal error: FileListClockAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void FileListClockAlpha_Free(void)
{
  for(u32 idx=0;idx<FileListClockSkinAlphaCount;idx++){
    TstructTGF *ptag=&FileListClockSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* FileListClockAlpha_GetSkin(EFileListClockSkinAlpha idx)
{
  TstructTGF *ptag=&FileListClockSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void ScrollBarAlpha_Init(void)
{
  for(u32 idx=0;idx<ScrollBarSkinAlphaCount;idx++){
    TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EScrollBarSkinAlpha)idx){
      case ESBSA_BG: pfn="SB_BG.tgf"; break;
      case ESBSA_DownBtn_Normal: pfn="SB_DownBtn_Normal.tgf"; break;
      case ESBSA_DownBtn_Press: pfn="SB_DownBtn_Press.tgf"; break;
      case ESBSA_UpBtn_Normal: pfn="SB_UpBtn_Normal.tgf"; break;
      case ESBSA_UpBtn_Press: pfn="SB_UpBtn_Press.tgf"; break;
      case ESBSA_GripBG_Normal: pfn="SB_GripBG_Normal.tgf"; break;
      case ESBSA_GripBG_Press: pfn="SB_GripBG_Press.tgf"; break;
      case ESBSA_GripBottom_Normal: pfn="SB_GripBottom_Normal.tgf"; break;
      case ESBSA_GripBottom_Press: pfn="SB_GripBottom_Press.tgf"; break;
      case ESBSA_GripTop_Normal: pfn="SB_GripTop_Normal.tgf"; break;
      case ESBSA_GripTop_Press: pfn="SB_GripTop_Press.tgf"; break;
  
     // case  EFLSA_Icon_FILEINFO: pfn = "FILEINFO.tgf";break;
     // case  EFLSA_Icon_sun: pfn = "icon_sun.tgf";break;
    //  case  EFLSA_Icon_reset0:pfn = "icon_reset0.tgf"; break;
     // case  EFLSA_Icon_reset1: pfn = "icon_reset1.tgf"; break;
      case  EFLSA_Btn_set0: pfn = "set_button0.tgf"; break;
      case  EFLSA_Btn_set1: pfn = "set_button1.tgf"; break;
      //mj添加 2009年04月15日      
   // case  EFLSA_top_bar: pfn = "top_bar.tgf";break;
//    case  EFLSA_bottom_bar: pfn = "bottom_bar.tgf";break;
   case  EFLSA_scroll_bar:pfn = "scroll_bar.tgf"; break;
 //   case  EFLSA_scroll_block: pfn = "scroll_block.tgf"; break;   
    //mj添加 2009年05月25日  
    case  EFLSA_bright_up: pfn = "bright_up.tgf";break;   
    case  EFLSA_bright_down: pfn = "bright_down.tgf";break;   
    case  EFLSA_reserve_up: pfn = "reserve_up.tgf";break;   
    case  EFLSA_reserve_down: pfn = "reserve_down.tgf";break;   
    case  EFLSA_help_up: pfn = "multimedia_up.tgf";break;   
    case  EFLSA_help_down: pfn = "multimedia_down.tgf";break;   
    case  EFLSA_setup_up: pfn = "setup_up.tgf";break;   
    case  EFLSA_setup_down: pfn = "setup_down.tgf";break;   
    case  EFLSA_shut_up: pfn = "shut_up.tgf";break;   
    case  EFLSA_shut_down: pfn = "shut_down.tgf";break;   
    case  EFLSA_up_up: pfn = "up_up.tgf";break;   
    case  EFLSA_up_down: pfn = "up_down.tgf";break;   
    case  EFLSA_down_up: pfn = "down_up.tgf";break;   
    case  EFLSA_down_down: pfn = "down_down.tgf";break;   
    case  EFLSA_run_up: pfn = "run_up.tgf";break;   
    case  EFLSA_run_down: pfn = "run_down.tgf";break;  
    case  EFLSA_NO_MS2: pfn = "Setting.tgf";break;  
      case ESBSACount: default: {
        _consolePrintf("Fatal error: ScrollBarAlpha: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void ScrollBarAlpha_Free(void)
{
  for(u32 idx=0;idx<ScrollBarSkinAlphaCount;idx++){
    TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* ScrollBarAlpha_GetSkin(EScrollBarSkinAlpha idx)
{
  TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void Setup_Init(void)
{
  for(u32 idx=0;idx<SetupSkinCount;idx++){
    TstructB15 *ptag=&SetupSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ESetupSkin)idx){
      case ESS_BG_Top: pfn="Setup_BG_Top.b15"; break;
      case ESS_BG_Bottom: pfn="Setup_BG_Bottom.b15"; break;
      case ESSCount: default: {
        _consolePrintf("Fatal error: Setup: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void Setup_Free(void)
{
  for(u32 idx=0;idx<SetupSkinCount;idx++){
    TstructB15 *ptag=&SetupSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* Setup_GetSkin(ESetupSkin idx)
{
  TstructB15 *ptag=&SetupSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void SetupAlpha_Init(void)
{
  for(u32 idx=0;idx<SetupSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ESetupSkinAlpha)idx){
      case ESSA_ChkOverlayOn: pfn="Setup_ChkOverlayOn.tgf"; break;
      case ESSA_RadioOverlayOn: pfn="Setup_RadioOverlayOn.tgf"; break;
      case ESSACount: default: {
        _consolePrintf("Fatal error: SetupAlpha: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void SetupAlpha_Free(void)
{
  for(u32 idx=0;idx<SetupSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* SetupAlpha_GetSkin(ESetupSkinAlpha idx)
{
  TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void SysMenuAlpha_Init(void)
{
  for(u32 idx=0;idx<SysMenuSkinAlphaCount;idx++){
    TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ESysMenuSkinAlpha)idx){
      case ESMSA_BG: pfn="SysMenu_BG.tgf"; break;
      case ESMSA_SelectBar: pfn="SysMenu_SelectBar.tgf"; break;
      case ESMSACount: default: {
        _consolePrintf("Fatal error: SysMenuAlpha: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void SysMenuAlpha_Free(void)
{
  for(u32 idx=0;idx<SysMenuSkinAlphaCount;idx++){
    TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* SysMenuAlpha_GetSkin(ESysMenuSkinAlpha idx)
{
  TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MoviePlayer_Init(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinCount;idx++){
    TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EMoviePlayerSkin)idx){
      case EMPS_bg: pfn="mp_bg.b15"; break;
      case EMPS_seekbar_off: pfn="mp_seekbar_off.b15"; break;
      case EMPS_seekbar_on: pfn="mp_seekbar_on.b15"; break;
      case EMPSCount: default: {
        _consolePrintf("Fatal error: MoviePlayer: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void MoviePlayer_Free(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinCount;idx++){
    TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* MoviePlayer_GetSkin(EMoviePlayerSkin idx)
{
  TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MoviePlayerAlpha_Init(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinAlphaCount;idx++){
    TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EMoviePlayerSkinAlpha)idx){
      case EMPSA_mode_repeat: pfn="mp_mode_repeat.tgf"; break;
      case EMPSA_mode_allrep: pfn="mp_mode_allrep.tgf"; break;
      case EMPSA_mode_random: pfn="mp_mode_random.tgf"; break;
      case EMPSA_modelbl_repeat: pfn="mp_modelbl_repeat.tgf"; break;
      case EMPSA_modelbl_allrep: pfn="mp_modelbl_allrep.tgf"; break;
      case EMPSA_modelbl_random: pfn="mp_modelbl_random.tgf"; break;
      case EMPSA_play: pfn="mp_play.tgf"; break;
      case EMPSA_pause: pfn="mp_pause.tgf"; break;
      case EMPSA_stop: pfn="mp_stop.tgf"; break;
      case EMPSA_prev: pfn="mp_prev.tgf"; break;
      case EMPSA_next: pfn="mp_next.tgf"; break;
      case EMPSA_volbar_off: pfn="mp_volbar_off.tgf"; break;
      case EMPSA_volbar_on: pfn="mp_volbar_on.tgf"; break;
      case EMPSA_backlight: pfn="mp_backlight.tgf"; break;
      case EMPSA_seekbargrip: pfn="mp_seekbargrip.tgf"; break;
      case EMPSA_digits: pfn="mp_digits.tgf"; break;
      case EMPSACount: default: {
        _consolePrintf("Fatal error: MoviePlayerAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void MoviePlayerAlpha_Free(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinAlphaCount;idx++){
    TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* MoviePlayerAlpha_GetSkin(EMoviePlayerSkinAlpha idx)
{
  TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void ImageView_Init(void)
{
  for(u32 idx=0;idx<ImageViewSkinCount;idx++){
    TstructB15 *ptag=&ImageViewSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EImageViewSkin)idx){
      case EIVS_OverlayBG: pfn="iv_OverlayBG.b15"; break;
      case EIVS_prgbar_off: pfn="iv_prgbar_off.b15"; break;
      case EIVS_prgbar_on: pfn="iv_prgbar_on.b15"; break;
      case EIVSCount: default: {
        _consolePrintf("Fatal error: ImageView: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void ImageView_Free(void)
{
  for(u32 idx=0;idx<ImageViewSkinCount;idx++){
    TstructB15 *ptag=&ImageViewSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* ImageView_GetSkin(EImageViewSkin idx)
{
  TstructB15 *ptag=&ImageViewSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void Launch_Init(void)
{
  LoadCustomBG();
  
  for(u32 idx=0;idx<LaunchSkinCount;idx++){
    TstructB15 *ptag=&LaunchSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ELaunchSkin)idx){
      case ELS_BGTop: pfn="Launch_BGTop.b15"; break;
      case ELS_BGBottom: pfn="Launch_BGBottom.b15"; break;
      case ELSCount: default: {
        _consolePrintf("Fatal error: Launch: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    if((pCustomBG!=NULL)&&((ELaunchSkin)idx==ELS_BGTop)){
      ptag->pbm=new CglB15(NULL,ScreenWidth | (ScreenHeight<<16));
      CglCanvas *pcan=ptag->pbm->pCanvas;
      pCustomBG->BitBlt(pcan,0,0,ScreenWidth,ScreenHeight,ScreenWidth,0,false);
      }else{
      if((pCustomBG!=NULL)&&((ELaunchSkin)idx==ELS_BGBottom)){
        ptag->pbm=new CglB15(NULL,ScreenWidth | (ScreenHeight<<16));
        CglCanvas *pcan=ptag->pbm->pCanvas;
        pCustomBG->BitBlt(pcan,0,0,ScreenWidth,ScreenHeight,ScreenWidth,ScreenHeight,false);
        }else{
        SkinFile_LoadB15(pfn,&ptag->pbm);
      }
    }
  }
}

static void Launch_Free(void)
{
  for(u32 idx=0;idx<LaunchSkinCount;idx++){
    TstructB15 *ptag=&LaunchSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* Launch_GetSkin(ELaunchSkin idx)
{
  TstructB15 *ptag=&LaunchSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void LaunchAlpha_Init(void)
{
  for(u32 idx=0;idx<LaunchSkinAlphaCount;idx++){
    TstructTGF *ptag=&LaunchSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ELaunchSkinAlpha)idx){
      case ELSA_Item_ClearBG: pfn="Launch_Item_ClearBG.tgf"; break;
      case ELSA_Item_SelectBG: pfn="Launch_Item_SelectBG.tgf"; break;
      case ELSA_Tab0_Launch: pfn="Launch_Tab0_Launch.tgf"; break;
      case ELSA_Tab1_NDS: pfn="Launch_Tab1_NDS.tgf"; break;
      case ELSA_FileInfoFrame: pfn="Launch_FileInfoFrame.tgf"; break;
      case ELSACount: default: {
        _consolePrintf("Fatal error: LaunchAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void LaunchAlpha_Free(void)
{
  for(u32 idx=0;idx<LaunchSkinAlphaCount;idx++){
    TstructTGF *ptag=&LaunchSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* LaunchAlpha_GetSkin(ELaunchSkinAlpha idx)
{
  TstructTGF *ptag=&LaunchSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void LongTapAlpha_Init(void)
{
  for(u32 idx=0;idx<LongTapSkinAlphaCount;idx++){
    TstructTGF *ptag=&LongTapSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ELongTapSkinAlpha)idx){
      case ELTSA_f0: pfn="LongTap_f0.tgf"; break;
      case ELTSA_f1: pfn="LongTap_f1.tgf"; break;
      case ELTSA_f2: pfn="LongTap_f2.tgf"; break;
      case ELTSA_f3: pfn="LongTap_f3.tgf"; break;
      case ELTSA_f4: pfn="LongTap_f4.tgf"; break;
      case ELTSA_f5: pfn="LongTap_f5.tgf"; break;
      case ELTSA_f6: pfn="LongTap_f6.tgf"; break;
      case ELTSA_f7: pfn="LongTap_f7.tgf"; break;
      case ELTSACount: default: {
        _consolePrintf("Fatal error: LongTapAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void LongTapAlpha_Free(void)
{
  for(u32 idx=0;idx<LongTapSkinAlphaCount;idx++){
    TstructTGF *ptag=&LongTapSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* LongTapAlpha_GetSkin(ELongTapSkinAlpha idx)
{
  TstructTGF *ptag=&LongTapSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// -----------------------------

static void MP3CntAlpha_Init(void)
{
  for(u32 idx=0;idx<MP3CntSkinAlphaCount;idx++){
    TstructTGF *ptag=&MP3CntSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EMP3CntSkinAlpha)idx){
      case EMP3SA_p0: pfn="MP3Cnt_p0.tgf"; break;
      case EMP3SA_p1_prev: pfn="MP3Cnt_p1_prev.tgf"; break;
      case EMP3SA_p2_stop: pfn="MP3Cnt_p2_stop.tgf"; break;
      case EMP3SA_p2_play: pfn="MP3Cnt_p2_play.tgf"; break;
      case EMP3SA_p2_pause: pfn="MP3Cnt_p2_pause.tgf"; break;
      case EMP3SA_p3_next: pfn="MP3Cnt_p3_next.tgf"; break;
      case EMP3SA_p4_repeat: pfn="MP3Cnt_p4_repeat.tgf"; break;
      case EMP3SA_p4_allrep: pfn="MP3Cnt_p4_allrep.tgf"; break;
      case EMP3SA_p4_random: pfn="MP3Cnt_p4_random.tgf"; break;
      case EMP3SACount: default: {
        _consolePrintf("Fatal error: MP3CntAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void MP3CntAlpha_Free(void)
{
  for(u32 idx=0;idx<MP3CntSkinAlphaCount;idx++){
    TstructTGF *ptag=&MP3CntSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* MP3CntAlpha_GetSkin(EMP3CntSkinAlpha idx)
{
  TstructTGF *ptag=&MP3CntSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void Custom_Init(void)
{
  for(u32 idx=0;idx<CustomSkinCount;idx++){
    TstructB15 *ptag=&CustomSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ECustomSkin)idx){
      case ECS_TopMsg: pfn="Custom_TopMsg.b15"; break;
      case ECS_BG: pfn="Custom_BG.b15"; break;
      case ECSCount: default: {
        _consolePrintf("Fatal error: Custom: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void Custom_Free(void)
{
  for(u32 idx=0;idx<CustomSkinCount;idx++){
    TstructB15 *ptag=&CustomSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* Custom_GetSkin(ECustomSkin idx)
{
  TstructB15 *ptag=&CustomSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

void StandbyClock_Init(bool BlackMode)
{
  LoadCustomBG();
  
  const char *pPrefixID;
  if(BlackMode==false){
    pPrefixID="W";
    }else{
    pPrefixID="B";
  }
  
  for(u32 idx=0;idx<StandbyClockSkinCount;idx++){
    TstructB15 *ptag=&StandbyClockSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EStandbyClockSkin)idx){
      case ESC_BG: pfn="SC%s_BG.b15"; break;
      case ESCCount: default: {
        _consolePrintf("Fatal error: StandbyClock: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    char fn[32];
    snprintf(fn,32,pfn,pPrefixID);
    SkinFile_LoadB15(fn,&ptag->pbm);
  }
}

void StandbyClock_Free(void)
{
  for(u32 idx=0;idx<StandbyClockSkinCount;idx++){
    TstructB15 *ptag=&StandbyClockSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* StandbyClock_GetSkin(EStandbyClockSkin idx)
{
  TstructB15 *ptag=&StandbyClockSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------
void StandbyClockAlpha_Init(bool BlackMode)
{
  const char *pPrefixID;
  if(BlackMode==false){
    pPrefixID="W";
    }else{
    pPrefixID="B";
  }
  
  for(u32 idx=0;idx<StandbyClockSkinAlphaCount;idx++){
    TstructTGF *ptag=&StandbyClockSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EStandbyClockSkinAlpha)idx){
      case ESCA_Font16: pfn="SC%s_Font16.tgf"; break;
      case ESCA_Font24: pfn="SC%s_Font24.tgf"; break;
      case ESCA_Font56: pfn="SC%s_Font56.tgf"; break;
      case ESCA_CalenderFont: pfn="SC%s_CalenderFont.tgf"; break;
      case ESCA_CalenderTodayFont: pfn="SC%s_CalenderTodayFont.tgf"; break;
      case ESCACount: default: {
        _consolePrintf("Fatal error: StandbyClockAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    if(pfn!=NULL){
      char fn[32];
      snprintf(fn,32,pfn,pPrefixID);
      SkinFile_LoadTGF(fn,&ptag->pbm);
    }
  }
}

void StandbyClockAlpha_Free(void)
{
  for(u32 idx=0;idx<StandbyClockSkinAlphaCount;idx++){
    TstructTGF *ptag=&StandbyClockSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* StandbyClockAlpha_GetSkin(EStandbyClockSkinAlpha idx)
{
  TstructTGF *ptag=&StandbyClockSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void TextView_Init(void)
{
  LoadCustomBG();
  
  for(u32 idx=0;idx<TextViewSkinCount;idx++){
    TstructB15 *ptag=&TextViewSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ETextViewSkin)idx){
      case ETV_PageBG: pfn="TV_PageBG.b15"; break;
      case ETV_Bookmark_LoadBG: pfn="TV_Bookmark_LoadBG.b15"; break;
      case ETV_Bookmark_SaveBG: pfn="TV_Bookmark_SaveBG.b15"; break;
      case ETV_Bookmark_PreviewBG: pfn="TV_Bookmark_PreviewBG.b15"; break;
      case ETVCount: default: {
        _consolePrintf("Fatal error: TextView: Unknown type. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void TextView_Free(void)
{
  for(u32 idx=0;idx<TextViewSkinCount;idx++){
    TstructB15 *ptag=&TextViewSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* TextView_GetSkin(ETextViewSkin idx)
{
  TstructB15 *ptag=&TextViewSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void TextViewAlpha_Init(void)
{
  for(u32 idx=0;idx<TextViewSkinAlphaCount;idx++){
    TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ETextViewSkinAlpha)idx){
      case ETVA_Bookmark_Cursor: pfn="TV_Bookmark_Cursor.tgf"; break;
      case ETVA_Bookmark_Clear: pfn="TV_Bookmark_Clear.tgf"; break;
      case ETVA_Bookmark_Slot0Icon: pfn="TV_Bookmark_Slot0Icon.tgf"; break;
      case ETVA_Bookmark_Slot1Icon: pfn="TV_Bookmark_Slot1Icon.tgf"; break;
      case ETVA_Bookmark_Slot2Icon: pfn="TV_Bookmark_Slot2Icon.tgf"; break;
      case ETVA_Bookmark_Slot3Icon: pfn="TV_Bookmark_Slot3Icon.tgf"; break;
      case ETVACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void TextViewAlpha_Free(void)
{
  for(u32 idx=0;idx<TextViewSkinAlphaCount;idx++){
    TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* TextViewAlpha_GetSkin(ETextViewSkinAlpha idx)
{
  TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
  return(ptag->pbm);
}
//added by mj 20090414----------------

static void SetupWAlpha_Init(void)
{
  for(u32 idx=0;idx<SetupWSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupWSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ESetupWSkinAlpha)idx){
      case ESWSA_SetupWBG: pfn="Setting.tgf"; break;
    //  case ESWSA_SelectOptionBG: pfn="SetupWSelectOptionBG.tgf"; break;
    //  case ESWSA_Opt_LET0: pfn="Opt_LET0.tgf"; break;
     // case ESWSA_Opt_RIT0: pfn="Opt_RIT0.tgf"; break;
     // case ESWSA_Opt_LET1: pfn="Opt_LET1.tgf"; break;
    //  case ESWSA_Opt_RIT1: pfn="Opt_RIT1.tgf"; break;
    //  case ESWSA_Opt_BG: pfn="Opt_BG.tgf"; break;
      case ESWSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void SetupWAlpha_Free(void)
{
  for(u32 idx=0;idx<SetupWSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupWSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* SetupWAlpha_GetSkin(ESetupWSkinAlpha idx)
{
  TstructTGF *ptag=&SetupWSkinAlpha[(u32)idx];
  return(ptag->pbm);
}
//--Cheat

static void CheatAlpha_Init(void)
{
  for(u32 idx=0;idx<CheatSkinAlphaCount;idx++){
    TstructTGF *ptag=&CheatSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ECheatSkinAlpha)idx){
      case ECWSA_CheatBG: pfn="Cheat_BG.tgf"; break;   
      case ECWSA_Opt_NO: pfn="Cheat_Opt_NO.tgf"; break;
       case ECWSA_Opt_YES: pfn="Cheat_Opt_YES.tgf"; break;
      case ECWSA_ItemBG_Select: pfn="Cheat_ItemBG_Select.tgf";break;
      case ECWSA_Sub: pfn="Cheat_Sub.tgf"; break; 
      case ECWSA_Mul: pfn="Cheat_Mul.tgf"; break; 
      case ECWSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void CheatAlpha_Free(void)
{
  for(u32 idx=0;idx<CheatSkinAlphaCount;idx++){
    TstructTGF *ptag=&CheatSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* CheatAlpha_GetSkin(ECheatSkinAlpha idx)
{
  TstructTGF *ptag=&CheatSkinAlpha[(u32)idx];
  return(ptag->pbm);
}
//--RomSet

static void RomSetAlpha_Init(void)
{
  for(u32 idx=0;idx<RomSetSkinAlphaCount;idx++){
    TstructTGF *ptag=&RomSetSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ERomSetSkinAlpha)idx){
      case ERSSA_RomSetBG: pfn="Rom_Set.tgf"; break;   
      case ERSSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void RomSetAlpha_Free(void)
{
  for(u32 idx=0;idx<RomSetSkinAlphaCount;idx++){
    TstructTGF *ptag=&RomSetSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* RomSetAlpha_GetSkin(ERomSetSkinAlpha idx)
{
  TstructTGF *ptag=&RomSetSkinAlpha[(u32)idx];
  return(ptag->pbm);
}
//--ShutDown

static void ShutDownAlpha_Init(void)
{
  for(u32 idx=0;idx<ShutDownSkinAlphaCount;idx++){
    TstructTGF *ptag=&ShutDownSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EShutDownSkinAlpha)idx){
      case ESDSA_ShutDownBG: pfn="shutdown.tgf"; break;   
      case ESDSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void ShutDownAlpha_Free(void)
{
  for(u32 idx=0;idx<ShutDownSkinAlphaCount;idx++){
    TstructTGF *ptag=&ShutDownSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* ShutDownAlpha_GetSkin(EShutDownSkinAlpha idx)
{
  TstructTGF *ptag=&ShutDownSkinAlpha[(u32)idx];
  return(ptag->pbm);
}


//--GBA

static void GBAAlpha_Init(void)
{
  for(u32 idx=0;idx<GBASkinAlphaCount;idx++){
    TstructTGF *ptag=&GBASkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EGBASkinAlpha)idx){
      case EGBASA_GBABG: pfn="popup_bg.tgf"; break; 
      case EGBASA_progrss_bar_bg: pfn="progrss_bar_bg.tgf"; break; 
      case EGBASA_progress_bar: pfn="progress_bar.tgf"; break; 
      case EGBASACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void GBAAlpha_Free(void)
{
  for(u32 idx=0;idx<GBASkinAlphaCount;idx++){
    TstructTGF *ptag=&GBASkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* GBAAlpha_GetSkin(EGBASkinAlpha idx)
{
  TstructTGF *ptag=&GBASkinAlpha[(u32)idx];
  return(ptag->pbm);
}

//--CpyCut

static void CpyCutAlpha_Init(void)
{
  for(u32 idx=0;idx<CpyCutSkinAlphaCount;idx++){
    TstructTGF *ptag=&CpyCutSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ECpyCutSkinAlpha)idx){
      case ECCSA_CpyCutBG: pfn="popup_bg.tgf"; break; 
      case ECCSA_progrss_bar_bg: pfn="progrss_bar_bg.tgf"; break; 
      case ECCSA_progress_bar: pfn="progress_bar.tgf"; break; 
      case ECCSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void CpyCutAlpha_Free(void)
{
  for(u32 idx=0;idx<CpyCutSkinAlphaCount;idx++){
    TstructTGF *ptag=&CpyCutSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* CpyCutAlpha_GetSkin(ECpyCutSkinAlpha idx)
{
  TstructTGF *ptag=&CpyCutSkinAlpha[(u32)idx];
  return(ptag->pbm);
}
//--BootRom

static void BootRomAlpha_Init(void)
{
  for(u32 idx=0;idx<BootRomSkinAlphaCount;idx++){
    TstructTGF *ptag=&BootRomSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EBootRomSkinAlpha)idx){
      case EBRSA_BootRomBG: pfn="progress_wnd.tgf"; break; 
      case EBRSA_progress_bar: pfn="progress_BootRom_bar.tgf"; break; 
      case EBRSACount: default: {
        _consolePrintf("Fatal error: TextViewAlpha: Unknown type value. %d\n",idx);
        ShowLogHalt();
      } break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void BootRomAlpha_Free(void)
{
  for(u32 idx=0;idx<BootRomSkinAlphaCount;idx++){
    TstructTGF *ptag=&BootRomSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* BootRomAlpha_GetSkin(EBootRomSkinAlpha idx)
{
  TstructTGF *ptag=&BootRomSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

TSkin_OwnerDrawText Skin_OwnerDrawText;
TSkin_Calender Skin_Calender;
TColorTable ColorTable;

#include "skin_colortbl.h"

// ---------------------------------------------------------------------------------
#include "lang.h"
#include "SkinDirListFile.h"
extern void TestPauseKey(void);
bool Skin_SetFilename(const UnicodeChar *pFullPath)
{
  static UnicodeChar DefaultFilenameW[64];

   
   if((ProcState.SetupW.SkinSelect > ListFileNum-1) && ((ProcState.SetupW.SkinSelect < 0)))
   {
	   	ProcState.SetupW.SkinSelect = 0;
	    char pp[256];    					        	
	    snprintf(pp,256,"/system/skin/%s",ListFile[ProcState.SetupW.SkinSelect].Filename);
	   	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
	   	FAT_FILE *pf=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(pp));
		if(pf)
		{
			FAT2_fclose(pf);
		}
   }
   else
   {
	    char pp[256];    					        	
	    snprintf(pp,256,"/system/skin/%s",ListFile[ProcState.SetupW.SkinSelect].Filename);
	   	StrConvert_Ank2Unicode(pp,DefaultFilenameW);
	   	FAT_FILE *pf=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(pp));
		if(pf)
		{
			FAT2_fclose(pf);
		}
   }
  
  if(Unicode_isEqual_NoCaseSensitive(pFullPath,DefaultFilenameW)==true){
    if(Shell_isJPmode()==true){
   //	const char *pfn=DefaultDataPath "/skin.dat"/*"/defaultj.skn"*/; 
    	//const char *pfn=DefaultDataPath "/cache.dat"/*"/defaultj.skn"*/;
    //  StrConvert_Ank2Unicode(pfn,DefaultFilenameW);
    //  pFullPath=DefaultFilenameW;
     // _consolePrintf("Default skin alias to '%s'.\n",pfn);
    }
  }
//  TestPauseKey();
	_consolePrintf("\n path111=%s",StrConvert_Unicode2Ank_Test(pFullPath));
//    	TestPauseKey();
  if(SkinFile_Check(pFullPath)==false){
    _consolePrintf("Skin file check failed.\n");
    return(false);
  }
  
  SkinFile_Close();
  
  if(SkinFile_Open(pFullPath)==false){
    _consolePrintf("Skin file open failed.\n");
    return(false);
  }
  
  LoadColorTable_colortbl_ini();
	_consolePrintf("\n dr1111");
//	TestPauseKey();
  return(true);
}

void Skin_CloseFile(void)
{
  SkinFile_Close();
}

void Skin_Load_ChkDsk(void)
{
  extmem_ShowMallocInfo();
}

void Skin_Load_Setup(void)
{
  ComponentAlpha_Init();
  Setup_Init();
  SetupAlpha_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_FileList(void)
{
  PrfStart();
  ScrollBarAlpha_Init();
  FileList_Init();
  FileListAlpha_Init(ProcState.FileList.Mode);
  FileListClockAlpha_Init();
  //MP3CntAlpha_Init();
  StandbyClock_Init(ProcState.FileList.ScreenSaver_BlackMode);
  StandbyClockAlpha_Init(ProcState.FileList.ScreenSaver_BlackMode);
  PrfEnd(0);
  PrintFreeMem();
  extmem_ShowMallocInfo();
}

void Skin_Load_SysMenu(void)
{
  SysMenuAlpha_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_DPGCustom(void)
{
  ComponentAlpha_Init();
  Custom_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_DPGPlay(void)
{
  MoviePlayer_Init();
  MoviePlayerAlpha_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_ImageCustom(void)
{
  ComponentAlpha_Init();
  Custom_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_ImageView(void)
{
  ImageView_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_ImageView_AfterFree(void)
{
  if(ImageViewSkin[EIVS_OverlayBG].pbm!=NULL){
    delete ImageViewSkin[EIVS_OverlayBG].pbm; ImageViewSkin[EIVS_OverlayBG].pbm=NULL;
  }
}

void Skin_Load_TextView(void)
{
  Skin_ClearCustomBG();
  extmem_ShowMallocInfo();
}

void Skin_Load_TextView_AfterLoad(void)
{
  TextView_Init();
  TextViewAlpha_Init();
  if(ProcState.Text.TopScrMode==ETTSM_Clock){
    StandbyClock_Init(ProcState.Text.ScreenSaver_BlackMode);
    StandbyClockAlpha_Init(ProcState.Text.ScreenSaver_BlackMode);
  }
  extmem_ShowMallocInfo();
}

void Skin_Load_TextCustom(void)
{
  ComponentAlpha_Init();
  Custom_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_Launch(void)
{
  Launch_Init();
  LaunchAlpha_Init();
  LongTapAlpha_Init();
  FileListAlpha_Init(EPSFLM_Double);
  extmem_ShowMallocInfo();
}

void Skin_Load_Custom(void)
{
  ComponentAlpha_Init();
  Custom_Init();
  extmem_ShowMallocInfo();
}

void Skin_Load_BootROM(void)
{
	BootRomAlpha_Init();
	extmem_ShowMallocInfo();
}


void Skin_Load_SetupW(void)
{
	SetupWAlpha_Init();
	ScrollBarAlpha_Init();
}

void Skin_Load_Cheat(void)
{
	CheatAlpha_Init();
	ScrollBarAlpha_Init();
}

void Skin_Load_RomSet(void)
{
	RomSetAlpha_Init();
	ScrollBarAlpha_Init();
}

void Skin_Load_ShutDown(void)
{
	ShutDownAlpha_Init();
}
void Skin_Load_CpyCut(void)
{
	CpyCutAlpha_Init();
}
void Skin_Load_GBA(void)
{
	GBAAlpha_Init();
}

void Skin_Free(void)
{
  ComponentAlpha_Free();
  FileList_Free();
  FileListAlpha_Free();
  FileListClockAlpha_Free();
  ScrollBarAlpha_Free();
  Setup_Free();
  SetupAlpha_Free();
  SysMenuAlpha_Free();
  MoviePlayer_Free();
  MoviePlayerAlpha_Free();
  ImageView_Free();
  Launch_Free();
  LaunchAlpha_Free();
  LongTapAlpha_Free();
  MP3CntAlpha_Free();
  Custom_Free();
  StandbyClock_Free();
  StandbyClockAlpha_Free();
  TextView_Free();
  TextViewAlpha_Free();
  SetupWAlpha_Free();
  CheatAlpha_Free();
  RomSetAlpha_Free();
  ShutDownAlpha_Free();
  CpyCutAlpha_Free();
  GBAAlpha_Free();
      
}

void DrawSkin(CglB15 *psrcbm,CglCanvas *pdstbm,s32 x,s32 y)
{
  psrcbm->pCanvas->BitBlt(pdstbm,x,y,psrcbm->GetWidth(),psrcbm->GetHeight(),0,0,false);
}

void DrawSkinAlpha(CglTGF *psrcbm,CglCanvas *pdstbm,s32 x,s32 y)
{
  psrcbm->BitBlt(pdstbm,x,y);
}

void Skin_ClearCustomBG(void)
{
  if(pCustomBG!=NULL){
    _consolePrint("Free CustomBG.\n");
    delete pCustomBG; pCustomBG=NULL;
  }
}

void Skin_ClearCustomBG_FileBody(void)
{
  FAT_FILE *pwfh=Shell_FAT_fopenwrite_Data(BGBMPFilename);
  if(pwfh==NULL){
    _consolePrintf("Open for write failed.\n");
    return;
  }
  
  u32 BGBMPType=EBGBT_None;
  FAT2_fwrite(&BGBMPType,1,4,pwfh);
  
  FAT2_fclose(pwfh);
}

CglTGF* Skin_GetErrorDialogBG(void)
{
  CglTGF *pbm=NULL;
  SkinFile_LoadTGF("ErrorDialogBG.tgf",&pbm);
  return(pbm);
}


////////////////////////////////////////////////////////////////
CglTGF* Skin_GetNDSinfo(void)
{
    CglTGF *pbm=NULL;
    SkinFile_LoadTGF("FILEINFO.tgf",&pbm);
    return(pbm);    
}

