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

GBAPatchDataHead g_GBAPatchDataHead;
GBAPatchData     *m_pGBAPatchData=NULL;


//extern s32 g_fpos;
//extern u32 gl_PatchFlag;

s32 TotalOff=0;
u8*	FindMotif(u8 *Buffer, s32 BufferSize, u8 *Motif, s32 MotifSize,s32 *off)
{
	bool b512=false;
	u8* find = Buffer;
	u8* pTemp = Buffer;
	while(find)
	{
		if(((s32)Buffer)>=0xA000000)
		{
			//page switch
			CloseNorWrite();
			Enable_Arm9DS();
			SetRompage(768);
			OpenNorWrite();
			SetSerialMode();
			chip_reset();	
	
			b512=true;
			Buffer-=0x2000000;
			chip_reset();
		}
		find = (u8* )memchr(Buffer,Motif[0],BufferSize-MotifSize);
		if (find)
		{
			if (memcmp(find,Motif,MotifSize) == 0)
			{
				break;
			}
		}
		BufferSize -= find - Buffer + 1;
		Buffer = find + 1;
	}
	*off=find-pTemp;
	*off+=TotalOff;
	if(b512==true)
	{
		*off+=0x2000000;
		CloseNorWrite();
		Enable_Arm9DS();
		SetRompage(0);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();	
		b512=true;
		chip_reset();
		return (find+0x2000000);
	}
	return find;
}
extern void TestPauseKey(void);
void ModifyFlash(s32 off,u8 *szbuf,s32 len)
{
	//****需要释放的空间包含两部分*****
	// *****GBAPatchData 和 GBAPatchData.pData*****
	
	//backup change 
	GBAPatchData *pCur=m_pGBAPatchData;
	GBAPatchData *pTemp;
	if(pCur==NULL)
	{
		pTemp=(GBAPatchData *)safemalloc(sizeof(GBAPatchData)); 
		pTemp->pNext=NULL;
		pTemp->sign[0]='G';pTemp->sign[1]='B';pTemp->sign[2]='A';pTemp->sign[3]='\0';
		if(((s32)off)>=0x2000000)
			pTemp->patchOff=off-0x2000000-TotalOff;
		else
			pTemp->patchOff=off-TotalOff;
		pTemp->patchdatasize=len;
		pTemp->pData=(u8 *)safemalloc(len);
		memcpy(pTemp->pData,szbuf,len);
		pCur=pTemp;
		g_GBAPatchDataHead.dwPatchNum++;
		m_pGBAPatchData=pCur;
		_consolePrintf("pCur->patchOff=%x\n",pCur->patchOff);
		_consolePrintf("pCur->patchdatasize=%x\n",pCur->patchdatasize);
	}
	else
	{
		while(pCur->pNext!=NULL)
		{
			pCur=pCur->pNext;
		}
		pTemp=(GBAPatchData *)safemalloc(sizeof(GBAPatchData)); 
		pTemp->pNext=NULL;
		pTemp->sign[0]='G';pTemp->sign[1]='B';pTemp->sign[2]='A';pTemp->sign[3]='\0';
		if(((s32)off)>=0x2000000)
			pTemp->patchOff=off-0x2000000-TotalOff;
		else
			pTemp->patchOff=off-TotalOff;
		pTemp->patchdatasize=len;
		pTemp->pData=(u8 *)safemalloc(len);
		memcpy(pTemp->pData,szbuf,len);
		pCur->pNext=pTemp;
		g_GBAPatchDataHead.dwPatchNum++;
		_consolePrintf("pCur->patchOff=%x\n",pCur->patchOff);
		_consolePrintf("pCur->patchdatasize=%x\n",pCur->patchdatasize);
	}
	_consolePrintf("m_pGBAPatchData=%x\n",m_pGBAPatchData);
	//
	_consolePrintf("Enter ModifyFlash Function\n");
	_consolePrintf("off=0x%x,szbuf=%s,len=%x\n",off,szbuf,len);
	bool b512=false;
	s32 bolckNum=0;
	s32 blockOff=0;
	s32 loop;
	u8* pBuf = NULL;
	//
	if(((s32)off)>=0x2000000)
	{
		//page switch
		CloseNorWrite();
		Enable_Arm9DS();
		SetRompage(768);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();	
		b512=true;
		off-=0x2000000;
		chip_reset();
	}
	off &=0xFFFFFF;
	pBuf=(u8*)safemalloc(0x40000);
	if(pBuf==NULL){
    	_consolePrint("Fatal error!! safemalloc 0x40000 failed.\n");
    	//ShowLogHalt();
    	while(1);
  	}
	//首先计算是哪一块
	bolckNum = off>>18;
	blockOff = off & 0x3FFFF;
	
//	TestPauseKey();
	_consolePrintf("bolckNum=0x%x,blockOff=%x\n",bolckNum,blockOff);
	
	//读出块中的数据
	Enable_Arm9DS();
	OpenNorWrite();
	SetSerialMode();
	chip_reset();
	uint32 id= ReadNorFlashID();
	_consolePrintf("id=%x\n",id);
	for(loop=0;loop<0x40000;loop+=0x8000)
		ReadNorFlash(&pBuf[loop],(u32)((bolckNum<<18)+loop),0x8000);

	for(loop=0;loop<0x40000;loop+=0x8000)
		_consolePrintf("pBuf[0x%x]=0x%x 0x%x 0x%x 0x%x\n",loop,pBuf[loop],pBuf[loop+1],pBuf[loop+2],pBuf[loop+3]);	
    	_consolePrintf("pBuf[0x%x]=0x%x 0x%x 0x%x 0x%x\n",blockOff,pBuf[blockOff],pBuf[blockOff+1],pBuf[blockOff+2],pBuf[blockOff+3]);
//    TestPauseKey();
    
    chip_reset();
	CloseNorWrite();
	
	
	
	
	_consolePrintf("ModifyFlash 1\n");
	//修改
	memcpy(&pBuf[blockOff],szbuf,len);
	_consolePrintf("pBuf[blockOff]=0x%x 0x%x 0x%x 0x%x\n",pBuf[blockOff],pBuf[blockOff+1],pBuf[blockOff+1],pBuf[blockOff+3]);
	_consolePrintf("ModifyFlash 2\n");
	
//	TestPauseKey();
	//写回
	Enable_Arm9DS();
 	OpenNorWrite();
 	SetSerialMode();
	chip_reset();
	Block_Erase((u32)(bolckNum<<18));
	chip_reset();
	CloseNorWrite();
	
	Enable_Arm9DS();
 	OpenNorWrite();
 	SetSerialMode();
	chip_reset();
	
	_consolePrintf("ModifyFlash 3\n");
	//WriteNorFlash((bolckNum<<18),pBuf,0x40000);
	
	for(loop=0;loop<0x40000;loop+=0x8000)
	{
		if(b512==true)
			{
				
				WriteNorFlash((u32)((bolckNum<<18)+loop+0x2000000),&pBuf[loop],0x8000);
				_consolePrintf("peeeee111");
				_consolePrintf("pBuf[0x%x]=0x%x 0x%x 0x%x 0x%x\n",loop,pBuf[loop],pBuf[loop+1],pBuf[loop+2],pBuf[loop+3]);	
				chip_reset();
//				TestPauseKey();
			}
		
		else
			{
				WriteNorFlash((u32)((bolckNum<<18)+loop),&pBuf[loop],0x8000);
				_consolePrintf("peeeee");
				_consolePrintf("pBuf[0x%x]=0x%x 0x%x 0x%x 0x%x\n",loop,pBuf[loop],pBuf[loop+1],pBuf[loop+2],pBuf[loop+3]);	
				chip_reset();
//				TestPauseKey();			
			}
	}
	CloseNorWrite();
//	_consolePrintf("ModifyFlash 4\n");
	//结束
	if(pBuf)
	{
		safefree(pBuf);
		pBuf = NULL;
	}
	if(b512==true)
	{
		CloseNorWrite();
		Enable_Arm9DS();
		SetRompage(0);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		b512=false;
		chip_reset();
	}
}

