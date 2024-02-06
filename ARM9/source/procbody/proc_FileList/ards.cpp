
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



uint32 	dw_GameCounter;//有金手指的游戏个数
AR_DATA	*m_pARDataHead;
uint32	m_CheatItemCounter;
uint32	m_FirstLevelCIC;
/*
vuint32 *m_pResultBegin;
vuint32 *m_pResult;
vuint32	*m_pResultSpecial;
*/
uint32  crc32(uint32   crc,unsigned   char   *buf,   uint32   len)  
{  
	static   const   uint32   crc32_table[256]   =  
    {  
        0x00000000,   0x77073096,   0xee0e612c,   0x990951ba,   0x076dc419,  
        0x706af48f,   0xe963a535,   0x9e6495a3,   0x0edb8832,   0x79dcb8a4,  
        0xe0d5e91e,   0x97d2d988,   0x09b64c2b,   0x7eb17cbd,   0xe7b82d07,  
        0x90bf1d91,   0x1db71064,   0x6ab020f2,   0xf3b97148,   0x84be41de,  
        0x1adad47d,   0x6ddde4eb,   0xf4d4b551,   0x83d385c7,   0x136c9856,  
        0x646ba8c0,   0xfd62f97a,   0x8a65c9ec,   0x14015c4f,   0x63066cd9,  
        0xfa0f3d63,   0x8d080df5,   0x3b6e20c8,   0x4c69105e,   0xd56041e4,  
        0xa2677172,   0x3c03e4d1,   0x4b04d447,   0xd20d85fd,   0xa50ab56b,  
        0x35b5a8fa,   0x42b2986c,   0xdbbbc9d6,   0xacbcf940,   0x32d86ce3,  
        0x45df5c75,   0xdcd60dcf,   0xabd13d59,   0x26d930ac,   0x51de003a,  
        0xc8d75180,   0xbfd06116,   0x21b4f4b5,   0x56b3c423,   0xcfba9599,  
        0xb8bda50f,   0x2802b89e,   0x5f058808,   0xc60cd9b2,   0xb10be924,  
        0x2f6f7c87,   0x58684c11,   0xc1611dab,   0xb6662d3d,   0x76dc4190,  
        0x01db7106,   0x98d220bc,   0xefd5102a,   0x71b18589,   0x06b6b51f,  
        0x9fbfe4a5,   0xe8b8d433,   0x7807c9a2,   0x0f00f934,   0x9609a88e,  
        0xe10e9818,   0x7f6a0dbb,   0x086d3d2d,   0x91646c97,   0xe6635c01,  
        0x6b6b51f4,   0x1c6c6162,   0x856530d8,   0xf262004e,   0x6c0695ed,  
        0x1b01a57b,   0x8208f4c1,   0xf50fc457,   0x65b0d9c6,   0x12b7e950,  
        0x8bbeb8ea,   0xfcb9887c,   0x62dd1ddf,   0x15da2d49,   0x8cd37cf3,  
        0xfbd44c65,   0x4db26158,   0x3ab551ce,   0xa3bc0074,   0xd4bb30e2,  
        0x4adfa541,   0x3dd895d7,   0xa4d1c46d,   0xd3d6f4fb,   0x4369e96a,  
        0x346ed9fc,   0xad678846,   0xda60b8d0,   0x44042d73,   0x33031de5,  
        0xaa0a4c5f,   0xdd0d7cc9,   0x5005713c,   0x270241aa,   0xbe0b1010,  
        0xc90c2086,   0x5768b525,   0x206f85b3,   0xb966d409,   0xce61e49f,  
        0x5edef90e,   0x29d9c998,   0xb0d09822,   0xc7d7a8b4,   0x59b33d17,  
        0x2eb40d81,   0xb7bd5c3b,   0xc0ba6cad,   0xedb88320,   0x9abfb3b6,  
        0x03b6e20c,   0x74b1d29a,   0xead54739,   0x9dd277af,   0x04db2615,  
        0x73dc1683,   0xe3630b12,   0x94643b84,   0x0d6d6a3e,   0x7a6a5aa8,  
        0xe40ecf0b,   0x9309ff9d,   0x0a00ae27,   0x7d079eb1,   0xf00f9344,  
        0x8708a3d2,   0x1e01f268,   0x6906c2fe,   0xf762575d,   0x806567cb,  
        0x196c3671,   0x6e6b06e7,   0xfed41b76,   0x89d32be0,   0x10da7a5a,  
        0x67dd4acc,   0xf9b9df6f,   0x8ebeeff9,   0x17b7be43,   0x60b08ed5,  
        0xd6d6a3e8,   0xa1d1937e,   0x38d8c2c4,   0x4fdff252,   0xd1bb67f1,  
        0xa6bc5767,   0x3fb506dd,   0x48b2364b,   0xd80d2bda,   0xaf0a1b4c,  
        0x36034af6,   0x41047a60,   0xdf60efc3,   0xa867df55,   0x316e8eef,  
        0x4669be79,   0xcb61b38c,   0xbc66831a,   0x256fd2a0,   0x5268e236,  
        0xcc0c7795,   0xbb0b4703,   0x220216b9,   0x5505262f,   0xc5ba3bbe,  
        0xb2bd0b28,   0x2bb45a92,   0x5cb36a04,   0xc2d7ffa7,   0xb5d0cf31,  
        0x2cd99e8b,   0x5bdeae1d,   0x9b64c2b0,   0xec63f226,   0x756aa39c,  
        0x026d930a,   0x9c0906a9,   0xeb0e363f,   0x72076785,   0x05005713,  
        0x95bf4a82,   0xe2b87a14,   0x7bb12bae,   0x0cb61b38,   0x92d28e9b,  
        0xe5d5be0d,   0x7cdcefb7,   0x0bdbdf21,   0x86d3d2d4,   0xf1d4e242,  
        0x68ddb3f8,   0x1fda836e,   0x81be16cd,   0xf6b9265b,   0x6fb077e1,  
        0x18b74777,   0x88085ae6,   0xff0f6a70,   0x66063bca,   0x11010b5c,  
        0x8f659eff,   0xf862ae69,   0x616bffd3,   0x166ccf45,   0xa00ae278,  
        0xd70dd2ee,   0x4e048354,   0x3903b3c2,   0xa7672661,   0xd06016f7,  
        0x4969474d,   0x3e6e77db,   0xaed16a4a,   0xd9d65adc,   0x40df0b66,  
        0x37d83bf0,   0xa9bcae53,   0xdebb9ec5,   0x47b2cf7f,   0x30b5ffe9,  
        0xbdbdf21c,   0xcabac28a,   0x53b39330,   0x24b4a3a6,   0xbad03605,  
        0xcdd70693,   0x54de5729,   0x23d967bf,   0xb3667a2e,   0xc4614ab8,  
        0x5d681b02,   0x2a6f2b94,   0xb40bbe37,   0xc30c8ea1,   0x5a05df1b,  
        0x2d02ef8d  
    };  
	unsigned   char   *end;  
	crc =~crc&0xffffffff;  
	for(end=buf+len;buf<end;++buf)  
	    crc=crc32_table[(crc^*buf)&0xff]^(crc>>8);  
	return   ~crc&0xffffffff;  
} 

