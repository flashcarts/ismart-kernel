#include "MartCartop.h"

//---------------------------------------------------
void Mart_Arm9Access()
{
    WAIT_CR &= ~0x0880;
}

void Mart_Arm7Access()
{
    WAIT_CR |= 0x0880;
}

void Mart_SetRomOP(uint8 * command) 
{
    uint32 status ,index;
    do
    {
        status = CARD_CR2 ;
    }while(status&0x80000000);  
    // 0-4  Not used            (always zero)
    //5    SPI Enable          (Both Bit13 and Bit15 must be set for SPI)
    //6    Transfer Ready IRQ  (0=Disable, 1=Enable) (for ROM, not for AUXSPI)
    //7    NDS Slot Enable     (0=Disable, 1=Enable) (for both ROM and AUXSPI)   
    CARD_CR1H = CARD_CR1_IRQ|CARD_CR1_ENABLE ;

    for (index = 0; index < 8; index++) {
        CARD_COMMAND[7-index] = command[index];
    }
}

uint32      Mart_Read4BYTE(uint8 * command,uint16 wait)
{
    uint32 status=0;
    Mart_Arm9Access();
    Mart_SetRomOP(command);

    CARD_CR2 = 0xA7586000 + wait ;

    do{
        status = CARD_CR2;
    }while(!(status & 0x800000));

    uint32 data = CARD_DATA_RD ;
    Mart_Arm7Access();
    return data ;

}
void Mart_Read0BYTE(uint8 * command,uint16 wait)
{
    uint32 status=0;
    Mart_SetRomOP(command);
    CARD_CR2 = 0xA0586000;
	status = CARD_CR2;
}
uint32 Mart_Read512BYTE(uint8 * command,uint8* pbuf,uint32 wait)
{
    uint32 status=0 ;
    Mart_Arm9Access();
    Mart_SetRomOP(command);
 
    CARD_CR2 = 0xA1586000+wait ;

    register uint32 i  = 0 ;
    if((uint32)pbuf&3)
    {
        
    }
    else
    {
        do{
            status = CARD_CR2;
            if((status & 0x800000)&&(i<0x200))
            {
                ((uint32*)pbuf)[i] = CARD_DATA_RD ;
                i ++ ;
            }
        }while(status & 0x80000000);
    }
    Mart_Arm7Access();    
    return 0 ;
}

void    cardWriteCommand(uint8 * command)
{
    int index;

    CARD_CR1H = CARD_CR1_ENABLE | CARD_CR1_IRQ;

    for (index = 0; index < 8; index++) {
        CARD_COMMAND[7-index] = command[index];
    }
}
uint32  Mart_ReadSram4byte(uint32 address)
{
	if(address>0x7D000)
		address = address & 0x7FF ;
	else if(address >= 0x800)
		return 0 ;
    uint8 cmd[8];
    cmd[7] = 0xB5;
    cmd[6] = 0x1;
    cmd[5] = (address >> 16) & 0xff;
    cmd[4] = (address >> 8) & 0xff;
    cmd[3] = address & 0xff;
    cmd[2] = 0;
    cmd[1] = 0;
    cmd[0] = 0;
    return Mart_Read4BYTE(cmd);
}
uint32 dsCardi_WriteSram(uint32 address,uint16 data)
{
	if(address>0x7D000)
		address = address & 0x7FF ;
	else if(address > 0x800)
		return 0 ;
    uint8 command[8];
    command[0]= data>>8;
    command[1]= data;
    command[2]= 0x00;
    command[3]= address;
    command[4]= address>>8;
    command[5]= address>>16;
    command[6]= 0xF4;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}
uint32 Mart_WriteSram(uint32 address,uint16 data)
{
	if(address>0x7D000)
		address = address & 0x7FF ;
	else if(address > 0x800)
		return 0 ;
    uint8 command[8];
    command[0]= data>>8;
    command[1]= data;
    command[2]= 0x00;
    command[3]= address;
    command[4]= address>>8;
    command[5]= address>>16;
    command[6]= 0xF4;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}

uint32 Mart_SetRompage(uint32 address)
{
    uint32 page= address/0x20000 ;
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x55;
    command[2]= 0xaa;
    command[3]= 0x55;
    command[4]= page;
    command[5]= 01;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}

