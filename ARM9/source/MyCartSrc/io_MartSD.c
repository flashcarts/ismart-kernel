#include "io_MartSD.h"
#include "MartCartop.h"
#include "stdlib.h"
#include "string.h"
#include "_console.h"

static bool  bSDHC =false;
static uint32 SDadd ;

bool IsSDHC()
{
    return bSDHC ;
}

void presskey()
{
   // while(*(vuint16*)0x4000130 == 0x3FF);
   // while(*(vuint16*)0x4000130 != 0x3FF);
}

//Function: CRC:X^16+X^12+X^5+1
unsigned int cal_crc_CCITT(unsigned char *ptr,unsigned char len)
{
    unsigned char i;
    unsigned int  crc=0;
    while(len--!=0)
    {
        for(i=0x80;i!=0;i/=2)
        {
            if((crc & 0x8000)!=0)
            {
                crc*=2;
                crc^=0x1021;
            }       
            else
            {
                crc*=2;
            }
            if((*ptr & i)!=0)
                crc ^= 0x1021;
        }
        ptr++;
    }
    return crc;
}

asm unsigned char cal_crc_730(unsigned char *ptr,unsigned char len)
{
;#r0 - buf , r1 size   
    stmfd   sp!,{r2-r4}
    mov     r4,#0               //; @  crc
crc_l1
    mov     r2,#0x80
    ldrb        r3,[r0],#+1
crc_l2
    tst     r4,#0x40
    mov     r4,r4,lsl #1
    eorne   r4,r4,#0x9
    tst     r3,r2
    eorne   r4,r4,#0x9
    movs    r2,r2,lsr #1
    bne     crc_l2
    subs        r1,r1,#1
    bne         crc_l1
    mov     r0,r4
    ldmfd   sp!,{r2-r4}
    bx      lr
}

asm unsigned short SD_cal_crc16(unsigned char *ptr,unsigned int len,unsigned char *buf2)
{
    stmfd   r13!, {r3 - r9}
    mov r9, r2

    mov r3, #0x0
    mov r4, #0x0
    mov r5, #0x0
    mov r6, #0x0

    ldr r7, =0x80808080
    ldr r8, =0x1021
    mov r1, r1, lsl #0x3

_crc16_lp
    tst r7, #0x80
    ldrneb  r2, [r0], #0x1

    mov r3, r3, lsl #1
    tst r3, #0x10000
    eorne   r3, r3, r8
    tst r2, r7, lsr #24
    eorne   r3, r3, r8

    mov r4, r4, lsl #1
    tst r4, #0x10000
    eorne   r4, r4, r8
    tst r2, r7, lsr #25
    eorne   r4, r4, r8

    mov r5, r5, lsl #1
    tst r5, #0x10000
    eorne   r5, r5, r8
    tst r2, r7, lsr #26
    eorne   r5, r5, r8
    
    mov r6, r6, lsl #1
    tst r6, #0x10000
    eorne   r6, r6, r8
    tst r2, r7, lsr #27
    eorne   r6, r6, r8

    mov r7, r7, ror #4
    subs    r1, r1, #4
        bne     _crc16_lp 

    mov r2, r9
    mov r8, #16

_crc16_write_data
    mov r7, r7, lsl #0x4
    tst r3, #0x8000
    orrne   r7, r7, #0x8
    tst r4, #0x8000
    orrne   r7, r7, #4
    tst r5, #0x8000
    orrne   r7, r7, #0x2
    tst r6, #0x8000
    orrne   r7, r7, #0x1

    mov r3, r3, lsl #0x1
    mov r4, r4, lsl #0x1
    mov r5, r5, lsl #0x1
    mov r6, r6, lsl #0x1

    sub r8, r8, #0x1
    tst r8, #0x1
    streqb  r7, [r2], #1
    cmp r8, #0
    bne _crc16_write_data

    ldmfd   r13!, {r3-r9}
    bx      r14
}

