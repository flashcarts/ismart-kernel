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
#include "proc_GBA_RomOP.h"
#include "proc_GBA.h"
#include "../../mycartsrc/MartCartop.h"
#include "proc_GBA_Patch.h"

extern LPIO_INTERFACE active_interface;

extern "C" {
void VRAMWriteCache_Enable(void);
void VRAMWriteCache_Disable(void);
}

TGBAOPInfo GBAOPInfo;
GbaTmp GbaTmpInfo;

static bool bEraseFlag;
static bool bWriteFlag;
static bool bSaveFlag;
static bool bSaveMagFlag;

const u32 EraseProgressVal=175;
static u32 EraseProgressValCur;
void SetbEraseFlag(void)
{
	bEraseFlag = true;
}
void ClearbEraseFlag(void)
{
	bEraseFlag = false;
}
bool GetbEraseFlag(void)
{
	return bEraseFlag;
}
void SetbWriteFlag(void)
{
	bWriteFlag = true;
}
void ClearbWriteFlag(void)
{
	bWriteFlag = false;
}
bool GetbWriteFlag(void)
{
	return bWriteFlag;
}
void SetbSaveFlag(void)
{
	bSaveFlag = true;
}
void ClearbSaveFlag(void)
{
	bSaveFlag = false;
}
bool GetbSaveFlag(void)
{
	return bSaveFlag;
}
void SetbSaveMagFlag(void)
{
	bSaveMagFlag = true;
}
void ClearbSaveMagFlag(void)
{
	bSaveMagFlag = false;
}
bool GetbSaveMagFlag(void)
{
	return bSaveMagFlag;
}
u32 SetEraseProgressVal(u32 Val)
{
	if(Val>=EraseProgressVal)
		EraseProgressValCur=EraseProgressVal;
	else
		EraseProgressValCur=Val;
}
u32 GetEraseProgressVal()
{

	return EraseProgressValCur;
}

extern void videoSub_SetShowLog(bool);
extern void TestPauseKey(void);


static bool EraseNorFlashJA(char *pFileNameAlis)
{
	//擦除块	
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pFileNameAlis);
	if(fp)
	{
		SetbEraseFlag();
		u32 Size=FAT2_GetFileSize(fp);
		Enable_Arm9DS();
		OpenNorWrite();
		SetSerialMode();
		
		for(u32 kk=0;kk<Size;kk+=0x40000)
		{			
			_consolePrintf("Erasing %x(%x)\n",Size,kk);
			SetEraseProgressVal((EraseProgressVal*((kk+0x40000)/0x40000)/(Size/0x40000)));
			UpdataWindows();
			Block_Erase(kk);
		}		
		CloseNorWrite();			
		Enable_Arm7DS();
		ClearbEraseFlag();
		FAT2_fclose(fp);
		return true;
	}
	FAT2_fclose(fp);
	return false;
}
static bool LoadSaveData2Struct(char *pFileNameAlis)
{
	FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFileNameAlis);
    UnicodeChar* pUcode = (UnicodeChar* )FAT2_GetLongFilenameUnicode();
    FAT2_fclose(FileHandle);
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
    char *ap = strrchr(pFileNameAlis,'/');

    if(ap)
        ap++;  
    
    memset(a_saver,0,13);
    UnicodeChar w_saver[256];
    memcpy(a_saver,ap,13);
    char* s = strchr(a_saver,'.');
    *(s+1)='D';
    *(s+2)='A';
    *(s+3)='T';
    *(s+4)=0;
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
    	FAT2_chdir_Alias("/GBADATA");
    	u32 FAT_FileType=FAT2_FindFirstFile(&pafn);
   
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
	        	 // if (( pufn[namePos] > 0x60) && ( pufn[namePos] < 0x7B))
	        	//	  pufn[namePos] -= 0x20;
	        	 // if (( w_saver[namePos] > 0x60) && ( w_saver[namePos] < 0x7B))
	        	//	  w_saver[namePos] -= 0x20;  
	        	 /// 
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
    	
    	//FILE_TYPE fstype = FAT_FileExists(a_saver);
        if(fstype2 == FT_NONE)
        {
    		//不存在此补丁数据文件
    		return false;
        }
        else
        {
        	memcpy(a_saver,pafn,13);//用这个别名打开已有存档        	
        }        
    }
    //写补丁数据文件到rom
    FAT_FILE *pf=FAT2_fopen_AliasForRead(a_saver);	
	const u32 LEN=0x8000;
	u8 *pSrc = NULL;
	pSrc = (u8*)safemalloc(LEN*2);
	FAT2_fread(pSrc,LEN*2,1,pf);
	FAT2_fclose(pf);
	{
		_consolePrintf("\nReadDattoStruct!");
		GBAPatchData *pCur=NULL;
		GBAPatchData *pCurtemp=NULL;
		GBAPatchDataHead *pCurHead=NULL;
		u32 offset12=0;	
		pCurHead=(GBAPatchDataHead*)safemalloc(sizeof(GBAPatchDataHead));
		g_GBAPatchDataHead=*pCurHead;
		
		memcpy((GBAPatchDataHead*)&g_GBAPatchDataHead,pSrc,sizeof(GBAPatchDataHead));
		
		if(pCurHead)
			safefree(pCurHead);
		
		_consolePrintf("SaverType= 0x%x \n",g_GBAPatchDataHead.SaverType);
		_consolePrintf("SaveSize= 0x%x \n",g_GBAPatchDataHead.dwSaveSize);
		_consolePrintf("dwPatchNum= 0x%x \n",g_GBAPatchDataHead.dwPatchNum);	
		
		if(g_GBAPatchDataHead.dwPatchNum!=0)
		{
			pCur=(GBAPatchData*)safemalloc(sizeof(GBAPatchData));
			pCur->pNext=NULL;	
		}
		m_pGBAPatchData=pCur;
		for(u8 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
		{
			pCurtemp=(GBAPatchData*)safemalloc(sizeof(GBAPatchData));
			pCurtemp->pNext=NULL;
			pCur->pNext=pCurtemp;	
			memcpy((GBAPatchData*)pCurtemp,&pSrc[sizeof(GBAPatchDataHead)+offset12],sizeof(GBAPatchData)-8);
			pCurtemp->pData=(u8*)safemalloc(pCurtemp->patchdatasize);
			memcpy((u8*)pCurtemp->pData,&pSrc[sizeof(GBAPatchDataHead)+(sizeof(GBAPatchData)-8)+offset12],pCurtemp->patchdatasize);
			offset12+=sizeof(GBAPatchData)-8+pCurtemp->patchdatasize;
			
			pCur=pCurtemp;
		}
		pCur=NULL;
		pCur=m_pGBAPatchData;
		if(g_GBAPatchDataHead.dwPatchNum!=0)
		{
			m_pGBAPatchData=pCur->pNext;
			pCur->pNext=NULL;
			if(pCur!=NULL) safefree(pCur);
		}
	}	
	if(pSrc)
		safefree(pSrc);	
	return true;

}