s32 GetSaveTypeAndSize(u8 **buf,s32 *length,SAVERTYPE *type,s32 *off)
{
	u8* find = NULL ;
	*type = UNKNOW ;
	s32 rvalue = 0;
	
	find = FindMotif(*buf, *length, (u8*)"SRAM_", 5,off);
	if(find)
	{
		*type = SRAM_TYPE ; //表示存档类型为sram
		rvalue = 32768;
	}
	find = FindMotif(*buf, *length, (u8*)"EEPROM_V", 8,off);
	if(find)
	{
		*type = EEPROM_TYPE ; //表示存档类型为sram
		rvalue = 8192 ;
	}
	find = FindMotif(*buf, *length, (u8*)"FLASH_V", 7,off);
	if(find) 
	{
		*type = FLASH_TYPE ;
		rvalue = 65536;
	}
	find = FindMotif(*buf, *length, (u8*)"FLASH512_V", 9,off);
	if(find) 
	{
		*type = FLASH_TYPE ;
		rvalue = 65536;
	}
	find = FindMotif(*buf, *length, (u8*)"FLASH1M_", 8,off);
	if(find)
	{
		*type = FLASH_TYPE ;
		rvalue = 65536*2;
	}
	
	char name[16] ;
	memset(name,0,16);
	//here 处理特别的游戏
	memcpy(name,&(*buf)[0xa0],12);
	memset(name,0,16);
	memcpy(name,&(*buf)[0xAC],4);
	if((!strcmp("GMBA",name))||(!strcmp("PCEA",name))||
		(!strcmp("FCA",name))||(!strcmp("PNES",name)))
	{
		*type = SRAM_TYPE ; //表示存档类型为sram
		rvalue = 32768;
	}
	return rvalue;
}
u8* FindSaveType(u8* Buffer,s32 BufferSize,SAVERTYPE *ptype)
{
	s32 loop;
	bool b512=false;
	u8* find = Buffer;
	
	u8 	sramsign[]="SRAM_";
	u16 sramsize=5;
	u8 	eepromsign[]="EEPROM_V";
	u16 eepromsize=8;
	u8  falshVSign[]="FLASH_V";
	u16 flashVSize=7;
	u8  flash512VSign[]="FLASH512_V";
	u16 flash512Size=10;
	u8  falsh1MSign[]="FLASH1M_V";
	u16 flash1MSize=9;
	
	_consolePrintf("Buffer=%x\n",Buffer);
	_consolePrintf("find=%x\n",(s32)find);
	_consolePrintf("&find=%x\n",&find);
	if(gl_ShakeID==0x89168916)
	{
			*((vuint16 *)(find)) = 0x50 ;
			*((vuint16 *)(find+0x1000*2)) = 0x50 ;
			*((vuint16 *)(find)) = 0xFF ;
			*((vuint16 *)(find+0x1000*2)) = 0xFF ;
	}
	_consolePrintf("find[0]=0x%x 0x%x 0%x 0x%x\n",find[0],find[1],find[2],find[3]);
	for(loop=0;loop<=BufferSize;loop++)
	{
		if(((s32)find+loop)>=0xA000000)
		{
			//page switch
			CloseNorWrite();
			Enable_Arm9DS();
			SetRompage(768);
			OpenNorWrite();
			SetSerialMode();
			chip_reset();
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find+loop)) = 0x50 ;
					*((vuint16 *)(find+loop+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find+loop)) = 0xFF ;
					*((vuint16 *)(find+loop+0x1000*2)) = 0xFF ;
			}
			
			b512=true;
			find-=0x2000000;
			chip_reset();
		}
		else
		{
			Enable_Arm9DS();
			OpenNorWrite();
			SetSerialMode();
			chip_reset();
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find+loop)) = 0x50 ;
					*((vuint16 *)(find+loop+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find+loop)) = 0xFF ;
					*((vuint16 *)(find+loop+0x1000*2)) = 0xFF ;
			}
			chip_reset();
		}
		if(find[loop]==sramsign[0])
		{
			_consolePrintf("sramsign\n");
			if(memcmp(&find[loop],sramsign,sramsize) == 0)
			{
				*ptype= SRAM_TYPE;
				if(b512)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();	
					b512=false;
					chip_reset();
					return (&find[loop]+0x2000000);
				}
				return &find[loop];
			}
		}
		if(find[loop]==eepromsign[0])
		{
			_consolePrintf("eepromsign\n");
			if(memcmp(&find[loop],eepromsign,eepromsize) == 0)
			{
				*ptype= EEPROM_TYPE;
				if(b512)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					chip_reset();
					return (&find[loop]+0x2000000);
				}
				return &find[loop];
			}
		}
		if(find[loop]==falshVSign[0])
		{
			_consolePrintf("falshVSign\n");
			if(memcmp(&find[loop],falshVSign,flashVSize) == 0)
			{
				*ptype= FLASH_TYPE;
				if(b512)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();	
					b512=false;
					chip_reset();
					return (&find[loop]+0x2000000);
				}
				return &find[loop];
			}
		}
		if(find[loop]==flash512VSign[0])
		{
			_consolePrintf("flash512VSign\n");
			if(memcmp(&find[loop],flash512VSign,flash512Size) == 0)
			{
				*ptype= FLASH512_TYPE;
				if(b512)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					chip_reset();
					return (&find[loop]+0x2000000);
				}
				return &find[loop];
			}
		}
		if(find[loop]==falsh1MSign[0])
		{
			_consolePrintf("falsh1MSign\n");
			if(memcmp(&find[loop],falsh1MSign,flash1MSize) == 0)
			{
				*ptype= FLASH1M_TYPE;
				_consolePrintf("loop=0x%x\n",loop);
				_consolePrintf("find[loop]=0x%x 0x%x 0%x 0x%x\n",find[loop],find[loop+1],find[loop+2],find[loop+3]);
				if(b512)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					chip_reset();
					return (&find[loop]+0x2000000);
				}
				return &find[loop];
			}
		}
	}
}

/*
u8 **buf    游戏数据指针
s32 *lengt 	游戏大小
s32 saverOffse 存档页偏移
SAVERTYPE *dwSaverType 存档类型
s32 *dwSaveSize 存档大小
*/

