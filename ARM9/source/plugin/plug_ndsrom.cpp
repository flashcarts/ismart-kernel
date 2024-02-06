
#pragma Ospace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>


#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "lang.h"

#include "procstate.h"
#include "launchstate.h"
#include "datetime.h"

#include "glib/glib.h"
#include "shell.h"
#include "component.h"
#include "lang.h"
#include "mediatype.h"
#include "gba_nds_fat.h"
#include "disc_io.h"

#include "../../../ipc6.h"

#include "../_console.h"

#include "../_const.h"

#include "fat2.h"
#include "io_dldi.h"

#include "libs/memtool.h"
#include "libs/shell.h"

#include "plug_ndsrom.h"
#include "strtool.h"
#include "../BootROM.h"
#include "../mycartsrc/io_MartSD.h"
#include "../mycartsrc/MartCartop.h"
#include "../gba_nds_fat/gba_nds_fat.h"
#include "../libs/ErrorDialog.h"
#include "proc_romset/proc_romset_bin.h"
#include "skin.h"
#include "../libnds/nds/card.h"
#include "../MyCartsrc/sdsavelib.h"

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
extern void PatchNDSGameITCM(uint32 game);
}




extern __declspec(noreturn) void _ShowLogHalt(void);
extern "C" void ReadArm9To2000000();
extern "C" uint32  A9_ReadSram(uint32 psramAdd) ;
extern void videoSub_SetShowLog(bool e);
void Check_RTS(char *RTSName);

u32 SRAM_BaseAddress = 0;
u32 DLDI_section_start_address = 0;
extern u32 DLDI_patchOffset = 0;
extern u32 EFS_patchOffset = 0;

static void _ShowLogHalt(void)
{
  _consoleLogResume();
  IPC6->LCDPowerControl=LCDPC_ON_BOTH;
  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE);
  _consoleSetLogOutFlag(true);
  //_consolePrint("\n Application halted!!\n Please refer [/moonshl2/logbuf.txt]\n\n");
  while(1){
    swiWaitForVBlank();
  }
}

#define _VRAM_OFFSET(n) ((n)<<3)
#define _VRAM_PTR ((u16*)0x06000000)
#define _VRAM_CD_MAIN_BG_0x6000000 (1 | _VRAM_OFFSET(0))
#define _VRAM_CD_MAIN_BG_0x6020000 (1 | _VRAM_OFFSET(1))
#define _VRAM_CD_ARM7_0x6000000 (2 | _VRAM_OFFSET(0))
#define _VRAM_CD_ARM7_0x6020000 (2 | _VRAM_OFFSET(1))

#include "plug_ndsrom_resetmem.h"
#include "plug_ndsrom_dldipatch.h"

///////////////////////////////////////////////////////////////////////////////
//****************************************************************************