void PrintfARDSData();

NDSCheatInfo *FindNDSCheatInfo(char *sign,uint32 RomCRC)
{
	char *pBuf=NULL;
	uint32 ii,kk,dw_temp;
	NDSCheatInfo *pNdsCheatInfo=NULL;

	FAT_FILE *h=NULL; 
	char pStrname[256];
	snprintf(pStrname,256,DefaultDataPath "/" CheatDataFilenme);
	_consolePrintf("\n pStrname=%s",pStrname);	
	h=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(pStrname));	
	if(h==NULL)
	{
		_consolePrintf("\nOpen File usrcheat.dat fail \n");
		FAT2_fclose(h);
		return NULL;
	}
	_consolePrintf("\nOpen File/moonshl/usrcheat.dat ok \n");
	
	FAT2_fseek(h,0,SEEK_END);
	u32 FileSize=FAT2_ftell(h);
	FAT2_fseek(h,0x100,SEEK_SET);
	pBuf = (char *)safemalloc(0x1000);
	dw_temp = (FileSize-0x100)/0x1000*0x1000;
	dw_GameCounter = 0;
	for(ii=0;ii<dw_temp;ii+=0x1000)
	{
		FAT2_fread(pBuf,0x1000,1,h);		
		for(kk=0;kk<0x1000;kk+=0x10)
		{
			if(*((uint32 *)&pBuf[kk])==0x00)
				goto END;
			dw_GameCounter++;
			if((memcmp(sign,&pBuf[kk],4)==0) && (*((uint32 *)&pBuf[kk+4])==RomCRC))
			{
				//返回一个NDSCheatInfo
			
				pNdsCheatInfo = (NDSCheatInfo *)safemalloc(sizeof(NDSCheatInfo));
				memcpy(pNdsCheatInfo->sign,(char *)&pBuf[kk],4);
				pNdsCheatInfo->sign[4]='\0';
				pNdsCheatInfo->dw_CRC=*((uint32 *)&pBuf[kk+4]);
				pNdsCheatInfo->dw_StartAddress=*((uint32 *)&pBuf[kk+8]);
				_consolePrintf("pNdsCheatInfo->sign=%s\n",&pNdsCheatInfo->sign[0]);
				_consolePrintf("pNdsCheatInfo->dw_CRC=0x%4x\n",pNdsCheatInfo->dw_CRC);
				_consolePrintf("pNdsCheatInfo->dw_StartAddress=0x%4x\n",pNdsCheatInfo->dw_StartAddress);		
			goto END;
			}
		}
	}
