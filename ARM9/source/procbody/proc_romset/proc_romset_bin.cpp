
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


extern LPIO_INTERFACE active_interface;
static u32 DataSectorIndex;
static char LastRomSign[5];
static char RomSign[5];
static u8 SaveRomData[0x100];

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}
#include "proc_romset_bin.h"
TRomSetState RomSetState;
TRomSetState RomSetStateTmp;
TRomSetInfo RomSetInfo;
static bool bFlag;
static bool bFlag2;
void SetFlag()
{
	bFlag=true;
}

void ClearFlag()
{
	bFlag=false;
}
void SetFlag2()
{
	bFlag2=true;
}

void ClearFlag2()
{
	bFlag2=false;
}
bool GetFlag2()
{
	return(bFlag2);
}

//检测文件夹的存在与否
static void isRomDatFolderExist()
{
	FAT_chdir("/");
	FILE_TYPE fstype = FAT_FileExists("/ROMDAT"); //大小写无关
    if(fstype == FT_DIR)
    { 
    	_consolePrintf("ROMDAT Folder exist \n");    
    }
    else
    {
        _consolePrintf("ROMDAT Folder NOT exist \n");
            //这里是创建新目录 显示信息
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVE");
        if(FAT_mkdir("/ROMDAT")==0)
        {
        	_consolePrintf("ROMDAT Folder Create success\n");
        }
        else
            _consolePrintf("ROMDAT Folder Create failed\n");
    }
}
extern void TestPauseKey(void);
static void RomDatFileDataSectorIndex()
{
	/*FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(RomSetInfo.FullPathAlias);
    UnicodeChar* pUcode = (UnicodeChar* )FAT2_GetLongFilenameUnicode();
    FAT2_fclose(FileHandle);
    _consolePrintf("\n Alis=%s",RomSetInfo.FullPathAlias);
    int j =0;
    do
    {
        _consolePrintf("%04x ",pUcode[j]);
        j++ ;
    }
    while(pUcode[j]);
        _consolePrintf("\n");
    _consolePrintf("***************\n");
    
    char a_saver[13];
    char *ap = strrchr(RomSetInfo.FullPathAlias,'/');
    if(ap)
        ap++;
 
    UnicodeChar w_saver[256];
    memcpy(a_saver,ap,13);
   
    char* s = strchr(a_saver,'.');
    *(s+1) = 'D' ;
    *(s+2) = 'A' ;
    *(s+3) = 'T' ;
    *(s+4) = 0 ;
    memcpy(w_saver,pUcode,j*2);
    w_saver[j-3]=L'D';
    w_saver[j-2]=L'A';
    w_saver[j-1]=L'T';
    w_saver[j]=0;
     
    FAT_FILE *filePK ;
	const char *pafn; 
	UnicodeChar *pufn;
    FILE_TYPE fstype2 = FT_NONE;
    {   
    	char pp[256];
    	strcpy(pp,ConvertFullPath_Ansi2Alias("/ROMDAT/"));
    	FAT2_chdir_Alias(pp);

    	u32 FAT_FileType=FAT2_FindFirstFile(&pafn);
        _consolePrintf("\n Here2!");
    	while(FAT_FileType!=FT_NONE)       	
    	{
	        pufn=(UnicodeChar* )FAT2_GetLongFilenameUnicode();
	        if(pufn==NULL)
	        {
	          _consolePrintf("FatalError: Can not read unicode filename.\n");
	          ShowLogHalt();
	          return;
	        }
	        if(FAT_FileType == FT_FILE)
	        {
	        	fstype2 =FT_FILE;   
	        	for (int namePos = 0; (namePos < MAX_FILENAME_LENGTH) && (pufn[namePos] != '\0') && (w_saver[namePos] != '\0'); namePos++)
	        	{
	        	  if (( pufn[namePos] > 0x60) && ( pufn[namePos] < 0x7B))
	        		  pufn[namePos] -= 0x20;
	        	  if (( w_saver[namePos] > 0x60) && ( w_saver[namePos] < 0x7B))
	        		  w_saver[namePos] -= 0x20;  
	        	  
	        	  if (w_saver[namePos] != pufn[namePos])
	        	  {
	        		  fstype2 =FT_NONE;
	        		  break;
	        	  }	        	  
	        	}
	        }
	      if(fstype2 == FT_FILE) break;
	      FAT_FileType=FAT2_FindNextFile(&pafn);     
    	}//end while
    	  _consolePrintf("\n Here3!");
    	//FILE_TYPE fstype = FAT_FileExists(a_saver);
        if(fstype2 == FT_NONE)
        {

            //创建存档文件
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVER");
            filePK = FAT2_fopen_CreateForWrite_on_CurrentFolder(a_saver,w_saver);
           	if(filePK->length==0)
            {
           		FAT2_SetSize(filePK,512,0xAB);            
            }           
            FAT2_fclose(filePK);        
            //ClearMessageBox(pScreenMainOverlay->pCanvas);
        }
        else
        {
        	memcpy(a_saver,pafn,13);//用这个别名打开已有存档
        }
    }
    FAT_FILE *pf=FAT2_fopen_AliasForRead(a_saver);*/
	char*extname = "DAT";
	char*Folder = "/ROMDAT";
    uint32 createSize = 512;
    FAT_FILE *pf=CheckFile_in_Folder(RomSetInfo.FullPathAlias,extname,Folder,createSize);
    if(pf!=NULL)
    {
    	if(FAT2_GetFileSize(pf)==512)
        {
            if(pf->firstCluster!=0)
            {//直接打开文件的在TF卡的所在物理扇区
            	DataSectorIndex=FAT2_ClustToSect(pf->firstCluster);
            }
        }
        FAT2_fclose(pf);
    } 
    else
    {
    	FAT2_fclose(pf);	
    }
    
    _consolePrintf("\n Here4!");
}
//-------------------------------------------------------------
//读写dat文件
static u8 fbuf[512];
static u32 fbuf_pos;

