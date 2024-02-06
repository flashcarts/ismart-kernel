
#include "nds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
//#include "procbody/proc_cheat/proc_Cheat_S

#include "SkinDirListFile.h"



SkinListFile  ListFile[128];
int ListFileNum;

SkinListFile  MulLangListFile[128];
int MulLangListFileNum;


static bool isExt32VSDAT(u32 Ext32)
{
  if(Ext32==MakeExt32(0,'D','A','T')) return(true);
  return(false);
}
static bool isExt32VSINI(u32 Ext32)
{
  if(Ext32==MakeExt32(0,'I','N','I')) return(true);
  return(false);
}
void ListSkinDirFile(void)
{
    //切换到制定目录
    memset(ListFile,0x00,128*sizeof(SkinListFile));
    ListFileNum=0;
    
	char pp[256];
	strcpy(pp,ConvertFullPath_Ansi2Alias(DefaultDataPath "/skin/"));
	
	if((pp==NULL)||(FAT2_chdir_Alias(pp)==false))
    {
        //_consolePrintf("Can not change path. [%s]\n",pBasePathAlias);
        ShowLogHalt();
    }
  

  
    {
        const char *pafn;
        u32 FAT_FileType=FAT2_FindFirstFile(&pafn);
    
        while(FAT_FileType!=FT_NONE)
        {
            if((FAT2_GetAttrib()&ATTRIB_HID)==0)
            {
                switch(FAT_FileType)
                {
                case FT_NONE: break;
                case FT_DIR: 
                {      	               	
                } break;
                case FT_FILE: 
                {
                	//if(strcmp(pafn,"SYS.BIN"))
                	{
                    	u32 Ext32=0;
                        {
                          const char *ptmp=pafn;
                          while(*ptmp!=0){
                            u32 ch=*ptmp++;
                            if(ch==(u32)'.'){
                              Ext32=0;
                              }else{
                              if((0x61<=ch)&&(ch<=0x7a)) ch-=0x20;
                              Ext32=(Ext32<<8)|ch;
                            }
                          }
                        }
                        
                        if(isExt32VSDAT(Ext32)){
                        	strcpy(ListFile[ListFileNum].Filename,pafn);
	                          int idx = 0;
                        	  while((pafn[idx] != '\0') && (pafn[idx] != '.'))
                        	  {
                        		  if(idx != 0)
                        		  {
                        			  if(((u32)pafn[idx]>=65) && ((u32)pafn[idx]<=90))
	                          		  {
	                          			  ListFile[ListFileNum].DispName[idx] = pafn[idx] + 0x20;
	                          		  }
	                          		  else
	                          			  ListFile[ListFileNum].DispName[idx] = pafn[idx];                        			  
                        		  }
                        		  else
                        		  {
                        			  if(((u32)pafn[idx]>=0x61) && ((u32)pafn[idx]<=0x7a))
	                          		  {
	                          			  ListFile[ListFileNum].DispName[idx] = pafn[idx] - 0x20;
	                          		  }
	                          		  else
	                          			  ListFile[ListFileNum].DispName[idx] = pafn[idx];                          			  
                        		  }
                        		  idx++;	
                        	  }
                        	  ListFile[ListFileNum].DispName[idx] = '\0';
                    		  
                        	ListFileNum++;                        	
                        }
                                      		
                	}
                } break;
              }
          }
          
          FAT_FileType=FAT2_FindNextFile(&pafn);
        }
    }
    
    
    _consolePrintf("Can not change path. [%d]\n",ListFileNum);
}
void printSkinFileList(void)
{
	int i=0;
	 _consolePrintf("ListFileNum=[%d]\n",ListFileNum);
	while(i < ListFileNum)
	{
		_consolePrintf("[%s][%s]\n",ListFile[i].Filename,ListFile[i].DispName);
		i++;
	}
}
void ListMulLangFile(void)
{
    //切换到制定目录
    memset(MulLangListFile,0x00,128*sizeof(SkinListFile));
    MulLangListFileNum=0;
    
	char pp[256];
	strcpy(pp,ConvertFullPath_Ansi2Alias(DefaultDataPath "/language/"));
	
	if((pp==NULL)||(FAT2_chdir_Alias(pp)==false))
    {
        //_consolePrintf("Can not change path. [%s]\n",pBasePathAlias);
        ShowLogHalt();
    }
  
    {
        const char *pafn;
        u32 FAT_FileType=FAT2_FindFirstFile(&pafn);
    
        while(FAT_FileType!=FT_NONE)
        {
            if((FAT2_GetAttrib()&ATTRIB_HID)==0)
            {
                switch(FAT_FileType)
                {
                case FT_NONE: break;
                case FT_DIR: 
                {      	               	
                } break;
                case FT_FILE: 
                {
                	//if(strcmp(pafn,"SYS.BIN"))
                	{
                    	u32 Ext32=0;
                        {
                          const char *ptmp=pafn;
                          while(*ptmp!=0){
                            u32 ch=*ptmp++;
                            if(ch==(u32)'.'){
                              Ext32=0;
                              }else{
                              if((0x61<=ch)&&(ch<=0x7a)) ch-=0x20;
                              Ext32=(Ext32<<8)|ch;
                            }
                          }
                        }
                        
                        if(isExt32VSINI(Ext32)){
                        	strcpy(MulLangListFile[MulLangListFileNum].Filename,pafn);
	                          int idx = 0;
                        	  while((pafn[idx] != '\0') && (pafn[idx] != '.'))
                        	  {
                        		 if(idx!=0)
                        		 {
	                           		  if(((u32)pafn[idx]>=65) && ((u32)pafn[idx]<=90))
	                           		  {
	                           			  MulLangListFile[MulLangListFileNum].DispName[idx] = pafn[idx] + 0x20;
	                           		  }
	                           		  else
	                           			  MulLangListFile[MulLangListFileNum].DispName[idx] = pafn[idx];
                        		 }
                        		 else
                        		 {
                        			 if(((u32)pafn[idx]>=0x61) && ((u32)pafn[idx]<=0x7a))
	                           		  {
	                           			  MulLangListFile[MulLangListFileNum].DispName[idx] = pafn[idx] - 0x20;
	                           		  }
	                           		  else
	                           			  MulLangListFile[MulLangListFileNum].DispName[idx] = pafn[idx];
                        		 }
                        		 idx++;	
                        	  }
                        	  MulLangListFile[MulLangListFileNum].DispName[idx] = '\0';
                    		  
                        	  MulLangListFileNum++;                        	
                        }
                                      		
                	}
                } break;
              }
          }
          
          FAT_FileType=FAT2_FindNextFile(&pafn);
        }
    }
    
    
    _consolePrintf("Can not change path. [%d]\n",MulLangListFileNum);
}
void printMulLangFileList(void)
{
	int i=0;
	 _consolePrintf("MulLangListFileNum=[%d]\n",MulLangListFileNum);
	while(i < MulLangListFileNum)
	{
		_consolePrintf("[%s][%s]\n",MulLangListFile[i].Filename,MulLangListFile[i].DispName);
		i++;
	}
}