uint32  SD_SendCommand (int type , unsigned int param )
{
    uint8 command[8];
    command[0]= (param)&0xFF;
    command[1]= (param>>8)&0xFF;
    command[2]= (param>>16)&0xFF;
    command[3]= (param>>24)&0xFF;
    command[4]= 0x40 | (type&0xFF);
    command[5]= 0x00;
    command[6]= 0xAA; 
    command[7]= 0xB9;
    return Mart_Read4BYTE(command);
}

bool SD_ReadResponse(unsigned char *ppbuf,int len)
{
    uint8 command[8];
    uint32 status ;
    int counterFA01=0;
    uint8 *p = (uint8 *)(&status);
    
    //等待起始标志位置
    WAIT_CR &= ~0x0800;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x01;
    command[6]= 0xAA; 
    command[7]= 0xB9;
    
    do
    {
        status = Mart_Read4BYTE(command);
        counterFA01++;
        if(counterFA01>99)   ///////modify 2008-12-16
            return false;
    }while(status & 0x000000FF);
    ppbuf[0] = p[1];
    ppbuf[1] = p[2];
    ppbuf[2] = p[3];
   
    if(len==6)
    {
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x02;
        command[6]= 0xAA; 
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
        
        ppbuf[3] = p[0];
        ppbuf[4] = p[1];
        ppbuf[5] = p[2];
        ppbuf[6] = p[3];
    }
    else if(len==17)
    {   
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x02;
        command[6]= 0xAA; 
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
        
        ppbuf[3] = p[0];
        ppbuf[4] = p[1];
        ppbuf[5] = p[2];
        ppbuf[6] = p[3];
        
        
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x03;
        command[6]= 0xAA; 
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
        ppbuf[7] = p[0];
        ppbuf[8] = p[1];
        ppbuf[9] = p[2];
        ppbuf[10] = p[3];
        
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x04;
        command[6]= 0xAA; 
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
        ppbuf[11] = p[0];
        ppbuf[12] = p[1];
        ppbuf[13] = p[2];
        ppbuf[14] = p[3];
        
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x05;
        command[6]= 0xAA; 
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
        ppbuf[15] = p[0];
        ppbuf[16] = p[1];
    }   
    return true;
}

bool SD_R16Response(unsigned char *ppbuf)
{
    return SD_ReadResponse(ppbuf,6);
}

bool SD_R2Response(unsigned char *ppbuf)
{
    return SD_ReadResponse(ppbuf,17);
}

void SD_ReadLoop()
{//空读
    uint32 i;
    uint32 target = 0x80;
    WAIT_CR &= ~0x0800;
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x06;
    command[6]= 0xAA;
    command[7]= 0xB9;
    cardWriteCommand(command);
    CARD_CR2 = 0xA1586000 ;
    i=0;
    do {
        // Read data if available
        if (CARD_CR2 & CARD_DATA_READY) {
            if (i< target) 
            {
                uint32 temp = CARD_DATA_RD;
            }
            i++;
        }
    } while (CARD_CR2 & 0x80000000);
}

bool wait_SD()
{
    uint32 status ;
    WAIT_CR &= ~0x0800;
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x00;
    command[6]= 0xF8;
    command[7]= 0xB8;
    do{
        status = Mart_Read4BYTE(command);
    }while(!(status & 0x1));
    return true;  
}


bool SD_WaitOK()
{
    //这段代码……貌似无用
    uint32 status ;
    WAIT_CR &= ~0x0800;
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x01;
    command[6]= 0xAA; 
    command[7]= 0xB9;
    do{
        status = Mart_Read4BYTE(command);
    }while(status & 0x000000FF);
    return true;
}

