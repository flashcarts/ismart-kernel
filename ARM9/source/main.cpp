
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nds.h>

#include "_const.h"
#include "maindef.h"
#include "_console.h"
#include "_consoleWriteLog.h"

#include "plugin/plug_ndsrom.h"

#include "memtool.h"
#include "strpcm.h"
#include "dll.h"
#include "dllsound.h"
#include "lang.h"
#include "resume.h"
#include "procstate.h"
#include "launchstate.h"
#include "shell.h"
#include "skin.h"
#include "extmem.h"
#include "splash.h"
#include "BootROM.h"
#include "ErrorDialog.h"
#include "component.h"
#include "extlink.h"
#include "strtool.h"

#include "../../ipc6.h"

#include "arm9tcm.h"
#include "setarm9_reg_waitcr.h"

#include "fat2.h"
#include "gba_nds_fat.h"
#include "zlibhelp.h"

#include "sndeff.h"
#include "datetime.h"
#include "mycartsrc/keyformSDK.h"

#include "ards.h"
#include "proc_romset/proc_romset_bin.h"
//#include "procbody/proc_cheat/proc_Cheat_SaveCheatData.h"
#include "SkinDirListFile.h"
#include "proc_SetupW.h"

extern "C" {
extern u32 dtcmend;
extern u32 mtcmend;
}
static KeyInfo gKey;                   // key input

#define Loading_sX  70
#define Loading_sY  40
#define Loading_X  120
#define Loading_Y  120

u32 *pDTCMEND,*pMTCMEND;

CglFont *pCglFontDefault=NULL;

bool isExistsROMEO2;

ENextProc NextProc;

UnicodeChar RelationalFilePathUnicode[MaxFilenameLength];
UnicodeChar RelationalFileNameUnicode[MaxFilenameLength];
u32 RelationalFilePos;

ETextEncode ManualTextEncode;
bool ManualTextEncode_OverrideFlag;

bool SetupWisComefromMenu = false;
bool isOnlyNDS=false;

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}
extern "C" void bxr0(void* a);
extern "C" void ResetEveryMemory();
// ------------------------------------------------------------------

void videoSub_SetShowLog(bool e)
{
    if(e)
        videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE);
    else
        videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256);
}

//------------Test Pause Key------------------------
void TestPauseKey(void)
{
   //videoSub_SetShowLog(true);
    _consolePrintf("\n Test pause key.");
    KeyInfo TstKey; 
    int qqidx=1;
    int qqidx2=0;
    do
    {
        KeyRead(&TstKey);
        WaitForVBlank();
    	if(TstKey.trg&PAD_BUTTON_X)
    	{
    		qqidx2 = 1;
    		do
    		{
	    		KeyRead(&TstKey);
	    		WaitForVBlank();
	    		if(TstKey.trg&PAD_BUTTON_Y)
	    		{
	    			qqidx = 0;
	    			qqidx2 = 0;
	    		}
    		}while(qqidx2);
    	}
    }while(qqidx);
}

void ShowLogHalt(void)
{
  _consoleLogResume();
  IPC6->LCDPowerControl=LCDPC_ON_BOTH;
  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE);
  
  _consoleSetLogOutFlag(true);
  
  _consolePrint("\n     Application halted!!\n");
 
  for(int i=0;i<26;i++)
	 // _consolePrint("\n");
  
  if(_consoleGetLogFile()==true){
    //_consolePrint("Please refer [/moonshl2/logbuf.txt]\n\n");
    }else{
    _consolePrint("\n");
  }
  while(1);
}

static __attribute__ ((always_inline)) void _ttywrch_tx_waitsend(const u32 data)
{
	while((*((vu8 *)0x0A000001) & 2)!=0);
	*((vu8 *)0x0A000000)=(u8)data;
}

extern "C" {
extern void _ttywrch(int ch);
}

void _ttywrch(int ch)
{
  _ttywrch_tx_waitsend(0xa0);
  _ttywrch_tx_waitsend(2);
  _ttywrch_tx_waitsend((u32)ch);
}

// ------------------------------------------------------

#include "chrglyph_999.h"
#include <wchar.h>

static void LoadDefaultFont(void)
{
  if(pCglFontDefault!=NULL){
    delete pCglFontDefault; pCglFontDefault=NULL;
  }
  pCglFontDefault=new CglFont(chrglyph_999,chrglyph_999_Size);
  
  pScreenMain->pBackCanvas->SetCglFont(pCglFontDefault);
  pScreenMain->pViewCanvas->SetCglFont(pCglFontDefault);
  pScreenMainOverlay->pCanvas->SetCglFont(pCglFontDefault);
  pScreenSub->pCanvas->SetCglFont(pCglFontDefault);
}

#pragma import __use_utf8_ctype

static void LangInitAndLoadFont(void)
{
    //language.set，
    Shell_FAT_fopen_LanguageInit();
    //chrglyph /moonshl2/language/chrglyph.936
    FAT_FILE *pf=Shell_FAT_fopen_Language_chrglyph();
    if(pf==NULL){
        _consolePrintf("Fatal error: Can not found font file.\n");
        ShowLogHalt();
    }
    u32 bufsize=FAT2_GetFileSize(pf);
    
    //TestPauseKey();
    
   // void *pdummy=(u8*)safemalloc((bufsize*2)+(192*1024));
    _consolePrintf("\n here maomao ");
    u8 *pbuf=(u8*)safemalloc(bufsize);
    _consolePrintf("\n here maomao1 ");
    //TestPauseKey();
    FAT2_fread_fast(pbuf,1,bufsize,pf);
    FAT2_fclose(pf);
  
   // safefree(pdummy); pdummy=NULL;
  
    if(pCglFontDefault!=NULL){
        delete pCglFontDefault; pCglFontDefault=NULL;
    }
    pCglFontDefault=new CglFont((const u8*)pbuf,bufsize);
  
    safefree(pbuf); pbuf=NULL;
    
    
    _consolePrintf("Loaded font for your language.\n");
 }

static void DrawBootWarn(const void *pSrcBuf,u32 SrcSize,u32 palmode)
{
  TZLIBData zd;
  
  zd.DstSize=ScreenWidth*ScreenHeight;
  zd.pDstBuf=(u8*)safemalloc(zd.DstSize);
  zd.SrcSize=SrcSize;
  zd.pSrcBuf=(u8*)pSrcBuf;
  
  if((zd.pSrcBuf[0]!=0x78)||(zd.pSrcBuf[1]!=0x01)){
    _consolePrintf("Fatal error: Unknown compress format.\n");
    ShowLogHalt();
  }

  if(zlibdecompress(&zd)==false){
    _consolePrintf("Fatal error: ZLIB decompress error.\n");
    ShowLogHalt();
  }
  
  u16 pals[4];
  
  if(palmode==0){
    pals[0]=RGB15(12,0,0)|BIT15;
    pals[1]=RGB15(7,0,0)|BIT15;
    pals[2]=RGB15(31,31,31)|BIT15;
    pals[3]=RGB15(0,0,0)|BIT15;
    }else{
    pals[0]=RGB15(24,0,0)|BIT15;
    pals[1]=RGB15(28,26,27)|BIT15;
    pals[2]=RGB15(0,0,0)|BIT15;
    pals[3]=0;
  }    
  
  u16 *psrc=(u16*)zd.pDstBuf;
  u32 *pdst=(u32*)pScreenMain->pViewCanvas->GetVRAMBuf();
  for(u32 idx=0;idx<(ScreenWidth*ScreenHeight)/2;idx++){
    u32 pal16=*psrc++;
    u32 col32=pals[pal16&0xff]|(pals[pal16>>8]<<16);
    *pdst++=col32;
  }
  
  zd.DstSize=0;
  safefree(zd.pDstBuf); zd.pDstBuf=NULL;
}

// ----------------------------------------

extern u32 _pGUID;
/* 
static void CheckGUID_Pre(void)
{
  u32 *pGUID=&_pGUID;
  
  if(pGUID[2]==pGUID[5]) return;
  
  _consolePrintf("pGUID=0x%x\n",(u32)pGUID);
  
  _consolePrintf("ID=0x%x\n",pGUID[0]);
  _consolePrintf("XID=0x%x\n",pGUID[1]);
  _consolePrintf("xcrc=0x%x\n",pGUID[2]);
  _consolePrintf("MemStartAddr=0x%x\n",pGUID[3]);
  _consolePrintf("MemEndAddr=0x%x\n",pGUID[4]);
  _consolePrintf("chkxcrc=0x%x\n",pGUID[5]);
  
  _consolePrint("\n");
  _consolePrint("  --- Detected fatal error !! ---\n");
  _consolePrint("\n");
  _consolePrint("  There is a loss in the main body of \n");
  _consolePrint("  The ARM9 code.\n");
  _consolePrint("  Please copy moonshl2.nds with \n");
  _consolePrint("  Windows again.\n");
  _consolePrint("  The cause might be a loose connection \n");
  _consolePrint("  of the equipment.\n");
  _consolePrint("\n");
}
static bool CheckGUID_Pre_for_CheckTCM(void)
{
  u32 *pGUID=&_pGUID;
  
  if(pGUID[2]==pGUID[5]) return(false);
  
  _consolePrintf("pGUID=0x%x\n",(u32)pGUID);
  
  _consolePrintf("ID=0x%x\n",pGUID[0]);
  _consolePrintf("XID=0x%x\n",pGUID[1]);
  _consolePrintf("xcrc=0x%x\n",pGUID[2]);
  _consolePrintf("MemStartAddr=0x%x\n",pGUID[3]);
  _consolePrintf("MemEndAddr=0x%x\n",pGUID[4]);
  _consolePrintf("chkxcrc=0x%x\n",pGUID[5]);
  
  _consolePrint("\n");
  _consolePrint("  --- Detected fatal error !! ---\n");
  _consolePrint("\n");
  _consolePrint("  There is a loss in the main body of \n");
  _consolePrint("  The ARM9 code.\n");
  _consolePrint("  Please copy moonshl2.nds with \n");
  _consolePrint("  Windows again.\n");
  _consolePrint("  The cause might be a loose connection \n");
  _consolePrint("  of the equipment.\n");
  _consolePrint("\n");
  
  return(true);
}
*/