uint32 Mart_ReadFlashCommand(uint32 address)
{
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= address;
    command[4]= address>>8;
    command[5]= address>>16;
    command[6]= address>>24;
    command[7]= 0xB5;
    return Mart_Read4BYTE(command);
}
uint32  Mart_ReadFlash(uint32 address, uint8* pbuf)
{
    uint8 cmd[8];
    cmd[7] = 0xB5;
    cmd[6] = 0;
    cmd[5] = (address >> 16) & 0xff;
    cmd[4] = (address >> 8) & 0xff;
    cmd[3] = address & 0xff;
    cmd[2] = 0;
    cmd[1] = 0;
    cmd[0] = 0;
    return Mart_Read512BYTE(cmd,pbuf);
}
uint32 Mart_SectorPerClus(uint8 SecNO)
{
    uint8 command[8];
    command[0]= 0xAA;
    command[1]= 0x55;
    command[2]= 0xAA;
    command[3]= 0x55;
    command[4]= SecNO;
    command[5]= 0x03;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}


uint32 Mart_SetFattableBase(uint8 base)
{
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x55;
    command[2]= 0xAA;
    command[3]= 0x55;
    command[4]= base;
    command[5]= 0x04;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}

uint32 Mart_SetCardUseSDHC(uint8 base)
{
    uint8 command[8];
    command[0]= 0xaa;
    command[1]= 0x55;
    command[2]= 0xAA;
    command[3]= 0x55;
    command[4]= base;
    command[5]= 0x05;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}
///////////////////////////////////////////////
uint32 Mart_ReadFirmwareVer()
{
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x55;
    command[2]= 0xaa;
    command[3]= 0x55;
    command[4]= 0x00;
    command[5]= 0x07;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}
uint32  Mart_SetHeaderCleanSRAMRead(u32 start ,u32 size)
{
    //先copy一端代码到psram中，设置此函数是为了仿造一些游戏进行代码检测防盗版
    // start 是psram的开始地址，size是 copy的数据大小
    // 特别的， 读的地址<0x1000 ,直接读地址 ，如果读的地址>0x4000 ,则读SRam地址 - 0x3000
    //传送前的数据是原始的数据，这里将它转换一下
    uint8 cmd[8] ;
    u16 start_SPI = (start>>8)&0xFFFF ;
    u16 size_sram = (size >> 12)&0xFF ;
    cmd[7] = 0xB4;
    cmd[6] = 0x00;
    cmd[5] = 0x8;
    cmd[4] = (start_SPI >> 8) & 0xff;
    cmd[3] = start_SPI & 0xff;
    cmd[2] = 0;
    cmd[1] = size_sram;
    cmd[0] = 0;    
    return Mart_Read4BYTE(cmd);
}
uint32 Mart_FPGA_IDLO()
{//04 01 xx xx 返回值
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x55;
    command[2]= 0xaa;
    command[3]= 0x55;
    command[4]= 0x02;
    command[5]= 0x07;
    command[6]= 0x00;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}
uint32   Mart_EnableSpiWrite()
{
    uint32 idl = Mart_FPGA_IDLO();
    uint8 command[8];
    command[0]= (idl>>24)&0xFF;
    command[1]= (idl>>16)&0xFF;
    command[2]= (idl>>8)&0xFF;
    command[3]= (idl)&0xFF;
    command[4]= 0xAA;
    command[5]= 0x55;
    command[6]= 0xFA;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}
uint32   Mart_DisableSpiWrite()
{
    uint8 command[8];
    command[0]= 0x00;
    command[1]= 0x00;
    command[2]= 0x00;
    command[3]= 0xAA;
    command[4]= 0x55;
    command[5]= 0xAA;
    command[6]= 0xFA;
    command[7]= 0xB4;
    return Mart_Read4BYTE(command);
}

//------------------------------------------------------------------
uint32 ID;
//3 in 1 
void OpenNorWrite()
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9C40000 = 0x1500;
	*(vuint16 *)0x9fc0000 = 0x1500;
}


void CloseNorWrite()
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9C40000 = 0xd200;
	*(vuint16 *)0x9fc0000 = 0x1500;
}