void BootRomProgress(int iCnt,int flg)
{
    CglCanvas *pTmpBM=new CglCanvas(NULL,ScreenWidth,ScreenHeight,pf15bit);
	pScreenSub->pCanvas->BitBltFullBeta(pTmpBM);
	pTmpBM->SetCglFont(pCglFontDefault);
	pTmpBM->SetFontTextColor(ColorTable.ZV_SYSTEM.SYS_Color1);//设置字体颜色黑色  	
	
	CglTGF * pbm2= BootRomAlpha_GetSkin(EBRSA_BootRomBG);
	pbm2->BitBlt(pTmpBM,16,66);
	
	switch(flg)
	{
		case 1:
		{
			pTmpBM->TextOutUTF8(24,66+22+15+2,Lang_GetUTF8("BOOT_LoadingNDS")); 		
		}break;
		case 2:
		{
			pTmpBM->TextOutUTF8(24,66+22+15+2,Lang_GetUTF8("BOOT_processing_save")); 		
		}break;		
		case 3:
		{
			pTmpBM->TextOutUTF8(24,66+22+15+2,Lang_GetUTF8("BOOT_processing_load")); 		
		}break;
		default:
			break;
	}
	
	pbm2= BootRomAlpha_GetSkin(EBRSA_progress_bar);		
	int iCntTmp=0;
	while(iCntTmp < iCnt+1)
	{
		pbm2->BitBlt(pTmpBM,20+iCntTmp,66+22);
		iCntTmp++;
	}	
	VRAMWriteCache_Enable();
	pTmpBM->BitBltFullBeta(pScreenSub->pCanvas);
	VRAMWriteCache_Disable();
	if(pTmpBM!=NULL)
	{
	    delete pTmpBM; pTmpBM=NULL;
	} 
}
static void CheckNDSfile(const char *pFilename)
{
    FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFilename);  
    u32 FileSize=FAT2_GetFileSize(FileHandle);
    FAT2_fclose(FileHandle);
    _consolePrintf(" NDS FileSize = %x\n",FileSize);
    uint32 clusterSize =  GetSecPerCls() ;
    _consolePrintf(" cluster Size = %x\n",clusterSize);
    switch(clusterSize)
    {
    case 4:if(FileSize <= 0x10000000) break;	//8K 16SecPerClus
    case 3:if(FileSize <= 0x8000000) break;		//4K 8SecPerClus
    case 2: //2K 4SecPerClus
    	_consolePrint("Can not open this NDS file.\n");
    	_consolePrint("Need reformat your TF card use 16Kbyte/cluster or larger.\n");
    	ShowLogHalt();
    	break;
    default:break;
    
    }
}
//homebrew booting
CODE_IN_ITCM void HomeBrewRebootITCM(bool bMidstage)
{
    REG_IME = IME_DISABLE;  // Disable interrupts
    REG_IE = IME_DISABLE ;
    REG_IF = REG_IF;    // Acknowledge interrupt
  
    IPC6->RESET_BootAddress=0;
    IPC6->RESET=RESET_Commercial;

#define _REG_WAIT_CR (*(vuint16*)0x04000204)
   _REG_WAIT_CR|=1 << 7; //0x80 arm7 gbaslot权限
   
	Mart_DisableSpiWrite();//must use this function,if not this is dangerous for card!
   u32 DLDI_section_start_address_ITCM = DLDI_section_start_address;
   ReadArm9To2000000();
 	
   u8 *pFileBuf=(u8*)(0x02350000);
   MemCopy8CPU(pFileBuf,(void *)(*(vuint32*)0x027ffe24+DLDI_section_start_address_ITCM),0x30000);
   MemSet32CPU(0,pFileBuf,0x30000);
   ITCM_DC_FlushAll();

   _REG_WAIT_CR|=1 << 7; 
   //_consolePrintf(" end\n");
   //while(*(vuint16*)0x04000130 == 0x3FF);
   //while(*(vuint16*)0x04000130 != 0x3FF); 
   {
        resetMemory1_ARM9();
          
        VRAM_C_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6000000;
        VRAM_D_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6020000;
           
        void (*lp_resetMemory2_ARM9) (vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory)=(void(*)(vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory))(0x027f0000);
       
        u32 *psrc=(u32*)ITCM_resetMemory2_ARM9;
        u32 *pdst=(u32*)lp_resetMemory2_ARM9;
    
        for(int i=0;i<4*1024;i++)
        {
            *(pdst+i) = *(psrc+i) ;
        }
        ITCM_DC_FlushAll();
        *(vuint32*)0x27ffe34 = 0x2380000 ;
        lp_resetMemory2_ARM9(&IPC6->RESET_BootAddress,1,bMidstage);
    }
    while(1); // 钙び叫し傅には耽らない。
}
static void RebootNDSHomeBrew(const char *pFilename)
{		
    u8 *pDLDIBuf=NULL;
    s32 DLDISize=0;
  
    const char fn[]="Mat.dldi";


    if(Shell_FAT_ReadAlloc(fn,(void**)&pDLDIBuf,&DLDISize)==false){
        _consolePrintf("Can not load DLDI patch file. (%s)\n",fn);
        pDLDIBuf=NULL;
        DLDISize=0;
    }
    _consolePrintf("found .dldi file\n");
    //FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFilename);
    FAT_FILE *FileHandle=FAT_fopen(pFilename,"rb+");
  
    if(FileHandle==NULL){
        _consolePrint("Can not open HomeBrew file.\n");
        _ShowLogHalt();
    }
    u32 FileSize=FAT2_GetFileSize(FileHandle);
    /*if(FileSize <0x360000) 
    {//非EFS
    	for(int kk=0;kk<0x20;kk++)
    		*((u32*)0x027FFF70+kk) = 0;
    }*/
    
    u8 *pFileBuf=(u8*)(0x02350000);
    
    for(DLDI_section_start_address=0;DLDI_section_start_address<0x120000;DLDI_section_start_address+=0x30000)
    {	
    	FAT2_fseek(FileHandle,(*(vuint32*)0x27FFE20)+DLDI_section_start_address,SEEK_SET);
    	FAT2_fread(pFileBuf,4,0x30000>>2,FileHandle);
        if((pDLDIBuf!=NULL)&&(DLDISize!=0))
        {
            if(DLDIPatch(pFileBuf,0x30000,pDLDIBuf,DLDISize)==true)
            {
                _consolePrintf(" DLDI patch complete.\n");
                _consolePrintf("success ! DLDI_section_start_address = %x\n",DLDI_section_start_address);
                break;
            }
            
        }   
        if(FileSize <= DLDI_section_start_address) break ; 
    }
    _consolePrintf("DLDI_patchOffset = %x\n",DLDI_patchOffset);
    _consolePrintf("EFS_patchOffset = %x\n",EFS_patchOffset);
    if((DLDI_patchOffset>0x30000 - 0x3000) && DLDI_patchOffset<0x30000)
    {
    	_consolePrintf("use dldi patch2\n");
    	FAT2_fseek(FileHandle,(*(vuint32*)0x27FFE20)+0x3000+DLDI_section_start_address,SEEK_SET);
    	FAT2_fread(pFileBuf,4,0x30000>>2,FileHandle);
        if((pDLDIBuf!=NULL)&&(DLDISize!=0))
        {
            if(DLDIPatch(pFileBuf,0x30000,pDLDIBuf,DLDISize)==true)
            {
            	DLDI_section_start_address += 0x3000;
                _consolePrintf("success2 ! DLDI_section_start_address = %x\n",DLDI_section_start_address);
            }
            
        }  
    }
    if(EFS_patchOffset > 0 && FileSize > EFS_patchOffset)
    {//EFS
    	EFS_patchOffset += DLDI_section_start_address + (*(vuint32*)0x27FFE20);
    	_consolePrintf("EFS_patchOffset file = %x\n",EFS_patchOffset);
    	char path[200]={0};
    	char *fat="fat:";
    	strcat(path,fat);
    	strcat(path,pFilename);
    	_consolePrintf("path = %s\n",path);
    	FAT2_fseek(FileHandle,EFS_patchOffset+0x14,SEEK_SET);
    	FAT2_fwrite(path,1,200,FileHandle);
    }
    MemSet32CPU(0,pDLDIBuf,DLDISize&~3);
    _consolePrint("read arm9.bin FAT_fclose\n");
    FAT2_fclose(FileHandle);  
    _consolePrint("FAT_FreeFiles\n");
    FAT2_FreeFiles();

    HomeBrewRebootITCM(false);
  
    _consolePrintf("Fatal error: Unknown error.\n");
    _ShowLogHalt();
    
}