#include "guidxcrcerror_b8zlib.h"

/*static void CheckGUID_Body(void)
{
  u32 *pGUID=&_pGUID;
  
  if(pGUID[2]==pGUID[5]) return;
  
  DrawBootWarn(guidxcrcerror_b8zlib,guidxcrcerror_b8zlib_Size,0);
  
  ShowLogHalt();
}*/

// ----------------------------------------

static __attribute__ ((noinline)) void mainloop(void);

#include "main_vardebug.h"

#include "bootwarn_b8zlib.h"
//#include "mycartsrc/LoadingImg.h"

static bool chkflag;

#include "mycartsrc/io_MartSD.h"
#include "mycartsrc/MartCartop.h"

static __attribute__ ((noinline)) void main_ins_start(void)
{

    u32 zero;
    asm {
        mov zero,#0
        MCR p15, 0, zero, c7, c10, 4 // drain write buffer
    }
    static u32 stackptr=__current_sp();
    static vu32 stackdata[32];
  
    {
        u32 *pptr=(u32*)stackptr;
        for(u32 idx=0;idx<32;idx++)
        {
            stackdata[idx]=pptr[-16+idx];
        }
    }
    SetARM9_REG_WaitCR();
    REG_POWERCNT = POWER_ALL_2D; // | POWER_SWAP_LCDS; // SWAPするとファイルリストが下
    atype_init();
  
    //DTCM 最后的128字节保留 ,因为DTCM 用作SP即堆栈
    pDTCMEND=(u32*)&dtcmend;
    pDTCMEND+=128/4;
    {
        // setup stack overflow checker 
        u32 *p=pDTCMEND;
        for(;p<(u32*)__current_sp();p++)
        {
            *p=(u32)p;
        }
    }
    glSetFuncDebugPrint(_consolePrint);
    glDefaultMemorySetting();
    vuint16* pa = (vuint16*)0x06018000;
    MemSet16CPU(0x8000,(void*)pa,0x18000);
  {
    SUB_BG2_CR = BG_256_COLOR | BG_RS_64x64 | BG_MAP_BASE(8) | BG_TILE_BASE(0) | BG_PRIORITY_1; // Tile16kb Map2kb(64x32)
    
    BG_PALETTE_SUB[(0*16)+0] = RGB15(0,0,0); // unuse (transparent)
    BG_PALETTE_SUB[(0*16)+1] = RGB15(0,0,2) | BIT(15); // BG color
    BG_PALETTE_SUB[(0*16)+2] = RGB15(0,0,0) | BIT(15); // Shadow color
    BG_PALETTE_SUB[(0*16)+3] = RGB15(16,16,16) | BIT(15); // Text color
    
    u16 XDX=(u16)((8.0/6)*0x100);
    u16 YDY=(u16)((8.0/6)*0x100);
    
    SUB_BG2_XDX = XDX;
    SUB_BG2_XDY = 0;
    SUB_BG2_YDX = 0;
    SUB_BG2_YDY = YDY;
    
    SUB_BG2_CX=1;
    SUB_BG2_CY=-1;
    
    //consoleInit() is a lot more flexible but this gets you up and running quick
    _consoleInitDefault((u16*)(SCREEN_BASE_BLOCK_SUB(8)), (u16*)(CHAR_BASE_BLOCK_SUB(0)));
  }

    glDefaultClassCreate();
    //videoSub_SetShowLog(true);
    SetARM9_REG_WaitCR();
    if(FAT2_InitFiles()==false)
    {
        _consolePrint("FAT_InitFiles() failed.\n");
        ShowLogHalt();
    }
    SetARM9_REG_WaitCR();
    _consolePrintf("boot %s %s\n%s\n%s\n\n",ROMTITLE,ROMVERSION,ROMDATE,ROMENV);
    _consolePrintf("__current pc=0x%08x sp=0x%08x\n\n",__current_pc(),__current_sp());
    //CheckGUID_Pre();
    //PrintFreeMem();
    LoadDefaultFont();
    _consolePrintf("%x,%x\n",IPC6,&IPC6->ARM7SelfCheck);
    //CheckGUID_Body();
    //DrawBootWarn(bootwarn_b8zlib,bootwarn_b8zlib_Size,1);  
    SetARM9_REG_WaitCR();
    //extmem_Init();
    //extmem_ShowMemoryInfo();
	//main_VarDebug();
    //if(Shell_FAT_fopen_isExists_Data(LogFilename)==true)
    //{
    //    FAT_FILE *pf=Shell_FAT_fopen_Data(LogFilename);
    //    if(pf!=NULL)
    //    {
    //        _consoleSetLogFile(pf);
    //        FAT2_fclose(pf);
    //    }
    //}
    Shell_ShellSet_Init();
    
    if(ShellSet.Cluster64k==false) FAT2_Disabled64kClusterMode();
    
    _consolePrintf("pDTCMEND=0x%08x, pMTCMEND=0x%08x. MTCM size=%dbyte.\n",pDTCMEND,pMTCMEND,(u32)pMTCMEND-(u32)pDTCMEND);
    pMTCMEND=(u32*)&mtcmend;
    if((0x02804000-1536)<(u32)pMTCMEND)
    {
        _consolePrintf("MTCM overflow. 0x%08x 0x%08x\n",0x02800000+1536,pMTCMEND);
        ShowLogHalt();
    }

    {
        _consolePrintf("Current stack pointer: 0x%08x.\n",stackptr);
        for(s32 idx=0;idx<32;idx+=4)
        {
            _consolePrintf("0x%08x: 0x%08x, 0x%08x, 0x%08x, 0x%08x.\n",stackptr+((-16+idx)*4),stackdata[idx+0],stackdata[idx+1],stackdata[idx+2],stackdata[idx+3]);
        }
    }
    //_consolePrintf("Start FPGA Initializer.\n");
    //FPGA_BusOwnerARM9();
    isExistsROMEO2=false;
/*    
    if(FPGA_isExistCartridge()==false)
    {
        _consolePrintf("Can not found ROMEO2 cartridge on GBA slot.\n");
    }
    else
    {
        bool halt=true;
        void *pFPGAData=NULL;
        s32 FPGADataSize=0;
        if(Shell_FAT_ReadAlloc(FPGAFilename,&pFPGAData,&FPGADataSize)==false)
        {
            _consolePrintf("Can not found RAW file. [%s]\n",FPGAFilename);
        }
        else
        {
            if(FPGA_CheckBitStreamFormat(pFPGAData,FPGADataSize)==false)
            {
                _consolePrintf("ROMEO2 FPGA bit stream unknown format?\n");
            }
            else
            {
                if(FPGA_Start(pFPGAData,FPGADataSize)==false)
                {
                    _consolePrintf("ROMEO2 configration failed.\n");
                }
                else
                {
                    FPGA_BusOwnerARM7();
                    _consolePrintf("Wait for ARM7 init.\n");
                    IPC6->RequestFPGAInit=true;
                    while(IPC6->RequestFPGAInit==true);
                    _consolePrintf("Initialized.\n");
                    halt=false;
                    isExistsROMEO2=true;
                }
            }
            safefree(pFPGAData); pFPGAData=NULL;
        }
        if(halt==true) ShowLogHalt();
    }
*/  
    {
        DateTime_ResetNow();
        TDateTime dt=DateTime_GetNow();
        TFAT2_TIME ft;
    
        ft.Year=dt.Date.Year;
        ft.Month=dt.Date.Month;
        ft.Day=dt.Date.Day;
        ft.Hour=dt.Time.Hour;
        ft.Minuts=dt.Time.Min;
        ft.Second=dt.Time.Sec;
        FAT2_SetSystemDateTime(ft);
    }
    IPC6->LCDPowerControl=LCDPC_ON_BOTH;
    
    Shell_CheckDataPath();
    
    InitInterrupts();
    //strpcmSetVolume64(64);
    strpcmSetVolume64(0);
  
    if(true)
    {
        u32 UserLanguage=(u32)-1;
        u32 Timeout=0x10000;
    
        while(UserLanguage==(u32)-1)
        {
            UserLanguage=IPC6->UserLanguage;
            Timeout--;
            if(Timeout==0)
            {
                _consolePrintf("NDS farmware language read error. ARM7CPU stopped...?\n");
                while(1);
            }
        }
        _consolePrintf("NDS farmware language ID : %d\n",UserLanguage);
    }
    LangInitAndLoadFont();
     
    //chkflag=CheckGUID_Pre_for_CheckTCM();
    //CheckGUID_Body();
}

static __attribute__ ((noinline)) void main_ins_end(void)
{
  FAT2_FreeFiles();
  
  glDefaultClassFree();
  
  _consolePrint("Terminated.\n");
extern int main_wma(void);
//  main_wma();
}

int main(void)
{
    REG_IME=0;
    main_ins_start();
    {
        SetARM9_REG_WaitCR();
        PrintFreeMem();
        mainloop();
        PrintFreeMem();
    }
    REG_IME=0;
    main_ins_end();
    return(0);
}

// -------------------------------- mainloop

