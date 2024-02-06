
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

#include "inifile.h"



TiniSkinUser GlobalUserINI;

static void InitSkinUser(TiniSkinUserBody *tag)
{
	tag->x=-1;
	tag->y=-1;
	tag->width=-1;
	tag->height=-1;
	tag->color=0x8000;
	strcpy(tag->png1,"NONPNGFLAG");
	strcpy(tag->png2,"NONPNGFLAG");
}

void InitINI_SkinUser(bool e)
{
	if(e)
	{
		TiniSkinUser *SkinUser=&GlobalUserINI;
		InitSkinUser(&SkinUser->LightControl);
		InitSkinUser(&SkinUser->ToggleControl);
		InitSkinUser(&SkinUser->HelpControl);
		InitSkinUser(&SkinUser->SystemControl);
		InitSkinUser(&SkinUser->ShutDownContol);
		InitSkinUser(&SkinUser->ScrollUP);
		InitSkinUser(&SkinUser->ScrollDOWN);
		InitSkinUser(&SkinUser->Launch);
		InitSkinUser(&SkinUser->Gameicon);
		InitSkinUser(&SkinUser->Gamename);
		//InitSkinUser(&SkinUser->Companyname);
		InitSkinUser(&SkinUser->Time);
		//InitSkinUser(&SkinUser->Rumble);
	}
	else
	{
		TiniSkinUser *SkinUser=&GlobalUserINI;
		//
		SkinUser->LightControl.x=4;
		SkinUser->LightControl.y=6;
		SkinUser->LightControl.width=32;
		SkinUser->LightControl.height=32;		
		SkinUser->LightControl.color=0x8000;
		strcpy(SkinUser->LightControl.png1,"NONPNGFLAG");
		strcpy(SkinUser->LightControl.png2,"bright_down.png");
		//
		SkinUser->ToggleControl.x=4;
		SkinUser->ToggleControl.y=43;
		SkinUser->ToggleControl.width=32;
		SkinUser->ToggleControl.height=32;		
		SkinUser->ToggleControl.color=0x8000;
		strcpy(SkinUser->ToggleControl.png1,"NONPNGFLAG");
		strcpy(SkinUser->ToggleControl.png2,"reserve_down.png");
		//
		SkinUser->HelpControl.x=4;
		SkinUser->HelpControl.y=79;
		SkinUser->HelpControl.width=32;
		SkinUser->HelpControl.height=32;		
		SkinUser->HelpControl.color=0x8000;
		strcpy(SkinUser->HelpControl.png1,"NONPNGFLAG");
		strcpy(SkinUser->HelpControl.png2,"help_down.png");
		//
		SkinUser->SystemControl.x=4;
		SkinUser->SystemControl.y=116;
		SkinUser->SystemControl.width=32;
		SkinUser->SystemControl.height=32;		
		SkinUser->SystemControl.color=0x8000;
		strcpy(SkinUser->SystemControl.png1,"NONPNGFLAG");
		strcpy(SkinUser->SystemControl.png2,"setup_down.png");
		//
		SkinUser->ShutDownContol.x=4;
		SkinUser->ShutDownContol.y=153;
		SkinUser->ShutDownContol.width=32;
		SkinUser->ShutDownContol.height=32;		
		SkinUser->ShutDownContol.color=0x8000;
		strcpy(SkinUser->ShutDownContol.png1,"NONPNGFLAG");
		strcpy(SkinUser->ShutDownContol.png2,"shut_down.png");		
		//
		SkinUser->ScrollUP.x=204;
		SkinUser->ScrollUP.y=32;
		SkinUser->ScrollUP.width=49;
		SkinUser->ScrollUP.height=51;		
		SkinUser->ScrollUP.color=0x8000;
		strcpy(SkinUser->ScrollUP.png1,"NONPNGFLAG");
		strcpy(SkinUser->ScrollUP.png2,"up_down.png");		
		//
		SkinUser->ScrollDOWN.x=204;
		SkinUser->ScrollDOWN.y=94;
		SkinUser->ScrollDOWN.width=49;
		SkinUser->ScrollDOWN.height=51;		
		SkinUser->ScrollDOWN.color=0x8000;
		strcpy(SkinUser->ScrollDOWN.png1,"NONPNGFLAG");
		strcpy(SkinUser->ScrollDOWN.png2,"down_down.png");	
		//
		SkinUser->Launch.x=72;
		SkinUser->Launch.y=135;
		SkinUser->Launch.width=113;
		SkinUser->Launch.height=37;		
		SkinUser->Launch.color=0x8000;
		strcpy(SkinUser->Launch.png1,"NONPNGFLAG");
		strcpy(SkinUser->Launch.png2,"run_down.png");	
		//
		SkinUser->Gameicon.x=113;
		SkinUser->Gameicon.y=11;
		SkinUser->Gameicon.width=0;
		SkinUser->Gameicon.height=0;		
		SkinUser->Gameicon.color=0x8000;
		strcpy(SkinUser->Gameicon.png1,"NONPNGFLAG");
		strcpy(SkinUser->Gameicon.png2,"NONPNGFLAG");	
		//
		SkinUser->Gamename.x=74;
		SkinUser->Gamename.y=47;
		SkinUser->Gamename.width=128;
		SkinUser->Gamename.height=26;		
		SkinUser->Gamename.color=0x8000;
		strcpy(SkinUser->Gamename.png1,"NONPNGFLAG");
		strcpy(SkinUser->Gamename.png2,"NONPNGFLAG");
		//
		/*SkinUser->Companyname.x=74;
		SkinUser->Companyname.y=73;
		SkinUser->Companyname.width=128;
		SkinUser->Companyname.height=26;		
		SkinUser->Companyname.color=0x8000;
		strcpy(SkinUser->Companyname.png1,"NONPNGFLAG");
		strcpy(SkinUser->Companyname.png2,"NONPNGFLAG");*/
		//
		SkinUser->Time.x=204;
		SkinUser->Time.y=4;
		SkinUser->Time.width=56;
		SkinUser->Time.height=12;		
		SkinUser->Time.color=33760;
		strcpy(SkinUser->Time.png1,"NONPNGFLAG");
		strcpy(SkinUser->Time.png2,"NONPNGFLAG");
		//
	/*	SkinUser->Rumble.x=96;
		SkinUser->Rumble.y=104;
		SkinUser->Rumble.width=70;
		SkinUser->Rumble.height=12;		
		SkinUser->Rumble.color=0x8000;
		strcpy(SkinUser->Rumble.png1,"NONPNGFLAG");
		strcpy(SkinUser->Rumble.png2,"NONPNGFLAG");		*/
	}
}
static bool SetSkinUserValue(char *key,s32 ivalue,char *bmp,TiniSkinUserBody *Rect)
{
  
    
  _consolePrintf("\nkey=%s",key);
  _consolePrintf("\nvalue=%s",bmp);
  _consolePrintf("\nivalue=%d",ivalue);

  
  if(ivalue<=-1) ivalue=-1;
    if(strcmp(key,"x")==0){
    Rect->x=ivalue;
    return(true);
  }
  if(strcmp(key,"y")==0){
    Rect->y=ivalue;
    return(true);
  }
  if(strcmp(key,"width")==0){
    Rect->width=ivalue;
    return(true);
  }
  if(strcmp(key,"height")==0){
    Rect->height=ivalue;
    return(true);
  }
  if(strcmp(key,"color")==0){
    Rect->color=(ivalue & 0xffff) | BIT(15);
    return(true); 
  }
  
  if(strcmp(key,"png1")==0){
    strcpy(Rect->png1,bmp);
    return(true);
  }
  if(strcmp(key,"png2")==0){
    strcpy(Rect->png2,bmp);
    return(true);
  }
  return(false);	
}