void SetRompage(u16 page)
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9880000 = page;
	*(vuint16 *)0x9fc0000 = 0x1500;
}
void SetRampage(u16 page)
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9c00000 = page;
	*(vu16 *)0x9fc0000 = 0x1500;
}
void SetSerialMode()
{
	
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9A40000 = 0xe200;
	*(vu16 *)0x9fc0000 = 0x1500;
	
}
#include "_console.h"
#include "_consolewritelog.h"
uint32   ReadNorFlashID()
{
		vuint16 id1,id2,id3,id4;
		ID=0;
		//check intel 512M 3in1 card
		*((vuint16 *)(FlashBase+0)) = 0xFF ;
		*((vuint16 *)(FlashBase+0x1000*2)) = 0xFF ;
		*((vuint16 *)(FlashBase+0)) = 0x90 ;
		*((vuint16 *)(FlashBase+0x1000*2)) = 0x90 ;
		id1 = *((vuint16 *)(FlashBase+0)) ;
		id2 = *((vuint16 *)(FlashBase+0x1000*2)) ;
		id3 = *((vuint16 *)(FlashBase+1*2)) ;
		id4 = *((vuint16 *)(FlashBase+0x1001*2)) ;
		if(id3==0x8810)
			id3=0x8816;
		if(id4==0x8810)
			id4=0x8816;
		_consolePrintf("id1=%x\,id2=%x,id3=%x,id4=%xn",id1,id2,id3,id4);
		if( (id1==0x89)&& (id2==0x89) &&(id3==0x8816) && (id4==0x8816))
		{
			ID = 0x89168916;
			return 0x89168916;
		}
		//检测256M卡
		*((vuint16 *)(FlashBase+0x555*2)) = 0xAA ;
		*((vuint16 *)(FlashBase+0x2AA*2)) = 0x55 ;
		*((vuint16 *)(FlashBase+0x555*2)) = 0x90 ;

		*((vuint16 *)(FlashBase+0x1555*2)) = 0xAA ;
		*((vuint16 *)(FlashBase+0x12AA*2)) = 0x55 ;
		*((vuint16 *)(FlashBase+0x1555*2)) = 0x90 ;

		id1 = *((vuint16 *)(FlashBase+0x2)) ;
		id2 = *((vuint16 *)(FlashBase+0x2002)) ;
		if( (id1!=0x227E)|| (id2!=0x227E))
			return 0;
		
		id1 = *((vuint16 *)(FlashBase+0xE*2)) ;
		id2 = *((vuint16 *)(FlashBase+0x100e*2)) ;
		if(id1==0x2218 && id2==0x2218)			//H6H6
		{
			ID = 0x227E2218;
			return 0x227E2218;
		}
			
		if(id1==0x2202 && id2==0x2202)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
		if(id1==0x2202 && id2==0x2220)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
		if(id1==0x2202 && id2==0x2215)			//VZ064
		{
			ID = 0x227E2202;
			return 0x227E2202;
		}
		
			
		return 0;
			
}
void chip_reset()
{
		if(ID==0x89168916)
		{
			*((vuint16 *)(FlashBase+0)) = 0x50 ;
			*((vuint16 *)(FlashBase+0x1000*2)) = 0x50 ;
			*((vuint16 *)(FlashBase+0)) = 0xFF ;
			*((vuint16 *)(FlashBase+0x1000*2)) = 0xFF ;	
			return;
		}
		*((vu16 *)(FlashBase)) = 0xF0 ;
		*((vu16 *)(FlashBase+0x1000*2)) = 0xF0 ;
		
		if(ID==0x227E2202)
		{
			*((vu16 *)(FlashBase+0x1000000)) = 0xF0 ;
			*((vu16 *)(FlashBase+0x1000000+0x1000*2)) = 0xF0 ;
		}
}