static void fbuf_LoadFromDisk(void)
{
  if(DataSectorIndex==0){
    _consolePrint("FatalError: Data sector index is 0.");
    ShowLogHalt();
  }
  active_interface->fn_ReadSectors(DataSectorIndex,1,fbuf);
}

static void fbuf_SaveToDisk(void)
{
  if(DataSectorIndex==0){
    _consolePrint("FatalError: Data sector index is 0.");
    ShowLogHalt();
  }
  active_interface->fn_WriteSectors(DataSectorIndex,1,fbuf);
}

static void fbuf_SetPos(u32 pos)
{
  fbuf_pos=pos;
}

static u32 fbuf_r32(void)
{
  u32 d=*(u32*)&(fbuf[fbuf_pos]);
  fbuf_pos+=4;
  return(d);
}

static void fbuf_w32(u32 d)
{
  *(u32*)&(fbuf[fbuf_pos])=d;
  fbuf_pos+=4;
}

static void* fbuf_GetCurrentPointer(void)
{
  return(&(fbuf[fbuf_pos]));
}

static void RomDataLoad(u8* toSaveRomData)
{
    REG_IME=0;
    fbuf_LoadFromDisk();
    REG_IME=1;	
	memcpy(toSaveRomData,fbuf,0x100);
}


static void RomDataSave(u8* toSaveRomData)
{
	memset(fbuf,0x00,512);
	memcpy(fbuf,toSaveRomData,0x100);
    REG_IME=0;
    fbuf_SaveToDisk();
    REG_IME=1;
}
//-------------------------------------------------------------
//找出游戏内部码
static void FindRomSign()
{
	uint8 *bm=(uint8*)safemalloc(512);
	FAT_FILE *fh=FAT2_fopen_AliasForRead(RomSetInfo.FullPathAlias);
	if(!fh)
	{
		FAT2_fclose(fh);
		if(bm)
		{
			safefree(bm);bm=NULL;
		}
	}
	FAT2_fread(bm,512,1,fh);
	FAT2_fclose(fh);
	memset(RomSign,0,5);
	RomSign[0]=bm[12];
	RomSign[1]=bm[13];		
	RomSign[2]=bm[14];
	RomSign[3]=bm[15];
	if(bm)
	{
		safefree(bm);bm=NULL;
	}
}
//打开特定游戏的dat文件
void OpenRomData()
{
	memset(RomSign,0x00,5);
	memset(SaveRomData,0x00,0x100);
	DataSectorIndex=0;	
	isRomDatFolderExist();		
	RomDatFileDataSectorIndex();
	FindRomSign();  
	if(memcmp(LastRomSign,RomSign,4))
	{
		memset((void*)&RomSetState,0x00,sizeof(TRomSetState));
	}	
	RomDataLoad(SaveRomData);
	if(memcmp(RomSign,&SaveRomData[0xa0],4))
	{
		//外部名与内部名不符，初始化为全0;
		memset(SaveRomData,0x00,0x100);
		memcpy(&SaveRomData[0xa0],RomSign,4);
		//初始化romset的状态
		memset((void*)&RomSetState,0x00,sizeof(TRomSetState));
		RomSetState.Speed=(SaveRomData[0xb6]==0)?5:SaveRomData[0xb6];
	}
	else//外部名与内部名相符，读取数据初始化
	{
		if(SaveRomData[0xb0])
			RomSetState.SoftReset=true;
		else
			RomSetState.SoftReset=false;
		if(SaveRomData[0xb1])
			RomSetState.RealtimeSave=true;
		else
			RomSetState.RealtimeSave=false;
		if(SaveRomData[0xb2])
			RomSetState.GameGuide=true;
		else
			RomSetState.GameGuide=false;
		if(SaveRomData[0xb3])
			RomSetState.CheatState=true;
		else
			RomSetState.CheatState=false;
		if(SaveRomData[0xb4])
			RomSetState.DownloadPlayState=true;
		else
			RomSetState.DownloadPlayState=false;
		if(SaveRomData[0xb5])
		{
			RomSetState.SpeciaMode=true;
			RomSetState.Speed=(SaveRomData[0xb6]==0)?5:SaveRomData[0xb6];
		}		
		else
		{
			RomSetState.SpeciaMode=false;
			RomSetState.Speed=(SaveRomData[0xb6]==0)?5:SaveRomData[0xb6];
		}
		if(bFlag)
		{
			memcpy(&RomSetState,&RomSetStateTmp,sizeof(TRomSetState));
			ClearFlag();
		}	
	}
}