static bool WriteNorFlashJA(char *pFileNameAlis,bool flag)
{
	//写入
	u32 LEN=0x8000;
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pFileNameAlis);
	u8 *pSrc;
	pSrc = (u8*)safemalloc(LEN);
	_consolePrintf("\n fp=%x",fp);
	if(fp)
	{
		if(!g_GBAPatchDataHead.dwPatchNum)
		{
			//存档补丁数据为空
			SetEraseProgressVal(0);	
			u32 Size=FAT2_GetFileSize(fp);
			FAT2_fseek(fp,0,SEEK_SET);
			SetbWriteFlag();
			for(u32 ii=0;ii<Size;ii+=LEN)
			{
				SetEraseProgressVal((EraseProgressVal*((ii+LEN)/LEN)/(Size/LEN)));
				UpdataWindows();
				_consolePrintf("Writing %x(%x)\n\n",ii,Size);	
				memset(pSrc,0,LEN);
				FAT2_fread(pSrc,LEN,1,fp);
				Enable_Arm9DS();
				OpenNorWrite();
				SetSerialMode();
				chip_reset();
				WriteNorFlash(ii,pSrc,LEN);
				chip_reset();
				CloseNorWrite();
			}		
			ClearbWriteFlag();
			if(pSrc)
				safefree(pSrc);
			FAT2_fclose(fp);
			return true;		
		}
		else
		{
			//存档补丁数据写入
			SetEraseProgressVal(0);	
			u32 Size=FAT2_GetFileSize(fp);
			FAT2_fseek(fp,0,SEEK_SET);
			SetbWriteFlag();
			
			for(u32 kk=0;kk<Size;kk+=LEN)
			{				
				SetEraseProgressVal((EraseProgressVal*((kk+LEN)/LEN)/(Size/LEN)));
				UpdataWindows();
				_consolePrintf("Writing %x(%x)\n\n",kk,Size);	
				memset(pSrc,0,LEN);
				FAT2_fread(pSrc,LEN,1,fp);
				
				GBAPatchData *pCur=NULL;
				u32 modifyC=0;
				u32 NextModifyIndex=0;
				u32 Nextdatalen=0;
				bool NextFlag=false;
				u32 offkk[256];
				pCur=NULL;
				pCur=m_pGBAPatchData;
				for(u32 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
				{
					offkk[ii]=(pCur->patchOff/0x8000)*0x8000;
					pCur=pCur->pNext;
					_consolePrintf("\n offkk[%d]=0x%x",ii,offkk[ii]);
				}
				if((Nextdatalen!=0)&&(NextFlag==true))//上次没有写完 这次接着写
				{
					pCur=NULL;
					pCur=m_pGBAPatchData;
					u32 kkqq=0;
					while(kkqq<NextModifyIndex)
					{
						pCur=pCur->pNext;
						kkqq++;
					}
					memcpy(pSrc,&pCur->pData[pCur->patchdatasize-Nextdatalen],Nextdatalen);
					Nextdatalen=0;
					NextFlag=false;
					NextModifyIndex=0;
				}
				for(u32 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
				{
					if(kk==offkk[ii])
					{
						_consolePrintf("\n offkk[%d]=0x%x",ii,offkk[ii]);
						
						pCur=NULL;
						pCur=m_pGBAPatchData;
						u32 kkqq=0;
						while(kkqq<ii)
						{
							pCur=pCur->pNext;
							kkqq++;
						}
						_consolePrintf("\n needsize=0x%x",pCur->patchdatasize);
						_consolePrintf("\n freesize=0x%x",0x8000-pCur->patchOff%0x8000);
						
						if(0x8000-(pCur->patchOff%0x8000)>=pCur->patchdatasize)
						{
							_consolePrintf("\n ONE Write");
							memcpy(&pSrc[pCur->patchOff%0x8000],pCur->pData,pCur->patchdatasize);
							Nextdatalen=0;
							NextFlag=false;
								
						}
						else
						{
							_consolePrintf("\n TwoWrite");
							memcpy(&pSrc[pCur->patchOff%0x8000],pCur->pData,0x8000-(pCur->patchOff%0x8000));
						    Nextdatalen=pCur->patchdatasize-(0x8000-(pCur->patchOff%0x8000));
							NextFlag=true;
							NextModifyIndex=ii;
						}	
						////TestPauseKey();	
					}
				}

				Enable_Arm9DS();
				OpenNorWrite();
				SetSerialMode();
				chip_reset();
				WriteNorFlash(kk,pSrc,LEN);
				chip_reset();				
				CloseNorWrite();
				 //_consolePrintf("\n idf=%x",idf);				
			}			
			ClearbWriteFlag();
			if(pSrc)
				safefree(pSrc);
			FAT2_fclose(fp);
			return true;			
		}
	}
	if(pSrc)
		safefree(pSrc);
	FAT2_fclose(fp);
	return false;
}

static bool SavePatchNorFlashJA(char *pFileNameAlis)
{
	//打存档补丁
	u32 LEN=0x8000;
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pFileNameAlis);

	if(fp)
	{
		u32 Size=FAT2_GetFileSize(fp);
		FAT2_fclose(fp);
		u8* pPos=(u8*)0x8000000;
		SAVERTYPE *dwSaverType;
		SAVERTYPE SaverType=UNKNOW;
		dwSaverType=&SaverType;
		s32 *dwSaveSize;
		s32 SaveS=0;
		dwSaveSize=&SaveS;
		s32 msize=0;
		s32 mRamPageOffset=0x10;
		msize=(s32)Size;
		
		SetbSaveFlag();
		SetEraseProgressVal(0);
		UpdataWindows();
		
		Enable_Arm9DS();
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		_consolePrintf("\n Start SavePatch:");
		SaverPatch(&pPos,&msize,mRamPageOffset,dwSaverType,dwSaveSize);
		_consolePrintf("\n SavePatch End!!:");
		CloseNorWrite();
		Enable_Arm9DS();
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		CloseNorWrite();
		
		_consolePrintf("dwSaverType=%x\,dwSaveSize=%x",*dwSaverType,*dwSaveSize);
		
		return true;
	}

	FAT2_fclose(fp);
	return false;
}
static FAT_FILE *CreatFile( char *pFullPathAlias,u32 DesSize)
{
	FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFullPathAlias);
    UnicodeChar* pUcode = (UnicodeChar* )FAT2_GetLongFilenameUnicode();
    FAT2_fclose(FileHandle);
    int j =0;
    do
    {
        _consolePrintf("%04x ",pUcode[j]);
        j++ ;
    }
    while(pUcode[j]);
        _consolePrintf("\n");
    _consolePrintf("***************\n");
    
    char a_saver[12];
    char *ap = strrchr(pFullPathAlias,'/');
    _consolePrintf("\n a_saver2=%s ",ap);
    if(ap)
        ap++;    
    UnicodeChar w_saver[256];
    memcpy(a_saver,ap,12);
    _consolePrintf("\n a_saver1=%s ",a_saver);
    char* s = strchr(a_saver,'.');
    *(s+1)='D';
    *(s+2)='A';
    *(s+3)='T';
    *(s+4)=0;
   
   _consolePrintf("\n a_saver=%s ",a_saver);
   
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
    	FAT2_chdir_Alias("/GBADATA");
    	u32 FAT_FileType=FAT2_FindFirstFile(&pafn);
   
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
	        	 // if (( pufn[namePos] > 0x60) && ( pufn[namePos] < 0x7B))
	        	//	  pufn[namePos] -= 0x20;
	        	 // if (( w_saver[namePos] > 0x60) && ( w_saver[namePos] < 0x7B))
	        	//	  w_saver[namePos] -= 0x20;  
	        	 /// 
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
    	
    	//FILE_TYPE fstype = FAT_FileExists(a_saver);
        if(fstype2 == FT_NONE)
        {

            //创建存档文件
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVER");
            filePK = FAT2_fopen_CreateForWrite_on_CurrentFolder(a_saver,w_saver);
           	FAT2_SetSize(filePK,DesSize,0xAB); 
           	FAT2_fclose(filePK);
            //ClearMessageBox(pScreenMainOverlay->pCanvas);
        }
        else
        {
        	memcpy(a_saver,pafn,12);//用这个别名打开已有存档
        }
        FAT_FILE *pf=FAT2_fopen_AliasForWrite(a_saver);
        return pf;
    }

}
static void isRomDatFolderExist()
{
	FAT_chdir("/");
	FILE_TYPE fstype = FAT_FileExists("/GBADATA"); //大小写无关
    if(fstype == FT_DIR)
    { 
    	_consolePrintf("GBADATA Folder exist \n");    
    }
    else
    {
        _consolePrintf("GBADATA Folder NOT exist \n");
            //这里是创建新目录 显示信息
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVE");
        if(FAT_mkdir("/GBADATA")==0)
        {
        	_consolePrintf("GBADATA Folder Create success\n");
        }
        else
            _consolePrintf("GBADATA Folder Create failed\n");
    }
}
static bool WriteSavePatchData2file(char *pFileNameAlis)
{
	//回写补丁数据文件
	isRomDatFolderExist();							
	FAT_FILE *fpData=CreatFile(pFileNameAlis,0x8000*2);
	if(fpData)
	{
		//写数据到文件
		const u32 LEN=0x8000;
		u8 *pSrc = NULL;
		u8 *pSrc1 = NULL;
		pSrc = (u8*)safemalloc(LEN*2);
		pSrc1 = (u8*)safemalloc(LEN);
		
		GBAPatchData *pCur=NULL;
		pCur=m_pGBAPatchData;
		u32 offset12=0;
		memset(pSrc,0,LEN*2);
		memcpy(pSrc,(GBAPatchDataHead*)&g_GBAPatchDataHead,sizeof(GBAPatchDataHead));
		_consolePrintf("\n num=%x",g_GBAPatchDataHead.dwPatchNum);
		_consolePrintf("\n Write Head Suc,Write data?");					
		for(u32 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
		{	
			_consolePrintf("\n Pass OR!");
			memcpy(&pSrc[sizeof(GBAPatchDataHead)+offset12],(GBAPatchData*)pCur,sizeof(GBAPatchData)-8);
			memcpy(&pSrc[sizeof(GBAPatchDataHead)+(sizeof(GBAPatchData)-8)+offset12],(u8*)pCur->pData,pCur->patchdatasize);
			offset12+=sizeof(GBAPatchData)-8+pCur->patchdatasize;
			pCur=pCur->pNext;
		}
		_consolePrintf("\n Write Data Suc!");
		
		for(u32 ii=0;ii<LEN*2;ii+=LEN)
		{
			memcpy(pSrc1,&pSrc[ii],LEN);
			FAT2_fwrite(pSrc1,LEN,1,fpData);
		}
		if(pSrc)
		{
			safefree(pSrc);
			pSrc=NULL;
		}
		if(pSrc1)
		{
			safefree(pSrc1);
			pSrc1=NULL;
		}
		FAT2_fclose(fpData);
		_consolePrintf("Write Data File ok\n");
		return true;
	}
	FAT2_fclose(fpData);
	return false;
}
static bool isGbaTmpExits()
{
	const char *pfn;	  
	pfn=DefaultDataPath "/" GBATmpFileName;
	if(FullPath_FileExistsAnsi(pfn))
		return true;
	else
		return false;
}

static bool SaveGbaTmpName(char *pFileNameAlis,bool Typeflag)
{
	bool QFlag;
	QFlag=Typeflag;
	 _consolePrintf("\nTypeflag1=%d",Typeflag);
	 _consolePrintf("\nQFlag=%d",QFlag);	 	 
	FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFileNameAlis);
    UnicodeChar* pUcode = (UnicodeChar* )FAT2_GetLongFilenameUnicode();
    FAT2_fclose(FileHandle);
    int j =0;
    do
    {
        _consolePrintf("%04x ",pUcode[j]);
        j++ ;
    }
    while(pUcode[j]);
        _consolePrintf("\n");
    _consolePrintf("***************\n");
    
    char a_saver[12];
    char *ap = strrchr(pFileNameAlis,'/');

    if(ap)
        ap++;  
    
    memset(a_saver,0,12);
    UnicodeChar w_saver[256];
    memcpy(a_saver,ap,12);
    char* s = strchr(a_saver,'.');
    *(s+1)='S';
    *(s+2)='A';
    *(s+3)='V';
    *(s+4)=0;  
    memcpy(w_saver,pUcode,j*2);
    w_saver[j-3]=L'S';
    w_saver[j-2]=L'A';
    w_saver[j-1]=L'V';
    w_saver[j]=0;
	
	memset(&GbaTmpInfo,0,sizeof(GbaTmp));	 
	 if(Typeflag==1)
    {
    	strcpy(GbaTmpInfo.Sign,"PS");
    }
    else
    {
    	strcpy(GbaTmpInfo.Sign,"NF");
    }
    strcpy(GbaTmpInfo.SAlis,a_saver);
    GbaTmpInfo.SAlis[12]='\0';
    Unicode_Copy(GbaTmpInfo.WName,w_saver);


}
static bool SaveGbaTmp()
{
	if(!isGbaTmpExits())
	{
		 _consolePrintf("\n GBA Save file erro,can not find 'gbatmp.bin' .");
		 ShowLogHalt();
	}
	else
	{
		u8 *pBuf;
		pBuf=(u8*)safemalloc(544);
		memset(pBuf,0,544);
		FAT_FILE *pf=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(DefaultDataPath "/" GBATmpFileName));
		if(pf)
		{
			FAT2_fread(pBuf,1,544,pf);
			FAT2_fclose(pf);
		}
		else
		{	
			FAT2_fclose(pf);
			return false;
		}
		
		pf=NULL;
		pf=FAT2_fopen_AliasForWrite(ConvertFullPath_Ansi2Alias(DefaultDataPath "/" GBATmpFileName));
		if(pf)
		{
			 _consolePrintf("\n 1=%s ",GbaTmpInfo.SAlis);
			 _consolePrintf("\n 2=%s ",GbaTmpInfo.Sign);
			if(!strcmp(GbaTmpInfo.Sign,"PS"))
			{
				//PSRAM写在前16+256字节
				memset(pBuf,0,16+256);
				memcpy(pBuf,&(GbaTmp)GbaTmpInfo,16+256);
				 _consolePrintf("\n Hear1");
			}
			else
			{
				//NorFlash写在后16+256字节
				memset(&pBuf[16+256],0,16+256);
				memcpy(&pBuf[16+256],&(GbaTmp)GbaTmpInfo,16+256);
				 _consolePrintf("\n Hear2");
			}
			FAT2_fwrite(pBuf,1,544,pf);			
			if(pBuf)
				safefree(pBuf);	
			FAT2_fclose(pf);
			return true;
		}
		else
		{
			if(pBuf)
				safefree(pBuf);
			FAT2_fclose(pf);
			return false;
		}
	}
}

