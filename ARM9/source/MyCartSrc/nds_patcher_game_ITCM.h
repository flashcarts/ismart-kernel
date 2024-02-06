//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
CODE_IN_ITCM uint32 fix_data_Pokemon[]={
		 0xE59F003C,0xE5901000,0xE59F2038,0xE1510002,
		 0x059F1034,0x05801008,0x05801148,0x059F102C,
		 0x058010A8,0x058011E8,0xE59F0024,0xE5901000,
		 0xE59F2020,0xE1510002,0x03A01001,0x05C0100A,
		 0xE12FFF1E,0x00000000,0xE15F034F,0x36AAB337,
		 0x36AAE06A,0x00000000,0x28AAFF1F,
};
CODE_IN_ITCM void patch_global_Pokemon(uint32 b_address,uint32 b_code,uint32 to_address,uint32 replace1,uint32 replace2)
{
	if(*(vuint32*)b_address == 0XE12FFF1E)
	{
		*(vuint32*)b_address = b_code;	
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)b_address,(uint32)b_code);
		fix_data_Pokemon[17] = replace1;
		fix_data_Pokemon[21] = replace2;
		for(int tt=0;tt<23;tt++)
		{
			*((vuint32*)to_address + tt) = fix_data_Pokemon[tt];
		}
		//--ww-- 登记写入
		WritePatchInfo((uint32)to_address,23*4,fix_data_Pokemon);
	}	
}
CODE_IN_ITCM void patch_4638(void)//魔法门
{
	uint32* patch_4638_data = (uint32*)0x01FF7000;
	for(int i=0;i<359;i++)
	{
		*(uint32*)patch_4638_data[i] = 0xE1500000;
	}
	for(int i=359;i<467;i++)
	{
		*(uint32*)patch_4638_data[i] = 0xE150000F;
	}	
}
//////////////////////////////////////////
CODE_IN_ITCM void patch_4501(void)//巨龙战争USA
{
	uint8* baseAddress = (uint8*)0x2000000;
	*(baseAddress+0x1d1d) = 0xe0;
	*(baseAddress+0x8915) = 0xe0;
	*(baseAddress+0xe0b7) = 0xe0;
	*(uint16*)(baseAddress+0xe1c0) = 0x0;
	*(uint16*)(baseAddress+0xedd2) = 0x0;
	*(uint16*)(baseAddress+0xeea2) = 0x0;
	*(baseAddress+0xefd9) = 0xe0;
	*(baseAddress+0xf72d) = 0xe0;
	*(baseAddress+0xf8bb) = 0xe0;
	*(baseAddress+0xf99d) = 0xe0;
	*(uint16*)(baseAddress+0xfa82) = 0x0;
	*(baseAddress+0xfb5f) = 0xe0;
	*(uint16*)(baseAddress+0xfc66) = 0x0;
	*(baseAddress+0xfda9) = 0xe0;
	*(baseAddress+0x10091) = 0xe0;
	*(baseAddress+0x1045f) = 0xe0;
	*(uint16*)(baseAddress+0x106d8) = 0x0;
	*(baseAddress+0x13eb7) = 0xe0;
	*(baseAddress+0x14465) = 0xe0;
	*(uint16*)(baseAddress+0x1563c) = 0x0;
	*(uint16*)(baseAddress+0x16d8a) = 0x0;
	*(baseAddress+0x16e9d) = 0xe0;
	*(uint16*)(baseAddress+0x1710a) = 0x0;
	*(baseAddress+0x172e9) = 0xe0;
	*(baseAddress+0x1755b) = 0xe0;
	*(baseAddress+0x1a88f) = 0xe0;
	*(uint16*)(baseAddress+0x1b5ba) = 0x0;
	*(uint16*)(baseAddress+0x1c20c) = 0x0;
	*(uint16*)(baseAddress+0x1cc52) = 0x0;
	*(baseAddress+0x1fbe7) = 0xe0;
	*(baseAddress+0x1fe37) = 0xe0;
	*(uint16*)(baseAddress+0x1fff6) = 0x0;
	*(baseAddress+0x20231) = 0xe0;
	*(uint16*)(baseAddress+0x20300) = 0x0;
	*(baseAddress+0x20939) = 0xe0;
	*(baseAddress+0x20e5f) = 0xe0;
	*(baseAddress+0x22da9) = 0xe0;
	*(baseAddress+0x24047) = 0xe0;
	*(baseAddress+0x24de1) = 0xe0;
	*(uint16*)(baseAddress+0x264e8) = 0x0;
	*(uint16*)(baseAddress+0x274da) = 0x0;
	*(uint16*)(baseAddress+0x276e8) = 0x0;
	*(uint16*)(baseAddress+0x2863c) = 0x0;
	*(baseAddress+0x2c959) = 0xe0;
	*(uint16*)(baseAddress+0x2cc70) = 0x0;
	*(baseAddress+0x2ce03) = 0xe0;
	*(baseAddress+0x2cf49) = 0xe0;
	*(uint16*)(baseAddress+0x2d00e) = 0x0;
	*(baseAddress+0x2fde7) = 0xe0;
	*(uint16*)(baseAddress+0x2ff4a) = 0x0;
	*(baseAddress+0x300a5) = 0xe0;
	*(baseAddress+0x344b9) = 0xe0;
	*(baseAddress+0x355a7) = 0xe0;
	*(uint16*)(baseAddress+0x35660) = 0x0;
	*(baseAddress+0x35773) = 0xe0;
	*(uint16*)(baseAddress+0x358d4) = 0x0;
	*(uint16*)(baseAddress+0x36390) = 0x0;
	*(uint16*)(baseAddress+0x3647e) = 0x0;
	*(uint16*)(baseAddress+0x36732) = 0x0;
	*(uint16*)(baseAddress+0x36e26) = 0x0;
	*(baseAddress+0x36f03) = 0xe0;
	*(baseAddress+0x37413) = 0xe0;
	*(baseAddress+0x3756b) = 0xe0;
	*(baseAddress+0x37a75) = 0xe0;
	*(baseAddress+0x37ec9) = 0xe0;
	*(uint16*)(baseAddress+0x37fda) = 0x0;
	*(baseAddress+0x394f1) = 0xe0;
	*(baseAddress+0x39623) = 0xe0;
	*(uint16*)(baseAddress+0x3fba6) = 0x0;
	*(uint16*)(baseAddress+0x410ca) = 0x0;
	*(uint16*)(baseAddress+0x4118e) = 0x0;
	*(baseAddress+0x41593) = 0xe0;
	*(uint16*)(baseAddress+0x42e72) = 0x0;
	*(baseAddress+0x42fbf) = 0xe0;
	*(baseAddress+0x44219) = 0xe0;
	*(baseAddress+0x44601) = 0xe0;
	*(baseAddress+0x47333) = 0xe0;
	*(uint16*)(baseAddress+0x4740a) = 0x0;
	*(baseAddress+0x477d3) = 0xe0;
	*(uint16*)(baseAddress+0x479ea) = 0x0;
	*(baseAddress+0x48207) = 0xe0;
	*(baseAddress+0x494e7) = 0xe0;
	*(uint16*)(baseAddress+0x4973e) = 0x0;
	*(uint16*)(baseAddress+0x4a4d6) = 0x0;
	*(uint16*)(baseAddress+0x4a706) = 0x0;
	*(baseAddress+0x4baa9) = 0xe0;
	*(baseAddress+0x4eb97) = 0xe0;
	*(baseAddress+0x52139) = 0xe0;
	*(uint16*)(baseAddress+0x53146) = 0x0;
	*(uint16*)(baseAddress+0x5326e) = 0x0;
	*(uint16*)(baseAddress+0x54c34) = 0x0;
	*(baseAddress+0x54e1d) = 0xe0;
	*(baseAddress+0x56213) = 0xe0;
	*(baseAddress+0x56cdf) = 0xe0;
	*(uint16*)(baseAddress+0x7d646) = 0x0;
	*(uint16*)(baseAddress+0x89dce) = 0x0;
	*(uint16*)(baseAddress+0x8a0a2) = 0x0;
	*(baseAddress+0x8bcc3) = 0xe0;
	*(uint16*)(baseAddress+0x8e37a) = 0x0;
	*(uint16*)(baseAddress+0x8e8ce) = 0x0;
	*(baseAddress+0x8fa19) = 0xe0;
	*(uint16*)(baseAddress+0x92bb0) = 0x0;
	*(baseAddress+0x9680b) = 0xe0;
	*(uint16*)(baseAddress+0x98444) = 0x0;
	*(baseAddress+0x9d09b) = 0xe0;
	*(baseAddress+0xa56e3) = 0xe0;
	*(baseAddress+0xa9039) = 0xe0;
	*(uint16*)(baseAddress+0xa9122) = 0x0;
	*(baseAddress+0xb247f) = 0xe0;
	*(uint16*)(baseAddress+0xb6dea) = 0x0;
	*(uint16*)(baseAddress+0xb7ad6) = 0x0;
	*(uint16*)(baseAddress+0xbc692) = 0x0;
	*(baseAddress+0xbcacf) = 0xe0;
	*(baseAddress+0xbd571) = 0xe0;
}
/////////////////////////////////////////////////////////
CODE_IN_ITCM void fixAddressValue(uint32 *address1,uint32 *address2,uint32 type)
{
	uint32 mov_r0_0 = 0xE3A00000; //mov r0,#0
	uint32 bx_lr = 0xE12FFF1E;     //bx lr	
	switch(type)
	{
	case 1:
		*address1 = mov_r0_0;
		*(address1+1) = bx_lr;
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)address1,mov_r0_0);
		WritePatchInfo_4BYTE((uint32)(address1+1),bx_lr);
		break;
	case 2:
		*address1 = mov_r0_0;
		*(address1+1) = bx_lr;
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)address1,mov_r0_0);
		WritePatchInfo_4BYTE((uint32)(address1+1),bx_lr);
    
		*address2 = mov_r0_0;
		*(address2+1) = bx_lr;
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)address2,mov_r0_0);
		WritePatchInfo_4BYTE((uint32)(address2+1),bx_lr);
		break;
	case 3:
		*address1 = (uint32)address2;
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)address1,(uint32)address2);
		break;	      

	}	
}
//------------------------------------------
CODE_IN_ITCM uint32 fix_data_3690[10]={
		0xE92D4008,0XE1A03000,0XE1A00001,0XE1A01003,
		0XE28FE008,0XE59F3000,0XE12FFF13,0X02000100,
		0XE3A00001,0XE8BD8008};