void Block_EraseIntel(u32 blockAdd)
{
	u32 loop;
	vu16 v1,v2;
	bool b512=false;
	if(blockAdd>=0x2000000)
	{
		blockAdd-=0x2000000;
		CloseNorWrite();
		Enable_Arm9DS();
		SetRompage(768);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		b512=true;
	}
	if(blockAdd==0)
	{		
		for(loop=0;loop<0x40000;loop+=0x10000)
		{
			
			*((vu16 *)(FlashBase+loop)) = 0x50 ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x50 ;
			*((vu16 *)(FlashBase+loop)) = 0xFF ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xFF ;
			*((vu16 *)(FlashBase+loop)) = 0x60 ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x60;
			*((vu16 *)(FlashBase+loop)) = 0xD0 ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xD0;
			*((vu16 *)(FlashBase+loop)) = 0x20 ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0x20;
			*((vu16 *)(FlashBase+loop)) = 0xD0 ;
			*((vu16 *)(FlashBase+loop+0x2000)) = 0xD0;
			
			do
			{	
//				_consolePrintf("\nErasing here333\n");
				v1 = *((vu16 *)(FlashBase+loop));
				v2 = *((vu16 *)(FlashBase+loop+0x2000));
//				_consolePrintf("\n111333\n");
			}
			while((v1!=0x80)||(v2!=0x80));
		}
	}
	else
	{
		*((vu16 *)(FlashBase+blockAdd)) = 0xFF ;
		*((vu16 *)(FlashBase+blockAdd+0x2000)) = 0xFF ;
		*((vu16 *)(FlashBase+blockAdd)) = 0x60 ;
		*((vu16 *)(FlashBase+blockAdd+0x2000)) = 0x60 ;
		*((vu16 *)(FlashBase+blockAdd)) = 0xD0 ;
		*((vu16 *)(FlashBase+blockAdd+0x2000)) = 0xD0 ;
		*((vu16 *)(FlashBase+blockAdd)) = 0x20 ;
		*((vu16 *)(FlashBase+blockAdd+0x2000)) = 0x20 ;
		*((vu16 *)(FlashBase+blockAdd)) = 0xD0 ;
		*((vu16 *)(FlashBase+blockAdd+0x2000)) = 0xD0 ;
		do
		{
			v1 = *((vu16 *)(FlashBase+blockAdd));
			v2 = *((vu16 *)(FlashBase+blockAdd+0x2000));
		
		}
		while((v1!=0x80)||(v2!=0x80));
	}
	if(b512)
	{
		CloseNorWrite();
		Enable_Arm9DS();
		OpenNorWrite();
		SetRompage(0);
		SetSerialMode();
		chip_reset();
		b512=true;
	}
}
void Block_Erase(u32 blockAdd)
{
		vu16 page,v1,v2;  
		u32 Address;
		u32 loop;
		u32 off=0;
		if(ID==0x89168916)
		{
			//intel 512 card
			Block_EraseIntel(blockAdd);
			return ;
		}
		if( (blockAdd>=0x1000000) &&  (ID==0x227E2202))
		{
			off=0x1000000;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xF0 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xF0 ;
		}
		else
			off=0;
		Address=blockAdd;
		*((vu16 *)(FlashBase+0x555*2)) = 0xF0 ;
		*((vu16 *)(FlashBase+0x1555*2)) = 0xF0 ;
		
	
		if( (blockAdd==0) || (blockAdd==0x1FC0000) || (blockAdd==0xFC0000) || (blockAdd==0x1000000))
		{
			for(loop=0;loop<0x40000;loop+=0x8000)
			{
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+Address+loop)) = 0x30 ;
				
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+Address+loop+0x2000)) = 0x30 ;
				
				*((vu16 *)(FlashBase+off+0x2555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x22AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x2555*2)) = 0x80 ;
				*((vu16 *)(FlashBase+off+0x2555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x22AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+Address+loop+0x4000)) = 0x30 ;
				
				*((vu16 *)(FlashBase+off+0x3555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x32AA*2)) = 0x55 ; 
				*((vu16 *)(FlashBase+off+0x3555*2)) = 0x80 ;
				*((vu16 *)(FlashBase+off+0x3555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x32AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+Address+loop+0x6000)) = 0x30 ;
				do
				{  
					
					v1 = *((vu16 *)(FlashBase+Address+loop)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x2000)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x4000)) ;
				}while(v1!=v2);
				do
				{
					
					v1 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
					v2 = *((vu16 *)(FlashBase+Address+loop+0x6000)) ;
				}while(v1!=v2);
			}	
		}	
		else
		{
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55;
			*((vu16 *)(FlashBase+Address)) = 0x30 ;
			
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+0x2000)) = 0x30 ;
			
			do
			{
				v1 = *((vu16 *)(FlashBase+Address)) ;
				v2 = *((vu16 *)(FlashBase+Address)) ;
			}while(v1!=v2);
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x2000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x2000)) ;
			}while(v1!=v2);
			
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55;
			*((vu16 *)(FlashBase+Address+0x20000)) = 0x30 ;
			
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0x80 ;
			*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
			*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase+Address+0x2000+0x20000)) = 0x30 ;
		
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x20000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x20000)) ;
			}while(v1!=v2);
			do
			{
				v1 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
				v2 = *((vu16 *)(FlashBase+Address+0x2000+0x20000)) ;
			}while(v1!=v2);	
		}
}
void 		ReadNorFlash(u8* pBuf,u32 address,u16 len)
{
	vu16 *p = (vu16 *)pBuf;
	u32 loop;
	bool b512=false;
	if(address>=0x2000000)//256M
	{
		CloseNorWrite();		
		Enable_Arm9DS();
		SetRompage(768);
		OpenNorWrite();
		SetSerialMode();
		chip_reset();
		
		address-=0x2000000;
		b512=true;
	}
	Enable_Arm9DS();
	OpenNorWrite();
	SetSerialMode();
	chip_reset();
	if(ID==0x89168916)
	{
			*((vuint16 *)(FlashBase+address)) = 0x50 ;
			*((vuint16 *)(FlashBase+address+0x1000*2)) = 0x50 ;
			*((vuint16 *)(FlashBase+address)) = 0xFF ;
			*((vuint16 *)(FlashBase+address+0x1000*2)) = 0xFF ;
	}
	for(loop=0;loop<len/2;loop++)
	{
		p[loop]=*((vu16 *)(FlashBase+address+loop*2) );
	}	
	CloseNorWrite();
	Enable_Arm9DS();
	OpenNorWrite();
	SetSerialMode();
	chip_reset();
	if(b512==true)
	{
		SetRompage(0);
	}
}