#include "..\mycartsrc\MartCartop.h"
////////////////////////////////////////////////////////
// argv struct magic number
#define ARGV_MAGIC 0x5f617267
//!	argv structure
/*!	\struct __argv	 	
		structure used to set up argc/argv on the DS 
 */
struct ___argv{
	int argvMagic_;		//!< argv magic number, set to 0x5f617267 ('_arg') if valid
  	char *commandLine;	//!< base address of command line, set of null terminated strings
  	int length;			//!< total length of command line
  	int argc;			//!< internal use, number of arguments
  	char *argv[];		//!< internal use, argv pointer
  	};
  //!	Default location for the libnds argv structure.
#define __system_argv		((struct ___argv *)0x027FFF70)

////////////////////////////////////////////////////////

static void BackupDLDIBody(const char *pFilename)
{
/* 
  const u32 *pdldibody=&dldibodytop;
  u32 dldibodysize=0;
  
  for(u32 idx=0;idx<dldibodymaxsize/4;idx++){
    if(pdldibody[idx]!=0) dldibodysize=(idx+1)*4;
  }
  
  if(dldibodysize==0) return;
  
  {
    u8 *p=(u8*)pdldibody;
    char ID[5];
    ID[0]=p[0x60+0];
    ID[1]=p[0x60+1];
    ID[2]=p[0x60+2];
    ID[3]=p[0x60+3];
    ID[4]=0;
    const char *pname=(const char*)&p[0x10];
    _consolePrintf("Backup implant DLDI body.\n");
    _consolePrintf("DLDI ID:%s\n",ID);
    _consolePrintf("DLDI Name:%s\n",pname);
    _consolePrintf("DLDI Size:%dbyte\n",dldibodysize);
  }
  
  const char fn[]="DLDIBODY.BIN";
  FAT_FILE *pf=Shell_FAT_fopenwrite_Data(fn);
  
  if(pf==NULL){
    _consolePrintf("Fatal error: Can not open DLDI patch file. (%s)\n",fn);
    ShowLogHalt();
  }
  
  if(FAT2_fwrite(pdldibody,1,dldibodysize,pf)!=dldibodysize){
    _consolePrintf("Fatal error: DLDI path backup write error. (%s)\n",fn);
    ShowLogHalt();
  }
    
  FAT2_fclose(pf);
*/
    Mart_EnableSpiWrite();//enable it for flash write
	//设置这个,让dldi的SD在初始化的时候,能够取
    dsCardi_WriteSram(0x7dfE0 , IsSDHC()); //  高N位有效来索引
    dsCardi_WriteSram(0x7dfE2 , 0); //  高N位有效来索引

	uint8 copy_arm7_code[] __attribute__ ((aligned (4))) = {
	0x2c,0x00,0x8f,0xe2,0x0e,0x00,0xb0,0xe8,0x01,0x40,0xa0,0xe1,0x03,0x30,0x81,0xe0,
	0x04,0x00,0x92,0xe4,0x04,0x00,0x81,0xe4,0x03,0x00,0x51,0xe1,0xfb,0xff,0xff,0x9a,
	0x00,0x00,0xa0,0xe3,0x00,0x10,0xa0,0xe3,0x00,0x20,0xa0,0xe3,0x00,0x30,0xa0,0xe3,
	0x14,0xff,0x2f,0xe1,0x00,0x80,0x7f,0x03,0x40,0x00,0x38,0x02,0xf0,0x1f,0x00,0x00
	};
    //检测文件是否存在
        //uint32 *pkStart = 0,*pkEnd = 0 ;
        FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFilename);
        
        if(FileHandle==NULL){
          _consolePrint("Can not open HomeBrew file.\n");
          _ShowLogHalt();
        }
        CheckNDSfile(pFilename);
        u32 FileSize=FAT2_GetFileSize(FileHandle);
        MemClearAllFreeBlocks();
        uint32 *p2x = (uint32*)0x27FFE00 ;
        FAT2_fread(p2x,1,0x160,FileHandle); 
        
        p2x = (uint32*)0x2300000 ;      
        for(int i = 0 ; i< 0xf0000/4 ;i++)
        {
            *(p2x+i) = 0 ;
        }
        p2x = (uint32*)0x2380040 ; //
        FAT2_fseek(FileHandle,*(vuint32*)0x27FFE30,SEEK_SET);
        FAT2_fread(p2x,4,(*(vuint32*)0x27FFE3C)>>2,FileHandle);
        
        *(uint32*)&copy_arm7_code[0x34] = *(vuint32*)0x27FFE34;
        *(uint32*)&copy_arm7_code[0x3C] = *(vuint32*)0x27FFE3C;
		for(int i=0;i<0x40;i++)
		{
	  		*(vuint8*)(0x2380000 + i) = copy_arm7_code[i];
		} 
		_consolePrint("copy arm7 code done.\n");
		{
			/*_consolePrintf("Backup arm7 code. \n");
			uint16 *pR = (uint16*)0x2380000;
			u32 size = *(vuint32*)0x27FFE3C + 0x40;      
		 	for(int i=0;i<size;i+=4)
		 	{
		    	dsCardi_WriteSram(0x0+i,*(pR + i/2));
		     	dsCardi_WriteSram(0x0+i+2,*(pR + i/2 +1));
		 	}*/
			//arm7 dldi
		    u8 *pDLDIBuf=NULL;
		    s32 DLDISize=0;		    
		    const char fn[]="Mat.dldi";		  

		    if(Shell_FAT_ReadAlloc(fn,(void**)&pDLDIBuf,&DLDISize)==false){
		        _consolePrintf("Can not load DLDI patch file. (%s)\n",fn);
		        pDLDIBuf=NULL;
		        DLDISize=0;
		    }
		    _consolePrintf("found .dldi for arm7\n");
		    if((pDLDIBuf!=NULL)&&(DLDISize!=0))
		    {
		        if(DLDIPatch((u8*)p2x,*(vuint32*)0x27FFE3C,pDLDIBuf,DLDISize)==false){
		            _consolePrintf("Fatal error: DLDI patch error or not found DLDI chank.\n");
		        }
		        MemSet32CPU(0,pDLDIBuf,DLDISize&~3);
		    }
			
		}
	    u32 zero;
	    asm {
	        mov zero,#0
	        MCR p15, 0, zero, c7, c10, 4 // drain write buffer
	    }
	    DC_FlushAll(); 	
	    
	    {
	    void WriteFat2Flash(FAT_FILE * filePK , u32 startadd , uint32 usize,bool  needsomeloop );
	    SetFattableBase(0x1c); //0x1c0000    
	    WriteFat2Flash(FileHandle,0x1c0000,0x20000,0) ;
	    }
	    
	    uint32 secp =  GetSecPerCls() ;
	    dsCardi_SectorPerClus(secp);
	    Mart_SetCardUseSDHC(IsSDHC());

	    _consolePrint("FAT_fclose\n");
	    FAT2_fclose(FileHandle);   
}

