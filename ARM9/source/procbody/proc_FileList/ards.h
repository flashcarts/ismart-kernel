#ifndef ards_h
#define ards_h

/*
'文件的1-12字节是固定内容
'文件的13-15字节应该是该库的版本号，1.0为：00 01 00 00
'后面是数据库描述字符串
'游戏列表从100h开始，结构为：一条信息4个DWORLD，分别是：GameCode、GameCRC、金手指开始地址、保留
'游戏列表结束标志为：上述4个字段都为0
'每个游戏的金手指数据格式：
'首先是一个字符串，长度小于40h且用\0补齐到4的整数倍（包括\0），是游戏说明
'然后是一个DWORD，是该游戏条目数量定义，注：尽管folder类型自身没有金手指条目，不过也算条目数量的一个，这个值要与FFFFFFFh
'然后是8个DWORD，具体含义未知
'下面就是具体各条目，每条目的格式：
'首先是一个DWORD，表示该条目类型，如果最高字节不为0表示为folder，否则表示code，
'    当类型为folder时：XYZZZZZZ，X表示foler，Y表示：one hot，ZZZZZZ是本folder下面包括的code数量（好像XY一般是1）
'    当类型为code时：XY=0，Z的含义不明
'然后是2个字符串，分别代表该条目的Name和Note，第2个字符串后面要用\0补齐，使2个字符串长度和是4的整数倍（为了照顾32位系统？）
'如果是folder则该条目到此结束，如果是code，后面还有ARDS码段，格式为：ARDS码大小+ARDS码
'   ARDS码大小用1个DWORD表示
'   ARDS码是n个DWORD，每2个DWORD构成一行语句，二进制格式
*/
typedef struct _ARCHEAT{
	char	*pDescription;//金手指描述
	uint32  datasize;//金手指大小
	uint32	*pData;//金手指条码
}ARCHEAT;

typedef struct _AR_DATA{
	char	*pFunction;//注释1
	char	*pFunction2;//注释2
	ARCHEAT ARCheat;
	bool	bFolder;//是否是文件夹
	bool	bUse;//该条金手指是否被使用
	bool    bOneHot;
	bool 	bSub;
	bool	bExpd;//文件夹展开还是收缩
	uint32  SubCnt;//文件夹里的条目数
	_AR_DATA *pARNextData;
	_AR_DATA *pARPreData;
}AR_DATA;

typedef struct _NDSCheatInfo{
  char		sign[5];
  uint32	dw_CRC; 
  uint32	dw_StartAddress;
} NDSCheatInfo;
//条件语句
typedef struct _IFFUN{
	uint32 index;
	vuint32 *pPos;
}IFFUN;

extern AR_DATA	*m_pARDataHead;
extern uint32	m_CheatItemCounter;
extern uint32	m_FirstLevelCIC;
uint32  crc32(uint32   crc,unsigned   char   *buf,   uint32   len) ;
NDSCheatInfo *FindNDSCheatInfo(char *sign,uint32 RomCRC);
bool GetCheatData(NDSCheatInfo *pInfo);
bool DeleteAR_DATA();
void PrintfARDSData();
//uint32 AnalyzeData(AR_DATA *pARData,uint32 off);
//void PrintfARDSData();
//uint32 FillARDSCheatData();
//void CleanbHotItem(AR_DATA *pARDataCurItem);



#endif