void WriteNorFlashINTEL(u32 address,u8 *buffer,u32 size)
{
	u32 mapaddress;
	u32 size2,lop,j;
	vu16* buf = (vu16*)buffer ;
	register u32 loopwrite,i ;
	vu16 v1,v2;
	
	bool b512=false;
	if(address>=0x2000000)
	{
		address-=0x2000000;
		CloseNorWrite();
		Enable_Arm9DS();
		OpenNorWrite();
		SetRompage(768);
		SetSerialMode();
		chip_reset();

		b512=true;
	}
	else
	{
		CloseNorWrite();
		Enable_Arm9DS();
		OpenNorWrite();
		SetRompage(0);
		SetSerialMode();
		chip_reset();
	}
	
	if(size>0x4000)
	{
		size2 = size >>1 ;
		lop = 2; 
	}
	else 
	{
		size2 = size  ;
		lop = 1;
	}
	mapaddress = address;

//	_consolePrintf("WriteNorFlashINTEL begin\n");
	for(j=0;j<lop;j++)
	{
		if(j!=0)
		{
			mapaddress += 0x4000;
			buf = (vu16*)(buffer+0x4000);
		}
		for(loopwrite=0;loopwrite<(size2);loopwrite+=64)
		{
//			_consolePrintf("WriteNorFlashINTEL begin 1\n");
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0x50;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x50;
				*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0xFF;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xFF;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0xE8;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xE8;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0x70;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x70;
			v1=v2=0;
			while((v1!= 0x80) || (v2 != 0x80) )
			{
				v1 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) ;
				v2 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) ;
			}
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0x0F;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0x0F;
			for(i=0;i<0x10;i++)
			{
				*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+i*2)) = buf[(loopwrite>>2)+i];
				*((vu16 *)(FlashBase+mapaddress+0x2000+(loopwrite>>1)+i*2)) = buf[0x1000+(loopwrite>>2)+i];
			}
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) = 0xD0;
			*((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) = 0xD0;
			v1=v2=0;
//			_consolePrintf("WriteNorFlashINTEL begin 2\n");
			while((v1!= 0x80) || (v2 != 0x80) )
			{
				v1 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1))) ;
				v2 = *((vu16 *)(FlashBase+mapaddress+(loopwrite>>1)+0x2000)) ;
				if( (v1==0x90) || (v2==0x90))
				{
					WriteSram(0xA000000,(u8 *)buf,0x8000);
//					_consolePrintf("Err \n");
					while(1);
					break;
				}
			}