static void isGBASAVEFolderExist()
{
	FAT_chdir("/");
	FILE_TYPE fstype = FAT_FileExists("/GBASAVE"); //大小写无关
    if(fstype == FT_DIR)
    { 
    	_consolePrintf("GBASAVE Folder exist \n");    
    }
    else
    {
        _consolePrintf("GBASAVE Folder NOT exist \n");
            //这里是创建新目录 显示信息
            //MessageDialog_Draw(pScreenMainOverlay->pCanvas,(UnicodeChar *)L"Create SAVE");
        if(FAT_mkdir("/GBASAVE")==0)
        {
        	_consolePrintf("GBASAVE Folder Create success\n");
        }
        else
            _consolePrintf("GBASAVE Folder Create failed\n");
    }
}
static bool BackupSav(char *a_saver,UnicodeChar *w_saver,bool TypeFlag)
{
	isGBASAVEFolderExist();	
	FAT2_chdir_Alias("/GBASAVE");
	FAT_FILE *filePK = FAT2_fopen_CreateForWrite_on_CurrentFolder_AA(a_saver,w_saver);
   	FAT2_fclose(filePK);
   	
    FAT_FILE *pf=FAT2_fopen_AliasForWrite(a_saver);
    FAT2_fseek(pf,-1,SEEK_SET);
    int LEN=0x8000;
	u8* savpSrc=NULL;
	savpSrc = (u8*)safemalloc(LEN);
    if(pf)
    {
    	if(TypeFlag)
    	{
    		//PSRAM
    		for(int kk=0;kk<0x20000;kk+=LEN)
    		{
    			Enable_Arm9DS();
    			SetRampage(0x70+kk/0x1000);
				ReadSram(0xA000000,savpSrc,LEN);
				Enable_Arm9DS();
				FAT2_fwrite(savpSrc,LEN,1,pf);
    		}
    		
    	}
    	else
    	{
    		//NORFLASH
    		for(int kk=0;kk<0x20000;kk+=LEN)
    		{
    			Enable_Arm9DS();
    			SetRampage(0x10+kk/0x1000);
				ReadSram(0xA000000,savpSrc,LEN);
				Enable_Arm9DS();
				FAT2_fwrite(savpSrc,LEN,1,pf);
    		}
    	}
    	if(savpSrc)
    		safefree(savpSrc);
    	FAT2_fclose(pf);
    	return true;
    }
    else
    {

    	if(savpSrc)
    		safefree(savpSrc);
    	FAT2_fclose(pf);
    	return false;
    }
   	
}