CODE_IN_ITCM inline void fix_3690()
{
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x0200353C + tt) = fix_data_3690[tt];//WRITEFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x0200353C,10*4,fix_data_3690);
	fix_data_3690[7] = 0X02000104;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x02003908 + tt) = fix_data_3690[tt];//READFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x02003908,10*4,fix_data_3690);
}
//---------------------
CODE_IN_ITCM inline void fix_4812()//老子制造
{
	fix_data_3690[7] = 0X02000150;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x0200355C + tt) = fix_data_3690[tt];//WRITEFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x0200355C,10*4,fix_data_3690);
	fix_data_3690[7] = 0X02000154;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x02003928 + tt) = fix_data_3690[tt];//READFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x02003928,10*4,fix_data_3690);
}
//---------------------
CODE_IN_ITCM inline void fix_4915()//老子制造
{
	fix_data_3690[7] = 0X02000230;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x020035FC + tt) = fix_data_3690[tt];//WRITEFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x020035FC,10*4,fix_data_3690);
	fix_data_3690[7] = 0X02000234;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x020039C8 + tt) = fix_data_3690[tt];//READFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x020039C8,10*4,fix_data_3690);
}
//---------------------
CODE_IN_ITCM inline void fix_4951()
{
	fix_data_3690[7] = 0X02000300;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x0206176c + tt) = fix_data_3690[tt];//WRITEFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x0206176c,10*4,fix_data_3690);
	fix_data_3690[7] = 0X02000304;
	for(int tt=0;tt<10;tt++)
	{
		*((uint32*)0x02061ac4 + tt) = fix_data_3690[tt];//READFLASH
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x02061ac4,10*4,fix_data_3690);
}
//---------------------------------------------------
CODE_IN_ITCM inline void fix_4377()//罪恶覆灭计划:刑警新兵
{
	if(*(vuint32*)0x2000800 == 0xEA0906BE) return;
    uint32 *puStart = (uint32*)(*(uint32*)0x27FFE24) ;	
    uint32 pA9_size = *(uint32*)0x27FFE2C ; 
    uint32 *puEnd = puStart + (pA9_size>>2); 
    uint32 count=0;
    do
    {
    	if((*puStart == 0x0A000009) || (*puStart == 0x0A000007)|| (*puStart == 0x0A000008) || (*puStart == 0x0A00000A))
    	{
        	if(((*(puStart+1) & 0xFFF00000) == 0xE5900000) || ((*(puStart+1) & 0xFFF00000) == 0xE0800000) )
        	{
        		count++;
        		uint32 k= *puStart;
        		*puStart =   k | 0xE0000000;
        		//WriteDubugInfo((uint32)puStart);     		
        	}
    	}
    	
    	puStart++;	
    }while(puStart<(uint32*)0x020a0000);//puEnd);
    
    uint32 kk = 0x0FFFFFFF;
    uint32 kk2 = 0xE1500000;
    *(vuint32*)0x020264D4 = (*(vuint32*)0x020264D4) & kk;
    *(vuint32*)0x0202CA84 = (*(vuint32*)0x0202CA84) & kk;    
    *(vuint32*)0x0202D6C8 = (*(vuint32*)0x0202D6C8) & kk; 
    *(vuint32*)0x02037A24 = (*(vuint32*)0x02037A24) & kk; 
    *(vuint32*)0x02039F0C = (*(vuint32*)0x02039F0C) & kk;     
    *(vuint32*)0x0203DCB0 = (*(vuint32*)0x0203DCB0) & kk; 
    *(vuint32*)0x0203E720 = (*(vuint32*)0x0203E720) & kk;     
    *(vuint32*)0x0203F6D0 = (*(vuint32*)0x0203F6D0) & kk;      
    *(vuint32*)0x0203FFFC = (*(vuint32*)0x0203FFFC) & kk;    
    *(vuint32*)0x02040B38 = (*(vuint32*)0x02040B38) & kk;
    *(vuint32*)0x020515A4 = (*(vuint32*)0x020515A4) & kk; 
    *(vuint32*)0x020532DC = (*(vuint32*)0x020532DC) & kk; 
    *(vuint32*)0x0205567C = (*(vuint32*)0x0205567C) & kk;  
    *(vuint32*)0x02058060 = (*(vuint32*)0x02058060) & kk; 
    *(vuint32*)0x02058BAC = (*(vuint32*)0x02058BAC) & kk; 
    *(vuint32*)0x02058D7C = (*(vuint32*)0x02058D7C) & kk; 
    *(vuint32*)0x0205CE58 = (*(vuint32*)0x0205CE58) & kk; 
    *(vuint32*)0x02026408 = kk2;
    *(vuint32*)0x02026480 = kk2;
    *(vuint32*)0x0202C7D8 = kk2;
    *(vuint32*)0x0202CD60 = kk2;
    *(vuint32*)0x0202D3CC = kk2;
    *(vuint32*)0x0202D920 = kk2;
    *(vuint32*)0x02030CF0 = kk2;
    *(vuint32*)0x0203288C = kk2;
#ifdef yafeidebug	
    WriteDubugInfo((uint32)'1111');
    WriteDubugInfo((uint32)count);
#endif    
}
//---------------------------------------------------