//			_consolePrintf("WriteNorFlashINTEL begin 3\n");
		}
	}
	if(b512==true)
	{
		CloseNorWrite();
		Enable_Arm9DS();
		OpenNorWrite();
		SetRompage(0);
		SetSerialMode();
		chip_reset();	
	}
}
void WriteNorFlash(u32 address,u8 *buffer,u32 size)
{
		if(ID==0x89168916)
		{
			WriteNorFlashINTEL(address,buffer,size);
			return;
		}
		vu16 v1,v2;
		register u32 loopwrite ;
		vu16* buf = (vu16*)buffer ;
		u32 size2,lop;
		u32 mapaddress;
		u32 j;
		v1=0;v2=1;
		u32 off=0;
		if( (address>=0x1000000) &&  (ID==0x227E2202))
		{
			off=0x1000000;
		}
		else
			off=0;
		if(size>0x4000)
		{
			size2 = size >>1 ;
			lop = 2; 
		}
		else 
		{
			size2 = size  ;
			lop = 1;
		}
		mapaddress = address;
		for(j=0;j<lop;j++)
		{
			if(j!=0)
			{
				mapaddress += 0x4000;
				buf = (vu16*)(buffer+0x4000);
			}
			for(loopwrite=0;loopwrite<(size2>>2);loopwrite++)
			{
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x2AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x555*2)) = 0xA0 ;
				*((vu16 *)(FlashBase+mapaddress+loopwrite*2)) = buf[loopwrite];
				
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xAA ;
				*((vu16 *)(FlashBase+off+0x12AA*2)) = 0x55 ;
				*((vu16 *)(FlashBase+off+0x1555*2)) = 0xA0 ;			
				*((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) = buf[0x1000+loopwrite];
				do
				{
					v1 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
					v2 = *((vu16 *)(FlashBase+mapaddress+loopwrite*2)) ;
				}while(v1!=v2);
				do
				{
					v1 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
					v2 = *((vu16 *)(FlashBase+mapaddress+0x2000+loopwrite*2)) ;
				}while(v1!=v2);
			}
		}	
}
void WriteSram(uint32 address, u8* data , uint32 size )
{	
	uint32 i ;
	for(i=0;i<size;i++)
		*(u8*)(address+i)=data[i];
}
void ReadSram(uint32 address, u8* data , uint32 size )
{
	uint32 i ;
	u16* pData = (u16*)data;
	for(i=0;i<size;i+=2)
	{
		pData[i>>1]=*(u8*)(address+i)+(*(u8*)(address+i+1)*0x100);
	}
}
#define NULL 0
void WritePSram(u8* address, u8* data , uint32 length )
{	
	vuint16 *pPatch=NULL;
	pPatch=(vuint16*)address;
	Enable_Arm9DS();
	CloseNorWrite();  
	SetRompage(192);
	OpenNorWrite();
	for(u32 pi=0;pi<length;pi+=2)
	{
		pPatch[pi>>1]= data[pi] + (data[pi+1]<<8);
	}
	CloseNorWrite();
	Enable_Arm7DS();
}
void ReadPSram(u8* address, u8* data , uint32 length )
{
	vuint16 *pPatch=NULL;
	pPatch=(vuint16*)address;
	Enable_Arm9DS();
	CloseNorWrite();  
	SetRompage(192);
	OpenNorWrite();
	for(u32 pi=0;pi<length;pi+=2)
	{
		data[pi]=pPatch[pi>>1] & 0xff; 
		data[pi+1]=(pPatch[pi>>1] &0xff00)>>8;
	}
	CloseNorWrite();
	Enable_Arm7DS();
}
void  OpenRamWrite()
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9C40000 = 0xA500;
	*(vu16 *)0x9fc0000 = 0x1500;
}

void CloseRamWrite()
{
	*(vu16 *)0x9fe0000 = 0xd200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xd200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)0x9C40000 = 0xA200;
	*(vu16 *)0x9fc0000 = 0x1500;
}
void SetShake(u16 data)
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9E20000 = data;
	*(vuint16 *)0x9fc0000 = 0x1500;
}
void resetgbasavemode()
{
	*(vuint16 *)0x9fe0000 = 0xd200;
	*(vuint16 *)0x8000000 = 0x1500;
	*(vuint16 *)0x8020000 = 0xd200;
	*(vuint16 *)0x8040000 = 0x1500;
	*(vuint16 *)0x9E00000 = 0x01;
	*(vuint16 *)0x9E00000 = 0x00;
	*(vuint16 *)0x9fc0000 = 0x1500;
}