s32 SaverPatch(u8 **buf,s32 *length,s32 saverOffset,SAVERTYPE *dwSaverType,s32 *dwSaveSize)
{

	g_GBAPatchDataHead.SaverType=UNKNOW;
	g_GBAPatchDataHead.dwSaveSize=0;
	g_GBAPatchDataHead.dwPatchNum=0;
	GBAPatchData *p=m_pGBAPatchData;
	while(p)
	{
		p=m_pGBAPatchData->pNext;
		if(m_pGBAPatchData)
		{
			if(m_pGBAPatchData->pData)
				safefree(m_pGBAPatchData->pData);
			safefree(m_pGBAPatchData);
			m_pGBAPatchData=NULL;
			m_pGBAPatchData=p;
		}
	}
	//GBAPatchData     *pGBAPatchData;
	s32 off=0;
	u8 *find, *findread, *findwrite, *findidentify, *finderase, *finderasesec,*findrampage;
	s32 rvalue = 0;
	SAVERTYPE SaverType=UNKNOW;
	bool b512=false;
	//*dwSaverType = (SAVERTYPE)0 ;	
	rvalue = 0;
	TotalOff=((s32)*buf)-0x8000000;
	_consolePrintf("*buf=0x%x\n",*buf);
	_consolePrintf("TotalOff=0x%x\n",TotalOff);
	//find = FindMotif(*buf, *length, (u8*)"SRAM_", 5,&off);
//	_consolePrintf(/*"*dwSaverType=%d\n"*/,*dwSaverType);		
	
//	TestPauseKey();
	
	find= FindSaveType(*buf,*length,dwSaverType);
	
	SetEraseProgressVal(80);
	UpdataWindows();

	_consolePrintf("find=%x\n",find);
	_consolePrintf("dwSaverType=0x%x\n",*dwSaverType);
	_consolePrintf("End FindSaveType \n");

//	TestPauseKey();
	
	if(*dwSaverType==SRAM_TYPE)
	{
		_consolePrintf("here 1\n");
		if(find)
		{
			//*dwSaverType = SRAM_TYPE ; //表示存档类型为sram
			*dwSaveSize = 32768;
			SaverType=SRAM_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=SRAM_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
		}
	//gl_PatchFlag=1;
	//g_fpos=18;
	//MWin_DrawClient(WM_GBAProgress);
	//MWin_TransWindow(WM_GBAProgress);
		
	}
	//find = FindMotif(*buf, *length, (u8*)"EEPROM_V", 8,&off);
	else if(*dwSaverType==EEPROM_TYPE)
	{
		_consolePrintf("here 2\n");
		if(find)
		{
//			*dwSaverType = EEPROM_TYPE ;
			*dwSaveSize = 8192;
			SaverType=EEPROM_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=EEPROM_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			_consolePrintf("here 3\n");
			if(((s32)find)>=0xA000000)
			{
				//page switch
				CloseNorWrite();
				Enable_Arm9DS();
				SetRompage(768);
				OpenNorWrite();
				SetSerialMode();
				chip_reset();
				b512=true;
				find-=0x2000000;
				if(gl_ShakeID==0x89168916)
				{
						*((vuint16 *)(find)) = 0x50 ;
						*((vuint16 *)(find+0x1000*2)) = 0x50 ;
						*((vuint16 *)(find)) = 0xFF ;
						*((vuint16 *)(find+0x1000*2)) = 0xFF ;
				}
				chip_reset();
			}
			else
			{
				if(gl_ShakeID==0x89168916)
				{
						*((vuint16 *)(find)) = 0x50 ;
						*((vuint16 *)(find+0x1000*2)) = 0x50 ;
						*((vuint16 *)(find)) = 0xFF ;
						*((vuint16 *)(find+0x1000*2)) = 0xFF ;
				}	
			}
			if((char)find[8]=='1' && (char)find[9]=='2' &&(char)find[10] =='4')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					chip_reset();
					find+=0x2000000;
				}
				else
				{
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}	
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				findread = FindMotif(*buf, *length, e12x_read, e12x_read_LEN,&off0);
				findwrite = FindMotif(*buf, *length, e124_write, e12x_write_LEN,&off1);
				if(!findwrite)
					findwrite = FindMotif(*buf, *length, e124_write2, 0x20,&off2);
				findidentify = FindMotif(*buf, *length, e12x_identify, e12x_identify_LEN,&off3);
				
				if(findread && findwrite)
				{
					if(findidentify)
					{
						//memcpy(findidentify+0x36, e12x_newidentify, e12x_newidentify_LEN);
						ModifyFlash(off3+0x36,e12x_newidentify,e12x_newidentify_LEN);
					}	
					//memcpy(findread, e12x_newread, e12x_newread_LEN);
					ModifyFlash(off0,e12x_newread,e12x_newread_LEN);
					//memcpy(findwrite, e12x_newwrite, e12x_newwrite_LEN);
					if(off2!=0)
					{
						ModifyFlash(off2,e12x_newwrite,e12x_newwrite_LEN);
					}
					else if(off1!=0)
					{
						ModifyFlash(off1,e12x_newwrite,e12x_newwrite_LEN);
					}
				
				}
				
			}
			else if((char)find[8]=='1' && (char)find[9]=='2')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();	
					b512=false;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					find+=0x2000000;
					chip_reset();
				}
				else
				{
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}	
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				_consolePrintf("here 4\n");
				findread = FindMotif(*buf, *length, e12x_read, e12x_read_LEN,&off0);
				_consolePrintf("off0=0x%x\n",off0);
				findwrite = FindMotif(*buf, *length, e12x_write, e12x_write_LEN,&off1);
				findidentify = FindMotif(*buf, *length, e12x_identify, e12x_identify_LEN,&off2);
				_consolePrintf("findread=0x%x  *findread=%x\n",findread,*findread);
				_consolePrintf("findwrite=0x%x\n",findwrite);
				_consolePrintf("findidentify=0x%x\n",findidentify);
				if(findread && findwrite)
				{
					if(findidentify)
					{
						//memcpy(findidentify+0x36, e12x_newidentify, e12x_newidentify_LEN);
						ModifyFlash(off2+0x36,e12x_newidentify,e12x_newidentify_LEN);
					}		
					//memcpy(findread, e12x_newread, e12x_newread_LEN);
					ModifyFlash(off0,e12x_newread,e12x_newread_LEN);
					//memcpy(findwrite, e12x_newwrite, e12x_newwrite_LEN);
					ModifyFlash(off1,e12x_newwrite,e12x_newwrite_LEN);
				}
			}
			else if((char)find[8]=='1' && (char)find[9]=='1')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					find+=0x2000000;
					chip_reset();
				}
				else
				{
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}	
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				findread = FindMotif(*buf, *length, e11x_read, e11x_read_LEN,&off0);
				findwrite = FindMotif(*buf, *length, e11x_write, e11x_write_LEN,&off1);
				_consolePrintf("findread=0x%x\n",findread);
				_consolePrintf("findwrite=0x%x\n",findwrite);
				if(findread && findwrite)
				{
					u8 *pTemp = (u8 *) safemalloc(256) ;
					int find2 = (int)(findread - *buf)+TotalOff;
					int find3 = (int)(findwrite - *buf)+TotalOff;
					u8 st[10], pr[188];
					memcpy(st,e11x_newread,e11x_newread_LEN);
					memcpy(pr,e11x_code,e11x_code_LEN);
					pr[184] = (find2-TotalOff) + 0x21;
					pr[186] = (find2-TotalOff) >> 16;
					
					//memcpy((*buf)+find3,e11x_newwrite,e11x_newwrite_LEN);
					_consolePrintf("find3=0x%x\n",find3);
					ModifyFlash(find3,e11x_newwrite,e11x_newwrite_LEN);
					memset(pTemp,0xFF,256);
					findrampage=FindMotif(*buf, *length, pTemp,256,&off3);
					_consolePrintf("findrampage=0x%x\n",findrampage);
					if(!findrampage)
					{
						memset(pTemp,0,256);
						findrampage=FindMotif(*buf, *length, pTemp,256,&off4);
					}
					if(findrampage)
					{
						u32 endf;
						_consolePrintf("*buf=0x%x\n",*buf);
						endf=findrampage-*buf+TotalOff;
						_consolePrintf("endf=0x%x\n",endf);
						endf=(endf +0x10)&0xFFFFF0;
						_consolePrintf("endf=0x%x\n",endf);
						if(((*buf)[endf-1] != 0xff) && ((*buf)[endf-1] != 0xcd))
							pr[185] = (find2-TotalOff + 0x1f) >> 8;
						else
							pr[185] = (find2-TotalOff) >> 8;

						st[4] = ((char)(endf-TotalOff)&0xFF)+1;
						st[5] = (char)((endf-TotalOff) >> 8)&0xFF;
						st[6] = (char)((endf-TotalOff) >> 16)&0xFF;
						
						//memcpy((*buf)+find2, st, 10);
						_consolePrintf("find2=0x%x\n",find2);
						_consolePrintf("endf=0x%x\n",endf);
						 	
						ModifyFlash(find2,st,10);
						//memcpy((*buf)+endf, pr, 188);
						ModifyFlash(endf,pr,188);
					}
					if(pTemp)
					{
						safefree(pTemp);
					}
				}
			}
		}
	//gl_PatchFlag=1;
	//g_fpos=18;
	//MWin_DrawClient(WM_GBAProgress);
	//MWin_TransWindow(WM_GBAProgress);	
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH_V", 7,&off);
	else if(*dwSaverType==FLASH_TYPE)
	{
		_consolePrintf("Find FLASH_V\n");

		if(((s32)find)>=0xA000000)
		{
			//page switch
			CloseNorWrite();
			Enable_Arm9DS();
			SetRompage(768);
			OpenNorWrite();
			SetSerialMode();
			chip_reset();
			b512=true;
			find-=0x2000000;
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find)) = 0x50 ;
					*((vuint16 *)(find+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find)) = 0xFF ;
					*((vuint16 *)(find+0x1000*2)) = 0xFF ;
			}
			chip_reset();
		}
		else
		{
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find)) = 0x50 ;
					*((vuint16 *)(find+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find)) = 0xFF ;
					*((vuint16 *)(find+0x1000*2)) = 0xFF ;
			}	
		}
		if(find)
		{
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			
			if((char)find[7]=='1' && (char)find[8]=='2' && (char)find[9]>'1')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					find+=0x2000000;
					chip_reset();
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				
				findread = FindMotif(*buf, *length, f12x_read, f12x_read_LEN,&off0);
				findwrite = FindMotif(*buf, *length, f12x_write, f12x_write_LEN,&off1);
				findidentify = FindMotif(*buf, *length, f12x_identify, f12x_identify_LEN,&off2);
				finderase = FindMotif(*buf, *length, f12x_erase, f12x_erase_LEN,&off3);
				finderasesec = FindMotif(*buf, *length, f12x_erasesec, f12x_erasesec_LEN,&off4);

				if(findread && findwrite)
				{
					if(findidentify)
					{
						//memcpy(findidentify, f12x_MNidentify, f12x_newidentify_LEN);
						ModifyFlash(off2,f12x_MNidentify,f12x_newidentify_LEN);
					}
					if(finderase)
					{
						//memcpy(finderase, f12x_return0, f12x_return0_LEN);
						ModifyFlash(off3,f12x_return0, f12x_return0_LEN);
					}
					
					if(finderasesec)
					{
						//memcpy(finderasesec, f12x_return0, f12x_return0_LEN);
						ModifyFlash(off4, f12x_return0, f12x_return0_LEN);
					}		
					//memcpy(findread, f12x_newread, f12x_newread_LEN);
					ModifyFlash(off0, f12x_newread, f12x_newread_LEN);
					//memcpy(findwrite, f12x_newwrite, f12x_newwrite_LEN);
					ModifyFlash(off1, f12x_newwrite, f12x_newwrite_LEN);
					
				}
			}
			if((char)find[7]=='1' && (char)find[8]=='2' && (char)find[9]<'2')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					find+=0x2000000;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					chip_reset();
				}
				else
				{
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}	
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				
				findwrite = FindMotif(*buf, *length, f121_write, f121_write_LEN,&off0);
				findidentify = FindMotif(*buf, *length, f121_identify, f121_identify_LEN,&off1);
				finderase = FindMotif(*buf, *length, f121_erase, f121_erase_LEN,&off2);
				finderasesec = FindMotif(*buf, *length, f121_erasesec, f121_erasesec_LEN,&off3);
				_consolePrintf("\nhere mao\n");
				_consolePrintf("\nfindwrite=%x",findwrite);
				_consolePrintf("\nfindidentify=%x",findidentify);
				_consolePrintf("\nfinderase=%x",finderase);
				_consolePrintf("\nfinderasesec=%x",finderasesec);						
				if(findwrite)
				{
					_consolePrintf("\nhere mao2\n");
					if(findidentify)
					{
						//memcpy(findidentify, f121_newidentify, f121_newidentify_LEN);
						ModifyFlash(off1, f121_newidentify, f121_newidentify_LEN);
					}
					if(finderase)
					{
						//memcpy(finderase, f121_return0, f121_return0_LEN);
						ModifyFlash(off2, f121_return0, f121_return0_LEN);
					}
						
					if(finderasesec)
					{
						//memcpy(finderasesec, f121_return0, f121_return0_LEN);
						ModifyFlash(off3, f121_return0, f121_return0_LEN);
					}
					//memcpy(findwrite, f121_newwrite, f121_newwrite_LEN);
					ModifyFlash(off0, f121_newwrite, f121_newwrite_LEN);
				}
				
			}
		}
