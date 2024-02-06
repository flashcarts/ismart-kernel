/////////////////////////////////////////////////////////
extern "C"
{
    extern uint32 VerifySaver_thumb();
    extern uint32 Mapping_SDRealAddress();
    
    extern uint32 special_for0991();
}
extern uint32 patch_4511_data[];
extern uint32 patch_4638_data[];
extern uint32 patch_4800_data[];
extern uint32 patch_5292_data[];
//that games need some patcher
const char *games[] = {
		"UBRJ",//0x01 /0506 
		"UBRP",//0x02 /0591	
		"APDK",//0x03 /0991 口袋妖怪 
		"YHGJ",//0x04 /1981放学后少年汉化版		
		"UORJ",//0x05 /3690 老子制造
		"BDUP",//0x06 /4377 - C.O.P. The Recruit - 罪恶覆灭计划:刑警新兵 - EUR		
		"VRGV",//0x07 /4375 - Rabbids Go Home - 疯狂兔子回家 - EUR
		"C7UE",//0x08 /4501 巨龙战争USA
		"CHNE",//0x09 /4511 4512 - Might & Magic - Clash of Heroes (USA)
		"CHNP",//0x0A /4638 - 魔法门之英雄交锋 (E)
		"IPGK",//0x0B /4698 - 口袋妖怪：银灵（韩）
		"IPKK",//0x0C /4701 - 口袋妖怪：金心（韩）
		"IPKF",//0x0D /4786 - 口袋妖怪：金心(法)
		"IPKE",//0x0E /4787 - 口袋妖怪：金心（美）
		"IPGE",//0x0F /4788 - 口袋妖怪：银灵（美）
		"IPGF",//0x10 /4791 - 口袋妖怪：银灵(法)
		"BIGP",//0x11 /4800 - 变异巨虫大战（欧）
		"UORE",//0x12 /4812 - 老子制造(美）	
		"IPKD",//0x13 /4828 - 口袋妖怪：金心（德）
		"IPGD",//0x14 /4829 - 口袋妖怪：银灵（德）
		"IPKS",//0x15 /4833 - 口袋妖怪：金心（西班）
		"IPGS",//0x16 /4834 - 口袋妖怪：银灵（西班）
		"IPKI",//0x17 /4835 - 口袋妖怪：金心（意大）
		"IPKE",//0x18 /4841 - 口袋妖怪：金心（欧）
		"IPGE",//0x19 /4842 - 口袋妖怪：银灵（欧）
		"IPGI",//0x1A /4843 - 口袋妖怪：银灵（意大）
		"VLCJ",//0x1B /4913 - 小莉加DS：女孩子的课程（日）
		"UORP",//0x1C /4915 - 老子制造(欧）
		"BDEE",//0x1D /4933 - 病房２
		"COLE",//0x1E /4273_-_Mario_And_Sonic_At_The_Olympic_Winter_Games_USA_NDS-XPA
		"COLP",//0x1F /4291
		"CLWE",//0x20 /2906 - 星球大战：克隆战争 绝地同盟（美）
		"CLWP",//0x21 /2971 - 星球大战：克隆战争 绝地同盟（欧）
		"VXHJ",//0xB22 /5142 - 家庭教师 Hitman Reborn！DS火焰对战XX（日）
		"YMYE",//0x23 /5138 - 神秘岛（美）
		"BM9J",//0x24 /5135 - 重装机兵3（日）
		"CY9P",//0x25 /5114 - 企鹅俱乐部：精英企鹅部队 赫伯特的复仇（欧）
		"CY9E",//0x26 /4970 - 企鹅俱乐部：精英企鹅部队 赫伯特的复仇（美）
		"BOJJ",//0x27 /5197 - 海贼王：巨人战争（日）
		"IRAJ",//0x28 /5215 - 口袋妖怪：白（日）
		"IRBJ",//0x29 /5216 - 口袋妖怪：黑（日）
		"BFLP",//0x2A /5237 - 我的模拟人生：蓝天英雄（欧）
		"BDEJ",//0x2B /5240 - 病房2（日）
		"BDEP",//0x2C /5246 - 病房2（欧）
		"BK9J",//0x2D /5255 - 王国之心：编码重制版（日）
		"UXBP",//0x2E /4951 - 大合奏！乐团兄弟DX（欧）
		"VCDJ",//0x2F /5289 - 天空机器人（日）
		"BO5J",//0x30 /5292 - 黄金太阳：漆黑的黎明（日）
		"BO5P",//0x31 /5387 - 黄金太阳：漆黑的黎明（欧）
		"BO5E",//0x32 /5367 - 黄金太阳：漆黑的黎明（美）
		"B2KJ",//0x33 /5400 - 双重国度：漆黑的魔导士（日）
		"V2GE",//0x34 /5325 - 马里奥对大金刚：迷你王国的大混乱（美）
		"V2GJ",//0x35 /5386 - 马里奥对大金刚：迷你王国的大混乱（日）
		"B3GJ",//0x36 /5371 - SD高达三国传 真三璃纱大战
		"CS3E",//0x37 /4743 - 索尼克与世嘉全明星赛车（美）
		"CS3P",//0x38 /4757 - 索尼克与世嘉全明星赛车（欧）
		"VCMV",//0x39/5394 - 摇滚青春：最后的JAM（欧）
		
		"V2GV",//0x3A/5547 - 马里奥对大金刚：迷你王国的大混乱（欧）
		"IRAO",//0x3B/5584 - 口袋妖怪：白（欧）
		"IRBO",//0x3C/5585 - 口袋妖怪：黑（欧）
		"IRAF",//0x3D/5586 - 口袋妖怪：白（法）
		"IRBF",//0x3E/5587 - 口袋妖怪：黑（法）
		"IRAD",//0x3F/5588 - 口袋妖怪：白（德）
		"IRBD",//0x40/5589 - 口袋妖怪：黑（德）
		"IRBO",//0x41/5593 - 口袋妖怪：黑（美)
		"IRAO",//0x42/5594 - 口袋妖怪：白（美）
		"IRAS",//0x43/5598 - Pokémon: Edición Blanca (S)
		"IRBS",//0x44/5599 - Pokémon: Edición Negra (S)
		"IRAI",//0x45/5600 - Pokémon: Versione Bianca (I)
		"IRBI",//0x46/5601 - Pokémon: Versione Nera (I)
		"BYVJ",//0x47/5499 - 怪兽克星：Powered（日）
		"TFLJ",//0x48/5529 - 天降之物F：梦幻季节（日）
		"TDBJ",//0x49/5530 - 龙珠改：究极武斗传（日）
		"BYYJ",//0x4A/5592 - 游戏王5Ds：世界冠军大会2011 超越羁绊（日）
		'\0',
		'\0',
};
const char *games_forAddress[] = {
		"YY8J",//0xB01 /2436 里克与约翰 消失的2幅画(JP) -存档初始化问题		
		"BKBD",//0xB02 /4446_-_200_Klassische_Buecher_GER_NDS-iND
		"BZOP",//0xB03 /4383_-_World_of_Zoo_EUR_MULTi8_NDS-BAHAMUT;;;补丁代码位置用
		"BKWE",//0xB04 /4649 - Bookworm (U)
		"BT8E",//0xB05 /4705  - 暮光之城	补丁位置
		"BSDJ",//0xB06 /4735-四龙神战记
		"CS3E",//0xB07 /4743 - 索尼克与世嘉全明星赛车（美）补丁位置
		"CS3P",//0xB08 /4757 - 索尼克与世嘉全明星赛车（欧）补丁位置
		"BT8P",//0xB09 /4827 - 暮光之城（欧）//补丁位置

		'\0',
		'\0',
		
};
bool CmpGames(char* pbuf,const char* gamecode,int number)
{
	if(!memcmp(gamecode,pbuf+12,4))
	{
		dsCardi_WriteSram(0x7DFF0 , number);
		dsCardi_WriteSram(0x7DFF2 , 0);		
		
		return true;
	}
	return false;
}
//-------------------------------------------------------------
bool FixGame(const char *pFilename,uint32 address,uint32 oldValue,uint32 newValue)
{
    uint32 *p2x = (uint32*)0x2300000 ; 
	FAT_FILE *FileHandle2=FAT_fopen(pFilename,"rb+");
	if(FileHandle2 ==NULL) return false;
	FAT2_fseek(FileHandle2,address,SEEK_SET);
	FAT2_fread(p2x,4,1,FileHandle2);
	if(*p2x == oldValue)
	{
		*p2x = newValue;
		FAT2_fseek(FileHandle2,address,SEEK_SET);
		FAT2_fwrite(p2x,4,1,FileHandle2);
	}
	FAT2_fclose(FileHandle2);
	return true;
}
//--------------------------------------------------------------
#define patch_data_at_ITCM_address 0x01FF7000   //大补丁的存放地址
bool GetGameSpecial(char* pbuf,const char *pFilename)
{
	//videoSub_SetShowLog(true);
	int count=0;
	u8 *ptoAddress=(u8*)(patch_data_at_ITCM_address);
	while(games[count] != '\0')
	{
		if(CmpGames(pbuf,games[count],count+0x01))
		{	
			//_consolePrintf("---number:%x\n",dsCardi_ReadSram(0x7DFF0));
		    //while(*(vuint16*)0x04000130 == 0x3FF);
		    //while(*(vuint16*)0x04000130 != 0x3FF);			
		    {  
		        switch(dsCardi_ReadSram(0x7DFF0))
		    	{	        
		        case 0x01:	
		        case 0x02://3966
		        	{
		    		 	Enable_Arm9DS(); 
		    			SetShake(0x08);
		    			SetRompage(256);
		    			//SetRompage(0x300);
		    			OpenNorWrite();
		    			
		    	     	WAIT_CR &= ~0x80;
		    			for(int k=0 ;k<0x100 ; k+=4)
		    			{
		    				*(vu32*)(0x9000000+k) = 0xFFFFFFFF ;
		    				*(vu32*)(0x8000000+k) = 0xFFFFFFFF ;
		    			}
		    			
		    			//查找08000080
		    				*(vu32*)0x90000B0 = 0xFFFF;
		    				*(vu32*)0x90000B4 = 0x24242400 ;
		    				*(vu32*)0x90000B8 = 0xFFFFFFFF ;
		    				*(vu32*)0x90000BC = 0x7FFFFFFF;
		    			//查找0801FFFE
		    				*(vu32*)0x901FFFC = 0x7FFFFFFF ;
		    			//查找08240002	
		    				*(vu16*)0x9240002 = 1 ;
		    			//查找08000004	 -- 2个地方
		    			//	*(vuint32*)0x9000000 = 0x544E494E ;
		    			//	*(vuint32*)0x9000004 = 0x4F444E45 ;
		    			WAIT_CR |= 0x80;
		            break;
		        	}
		        case 0x03://0991
		    		{
		    			*(vuint32*)special_for0991 =  0xe2803040; //ADD      r3,r0,#0x40
		    		break;
		    		}
		        case 0x04://1981-YHGJ放学后少年汉化版
		    		{
		    			//FixGame(pFilename,0x0007e9e8,0x8E1BA1C4,0x8E1BA110);
		    			//MemCopy8CPU((void *)Game2patch,ptoAddress,(uint8*)Game2patch_end - (uint8*)Game2patch);
		    			break;
		    		}	 
		    	case 0x09://4511
		    		{
		    			MemCopy8CPU((void *)patch_4511_data,ptoAddress,0xF00/*sizeof(patch_4511_data)*/);
		    			break;
		    		}

		    	case 0x0A://4638
		    		{
		    			MemCopy8CPU((void *)patch_4638_data,ptoAddress,0xF00/*sizeof(patch_4638_data)*/);
		    			break;
		    		}
		        case 0x0B://4698
		        	{
		        		FixGame(pFilename,0x000DFE18,0x22FAD10A,0x22FAE00A);
		        		break;
		        	}
		        case 0x0C://4701
		        	{
		        		FixGame(pFilename,0x000DFE14,0xFAD10A0A,0xFAE00A0A);
		        		break;
		        	}

		        case 0x0D://4786 - 口袋妖怪：金心(法)
        			{
        				FixGame(pFilename,0x000DF418,0xFAD10A01,0xFAE00A01);        			
        				break;
        			}        	
		        case 0x0E://4787 - 口袋妖怪：金心（美）	        	
		        case 0x0F://4788 - 口袋妖怪：银灵（美）
	        		{
	        			FixGame(pFilename,0x000DF418,0xD10A2800,0xE00A2800);        			
	        			break;
	        		}
		        case 0x10://4791 - 口袋妖怪：银灵(法)
	        		{
	        			FixGame(pFilename,0x000DF418,0xFAD10A28,0xFAE00A28);        			
	        			break;
	        		}
		    	case 0x11://4800
		    		{
		    			MemCopy8CPU((void *)patch_4800_data,ptoAddress,0xF00/*sizeof(patch_4800_data)*/);
		    			break;			    		
		    		}
		    	case 0x1B://4913
		    		{
		    			FixGame(pFilename,0x00004000,0xBE966111,0xE7FFDEFF);
		    			FixGame(pFilename,0x00004004,0x7116E11A,0xE7FFDEFF);
		    			FixGame(pFilename,0x00004008,0x1BA14B7A,0xE7FFDEFF);
		    			FixGame(pFilename,0x0000400C,0x0D6E4C75,0x0D6EDEFF);
		    			FixGame(pFilename,0x000040DC,0x07381F51,0x47701F51);
		    			FixGame(pFilename,0x0000430C,0xFAE774DA,0xFAE74770);
		    			break;    			
		    		}
		    	case 0x30://5292
		    	case 0x31://5387
		    	case 0x32://5367
		    		{
		    			MemCopy8CPU((void *)patch_5292_data,ptoAddress,0xF00/*sizeof(patch_4800_data)*/);
		    			break;			    		
		    		}
		        case 0x36://5371
		        	{
		        		FixGame(pFilename,0x001089E4,0xE0A1C47A,0x30F1C47A);
		        		FixGame(pFilename,0x00108b80,0xE091BDA6,0x30E1BDA6);
		        		break;
		        	}
		        default:
		    		break;	
		    	}//end switch
		    }//end
		    
			return true;
		}		
		else count++;	
	} 
	
	//////////////////////////////////////////////
	count=0;
	while(games_forAddress[count] != '\0')
	{
		if(!memcmp(games_forAddress[count],pbuf+12,4))
		{	
			dsCardi_WriteSram(0x7DFF0 , count+0xB01);
			dsCardi_WriteSram(0x7DFF2 , 0);		
			//_consolePrintf("---B number:%x\n",dsCardi_ReadSram(0x7DFF0));
		    //while(*(vuint16*)0x04000130 == 0x3FF);
		    //while(*(vuint16*)0x04000130 != 0x3FF);		
			return true;
		}
		else count++;
	}
	dsCardi_WriteSram(0x7DFF0 , 0);
	dsCardi_WriteSram(0x7DFF2 , 0);	
	return false;
}
//---------------------------------------------
void GetSpecialSave(char *pbuf)
{	
	const char rom0856[] = "BLEACH DS2\0\0" ;//0856/2761/3494
	if(!memcmp(rom0856,pbuf,12))
	{
	  _consolePrintf("0856\n");
	  *((vuint16*)VerifySaver_thumb + 0x4) = 0x2001;
	  return ;
	}
	const char above2M_2385[] = "BANDBROS DX\0" ;
	if(!memcmp(above2M_2385,pbuf,12))
	{
		_consolePrintf("2385\n");
		if((*((vuint16*)Mapping_SDRealAddress + 2)) == 0x2f08)
		{
			*((vuint16*)Mapping_SDRealAddress + 2) = 0x2f80;
		}
		
	    return ;
	}
	const char save1M_3499[] = "PICROSS3D\0\0\0" ;
	if(!memcmp(save1M_3499,pbuf,12))
	{
		_consolePrintf("3499 is 1MByte size\n");
		if((*((vuint16*)Mapping_SDRealAddress + 2)) == 0x2f08)
			*((vuint16*)Mapping_SDRealAddress + 2) = 0x2fc0;		
	    return ;
	}
	const char save1M_4526[] = "SPIRITTRACKSBKI" ;
	if(!memcmp(save1M_4526,pbuf,15))
	{
		_consolePrintf("4526 is 1MByte size\n");
		if((*((vuint16*)Mapping_SDRealAddress + 2)) == 0x2f08)
			*((vuint16*)Mapping_SDRealAddress + 2) = 0x2fc0;
		return;
	}
	const char save8M_5054[] = "ARTACADEMYRTVAAJ" ;
	if(!memcmp(save8M_5054,pbuf,13))
	{
		_consolePrintf("5054 is 8MByte size\n");
		if((*((vuint16*)Mapping_SDRealAddress + 2)) == 0x2f08)
			*((vuint16*)Mapping_SDRealAddress + 2) = 0x2fc0;
		return;
	}
	const char rom0026[] = "POKEMON_DASH" ;//0026/0057/0119/0991
	if(!memcmp(rom0026,pbuf,12))
	{
	  _consolePrintf("0026/0057/0119/0991\n");
	  dsCardi_WriteSram(0x7DFF0 , 0x3333);
	  dsCardi_WriteSram(0x7DFF2 , 0x1234);	
	  return ;
	}
	
}
//---------------------------------------------
////////////////////////////////////////////////////////////////////
void WriteFat2_FPGA_SRAM(FAT_FILE * filePK , u32 startadd ,bool  needsomeloop)
{
    //写入文件的FAT表
    uint32 secp =  GetSecPerCls() ;
    uint32 value = GetFisrtCls()*0x200;
    FS_TYPE fs_systemType=FAT_GetFileSystemType();
    if(IsSDHC())
    {
        value = value/0x200 ;
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;
                if(needsomeloop)
                {
                    getcluster =  filePK->firstCluster ;
                    culvalue = value + (getcluster<<secp) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    i++ ;
                    getcluster = FAT2_NextCluster(getcluster);
                    culvalue = value + (getcluster<<secp) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
   
                }
                break;

            }
        case FS_FAT32:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF));
                    
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;
                if(needsomeloop)
                {
                    getcluster =  filePK->firstCluster ;
                    culvalue = value + (getcluster<<secp) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    i++ ;
                    getcluster = FAT2_NextCluster(getcluster);
                    culvalue = value + (getcluster<<secp) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
   
                }

                break;
            }
        }
        
    }
    else
    {
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;
                if(needsomeloop)
                {
                    getcluster =  filePK->firstCluster ;
                    culvalue = value + (getcluster<<(secp+9)) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    i++ ;
                    getcluster = FAT2_NextCluster(getcluster);
                    culvalue = value + (getcluster<<(secp+9)) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
   
                }
                break;

            }
        case FS_FAT32:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF));
                    
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;
                if(needsomeloop)
                {
                    getcluster =  filePK->firstCluster ;
                    culvalue = value + (getcluster<<(secp+9)) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    i++ ;
                    getcluster = FAT2_NextCluster(getcluster);
                    culvalue = value + (getcluster<<(secp+9)) ;
                    dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
   
                }
                break;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void WriteFat2Flash(FAT_FILE * filePK , u32 startadd , uint32 usize,bool  needsomeloop )
{
	Mart_Arm9Access();
	startadd += 0x10000 ;
	if((startadd == 0) || (startadd == 0x200000)) return;
	cardEepromSectorErase(startadd);
	if((usize>0x10000) && (startadd!=0x1F0000))
	{
		cardEepromSectorErase(startadd+0x10000);
	}
	
    uint32 secp =  GetSecPerCls() ;
    uint32 value = GetFisrtCls()*0x200;
    FS_TYPE fs_systemType=FAT_GetFileSystemType();
    _consolePrintf("secp = %x\n",secp) ;
    _consolePrintf("value = %x\n",value) ;
    _consolePrintf("fs_systemType = %x\n",fs_systemType) ;
    if(IsSDHC())
    {
    	_consolePrintf(" is SDHC \n") ;
        value = value/0x200 ;
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
                	Mart_Arm9Access();
                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;
                if(needsomeloop)
                {
                	for(int loop=0;loop<0x20;loop++)
                	{
	                    getcluster =  filePK->firstCluster ;
	                    culvalue = value + (getcluster<<secp) ;
	                	Mart_Arm9Access();
	                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
	                    i++ ;
	                    if(i>=0x4000)break;
                	}
                }
                break;

            }
        case FS_FAT32:
            {
            	_consolePrintf(" is FAT32 \n") ;
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
                	Mart_Arm9Access();
                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
                    
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;
                if(needsomeloop)
                {
                	for(int loop=0;loop<0x20;loop++)
                	{
	                    getcluster =  filePK->firstCluster ;
	                    culvalue = value + (getcluster<<secp) ;
	                	Mart_Arm9Access();
	                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
	                    i++ ;
	                    if(i>=0x4000)break;
                	}
                }

                break;
            }
        }
        
    }
    else
    {
    	_consolePrintf(" is SD \n") ;
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
                	Mart_Arm9Access();
                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;
                if(needsomeloop)
                {
                	for(int loop=0;loop<0x20;loop++)
                	{
                		getcluster =  filePK->firstCluster ;
	                    culvalue = value + (getcluster<<secp+9) ;
	                	Mart_Arm9Access();
	                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
	                    i++ ;
	                    if(i>=0x4000)break;
                	}
                }
                break;

            }
        case FS_FAT32:
            {
            	_consolePrintf(" is FAT32 \n") ;
                uint32 getcluster =  filePK->firstCluster ;
                _consolePrintf("getcluster = %x\n",getcluster) ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
                	Mart_Arm9Access();
                    cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
                    
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;
                if(needsomeloop)
                {
                	for(int loop=0;loop<0x20;loop++)
                	{
                		getcluster =  filePK->firstCluster ;
                		culvalue = value + (getcluster<<(secp+9)) ;
                		Mart_Arm9Access();
                        cardWriteEeprom(startadd+i*4 , (uint8*)&culvalue , 4 , 3);
                		i++ ;
                		if(i>=0x4000)break;
                 	}
                }
                break;
            }
        }
    } 
}
// prepare sav file
void testSaver(char *SaverName)
{   
    uint32 createSize = 0x80000; //512KB size
	const char above2M_2385[] = "BANDBROS DX\0AXBJ" ;
	if(!memcmp(above2M_2385,(char*)0x27FFE00,16))
	{
		createSize = 8*1024*1024 ;
		dsCardi_WriteSram(0x7DF80 , 0xB01); //Uses 00 fill
		dsCardi_WriteSram(0x7DF82 , 0);  	
	}
	const char above32M_4951[] = "BANDBROS DX\0UXBP" ;
	if(!memcmp(above32M_4951,(char*)0x27FFE00,16))
	{
		createSize = 32*1024*1024 ;
		dsCardi_WriteSram(0x7DF80 , 0xB01); //Uses 00 fill
		dsCardi_WriteSram(0x7DF82 , 0);  
	}
	const char save32M_3690[] = "DSMIO\0\0\0\0\0\0\0" ;
	if(!memcmp(save32M_3690,(char*)0x27FFE00,12))
		createSize = 32*1024*1024 ;
	const char save1M_3499[] = "PICROSS3D\0\0\0" ;//3499 - Rittai Picross - 立体方块 - JPN
	if(!memcmp(save1M_3499,(char*)0x27FFE00,12))
		createSize = 1*1024*1024 ;
	
	const char save1M_4526[] = "SPIRITTRACKSBKI" ;
	if(!memcmp(save1M_4526,(char*)0x27FFE00,13))		
		createSize = 1*1024*1024 ;
	
	const char save512_1566[] = "COMMONSENSE." ;
	if(!memcmp(save512_1566,(char*)0x27FFE00,12))
	{
		dsCardi_WriteSram(0x7DF80 , 0xB01); //Uses 00 fill
		dsCardi_WriteSram(0x7DF82 , 0);   	
	}
	const char save8M_5054[] = "ARTACADEMYRTVAAJ" ;
	if(!memcmp(save8M_5054,(char*)0x27FFE00,13))		
		createSize = 8*1024*1024 ;
	
	char*extname = "sav";
	char*Folder = "/SAVE";
    FAT_FILE *filePK=CheckFile_in_Folder(SaverName,extname,Folder,createSize);

	dsCardi_WriteSram(0x7dfE0 , IsSDHC());
	dsCardi_WriteSram(0x7dfE2 , 0);
        
	uint32 secp =  GetSecPerCls() ;
	dsCardi_WriteSram(0x7dfE4 , secp);  
	dsCardi_WriteSram(0x7dfE6 , 0); 

    WriteFat2Flash(filePK,0x1e0000,0x10000,0) ;
                
	filePK ->write = 1 ;
	FAT_fclose(filePK); 
	FAT_chdir("/");
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void WriteFat2_File(FAT_FILE *filePK ,FAT_FILE * fileRTS ,uint32 startAddress)
{
    uint32 secp =  GetSecPerCls() ;
    uint32 value = GetFisrtCls()*0x200;
    FS_TYPE fs_systemType=FAT_GetFileSystemType();
    uint32 *pFAT_temp = (uint32*)0x2350000;
    if(IsSDHC())
    {
        value = value/0x200 ;
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    //dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    //dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF));
                    *(pFAT_temp+i) = culvalue;
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;

                break;

            }
        case FS_FAT32:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<secp) ;
    //_consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    //dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    //dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF));
                    *(pFAT_temp+i) = culvalue;
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;

                break;
            }
        }
        
    }
    else
    {
        switch(fs_systemType)
        {
        case FS_FAT16:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    //dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    //dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF)); 
                    *(pFAT_temp+i) = culvalue;
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFF7) ;

                break;

            }
        case FS_FAT32:
            {
                uint32 getcluster =  filePK->firstCluster ;
                uint32 culvalue = 0 , i = 0 ;
                do
                {
                    culvalue = value + (getcluster<<(secp+9)) ;
   // _consolePrintf("cluster [%x] at %x.\n",i,culvalue);
                    //dsCardi_WriteSram(startadd+i*4 , (culvalue&0xFFFF));
                    //dsCardi_WriteSram(startadd+i*4+2 , ((culvalue>>16)&0xFFFF));
                    *(pFAT_temp+i) = culvalue;
                    
                    getcluster = FAT2_NextCluster(getcluster);
                    i++;    
                }
                while(getcluster <0xFFFFFF7) ;

                break;
            }
        }
    }
    FAT2_fseek(fileRTS,startAddress,SEEK_SET);   
	FAT2_fwrite(pFAT_temp,4,1024,fileRTS);
	//FAT2_fclose(fileRTS);
    /*_consolePrintf("0x2350000= %x\n",*(uint32*)0x2350000);
    _consolePrintf("0x2350004= %x\n",*(uint32*)0x2350004);
    _consolePrintf("0x2350008= %x\n",*(uint32*)0x2350008);
    _consolePrintf("0x235000c= %x\n",*(uint32*)0x235000c);
    _consolePrintf("0x2350010= %x\n",*(uint32*)0x2350010);
    _consolePrintf("0x2350014= %x\n",*(uint32*)0x2350014);
    _consolePrintf("0x2350018= %x\n",*(uint32*)0x2350018);
    _consolePrintf("0x235001c= %x\n",*(uint32*)0x235001c);*/
    //while(*(vuint16*)0x04000130 == 0x3ff);
    //while(*(vuint16*)0x04000130 != 0x3ff);
}
//----------------------------------------------------------
uint32 patch_4638_data[]=
{
0x20047DC,0x20050D0,0x200635C,0x2007184,
0x20074FC,0x20077D0,0x200803C,0x200859C,
0x2008E60,0x2009404,0x20095F4,0x2009840,
0x200A63C,0x200A744,0x200A8E0,0x200B000,
0x200B578,0x200B784,0x200CFE0,0x200D130,
0x200D34C,0x200D488,0x200D7A0,0x200D944,
0x200DB14,0x200DC5C,0x200DE40,0x200E3E8,
0x200E7C4,0x200EAA4,0x200ED04,0x200EE8C,
0x200EFF4,0x2010E14,0x2010FFC,0x2011480,
0x20115F0,0x2011738,0x20118D4,0x2011E24,
0x20153F0,0x2015560,0x2015974,0x2015B1C,
0x2015EC4,0x20185D8,0x2018FC0,0x20191E8,
0x2019D58,0x2019ED0,0x201BAE4,0x201BC20,
0x201BD5C,0x201C7AC,0x201C9F0,0x201CAF0,
0x201DA2C,0x201DBD8,0x201DF4C,0x201E4C8,
0x201E934,0x201EB40,0x201F468,0x2020E70,
0x2021274,0x20215C4,0x2021A50,0x2021BB0,
0x2021CB4,0x2022104,0x202264C,0x2022848,
0x202377C,0x20238B4,0x2023A58,0x2023BC8,
0x2023D10,0x20243B8,0x20245FC,0x2025990,
0x20268AC,0x20269E4,0x2027078,0x2029E6C,
0x202AA98,0x202AFBC,0x202B188,0x202B5D0,
0x202BC90,0x202BFE0,0x202C0E8,0x202CA18,
0x202CC68,0x202CD7C,0x202D12C,0x202D234,
0x202D54C,0x202DB3C,0x202FD8C,0x2030258,
0x203088C,0x203203C,0x2032288,0x2032CC0,
0x2033684,0x2033B40,0x20344EC,0x2034818,
0x2035628,0x2035D08,0x203617C,0x2037268,
0x20373DC,0x2038EA0,0x2039A94,0x203AA40,
0x203AC74,0x203B0E4,0x203B238,0x203B7A8,
0x203B9D8,0x203C1BC,0x203C2D8,0x203C3D0,
0x203C5E0,0x203C7C8,0x203C920,0x203CB58,
0x203CCC8,0x203CE48,0x203D184,0x203D2FC,
0x203D538,0x203EC1C,0x203ED60,0x203EEA4,
0x203F0A8,0x203FBAC,0x2040014,0x2040180,
0x204094C,0x2040EB0,0x204136C,0x2041614,
0x20417AC,0x2041900,0x2041CA4,0x2042098,
0x204295C,0x2042AD8,0x2045504,0x204573C,
0x2045DB4,0x2046070,0x20464A8,0x20479C4,
0x2047C54,0x2048320,0x2048648,0x204886C,
0x2048A0C,0x2049C90,0x204BAA4,0x204BE68,
0x204BF84,0x204CA70,0x204E2C0,0x204F0C0,
0x204FEA8,0x2050424,0x2050C70,0x2050DC0,
0x20517D0,0x205192C,0x2052084,0x20525C4,
0x2052EAC,0x2053CA8,0x205412C,0x20557D8,
0x2055A00,0x2055B14,0x2055EB8,0x2056080,
0x2056374,0x2056584,0x20566D8,0x205714C,
0x20572AC,0x2057834,0x2057B90,0x2057CA0,
0x2057EE4,0x2057FF8,0x2058378,0x205868C,
0x2058B0C,0x205A1F4,0x205A6DC,0x205ABB8,
0x205BF0C,0x205C068,0x205E56C,0x205EAC0,
0x20602CC,0x2060AF8,0x2060D24,0x2061774,
0x206415C,0x20645B4,0x2065E34,0x206789C,
0x206AFC4,0x206B1EC,0x206BF9C,0x206C8D8,
0x206CE68,0x206D0F8,0x206D634,0x206DB30,
0x206DC7C,0x206DDE8,0x206E228,0x206E5A0,
0x206F76C,0x206F91C,0x20701E0,0x2070DF0,
0x2071458,0x207194C,0x2071D2C,0x207264C,
0x2075658,0x2075CE0,0x2075E70,0x2075FC0,
0x2076774,0x20768E8,0x2076A24,0x2076B38,
0x20780FC,0x207916C,0x20797A4,0x20799CC,
0x2079B2C,0x2079C54,0x2079D60,0x2079EB4,
0x2079FEC,0x207A388,0x207A534,0x207C5A8,
0x207C858,0x207DA84,0x207DCD4,0x207DFDC,
0x207E21C,0x207E5EC,0x207E89C,0x207EBE8,
0x207ED48,0x207EE60,0x207FCD0,0x2080374,
0x2083020,0x2083150,0x2083FA0,0x20841A4,
0x2084300,0x20844B8,0x2084920,0x2084EB4,
0x2085050,0x208517C,0x208536C,0x2085520,
0x20857A8,0x20867E8,0x2087470,0x208766C,
0x20878F8,0x2087AB0,0x2087BF8,0x2087D34,
0x2087E3C,0x2087F70,0x208882C,0x2088A10,
0x208909C,0x20894D4,0x2089668,0x208A420,
0x208AEC8,0x208B090,0x208B258,0x208C5C8,
0x208D33C,0x208D668,0x208DC94,0x208DE7C,
0x2090AA4,0x2090CDC,0x2091234,0x2091520,
0x20919C8,0x2092A14,0x20932CC,0x2093518,
0x2093808,0x2093B08,0x2096560,0x2096670,
0x2096850,0x2096C24,0x2096E70,0x2096FA8,
0x20972A8,0x209ADF8,0x209AF18,0x209B084,
0x209BE0C,0x209BF78,0x209C2F8,0x209C428,
0x209C534,0x209C654,0x209C77C,0x209D7AC,
0x20A1194,0x20A1338,0x20A155C,0x20A1838,
0x20A1C9C,0x20A1DDC,0x20A2070,0x20A2244,
0x20A2440,0x20A35F0,0x20A4588,0x20A483C,
0x20A4EBC,0x20A504C,0x20A51C0,0x20A52C8,
0x20A5B2C,0x20A5CA4,0x20A5F38,0x20A604C,
0x20A6630,0x20A6E3C,0x20A75E4,0x20A76F0,
0x20A780C,0x20A8440,0x20A8860,0x20A8AB8,
0x20A8C58,0x20A9658,0x20A97C8,        
0x20013B4,0x2003E58,0x2004EEC,0x2006928,
0x2006CB4,0x200764C,0x2007EC8,0x2009024,
0x2009210,0x20096FC,0x200E518,0x200E908,
0x2010CA8,0x2011D3C,0x201342C,0x201C3B8,
0x201C8E4,0x201E0D0,0x201E36C,0x2025844,
0x2027E94,0x2029D20,0x202A19C,0x202D840,
0x2030078,0x2030B54,0x20331DC,0x203354C,
0x20338BC,0x2033A2C,0x2033D3C,0x20362E4,
0x2036434,0x2038A80,0x2039214,0x203AD8C,
0x203AEA8,0x203BBE4,0x203CF90,0x203D6A8,
0x203D8F4,0x2040AA0,0x2040CB0,0x20424F4,
0x20427E0,0x2042DA0,0x2045930,0x2045AE4,
0x204748C,0x2048C9C,0x2049628,0x204C0C8,
0x2051A40,0x20521F4,0x2056810,0x2057DB8,
0x205E458,0x205F5F0,0x20601A8,0x2061478,
0x2063FDC,0x2066A60,0x206B308,0x206C0B8,
0x206D86C,0x206FAA0,0x206FC98,0x207004C,
0x20710CC,0x20752A4,0x20753C8,0x20754F4,
0x2076144,0x20763E4,0x2077200,0x2077DF4,
0x207A63C,0x207B08C,0x207CEFC,0x207D598,
0x207D708,0x207E0F8,0x207E320,0x207E4B8,
0x207F870,0x207FEF0,0x2085FE4,0x208ACFC,
0x208CC44,0x208FDC4,0x20905CC,0x2090960,
0x2090F4C,0x2091B4C,0x2091FBC,0x209391C,
0x2096D4C,0x209B64C,0x209B8AC,0x209B9BC,
0x209BAD4,0x209CA50,0x209DA14,0x20A2594,
0x20A4A38,0x20A4D80,0x20A53DC,0x20A74C0,
};
///////////////////////////////////////////
uint32 patch_4511_data[]=
{
0x20047DC,0x20050D0,0x200635C,0x2007184,
0x20074FC,0x20077D0,0x200803C,0x200859C,
0x2008E60,0x2009404,0x20095F4,0x2009840,
0x200A5F4,0x200A6FC,0x200A898,0x200AFB8,
0x200B530,0x200B73C,0x200CF98,0x200D0E8,
0x200D304,0x200D440,0x200D758,0x200D8FC,
0x200DACC,0x200DC14,0x200DDF8,0x200E3A0,
0x200E77C,0x200EA5C,0x200ECBC,0x200EE44,
0x200EFAC,0x2010DCC,0x2010FB4,0x2011438,
0x20115A8,0x20116F0,0x201188C,0x2011DDC,
0x20153A8,0x2015518,0x201592C,0x2015AD4,
0x2015E7C,0x2018590,0x2018F78,0x20191A0,
0x2019D10,0x2019E88,0x201BA9C,0x201BBD8,
0x201BD14,0x201C760,0x201C9A4,0x201CAA4,
0x201D9E0,0x201DB8C,0x201DF00,0x201E47C,
0x201E8E8,0x201EAF4,0x201F41C,0x2020E24,
0x2021228,0x2021578,0x2021A04,0x2021B64,
0x2021C68,0x20220B8,0x2022600,0x20227FC,
0x2023730,0x2023868,0x2023A0C,0x2023B7C,
0x2023CC4,0x202436C,0x20245B0,0x2025944,
0x2026860,0x2026998,0x202702C,0x2029E20,
0x202AA4C,0x202AF70,0x202B13C,0x202B584,
0x202BC04,0x202BF54,0x202C05C,0x202C98C,
0x202CBDC,0x202CCF0,0x202D0A0,0x202D1A8,
0x202D4C0,0x202DAB0,0x202FD00,0x20301CC,
0x2030800,0x2031FB0,0x20321FC,0x2032C34,
0x20335F8,0x2033AB4,0x2034430,0x203475C,
0x203556C,0x2035C4C,0x20360C0,0x20371AC,
0x2037320,0x2038DB4,0x20399A8,0x203A954,
0x203AB88,0x203AFF4,0x203B148,0x203B6B8,
0x203B8E8,0x203C0CC,0x203C1E8,0x203C2E0,
0x203C4F0,0x203C6D8,0x203C830,0x203CA68,
0x203CBD8,0x203CD58,0x203D094,0x203D20C,
0x203D448,0x203EB2C,0x203EC70,0x203EDB4,
0x203EFB8,0x203FABC,0x203FF24,0x2040090,
0x204085C,0x2040DC0,0x204127C,0x2041524,
0x20416BC,0x2041810,0x2041BB4,0x2041FA8,
0x204286C,0x20429E8,0x20453D8,0x2045610,
0x2045C88,0x2045F44,0x204637C,0x2047898,
0x2047B28,0x20481F4,0x204851C,0x2048740,
0x20488E0,0x2049B64,0x204B978,0x204BD3C,
0x204BE58,0x204C944,0x204E194,0x204EF94,
0x204FD7C,0x20502F8,0x2050B44,0x2050C94,
0x20516A4,0x2051800,0x2051F58,0x2052498,
0x2052D80,0x2053B74,0x205406C,0x2055718,
0x2055940,0x2055A54,0x2055DF8,0x2055FC0,
0x20562B4,0x20564C4,0x2056618,0x205708C,
0x20571EC,0x2057774,0x2057AD0,0x2057BE0,
0x2057E24,0x2057F38,0x20582B4,0x20585C8,
0x2058A48,0x205A130,0x205A618,0x205AAF4,
0x205BE48,0x205BFA4,0x205E4A8,0x205E9FC,
0x2060208,0x2060A34,0x2060C60,0x20616F8,
0x20640EC,0x2064548,0x2065DC8,0x2067830,
0x206AF58,0x206B180,0x206BF30,0x206C86C,
0x206CDFC,0x206D08C,0x206D5C8,0x206DAC4,
0x206DC10,0x206DD7C,0x206E1BC,0x206E534,
0x206F700,0x206F8B0,0x2070174,0x2070D84,
0x20713EC,0x20718E0,0x2071CC0,0x20725E0,
0x20755EC,0x2075C74,0x2075E04,0x2075F54,
0x2076708,0x207687C,0x20769B8,0x2076ACC,
0x2078090,0x2079100,0x2079738,0x2079960,
0x2079AC0,0x2079BE8,0x2079CF4,0x2079E48,
0x2079F80,0x207A31C,0x207A4C8,0x207C53C,
0x207C7EC,0x207DA18,0x207DC68,0x207DF70,
0x207E1B0,0x207E580,0x207E830,0x207EB7C,
0x207ECDC,0x207EDF4,0x207FC64,0x2080308,
0x2082F08,0x2083038,0x2083E88,0x208408C,
0x20841E8,0x20843A0,0x2084808,0x2084D9C,
0x2084F38,0x2085064,0x2085254,0x2085408,
0x2085690,0x20866D0,0x2087358,0x2087554,
0x2087800,0x20879B8,0x2087B00,0x2087C3C,
0x2087D44,0x2087E78,0x2088734,0x2088918,
0x2088FA4,0x20893DC,0x2089570,0x208A328,
0x208ADD0,0x208AF98,0x208B160,0x208C4CC,
0x208D240,0x208D56C,0x208DB98,0x208DD80,
0x20909A8,0x2090BE0,0x2091138,0x2091424,
0x20918CC,0x2092918,0x20931D0,0x209341C,
0x209370C,0x2093A0C,0x2096464,0x2096574,
0x2096754,0x2096B28,0x2096D74,0x2096EAC,
0x20971AC,0x209ACFC,0x209AE1C,0x209AF88,
0x209BD10,0x209BE7C,0x209C1FC,0x209C32C,
0x209C438,0x209C558,0x209C680,0x209D6B0,
0x20A1098,0x20A123C,0x20A1460,0x20A173C,
0x20A1BA0,0x20A1CE0,0x20A1F74,0x20A2148,
0x20A2344,0x20A34F4,0x20A448C,0x20A4740,
0x20A4DC0,0x20A4F50,0x20A50C4,0x20A51CC,
0x20A5A30,0x20A5BA8,0x20A5E3C,0x20A5F50,
0x20A6534,0x20A6D40,0x20A74E8,0x20A75F4,
0x20A7710,0x20A8344,0x20A8764,0x20A89BC,
0x20A8B5C,0x20A955C,0x20A96CC,
0x20013B4,0x2003E58,0x2004EEC,0x2006928,
0x2006CB4,0x200764C,0x2007EC8,0x2009024,
0x2009210,0x20096FC,0x200E4D0,0x200E8C0,
0x2010C60,0x2011CF4,0x20133E4,0x201C370,
0x201C898,0x201E084,0x201E320,0x20257F8,
0x2027E48,0x2029CD4,0x202A150,0x202D7B4,
0x202FFEC,0x2030AC8,0x2033150,0x20334C0,
0x2033830,0x20339A0,0x2033CB0,0x2036228,
0x2036378,0x20389C4,0x2039128,0x203ACA0,
0x203ADBC,0x203BAF4,0x203CEA0,0x203D5B8,
0x203D804,0x20409B0,0x2040BC0,0x2042404,
0x20426F0,0x2042CB0,0x2045804,0x20459B8,
0x2047360,0x2048B70,0x20494FC,0x204BF9C,
0x2051914,0x20520C8,0x2056750,0x2057CF8,
0x205E394,0x205F52C,0x20600E4,0x20613B4,
0x2063F60,0x20669F4,0x206B29C,0x206C04C,
0x206D800,0x206FA34,0x206FC2C,0x206FFE0,
0x2071060,0x2075238,0x207535C,0x2075488,
0x20760D8,0x2076378,0x2077194,0x2077D88,
0x207A5D0,0x207B020,0x207CE90,0x207D52C,
0x207D69C,0x207E08C,0x207E2B4,0x207E44C,
0x207F804,0x207FE84,0x2085ECC,0x208AC04,
0x208CB48,0x208FCC8,0x20904D0,0x2090864,
0x2090E50,0x2091A50,0x2091EC0,0x2093820,
0x2096C50,0x209B550,0x209B7B0,0x209B8C0,
0x209B9D8,0x209C954,0x209D918,0x20A2498,
0x20A493C,0x20A4C84,0x20A52E0,0x20A73C4,
};
uint32 patch_4800_data[]=
{
0x02000e0c,0x020021c8,0x020029d6,0x02004b5c,
0x02004f6c,0x020051aa,0x02007942,0x0200bb50,
0x0200bcee,0x0200be0e,0x0200c134,0x0200cbd8,
0x0200d032,0x0200d41e,0x0200d776,0x0200d890,
0x0200d960,0x0200da3e,0x0200db4c,0x02011190,
0x0201147e,0x0201172e,0x02011f06,0x020133f2,
0x0201366c,0x02018e7e,0x02019fd6,0x0201aff6,
0x0201b0f0,0x0201b462,0x0201b57c,0x0201b6bc,
0x0201ba2e,0x0201bb44,0x0201bce8,0x0201bdd2,
0x0201c140,0x0201c602,0x0201c6c6,0x0201c826,
0x0201cd12,0x0201ce28,0x0201cef4,0x0201d1f4,
0x0201d3e0,0x0201d844,0x0201ddd2,0x0201eee6,
0x0201f264,0x0201f4a8,0x0201f59c,0x02020184,
0x0202031e,0x020215bc,0x02021792,0x02021a8c,
0x02021bbc,0x02022c22,0x02022f08,0x02024fd0,
0x02025c72,0x02025f9e,0x02026154,0x02026226,
0x0202630a,0x02026fde,0x020273e0,0x02027c2a,
0x02027eca,0x02028498,0x020286c4,0x02028784,
0x02028928,0x020289f6,0x02028bf2,0x0202a05e,
0x0202b28a,0x0202b36e,0x0202b88c,0x0202b97a,
0x0202badc,0x0202d0a6,0x0202d1c6,0x0202d3ce,
0x0202d4da,0x0202d686,0x0202d78a,0x0202d866,
0x0202d9d8,0x0202dd22,0x0202e45c,0x0202ee16,
0x0202eff0,0x0202f0b2,0x0202f436,0x0202f5b4,
0x0202f716,0x0202f926,0x0202f9e8,0x02030de0,
0x0203241c,0x02032d36,0x02032e50,0x0203300a,
0x020334d0,0x0203377c,0x02033880,0x02033944,
0x02033a2e,0x02033caa,0x020346dc,0x02034812,
0x0203518a,0x020352ac,0x0203568e,0x02037334,
0x02037c70,0x020387bc,0x020388c4,0x02038ff0,
0x020390f8,0x0203945c,0x02039596,0x02039d30,
0x02039e74,0x0203ba2c,0x0203bf16,0x0203df82,
0x0203e23c,0x0203e390,0x0203eb7c,0x0203fbea,
0x0203fd5e,0x020413c6,0x020416aa,0x020419d0,
0x02042c36,0x02044dba,0x020464aa,0x02046c74,
0x02046dcc,0x02046f0a,0x02047226,0x0204734a,
0x02048230,0x02048c58,0x02048f78,0x0204906c,
0x02049130,0x020499fc,0x0204b4aa,0x0204c058,
0x0204caac,0x02077262,0x02077494,0x02077616,
0x02078e42,0x02078fbc,0x0207a524,0x0207c170,
0x0207c364,0x0207c428,0x0207c524,0x0207c5e4,
0x0207c6a4,0x0207cba2,0x0207cc8a,0x0207cd84,
0x0207d74a,0x0207eb5a,0x020818de,0x0208325a,
0x020838ae,0x02095cbe,0x0209616e,0x02096382,
0x0209650a,0x020a9968,0x020aaa20,0x020aaba8
  };