static uint32 ss = 0 ;
bool SD_ReadData(unsigned char *ppbuf, int len,int wait)
{
    uint32 i=0;
    uint8 command[8];
    uint32 target = 512;
    uint32 status ;
 
    //等待起始标志位置
    
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x01;
    command[6]= 0xAA; 
    command[7]= 0xB9;
    do{
        status = Mart_Read4BYTE(command);
        wait -- ;
        if(wait<0)
        {
            WAIT_CR &= 0x880 ;
            _consolePrintf("the read at sectors %x is dead !\n",ss) ;
            return  false;
        }
    }while(status & 0x000000FF);
    //读512 Byte数据
    WAIT_CR &= ~0x0800;
  
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x06;
    command[6]= 0xAA;
    command[7]= 0xB9;
    Mart_SetRomOP(command);
    CARD_CR2 = 0xA1586000 ;
    if((uint32)ppbuf&0x3)
    {
        uint32 temp ;
        uint8* p = (uint8*)&temp ;
        target = target>>2;
        i = 0;
        do {
        // Read data if available
            if (CARD_CR2 & CARD_DATA_READY) {
                if (i< target) 
                {
                    temp = CARD_DATA_RD;
                    ppbuf[i*4] = *p;
                    ppbuf[i*4+1] = *(p+1);
                    ppbuf[i*4+2] = *(p+2);
                    ppbuf[i*4+3] = *(p+3);
                }
                i+=1;
            }
        } while (CARD_CR2 & 0x80000000);
    }
    else
    {
        i = 0;
        do {
        // Read data if available
            if (CARD_CR2 & CARD_DATA_READY) {
                if (i< target) 
                {
                    *((uint32 *)(&ppbuf[i])) = CARD_DATA_RD;
                }
                i+=4;
            }
        } while(CARD_CR2 & 0x80000000);
    }
    
    return true ;
}


bool SD_ReadSingleBlock(unsigned int address , unsigned char *ppbuf, int len)
{
 
    ss = address ;
    SD_SendCommand(17,address); // single block  read , parm = address
    SD_ReadData(ppbuf,len,0x10000); 
    return true ;
}

void SD_WriteData(unsigned char *ppbuf, int len,int wait)
{
    uint32 status;
    //BYTE ii;
    WAIT_CR &= ~0x0800;
    uint8 command[8];
    int i=0;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0xF0;
    command[3]= 0xFF;
    command[4]= 0xFF;
    command[5]= 0xFF;
    command[6]= 0xA6; 
    command[7]= 0xB9;
    status = Mart_Read4BYTE(command);

    WAIT_CR &= ~0x0800;
   for(i=0;i<len;i+=5)
    {
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= ppbuf[i+4];
        command[3]= ppbuf[i+3];
        command[4]= ppbuf[i+2];
        command[5]= ppbuf[i+1];
        command[6]= ppbuf[i]; 
        command[7]= 0xBC;
        cardWriteCommand(command);
        CARD_CR2 = 0xA0586000 ;
        status = CARD_CR2; //0x40001a4
    }
    do
    {
        WAIT_CR &= ~0x0800;
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x00;
        command[6]= 0xF8;
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
    }while(status & 0x00000001);

//读CRC状态
    WAIT_CR &= ~0x0800;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x00;
    command[6]= 0xF8;
    command[7]= 0xB9;
    status = Mart_Read4BYTE(command);
    
    do
    {
        WAIT_CR &= ~0x0800;
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x00;
        command[6]= 0xF8;
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
    }while((status & 0x00000001)!=0x01);
}