//	gl_PatchFlag=1;	
//	g_fpos=18;
//	MWin_DrawClient(WM_GBAProgress);
//	MWin_TransWindow(WM_GBAProgress);
	
	
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH512_V", 10,&off);
	else if(*dwSaverType==FLASH512_TYPE)
	{
		_consolePrintf("Find FLASH512_V\n");
		
		if(((s32)find)>=0xA000000)
		{
			//page switch
			CloseNorWrite();
			Enable_Arm9DS();
			SetRompage(768);
			OpenNorWrite();
			SetSerialMode();
			chip_reset();
			b512=true;
			find-=0x2000000;
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find)) = 0x50 ;
					*((vuint16 *)(find+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find)) = 0xFF ;
					*((vuint16 *)(find+0x1000*2)) = 0xFF ;
			}
			chip_reset();
		}
		else
		{
			if(gl_ShakeID==0x89168916)
			{
					*((vuint16 *)(find)) = 0x50 ;
					*((vuint16 *)(find+0x1000*2)) = 0x50 ;
					*((vuint16 *)(find)) = 0xFF ;
					*((vuint16 *)(find+0x1000*2)) = 0xFF ;
			}	
		}
		{
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			if((char)find[10]=='1'&& (char)find[11]=='3')
			{
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					if(gl_ShakeID==0x89168916)
					{
							*((vuint16 *)(find)) = 0x50 ;
							*((vuint16 *)(find+0x1000*2)) = 0x50 ;
							*((vuint16 *)(find)) = 0xFF ;
							*((vuint16 *)(find+0x1000*2)) = 0xFF ;
					}
					find+=0x2000000;
					chip_reset();
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				
				findidentify = FindMotif(*buf, *length, f512_130_identify, f512_130_identify_LEN,&off0);
				findread = FindMotif(*buf, *length, f512_130_read, f512_130_read_LEN,&off1);
				findwrite = FindMotif(*buf, *length, f512_130_write, f512_130_write_LEN,&off2);
				finderase = FindMotif(*buf, *length, f512_130_erase, f512_130_erase_LEN,&off3);
				finderasesec = FindMotif(*buf, *length, f512_130_erasesec, f512_130_erasesec_LEN,&off4);
				if(findread && findwrite)
				{
					if(findidentify)
					{
						//memcpy(findidentify, f512_130_MNidentify, f512_130_newidentify_LEN);
						ModifyFlash(off0, f512_130_MNidentify, f512_130_newidentify_LEN);
					}
						
					if(finderase)
					{
						//memcpy(finderase, f512_130_return0, f512_130_return0_LEN);
						ModifyFlash(off3, f512_130_return0, f512_130_return0_LEN);
					}
						

					if(finderasesec)
					{
						//memcpy(finderasesec, f512_130_return0, f512_130_return0_LEN);
						ModifyFlash(off4, f512_130_return0, f512_130_return0_LEN);
					}
					//memcpy(findread, f512_130_newread, f512_130_newread_LEN);
					ModifyFlash(off1, f512_130_newread, f512_130_newread_LEN);
					//memcpy(findwrite, f512_130_newwrite, f512_130_newwrite_LEN);
					ModifyFlash(off2, f512_130_newwrite, f512_130_newwrite_LEN);
				}
				
			}
		}
		//gl_PatchFlag=1;
		//g_fpos=18;
	//MWin_DrawClient(WM_GBAProgress);
	//MWin_TransWindow(WM_GBAProgress);	
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH1M_V", 9,&off);
	else if(*dwSaverType==FLASH1M_TYPE)
	{
		u8  *find1M1,*find1M2,*find1M3,*find1M4,*find1M5,*find1M6;
		find1M1=NULL;find1M2=NULL;find1M3=NULL;find1M4=NULL;find1M5=NULL;find1M6=NULL;
		_consolePrintf("Find FLASH1M_V,off=%x\n",off);
	
		if(find)
		{
			_consolePrintf("Enter FLASH1M_V Patch...\n");
			if(((s32)find)>=0xA000000)
			{
				//page switch
				CloseNorWrite();
				Enable_Arm9DS();
				SetRompage(768);
				OpenNorWrite();
				SetSerialMode();
				chip_reset();
				b512=true;
				find-=0x2000000;
				if(gl_ShakeID==0x89168916)
				{
						*((vuint16 *)(find)) = 0x50 ;
						*((vuint16 *)(find+0x1000*2)) = 0x50 ;
						*((vuint16 *)(find)) = 0xFF ;
						*((vuint16 *)(find+0x1000*2)) = 0xFF ;
				}
				chip_reset();
			}
			else
			{
				if(gl_ShakeID==0x89168916)
				{
						*((vuint16 *)(find)) = 0x50 ;
						*((vuint16 *)(find+0x1000*2)) = 0x50 ;
						*((vuint16 *)(find)) = 0xFF ;
						*((vuint16 *)(find+0x1000*2)) = 0xFF ;
				}	
			}
		
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536*2;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			_consolePrintf("find[9]=%x\n",(char)find[9]);
			_consolePrintf("find[10]=%x\n",(char)find[10]);
			_consolePrintf("find[11]=%x\n",(char)find[11]);
			
			if((char)find[9]=='1'&&(char)find[10]=='0'&&(char)find[11]=='3')
			{//special 口袋妖怪
				if(b512==true)
				{
					CloseNorWrite();
					Enable_Arm9DS();
					SetRompage(0);
					OpenNorWrite();
					SetSerialMode();
					chip_reset();
					b512=false;
					find+=0x2000000;
					chip_reset();
				}
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				s32 off5=0;
				s32 off6=0;
				s32 off7=0;
				s32 off8=0;
				s32 off9=0;
				bool b1M3=false;
				find1M1 = FindMotif(*buf, *length, f1M_102_orig1, f1M_102_orig1_LEN,&off0);
				find1M2 = FindMotif(*buf, *length, f1M_102_orig2, f1M_102_orig2_LEN,&off1);
				find1M3 = FindMotif(*buf, *length, f1M_102_orig3, f1M_102_orig3_LEN,&off2);
				if(!find1M3)
				{
					find1M3 = FindMotif(*buf, *length, f1M_102_orig3_1, f1M_102_orig3_LEN,&off3);
					b1M3 = true;
				}	
				find1M4 = FindMotif(*buf, *length, f1M_102_orig4, f1M_102_orig4_LEN,&off4);
				find1M5 = FindMotif(*buf, *length, f1M_102_orig5, f1M_102_orig5_LEN,&off5);
				find1M6 = FindMotif(*buf, *length, f1M_102_orig6, f1M_102_orig6_LEN,&off6);
				finderase = FindMotif(*buf, *length, f512_130_erase, f512_130_erase_LEN,&off7);
				finderasesec = FindMotif(*buf, *length, f512_130_erasesec, f512_130_erasesec_LEN,&off8);
				findidentify = FindMotif(*buf,*length,flash1M103_identify,flash1M103_identify_LEN,&off9);
				
				_consolePrintf("find1M1=0x%x  find1M2=%x\n",find1M1,find1M2);
				_consolePrintf("find1M3=0x%x  find1M4=%x\n",find1M3,find1M4);
				_consolePrintf("find1M5=0x%x  find1M6=%x\n",find1M5,find1M6);
				_consolePrintf("finderase=0x%x  finderasesec=%x\n",finderase,finderasesec);
				_consolePrintf("findidentify=0x%x  \n",findidentify);
					
				if(find1M4 && find1M5&&find1M6)
				{
					f1M_102_repl4[0x24] = 0x00 ;
					f1M_102_repl4[0x25] = 0x01 ;
					f1M_102_repl5[0x2c] = 0x1b;
					f1M_102_repl5[0x2d] = 0x01;
					f1M_102_repl6[0x30] = 0x24;
					f1M_102_repl6[0x31] = 0x01;
					f1M_102_repl4[0x26] = saverOffset ;
					f1M_102_repl5[0x2e] = saverOffset ;
					f1M_102_repl6[0x32] = saverOffset ;
					if(find1M1)
					{
						//memcpy(find1M1, f1M_102_repl1, f1M_102_repl1_LEN);
						ModifyFlash(off0, f1M_102_repl1, f1M_102_repl1_LEN);
					}
					if(find1M2)
					{
						//memcpy(find1M2, f1M_102_repl2, f1M_102_repl2_LEN);
						ModifyFlash(off1, f1M_102_repl2, f1M_102_repl2_LEN);
					}
					if(find1M3)
					{
						//memcpy(find1M3, f1M_102_repl3, f1M_102_repl3_LEN);
						if(b1M3==true)	
							ModifyFlash(off3, f1M_102_repl3, f1M_102_repl3_LEN);
						else 
							ModifyFlash(off2, f1M_102_repl3, f1M_102_repl3_LEN);
					}
					//memcpy(find1M4, f1M_102_repl4, f1M_102_repl4_LEN);
					ModifyFlash(off4, f1M_102_repl4, f1M_102_repl4_LEN);
					//memcpy(find1M5, f1M_102_repl5, f1M_102_repl5_LEN);
					ModifyFlash(off5, f1M_102_repl5, f1M_102_repl5_LEN);
					//memcpy(find1M6, f1M_102_repl6, f1M_102_repl6_LEN);
					ModifyFlash(off6, f1M_102_repl6, f1M_102_repl6_LEN);
					if(finderase)
					{
						//memcpy(finderase, f512_130_return0, f512_130_return0_LEN);
						ModifyFlash(off7, f512_130_return0, f512_130_return0_LEN);
					}
					if(finderasesec)
					{
						//memcpy(finderasesec, f512_130_return0, f512_130_return0_LEN);	
						ModifyFlash(off8, f512_130_return0, f512_130_return0_LEN);	
					}
					if(findidentify)
					{
						//memcpy(findidentify,flash1M103_new_identify,flash1M103_new_identify_LEN);
						ModifyFlash(off9,flash1M103_new_identify,flash1M103_new_identify_LEN);
					}
						
				}
			}
			else if((char)find[9]=='1'&&(char)find[10]=='0'&&(char)find[11]=='2')
			{
				do{
					if(b512==true)
					{
						CloseNorWrite();
						Enable_Arm9DS();
						SetRompage(0);
						OpenNorWrite();
						SetSerialMode();
						chip_reset();
						b512=false;
						if(gl_ShakeID==0x89168916)
						{
								*((vuint16 *)(find)) = 0x50 ;
								*((vuint16 *)(find+0x1000*2)) = 0x50 ;
								*((vuint16 *)(find)) = 0xFF ;
								*((vuint16 *)(find+0x1000*2)) = 0xFF ;
						}
						find+=0x2000000;
						chip_reset();
					}
					else
					{
						if(gl_ShakeID==0x89168916)
						{
								*((vuint16 *)(find)) = 0x50 ;
								*((vuint16 *)(find+0x1000*2)) = 0x50 ;
								*((vuint16 *)(find)) = 0xFF ;
								*((vuint16 *)(find+0x1000*2)) = 0xFF ;
						}	
					}
					s32 off0=0;
					s32 off1=0;
					s32 off2=0;
					s32 off3=0;
					s32 off4=0;
					s32 off5=0;
					s32 off6=0;
					s32 off7=0;
					s32 off8=0;
					s32 off9=0;
				
					findidentify = FindMotif(*buf,*length,flash1M103_identify,flash1M103_identify_LEN,&off0);
					findrampage = FindMotif(*buf,*length,flash1M103_Setrampage,flash1M103_Setrampage_LEN,&off1);
					finderase = FindMotif(*buf,*length,flash1M103_Erase,flash1M103_Erase_LEN,&off3);
					find1M5 = FindMotif(*buf,*length,flash1M103_Program,flash1M103_Program_LEN,&off4);
					findwrite = FindMotif(*buf,*length,flash1M103_Write,flash1M103_Write_LEN,&off5);
					flash1M103_new_Erase[0x24] = 0x00 ;
					flash1M103_new_Erase[0x25] = 0x01 ;
					flash1M103_new_Write[0x30] = 0x24;
					flash1M103_new_Write[0x31] = 0x01;
					flash1M103_new_Erase[0x26] = saverOffset ;
					flash1M103_new_Write[0x32] = saverOffset ;
					if(finderase&&findrampage)
					{
						if(findidentify)
						{
							//memcpy(findidentify,flash1M103_new_identify,flash1M103_new_identify_LEN);	
							ModifyFlash(off0,flash1M103_new_identify,flash1M103_new_identify_LEN);	
						}
						if(findrampage)
						{
							//memcpy(findrampage,flash1M103_new_Setrampage,flash1M103_new_Setrampage_LEN);
							ModifyFlash(off1,flash1M103_new_Setrampage,flash1M103_new_Setrampage_LEN);
						}
						if(finderase)
						{
							//memcpy(finderase,flash1M103_new_Erase,flash1M103_new_Erase_LEN);
							ModifyFlash(off3,flash1M103_new_Erase,flash1M103_new_Erase_LEN);
						}
							
						if(find1M5)
						{
							//memcpy(find1M5,flash1M103_new_Program,flash1M103_new_Program_LEN);
							ModifyFlash(off4,flash1M103_new_Program,flash1M103_new_Program_LEN);
						}
						
						if(findwrite)
						{
							//memcpy(findwrite,flash1M103_new_Write,flash1M103_new_Write_LEN);
							ModifyFlash(off5,flash1M103_new_Write,flash1M103_new_Write_LEN);
						}
						
					}
				}while(findidentify&&findrampage&&finderase&&findwrite);
				
			}
			
		}
		//gl_PatchFlag=1;
		//g_fpos=18;
		//MWin_DrawClient(WM_GBAProgress);
		//MWin_TransWindow(WM_GBAProgress);	
	}		
	SetEraseProgressVal(140);
	UpdataWindows();
	
	dwSaverType = &SaverType;