static char section[128];
static u32 readline;
static void readsection(char *str)
{
  str++;
  
  u32 ofs;
  
  ofs=0;
  while(*str!=']'){
    if((128<=ofs)||(*str==0)){
      _consolePrintf("line%d error.\nThe section name doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    section[ofs]=*str;
    str++;
    ofs++;
  }
  section[ofs]=0;
}
static void readkey(char *str)
{
  if(section[0]==0){
    _consolePrintf("line%d error.\nThere is a key ahead of the section name.\n",readline);
//    ShowLogHalt();
    return;
  }
  
  char key[128],value[128];
  
  u32 ofs;
  
  ofs=0;
  while(*str!='='){
    if((128<=ofs)||(*str==0)){
      _consolePrintf("line%d error.\nThe key name doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    key[ofs]=*str;
    str++;
    ofs++;
  }
  key[ofs]=0;
  
  str++;
  
  ofs=0;
  while(*str!=0){
    if(128<=ofs){
      _consolePrintf("line%d error.\nThe value doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    value[ofs]=*str;
    str++;
    ofs++;
  }
  value[ofs]=0;

  
  s32 ivalue=atoi(value);


  bool bvalue;
  
  if(ivalue==0){
    bvalue=false;
    }else{
    bvalue=true;
  }
  //added 20090525 加入自定义用户界面
 if(strcmp(section,"LightControl")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.LightControl;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"ToggleControl")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.ToggleControl;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"MultimediaControl")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.HelpControl;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"SystemControl")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.SystemControl;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"ShutDownContol")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.ShutDownContol;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"ScrollUP")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.ScrollUP;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"ScrollDOWN")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.ScrollDOWN;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"Launch")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Launch;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"Gameicon")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Gameicon;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 if(strcmp(section,"Gamename")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Gamename;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 /*if(strcmp(section,"Companyname")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Companyname;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }*/
 if(strcmp(section,"Time")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Time;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }
 /*if(strcmp(section,"Rumble")==0){
    TiniSkinUserBody *Rect=&GlobalUserINI.Rumble;
    
    if(SetSkinUserValue(key,ivalue,value,Rect)==true) return;
  }  */
}
static void internal_LoadGlobalINI(char *pini,u32 inisize)
{
  section[0]=0;
  readline=0;
  
  u32 iniofs=0;
  
  while(iniofs<inisize){
    
    readline++;
    
    u32 linelen=0;
    
    // Calc Line Length
    {
      char *s=&pini[iniofs];
      
      while(0x20<=*s){
        linelen++;
        s++;
        if(inisize<=(iniofs+linelen)) break;
      }
      *s=0;
    }
    
    if(linelen!=0){
      char c=pini[iniofs];
      if((c==';')||(c=='/')||(c=='!')){
        // comment line
        }else{
        if(c=='['){
          readsection(&pini[iniofs]);
          }else{
          readkey(&pini[iniofs]);
        }
      }
    }
    
    iniofs+=linelen;
    
    // skip NULL,CR,LF
    {
      char *s=&pini[iniofs];
      
      while(*s<0x20){
        iniofs++;
        s++;
        if(inisize<=iniofs) break;
      }
    }
    
  } 
}

