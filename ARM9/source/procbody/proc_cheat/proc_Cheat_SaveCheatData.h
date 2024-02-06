
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "_const.h"
#include "memtool.h"
#include "fat2.h"
#include "lang.h"
#include "zlibhelp.h"
#include "splash.h"
#include "shell.h"
#include "disc_io.h"
#include "../../ipc6.h"


extern LPIO_INTERFACE active_interface;
static u32 DataSectorIndex=0;

void CheatData_Init(void)
{  
    DataSectorIndex=0;
    const char *fn=DefaultDataPath "/cheatdata.bin";
    const char *palias=ConvertFullPath_Ansi2Alias(fn);
    _consolePrintf("Open setting file. [%s]\n",fn);
    if(palias!=NULL)
    {
        FAT_FILE *pf=FAT2_fopen_AliasForModify(palias);
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
    }
    else
    {
    	
    }    _consolePrintf("DataSectorIndex=%d.\n",DataSectorIndex);
  
    if(DataSectorIndex==0)
    {
        _consolePrintf("FatalError: File not found or size error. [%s]\n",DataFilename);
        ShowLogHalt();
    }
}
static void Set_DSIdx(u32 Idx)
{
	DataSectorIndex=Idx;
}
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

static void CheatDataLoad(u8* toSaveCheatData)
{
    REG_IME=0;
    fbuf_LoadFromDisk();
    REG_IME=1;	
	memcpy(toSaveCheatData,fbuf,0x100);
}
static void CheatDataSave(u8* toSaveCheatData)
{
	memset(fbuf,0x00,512);
	memcpy(fbuf,toSaveCheatData,0x100);
    REG_IME=0;
    fbuf_SaveToDisk();
    REG_IME=1;
  
		
}