//	_consolePrintf("dwSaverType=0x%x\n",*dwSaverType);
	
	if(*dwSaverType == UNKNOW)
	{
		*dwSaveSize = 0;
		return -1;
	}
	else
	{
		if((*dwSaveSize == 0) || (*dwSaveSize >= 0x40000))
		{
			_consolePrintf("\n SaverType and SaveSize not match ! \n");
			*dwSaveSize = 0x0FFFFFFF; 
			return -2;
		}
	}
	return 1;
}
/***************************************PsRam Start*******************************************************************/


u8*	FindMotifPsRam(u8 *Buffer, s32 BufferSize, u8 *Motif, s32 MotifSize,s32 *off)
{
	bool b512=false;
	u8* find = Buffer;
	u8* pTemp = Buffer;
	while(find)
	{
		find = (u8* )memchr(Buffer,Motif[0],BufferSize-MotifSize);
		if (find)
		{
			if (memcmp(find,Motif,MotifSize) == 0)
			{
				break;
			}
		}
		BufferSize -= find - Buffer + 1;
		Buffer = find + 1;
	}
	*off=find-pTemp;
	return find;
}


u8* FindSaveTypePsRam(u8* Buffer,s32 BufferSize,SAVERTYPE *ptype)
{
	s32 loop;
	u8* find = Buffer;
	
	u8 	sramsign[]="SRAM_";
	u16 sramsize=5;
	u8 	eepromsign[]="EEPROM_V";
	u16 eepromsize=8;
	u8  falshVSign[]="FLASH_V";
	u16 flashVSize=7;
	u8  flash512VSign[]="FLASH512_V";
	u16 flash512Size=10;
	u8  falsh1MSign[]="FLASH1M_V";
	u16 flash1MSize=9;
	
	_consolePrintf("Buffer=%x\n",Buffer);
	_consolePrintf("find=%x\n",(s32)find);
	_consolePrintf("&find=%x\n",&find);
	_consolePrintf("BufferSize=%x\n",BufferSize);
	_consolePrintf("find[0]=0x%x 0x%x 0%x 0x%x\n",find[0],find[1],find[2],find[3]);
	
	for(loop=0;loop<=BufferSize;loop++)
	{
		
		if(find[loop]==sramsign[0])
		{
			_consolePrintf("sramsign\n");
			if(memcmp(&find[loop],sramsign,sramsize) == 0)
			{
				*ptype= SRAM_TYPE;
				return &find[loop];
			}
		}
		if(find[loop]==eepromsign[0])
		{
			_consolePrintf("eepromsign\n");
			if(memcmp(&find[loop],eepromsign,eepromsize) == 0)
			{
				*ptype= EEPROM_TYPE;
				return &find[loop];
			}
		}
		if(find[loop]==falshVSign[0])
		{
			_consolePrintf("falshVSign\n");
			if(memcmp(&find[loop],falshVSign,flashVSize) == 0)
			{
				*ptype= FLASH_TYPE;
				return &find[loop];
			}
		}
		if(find[loop]==flash512VSign[0])
		{
			_consolePrintf("flash512VSign\n");
			if(memcmp(&find[loop],flash512VSign,flash512Size) == 0)
			{
				*ptype= FLASH512_TYPE;
				return &find[loop];
			}
		}
		if(find[loop]==falsh1MSign[0])
		{
			_consolePrintf("falsh1MSign\n");
			if(memcmp(&find[loop],falsh1MSign,flash1MSize) == 0)
			{
				*ptype= FLASH1M_TYPE;
				_consolePrintf("loop=0x%x\n",loop);
				_consolePrintf("find[loop]=0x%x 0x%x 0%x 0x%x\n",find[loop],find[loop+1],find[loop+2],find[loop+3]);
				return &find[loop];
			}
		}
	}
}