void SD_WriteData_slow(unsigned char *ppbuf, int len,int wait)
{
    uint32 status;
    //BYTE ii;
    WAIT_CR &= ~0x0800;
    uint8 command[8];
    int i=0;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0xF0;
    command[3]= 0xFF;
    command[4]= 0xFF;
    command[5]= 0xFF;
    command[6]= 0xA6; 
    command[7]= 0xB9;
    status = Mart_Read4BYTE(command);

    WAIT_CR &= ~0x0800;
    for(i=0;i<len;i+=2)
     {
         command[0]= 0x00;
         command[1]= 0x00;
         command[2]= ppbuf[i+1] | 0xF0;
         command[3]= (ppbuf[i+1]>>4) | 0xF0;
         command[4]= ppbuf[i] | 0xF0;
         command[5]= (ppbuf[i]>>4) | 0xF0;
         command[6]= 0xA6; 
         command[7]= 0xB9;
        cardWriteCommand(command);
        CARD_CR2 = 0xA0586000 ;
        status = CARD_CR2; //0x40001a4
    }
    do
    {
        WAIT_CR &= ~0x0800;
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x00;
        command[6]= 0xF8;
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
    }while(status & 0x00000001);

//读CRC状态
    WAIT_CR &= ~0x0800;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x00;
    command[6]= 0xF8;
    command[7]= 0xB9;
    status = Mart_Read4BYTE(command);
    
    do
    {
        WAIT_CR &= ~0x0800;
        command[0]= 0x00;
        command[1]= 0x00;
        command[2]= 0x00;
        command[3]= 0x00;
        command[4]= 0x00;
        command[5]= 0x00;
        command[6]= 0xF8;
        command[7]= 0xB9;
        status = Mart_Read4BYTE(command);
    }while((status & 0x00000001)!=0x01);
}

bool    SD_WriteSingleBlock(unsigned int address , unsigned char *ppbuf, int len)
{
    u8  pbuf[528] __attribute__ ((aligned (4)));
     unsigned char pres[40] ;
    memcpy(pbuf, ppbuf, 512);
    SD_cal_crc16(pbuf, 512, pbuf + 512);
    
    SD_SendCommand(24,address); 
    SD_R16Response(pres); 

    //这里是随便加一个读，解决时间问题
    uint8 command[8];   
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x01;
    command[6]= 0xAA;
    command[7]= 0xB9;   
    Mart_Read4BYTE(command);
    
    WAIT_CR &= ~0x0800;
    SD_WriteData(pbuf,520,0x10000);
    SD_WaitOK();
    return true ;
}

bool  WaitCmd_return00()
{
    uint32 loop= 8;
    uint8 command[8];
    //等待起始标志位置
    WAIT_CR &= ~0x0800;
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0x00;
    command[4]= 0x00;
    command[5]= 0x01;
    command[6]= 0xAA;
    command[7]= 0xB9;
    do
    {
        uint32 temp = Mart_Read4BYTE(command);
        loop --;
    }while(loop);   
    return true;
}

bool SD_R3Response(unsigned char *ppbuf)
{
    bool ret ;
    ret = SD_ReadResponse(ppbuf,6);
    if(ret)
    {
        if((ppbuf[0]!=0x3F)&&(ppbuf[5]!=0xFF))
            return false ;
        return true ;
    }

    return false ;
}

void delay(int times)
{
    
    for(int i=0;i<times;i++)
    {
        i++;
        i--;
    }
}

