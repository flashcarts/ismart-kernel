#include "nds.h"
#include "arm9tcm.h"

//#define yafeidebug

extern "C"  uint32  A9_ReadSram(uint32 psramAdd) ;
extern "C"  uint32 ds_WriteSram(uint32 address,uint16 data);
extern "C" void WritePatchInfo_4BYTE(uint32 address ,  uint32  data);

extern "C" void SAVE_32M_3690_start(void) ;
extern "C" void SAVE_32M_3690_end(void) ;

extern "C" void patchSpecialGame(uint32 *r0_start,uint32 *r1_end);
extern "C" void PackSoftRest_and_cheat(uint32* uStart,uint32* uEnd);

extern "C" void ReplaceIRQ_start_arm9(void) ;
extern "C" void ReplaceIRQ_end_arm9(void);
extern "C" void copy_in();
extern "C" void quit_irq();
extern "C" void arm7_interrup();
extern "C" void arm7_Readkey();
extern "C" void copy_in_2();

//即时存档
extern "C" void RTS_ReplaceIRQ_start(void) ;
extern "C" void RTS_ReplaceIRQ_end(void);
extern "C" void RTS_copy_in();
extern "C" void RTS_quit_irq();
extern "C" void RTS_arm7_interrup();
extern "C" void RTS_arm7_Readkey();
extern "C" void RTS_copy_in_2();

extern "C" void AR_start();
extern "C" void AR_end();
extern "C" void fix_data();
extern "C" void is_softReset();
extern "C" void b_arm7Reset_wait();

//补丁过程使用的代码 GamePatch.s文件里面
extern "C" void WriteDubugInfo(uint32 r0_result) ;
extern "C" void WritePatchInfo(uint32 address , uint32 size , uint32* buffer);
extern "C" void WritePatchInfo_4BYTE(uint32 address ,  uint32  data);