void WaitForVBlank(void)
{
  if(VBlankPassedFlag==false){
    swiWaitForVBlank();
  }
  VBlankPassedFlag=false;
}

// ------------------------------------------------------------

static TCallBack CallBack;

static void CallBackInit(void)
{
  MemSet32CPU(0,&CallBack,sizeof(TCallBack));
}

void CallBack_ExecuteVBlankHandler(void)
{
  if(CallBack.VBlankHandler!=NULL) CallBack.VBlankHandler();
}

TCallBack* CallBack_GetPointer(void)
{
  return(&CallBack);
}

// ------------------------------------------------------------

static bool mf;
static s32 mx,my;

static void Proc_TouchPad(u32 VsyncCount)
{
  if(IPC6->RequestUpdateIPC==true) return;
  
  bool tpress;
  s32 tx,ty;
  
  if((IPC6->buttons&IPC_PEN_DOWN)==0){
    tpress=false;
    tx=0;
    ty=0;
    }else{
    tpress=true;
    tx=IPC6->touchXpx;
    ty=IPC6->touchYpx;
  }
  
  IPC6->RequestUpdateIPC=true;
  
  if(tpress==true){
    if(mf==false){
      mf=true;
      if(CallBack.MouseDown!=NULL) CallBack.MouseDown(tx,ty);
      mx=tx;
      my=ty;
      }else{
      s32 dx=abs(mx-tx);
      s32 dy=abs(my-ty);
      if((1<=dx)||(1<=dy)){
        if(CallBack.MouseMove!=NULL) CallBack.MouseMove(tx,ty);
        mx=tx;
        my=ty;
      }
    }
    }else{
    if(mf==true){
      mf=false;
      if(CallBack.MouseUp!=NULL) CallBack.MouseUp(mx,my);
    }
  }
}

#include "main_keyrepeat.h"

static u32 KEYS_Last;
static bool KEYS_PressedLR;
static u32 KEYS_PressStartCount,KEYS_PressSelectCount;
static bool KEYS_HPSwitch_Pressed;

void Proc_KeyInput_Init(void)
{
  KEYS_Last=~0;
  KEYS_PressedLR=false;
  KEYS_PressStartCount=0;
  KEYS_PressSelectCount=0;
  KEYS_HPSwitch_Pressed=false;
}

#include "main_savepreview.h"

void Proc_KeyInput(u32 VsyncCount)
{
  if(KeyRepeatFlag==true){ cwl();
    if(KeyRepeatCount<=VsyncCount){ cwl();
      KeyRepeatCount=0;
      }else{ cwl();
      KeyRepeatCount-=VsyncCount;
    }
  }
  
  u32 KEYS_Cur=(~REG_KEYINPUT)&0x3ff;
  
  {
    u32 btns=IPC6->buttons;
    
    KEYS_Cur|=(~REG_KEYINPUT)&0x3ff;
    if((btns&IPC_PEN_DOWN)!=0) KEYS_Cur|=KEY_TOUCH;
    if((btns&IPC_X)!=0) KEYS_Cur|=KEY_X;
    if((btns&IPC_Y)!=0) KEYS_Cur|=KEY_Y;
  }
  
  {
    const u32 Timeout=60*3;
    if((KEYS_Cur & KEY_START)!=0){/*
      if(KEYS_PressStartCount<Timeout){
        KEYS_PressStartCount+=VsyncCount;
        if(Timeout<=KEYS_PressStartCount){
          if(CallBack.KeyLongPress!=NULL) CallBack.KeyLongPress(KEY_START);
          KEYS_PressStartCount=Timeout;
        }
      }*/
      }else{
      if((KEYS_PressStartCount!=0)&&(KEYS_PressStartCount!=Timeout)){
        if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEY_START);
      }
      KEYS_PressStartCount=0;
    }
    KEYS_Cur&=~KEY_START;
  }
  
  {
    const u32 Timeout=60*3;
    if((KEYS_Cur & KEY_SELECT)!=0){
      if(KEYS_PressSelectCount<Timeout){
        KEYS_PressSelectCount+=VsyncCount;
        if(Timeout<=KEYS_PressSelectCount){
          main_SavePreviewAndHalt();
          ShowLogHalt();
        }
      }
      }else{
      if((KEYS_PressSelectCount!=0)&&(KEYS_PressSelectCount!=Timeout)){
        if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEY_SELECT);
      }
      KEYS_PressSelectCount=0;
    }
    KEYS_Cur&=~KEY_SELECT;
  }
  /*
  if((KEYS_Cur&(KEY_L|KEY_R))==(KEY_L|KEY_R)){
    if(KEYS_PressedLR==false){
      KEYS_PressedLR=true;
      if(CallBack.KeySameLRDown!=NULL) CallBack.KeySameLRDown();
    }
  }
  if((KEYS_Cur&(KEY_L|KEY_R))!=(KEY_L|KEY_R)){
    if(KEYS_PressedLR==true){
      KEYS_PressedLR=false;
      if(CallBack.KeySameLRUp!=NULL) CallBack.KeySameLRUp();
    }
  }
  */
  const u32 DupMask=KEY_A|KEY_B|KEY_X|KEY_Y|KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT;
  
  if((KEYS_Last&DupMask)!=0){
    u32 l=KEYS_Last&DupMask;
    u32 c=KEYS_Cur&DupMask;   
    KEYS_Cur&=KEYS_Last;
  }
  
  KEYS_Last=KEYS_Cur;
  KEYS_Cur=KeyRepeat_Proc(KEYS_Cur,VsyncCount);
  
  if(8<VsyncCount) VsyncCount=8;
  
  if(KEYS_Cur!=0){
    if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEYS_Cur);
  }
  
}

static bool Proc_PanelOpened_Last;

static void Proc_PanelOpened(void)
{
  if(Proc_PanelOpened_Last!=IPC6->PanelOpened){
    Proc_PanelOpened_Last=IPC6->PanelOpened;
    if(Proc_PanelOpened_Last==true){
      if(CallBack.PanelOpen!=NULL) CallBack.PanelOpen();
      }else{
      if(CallBack.PanelClose!=NULL) CallBack.PanelClose();
    }
  }
}

#include "main_Trigger.h"

// ------------------------------------------------------------

EProcFadeEffect ProcFadeEffect;

static const u32 HorizontalFadeStepCount=35;
static const u32 HorizontalFadeStep[HorizontalFadeStepCount]={2,4,4,6,6,8,8,8,8,10,10,10,10,10,10,10,10,10,10,10,10,8,8,8,8,8,8,6,6,6,4,4,4,2,2,};

static const u32 VerticalFadeStepCount=27;
static const u32 VerticalFadeStep[VerticalFadeStepCount]={2,4,5,7,8,8,9,9,9,9,9,9,9,9,9,9,9,9,8,8,7,7,6,5,4,3,2,};

void ScreenMain_Flip_ProcFadeEffect(void)
{
  if(NextProc!=ENP_Loop) return;
  
  if(ProcState.System.EnableFadeEffect==false) ProcFadeEffect=EPFE_None;
  
  u16 *pviewbuf=pScreenMain->pViewCanvas->GetVRAMBuf();
  u16 *pbackbuf=pScreenMain->pBackCanvas->GetVRAMBuf();
  
  switch(ProcFadeEffect){
    case EPFE_None: {
    } break;
    case EPFE_LeftToRight: {
      u32 sx=0;
      for(u32 idx=0;idx<HorizontalFadeStepCount;idx++){
        u32 step=HorizontalFadeStep[idx];
        Splash_Update();
        swiWaitForVBlank();
        u16 tmpbuf[ScreenWidth];
        for(u32 y=0;y<ScreenHeight;y++){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          u16 *pbacklinebuf=&pbackbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[0],tmpbuf,(ScreenWidth-step)*2);
          MemCopy32CPU(tmpbuf,&pviewlinebuf[step],(ScreenWidth-step)*2);
          MemCopy32CPU(&pbacklinebuf[ScreenWidth-step-sx],&pviewlinebuf[0],step*2);
        }
        sx+=step;
      }
    } break;
    case EPFE_RightToLeft: {
      u32 sx=0;
      for(u32 idx=0;idx<HorizontalFadeStepCount;idx++){
        u32 step=HorizontalFadeStep[idx];
        Splash_Update();
        swiWaitForVBlank();
        for(u32 y=0;y<ScreenHeight;y++){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          u16 *pbacklinebuf=&pbackbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[step],&pviewlinebuf[0],(ScreenWidth-step)*2);
          MemCopy32CPU(&pbacklinebuf[sx],&pviewlinebuf[ScreenWidth-step],step*2);
        }
        sx+=step;
      }
    } break;
    case EPFE_UpToDown: {
      u32 sy=0;
      for(u32 idx=0;idx<VerticalFadeStepCount;idx++){
        u32 step=VerticalFadeStep[idx];
        Splash_Update();
        swiWaitForVBlank();
        s32 y;
        y=ScreenHeight-(step*2);
        y=(y/step)*step;
        for(;y>=0;y-=step){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[0*ScreenWidth],&pviewlinebuf[step*ScreenWidth],(step*ScreenWidth)*2);
        }
        u16 *pviewlinebuf=&pviewbuf[0*ScreenWidth];
        u16 *pbacklinebuf=&pbackbuf[0*ScreenWidth];
        MemCopy32CPU(&pbacklinebuf[(ScreenHeight-step-sy)*ScreenWidth],&pviewlinebuf[0*ScreenWidth],(step*ScreenWidth)*2);
        sy+=step;
      }
    } break;
    case EPFE_DownToUp: {
      u32 sy=0;
      for(u32 idx=0;idx<VerticalFadeStepCount;idx++){
        u32 step=VerticalFadeStep[idx];
        Splash_Update();
        swiWaitForVBlank();
        for(u32 y=0;y<ScreenHeight-step;y+=step){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[step*ScreenWidth],&pviewlinebuf[0*ScreenWidth],(step*ScreenWidth)*2);
        }
        u16 *pviewlinebuf=&pviewbuf[0*ScreenWidth];
        u16 *pbacklinebuf=&pbackbuf[0*ScreenWidth];
        MemCopy32CPU(&pbacklinebuf[sy*ScreenWidth],&pviewlinebuf[(ScreenHeight-step)*ScreenWidth],(step*ScreenWidth)*2);
        sy+=step;
      }
    } break;
    case EPFE_CrossFade: {
      for(u32 idx=16;idx>0;idx--){
        Splash_Update();
        WaitForVBlank();
        pScreenMain->SetBlendLevel(idx);
        while(VBlankPassedFlag==false){
          DLLSound_Update();
        }
      }
    } break;
    case EPFE_FastCrossFade: {
      for(u32 idx=16;idx>0;idx-=4){
        Splash_Update();
        WaitForVBlank();
        pScreenMain->SetBlendLevel(idx);
        while(VBlankPassedFlag==false){
          DLLSound_Update();
        }
      }
    } break;
  }
  pScreenMain->Flip(true);
  
  if(ProcFadeEffect!=EPFE_None){
    ProcFadeEffect=EPFE_None;
    REG_IME=0;
    VBlankPassedCount=1;
    REG_IME=1;
  }
}