static bool LoadSav(char *a_saver,UnicodeChar *w_saver,bool TypeFlag)
{

	{
		FAT_chdir("/");
		FILE_TYPE fstype = FAT_FileExists("/GBASAVE"); //大小写无关
	    if(fstype == FT_DIR)
	    { 
	    	_consolePrintf("GBASAVE Folder exist \n");    
	    }
	    else
	    {
	        return false;
	    }
	}	
	FAT2_chdir_Alias("/GBASAVE");   	
    FAT_FILE *pf=FAT2_fopen_AliasForRead(a_saver);
    
    int LEN=0x8000;
	u8* savpSrc=NULL;
	savpSrc = (u8*)safemalloc(LEN);
    if(pf)
    {
    	FAT2_fseek(pf,-1,SEEK_SET);
    	if(TypeFlag)
    	{
    		//PSRAM
    		for(int kk=0;kk<0x20000;kk+=LEN)
    		{
    			Enable_Arm9DS();
    			FAT2_fread(savpSrc,LEN,1,pf);
    			Enable_Arm9DS();
    			SetRampage(0x70+kk/0x1000);
				WriteSram(0xA000000,savpSrc,LEN);				
    		}    		
    	}
    	else
    	{
    		//NORFLASH
    		for(int kk=0;kk<0x20000;kk+=LEN)
    		{
    			Enable_Arm9DS();
    			FAT2_fread(savpSrc,LEN,1,pf);    			
    			Enable_Arm9DS();
    			SetRampage(0x10+kk/0x1000);
				WriteSram(0xA000000,savpSrc,LEN);				
    		}    
    	}
    	if(savpSrc)
    		safefree(savpSrc);
    	FAT2_fclose(pf);
    	return true;
    }
    else
    {
    	if(savpSrc)
    		safefree(savpSrc);
    	FAT2_fclose(pf);
    	return false;
    }
	   	
}