///////////////////////////////////////////////////////////////////////////////
//****************************************************************************
#include "mycartsrc/CBMPResource.h"
#include "lang.h"
/////////////////////////////////////////////////////////////
#include "patch_special_game.h"
/////////////////////////////////////////////////////////////
#define FAT_at_SRAM  0x80
#include "EnableCheatData.h"
void resettest(vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory);

#include "ProcState.h"
#include "SkinDirListFile.h"
bool ProcessARCode(uint32 *pARDS , uint32 size , uint32 *pOUT ,uint32 &outsize);

extern bool gl_homebrew ;
void videoSub_SetShowLog(bool e);
void RebootCommercialNDSROM(const char *pFilename);
void BootNDSROM(void)
{
  const char *pFilename=BootROM_GetFullPathAlias();
  
	REG_IME = 0;
	REG_IE = 0;
	REG_IF = ~0;
	
	DMA0_CR=0;
	DMA1_CR=0;
	DMA2_CR=0;
	DMA3_CR=0;
    memset((uint32*)0x27FFc30,0,0x8);
	if(gl_homebrew == true)
	{
	    BackupDLDIBody(pFilename);
      
	    RebootNDSHomeBrew(pFilename);
	}
	else
	{  
	    _consolePrintf("Booting  commercial Rom.\n");
	    RebootCommercialNDSROM(pFilename);
	    while(1);
	}
	_consolePrintf("Fatal error: Unknown error.\n");
	_ShowLogHalt();
}
//######################################################
//ITCM的启动代码
CODE_IN_ITCM void rebootITCM(bool bMidstage)
{
    REG_IME = IME_DISABLE;  // Disable interrupts
    REG_IE = IME_DISABLE ;
    REG_IF = REG_IF;    // Acknowledge interrupt
  
    //arm7 reset
    IPC6->RESET_BootAddress=0;
    IPC6->RESET=RESET_Commercial;

#define _REG_WAIT_CR (*(vuint16*)0x04000204)
   _REG_WAIT_CR|=1 << 7; //0x80 arm7 gbaslot权限

   /*//here check some value
   _consolePrintf("%x = %x\n",0x01FF7800,*(uint32*)0x01FF7800 ) ;
   _consolePrintf("%x = %x\n",0x01FF7804,*(uint32*)0x01FF7804 ) ;
   _consolePrintf("%x = %x\n",0x01FF7808,*(uint32*)0x01FF7808 ) ;
   _consolePrintf("%x = %x\n",0x01FF780c,*(uint32*)0x01FF780c ) ;
   _consolePrintf("0x7DFC0 = %x\n",dsCardi_ReadSram(0x7DFC0) ) ;//soft reset
   _consolePrintf("0x7DFC4 = %x\n",dsCardi_ReadSram(0x7DFC4) ) ;//cheat
   while(*(vuint16*)0x04000130 == 0x3ff);
   while(*(vuint16*)0x04000130 != 0x3ff);*/
   
	Mart_DisableSpiWrite();//must use this function,if not this is dangerous for card!
    if(RomSetState.SpeciaMode)
    {      
    	if(!RomSetState.DownloadPlayState)
    	{
    		*(vuint32*)0x27ffe60 = 0x586000 + (RomSetState.Speed*0x400) ;
    	}
    	ReadArm9To2000000();
    }
    else
    {
        switch(dsCardi_ReadSram(0x7DFF0))
    	{	        
        case 0x1E://4273_-_Mario_And_Sonic_At_The_Olympic_Winter
        case 0x1F://3966
        case 0x20://2906
        case 0x21://2971
        	{
        		RomSetState.DownloadPlayState = true;        	
        		break;
        	}
        default:
    		break;
    	}
    	if(!RomSetState.DownloadPlayState)
    	{
            *(vuint32*)0x27ffe60 = 0x586000 ;	
    	}
        ReadArm9To2000000();
        PatchNDSGameITCM(0);
    }
	extern bool patch_special_games(void);
	patch_special_games();
	{
        resetMemory1_ARM9();
          
        VRAM_C_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6000000;
        VRAM_D_CR = VRAM_ENABLE | _VRAM_CD_ARM7_0x6020000;
           
        void (*lp_resetMemory2_ARM9) (vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory)=(void(*)(vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory))(0x027f0000);
       
        u32 *psrc=(u32*)ITCM_resetMemory2_ARM9;
        u32 *pdst=(u32*)lp_resetMemory2_ARM9;
    
        for(int i=0;i<4*1024;i++)
        {
            *(pdst+i) = *(psrc+i) ;
        }             
        SendError(0x1234);       
        lp_resetMemory2_ARM9(&IPC6->RESET_BootAddress,1,bMidstage);
    }
    while(1); // 钙び叫し傅には耽らない。
}
//
static bool CheckSaverSynPatch()
{
    uint32 readcache[8] ;
    uint32 returnsize ;
    const char *pfullalias;
    const char *pbuf = (char*)0x27FFE00 ;
    bool  hasfound = false;
    dsCardi_WriteSram(0x7DFB8 , 0);
    dsCardi_WriteSram(0x7DFBA , 0);
    pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/miscneed.bin");
    FAT_FILE *pfile = FAT2_fopen_AliasForRead(pfullalias);
    if(pfile)
    {//32字节一块，有类型，和数据处理
        do
        {
            returnsize = FAT2_fread(readcache,1,0x20,pfile);
            int cmpsize =  ((u8*)readcache)[1] ;
            if(*(u8*)readcache == 0x51) // 类型1 ，表示需要直存同步的类型
            {
                if(!memcmp((char*)&readcache[4],pbuf,cmpsize))
                {
                    hasfound = true;
                    break;
                }
            }
            if(*(u8*)readcache == 0x52) //类型2，表示软复位地址需要特殊指定
            {
                if(!memcmp((char*)&readcache[4],pbuf,cmpsize))
                {
                    //while(*(vuint16*)0x04000130 == 0x3ff);
                    //while(*(vuint16*)0x04000130 != 0x3ff); 
                    dsCardi_WriteSram(0x7DFB0 , 0x5259);
                    dsCardi_WriteSram(0x7DFB2 , 0xF5A0);
                    //写入地址
                    dsCardi_WriteSram(0x7DFB4 , readcache[1]&0xFFFF);
                    dsCardi_WriteSram(0x7DFB6 , (readcache[1]>>16)&0xFFFF);
                    
                    //代替头部 swi B
                    dsCardi_WriteSram(0x7DFB8 , readcache[2]&0xFFFF);
                    dsCardi_WriteSram(0x7DFBA , (readcache[2]>>16)&0xFFFF);
                    dsCardi_WriteSram(0x7DFBC , readcache[3]&0xFFFF);
                    dsCardi_WriteSram(0x7DFBE , (readcache[3]>>16)&0xFFFF);
                }
            }
            if(*(u8*)readcache == 0x53) //类型2，表示软复位地址需要特殊指定
            {//如果需要指定特殊游戏类型，此类型最好放前面
                if(!memcmp((char*)&readcache[4],pbuf,cmpsize))
                {
                    dsCardi_WriteSram(0x7DFB0 , 0x55AA);
                    dsCardi_WriteSram(0x7DFB2 , 0xF5A0);
                    //写入地址
                    dsCardi_WriteSram(0x7DFB4 , 1);
                    dsCardi_WriteSram(0x7DFB6 , 0);
                }
            }
        }while(returnsize);
    }
    return hasfound ;
}
extern "C" uint32  Read_FPGA_IDLO(void) ;
//-------------------------------------------------------------
void RebootCommercialNDSROM(const char *pFilename)
{
    //videoSub_SetShowLog(true); //use console for debug

    BootRomProgress(30,0);//progress bar
    Mart_EnableSpiWrite();//enable it for flash write

    uint32 *pkStart = 0,*pkEnd = 0 ;
    FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFilename);  
    if(FileHandle==NULL){
      _consolePrint("Can not open NDS file.\n");
      _ShowLogHalt();
    }
    CheckNDSfile(pFilename);
    MemClearAllFreeBlocks();  
    //read Cartridge Header to 0x27FFE00，大小0x160 size
    uint32 *p2x = (uint32*)0x27FFE00 ;
    FAT2_fread(p2x,1,0x160,FileHandle);
    //check special
    char*filename = (char*)pFilename;
    DecryptSecureArea(filename); 
    GetGameSpecial((char*)0x27FFE00,pFilename);
    p2x = (uint32*)0x2300000 ;  
    for(int i = 0 ; i< 0xf0000/4 ;i++)
    {
        *(p2x+i) = 0 ;
    }
    //load arm7 code,and patch 
    p2x = (uint32*)0x2380000 ;
    FAT2_fseek(FileHandle,*(s32*)0x27FFE30,SEEK_SET);
    FAT2_fread(p2x,4,(*(vuint32*)0x27FFE3C)>>2,FileHandle);

    //这里之所以用C地址，是因为在后面需要放置一个补丁代码，Bl 0x23XXXXXX
    // bootGame.s中
    pSave2_23XX = (uint32*)0x0230000C ;
    memset(pSave2_23XX,0,0x2000);
    GetSpecialSave((char*)0x27FFE00); //Special save
    if(RomSetState.RealtimeSave | RomSetState.GameGuide) addsize = 0x50;
    FindAndPatch_DirectSave((void*)0x2380000,*(uint32*)0x27FFE3C,pkStart ,pkEnd );
    
    /*_consolePrintf(" pkStart = %x\n",pkStart);
    _consolePrintf(" pkEnd = %x\n",pkEnd); 
    while(*(vuint16*)0x04000130 == 0x3ff);
    while(*(vuint16*)0x04000130 != 0x3ff);*/ 
    //address for arm7 
    dsCardi_WriteSram(0x7DFCC, ((u32)pSave2_23XX)&0xFFFF);  
    dsCardi_WriteSram(0x7DFCE, (((u32)pSave2_23XX)>>16)&0xFFFF);         
    
    //address for arm9
    pSave2_23XX = (uint32*)0x02300600 ;
    dsCardi_WriteSram(0x7dfdc , ((u32)pSave2_23XX)&0xFFFF);  
    dsCardi_WriteSram(0x7dfde , (((u32)pSave2_23XX)>>16)&0xFFFF);         
    
    {
        bool k = CheckSaverSynPatch();
        if(k)
        {//true,need patch       
            dsCardi_WriteSram(0x7DFD4 , 0xFFFF);  
            dsCardi_WriteSram(0x7DFD6 , 0xFFFF);                 
        }
        else
        {
            dsCardi_WriteSram(0x7DFD4 , 0);  
            dsCardi_WriteSram(0x7DFD6 , 0);                 
        }
        //save some setting 
        uint32 tmp =RomSetState.SoftReset;// ProcState.SetupW.SoftReset ;
        _consolePrintf(" SoftReset = %x\n",tmp);
        dsCardi_WriteSram(0x7DFC0 , tmp&0xFFFF);  
        dsCardi_WriteSram(0x7DFC2 , 0);         
        tmp = RomSetState.CheatState;//ProcState.SetupW.CheatOnOff ;
        _consolePrintf(" CheatState = %x\n",tmp);
        dsCardi_WriteSram(0x7DFC4 , tmp&0xFFFF);  
        dsCardi_WriteSram(0x7DFC6 , 0);         
        
    }

    //fat table start address
    SetFattableBase(0x1c); //0x1c0000 start  
    //map to flash
    WriteFat2Flash(FileHandle,0x1c0000,0x20000,1) ;
    
    uint32 secp =  GetSecPerCls() ;
    dsCardi_SectorPerClus(secp);
    Mart_SetCardUseSDHC(IsSDHC());
    
    //save first cluster
    dsCardi_WriteSram(0x7F4,Mart_ReadFlashCommand(0x1c0000)&0xFFFF);
    dsCardi_WriteSram(0x7F6,Mart_ReadFlashCommand(0x1c0000)>>16);      
    ///////////////////////////////////////////////////
    {
        uint8 pbufa4[0x200];
        Mart_SetHeaderCleanSRAMRead(0x18fE00,0x1000); 
        //Mart_SetHeaderCleanSRAMRead(0,0);       
        FAT2_fseek(FileHandle,0x8000,SEEK_SET);
        FAT2_fread(pbufa4,1,0x200,FileHandle);
        Mart_Arm9Access();
    	cardEepromBlockErase(0x18f000);
    	cardWriteEeprom(0x18fE00, pbufa4, 0x200, 3);
    }      
    _consolePrint("FAT_fclose\n");
    FAT2_fclose(FileHandle);
    
    ///////////////////////////////////////////////////////////
    //read intermediate  file mscdat.dat
    const char* pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/mscdat.dat");
    bool  buseMidStage = 0 ;
    FileHandle = FAT2_fopen_AliasForRead(pfullalias);
    if(FileHandle != NULL)
    {
        _consolePrintf("find mscdat.BIN \n");
        FAT_fread((void*)0x2330800 ,4,0x3000,FileHandle);       
        FAT_fclose(FileHandle);
        dsCardi_WriteSram(0x7DFD0,0xFFFF);
        dsCardi_WriteSram(0x7DFD2,0xFFFF);
        buseMidStage = 1 ;
    }
    else
    {
        _consolePrintf("can't find mscdat.BIN \n");
        dsCardi_WriteSram(0x7DFD0,0);
        dsCardi_WriteSram(0x7DFD2,0);
        buseMidStage = 0 ;
    }

    //load softreset file to 
    pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/mingk.dat");    
    FileHandle = FAT2_fopen_AliasForRead(pfullalias);
    if(FileHandle != NULL)
    {
        _consolePrintf("find mingk.dat \n");
        uint8 *pR = (uint8*)0x2350000;
        FAT_fseek(FileHandle,0,0);
        int FileSize = FAT2_GetFileSize(FileHandle);
        if(FileSize>0x1000) FileSize = 0x1000;
        FAT_fread(pR,4,(FileSize+3)>>2,FileHandle);       
        FAT_fclose(FileHandle);
        uint8 *pR2 = (uint8*)0x2351000;
        for(int size=0;size<0x800;size+=0x200)
        {
        	Mart_ReadFlash(0x17F000+size,pR2+size);
        }
        if(memcmp(pR,pR2,FileSize))
        {
        	//_consolePrintf("FileSize = %x\n",FileSize);
        	Mart_Arm9Access();
        	//cardEepromBlockErase(0x18f000);//no need erase now
        	cardWriteEeprom(0x18f000, pR, FileSize, 3);
        }
    }
    else
    {
        _consolePrintf("can't find mingk.dat \n");
    }    
    
    //debug file
    FileHandle = FAT_fopen("/DEBUG.BIN","r+");
    if(FileHandle == NULL)
    {
        dsCardi_WriteSram(0x7dfE8 , 0);  
        dsCardi_WriteSram(0x7dfEA , 0);         
        dsCardi_WriteSram(0x7dfEC , 0);  
        dsCardi_WriteSram(0x7dfEE , 0);    
        
        dsCardi_WriteSram(0x7DFFC , 0);  
        dsCardi_WriteSram(0x7DFFE , 0);         
	}
    else
    {      
        _consolePrintf("find debug.bin \n");
        WriteFat2_FPGA_SRAM(FileHandle,0x7DFE8,0);
        //debug info address， 231XXXX
        dsCardi_WriteSram(0x7DFFC , 0);  
        dsCardi_WriteSram(0x7DFFE , 0x0231); 

    }
    FAT_fclose(FileHandle);
    
    //check sav file
    testSaver((char*)pFilename); 
    //check game guide file
    Check_RTS((char*)pFilename);
    
    BootRomProgress(200,1); //progress bar   
    
    _consolePrint("FAT_FreeFiles\n");
    FAT2_FreeFiles();
    
    //if(RomSetState.CheatState)
    {
	    _consolePrintf("%x cheats \n",EnableNum);
		uint32 *pOUT = (uint32*)0x2370000;
		//uint32  outsize = 0 ;
		uint32 *pSize = (uint32*)(0x2370000-4);
		int s = 0;
		*pSize = 0;
	    if(EnableNum)
	    {
	        do
	        {            
	            for(int tt=0;tt<EnableCheatData[s].datasize;tt++)
	            {
	            	*pOUT++ = EnableCheatData[s].pData[tt];
	            	*pSize += 4 ;
	            }
	            s++;
	        }while(s<EnableNum);
	    } 
		*pOUT = 0xCF000000;  //end flag
		*pSize += 4 ;
		/*pOUT = (uint32*)0x2370000;
		for(int ii=0;ii<*pSize>>2;ii++)
		{
			_consolePrintf("  %08x\n",pOUT[ii]);
			//while(*(vuint16*)0x04000130 == 0x3ff);
		    //while(*(vuint16*)0x04000130 != 0x3ff);
		}
		_consolePrintf("total size %x\n",*pSize);
		//while(*(vuint16*)0x04000130 == 0x3ff);
	    //while(*(vuint16*)0x04000130 != 0x3ff);
	    */
    }  
    extern void disc_SystemCache_ClearAll(void);
    disc_SystemCache_ClearAll();
    