bool SD_initial()
{
    bool  ret ;
    int   kk ;
    unsigned char pres[40] ;

    SD_ReadLoop();
    SD_SendCommand(0,0);
    SD_ReadLoop();
    
    WaitCmd_return00();//add 2008-12-16
    SD_ReadLoop();
 
    {
         for( kk=0;kk<8;kk++)
            pres[kk]=0;
 
         _consolePrintf("before cmd8 , press to continue \n");
         presskey();
         //发送CMD8，检测SDHC
        SD_SendCommand(8,0x1AA);
        SD_R16Response(pres);
        _consolePrintf("responde 0[%x] 1[%x] 5[%x] \n",pres[0],pres[1],pres[5]);
        if(pres[0] == 8)
            ret = true ;
        else
            ret = false ;
        
        //ret = true ;
    }
    if(ret==false)
    {
        _consolePrintf("enter SD process \n");
        //SD_SendCommand(0,0);
        //WaitCmd_return00();//add 2008-12-16

        //SD卡
        SD_SendCommand(55,0);   
        ret = SD_R16Response(pres);   
        if(pres[0]!=55)
            ret = false ;   
        if(ret)
        {
            SD_SendCommand(41,0x0); 
            ret = SD_R16Response(pres);
            do
            {
                SD_SendCommand(55,0);
                ret = SD_R16Response(pres);
                SD_SendCommand(41,0x800000);
                ret = SD_R3Response(pres);
            }while(pres[1]!=0x80) ;
        }
        _consolePrintf("before cmd3 is cmd2 , press to continue \n");
        presskey();
        
        if(ret)
        {
            SD_SendCommand(2,0);    //Get CID 
            ret = SD_R2Response(pres);
            //SD_GetCIDStruct(pres,17,&cid);
        }
        
        
        if(ret)
        {
            do
            {
                SD_SendCommand(3,0);        
                ret = SD_R16Response(pres);
            }while((pres[3]&0x1E) != 0x6); //stand by state
        }    
        if(ret)
        {
            SDadd = pres[1]*0x100 + pres[2] ;
            SD_SendCommand(9,(SDadd<<16));//send_csd
            ret = SD_R2Response(pres);
            //SD_GetCSDStruct(pres,17,&csd);
        }
        if(ret)
        {
            SD_SendCommand(7,SDadd<<16);    //select card
            ret = SD_R16Response(pres);   
        }
        if(ret)
        {
            SD_SendCommand(55,SDadd<<16); //app command
            ret = SD_R16Response(pres);
            SD_SendCommand(6,2);        //00, 1 bit , 10  4 bit 
            ret = SD_R16Response(pres);
        }
        if(ret)
        {
            SD_SendCommand(16,0x200) ;  
            ret = SD_R16Response(pres);
        }
        _consolePrintf("initial SD %x Ok %x\n",bSDHC,ret);
    }
    else
    {
        _consolePrintf("enter SDHC process \n");
        //SDHC卡
        u32 retry = 0;
         delay(0x1000);
        SD_SendCommand(55,0);
        ret = SD_R16Response(pres);
        delay(0x1000);  
        ret = SD_R16Response(pres);
        if(ret)
        {
            SD_SendCommand(41,0x40ff8000);
            ret = SD_R16Response(pres);
            delay(0x1000);
            do
            {
                SD_SendCommand(55,0);
                ret = SD_R16Response(pres);
                delay(0x1000);
                SD_SendCommand(41,0x40ff8000);
                ret = SD_R3Response(pres);
                delay(0x1000);
                retry++;
                if(retry>1000)
                {
                    bSDHC = true;
                    break;
                }
            }while(pres[1]!=0x80) ;
        }
      
        _consolePrintf("sdhc run=  %d\n",2);
        _consolePrintf("before cmd3 is cmd2 , press to continue \n");
        presskey();

        do
        {
             SD_SendCommand(2,0);    //Get CID 
            ret = SD_R2Response(pres);
        }while(ret==false);
        
        //SD_GetCIDStruct(pres,17,&cid);
        if(ret)
        {
            do
            {
                SD_SendCommand(3,0);
                ret = SD_R16Response(pres);
            }while((pres[3]&0x1E) != 0x6); //stand by state
        }
        if(ret)
        {
            SDadd = pres[1]*0x100 + pres[2] ;
            SD_SendCommand(9,(SDadd<<16));//send_csd
            ret = SD_R2Response(pres);
            //SD_GetCSDStruct(pres,17,&csd);
        }
        if(ret)
        {
            SD_SendCommand(7,SDadd<<16);    //select card
            ret = SD_R16Response(pres);   
        }
        if(ret)
        {
            SD_SendCommand(55,SDadd<<16); //app command
            ret = SD_R16Response(pres);
            SD_SendCommand(6,2);        //00, 1 bit , 10  4 bit 
            ret = SD_R16Response(pres);
        }
        if(ret)
        {
            SD_SendCommand(16,0x200) ;
            ret = SD_R16Response(pres);
        }
        _consolePrintf("initial SDHC %x Ok %x\n",bSDHC,ret);
    }
    return ret ;
}