uint32 patch_5292_data[]=
{
	0xE59F0008 ,0xE59F1008 ,0xE5801000 ,0xE12FFF1E,
	0x0204B868 ,0xEBFED31C ,0x00000000 ,0x00000000,
	0xE1A05000 ,0xE24DD008 ,0xE92D403F ,0xE2854004,				
	0xE28D5024 ,0xE8950007 ,0xE2455008 ,0xE28F3004, 				
	0xE885001F ,0xE8BD803F ,0xE8BD0004 ,0xE92D003F, 
	0xE8920003 ,0xE0801001 ,0xE28F20E4 ,0xE28F3020, 
	0xE3A04004 ,0xE59F50FC ,0xEB00001B ,0xE28F20DC, 
	0xE28F303C ,0xE3A04008 ,0xE3A05000 ,0xEB000016, 
	0xE8BD803F ,0xE5802004 ,0xE5802000 ,0xE58DE004, 
	0xE92D003F ,0xE1A01005 ,0xE2410010 ,0xE28F20AC, 
	0xE28F300C ,0xE3A04008 ,0xE3A05000 ,0xEB00000A, 
	0xE8BDC03F ,0xE3A00000 ,0xE92D41FF ,0xE1A00005,
	0xE080100B ,0xE28F2090 ,0xE28F3044 ,0xE3A04004,
	0xE3A05000 ,0xEB000000 ,0xE8BD81FF ,0xE92D41FF, 
	0xFA000021 ,0xE3500000 ,0x0AFFFFFA ,0xE0406004, 
	0xFB000024 ,0xE38774EB ,0xE5867000 ,0xEE076F3E, 
	0xE3550000 ,0x12466004 ,0x15865000 ,0x1E076F3E, 
	0xEAFFFFF2 ,0xE92D500F ,0xE1A01004 ,0xE3510902, 
	0x33A01902 ,0xE3A000B7 ,0xE1A00000 ,0xE1A00000, 
	0xE59F2008 ,0xE12FFF32 ,0xE3A0E000 ,0xE8BD900F, 
	0x02000041 ,0x3AFFFFF8 ,0xE5802004 ,0xE5802000, //0x02000041
	0x3AFFFFAE ,0xE3A00000 ,0xE28DD008 ,0xE5C101AD, 
	0xE5C101AE ,0xE5C101AF ,0xE92DC000 ,0xCA1CB5FC, 
	0xD2084288 ,0x42AAC8E0 ,0x42B3D103 ,0x42BCD101, 
	0x3808D002 ,0x2000E7F4 ,0x1B9FBDFC ,0x01BF3F08, 
	0x47700A3F 
};
///////////////////////
//-------------------------------------------------------------------
#include "Encryption.h"