void WritePSramBK(u8* address,u8* data,uint32 length )
{
	//backup change 
	GBAPatchData *pCur=m_pGBAPatchData;
	GBAPatchData *pTemp;
	if(pCur==NULL)
	{
		pTemp=(GBAPatchData *)safemalloc(sizeof(GBAPatchData)); 
		pTemp->pNext=NULL;
		pTemp->sign[0]='G';pTemp->sign[1]='B';pTemp->sign[2]='A';pTemp->sign[3]='\0';
		pTemp->patchOff=(s32)(address-0x8800000);
		pTemp->patchdatasize=length;
		pTemp->pData=(u8 *)safemalloc(length);
		memcpy(pTemp->pData,data,length);
		pCur=pTemp;
		g_GBAPatchDataHead.dwPatchNum++;
		m_pGBAPatchData=pCur;
		_consolePrintf("pCur->patchOff=%x\n",pCur->patchOff);
		_consolePrintf("pCur->patchdatasize=%x\n",pCur->patchdatasize);
	}
	else
	{
		while(pCur->pNext!=NULL)
		{
			pCur=pCur->pNext;
		}
		pTemp=(GBAPatchData *)safemalloc(sizeof(GBAPatchData)); 
		pTemp->pNext=NULL;
		pTemp->sign[0]='G';pTemp->sign[1]='B';pTemp->sign[2]='A';pTemp->sign[3]='\0';
		pTemp->patchOff=(s32)(address-0x8800000);
		pTemp->patchdatasize=length;
		pTemp->pData=(u8 *)safemalloc(length);
		memcpy(pTemp->pData,data,length);
		pCur->pNext=pTemp;
		g_GBAPatchDataHead.dwPatchNum++;
		_consolePrintf("pCur->patchOff=%x\n",pCur->patchOff);
		_consolePrintf("pCur->patchdatasize=%x\n",pCur->patchdatasize);
	}
	_consolePrintf("m_pGBAPatchData=%x\n",m_pGBAPatchData);
	//
	WritePSram(address,data,length);
}
s32 SaverPatchPsRam(u8 **buf,s32 *length,s32 saverOffset,SAVERTYPE *dwSaverType,s32 *dwSaveSize)
{
	g_GBAPatchDataHead.SaverType=UNKNOW;
	g_GBAPatchDataHead.dwSaveSize=0;
	g_GBAPatchDataHead.dwPatchNum=0;
	GBAPatchData *p=m_pGBAPatchData;
	while(p)
	{
		p=m_pGBAPatchData->pNext;
		if(m_pGBAPatchData)
		{
			if(m_pGBAPatchData->pData)
				safefree(m_pGBAPatchData->pData);
			safefree( m_pGBAPatchData);
			m_pGBAPatchData=NULL;
			m_pGBAPatchData=p;
		}
	}
	//GBAPatchData     *pGBAPatchData;

	s32 off=0;
	u8 *find, *findread, *findwrite, *findidentify, *finderase, *finderasesec,*findrampage;
	s32 rvalue = 0;
	SAVERTYPE SaverType=UNKNOW;
	bool b512=false;
		
//	*dwSaverType = UNKNOW ;
	dwSaverType=&SaverType;
	rvalue = 0;
	
	_consolePrintf("*buf=0x%x\n",*buf);
	TotalOff=((s32)*buf)-0x8000000;
	_consolePrintf("*buf=0x%x\n",*buf);
	_consolePrintf("TotalOff=0x%x\n",TotalOff);
	//find = FindMotif(*buf, *length, (u8*)"SRAM_", 5,&off);
	
	find= FindSaveTypePsRam(*buf,*length,dwSaverType);
	//gl_PatchFlag=1;
	//g_fpos=12;
	//MWin_DrawClient(WM_GBAProgress);
///	MWin_TransWindow(WM_GBAProgress);
	
	SetEraseProgressVal(80);
	UpdataWindows(); 
	
	_consolePrintf("find=%x\n",find);
	_consolePrintf("dwSaverType=0x%x\n",*dwSaverType);
	_consolePrintf("End FindSaveType \n");
	_consolePrintf("TotalOff=0x%x\n",TotalOff);
	
	if(*dwSaverType==SRAM_TYPE)
	{
		_consolePrintf("here 1\n");
		if(find)
		{
//			*dwSaverType = SRAM_TYPE ; //表示存档类型为sram
			*dwSaveSize = 32768;
			SaverType=SRAM_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=SRAM_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
		}
//	gl_PatchFlag=1;
//	g_fpos=18;
//	MWin_DrawClient(WM_GBAProgress);
//	MWin_TransWindow(WM_GBAProgress);	
	}
	//find = FindMotif(*buf, *length, (u8*)"EEPROM_V", 8,&off);
	else if(*dwSaverType==EEPROM_TYPE)
	{
		_consolePrintf("here 2\n");
		if(find)
		{
//			*dwSaverType = EEPROM_TYPE ;
			*dwSaveSize = 8192;
			SaverType=EEPROM_TYPE;			
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=EEPROM_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			_consolePrintf("here 3\n");
			if((char)find[8]=='1' && (char)find[9]=='2' &&(char)find[10] =='4')
			{
			
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				findread = FindMotifPsRam(*buf, *length, e12x_read, e12x_read_LEN,&off0);
				findwrite = FindMotifPsRam(*buf, *length, e124_write, e12x_write_LEN,&off1);
				if(!findwrite)
					findwrite = FindMotifPsRam(*buf, *length, e124_write2, 0x20,&off2);
				findidentify = FindMotifPsRam(*buf, *length, e12x_identify, e12x_identify_LEN,&off3);
				
				if(findread && findwrite)
				{
					if(findidentify)
					{
						WritePSramBK(findidentify+0x36, e12x_newidentify, e12x_newidentify_LEN);
						//ModifyFlash(off3+0x36,e12x_newidentify,e12x_newidentify_LEN);
					}	
					WritePSramBK(findread, e12x_newread, e12x_newread_LEN);
					//ModifyFlash(off0,e12x_newread,e12x_newread_LEN);
					WritePSramBK(findwrite, e12x_newwrite, e12x_newwrite_LEN);
				/*	if(off2!=0)
					{
						ModifyFlash(off2,e12x_newwrite,e12x_newwrite_LEN);
					}
					else if(off1!=0)
					{
						ModifyFlash(off1,e12x_newwrite,e12x_newwrite_LEN);
					}*/
				
				}
				
			}
			else if((char)find[8]=='1' && (char)find[9]=='2')
			{
				
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				_consolePrintf("here 4\n");
				findread = FindMotifPsRam(*buf, *length, e12x_read, e12x_read_LEN,&off0);
				_consolePrintf("off0=0x%x\n",off0);
				findwrite = FindMotifPsRam(*buf, *length, e12x_write, e12x_write_LEN,&off1);
				findidentify = FindMotifPsRam(*buf, *length, e12x_identify, e12x_identify_LEN,&off2);
				_consolePrintf("findread=0x%x  *findread=%x\n",findread,*findread);
				_consolePrintf("findwrite=0x%x\n",findwrite);
				_consolePrintf("findidentify=0x%x\n",findidentify);
				if(findread && findwrite)
				{
					if(findidentify)
					{
						memcpy(findidentify+0x36, e12x_newidentify, e12x_newidentify_LEN);
						//ModifyFlash(off2+0x36,e12x_newidentify,e12x_newidentify_LEN);
					}		
					WritePSramBK(findread, e12x_newread, e12x_newread_LEN);
					//ModifyFlash(off0,e12x_newread,e12x_newread_LEN);
					WritePSramBK(findwrite, e12x_newwrite, e12x_newwrite_LEN);
					//ModifyFlash(off1,e12x_newwrite,e12x_newwrite_LEN);
				}
			}
			else if((char)find[8]=='1' && (char)find[9]=='1')
			{
				_consolePrintf("\n super mario");
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				findread = FindMotifPsRam(*buf, *length, e11x_read, e11x_read_LEN,&off0);
				findwrite = FindMotifPsRam(*buf, *length, e11x_write, e11x_write_LEN,&off1);
				_consolePrintf("findread=0x%x\n",findread);
				_consolePrintf("findwrite=0x%x\n",findwrite);
				if(findread && findwrite)
				{
					u8 *pTemp = (u8 *) safemalloc(256) ;
				//	int find2 = (int)(findread - *buf)+TotalOff; find2=off0
					//int find3 = (int)(findwrite - *buf)+TotalOff; find3=off1
					u8 st[10], pr[188];
					memcpy(st,e11x_newread,e11x_newread_LEN);
					memcpy(pr,e11x_code,e11x_code_LEN);
				//	pr[184] = (find2-TotalOff) + 0x21;
				//	pr[186] = (find2-TotalOff) >> 16;
					
					pr[184] = (int)(findread - *buf)+ 0x21;
					pr[186] = ((int)(findread - *buf))>> 16;
					
					// _consolePrintf("find3=0x%x\n",find3);
					//ModifyFlash(find3,e11x_newwrite,e11x_newwrite_LEN);
					WritePSramBK(findwrite,e11x_newwrite,e11x_newwrite_LEN);
					memset(pTemp,0xFF,256);
					findrampage=FindMotifPsRam(*buf, *length, pTemp,256,&off3);
					_consolePrintf("findrampage=0x%x\n",findrampage);
					if(!findrampage)
					{
						memset(pTemp,0,256);
						findrampage=FindMotifPsRam(*buf, *length, pTemp,256,&off4);
					}

					if(findrampage)
					{
						u32 endf;
						_consolePrintf("*buf=0x%x\n",*buf);
						endf=findrampage-*buf;
						_consolePrintf("endf=0x%x\n",endf);
						endf=(endf +0x10)&0xFFFFF0;
						_consolePrintf("endf=0x%x\n",endf);
						if(((*buf)[endf-1] != 0xff) && ((*buf)[endf-1] != 0xcd))
							pr[185] = ((int)(findread - *buf)+ 0x1f) >> 8;
						else
							pr[185] = ((int)(findread - *buf)) >> 8;

						st[4] = ((char)(endf)&0xFF)+1;
						st[5] = (char)((endf) >> 8)&0xFF;
						st[6] = (char)((endf) >> 16)&0xFF;
						
						
						_consolePrintf("endf=0x%x\n",endf);
						//ModifyFlash(find2,st,10);
					    WritePSramBK(findread,st,10);
						//ModifyFlash(endf,pr,188);
					    WritePSramBK((u8*)((*buf)+endf),pr,188);	
											
					}
					if(pTemp)
					{
						safefree(pTemp);
					}
					
					
				}
			}
	
		}
	//gl_PatchFlag=1;
	//g_fpos=18;
	//MWin_DrawClient(WM_GBAProgress);
	//MWin_TransWindow(WM_GBAProgress);	
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH_V", 7,&off);
	else if(*dwSaverType==FLASH_TYPE)
	{
		_consolePrintf("Find FLASH_V\n");
		if(find)
		{
			
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			_consolePrintf("\n dwSaverType=%x",*dwSaverType);
			_consolePrintf("\n dwSaveSize=%x",*dwSaveSize);			
			if((char)find[7]=='1' && (char)find[8]=='2' && (char)find[9]>'1')
			{
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				
				findread = FindMotifPsRam(*buf, *length, f12x_read, f12x_read_LEN,&off0);
				findwrite = FindMotifPsRam(*buf, *length, f12x_write, f12x_write_LEN,&off1);
				findidentify = FindMotifPsRam(*buf, *length, f12x_identify, f12x_identify_LEN,&off2);
				finderase = FindMotifPsRam(*buf, *length, f12x_erase, f12x_erase_LEN,&off3);
				finderasesec = FindMotifPsRam(*buf, *length, f12x_erasesec, f12x_erasesec_LEN,&off4);

				if(findread && findwrite)
				{
					if(findidentify)
					{
						WritePSramBK(findidentify, f12x_MNidentify, f12x_newidentify_LEN);
						//ModifyFlash(off2,f12x_MNidentify,f12x_newidentify_LEN);
					}
					if(finderase)
					{
						WritePSramBK(finderase, f12x_return0, f12x_return0_LEN);
						//ModifyFlash(off3,f12x_return0, f12x_return0_LEN);
					}
					
					if(finderasesec)
					{
						WritePSramBK(finderasesec, f12x_return0, f12x_return0_LEN);
						//ModifyFlash(off4, f12x_return0, f12x_return0_LEN);
					}		
					WritePSramBK(findread, f12x_newread, f12x_newread_LEN);
					//ModifyFlash(off0, f12x_newread, f12x_newread_LEN);
					WritePSramBK(findwrite, f12x_newwrite, f12x_newwrite_LEN);
					//ModifyFlash(off1, f12x_newwrite, f12x_newwrite_LEN);
					
				}
			}
			if((char)find[7]=='1' && (char)find[8]=='2' && (char)find[9]<'2')
			{
				_consolePrintf("\nThis Here\n");
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				
				findwrite = FindMotifPsRam(*buf, *length, f121_write, f121_write_LEN,&off0);
				findidentify = FindMotifPsRam(*buf, *length, f121_identify, f121_identify_LEN,&off1);
				finderase = FindMotifPsRam(*buf, *length, f121_erase, f121_erase_LEN,&off2);
				finderasesec = FindMotifPsRam(*buf, *length, f121_erasesec, f121_erasesec_LEN,&off3);
				_consolePrintf("\nfindwrite=%x",findwrite);
				_consolePrintf("\nfindidentify=%x",findidentify);
				_consolePrintf("\nfinderase=%x",finderase);
				_consolePrintf("\nfinderasesec=%x",finderasesec);
				_consolePrintf("\nvalue=%x",findwrite&&findread);
							
				if(findwrite)
				{
						_consolePrintf("\nThis Here11111\n");
					if(findidentify)
					{
						_consolePrintf("\nThis Here1");
						WritePSramBK(findidentify, f121_newidentify, f121_newidentify_LEN);
						//ModifyFlash(off1, f121_newidentify, f121_newidentify_LEN);
					}
					if(finderase)
					{
						_consolePrintf("\nThis Here2");
						WritePSramBK(finderase, f121_return0, f121_return0_LEN);
						//ModifyFlash(off2, f121_return0, f121_return0_LEN);
					}
						
					if(finderasesec)
					{
						_consolePrintf("\nThis Here3");
						WritePSramBK(finderasesec, f121_return0, f121_return0_LEN);
						//ModifyFlash(off3, f121_return0, f121_return0_LEN);
					}
					_consolePrintf("\nThis Here4");
					WritePSramBK(findwrite, f121_newwrite, f121_newwrite_LEN);
					//ModifyFlash(off0, f121_newwrite, f121_newwrite_LEN);
				}
				
			}
		}
//	gl_PatchFlag=1;
//	g_fpos=18;
//	MWin_DrawClient(WM_GBAProgress);
//	MWin_TransWindow(WM_GBAProgress);
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH512_V", 10,&off);
	else if(*dwSaverType==FLASH512_TYPE)
	{
		_consolePrintf("Find FLASH512_V\n");
	
		{
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			if((char)find[10]=='1'&& (char)find[11]=='3')
			{
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				
				findidentify = FindMotifPsRam(*buf, *length, f512_130_identify, f512_130_identify_LEN,&off0);
				findread = FindMotifPsRam(*buf, *length, f512_130_read, f512_130_read_LEN,&off1);
				findwrite = FindMotifPsRam(*buf, *length, f512_130_write, f512_130_write_LEN,&off2);
				finderase = FindMotifPsRam(*buf, *length, f512_130_erase, f512_130_erase_LEN,&off3);
				finderasesec = FindMotifPsRam(*buf, *length, f512_130_erasesec, f512_130_erasesec_LEN,&off4);
				if(findread && findwrite)
				{
					if(findidentify)
					{
						WritePSramBK(findidentify, f512_130_MNidentify, f512_130_newidentify_LEN);
						//ModifyFlash(off0, f512_130_MNidentify, f512_130_newidentify_LEN);
					}
						
					if(finderase)
					{
						WritePSramBK(finderase, f512_130_return0, f512_130_return0_LEN);
						//ModifyFlash(off3, f512_130_return0, f512_130_return0_LEN);
					}
						

					if(finderasesec)
					{
						WritePSramBK(finderasesec, f512_130_return0, f512_130_return0_LEN);
						//ModifyFlash(off4, f512_130_return0, f512_130_return0_LEN);
					}
					WritePSramBK(findread, f512_130_newread, f512_130_newread_LEN);
					//ModifyFlash(off1, f512_130_newread, f512_130_newread_LEN);
					WritePSramBK(findwrite, f512_130_newwrite, f512_130_newwrite_LEN);
					//ModifyFlash(off2, f512_130_newwrite, f512_130_newwrite_LEN);
				}
				
			}
		}
//		gl_PatchFlag=1;
//		g_fpos=18;
//	MWin_DrawClient(WM_GBAProgress);
//	MWin_TransWindow(WM_GBAProgress);
	}
	//find = FindMotif(*buf, *length, (u8*)"FLASH1M_V", 9,&off);
	else if(*dwSaverType==FLASH1M_TYPE)
	{
		u8  *find1M1,*find1M2,*find1M3,*find1M4,*find1M5,*find1M6;
		find1M1=NULL;find1M2=NULL;find1M3=NULL;find1M4=NULL;find1M5=NULL;find1M6=NULL;
		_consolePrintf("Find FLASH1M_V,off=%x\n",off);
		
//			*dwSaverType = FLASH_TYPE ;
			*dwSaveSize = 65536*2;
			SaverType=FLASH_TYPE;
			dwSaverType=&SaverType;
			g_GBAPatchDataHead.SaverType=FLASH_TYPE;
			g_GBAPatchDataHead.dwSaveSize=*dwSaveSize;
			_consolePrintf("find[9]=%x\n",(char)find[9]);
			_consolePrintf("find[10]=%x\n",(char)find[10]);
			_consolePrintf("find[11]=%x\n",(char)find[11]);
			
			if((char)find[9]=='1'&&(char)find[10]=='0'&&(char)find[11]=='3')
			{//special 口袋妖怪
			
				s32 off0=0;
				s32 off1=0;
				s32 off2=0;
				s32 off3=0;
				s32 off4=0;
				s32 off5=0;
				s32 off6=0;
				s32 off7=0;
				s32 off8=0;
				s32 off9=0;
				bool b1M3=false;
				find1M1 = FindMotifPsRam(*buf, *length, f1M_102_orig1, f1M_102_orig1_LEN,&off0);
				find1M2 = FindMotifPsRam(*buf, *length, f1M_102_orig2, f1M_102_orig2_LEN,&off1);
				find1M3 = FindMotifPsRam(*buf, *length, f1M_102_orig3, f1M_102_orig3_LEN,&off2);
				if(!find1M3)
				{
					find1M3 = FindMotifPsRam(*buf, *length, f1M_102_orig3_1, f1M_102_orig3_LEN,&off3);
					b1M3 = true;
				}	
				find1M4 = FindMotifPsRam(*buf, *length, f1M_102_orig4, f1M_102_orig4_LEN,&off4);
				find1M5 = FindMotifPsRam(*buf, *length, f1M_102_orig5, f1M_102_orig5_LEN,&off5);
				find1M6 = FindMotifPsRam(*buf, *length, f1M_102_orig6, f1M_102_orig6_LEN,&off6);
				finderase = FindMotifPsRam(*buf, *length, f512_130_erase, f512_130_erase_LEN,&off7);
				finderasesec = FindMotifPsRam(*buf, *length, f512_130_erasesec, f512_130_erasesec_LEN,&off8);
				findidentify = FindMotifPsRam(*buf,*length,flash1M103_identify,flash1M103_identify_LEN,&off9);
				
				_consolePrintf("find1M1=0x%x  find1M2=%x\n",find1M1,find1M2);
				_consolePrintf("find1M3=0x%x  find1M4=%x\n",find1M3,find1M4);
				_consolePrintf("find1M5=0x%x  find1M6=%x\n",find1M5,find1M6);
				_consolePrintf("finderase=0x%x  finderasesec=%x\n",finderase,finderasesec);
				_consolePrintf("findidentify=0x%x  \n",findidentify);
					
				if(find1M4 && find1M5&&find1M6)
				{
					f1M_102_repl4[0x24] = 0x00 ;
					f1M_102_repl4[0x25] = 0x01 ;
					f1M_102_repl5[0x2c] = 0x1b;
					f1M_102_repl5[0x2d] = 0x01;
					f1M_102_repl6[0x30] = 0x24;
					f1M_102_repl6[0x31] = 0x01;
					f1M_102_repl4[0x26] = saverOffset ;
					f1M_102_repl5[0x2e] = saverOffset ;
					f1M_102_repl6[0x32] = saverOffset ;
					if(find1M1)
					{
						WritePSramBK(find1M1, f1M_102_repl1, f1M_102_repl1_LEN);
						//ModifyFlash(off0, f1M_102_repl1, f1M_102_repl1_LEN);
					}
					if(find1M2)
					{
						WritePSramBK(find1M2, f1M_102_repl2, f1M_102_repl2_LEN);
						//ModifyFlash(off1, f1M_102_repl2, f1M_102_repl2_LEN);
					}
					if(find1M3)
					{
						 WritePSramBK(find1M3, f1M_102_repl3, f1M_102_repl3_LEN);
						/*if(b1M3==true)	
							ModifyFlash(off3, f1M_102_repl3, f1M_102_repl3_LEN);
						else 
							ModifyFlash(off2, f1M_102_repl3, f1M_102_repl3_LEN);*/
					}
					WritePSramBK(find1M4, f1M_102_repl4, f1M_102_repl4_LEN);
					//ModifyFlash(off4, f1M_102_repl4, f1M_102_repl4_LEN);
					WritePSramBK(find1M5, f1M_102_repl5, f1M_102_repl5_LEN);
					//ModifyFlash(off5, f1M_102_repl5, f1M_102_repl5_LEN);
					WritePSramBK(find1M6, f1M_102_repl6, f1M_102_repl6_LEN);
					//ModifyFlash(off6, f1M_102_repl6, f1M_102_repl6_LEN);
					if(finderase)
					{
						WritePSramBK(finderase, f512_130_return0, f512_130_return0_LEN);
						//ModifyFlash(off7, f512_130_return0, f512_130_return0_LEN);
					}
					if(finderasesec)
					{
						WritePSramBK(finderasesec, f512_130_return0, f512_130_return0_LEN);	
						//ModifyFlash(off8, f512_130_return0, f512_130_return0_LEN);	
					}
					if(findidentify)
					{
						WritePSramBK(findidentify,flash1M103_new_identify,flash1M103_new_identify_LEN);
						//ModifyFlash(off9,flash1M103_new_identify,flash1M103_new_identify_LEN);
					}
						
				}
			}
			else if((char)find[9]=='1'&&(char)find[10]=='0'&&(char)find[11]=='2')
			{
				do{
					
					s32 off0=0;
					s32 off1=0;
					s32 off2=0;
					s32 off3=0;
					s32 off4=0;
					s32 off5=0;
					s32 off6=0;
					s32 off7=0;
					s32 off8=0;
					s32 off9=0;
				
					findidentify = FindMotifPsRam(*buf,*length,flash1M103_identify,flash1M103_identify_LEN,&off0);
					findrampage = FindMotifPsRam(*buf,*length,flash1M103_Setrampage,flash1M103_Setrampage_LEN,&off1);
					finderase = FindMotifPsRam(*buf,*length,flash1M103_Erase,flash1M103_Erase_LEN,&off3);
					find1M5 = FindMotifPsRam(*buf,*length,flash1M103_Program,flash1M103_Program_LEN,&off4);
					findwrite = FindMotifPsRam(*buf,*length,flash1M103_Write,flash1M103_Write_LEN,&off5);
					flash1M103_new_Erase[0x24] = 0x00 ;
					flash1M103_new_Erase[0x25] = 0x01 ;
					flash1M103_new_Write[0x30] = 0x24;
					flash1M103_new_Write[0x31] = 0x01;
					flash1M103_new_Erase[0x26] = saverOffset ;
					flash1M103_new_Write[0x32] = saverOffset ;
					if(finderase&&findrampage)
					{
						if(findidentify)
						{
							WritePSramBK(findidentify,flash1M103_new_identify,flash1M103_new_identify_LEN);	
							//ModifyFlash(off0,flash1M103_new_identify,flash1M103_new_identify_LEN);	
						}
						if(findrampage)
						{
							WritePSramBK(findrampage,flash1M103_new_Setrampage,flash1M103_new_Setrampage_LEN);
							//ModifyFlash(off1,flash1M103_new_Setrampage,flash1M103_new_Setrampage_LEN);
						}
						if(finderase)
						{
							WritePSramBK(finderase,flash1M103_new_Erase,flash1M103_new_Erase_LEN);
							//ModifyFlash(off3,flash1M103_new_Erase,flash1M103_new_Erase_LEN);
						}
							
						if(find1M5)
						{
							WritePSramBK(find1M5,flash1M103_new_Program,flash1M103_new_Program_LEN);
							//ModifyFlash(off4,flash1M103_new_Program,flash1M103_new_Program_LEN);
						}
						
						if(findwrite)
						{
							WritePSramBK(findwrite,flash1M103_new_Write,flash1M103_new_Write_LEN);
							//ModifyFlash(off5,flash1M103_new_Write,flash1M103_new_Write_LEN);
						}
						
					}
				}while(findidentify&&findrampage&&finderase&&findwrite);
				
			}
//		gl_PatchFlag=1;	
//		g_fpos=18;
//		MWin_DrawC/lient(WM_GBAProgress);
//		MWin_TransWindow(WM_GBAProgress);
		}
		
	SetEraseProgressVal(140);
	UpdataWindows();		
	*dwSaverType = SaverType;
//	_consolePrintf("dwSaverType=0x%x\n",*dwSaverType);

	/*GBAPatchData *pCur=m_pGBAPatchData;
	while(pCur)
	{
		
			_consolePrintf("offset= 0x%x\n",pCur->patchOff);
			pCur=pCur->pNext;
	}
	TestPauseKey();*/
	if(*dwSaverType == UNKNOW)
	{
		*dwSaveSize = 0;
		return -1;
	}
	else
	{
		if((*dwSaveSize == 0) || (*dwSaveSize >= 0x40000))
		{
			_consolePrintf("\n SaverType and SaveSize not match ! \n");
			*dwSaveSize = 0x0FFFFFFF; 
			return -2;
		}
	}
	return 1;
	
	}


void FreeQuickPatchData()
{
	_consolePrintf("\nfree Struct");
	if(g_GBAPatchDataHead.dwPatchNum)
	{
		//释放内存
		GBAPatchData *p=m_pGBAPatchData;
		while(p)
		{
			p=m_pGBAPatchData->pNext;
			if(m_pGBAPatchData)
			{
				if(m_pGBAPatchData->pData)
					safefree(m_pGBAPatchData->pData);
				safefree(m_pGBAPatchData);
				m_pGBAPatchData=NULL;
				m_pGBAPatchData=p;
			}
		}	
	}
	_consolePrintf("\nfree Struct End");
}