END:
	_consolePrintf("dw_GameCounter=%d\n",dw_GameCounter);		
	if(pBuf)
	{
		safefree(pBuf);pBuf=NULL;
	}
	FAT2_fclose(h);
	return pNdsCheatInfo;
}

/*bool DeleteAR_DATA()
{
	AR_DATA *pARCheatCur;
	if(!m_pARDataHead)
		return true;
	pARCheatCur=m_pARDataHead->pARNextData;
	while(pARCheatCur!=NULL)
	{
		if(m_pARDataHead->ARCheat.pDescription)
			safefree(m_pARDataHead->ARCheat.pDescription);
		if(m_pARDataHead->pFunction)
			safefree(m_pARDataHead->pFunction);
		if(m_pARDataHead->pFunction2)
			safefree(m_pARDataHead->pFunction2);
		if(m_pARDataHead->ARCheat.pData)
		{
			safefree(m_pARDataHead->ARCheat.pData);
			m_pARDataHead->ARCheat.pData=NULL;
		}
		if(m_pARDataHead)
		{
			safefree(m_pARDataHead);
			 m_pARDataHead=NULL;
		}
		m_pARDataHead=pARCheatCur;
		pARCheatCur=pARCheatCur->pARNextData;
	}
	return true;
}*/
bool DeleteAR_DATA()
{
	AR_DATA *pARCheatCur;
	if(!m_pARDataHead)
		return true;
	pARCheatCur=m_pARDataHead->pARNextData;
	if(pARCheatCur)
	{
		do
		{
			if(m_pARDataHead->ARCheat.pDescription)
				safefree(m_pARDataHead->ARCheat.pDescription);
			if(m_pARDataHead->pFunction)
				safefree(m_pARDataHead->pFunction);
			if(m_pARDataHead->pFunction2)
				safefree(m_pARDataHead->pFunction2);
			if(m_pARDataHead->ARCheat.pData)
			{
				safefree(m_pARDataHead->ARCheat.pData);
				m_pARDataHead->ARCheat.pData=NULL;
			}
			if(m_pARDataHead)
			{
				safefree(m_pARDataHead);
				 m_pARDataHead=NULL;
			}
			if(pARCheatCur)
			{
				m_pARDataHead=pARCheatCur;
			}
			else
				break;
			pARCheatCur=pARCheatCur->pARNextData;
		}while(m_pARDataHead);
	}
	return true;
}
extern void TestPauseKey(void);
u32 gl_SumofGold;
bool GetCheatData(NDSCheatInfo *pInfo)
{
	uint32 loop;
	AR_DATA *pARCheatCur;
	char *pBuf=NULL;
	bool bOneHot=false;
	bool bSub=false;
	//读数据
	
	FAT_FILE *h=NULL; 
	char pStrname[256];
	snprintf(pStrname,256,DefaultDataPath "/" CheatDataFilenme);
	_consolePrintf("\n pStrname=%s",pStrname);
	h=FAT2_fopen_AliasForRead(ConvertFullPath_Ansi2Alias(pStrname));
	if(h==NULL)
	{	
		_consolePrintf("\nOpen File/moonshl/usrcheat.dat fail \n");
		FAT2_fclose(h);
		return NULL;
	}
	FAT2_fseek(h,0,SEEK_END);
	u32 FileSize=FAT2_ftell(h);
	uint32 SeekPos=pInfo->dw_StartAddress/0x10*0x10;
	FAT2_fseek(h,SeekPos/*pInfo->dw_StartAddress*/,SEEK_SET);
	_consolePrintf("Seek to=0x%x\n",SeekPos);
	pBuf= (char*)safemalloc(0x40000);//256K
	if(pBuf==NULL)
	{
		_consolePrintf("\nCan not alloc BUF \n");
		safefree(pBuf);
		return false;
	}
	FAT2_fread(pBuf,0x40000,1,h);
	
	DeleteAR_DATA();
	uint32 off=pInfo->dw_StartAddress-SeekPos;
	if(pInfo)
	{
		safefree(pInfo);pInfo=NULL;
	}
	//读取游戏的描述符
	loop=0;
	do
	{
		loop++;
	}while(pBuf[off+loop]!='\0');
	loop = (loop+4)/4*4;
	pARCheatCur=(AR_DATA *)safemalloc(sizeof(AR_DATA));
	pARCheatCur->pARPreData=NULL;
	pARCheatCur->bOneHot=false;
	pARCheatCur->bFolder=false;
	pARCheatCur->bSub=false;
	pARCheatCur->bExpd=false;
	pARCheatCur->SubCnt=0x00;
	pARCheatCur->ARCheat.pDescription=NULL;
	pARCheatCur->ARCheat.pData=NULL;
	pARCheatCur->ARCheat.datasize=0;
	pARCheatCur->pARNextData=NULL;

	m_pARDataHead = pARCheatCur;
	pARCheatCur->pFunction=(char*)safemalloc(loop);
	memset(pARCheatCur->pFunction,0,loop);
	memcpy(pARCheatCur->pFunction,&pBuf[off],loop);
	pARCheatCur->pFunction2=NULL;
	//取得金手指条目个数
	m_CheatItemCounter = *((uint32 *)&pBuf[off+loop]) & 0xFFFFFFF;
	_consolePrintf("m_CheatItemCounter=%d\n",m_CheatItemCounter);
	
	gl_SumofGold=m_CheatItemCounter;
	if(m_CheatItemCounter>300)
	{
		m_CheatItemCounter=300;
		gl_SumofGold=m_CheatItemCounter;
	}
	
	m_FirstLevelCIC=0x00;	
	u32 SubCnt=0x00;
	
	uint32 SubCnt_cpy = 0;
	uint32 count_cpy = 1;
	//填充数据
	off += loop+36;
	for(uint32 kk=0;kk<m_CheatItemCounter;kk++)
	{
		/*
		下面就是具体各条目，每条目的格式：
'首先是一个DWORD，表示该条目类型，如果最高字节不为0表示为folder，否则表示code，
'    当类型为folder时：XYZZZZZZ，X表示foler，Y表示：one hot，ZZZZZZ是本folder下面包括的code数量（好像XY一般是1）
'    当类型为code时：XY=0，Z的含义不明
'然后是2个字符串，分别代表该条目的Name和Note，第2个字符串后面要用\0补齐，使2个字符串长度和是4的整数倍（为了照顾32位系统？）
'如果是folder则该条目到此结束，如果是code，后面还有ARDS码段，格式为：ARDS码大小+ARDS码
'   ARDS码大小用1个DWORD表示
'   ARDS码是n个DWORD，每2个DWORD构成一行语句，二进制格式
		*/
		uint32 type=*((uint32 *)&pBuf[off]);
		uint32 X=0xF0000000 & type;
		uint32 Y=0xF000000 & type;		
		if(X==false && Y==false)
		{
			if(!bSub)
				m_FirstLevelCIC++;
			///////////////////////////////////add by cpy
			if(bSub)
			{
				_consolePrintf(" if(bSub)\n");
				_consolePrintf(" count_cpy[%d] VS SubCnt_cpy[%d]\n",count_cpy,SubCnt_cpy);
				
				if(count_cpy-1 == SubCnt_cpy)
				{
					bSub = false;
					count_cpy = 0;
					SubCnt_cpy =0;
					_consolePrintf(" bSub = false;\n");
				}
				count_cpy++;
			}
			///////////////////////////////////	
			//表示code
			AR_DATA *p = (AR_DATA *)safemalloc(sizeof(AR_DATA));
			p->bOneHot=bOneHot;
			p->bFolder=false;
			p->bUse=false;
			p->bSub=bSub;
			p->bExpd=false;
			p->SubCnt=0x00;
			p->ARCheat.pDescription=NULL;
			p->ARCheat.pData=NULL;
			p->ARCheat.datasize=0;
			p->pARNextData=NULL;
			p->pARPreData=NULL;
			//拷贝描述符号
			uint32 dwLen1=0;
			uint32 dwLen2=0;
			while(pBuf[off+4+dwLen1]!='\0')
			{
				dwLen1++;
			}
			p->pFunction=(char *)safemalloc(dwLen1+1);
			memcpy(p->pFunction,&pBuf[off+4],dwLen1);
			*(p->pFunction+dwLen1)='\0';
			dwLen1++;
			//_consolePrintf("kk=%d,dwLen1=%d\n",kk,dwLen1);
			//_consolePrintf("p->pFunction=%s\n",p->pFunction);
			//处理第二个字符串
			while(pBuf[off+4+dwLen1+dwLen2]!='\0')
			{
				dwLen2++;
			}
			if(dwLen2>0)
			{
				p->pFunction2=(char *)safemalloc(dwLen2+1);
				memcpy(p->pFunction2,&pBuf[off+4+dwLen1],dwLen2);
				*(p->pFunction2+dwLen2)='\0';
			}
			else
				p->pFunction2=NULL;
			//处理字节对齐
			dwLen1=(dwLen1+dwLen2+4)/4*4;
			p->ARCheat.datasize=*((uint32 *)&pBuf[off+4+dwLen1]);
			//填充数据
			p->ARCheat.pData=(uint32 *)safemalloc(p->ARCheat.datasize*4);
			for(uint32 jj=0;jj<p->ARCheat.datasize;jj++)
			{
				p->ARCheat.pData[jj]=*((uint32 *)&pBuf[off+4+dwLen1+4+jj*4]);
			}
			pARCheatCur->pARNextData=p;
			p->pARPreData=pARCheatCur;
			pARCheatCur=p;
			off=off+8+dwLen1+p->ARCheat.datasize*4;
		}
		else
		{
			m_FirstLevelCIC++;
			SubCnt=0x000000FF & type;
			SubCnt_cpy = SubCnt;
			count_cpy = 1;
			bSub=true;
			//表示floder
		//	AR_DATA *p = new AR_DATA;
			AR_DATA *p = (AR_DATA *)safemalloc(sizeof(AR_DATA));
			p->bOneHot=false;
			bOneHot=false;
			p->bFolder = true;
			p->bSub=false;
			p->bExpd=false;
			p->SubCnt=SubCnt;
			p->ARCheat.pDescription=NULL;
			p->ARCheat.pData=NULL;
			p->ARCheat.datasize=0;
			p->pARNextData=NULL;
			if(Y==0x1000000)
			{
				p->bOneHot=true;
				bOneHot=true;
			}
			//拷贝描述符号
			uint32 dwLen1=0;
			uint32 dwLen2=0;
			while(pBuf[off+4+dwLen1]!='\0')
			{
				dwLen1++;
			}
			p->pFunction=(char *)safemalloc(dwLen1+1);
			memset(p->pFunction,0,dwLen1+1);
			memcpy(p->pFunction,&pBuf[off+4],dwLen1);
			*(p->pFunction+dwLen1)='\0';
			dwLen1++;
			//处理第二个字符串
			while(pBuf[off+4+dwLen1+dwLen2]!='\0')
			{
				dwLen2++;
			}
			if(dwLen2>0)
			{
				//p->pFunction2=new char[dwLen2];
				p->pFunction2=(char *)safemalloc(dwLen2+1);
				memset(p->pFunction2,0,dwLen2+1);
				memcpy(p->pFunction2,&pBuf[off+4+dwLen1],dwLen2);
				*(p->pFunction2+dwLen2)='\0';
			}
			else
				p->pFunction2=NULL;
			//处理字节对齐
			dwLen1=(dwLen1+dwLen2+4)/4*4;

			pARCheatCur->pARNextData=p;
			p->pARPreData=pARCheatCur;
			pARCheatCur=p;
			off=off+4+dwLen1;
		}
	}
	safefree(pBuf);
	FAT2_fclose(h);
	//PrintfARDSData();
	return true;	
}
void PrintfARDSData()
{
	AR_DATA *pARCheatCur;
	pARCheatCur=m_pARDataHead;
	if(pARCheatCur->pARNextData!=NULL)
		pARCheatCur=pARCheatCur->pARNextData;
	_consolePrintf("m_CheatItemCounter=0x%x\n",m_CheatItemCounter);
	s32 i=0;
	do
	{
		_consolePrintf("\n %0xd %0xd %0xd %0xd \n",pARCheatCur->pFunction[0],pARCheatCur->pFunction[2],pARCheatCur->pFunction[4],pARCheatCur->pFunction[6]);
		_consolePrintf("size=%d",strlen(pARCheatCur->pFunction));
		_consolePrintf("bOne=%d",pARCheatCur->bOneHot);
		_consolePrintf(" %d bSub=%d\n",i++,pARCheatCur->bSub);
		pARCheatCur=pARCheatCur->pARNextData;
	}while(pARCheatCur!=NULL);
}