static bool CmpGBASave(bool TypeFlag)
{
	//当前的文件的名字存在结构体中，上次的文件的名字存在文件中
	//需要比较两次的名字，名字不同则需要备份存档和加在存档文件
	if(!isGbaTmpExits())
	{
		//文件不存在,加载当前文件名的存档
		
		_consolePrintf("\n q2....");
		_consolePrintf("\n GbaTmpInfo.SAlis=%s",GbaTmpInfo.SAlis);
		_consolePrintf("\n TypeFlag=%d",TypeFlag);		
	
		SetbSaveMagFlag();
		UpdataWindows();
		LoadSav(GbaTmpInfo.SAlis,GbaTmpInfo.WName,TypeFlag);
		ClearbSaveMagFlag();
		return false;
	}
	else
	{
		//文件存在,比较两文件名,备份上次文件名存档,加载当前文件名的存档
		u8 *pBuf;
		pBuf=(u8*)safemalloc(544);
		memset(pBuf,0,544);
		FAT_FILE *pf=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(DefaultDataPath "/" GBATmpFileName));
		if(pf)
		{
			FAT2_fread(pBuf,1,544,pf);
			FAT2_fclose(pf);
		}
		else
		{	
			FAT2_fclose(pf);
			return false;
		}
		
		_consolePrintf("\n q3....");
		_consolePrintf("\n GbaTmpInfo.SAlis=%s",GbaTmpInfo.SAlis);
		_consolePrintf("\n TypeFlag=%d",TypeFlag);		
		//TestPauseKey();
		
		if(TypeFlag)
		{
			//比较PSRAM
			if(memcmp(GbaTmpInfo.SAlis,&pBuf[3],12))
			{
				
				SetbSaveMagFlag();
				UpdataWindows();
				//两文件名不同,备份,加载
				GbaTmp Tmp1;
				memcpy(Tmp1.Sign,&pBuf[0],3);
				memcpy(Tmp1.SAlis,&pBuf[3],13);
				memcpy(Tmp1.WName,&pBuf[16],256);		
				
				_consolePrintf("\n %c%c%c%c%c%c%c%c%c%c%c%c",pBuf[3],pBuf[4],pBuf[5],pBuf[6],pBuf[7],pBuf[8],pBuf[9],pBuf[10],pBuf[11],pBuf[12],pBuf[13],pBuf[14]);
				_consolePrintf("\n q4....");
				_consolePrintf("\n Tmp1.SAlis=%s",Tmp1.SAlis);
				_consolePrintf("\n TypeFlag=%d",TypeFlag);	
				
				//TestPauseKey();				
				if(!strcmp(Tmp1.Sign,"PS"))
				{
					//文件存在,但文件名为空,不备分存档
					BackupSav(Tmp1.SAlis,Tmp1.WName,TypeFlag);
				}
				LoadSav(GbaTmpInfo.SAlis,GbaTmpInfo.WName,TypeFlag);				
				ClearbSaveMagFlag();

;
			}
		}
		else
		{
			//比较NorFlash
			if(memcmp(GbaTmpInfo.SAlis,&pBuf[3+16+256],12))
			{
				SetbSaveMagFlag();
				UpdataWindows();
				//两文件名不同,备份,加载
				GbaTmp Tmp1;
//				memcpy(&(GbaTmp)Tmp1,&pBuf[16+256],16+256);
				memcpy(Tmp1.Sign,&pBuf[0+16+256],3);
				memcpy(Tmp1.SAlis,&pBuf[3+16+256],13);
				memcpy(Tmp1.WName,&pBuf[16+16+256],256);
				if(!strcmp(Tmp1.Sign,"NF"))
				{
					//文件存在,但文件名为空,不备分存档
					BackupSav(Tmp1.SAlis,Tmp1.WName,TypeFlag);
				}				
				LoadSav(GbaTmpInfo.SAlis,GbaTmpInfo.WName,TypeFlag);
				ClearbSaveMagFlag();
			}
		}
		if(pBuf)
			safefree(pBuf);		
	}
}