// ------------------------------------------------------------

static void Proc_ExternalPowerPresent(void)
{
  static bool LastState;
  
  static bool FirstStart=true;
  if(FirstStart==true){
    FirstStart=false;
    LastState=IPC6->ExternalPowerPresent;
    return;
  }
  
  bool curstate=IPC6->ExternalPowerPresent;
  if(LastState==curstate) return;
  LastState=curstate;
  
  if(curstate==true){
    if(CallBack.ExternalPowerAttach!=NULL) CallBack.ExternalPowerAttach();
    }else{
    if(CallBack.ExternalPowerDetach!=NULL) CallBack.ExternalPowerDetach();
  }
}

// ------------------------------------------------------------

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

#include "bootreply_b8zlib.h"

// ------------------------------------------------------------
void LangInitAndLoadFontExt(void)
{
	LangInitAndLoadFont();
}
static __attribute__ ((noinline)) void mainloop_ins_start(void)
{
    ProcState_Init();

    ProcState_Load();	
    //videoSub_SetShowLog(true);
    ListSkinDirFile();
   // printSkinFileList();
    
    ListMulLangFile();
  //  printMulLangFileList();
  // while(1);
    Lang_Load();
    const char *pfmt=Lang_GetUTF8("DateTimeFormat");
    if(isStrEqual_NoCaseSensitive(pfmt,"YYYYMMDD")==true) Date_SetDateFormat(EDF_YMD);
    if(isStrEqual_NoCaseSensitive(pfmt,"DDMMYYYY")==true) Date_SetDateFormat(EDF_DMY);
    if(isStrEqual_NoCaseSensitive(pfmt,"MMDDYYYY")==true) Date_SetDateFormat(EDF_MDY);

  
    if(chkflag==true)
    {
        ProcState.System.BootCount++;
        if(ProcState.System.BootCount==16)
        {
            DrawBootWarn(bootreply_b8zlib,bootreply_b8zlib_Size,1);
            while(1);
        }
        ProcState_RequestSave=true;
        ProcState_Save();
    }  
    if(true)
    {
        _consolePrintf("Initialize random seed.\n");
        DateTime_ResetNow();
        TDateTime now=DateTime_GetNow();
        u32 cnt=((now.Time.Min*60)+now.Time.Sec)&0xff;
        for(u32 idx=0;idx<cnt;idx++)
        {
            rand();
        }
    }
    ErrorDialog_Init();
    Component_SetFont(pCglFontDefault);
    //LaunchState_Load();
    Resume_Load();
  
    ApplyCurrentBacklightLevel();
    strpcmSetVolume64(ProcState.System.Volume64);
    BootROM_Init();
    //Shell_Init_SwapFile();
  
    mf=false;
    mx=0;
    my=0;
  
    KEYS_Last=0;
    KeyRepeatLastKey=0;
    KeyRepeatFlag=false;
    KeyRepeatCount=0;
  
    //DLLList_Init();
    //Splash_Update();
 
    Sound_Init();    
    Splash_Update();

    //set“/”
    UnicodeChar *pcpu=ProcState.FileList.CurrentPathUnicode;
    const char *pafn=StrConvert_Unicode2Ank_Test(pcpu);
    _consolePrintf("Check current path. [%s]\n",pafn);
    const char *pPathAlias=ConvertFull_Unicode2Alias(pcpu,NULL);
    if(pPathAlias==NULL)
    {
        _consolePrintf("Set to default path.\n");
        pcpu[0]=(UnicodeChar)'/';
        pcpu[1]=(UnicodeChar)0;
    }
    else
    {
        if(FAT2_chdir_Alias(pPathAlias)==false)
        {
            _consolePrintf("Set to default path.\n");
            pcpu[0]=(UnicodeChar)'/';
            pcpu[1]=(UnicodeChar)0;
        }
        else
        {
            _consolePrintf("finded. [%s]\n",pPathAlias);
        }
    }
    
    Skin_ClearCustomBG();

	//SkinData_Load();

	//ProcState.SetupW.SkinSelect=1;
	_consolePrintf("\n skin=%d",ProcState.SetupW.SkinSelect);
	//TestPauseKey();
    
    _consolePrintf("Set skin.\n");
    if(1)//Skin_SetFilename(ProcState.System.SkinFilenameUnicode)==false)
    {
        _consolePrintf("Set default skin.\n");
 
        if((ProcState.SetupW.SkinSelect > ListFileNum - 1) || ((ProcState.SetupW.SkinSelect<0)))
        {
        	ProcState.SetupW.SkinSelect = 0;
        }
        else
        {

        	char pp[256];    					        	
        	snprintf(pp,256,"/system/skin/%s",ListFile[ProcState.SetupW.SkinSelect].Filename);
        	StrConvert_Ank2Unicode(pp,ProcState.System.SkinFilenameUnicode);

        	_consolePrintf("\n pp=%s  %d",pp,ProcState.SetupW.SkinSelect);
        	_consolePrintf("\n dr2");
        	printSkinFileList();

        	FAT_FILE *pf=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(pp));
			if(pf)
			{
				FAT2_fclose(pf);
			}
			else
			{
				//
				FAT2_fclose(pf);
				snprintf(pp,256,"/system/skin/%s",ListFile[0].Filename);
				StrConvert_Ank2Unicode(pp,ProcState.System.SkinFilenameUnicode);
	
			}
        }
        if(Skin_SetFilename(ProcState.System.SkinFilenameUnicode)==false)
        {
            _consolePrintf("Not found skin package.\n");
            ShowLogHalt();
        }
    }
  
    PrintFreeMem();
    //Splash_Update();

    VBlankPassedFlag=false;
    VBlankPassedCount=0;
  
    _consolePrintf("Set NextProc.\n");
  
    RelationalFile_Clear();
  
    NextProc=ENP_Loop;
  
    {
    u32 KEYS_Cur=(~REG_KEYINPUT)&0x3ff;
    {
        u32 btns=IPC6->buttons;
        KEYS_Cur|=(~REG_KEYINPUT)&0x3ff;
        if((btns&IPC_PEN_DOWN)!=0) KEYS_Cur|=KEY_TOUCH;
        if((btns&IPC_X)!=0) KEYS_Cur|=KEY_X;
        if((btns&IPC_Y)!=0) KEYS_Cur|=KEY_Y;
    }
    if((NextProc==ENP_Loop)&&(KEYS_Cur==0)) mainloop_autoboot(DefaultNFilename);
    if((NextProc==ENP_Loop)&&(KEYS_Cur==KEY_X)) mainloop_autoboot(DefaultXFilename);
    if((NextProc==ENP_Loop)&&(KEYS_Cur==KEY_Y)) mainloop_autoboot(DefaultYFilename);
    //if(NextProc!=ENP_Loop) Splash_Free();
    }
  
    //ExtLink_Init();
  
    atype_lockall();
    if(NextProc==ENP_Loop)
    {
        atype_showallocated();
        atype_checkoverrange();
        atype_checkmemoryleak();
        Splash_Update();
    }
  
    if(NextProc==ENP_Loop)
    {
        // Resume_SetResumeMode(ERM_None); // Disabled resume function.
        if(Resume_GetResumeMode()!=ERM_None)
        {
            _consolePrintf("Restart resume.\n");
            UnicodeChar PathUnicode[MaxFilenameLength],FilenameUnicode[MaxFilenameLength];
            SplitItemFromFullPathUnicode(Resume_GetFilename(),PathUnicode,FilenameUnicode);
            if(FileExistsUnicode(PathUnicode,FilenameUnicode)==true)
            {
                Unicode_Copy(RelationalFilePathUnicode,PathUnicode);
                Unicode_Copy(RelationalFileNameUnicode,FilenameUnicode);
                Unicode_Copy(ProcState.FileList.CurrentPathUnicode,PathUnicode);
                Unicode_Copy(ProcState.FileList.SelectFilenameUnicode,FilenameUnicode);
                RelationalFilePos=Resume_GetPos();
                Splash_Free();
                switch(Resume_GetResumeMode())
                {
                case ERM_None: break;
                case ERM_Audio: {
                    SetNextProc(ENP_FileList,EPFE_CrossFade);
                } break;
                case ERM_Video: {
                    pScreenSub->pCanvas->FillFull(ColorTable.Video.InitBG);
                    SetNextProc(ENP_DPGPlay,EPFE_None);
                } break;
                case ERM_Image: {
                    SetNextProc(ENP_ImageView,EPFE_None);
                  } break;
                case ERM_Text: {
                    SetNextProc(ENP_TextView,EPFE_None);
                } break;
                }
            }//if(FileExistsUnicode(PathUnicode,FilenameUnicode)==true)
            Resume_Clear();
        }//if(Resume_GetResumeMode()!=ERM_None)
    }
  
    if(NextProc==ENP_Loop)
    {
        SetNextProc(ENP_FileList,EPFE_CrossFade); 
    }
  
    Proc_PanelOpened_Last=IPC6->PanelOpened;
}