CODE_IN_ITCM void patch_4511(void)//Might & Magic
{
	uint32* patch_4511_data = (uint32*)0x01FF7000;
	for(int i=0;i<359;i++)
	{
		*(uint32*)patch_4511_data[i] = 0xE1500000;
	}
	for(int i=359;i<467;i++)
	{
		*(uint32*)patch_4511_data[i] = 0xE150000F;
	}	
}
//------------------------------------------
CODE_IN_ITCM void patch_4800(void)
{
	*(vuint32*)0x027FFE24 = 0x2000800; 
	uint32* patch_4800_data = (uint32*)0x01FF7000;
	for(int i=0;i<180;i++)
	{
		*(vuint16*)patch_4800_data[i] = 0x4280;
	}
	*(vuint16*)0x0207d8ea = 0x2500;
	*(vuint16*)0x0204ecba = 0x46c0;
	
}
//----------------------------------------------
CODE_IN_ITCM void patch_5292(void)
{
	uint32* patch_5292_data = (uint32*)0x01FF7000;
	//*(vuint32*)0x020009f8 = 0xEAFFFEB0;
	*(vuint32*)0x0204B868 = 0xEBFED31C;
	//--ww-- 登记写入
	//WritePatchInfo_4BYTE((uint32)0x020009f8,(uint32)0xEAFFFEB0);
	for(int tt=0;tt<104;tt++)
	{
		*((vuint32*)0x020004C0 + tt) = patch_5292_data[tt];
	}
	//WritePatchInfo((uint32)0x020004C0,104*4,patch_5292_data);
}
//----------------------------------------------
CODE_IN_ITCM void patch_5387(void)
{
	uint32* patch_5292_data = (uint32*)0x01FF7000;
	//*(vuint32*)0x020009f8 = 0xEAFFFDC0; 

	*(vuint32*)0X0204BB24 = 0xEBFED17D;
	//--ww-- 登记写入
	WritePatchInfo_4BYTE((uint32)0X0204BB24,(uint32)0xEBFED17D);
	patch_5292_data[80] = 0x02000331;//0X02059909;
	//patch_5292_data[4] = 0X0204BB24;
	//patch_5292_data[5] = 0xEBFED17D;
	for(int tt=0;tt<101;tt++)
	{
		*((vuint32*)0x02000100 + tt) = patch_5292_data[tt];
	}
	WritePatchInfo((uint32)0x02000100,101*4,patch_5292_data);
}
//----------------------------------------------
CODE_IN_ITCM void patch_5367(void)
{
	uint32* patch_5292_data = (uint32*)0x01FF7000;
	//*(vuint32*)0x020009f8 = 0xEAFFFDC0; 
	*(vuint32*)0X0204BA64 = 0xEBFED2AD;
	//--ww-- 登记写入
	WritePatchInfo_4BYTE((uint32)0X0204BA64,(uint32)0xEBFED2AD);	
	patch_5292_data[80] = 0x02059849;
	for(int tt=0;tt<101;tt++)
	{
		*((vuint32*)0x02000500 + tt) = patch_5292_data[tt];
	}
	WritePatchInfo((uint32)0x02000500,101*4,patch_5292_data);
}
//----------------------------------------------
CODE_IN_ITCM uint32 fix_data_4375[13]={
		0XE59D1000,0XE3510001,0X1A000003,0XE28F1010,
		0XE891001E,0XE5813000,0XE5824000,0XE28DD02C,
		0XE12FFF1E,0X0218DA4C,0X0218DA54,0XE3A05921,
		0XE3A04000
};
CODE_IN_ITCM void patch_4375()//疯狂兔子回家
{
	*(vuint32*)0x0203d108 = 0xEBFF0FBC; 
	//--ww-- 登记写入
	//WritePatchInfo_4BYTE((uint32)0x0203d108,(uint32)0xEBFF0FBC);
	for(int tt=0;tt<13;tt++)
	{
		*((vuint32*)0x02001000 + tt) = fix_data_4375[tt];
	}
	//--ww-- 登记写入
	//WritePatchInfo((uint32)0x02001000,13*4,fix_data_4375);	
}
//----------------------------------------------
CODE_IN_ITCM uint32 fix_data_5400[19]={
		0xE59F0000,0xE12FFF1E,0x0000B3CF,0xE59F0000,
		0xE12FFF1E,0x0000B177,0xE92D4007,0xE59F001C,
		0xE59F101C,0xE5912000,0xE1500002,0x059F0014,
		0x05810000,0x0280000C,0x0581003C,0xE8BD8007,
		0x0215D7E0,0x0215C04C,0x02002300
};
CODE_IN_ITCM void patch_5400()
{
	*(vuint32*)0x020009f8 = 0xEAFFFE06; 
	//--ww-- 登记写入
	//WritePatchInfo_4BYTE((uint32)0x0203d108,(uint32)0xEBFF0FBC);
	for(int tt=0;tt<19;tt++)
	{
		*((vuint32*)0x02000200 + tt) = fix_data_5400[tt];
	}
	//--ww-- 登记写入
	//WritePatchInfo((uint32)0x02001000,13*4,fix_data_4375);	
}
CODE_IN_ITCM void patch_global_pp(uint32 b_address,uint32 b_code,uint32 to_address,uint32 replace1,uint32 replace2,uint32 replace3)
{
	if(*(vuint32*)b_address == 0XE12FFF1E)
	{
		*(vuint32*)b_address = b_code;	
		//--ww-- 登记写入
		WritePatchInfo_4BYTE((uint32)b_address,(uint32)b_code);
		fix_data_5400[16] = replace1;
		fix_data_5400[17] = replace2;
		fix_data_5400[18] = replace3;
		for(int tt=0;tt<19;tt++)
		{
			*((vuint32*)to_address + tt) = fix_data_5400[tt];
		}
		//--ww-- 登记写入
		WritePatchInfo((uint32)to_address,19*4,fix_data_5400);
	}	
}
//-----------------------------------------------
CODE_IN_ITCM void patch_1981()
{
	uint32* patch_1981_data = (uint32*)0x01FF7000;
	//uint32* patch_4273_data2 = (uint32*)0x01FF7200;
	*(vuint32*)0x0203c448 = 0xe51ff004;
	*(vuint32*)0x0203c44C = 0x02000010;
	//--ww-- 登记写入
	WritePatchInfo_4BYTE((uint32)0x0203c448,(uint32)0xe51ff004);
	WritePatchInfo_4BYTE((uint32)0x0203c44C,(uint32)0x02000010);

	for(int tt=0;tt<20;tt++)
	{
		*((vuint32*)0x02000010 + tt) = patch_1981_data[tt];
	}
	//--ww-- 登记写入
	WritePatchInfo((uint32)0x02000010,20*4,patch_1981_data);

}