bool WriteGbaRomToNorFlash()
{
	/**********************************************************************/
	//写rom到3in1
	//打开rom文件
	char pp[256];
	char pp2[256];
	strcpy(pp,GBAOPInfo.FullPathAlias);
	strcpy(pp2,GBAOPInfo.FullPathAlias);
	
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pp);
			
	if(fp)
	{
		FAT2_fclose(fp);
		//擦除
		_consolePrintf("\n Erase....");
		strcpy(pp,GBAOPInfo.FullPathAlias);
		EraseNorFlashJA(pp);
		//写入
		_consolePrintf("\n Load....");
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		

		bool bSaveDataFlag = LoadSaveData2Struct(pp);
				
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		SaveGbaTmpName(pp,false);
		CmpGBASave(false);
		SaveGbaTmp();
		_consolePrintf("\n Write....");
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		_consolePrintf("\n pp=%s",pp);
	
		bSaveDataFlag = LoadSaveData2Struct(pp);
		WriteNorFlashJA(pp,bSaveDataFlag);	
		
		bSaveDataFlag = LoadSaveData2Struct(pp);

		if(!bSaveDataFlag)
		{
			//第一次写入 打存档补丁	
			_consolePrintf("\n Patching....");
			
			strcpy(pp,GBAOPInfo.FullPathAlias);
			SavePatchNorFlashJA(pp);
			
			strcpy(pp,GBAOPInfo.FullPathAlias);
			WriteSavePatchData2file(pp);
			ClearbSaveFlag();
		}			
		FreeQuickPatchData();
	}
	/**********************************************************************/
}

