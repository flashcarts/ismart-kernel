1. Resources on the card:
	1)EEPROM, size 0x200000Byte, refer to SPI graphic for details
	2)SPAM, size0x800Byte, detail as below:
	//****************************************************
		0x0-0x600 reserved
		0x600 - cluster for RTS.bin  RTG fat table begins at 0x1000
		0x700 - 0x58050 - replace 0x5c000 - 4000000  registers backup //=0x4000240
	
		0x780 - use full 00 fill save file
		0x79C - for RTS, SDK_STATIC_BSS_START-0x2000; starting backup address 2000000
		0x7A8 - arm7 waiting code jump to reset address
		0x7AC - arm7 waiting code starting address
		
		0x7B0 - Special game sign position, enabled sign 0xF5A05259, General game without the sign
		0x7B4 - Special game setup position, saved position
		0x7B8 - swi B address
		0x7BC - swi B instruction	
		
		0x7C0 - Soft Reset (Y/N)?
		0x7C4 - Cheat finger?
		0x7C8 - RTS (Y/N)?
		0x7CC - ARM7 Patched Code location 0x230000C - 0x2300600 ，This data is reserved for soft reset median
		
		0x7D0 - Start up Code of the 2nd process existence (Y/N)?
		0x7D4 - Saved code Asynchronous (Y/N)?  1 means synchronous（modification needed） ， 0 means Asynchronous
		0x7D8 - Execute ReadRom synchronous (Y/N)?   1 means synchronous（modification needed） ， 0 means Asynchronous
		0x7DC - Address for the current patched information, no need to look for game to patch on next startup. It can execute directly. This data is for ARM9 patch
		0x7E0 - SDHC sign
		0x7E4 - how many sectors per cluster
		0x7E8 - cluster 1 for debug.bin 
		0x7EC - cluster 2 for debug.bin
		0x7F0 - Special game
		0x7F4 - 1st cluster of the game
		0x7FC - address for debug information
	//***************************************************
2.Methods for visiting information
	1)EEPROM reads 0x4 Bytes:uint32 Mart_ReadFlashCommand(uint32 address)
	2)EEPROM reads 0x200 bytes:uint32  Mart_ReadFlash(uint32 address, uint8* pbuf)
	3)EEPROM erase 0x1000 bytes:void cardEepromBlockErase(uint32 address)
	4)EEPROM erase 0x10000 bytes:void cardEepromSectorErase(uint32 address)
	5)EEPROM writes :void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype)
	6)SRAM reads 0x4 bytes:uint32  Mart_ReadSram4byte(uint32 address)
	7)SRAM writes0x4 bytes:uint32 dsCardi_WriteSram(uint32 address,uint16 data)
		for example:
		{    
		    temp = 0x12345678 ;
		    dsCardi_WriteSram(0x7dfdc , temp&0xFFFF);  
		    dsCardi_WriteSram(0x7dfde , (temp>>16)&0xFFFF); 
	    }
3.Special Notes
	1)ismart premium uses 0x1C0000-0x1E0000, total of 0x20000 Bytes for mapping of game FAT table
	//***************************************************
	ROM size	Byte/cluster  EEPROM 	
    512MBYTE  	- 4K   	-- use 0x80000 (cant use) 
              	- 8K   	-- use 0x40000 (cant use)
              	- 16K  	-- use 0x20000
              	- 32K  	-- use 0x10000
              
    256MBYTE  	- 4K   	-- use 0x40000 (cant use) 
              	- 8K   	-- use 0x20000
              	- 16K  	-- use 0x10000
              	- 32K  	-- use 0x8000
              
    128MBYTE  	- 4K   	-- use 0x20000 
              	- 8K   	-- use 0x10000
              	- 16K  	-- use 0x8000
              	- 32K  	-- use 0x4000
              
    64MBYTE   	- 4K   	-- use 0x10000 
              	- 8K   	-- use 0x8000
              	- 16K  	-- use 0x4000
              	- 32K  	-- use 0x2000
              
    32MBYTE   	- 4K   	-- use 0x8000 
              	- 8K   	-- use 0x4000
              	- 16K  	-- use 0x2000
              	- 32K  	-- use 0x1000 
              
    16MBYTE   	- 4K   	-- use 0x4000 
              	- 8K   	-- use 0x2000
              	- 16K  	-- use 0x1000
              	- 32K  	-- use 0x800
              
    8MBYTE    	- 4K   	-- use 0x2000 
              	- 8K   	-- use 0x1000
              	- 16K  	-- use 0x800
              	- 32K  	-- use 0x400  
	//***************************************************
	Therefore, you can not format your TF as 4K/cluster,, it has to more 8K/cluster or larger

	2)The red portion of the SPI graphics resource allocation can not be modified. Otherwise, iSmart Premium will not work.