#define _REG_WAIT_CR (*(vuint16*)0x04000204)
    _REG_WAIT_CR = 0xe000;

    u32 zero;
    asm {
        mov zero,#0
        MCR p15, 0, zero, c7, c10, 4 // drain write buffer
    }
    DC_FlushAll();  
    
    rebootITCM(buseMidStage);
}
//######################################################
//######################################################
extern "C" void InstanceEntry(void) ;
extern "C" void ARM7_RTS_start(void) ;
void Check_RTS(char *RTSName)
{
	//char *RTSName_temp = RTSName;
	//TProcState *CurProcState = &ProcState;
	
    uint32 is_RTS =(RomSetState.RealtimeSave | RomSetState.GameGuide) && RomSetState.SoftReset;    
	if(is_RTS)
	{
		uint8 *pTemp = (uint8*)InstanceEntry ;
		/*int wr;
		for(int i = 0 ;i<0x1000;i++)
		{
			wr = *(pTemp + i) ;
			dsCardi_WriteSram(0x65000+i*4 , (wr&0xFFFF));
			dsCardi_WriteSram(0x65000+i*4+2 , ((wr>>16)&0xFFFF));	
		}*/
		{
	        uint8 *pR = (uint8*)0x2350000;
	        for(int size=0;size<0x3000;size+=0x200)
	        {
	        	Mart_ReadFlash(0x17C000+size,pR+size);
	        }
	        if(memcmp(pR,pTemp,0x3000))
	        {
	        	Mart_Arm9Access();
	        	cardEepromBlockErase(0x18C000);
	        	cardEepromBlockErase(0x18D000);
	        	cardEepromBlockErase(0x18E000);
	        	cardWriteEeprom(0x18C000,pTemp, 0x3000, 3);
	        }	
		}
		
	    dsCardi_WriteSram(0x7DFA4 , 0);  
	    dsCardi_WriteSram(0x7DFA6 , 0);     
	    if(RomSetState.RealtimeSave)
	    {//check RTS file,create file,map file
		    dsCardi_WriteSram(0x7DFA4 , 0xFF);
		    dsCardi_WriteSram(0x7DFA6 , 0); 
			char*extname = "ITS";
			char*Folder = "/SSTATE";
		    uint32 createSize = 0x500000;
		    FAT_FILE *filePK=CheckFile_in_Folder(RTSName,extname,Folder,createSize);
    		if(filePK==NULL)
    		{
    			_consolePrint("Can not open ITS file.\n");
    			ShowLogHalt(); 
    		}
    		const char* pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/RTS.bin");    
    		FAT_FILE *fileRTS = FAT_fopen(pfullalias,"rb+");
    		if(fileRTS==NULL)
    		{
    			_consolePrint("Can not open RTS.bin file.\n");
    			ShowLogHalt(); 
    			//return ;
    		}
    		else
    		{
    		    memset((uint8*)0x2350000,0,0x1400);
    			WriteFat2_File(filePK,fileRTS,0x0);//(FAT_FILE *filePK ,FAT_FILE * fileRTS ,uint32 startAddress)
    			FAT2_fclose(fileRTS);
    		}
	        filePK ->write = 1 ;
	        FAT_fclose(filePK); 
	        FAT_chdir("/");	
	    }	    
	    
	    //game guide 
    	_consolePrintf("check sst\n");
	    _consolePrintf(RTSName);_consolePrintf("\n"); 
		char*extname = "SST";
		char*Folder = "/SSTATE";
	    uint32 createSize = 0x0;
	    FAT_FILE *filePK=CheckFile_in_Folder(RTSName,extname,Folder,createSize);
		if(filePK==NULL)
		{
			_consolePrint("Can not open SST file.\n");
			ShowLogHalt(); 
		}
		const char* pfullalias=ConvertFullPath_Ansi2Alias(DefaultDataPath"/RTS.bin");    
		FAT_FILE *fileRTS = FAT_fopen(pfullalias,"rb+");
		if(fileRTS==NULL)
		{
			_consolePrint("Can not open RTS.bin file.\n");
			ShowLogHalt(); 
			//return ;
		}
		else
		{
		    memset((uint8*)0x2350000,0,0x10000);
			WriteFat2_File(filePK,fileRTS,0x1000);//(FAT_FILE *filePK ,FAT_FILE * fileRTS ,uint32 startAddress)
			WriteFat2_FPGA_SRAM(fileRTS,0x600,0);
			FAT2_fclose(fileRTS);
		}
        filePK ->write = 1 ;
        FAT_fclose(filePK); 		
 
	    dsCardi_WriteSram(0x7DFC8 , is_RTS&0xFFFF);  
	    dsCardi_WriteSram(0x7DFCA , 0); 	
	}
	else
	{
		_consolePrintf("no ITS\n");
	    dsCardi_WriteSram(0x7DFC8 , 0);  
	    dsCardi_WriteSram(0x7DFCA , 0); 	   	
	}
    FAT_chdir("/");
    //_consolePrintf("  end sst \n");
    //while(*(vuint16*)0x04000130 == 0x3ff);
    //while(*(vuint16*)0x04000130 != 0x3ff); 
}