void Shell_ReadSkinFile(const char *fn,void **pbuf,int *psize)
{
  FAT_FILE *fh;
  
  {
    char fullfn[256];
    snprintf(fullfn,256,"%s/%s",DefaultDataPath,fn);
    const char *pfullalias;
    pfullalias=ConvertFullPath_Ansi2Alias(fullfn);
    _consolePrintf("Shell_FAT_fopen_Data=%s\n",pfullalias);
    fh=FAT2_fopen_AliasForRead(pfullalias);    
  }
  
  if(fh!=NULL){
    FAT2_fseek(fh,0,SEEK_END);
    *psize=FAT2_ftell(fh);
    FAT2_fseek(fh,0,SEEK_SET);
    *pbuf=(void*)safemalloc(*psize);
    
    FAT2_fread(*pbuf,1,*psize,fh);
    FAT2_fclose(fh);
    return;
  }
  else
  {
	  FAT2_fclose(fh);  
  }
}

extern u32 SkinFile_GetFileIndexFromFilename1(const char *pFilename);
extern void SkinFile_LoadFileAllocate1(u32 SkinFileIndex,void **pbuf,s32 *psize);
extern void TestPauseKey(void);
void LoadINI_SkinUser(void)
{
  char *pini=NULL;
  s32 inisize=0;
  /*char pp[256];		        	
  snprintf(pp,256,"skin%d/EZSkin.ini",ProcState.SetupW.SkinSelect);
  Shell_ReadSkinFile(pp,(void**)&pini,(int*)&inisize);

  _consolePrintf("\n pp=%s",pp);
  */
  
  
  const char fn[]="EZSkin.ini";
  
  _consolePrintf("Load EZSkin table. [%s]\n",fn);
  
  
  u32 fidx=SkinFile_GetFileIndexFromFilename1(fn);
  
  if(fidx==(u32)-1){
    _consolePrintf("Fatal error: Skin file '%s' not found.\n",fn);
    ShowLogHalt();
  }
  
  SkinFile_LoadFileAllocate1(fidx,(void**)&pini,&inisize);

  if((pini==NULL)||(inisize==0)) 
  {
  	return;
  }
   
   

  internal_LoadGlobalINI(pini,inisize);
  if(pini)
  {
	  safefree(pini); 
	  pini=NULL;  
  }
}