#############################################################
1.卡上的资源
	1)EEPROM,大小0x200000Byte,具体分配看SPI图片
	2)SRAM,大小0x800Byte,具体分配如下:
	//***************************************************
		0x0-0x600 保留
		0x600 - RTS.bin文件的簇 0x1000开始是即时攻略的fat表
		0x700 - 0x58050 - 代替 0x5c000 - 4000000 寄存器备份 /=0x4000240
		
		0x780 - 存档文件使用全00填充
		0x79C - 即时存档里用,SDK_STATIC_BSS_START-0x2000;起始备份2000000的地址
		0x7A8 - arm7等待代码里跳到reset的地址
		0x7AC - arm7等待代码开始地址
		
		0x7B0 - 金软特殊游戏设置标志位，启用标志 0xF5A05259 ，非此标志表示通用
		0x7B4 - 金软的地址设置，即存放位置
		0x7B8 - swi B 地址
		0x7BC - swi B 指令
		
		0x7C0 - 是否软复位
		0x7C4 - 是否金手指
		0x7C8 - 是否即时存档
		0x7CC - ARM7代码补丁所在区域的位置 0x230000C - 0x2300600 ，此数据保留为软复位时使用做中间值
		
		0x7D0 - 第二过程的启动代码是否存在
		0x7D4 - 存档代码的同步与否， 1 表示使用同步（需要修改） ， 0 表示异步
		0x7D8 - 是否执行ReadRom的同步与否 1.表示使用同步（需要修改） ， 0 表示异步
		0x7DC - 当前的打补丁信息的地址,即下次启动可以不再寻找游戏进行补丁，可以直接执行 , 此数据是ARM9补丁
		0x7E0 - SDHC 标志
		0x7E4 - 每簇多少个sector
		0x7E8 - debug.bin文件的簇1
		0x7EC - debug.bin文件的簇2
		0x7F0 - 特殊游戏
		0x7F4 - 游戏的第一个簇
		0x7FC - debug信息地址
	//***************************************************
2.资源访问方法
	1)EEPROM读0x4字节:uint32 Mart_ReadFlashCommand(uint32 address)
	2)EEPROM读0x200字节:uint32  Mart_ReadFlash(uint32 address, uint8* pbuf)
	3)EEPROM擦除0x1000字节:void cardEepromBlockErase(uint32 address)
	4)EEPROM擦除0x10000字节:void cardEepromSectorErase(uint32 address)
	5)EEPROM写:void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype)
	6)SRAM读0x4字节:uint32  Mart_ReadSram4byte(uint32 address)
	7)SRAM写0x4字节:uint32 dsCardi_WriteSram(uint32 address,uint16 data)
		for example:
		{    
		    temp = 0x12345678 ;
		    dsCardi_WriteSram(0x7dfdc , temp&0xFFFF);  
		    dsCardi_WriteSram(0x7dfde , (temp>>16)&0xFFFF); 
	    }
3.注意事项
	1)这个卡上使用0x1C0000-0x1E0000,共0x20000Byte大小做为游戏FAT表的映射.
	//***************************************************
	ROM size	Byte/cluster  EEPROM 	
    512MBYTE  	- 4K   	-- use 0x80000 (cant use) 
              	- 8K   	-- use 0x40000 (cant use)
              	- 16K  	-- use 0x20000
              	- 32K  	-- use 0x10000
              
    256MBYTE  	- 4K   	-- use 0x40000 (cant use) 
              	- 8K   	-- use 0x20000
              	- 16K  	-- use 0x10000
              	- 32K  	-- use 0x8000
              
    128MBYTE  	- 4K   	-- use 0x20000 
              	- 8K   	-- use 0x10000
              	- 16K  	-- use 0x8000
              	- 32K  	-- use 0x4000
              
    64MBYTE   	- 4K   	-- use 0x10000 
              	- 8K   	-- use 0x8000
              	- 16K  	-- use 0x4000
              	- 32K  	-- use 0x2000
              
    32MBYTE   	- 4K   	-- use 0x8000 
              	- 8K   	-- use 0x4000
              	- 16K  	-- use 0x2000
              	- 32K  	-- use 0x1000 
              
    16MBYTE   	- 4K   	-- use 0x4000 
              	- 8K   	-- use 0x2000
              	- 16K  	-- use 0x1000
              	- 32K  	-- use 0x800
              
    8MBYTE    	- 4K   	-- use 0x2000 
              	- 8K   	-- use 0x1000
              	- 16K  	-- use 0x800
              	- 32K  	-- use 0x400  
	//***************************************************
	因此,你的TF卡不能格式化为4k字节每簇.必须大于等于8k字节每簇.
	
	2)SPI图片上的资源分配,红色部分不能修改,如果修改,你的卡将不能工作.

#############################################################

	