static bool WritePsRamJA(char *pFileNameAlis,bool flag)
{
	//写入
	vuint16* pPSram=(vuint16*)0x08800000;
	vuint16* pTemp=pPSram;
	u32 LEN=0x8000;
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pFileNameAlis);
	u8 *pSrc;
	pSrc = (u8*)safemalloc(LEN);
	_consolePrintf("\n fp=%x",fp);
	if(fp)
	{
		if(!g_GBAPatchDataHead.dwPatchNum)
		{
			//存档补丁数据为空
			SetEraseProgressVal(0);	
			u32 Size=FAT2_GetFileSize(fp);
			FAT2_fseek(fp,0,SEEK_SET);
			SetbWriteFlag();
			for(u32 ii=0;ii<Size;ii+=LEN)
			{
				pTemp=pPSram+ii/2;
				SetEraseProgressVal((EraseProgressVal*((ii+LEN)/LEN)/(Size/LEN)));
				UpdataWindows();
				_consolePrintf("Writing %x(%x)\n\n",ii,Size);	
				memset(pSrc,0,LEN);
				FAT2_fread(pSrc,LEN,1,fp);
				Enable_Arm9DS();
				SetRompage(192);
				OpenNorWrite();
				SetSerialMode();
				for(s32 jj=0;jj<LEN;jj+=2)
				{

					pTemp[jj>>1]=(pSrc[jj])+(pSrc[jj+1]<<8);
				}
				CloseNorWrite();
				Enable_Arm7DS();
			}		
			ClearbWriteFlag();
			if(pSrc)
				safefree(pSrc);
			FAT2_fclose(fp);
			return true;		
		}
		else
		{
			//存档补丁数据写入
			SetEraseProgressVal(0);	
			u32 Size=FAT2_GetFileSize(fp);
			FAT2_fseek(fp,0,SEEK_SET);
			SetbWriteFlag();
			
			for(u32 kk=0;kk<Size;kk+=LEN)
			{				
				pTemp=pPSram+kk/2;
				SetEraseProgressVal((EraseProgressVal*((kk+LEN)/LEN)/(Size/LEN)));
				UpdataWindows();
				_consolePrintf("Writing %x(%x)\n\n",kk,Size);	
				memset(pSrc,0,LEN);
				FAT2_fread(pSrc,LEN,1,fp);
				
				GBAPatchData *pCur=NULL;
				u32 modifyC=0;
				u32 NextModifyIndex=0;
				u32 Nextdatalen=0;
				bool NextFlag=false;
				u32 offkk[256];
				pCur=NULL;
				pCur=m_pGBAPatchData;
				for(u32 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
				{
					offkk[ii]=(pCur->patchOff/0x8000)*0x8000;
					pCur=pCur->pNext;
					_consolePrintf("\n offkk[%d]=0x%x",ii,offkk[ii]);
				}
				if((Nextdatalen!=0)&&(NextFlag==true))//上次没有写完 这次接着写
				{
					pCur=NULL;
					pCur=m_pGBAPatchData;
					u32 kkqq=0;
					while(kkqq<NextModifyIndex)
					{
						pCur=pCur->pNext;
						kkqq++;
					}
					memcpy(pSrc,&pCur->pData[pCur->patchdatasize-Nextdatalen],Nextdatalen);
					Nextdatalen=0;
					NextFlag=false;
					NextModifyIndex=0;
				}
				for(u32 ii=0;ii<g_GBAPatchDataHead.dwPatchNum;ii++)
				{
					if(kk==offkk[ii])
					{
						_consolePrintf("\n offkk[%d]=0x%x",ii,offkk[ii]);
						
						pCur=NULL;
						pCur=m_pGBAPatchData;
						u32 kkqq=0;
						while(kkqq<ii)
						{
							pCur=pCur->pNext;
							kkqq++;
						}
						_consolePrintf("\n needsize=0x%x",pCur->patchdatasize);
						_consolePrintf("\n freesize=0x%x",0x8000-pCur->patchOff%0x8000);
						
						if(0x8000-(pCur->patchOff%0x8000)>=pCur->patchdatasize)
						{
							_consolePrintf("\n ONE Write");
							memcpy(&pSrc[pCur->patchOff%0x8000],pCur->pData,pCur->patchdatasize);
							Nextdatalen=0;
							NextFlag=false;
								
						}
						else
						{
							_consolePrintf("\n TwoWrite");
							memcpy(&pSrc[pCur->patchOff%0x8000],pCur->pData,0x8000-(pCur->patchOff%0x8000));
						    Nextdatalen=pCur->patchdatasize-(0x8000-(pCur->patchOff%0x8000));
							NextFlag=true;
							NextModifyIndex=ii;
						}	
					}
				}
				Enable_Arm9DS();
				SetRompage(192);
				OpenNorWrite();
				SetSerialMode();
				for(s32 jj=0;jj<LEN;jj+=2)
				{

					pTemp[jj>>1]=(pSrc[jj])+(pSrc[jj+1]<<8);
				}
				CloseNorWrite();
				Enable_Arm7DS();		
			}			
			ClearbWriteFlag();
			if(pSrc)
				safefree(pSrc);
			FAT2_fclose(fp);
			return true;			
		}
	}
	if(pSrc)
		safefree(pSrc);
	FAT2_fclose(fp);
	return false;
}
static bool SavePatchPsRamJA(char *pFileNameAlis)
{
	//打存档补丁
	u32 LEN=0x8000;
	FAT_FILE *fp=FAT2_fopen_AliasForRead(pFileNameAlis);

	if(fp)
	{
		u32 Size=FAT2_GetFileSize(fp);
		FAT2_fclose(fp);
		u8* pPos=(u8*)0x8800000;
		SAVERTYPE *dwSaverType;
		SAVERTYPE SaverType=UNKNOW;
		dwSaverType=&SaverType;
		s32 *dwSaveSize;
		s32 SaveS=0;
		dwSaveSize=&SaveS;
		s32 msize=0;
		s32 mRamPageOffset=0x10;
		msize=(s32)Size;
		
		SetbSaveFlag();
		SetEraseProgressVal(0);
		UpdataWindows();
		
		Enable_Arm9DS();
		SetRompage(192);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		_consolePrintf("\n pPos=%x",*pPos);
		_consolePrintf("\n Size=%x",Size);
		
		_consolePrintf("\n Start SavePatch:");
		SaverPatchPsRam(&pPos,&msize,mRamPageOffset,dwSaverType,dwSaveSize);
		_consolePrintf("\n SavePatch End!!:");
		Enable_Arm9DS();
		chip_reset();
		CloseNorWrite();
		Enable_Arm7DS();

		_consolePrintf("dwSaverType=%x\,dwSaveSize=%x",*dwSaverType,*dwSaveSize);
		
		return true;
	}

	FAT2_fclose(fp);
	return false;
}
bool WriteGbaRomToPsRam()
{
	char pp[256];
	char pp2[256];
	strcpy(pp,GBAOPInfo.FullPathAlias);
	strcpy(pp2,GBAOPInfo.FullPathAlias);
	_consolePrintf("\n a=%s",GBAOPInfo.FullPathAlias);
	_consolePrintf("\n 11....");
	

	FAT_FILE *fp=FAT2_fopen_AliasForRead(pp);
	//大于16M游戏 退出。
	if( FAT2_GetFileSize(fp) > 0x1000000)
	{
		
		FAT2_fclose(fp);
		return;
	}
	
	_consolePrintf("\n 22....");

	if(fp)
	{
		FAT2_fclose(fp);
		//写入
		_consolePrintf("\n Load....");
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		bool bSaveDataFlag = LoadSaveData2Struct(pp);
		
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		
		SaveGbaTmpName(pp,true);		
		CmpGBASave(true);		
		SaveGbaTmp();
		
		_consolePrintf("\n Write....");
		strcpy(GBAOPInfo.FullPathAlias,pp2);
		strcpy(pp,GBAOPInfo.FullPathAlias);
		_consolePrintf("\n pp=%s",pp);
		
		WritePsRamJA(pp,bSaveDataFlag);	
		
		if(!bSaveDataFlag)
		{
			//第一次写入 打存档补丁	
			_consolePrintf("\n Patching....");
			strcpy(pp,GBAOPInfo.FullPathAlias);
			SavePatchPsRamJA(pp);
			
			strcpy(pp,GBAOPInfo.FullPathAlias);
			WriteSavePatchData2file(pp);
			ClearbSaveFlag();
		}			
		FreeQuickPatchData();

	}
	/**********************************************************************/
}