//--------------------------------------------------------------------
CODE_IN_ITCM void patchSpecialGame(uint32 *r0_start,uint32 *r1_end)
{
	{//代替 swi B命令
		uint32 swi3Address = A9_ReadSram(0x7DFB8); 
		if(swi3Address)
		{
			*(uint32*)swi3Address = A9_ReadSram(0x7DFBC); 
			WritePatchInfo_4BYTE((uint32)swi3Address,A9_ReadSram(0x7DFBC));
		}
		
	}
    uint32 number = A9_ReadSram(0x7DFF0) & 0xFF;
    if(number==0) return ;  

	switch(number)
	{

	case 0x05://3690 老子制造
		{
			copyCode((void*)((uint32)SAVE_32M_3690_start),(void*)0x2000100,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start);
			//--ww-- 登记写入
			WritePatchInfo((uint32)0x2000100,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start,(uint32*)SAVE_32M_3690_start);
			fixAddressValue((uint32*)0x02003880,(uint32*)0xE12FFF1E,3);//NandWait
			fixAddressValue((uint32*)0x02003A94,(uint32*)0xE3A00003,3);//NandState		
			fixAddressValue((uint32*)0x02003A98,(uint32*)0xE12FFF1E,3);//NandState
			fixAddressValue((uint32*)0x02003AAC,(uint32*)0,1);//NandError
			fixAddressValue((uint32*)0x020030F0,(uint32*)0xE3A00001,3);//NandInit 		
			fixAddressValue((uint32*)0x020030F4,(uint32*)0xE12FFF1E,3);//NandInit 
			fix_3690();
		}
		break;
    case 0x04://1981
		patch_1981();
		break;
	case 0x06:
		fix_4377();//罪恶覆灭计划:刑警新兵
		break;
	case 0x07:
		patch_4375();//疯狂兔子回家
		break;
	case 0x08://4501巨龙战争USA
		patch_4501();
		break;
	case 0x09://4511 Might & Magic
		patch_4511();
		break;
	case 0x0A://4638魔法门之英雄交锋 (E)
		patch_4638();
		break;
	case 0x11://4800
		patch_4800();
		break;
	case 0x12://4812 老子制造
		{
			copyCode((void*)((uint32)SAVE_32M_3690_start),(void*)0x2000150,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start);
			//--ww-- 登记写入
			WritePatchInfo((uint32)0x2000150,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start,(uint32*)SAVE_32M_3690_start);
			fixAddressValue((uint32*)0x020038A0,(uint32*)0xE12FFF1E,3);//NandWait
			fixAddressValue((uint32*)0x02003AB4,(uint32*)0xE3A00003,3);//NandState		
			fixAddressValue((uint32*)0x02003AB8,(uint32*)0xE12FFF1E,3);//NandState
			fixAddressValue((uint32*)0x02003ACC,(uint32*)0,1);//NandError
			fixAddressValue((uint32*)0x02003110,(uint32*)0xE3A00001,3);//NandInit		
			fixAddressValue((uint32*)0x02003114,(uint32*)0xE12FFF1E,3);//NandInit 	
			fix_4812();
		}
		break;
	case 0x13://4828 - 口袋妖怪：金心（德
	case 0x14://4829 - 口袋妖怪：银灵（德）
		{
			fixAddressValue((uint32*)0x020de220,(uint32*)0xe3a000aa,3);
			break;
		}
	case 0x15://4833 - 口袋妖怪：金心（西班）
		{
			fixAddressValue((uint32*)0x020de218,(uint32*)0xe3a000aa,3);
			break;	
		}
	case 0x16://4834 - 口袋妖怪：银灵（西班）
		{
			fixAddressValue((uint32*)0x020de220,(uint32*)0xe3a000aa,3);
			break;
		}
	case 0x17://4835 - 口袋妖怪：金心（意大）
	case 0x1A://4843 - 口袋妖怪：银灵（意大）
		fixAddressValue((uint32*)0x020de220,(uint32*)0xe3a000aa,3);
		break;
	case 0x18://4841 - 口袋妖怪：金心（欧）
	case 0x19://4842 - 口袋妖怪：银灵（欧）
		fixAddressValue((uint32*)0x020de220,(uint32*)0xe3a000aa,3);
		break;

	case 0x1C://4915 老子制造
		{
			*(vuint32*)0x020A65B8 = 0xDF0B0000; //SWI B
			WritePatchInfo_4BYTE((uint32)0x020A65B8,0xDF0B0000);
			copyCode((void*)((uint32)SAVE_32M_3690_start),(void*)0x2000230,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start);
			//--ww-- 登记写入
			WritePatchInfo((uint32)0x2000230,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start,(uint32*)SAVE_32M_3690_start);
			fixAddressValue((uint32*)0x02003940,(uint32*)0xE12FFF1E,3);//NandWait
			fixAddressValue((uint32*)0x02003B54,(uint32*)0xE3A00003,3);//NandState		
			fixAddressValue((uint32*)0x02003B58,(uint32*)0xE12FFF1E,3);//NandState
			fixAddressValue((uint32*)0x02003B6C,(uint32*)0,1);//NandError
			fixAddressValue((uint32*)0x020031B0,(uint32*)0xE3A00001,3);//NandInit 		
			fixAddressValue((uint32*)0x020031B4,(uint32*)0xE12FFF1E,3);//NandInit 	
			fix_4915();
		}
		break;
	case 0x1D://4933
	case 0x2C://5246
		*(vuint32*)0x020E9120 = 0xE3A00002; 
		WritePatchInfo_4BYTE((uint32)0x020E9120,0xE3A00002);
		*(vuint32*)0x020E9124 = 0xEA000029; 
		WritePatchInfo_4BYTE((uint32)0x020E9124,0xEA000029);
		break;
	case 0x28:
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02188084,0x02180C80);
		break;
	case 0x29:
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02188064,0x02180C60);
		break;
	case 0x2E://4951 大合奏
		{	
			*(vuint32*)0x020E8354 = 0xDF0B0000; //SWI B
			WritePatchInfo_4BYTE((uint32)0x020E8354,0xDF0B0000);
			copyCode((void*)((uint32)SAVE_32M_3690_start),(void*)0x2000300,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start);
			//--ww-- 登记写入
			WritePatchInfo((uint32)0x2000300,(uint32)SAVE_32M_3690_end-(uint32)SAVE_32M_3690_start,(uint32*)SAVE_32M_3690_start);
			fixAddressValue((uint32*)0x02061c24,(uint32*)0,1);//NandError
			fixAddressValue((uint32*)0x020613cc,(uint32*)0xE3A00001,3);//NandInit 		
			fixAddressValue((uint32*)0x020613D0,(uint32*)0xE12FFF1E,3);//NandInit 
			fixAddressValue((uint32*)0x02061a4c,(uint32*)0xE3A00000,3);//NandResume 		
			fixAddressValue((uint32*)0x02061a50,(uint32*)0xE12FFF1E,3);//NandResume 
			fix_4951();
		}
		break;
	case 0x30://5292
		patch_5292();
		break;
	case 0x31://5387
		patch_5387();
		break;
	case 0x32://5367
		patch_5367();
		break;
	case 0x33://5400
		patch_5400();
		break;
	case 0x34:
	{
        *(vuint32*)0x02039384 = 0xEA000008 ;
        *(vuint32*)0x020398F0 = 0xFAFF25C4 ;//blx 2003008
        break;
	}
	case 0x35:
	{
        *(vuint32*)0x02039D20 = 0xEA000008 ;
        *(vuint32*)0x0203A28C = 0xFAFF235D ;
		break;
	}
	case 0x3A:
	{
        *(vuint32*)0x0203A888 = 0xEA000008 ;
        *(vuint32*)0x0203ADF4 = 0xFAFF2083 ;
		break;
	}
	case 0x3B://5584
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02187F84,0x02180A00);
		break;
	case 0x3C://5585
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187F64,0x021809E0);
		break;
	case 0x3D://5586
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02188004,0x02180980);
		break;
	case 0x3E://5587
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187EE4,0x02180960);
		break;
	case 0x3F://5588
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02187EC4,0x02180940);
		break;
	case 0x40://5589
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187EA4,0x02180920);
		break;
	case 0x41://5593
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02187F84,0x02180A00);
		break;	
	case 0x42://5594
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187F64,0x021809E0);
		break;
	case 0x43://5598-PEB
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02187F24,0x021809A0);
		break;
	case 0x44://5599-PEN
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187F24,0x021809A0);
		break;
	case 0x45://5600-PVB
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDD5,0x02004600,0x02187E84,0x02180900);
		break;	
	case 0x46://5601-PVN
		patch_global_Pokemon(0X02004EA4 ,0xEAFFFDED,0x02004660,0x02187E64,0x021808E0);
		break;
	case 0x47://5499
		fix_data_5400[5] = 0xA2DD;
		patch_global_pp(0X020009f8 ,0xEAFFFF06,0x02000600,0x020F8DC0,0x020F7198,0x02002700);
		break;
	case 0x48://5529
		patch_global_pp(0X020009f8 ,0xEAFFFEC6,0x02000500,0x020E84B0,0x020E6D1C,0x02002600);
		break;
	case 0x49://5530
		fix_data_5400[5] = 0xA2DD;
		patch_global_pp(0X020009f8 ,0xEAFFFF0A,0x02000610,0x0212AB20,0x02128EF8,0x02002710);
		break;
	case 0x4A://5592
		patch_global_pp(0X020009f8 ,0xEAFFFEC6,0x02000500,0x022DFC30,0x022DE4E4,0x02002600);
		break;
	default:
		break;
	}		
	//补丁overlay
	//patch_Overlay();
}
CODE_IN_ITCM bool patch_special_games(void)
{
	uint32 gamecode = *(vuint32*)0x27FFE0C;
	switch(gamecode)
	{
	case 0x45584759:
	case 0x50584759:
		fixAddressValue((uint32*)0x020fcb3c,(uint32*)0x22329203,3);
		break;
	default:
		break;	
	
	}
	return true;
}