#include "EnableCheatData.h"
#include "BootROM.h"
static char NdsRomSign[5];
static u8 toSaveCheatData[512];
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
{
	_consolePrintf("\n this");
	u32 gl_RomCRC;
	uint8 *bm=(uint8*)safemalloc(512);
	_consolePrintf("\n name=%s",pBNRN);
	FAT_FILE *fh=FAT2_fopen_AliasForRead(pBNRN);
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
		 	//CRC32
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
	if(isUserCheatDataExits())
	{
		NDSCheatInfo *pInfo=FindNDSCheatInfo(sign,gl_RomCRC);	
		return pInfo;
	}
	else
	{
		return NULL;
	}
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
extern LPIO_INTERFACE active_interface;
static u32 DataSectorIndex1;
static u8 fbuf1[512];

static int RomDatFileDataSectorIndex1()
{	
	char pFileN[256];
	strcpy(pFileN,BootROM_GetFullPathAlias()) ;
	char*extname = "DAT";
	char*Folder = "/ROMDAT";
    uint32 createSize = 512;
    FAT_FILE *pf=CheckFile_in_Folder(pFileN,extname,Folder,createSize);
    if(pf!=NULL)
    {
        if(FAT2_GetFileSize(pf)==512)
        {
            if(pf->firstCluster!=0)
            {
            	DataSectorIndex1=FAT2_ClustToSect(pf->firstCluster);
            }
        }
        FAT2_fclose(pf);
    }   
}



void CheatData1_Init(void)
{  
    DataSectorIndex1=0;
    const char *fn=DefaultDataPath "/cheatdata.bin";
    const char *palias=ConvertFullPath_Ansi2Alias(fn);
    _consolePrintf("Open setting file. [%s]\n",fn);
    if(palias!=NULL)
    {
        FAT_FILE *pf=pf=FAT2_fopen_AliasForModify(palias);
        if(pf!=NULL)
        {
            if(FAT2_GetFileSize(pf)==512)
            {
                if(pf->firstCluster!=0)
                {
                    DataSectorIndex1=FAT2_ClustToSect(pf->firstCluster);
                }
            }
            FAT2_fclose(pf);
        }
    }
    else
    {
    	
    }
    _consolePrintf("DataSectorIndex=%d.\n",DataSectorIndex1);  
    if(DataSectorIndex1==0)
    {
        _consolePrintf("FatalError: File not found or size error. [%s]\n",DataFilename);
        ShowLogHalt();
    }
}
static void fbuf1_LoadFromDisk(void)
{
  if(DataSectorIndex1==0){
    _consolePrint("FatalError: Data sector index is 0.");
    ShowLogHalt();
  }
  active_interface->fn_ReadSectors(DataSectorIndex1,1,fbuf1);
}
static void CheatDataLoad1(u8* toSaveCheatData)
{
    REG_IME=0;
    fbuf1_LoadFromDisk();
    REG_IME=1;	
	memcpy(toSaveCheatData,fbuf1,0x100);
}

static void isRomDatFolderExist()
{
	FAT_chdir("/");
	FILE_TYPE fstype = FAT_FileExists("/ROMDAT"); 
    if(fstype == FT_DIR)
    { 
    	_consolePrintf("ROMDAT Folder exist \n");    
    }
    else
    {
        _consolePrintf("ROMDAT Folder NOT exist \n");
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVE");
        if(FAT_mkdir("/ROMDAT")==0)
        {
        	_consolePrintf("ROMDAT Folder Create success\n");
        }
        else
            _consolePrintf("ROMDAT Folder Create failed\n");
    }
}
static int GetEnableCheatData(void)
{
	//CheatData1_Init();
	isRomDatFolderExist();
	RomDatFileDataSectorIndex1();
	memset(toSaveCheatData,0x00,512);
	CheatDataLoad1(toSaveCheatData);
	char sign[5];
    {
		sign[0]=toSaveCheatData[0xA0];
		sign[1]=toSaveCheatData[0xA1];
		sign[2]=toSaveCheatData[0xA2];
		sign[3]=toSaveCheatData[0xA3];
		sign[4]=0;
	}
    
    
    u8 isItemEnable[1024];
    memset(isItemEnable,0x00,1024);
    
    NDSCheatInfo *pInfo=FindSelectNDSCheatHeadInfo();		    
    if(pInfo)
    {
    	safefree(pInfo);
    }
    else
    {
    	if(!memcmp(sign,NdsRomSign,5))
       	{  
    		_consolePrintf("\n Here12");
    		if(toSaveCheatData[0xb0]==0x01)
           		RomSetState.SoftReset=true;
           	else
           		RomSetState.SoftReset=false;
           	if(toSaveCheatData[0xb1]==0x01)
           		RomSetState.RealtimeSave=true;
           	else
           		RomSetState.RealtimeSave=false;
           	if(toSaveCheatData[0xb2]==0x01)
           		RomSetState.GameGuide=true;
           	else
           		RomSetState.GameGuide=false;  
           	if(toSaveCheatData[0xb3]==0x01)
           		RomSetState.CheatState=true;
           	else
           		RomSetState.CheatState=false;
           	if(toSaveCheatData[0xb4]==0x01)
           		RomSetState.DownloadPlayState=true;
           	else
           		RomSetState.DownloadPlayState=false;
           	if(toSaveCheatData[0xb5]==0x01)
           	{
           		RomSetState.SpeciaMode=true;
    			RomSetState.Speed=(toSaveCheatData[0xb6]==0)?5:toSaveCheatData[0xb6];
           	}
           	else
           		RomSetState.SpeciaMode=false;
       	}
    	return;
    }
    _consolePrintf("\n %c%c%c%c",NdsRomSign[0],NdsRomSign[1],NdsRomSign[2],NdsRomSign[3]);
   // TestPauseKey();
    
    
    if(!memcmp(sign,NdsRomSign,5))
	{  
    	if(toSaveCheatData[0xb0]==0x01)
    		RomSetState.SoftReset=true;
    	else
    		RomSetState.SoftReset=false;
    	if(toSaveCheatData[0xb1]==0x01)
    		RomSetState.RealtimeSave=true;
    	else
    		RomSetState.RealtimeSave=false;
    	if(toSaveCheatData[0xb2]==0x01)
    		RomSetState.GameGuide=true;
    	else
    		RomSetState.GameGuide=false;  
    	if(toSaveCheatData[0xb3]==0x01)
    		RomSetState.CheatState=true;
    	else
    		RomSetState.CheatState=false;
       	if(toSaveCheatData[0xb4]==0x01)
       		RomSetState.DownloadPlayState=true;
       	else
       		RomSetState.DownloadPlayState=false;    	
       	if(toSaveCheatData[0xb5]==0x01)
       	{
       		RomSetState.SpeciaMode=true;
			RomSetState.Speed=(toSaveCheatData[0xb6]==0)?5:toSaveCheatData[0xb6];
       	}
       	else
       		RomSetState.SpeciaMode=false; 
       	
    	if(!RomSetState.CheatState)
    		return;    	
    	
    	for(u32 idx=0;idx<128;idx++)
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
	_consolePrintf("\n savedata=%x",toSaveCheatData[0]);
	_consolePrintf("\n isItemEnable=%x %x",isItemEnable[0],isItemEnable[1]);
	    StartNDSCheatWindows();
		AR_DATA *pARCheatCur;
		pARCheatCur=m_pARDataHead;
		pARCheatCur=pARCheatCur->pARNextData;
		EnableNum=0;
		u32 iidx=0;
		while(pARCheatCur!=NULL)
		{
			if(isItemEnable[iidx] == 0x01)
			{
				//enable cheatdata
				
				EnableCheatData[EnableNum].datasize=pARCheatCur->ARCheat.datasize;
				EnableCheatData[EnableNum].pData=(uint32 *)safemalloc(EnableCheatData[EnableNum].datasize*4);
				memcpy(EnableCheatData[EnableNum].pData,pARCheatCur->ARCheat.pData,EnableCheatData[EnableNum].datasize*4);
				EnableNum++;
			}
			iidx++;
			if(pARCheatCur->pARNextData)
			{	
				pARCheatCur=pARCheatCur->pARNextData;			
			}
			else
			{
				break;					
			}
			if(iidx>1024)
				break;
		}
		DeleteAR_DATA();
	} 
}
static bool isHomeBrew(char *pFullPathName)
{

	  u8 header[16];
	  {
	    FAT_FILE *pf=FAT2_fopen_AliasForRead(pFullPathName);
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
static __attribute__ ((noinline)) void mainloop_ins_end(void)
{
  Skin_Free();
  Skin_CloseFile();
   
  Sound_Stop();
  Sound_Free();  
  _consolePrint("mainloop terminated.\n");
  _consolePrintf("Reboot ROM '%s'\n",BootROM_GetFullPathAlias());
  TProcState *CurProcState;
  CurProcState = &ProcState;
  if(1/*CurProcState->SetupW.CheatOnOff==true*/){
	  
	  char p[256];
	  if(!isHomeBrew(strcpy(p,BootROM_GetFullPathAlias())))
	  {
		// if(isUserCheatDataExits())
		  {
			  GetEnableCheatData();	  
		  } 
	  }
  }
  BootNDSROM();
}

// ------------------------------------------------------------

static bool chkstack;

static __attribute__ ((noinline)) void mainloop_ins_loopstart(void)
{
    if(NextProc!=ENP_BootROM){
        
      _consolePrintf("Wait for key releases.\n");
      while(1){
        if(IPC6->RequestUpdateIPC==false){
          u32 btns=IPC6->buttons;
          u32 keys=(~REG_KEYINPUT)&0x3ff;
          if((btns&IPC_PEN_DOWN)!=0) keys|=KEY_TOUCH;
          if((btns&IPC_X)!=0) keys|=KEY_X;
          if((btns&IPC_Y)!=0) keys|=KEY_Y;
          if(keys==0) break;
          IPC6->RequestUpdateIPC=true;
        }
      }
    }
    
    chkstack=true;

    if(NextProc==ENP_DPGPlay) chkstack=false;
    
    if(chkstack==true){ // setup stack overflow checker
      u32 *p=pDTCMEND;
      for(;p<(u32*)__current_sp();p++){
        *p=(u32)p;
      }
      }else{
      u32 *p=pMTCMEND;
      for(;p<(u32*)__current_sp();p++){
        *p=(u32)p;
      }
    }
    
    CallBackInit();
    Skin_Free();    
   
    switch(NextProc){
      case ENP_Loop: {
        _consolePrintf("Illigal process error! NextProc==ENP_Loop\n");
        ShowLogHalt();
      } break;
      case ENP_ChkDsk: //Skin_Load_ChkDsk(); break;
          break;
      case ENP_Setup: Skin_Load_Setup(); break;
      case ENP_FileList: Skin_Load_FileList(); break;
      case ENP_SysMenu: Skin_Load_SysMenu(); break;
      case ENP_DPGCustom: //Skin_Load_DPGCustom(); 
          break;
      case ENP_DPGPlay: //Skin_ClearCustomBG(); Skin_Load_DPGPlay(); 
          break;
      case ENP_ImageCustom: //Skin_Load_ImageCustom(); 
          break;
      case ENP_ImageView: //Skin_Load_ImageView(); 
          break;
      case ENP_TextCustom: Skin_Load_TextCustom(); break;
      case ENP_TextView: Skin_Load_TextView(); break;
      case ENP_Launch: //Skin_Load_Launch(); 
          break;
      case ENP_Custom: Skin_Load_Custom(); break;
      case ENP_BootROM: Skin_Load_BootROM(); break;
      case ENP_SetupW: Skin_Load_SetupW(); break;
      case ENP_Cheat: Skin_Load_Cheat(); break;
      case ENP_ROMSET: Skin_Load_RomSet(); break;
      case ENP_ShutDown: Skin_Load_ShutDown(); break;
      case ENP_CpyCut: Skin_Load_CpyCut(); break;
      case ENP_GBA: Skin_Load_GBA(); break;
            
      default: {
        _consolePrintf("Unknown process error! NextProc==%d\n",NextProc);
        ShowLogHalt();
      } break;
    }
    
    if((NextProc==ENP_Setup)||(NextProc==ENP_FileList)||(NextProc==ENP_Launch)||(NextProc==ENP_Custom)||(NextProc==ENP_BootROM)) Splash_Free();
    
    switch(NextProc){
      case ENP_Loop: {
        _consolePrintf("Illigal process error! NextProc==ENP_Loop\n");
        ShowLogHalt();
      } break;
      case ENP_ChkDsk: //ProcChkDsk_SetCallBack(&CallBack); break;
          break;
      case ENP_Setup: ProcSetup_SetCallBack(&CallBack); break;
      case ENP_FileList: ProcFileList_SetCallBack(&CallBack); break;
      case ENP_SysMenu: ProcSysMenu_SetCallBack(&CallBack); break;
      case ENP_DPGCustom: //ProcDPGCustom_SetCallBack(&CallBack); 
          break;
      case ENP_DPGPlay: //ProcDPGPlay_SetCallBack(&CallBack); 
          break;
      case ENP_ImageCustom: //ProcImageCustom_SetCallBack(&CallBack); 
          break;
      case ENP_ImageView: //ProcImageView_SetCallBack(&CallBack); 
          break;
      case ENP_TextCustom: ProcTextCustom_SetCallBack(&CallBack); break;
      case ENP_TextView: ProcTextView_SetCallBack(&CallBack); break;
      case ENP_Launch: //ProcLaunch_SetCallBack(&CallBack); 
          break;
      case ENP_Custom: ProcCustom_SetCallBack(&CallBack); break;
      case ENP_BootROM: ProcBootROM_SetCallBack(&CallBack); break;
      case ENP_SetupW: ProcSetupW_SetCallBack(&CallBack);break;
      case ENP_Cheat: ProcCheat_SetCallBack(&CallBack); break;
      case ENP_ROMSET:ProcRomSet_SetCallBack(&CallBack); break;
      case ENP_ShutDown:ProcShutDown_SetCallBack(&CallBack); break;
      case ENP_CpyCut:ProcCpyCut_SetCallBack(&CallBack); break;
      case ENP_GBA:ProcGBA_SetCallBack(&CallBack); break;
      default: {
        _consolePrintf("Unknown process error! NextProc==%d\n",NextProc);
        ShowLogHalt();
      } break;
    }
    
    NextProc=ENP_Loop;
    
    if(CallBack.Start!=NULL) CallBack.Start();
  
    _consolePrintf("\n nnn11_1");
    if(chkstack==true){ // fast stack overflow checker
      DTCM_StackCheck(-1);
      }else{
      MTCM_StackCheck(-1);
    }
    _consolePrintf("\n nnn11_2");
    ProcState_Save();
    
    atype_checkoverrange();
    _consolePrintf("\n nnn11_3");
   // PrintFreeMem();
    _consolePrintf("\n nnn11_4");
    VBlankPassedFlag=false;
    VBlankPassedCount=0;
    _consolePrintf("\n nnn11_5");
    Proc_TouchPad(0);
    _consolePrintf("\n nnn11_6");
    Proc_KeyInput_Init();
    _consolePrintf("\n nnn11_7");
    Proc_Trigger(true,0);
  
    REG_IME=0;
    VBlankPassedCount=0;
    REG_IME=1;
    
    _consolePrintf("\n nnn11");

}

// ------------------------------------------------------------

static __attribute__ ((noinline)) void mainloop_ins_loopend(void)
{
    VBlankPassedFlag=false;
    VBlankPassedCount=0;
    if(CallBack.End!=NULL) CallBack.End();
    
    IPC6->LCDPowerControl=LCDPC_ON_BOTH;
    atype_checkoverrange();
    atype_checkmemoryleak();
    
    if(chkstack==true){
      DTCM_StackCheck(0);
      }else{
      MTCM_StackCheck(0);
    }
}
#include "mycartsrc/CBMPResource.h"
void Clock_Standby_Draw(CglCanvas *pCanvas,int drawatMenu = 0);
void StandbyClock_Init(bool BlackMode);
void StandbyClockAlpha_Init(bool BlackMode);
void StandbyClockAlpha_Free(void);
extern void StandbyClock_Free(void);

// ------------------------------------------------------------
int count =0xd000;
static void PressRight(int menuselect)
{
    _consolePrintf("dosomething menuselect = %d \n",menuselect);
    CBMPResource *pbmpbigicons = new CBMPResource(DefaultDataPath"/boot/big icon.bmp");
    u16 *pbmpbigiconsdata = pbmpbigicons->Get16bitData();
    uint16 hbigicons = pbmpbigicons->GetBMPHeight();
    uint16 wbigicons = pbmpbigicons->GetBMPWidth();
    CglCanvas *pcanbigicons = new CglCanvas(pbmpbigiconsdata,wbigicons,hbigicons,pf15bit);

    for(int num=0;num<5;num++)
    {
        char pStrname[256];
        //char ctemp[50]={0};
        //sprintf(ctemp,"H[%03x]",(num & 0x0000ffff));
        int i_num = menuselect+num;
        if(i_num>5) i_num -=5;
        snprintf(pStrname,256,DefaultDataPath"/boot/%d.bmp",i_num);
        CBMPResource *pbmpresicons = new CBMPResource(pStrname);
        u16 *pbmpicons = pbmpresicons->Get16bitData();
        uint16 hicons = pbmpresicons->GetBMPHeight();
        uint16 wicons = pbmpresicons->GetBMPWidth();
        CglCanvas *pcanicons = new CglCanvas(pbmpicons,wicons,hicons,pf15bit);
        for(int right=62;right>=0;right-=2)
        {
        	pcanbigicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-24+right,114,wbigicons,hbigicons,0,0,false);
        	pScreenMainOverlay->pCanvas->SetColor(0);
        	pScreenMainOverlay->pCanvas->FillBox(num*64+24+right,114,2,48);
        	pcanicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-16+right,122,wicons,hicons,0,0,false);		        	
            if(right!=0) pScreenMainOverlay->pCanvas->FillBox(num*64+16+right,122,2,32); 
            // 
        	for(int ii=0;ii<count;ii++);
        }
        delete pbmpresicons;
        pbmpresicons = NULL ;
        delete pcanicons;
        pcanicons = NULL ;
    }
    delete pbmpbigicons;
    pbmpbigicons = NULL ;
    delete pcanbigicons;
    pcanbigicons = NULL ;	

}
static void PressLeft(int menuselect)
{
    _consolePrintf("menuselect = %d \n",menuselect);
    CBMPResource *pbmpbigicons = new CBMPResource(DefaultDataPath"/boot/big icon.bmp");
    u16 *pbmpbigiconsdata = pbmpbigicons->Get16bitData();
    uint16 hbigicons = pbmpbigicons->GetBMPHeight();
    uint16 wbigicons = pbmpbigicons->GetBMPWidth();
    CglCanvas *pcanbigicons = new CglCanvas(pbmpbigiconsdata,wbigicons,hbigicons,pf15bit);

    for(int num=4;num>=0;num--)
    {
        char pStrname[256];
        int i_num = menuselect+num;
        if(i_num>5) i_num -=5;
        snprintf(pStrname,256,DefaultDataPath"/boot/%d.bmp",i_num);
        CBMPResource *pbmpresicons = new CBMPResource(pStrname);
        u16 *pbmpicons = pbmpresicons->Get16bitData();
        uint16 hicons = pbmpresicons->GetBMPHeight();
        uint16 wicons = pbmpresicons->GetBMPWidth();
        CglCanvas *pcanicons = new CglCanvas(pbmpicons,wicons,hicons,pf15bit);
        for(int right=62;right>=0;right-=2)
        {
        	pcanbigicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-24-right,114,wbigicons,hbigicons,0,0,false);
        	pScreenMainOverlay->pCanvas->SetColor(0);
        	pScreenMainOverlay->pCanvas->FillBox(num*64-26-right,114,2,48);	        	
        	pcanicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-16-right,122,wicons,hicons,0,0,false);		        	
        	if(right!=0) pScreenMainOverlay->pCanvas->FillBox(num*64-18-right,122,2,32); 
            // 
        	for(int ii=0;ii<count;ii++);
        }
        delete pbmpresicons;
        pbmpresicons = NULL ;
        delete pcanicons;
        pcanicons = NULL ;
    }
    delete pbmpbigicons;
    pbmpbigicons = NULL ;
    delete pcanbigicons;
    pcanbigicons = NULL ;	
}
//#include "plug_ndsrom_resetmem.h"
#include <nds/arm9/video.h>
#define _VRAM_OFFSET(n) ((n)<<3)
#define _VRAM_PTR ((u16*)0x06000000)
#define _VRAM_CD_MAIN_BG_0x6000000 (1 | _VRAM_OFFSET(0))
#define _VRAM_CD_MAIN_BG_0x6020000 (1 | _VRAM_OFFSET(1))
#define _VRAM_CD_ARM7_0x6000000 (2 | _VRAM_OFFSET(0))
#define _VRAM_CD_ARM7_0x6020000 (2 | _VRAM_OFFSET(1))
extern void ITCM_resetMemory2_ARM9 (vu32 *pBootAddress,u32 BootAddress,u32 ClearMainMemory);
extern void resetMemory1_ARM9 (void) ;
static void EnterStartMenu(void)
{
	//videoSub_SetShowLog(true);
	
    CBMPResource *pbmpres = new CBMPResource(DefaultDataPath"/boot/bkg.bmp");
    u16 *pbmp = pbmpres->Get16bitData();
    u16 *pv = pScreenMain->pViewCanvas->GetVRAMBuf();
    dmaCopy(pbmp,pv,ScreenWidth*ScreenHeight*2);
    delete pbmpres;
    pbmpres = NULL ;

 
    //StandbyClock_Init(true);
    //StandbyClockAlpha_Init(true);
    //Clock_Standby_Draw(pScreenSub->pCanvas,1);

    pbmpres = new CBMPResource(DefaultDataPath"/boot/logo.bmp");
    pbmp = pbmpres->Get16bitData();    
    pv = pScreenSub->pCanvas->GetVRAMBuf();
    dmaCopy(pbmp,pv,ScreenWidth*ScreenHeight*2);
	REG_POWERCNT&=~POWER_SWAP_LCDS;
    delete pbmpres;
    pScreenSub->pCanvas->SetCglFont(pCglFontDefault);
    pScreenSub->pCanvas->SetFontTextColor(RGB15(31,31,31) | BIT(15)); 
    
    int  menuselect  = 5 ;  
    {
        CBMPResource *pbmpresicons = new CBMPResource(DefaultDataPath"/boot/big icon.bmp");
        u16 *pbmpicons = pbmpresicons->Get16bitData();
        uint16 hicons = pbmpresicons->GetBMPHeight();
        uint16 wicons = pbmpresicons->GetBMPWidth();
        CglCanvas *pcanicons = new CglCanvas(pbmpicons,wicons,hicons,pf15bit);
    	for(int num=0;num<5;num++)
    	{
            pcanicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-24,114,wicons,hicons,0,0,false);
    	}
        delete pbmpresicons;
        pbmpresicons = NULL ;
        delete pcanicons;
        pcanicons = NULL ;	
        
    	for(int num=0;num<5;num++)
    	{
        	char pStrname[256];
        	int i_num = menuselect+num;
        	if(i_num>5) i_num -=5;

        	snprintf(pStrname,256,DefaultDataPath"/boot/%d.bmp",i_num);
            CBMPResource *pbmpresicons = new CBMPResource(pStrname);
            u16 *pbmpicons = pbmpresicons->Get16bitData();
            uint16 hicons = pbmpresicons->GetBMPHeight();
            uint16 wicons = pbmpresicons->GetBMPWidth();
            CglCanvas *pcanicons = new CglCanvas(pbmpicons,wicons,hicons,pf15bit);
            pcanicons->BitBlt(pScreenMainOverlay->pCanvas,num*64-16,122,wicons,hicons,0,0,false);
            delete pbmpresicons;
            pbmpresicons = NULL ;
            delete pcanicons;
            pcanicons = NULL ;
    	}
    	
    }

    const TRect  menurect1={-24,114,48,48};
    const TRect  menurect2={40,114,48,48};
    const TRect  menurect3={104,114,48,48};
    const TRect  menurect4={168,114,48,48};
    const TRect  menurect5={232,114,48,48};
    uint32  noSelectCount = 0;
    u8 color = 0 ,quit = 1;
    int colordirection = 1;
    TPoint rp ;

    KeyRead(&gKey);
    pScreenMainOverlay->pCanvas->SetCglFont(pCglFontDefault);
    pScreenMain->pViewCanvas->SetCglFont(pCglFontDefault);
    pScreenMainOverlay->SetAlpha(15);
    
    do
    {            

        KeyRead(&gKey);
        WaitForVBlank();
        noSelectCount ++ ;
        if(gKey.trg&PAD_TOUCH)
        {
            rp.x = IPC6->touchXpx;
            rp.y = IPC6->touchYpx;
            if(isInsideRect(menurect1,rp.x,rp.y))
            {
            	_consolePrintf("    menuselect = %d \n",menuselect);
                pScreenMainOverlay->pCanvas->SetColor(RGB15(0,31,0)|0x8000);
                for(int line=1;line<7;line++)
                {
                	pScreenMainOverlay->pCanvas->DrawBox(menurect1.x+line,menurect1.y+line,menurect1.w-2*line,menurect1.h-2*line);
                }
                if(menuselect==1) menuselect=4;
            	else if(menuselect==2) menuselect=5;
            	else if(menuselect==3) menuselect=1;
            	else if(menuselect==4) menuselect=2;
            	else if(menuselect==5) menuselect=3;
            	PressLeft(menuselect);
            }
            else if(isInsideRect(menurect2,rp.x,rp.y))
            {
                pScreenMainOverlay->pCanvas->SetColor(RGB15(0,31,0)|0x8000);
                for(int line=1;line<7;line++)
                {
                	pScreenMainOverlay->pCanvas->DrawBox(menurect2.x+line,menurect2.y+line,menurect2.w-2*line,menurect2.h-2*line);
                }
            	if(menuselect==1) menuselect=5;
            	else if(menuselect==2) menuselect=1;
            	else if(menuselect==3) menuselect=2;
            	else if(menuselect==4) menuselect=3;
            	else if(menuselect==5) menuselect=4;
            	PressLeft(menuselect);
            }
            else if(isInsideRect(menurect3,rp.x,rp.y))
            {
                noSelectCount = 0;
                quit = 0;
                
            }
            else if(isInsideRect(menurect4,rp.x,rp.y))
            {
                pScreenMainOverlay->pCanvas->SetColor(RGB15(0,31,0)|0x8000);
                for(int line=1;line<7;line++)
                {
                	pScreenMainOverlay->pCanvas->DrawBox(menurect4.x+line,menurect4.y+line,menurect4.w-2*line,menurect4.h-2*line);
                }
                if(menuselect==1) menuselect=2;
            	else if(menuselect==2) menuselect=3;
            	else if(menuselect==3) menuselect=4;
            	else if(menuselect==4) menuselect=5;
            	else if(menuselect==5) menuselect=1;
            	PressRight(menuselect);
            }
            else if(isInsideRect(menurect5,rp.x,rp.y))
            {
                pScreenMainOverlay->pCanvas->SetColor(RGB15(0,31,0)|0x8000);
                for(int line=1;line<7;line++)
                {
                	pScreenMainOverlay->pCanvas->DrawBox(menurect5.x+line,menurect5.y+line,menurect5.w-2*line,menurect5.h-2*line);
                }           	if(menuselect==1) menuselect=3;
            	else if(menuselect==2) menuselect=4;
            	else if(menuselect==3) menuselect=5;
            	else if(menuselect==4) menuselect=1;
            	else if(menuselect==5) menuselect=2;
            	PressRight(menuselect);
            }
        }
        if(gKey.trg&PAD_BUTTON_A)
        {
            noSelectCount = 0;
            quit = 0;
        }
        if(gKey.trg&PAD_BUTTON_X)
        {
            _consolePrintf("X is triger \n");
        }
        /*if(gKey.trg&PAD_KEY_UP)
        {
        	count += 0x1000;
            _consolePrintf("count = %x \n",count);
        }
        if(gKey.trg&PAD_KEY_DOWN)
        {
        	count -= 0x1000;
            _consolePrintf("count = %x \n",count);
        }*/
        if(gKey.trg&PAD_KEY_RIGHT)
        {
            noSelectCount = 0;
            menuselect ++ ;
            if(menuselect>5)
                menuselect = 1;
            
            PressRight(menuselect);
        }
        if(gKey.trg&PAD_KEY_LEFT)
        {
            noSelectCount = 0;
            menuselect -- ;
            if(menuselect<1)
                menuselect = 5 ;
            PressLeft(menuselect);
        }
       
    	pScreenMainOverlay->pCanvas->SetColor(0);
    	pScreenMainOverlay->pCanvas->FillBox(50,30,180,24);
    	pScreenMainOverlay->pCanvas->SetFontTextColor(0x8000); 
        switch(menuselect)
        {
        case 1:
        	pScreenMainOverlay->pCanvas->TextOutUTF8(60,30,Lang_GetUTF8("STS_GBA"));
            break;
        case 2:
        	pScreenMainOverlay->pCanvas->TextOutUTF8(60,30,Lang_GetUTF8("STS_Moonshell"));            
            break;
        case 3:
        	pScreenMainOverlay->pCanvas->TextOutUTF8(55,30,Lang_GetUTF8("STS_Slot2"));          
            break;
        case 4:
        	pScreenMainOverlay->pCanvas->TextOutUTF8(80,30,Lang_GetUTF8("STS_Setting"));          
            break;
        case 5:
        	pScreenMainOverlay->pCanvas->TextOutUTF8(80,30,Lang_GetUTF8("STS_NDS"));          
            break;
        }
        pScreenMainOverlay->pCanvas->TextOutA(4,175,"Ver 1.05");   

        char DataTimeChar[256];
        memset(DataTimeChar,0,256);
        DateTime_ResetNow();
        TDateTime now=DateTime_GetNow();
        snprintf(DataTimeChar,256,"%02d/%02d %02d:%02d",now.Date.Day,now.Date.Month,now.Time.Hour,now.Time.Min);
        pScreenMainOverlay->pCanvas->SetColor(0);
        pScreenMainOverlay->pCanvas->FillBox(180,175,96,24);
        pScreenMainOverlay->pCanvas->SetFontTextColor(0x8000);
        pScreenMainOverlay->pCanvas->TextOutA(180,175,DataTimeChar);
        
    }while(quit);
    
    switch(menuselect)
    {
    case 1:
    	isOnlyNDS = false;
    	SetupWisComefromMenu = false;
    	break;
    case 2: //MOONSHELL
        {
			if(FullPath_FileExistsAnsi("/moonshl2.nds"))
			{
				mainloop_autoboot("/moonshl2.nds");
				BootROM_SetInfo((UnicodeChar *)L"/",(UnicodeChar *)L"moonshl2.nds");
			}
			else
			{
				pScreenMainOverlay->pCanvas->TextOutUTF8(80,30,Lang_GetUTF8("GotoMS2_Site1"));	
				pScreenMainOverlay->pCanvas->TextOutUTF8(80,50,Lang_GetUTF8("GotoMS2_Site2"));
			}
        }  
        break;
    case 3:
		{
		 	WAIT_CR &= ~0x80;
			OpenNorWrite();
			chip_reset();
			SetSerialMode();
			CloseNorWrite();
			if(*(vuint16*)0x080000b2 == 0x96)
			{	
				Enable_Arm9DS();
				OpenNorWrite();
				chip_reset();
				SetSerialMode();
				u32 idf=ReadNorFlashID();			
				CloseNorWrite();
				//Enable_Arm9DS();
				_consolePrintf("  idf = %x \n",idf);
				pScreenMainOverlay->pCanvas->FillBox(10,30,180,24);
				pScreenMainOverlay->pCanvas->FillBox(10,50,180,24);
				if(idf>0) 
				{
					pScreenMainOverlay->pCanvas->TextOutUTF8(50,30,Lang_GetUTF8("STS_Slot2_M4"));
					pScreenMainOverlay->pCanvas->TextOutUTF8(50,50,Lang_GetUTF8("STS_Slot2_M5"));
					while(1)
					{
						KeyRead(&gKey);
						_consolePrintf("  gKey.trg = %x \n",gKey.trg);
	
						if(gKey.trg&PAD_BUTTON_A)
						{
							break;
						}
					}
					
				}
				else
				{
					//videoSetModeSub_SetShowLog(false);
					_consolePrintf("Found GBA cart in slot1 ! \n");
					_consolePrintf("press A key to continue B cancel [A/B] \n");
					pScreenMainOverlay->pCanvas->TextOutUTF8(30,30,Lang_GetUTF8("STS_Slot2_M1"));
					pScreenMainOverlay->pCanvas->TextOutUTF8(10,50,Lang_GetUTF8("STS_Slot2_M2"));
					while(1)
					{
						KeyRead(&gKey);
						if(gKey.trg&PAD_BUTTON_A)
						{
							VBlank_AutoFlip_Disabled();
							pScreenSub->pCanvas->FillFull(0);
							pScreenMain->SetMode(ESMM_Normal);
							pScreenMain->Flip(true);
							pScreenMain->pViewCanvas->FillFull(0);
							pScreenMainOverlay->pCanvas->FillFull(0);
							_consolePrintf(" press A\n");
					 	 	WAIT_CR = 0xe880;
							REG_WRAMCNT = 3 ;
							
							resetMemory1_ARM9();
							          
							VRAM_C_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6000000;
							VRAM_D_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6020000;
							           
							void (*lp_resetMemory2_ARM9) (vu32 *pBootAddress,u32 BootAddress,u32 ClearMainMemory)=(void(*)(vu32 *pBootAddress,u32 BootAddress,u32 ClearMainMemory))(0x027f0000);
							       
							u32 *psrc=(u32*)ITCM_resetMemory2_ARM9;
							u32 *pdst=(u32*)lp_resetMemory2_ARM9;
							    
							for(int i=0;i<4*1024;i++)
							{
								*(pdst+i) = *(psrc+i) ;
							}
							lp_resetMemory2_ARM9(&IPC6->RESET_BootAddress,1,2);
						}
						if(gKey.trg&PAD_BUTTON_B)
						{
							_consolePrintf("PassME cancelled ! \n");
							 break;
						}
					}	
				}
		 	}
		 	else
	 		{
				_consolePrintf("No GBA cart in slot1 ! \n");
				pScreenMainOverlay->pCanvas->FillBox(10,50,180,24);
				pScreenMainOverlay->pCanvas->TextOutUTF8(10,50,Lang_GetUTF8("STS_Slot2_M3"));
	 		}
	 	 	WAIT_CR |= 0x80;
		}
    	break;
    case 4: 
    	SetupWisComefromMenu = true;
    	//dmaCopy(pScreenSub->pCanvas->GetVRAMBuf(),pScreenMain->pViewCanvas->GetVRAMBuf(),ScreenWidth*ScreenHeight*2);
    	SetNextProc(ENP_SetupW,EPFE_CrossFade);
    	break;
    case 5: 
    	isOnlyNDS = true;
    	SetupWisComefromMenu = false;
    	break;
    case 0: 
    default:
    	SetupWisComefromMenu = false;
        break;
    }
    pScreenMainOverlay->pCanvas->SetColor(0);
    pScreenMainOverlay->pCanvas->FillBox(0,0,256,192); 
    StandbyClock_Free();
    StandbyClockAlpha_Free();

}