extern uint32 gl_BACKROR;
/////////////////////////////////////////////////////////
CODE_IN_ITCM static void copyCode(void* psrc ,void * pdes , int size)
{
    uint32 *pCodesrc = (uint32*) psrc ;
    uint32 *pCodedes = (uint32*) pdes ;
    do
    {
        *pCodedes = *pCodesrc ;
        size -= 4 ;
        pCodedes ++ ;
        pCodesrc ++ ;
    }
    while(size>0);
}
//---------------------------------------------------------
CODE_IN_ITCM static uint32* Find_FunctionStart_ARM(uint32 *pPoint)
{
    int i =0 ;
    pPoint -= 2 ;
    do
    {
        uint32 res = *pPoint ;
        if((res&0xFFFF0000) == 0xE92D0000)
            return pPoint ;
        pPoint -- ;
        i ++ ;
        if(i>0x100) return 0 ;
    }while(1);        
}
//---------------------------------------------------------------------------
CODE_IN_ITCM static void Find_CallARM(uint32 *Callto, uint32** plist, uint32 &listsize)
{//寻找对函数Callto 的调用
    int k =0 ;
    uint32*pBf = Callto - 0x400 ;
    int bl_fc ,call_off;
    listsize = 0;
    do
    {
        bl_fc = *(pBf + k);
        if((bl_fc&0xFF000000)==0xEB000000)
        {
            call_off = ((uint32)Callto - 8 - (uint32)(pBf + k) )/4 ;
             
            if((call_off&0xFFFFFF) == (bl_fc&0xFFFFFF))
            {
                 plist[listsize++] = (pBf + k);
            }           
        }
        k++ ;
        if(listsize==4) break;
    }
    while(k<0x800);

}
/////////////////////////////////////////////////////////
#include  "nds_patcher_game_ITCM.h"
/////////////////////////////////////////////////////////
#define  RESET_SSIZE 0x1000
#define  arm7Patch_address 0x23ff200
CODE_IN_ITCM void PackSoftRest_and_cheat(uint32* uStart,uint32* uEnd)
{
	uint32 softreset_switch = A9_ReadSram(0x7DFC0);
	uint32 cheat_switch = A9_ReadSram(0x7DFC4);
	uint32 RTS_switch = A9_ReadSram(0x7DFC8);
	
	if(cheat_switch || softreset_switch ) //只打开金手指 
	{
		uint32 *pA7Patchinfo = (uint32*)A9_ReadSram(0x7DFCC) ;
	    uint32 *parm7_interrup  = (uint32*)arm7_interrup;
	    uint32 *pAR_end  = (uint32*)AR_end;
	    uint32 *pb_arm7Reset_wait = (uint32*)b_arm7Reset_wait;
	    asm
	    {
	        bic parm7_interrup,parm7_interrup,#3
	        bic pAR_end,pAR_end,#3
	        bic pb_arm7Reset_wait,pb_arm7Reset_wait,#3
	    }	
	    uint32 *pA7_Start = (uint32*)0x2380000 ;
	    uint32 pA7_size = *(uint32*)0x27FFE3C ; 
	    uint32 *pA7End = pA7_Start + (pA7_size>>2) ;
	    uint32 *irqHandler7=0;
	    uint32 return7=0;
	    for(uint32* ii=pA7_Start;ii<pA7End;++ii)
	    {
	      if(0x380fffc==*ii&&(*(ii+1)>=0x37f8000&&*(ii+1)<0x3800000))
	      {
	        irqHandler7=ii+1;
	        return7=*irqHandler7;
			#ifdef yafeidebug
                    WriteDubugInfo((uint32)'7qri') ;
                    WriteDubugInfo((uint32)irqHandler7) ;
			#endif
	        break;
	      }
	    }
	    *(vuint32*)0x27FFF68 = return7;
	    
	    *irqHandler7 = arm7Patch_address;//跳到固定地址
        //保存arm7的中断入口,LRSS使用
        *(pA7Patchinfo) = (uint32)irqHandler7;
        *(pA7Patchinfo+1) = 4;
        *(pA7Patchinfo+2) = arm7Patch_address;
	    
	    uint32 p238CopyFrom = 0 ;
	    pA7_Start = (uint32*)0x2380000 ;
	    do
	    {
	        //搜索第一个bx  R1
	        if(((*pA7_Start & 0xFFFFFF00) == 0xe12FFF00)&&(!p238CopyFrom))
	        {

	            if((*(pA7_Start+1) & 0xFFFF0000) == 0x2380000)
	            {
	                p238CopyFrom = *(pA7_Start+1);
	                break;
	            }
	        }
	        pA7_Start++;
	    }while(1);
	    /////有一些特殊情况
	    uint32 *pA7End_temp = pA7End - 0x20;
	    do
	    {
	    	if(*pA7End_temp == 0x27E0000)//4786系列
	    	{
	    		p238CopyFrom += *(pA7End_temp+1);
	    		break;
	    	}
	    	else if(*pA7End_temp == 0x37F8000)//0232
	    	{
	    		break;
	    	}
	    	pA7End_temp++;	
	    }while(pA7End_temp<pA7End);
	    if(*(uint32*)0x2380010 == 0xE2823028) //貌似是i的游戏
	    {
	    	pA7End_temp = (uint32*)p238CopyFrom;
	    	for(uint32 ii=0;ii<0x20;ii++)
	    	{
	    		if((*(pA7End_temp+ii) & 0xFFFF0000)  == 0xE92D0000)
	    		{
	    			p238CopyFrom += (ii<<2);
	    			break;
	    		}
	    	}   	
	    }
	    
	    uint32 *pA7_Reset = 0;
	    do{
    		if((*pA7_Start == 0xE3A00701)&&(*(pA7_Start+2) == 0xE3E00000))
	    	{
    			pA7_Reset = Find_FunctionStart_ARM(pA7_Start+2);
	            if(RTS_switch)//使用即时存档,跳到直存后面的函数 
	            {
	            	//让arm7中断跳到等待
	            	uint32 b_arm7Reset_wait_in_23FFxxx = ((pb_arm7Reset_wait - parm7_interrup)<<2) + arm7Patch_address;//arm7Patch_address;
	            	uint32 arm7Reset_wait_in_37f8xx = A9_ReadSram(0x7DFAC)- p238CopyFrom + 0x37f8000 ;
	            	
	            	*pb_arm7Reset_wait = 0x0b000000 + (((arm7Reset_wait_in_37f8xx - b_arm7Reset_wait_in_23FFxxx)/4 - 2) & 0x00ffffff) ; 
	            	*(uint32*)(pb_arm7Reset_wait+1) = 0xE3A00000;
	            	
	            	uint32 *pA7WaitToReset = (uint32*)A9_ReadSram(0x7DFA8);//RTS_arm9sendcommreset          
	            	*pA7WaitToReset = 0x0b000000 + ( ( (pA7_Reset - pA7WaitToReset) - 2) & 0x00ffffff) ;
					 #ifdef yafeidebug           
					     WriteDubugInfo((uint32)'1TTT') ;
					     WriteDubugInfo((uint32)b_arm7Reset_wait_in_23FFxxx);
					     WriteDubugInfo((uint32)arm7Reset_wait_in_37f8xx);
					     WriteDubugInfo((uint32)pb_arm7Reset_wait);
					     WriteDubugInfo((uint32)*pb_arm7Reset_wait); 
					     WriteDubugInfo((uint32)'2TTT') ;
					     WriteDubugInfo((uint32)pA7_Reset); 
					     WriteDubugInfo((uint32)pA7WaitToReset); 
					     WriteDubugInfo((uint32)*pA7WaitToReset); 
					 #endif
	             }
    			else
    			{
			        //计算reset在37f8中的地址
			        uint32  u37f8_reset = (uint32)(pA7_Reset) - p238CopyFrom + 0x37f8000 ;
			        *(uint32*)fix_data = u37f8_reset;             
				 	#ifdef yafeidebug           
				     WriteDubugInfo((uint32)'ERSO') ;
				     WriteDubugInfo((uint32)p238CopyFrom);
				     WriteDubugInfo((uint32)pA7_Start);
				     WriteDubugInfo((uint32)pA7_Reset);
				     WriteDubugInfo((uint32)u37f8_reset);    
				 	#endif
    			}
				    break;
    			
	        } 
	        pA7_Start++;  
	    }while(pA7_Start<pA7End);
	    if(!pA7_Reset)
	    {//表示无法寻找到RESET函数入口,读取设置
			#ifdef yafeidebug           
	    	WriteDubugInfo((uint32)'2222') ;   
			#endif
	        pA7_Start = (uint32*)0x2380000 ;
	        do
	        {           
	            if(*pA7_Start == 0x27FFE00)
	            {
	                uint32* pRstemp =  pA7_Start ;
	                uint32  L = 0 ;
	                do
	                {
	                    pRstemp -- ;
	                    L ++ ;
	                    if(((*pRstemp&0xFFFF0000) == 0xE3A00000)&&((*(pRstemp-1)&0xFFFF0000) == 0x0))
	                    {
	                        //找到函数头
	                        uint32 *pFun[4] ;
	                        uint32  numFun =0;
	                        
	                        Find_CallARM(pRstemp,pFun,numFun) ;
	                        if(numFun)
	                        {
	                            //通常，这个就是查找到的reset
	                            pA7_Reset = Find_FunctionStart_ARM(pFun[0]);
	                            break;
	                        }
	                        else
	                        {//找不到，是37f8000 + XXXX的地址形式
	                        }
	                    }
	                        
	                }while(L<0x40);
	                
	                
	                //向上寻找的函数开始
	                break;
	            }
	            pA7_Start ++ ;
	        }while(pA7_Start<pA7End);
	    }//end if(!pA7_Reset)
	    if(softreset_switch) *(uint32*)is_softReset = 0x1;
	    copyCode(parm7_interrup,(uint8*)arm7Patch_address,(pAR_end-parm7_interrup)*4);
	    //copy cheat data
	    copyCode((uint32*)0x2370000,(uint8*)(arm7Patch_address+(pAR_end-parm7_interrup)*4),*(uint32*)0x236FFFC);

	}
	//#####################################################
	//#####################################################
	if(RTS_switch)
	{
		uint32 *pA9_Start = (uint32*)(*(uint32*)0x27FFE24) ;
		do
		{
			if((*pA9_Start== *(pA9_Start+1)) && ((*pA9_Start&0xFF000000)==0x02000000) )
			{
				#ifdef yafeidebug
				WriteDubugInfo((uint32)'sSSB') ;
				WriteDubugInfo((uint32)pA9_Start) ;
				WriteDubugInfo((uint32)*pA9_Start) ;
				#endif						  
				uint32 SDK_AUTOLOAD_START = *pA9_Start-0x2000;
				ds_WriteSram(0x7DF9C,SDK_AUTOLOAD_START);
				ds_WriteSram(0x7DF9E,SDK_AUTOLOAD_START>>16);
				//if(*pA9_Start  != 0xE12FFF1E)
				break;
			}
			pA9_Start++;
		}
		while(pA9_Start<uEnd);
	}
	//#####################################################
	//#####################################################
	if(softreset_switch)
	{
	    uint32 *pA9_Start = (uint32*)(*(uint32*)0x27FFE24) ; //开始执行程序的地方
	    uint32 *pStackSP = 0;
	    uint32 *p1ff8XXX = 0 ;
	    bool isDSihybrid = false;
		//*(vuint32*)0x2000030 = 0x1; //软复位开关标记
	     
	    do
	    {
	    	if(*pA9_Start == 0xFFFF0000)
	    	{
	        	if(*(pA9_Start-2) == 0x4000006 )
	        	{
	        		isDSihybrid = true;
	        		p1ff8XXX = pA9_Start-3;
	        		break;
	        	}
	        	else
	        	{
	        		p1ff8XXX = pA9_Start-2; 
	        		do
	        		{      		     			
	        			if((*pA9_Start & 0xFFFFFF00 ) == 0xE12FFF00)//寻找 BX R1
	        			{
	        				uint32  pStack =  *(pA9_Start+1);
	        				if((pStack &0xFF000000)==0x2000000)
	        				{
	        					//这里可能是0x27XXXXX 或者是23XXXXX
	        					if(pStack>0x2400000)
	        						pStackSP = (uint32*)(pStack - 0x400000) ;
	        					else
	        						pStackSP = (uint32*)(pStack + 0x400000) ;
	                            
	        					break;
	        				}
	        			}
	        			pA9_Start--;	
	        		}while(1);
	        		break;
	        	}
	    	}
	    	pA9_Start ++ ;
	    }while(1);
	    //中断替换代码起始和结束的地址
	     uint32 * pA9irq_patch  = (uint32*)ReplaceIRQ_start_arm9;
	     uint32 * pA9irq_patch_end  = (uint32*)ReplaceIRQ_end_arm9;
	     {//使用即时存档 
	     	if(RTS_switch)
	     	{
	     		pA9irq_patch  = (uint32*)RTS_ReplaceIRQ_start;
	     		pA9irq_patch_end  = (uint32*)RTS_ReplaceIRQ_end;	
	     	}
	     }
	     asm
	     {
	         bic pA9irq_patch,pA9irq_patch,#3
	         bic pA9irq_patch_end,pA9irq_patch_end,#3
	     }
	     *(pA9irq_patch_end-1) = *p1ff8XXX ;
	     
	     //准备搜索，继续向下搜素
	     uint32 pResetAdd = 0 ;
	     uStart =(uint32*)0x2000800 ;
	     
	    if(A9_ReadSram(0x7DFB0) != 0xF5A05259)//采用强制指定地址
	    {
	     do
	     {
	         uint32 res_378 = *uStart ;
	         uint32 res_27ff = *(uStart - 1);
	         
	         if(isDSihybrid)
	         {
	        	 if((res_378==0x2FFF680)/*&&((res_27ff&0xFF000000)==0x2700000)*/)
	        	 {
		             //这里找到OS_GetInitArenaHi 函数，需要修改 mainram分配内存的最高地址
		             //从这里向上寻找 
		             int size = 0x10 ;
		             uint32 *p37f8 = uStart ;
		             do
		             {
		                 uint32 fn_end = *p37f8 ;
		                 if((fn_end&0xFFF00000)==0x02300000)
		                 {
		 					#ifdef yafeidebug
		                     WriteDubugInfo((uint32)'2nuf') ;
		                     WriteDubugInfo((uint32)p37f8) ;
		 					#endif
		                     if((*(p37f8-1)&0xFFF00000) == 0x02300000)
		                     {
		                         p37f8 -- ;
		                         fn_end = *p37f8 ;
		                     }
		                     pResetAdd = fn_end ;
		                     *p37f8 = fn_end - RESET_SSIZE ;
		                     //登记写入
		                     WritePatchInfo_4BYTE((uint32)p37f8,fn_end - RESET_SSIZE );
		                     break;
		                 }
		                 p37f8 -- ; 
		                 size -- ;
		             }while(size>0);
	        	 }
	         }//end if()
	         else if( (res_378==0x37f8000) &&  ( ((res_27ff&0xFFFFF000)==0x027FF000) || ((res_27ff&0xFFFFF000)==0x02FFF000)) )
	         {
	             if(res_27ff<0x27FF100)
	             {
	                 uStart++ ;
	                 continue ;
	             }

	             //这里找到OS_GetInitArenaHi 函数，需要修改 mainram分配内存的最高地址
	             //从这里向上寻找 
	             int size = 0x10 ;
	             uint32 *p37f8 = uStart ;
	             do
	             {
	                 uint32 fn_end = *p37f8 ;

	                 if((fn_end&0xFFF00000)==0x02300000)
	                 {
	 					#ifdef yafeidebug
	                     WriteDubugInfo((uint32)'dnuf') ;
	                     WriteDubugInfo((uint32)p37f8) ;
	 					#endif
	                     if((fn_end&0xFFFF0000)==0x023C0000)
	                     {//0138/0183类似的游戏,是23c0000,且是共用的
	 					#ifdef yafeidebug
	                     WriteDubugInfo((uint32)'C320') ;
	                     WriteDubugInfo((uint32)fn_end) ;
	 					#endif
	                     	pResetAdd = fn_end+0x2000;
	                     	break;
	                     }
	                     if((*(p37f8-1)&0xFFF00000) == 0x02300000)
	                     {
	                         p37f8 -- ;
	                         fn_end = *p37f8 ;
	                     }
	                     pResetAdd = fn_end ;
	                     *p37f8 = fn_end - RESET_SSIZE ;
	                     //登记写入
	                     WritePatchInfo_4BYTE((uint32)p37f8,fn_end - RESET_SSIZE );
	                     break;
	                 }
	                 p37f8 -- ; 
	                 size -- ;
	             }while(size>0);
	            
	             if(size<1)
	             {
	                 //表示这个不是采用LDR R0，=0x23XXXXX形式
	                 size = 0x40 ;
	                 do
	                 {
	                     uint32 fn_end = *p37f8 ;
	                     if(fn_end == 0xE3A0078F) //mov r0,#0x23C0000
	                     {//这个怎么修改
	 						#ifdef yafeidebug
	                         WriteDubugInfo((uint32)'xc32') ;
	                         WriteDubugInfo((uint32)p37f8) ;
	 						#endif
	                        
	                         gl_BACKROR +=4 ; //向后移，防止错写
	                         uint32*p = (uint32*)gl_BACKROR ;
	  
	                         *p = 0xe59f0004 ;
	                         *(p+1)=*(p37f8+1);
	                         *(p+2)=*(p37f8+2);
	                         *(p+3)=0x23bC000 ;
	                         
	                         WritePatchInfo_4BYTE((uint32)p,0xe59f0004);
	                         WritePatchInfo_4BYTE((uint32)(p+1),*(p37f8+1));
	                         WritePatchInfo_4BYTE((uint32)(p+2),*(p37f8+2));
	                         WritePatchInfo_4BYTE((uint32)(p+3),0x23bC000);

	                         int  s = (int)(p-p37f8) - 2 ;
	                         *p37f8 = 0xea000000 + (s&0xFFFFFF) ;
	                         WritePatchInfo_4BYTE((uint32)p37f8,*p37f8);
	                         
	                         pResetAdd = 0x23C0000 ;
	                         break;
	                     }
	                     p37f8 -- ; 
	                     size -- ;
	                 }while(size>0);
	                 
	             }
	             
	             break;
	         }
	         uStart++ ;
	     }while(uStart<uEnd);
	     }
	    else 
	    {
	        pResetAdd = A9_ReadSram(0x7DFB4) + RESET_SSIZE; 
	    }
	 #ifdef yafeidebug
	     WriteDubugInfo((uint32)'TSER') ;
	     WriteDubugInfo((uint32)pResetAdd) ;
	     WriteDubugInfo((uint32)uStart) ; 
	     WriteDubugInfo((uint32)uEnd) ; 
	     WriteDubugInfo((uint32)pStackSP) ; 
	     WriteDubugInfo((uint32)p1ff8XXX) ; 
	     WriteDubugInfo((uint32)*p1ff8XXX) ; 
	     WriteDubugInfo((uint32)*(pA9irq_patch_end-1) ) ;
	     WriteDubugInfo((uint32)gl_BACKROR ) ;
	     WriteDubugInfo((uint32)&gl_BACKROR ) ;  
	 #endif
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
skipAM7:
	      if(pResetAdd)
	      {
	             pStackSP = (uint32*)(pResetAdd-RESET_SSIZE) ;
	             
	             copyCode(pA9irq_patch,pStackSP,(pA9irq_patch_end-pA9irq_patch)*4);
	             
	             *p1ff8XXX = (uint32)(pStackSP) + 1 ;
	             WritePatchInfo_4BYTE((uint32)p1ff8XXX,(uint32)(pStackSP) + 1 );             
	      } 
	}

}