void CloseRomData()
{
	 {
		if(RomSetState.SoftReset==true)
			SaveRomData[0xb0]=0x01;
		else
			SaveRomData[0xb0]=0x00;
		if(RomSetState.RealtimeSave==true)
			SaveRomData[0xb1]=0x01;
		else
			SaveRomData[0xb1]=0x00;
		if(RomSetState.GameGuide==true)
			SaveRomData[0xb2]=0x01;
		else
			SaveRomData[0xb2]=0x00;
		if(RomSetState.CheatState==true)
			SaveRomData[0xb3]=0x01;
		else
			SaveRomData[0xb3]=0x00;				
		if(RomSetState.DownloadPlayState==true)
			SaveRomData[0xb4]=0x01;
		else
			SaveRomData[0xb4]=0x00;	
		if(RomSetState.SpeciaMode==true)
		{
			SaveRomData[0xb5]=0x01;
			SaveRomData[0xb6]=RomSetState.Speed;
		}		
		else
			SaveRomData[0xb5]=0x00;	
	 }
	 {
		 memcpy(&SaveRomData[0xa0],RomSign,5);
	 }
	 RomDataSave(SaveRomData);
	 strcpy(LastRomSign,RomSign);
}

u32 dsi()
{
	return(DataSectorIndex);
}