static __attribute__ ((noinline)) void mainloop(void)
{
    _consolePrint("mainloop.\n");
  
    mainloop_ins_start();
    
    DTCM_StackCheck(0);

restartMenu :   
    EnterStartMenu();
    _consolePrintf("Start event loop...\n");
    while(1)
    {

    	mainloop_ins_loopstart();
    	 _consolePrintf("\n nnn12");
        while(NextProc==ENP_Loop)
        {
     	
            if(DLLSound_isOpened()==true)
            {
                while(DLLSound_Update()==true){};
                if(strpcmRequestStop==true)
                {
                    if(CallBack.strpcmRequestStop!=NULL) CallBack.strpcmRequestStop();
                }
            }
        	
            WaitForVBlank();
            _consolePrintf("\n nnn13");
            REG_IME=0;
            u32 vsynccount=VBlankPassedCount;
            VBlankPassedCount=0;
            REG_IME=1;
    
            if(CallBack.VsyncUpdate!=NULL) CallBack.VsyncUpdate(vsynccount);
	            
            Proc_TouchPad(vsynccount);
            Proc_KeyInput(vsynccount);
            Proc_PanelOpened();
            Proc_Trigger(false,vsynccount);
            Proc_ExternalPowerPresent();
            if(chkstack==true)
            { // fast stack overflow checker
                DTCM_StackCheck(-1);
            }
            else
            { 
                MTCM_StackCheck(-1);
            }
      
        }
    
        mainloop_ins_loopend();
    
        if(BootROM_GetExecuteFlag()==true) break;
        if(NextProc==ENP_ReMenu) break;
    }
    if(NextProc==ENP_ReMenu)
    {
        SetNextProc(ENP_FileList,EPFE_CrossFade);
        goto restartMenu ;
    }
    mainloop_ins_end();
  
    ShowLogHalt();
}