bool SD_ReadMultiBlock(uint32 address , unsigned char *ppbuf, int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        if(bSDHC==true)
            SD_ReadSingleBlock((address+i),ppbuf+i*0x200,0x200);
        else
            SD_ReadSingleBlock((address+i)*0x200,ppbuf+i*0x200,0x200);
    }
    return true;    
}
//======================================================
bool MartSD_read1sector(u32 sectorn,u32 TAddr)
{
    
    return true;
    
} 
//==================================================


//======================================================
bool MartSD_write1sector(u32 sectorn,u32 TAddr)
{
    return true;
} 
//==================================================

/*-----------------------------------------------------------------
MartSD_IsInserted
Is a compact flash card inserted?
bool return OUT:  true if a CF card is inserted
-----------------------------------------------------------------*/
bool MartSD_IsInserted (void) 
{
    return true;
}


/*-----------------------------------------------------------------
MartSD_ClearStatus
Tries to make the CF card go back to idle mode
bool return OUT:  true if a CF card is idle
-----------------------------------------------------------------*/
bool MartSD_ClearStatus (void) 
{
    return true;
}


/*-----------------------------------------------------------------
MartSD_ReadSectors
Read 512 byte sector numbered "sector" into "buffer"
u32 sector IN: address of first 512 byte sector on CF card to read
u8 numSecs IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read, 0 = 256
void* buffer OUT: pointer to 512 byte buffer to store data in
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool MartSD_ReadSectors (u32 sector, u8 numSecs, void* buffer)
{
    SD_ReadMultiBlock(sector,(u8*)buffer,numSecs);
    return true;
}

/*-----------------------------------------------------------------
MartSD_WriteSectors
Write 512 byte sector numbered "sector" from "buffer"
u32 sector IN: address of 512 byte sector on CF card to read
u8 numSecs IN: number of 512 byte sectors to read,
 1 to 256 sectors can be read, 0 = 256
void* buffer IN: pointer to 512 byte buffer to read data from
bool return OUT: true if successful
-----------------------------------------------------------------*/
bool MartSD_WriteSectors (u32 sector, u8 numSecs, void* buffer)
{
    /*
    int i;
    for(i=0;i<len;i+=0x200)
    {
        if(bSDHC==true)
            SD_ReadSingleBlock((address+i)/512,ppbuf+i,0x200);
        else
            SD_ReadSingleBlock(address+i,ppbuf+i,0x200);
    }
    */
    int i;
    for(i=0;i<numSecs;i++)
    {
        if(bSDHC==true)
            SD_WriteSingleBlock( (i+sector),(u8*)buffer+i*512,0x200);
        else
            SD_WriteSingleBlock( (i+sector)*512,(u8*)buffer+i*512,0x200);
    }
    return true;
}
bool MartSD_Shutdown(void) {
    return true;
} ;

bool MartSD_StartUp(void) {
    //init sd 
    return SD_initial();
} ;


IO_INTERFACE io_martsd = {

        'MA_I', 
    FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE|FEATURE_SLOT_GBA,
    (FN_MEDIUM_STARTUP)&MartSD_StartUp,
    (FN_MEDIUM_ISINSERTED)&MartSD_IsInserted,
    (FN_MEDIUM_READSECTORS)&MartSD_ReadSectors,
    (FN_MEDIUM_WRITESECTORS)&MartSD_WriteSectors,
    (FN_MEDIUM_CLEARSTATUS)&MartSD_ClearStatus,
    (FN_MEDIUM_SHUTDOWN)&MartSD_Shutdown
} ;


LPIO_INTERFACE MartSD_GetInterface(void) 
{
    return &io_martsd ;
} 
//---------------------------------------
