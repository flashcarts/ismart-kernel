
/*
 referrence from 2006-01-13 - v2.11
 
  NDS MP
 GBAMP NDS Firmware Hack Version 2.0
 An NDS aware firmware patch for the GBA Movie Player.
 By Michael Chisholm (Chishm)
 
 Large parts are based on MultiNDS loader by Darkain.
 Filesystem code based on gbamp_cf.c by Chishm (me).
 Flashing tool written by DarkFader.
 Chunks of firmware removed with help from Dwedit.

 GBAMP firmware flasher written by DarkFader.
 
 This software is completely free. No warranty is provided.
 If you use it, please give due credit and email me about your
 project at chishm@hotmail.com
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Loader functions

CODE_IN_ITCM asm void reset_MemSet32CPU(u32 v,void *dst,u32 len)
{/* MEMCHK_SET(3,v,dst,len); */
s32data RN r0
s32pbuf RN r1
s32size RN r2
  
  cmp s32size,#0
  bxeq lr
  
  PUSH {r4,r5,r6,r7,r8,r9}
  
  mov r3,s32data
  mov r4,s32data
  mov r5,s32data
  mov r6,s32data
  mov r7,s32data
  mov r8,s32data
  mov r9,s32data
  
  cmp s32size,#4*8
  blo s32set32x1
    
s32set32x8
  stmia s32pbuf!,{s32data,r3,r4,r5,r6,r7,r8,r9}
  subs s32size,s32size,#4*8
  cmp s32size,#4*8
  bhs s32set32x8
      
  cmp s32size,#0
  beq s32setend
   
s32set32x1
  str s32data,[s32pbuf],#4
  subs s32size,s32size,#4
  bne s32set32x1

s32setend
  POP {r4,r5,r6,r7,r8,r9}
  bx lr      
}

/*-------------------------------------------------------------------------
resetMemory2_ARM9
Clears the ARM9's icahce and dcache
Written by Darkain.
Modified by Chishm:
 Changed ldr to mov & add
 Added clobber list
--------------------------------------------------------------------------*/
asm void resetMemory2_ARM9 (vu32 *pBootAddress,u32 BootAddress,bool ClearMainMemory)
{
	// backup params
  mov r10,r0
  mov r11,r1
  mov r12,r3
  
  cmps r2,#0
  beq clearMainMemory_end

	// clear MainMemory
    mov r0, #0x02000000
    add r1,r0,#2*1024*1024
    sub r1,r1,#16*1024
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
clearMainMemory_loop
	stmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	cmps r0,r1
	bne clearMainMemory_loop
clearMainMemory_end
	
  mov r1, #0
outer_loop
   mov r0, #0
inner_loop
    orr r2, r1, r0
    mcr p15, 0, r2, c7, c14, 2
    add r0, r0, #0x20
    cmp r0, #0x400
   bne inner_loop
   add r1, r1, #0x40000000
   cmp r1, #0x0
  bne outer_loop
    
  mov r1, #0
  mcr p15, 0, r1, c7, c5, 0 // Flush ICache
  mcr p15, 0, r1, c7, c6, 0 // Flush DCache
  mcr p15, 0, r1, c7, c10, 4 // empty write buffer

  mcr p15, 0, r1, c3, c0, 0 // disable write buffer       (def = 0)

#define ITCM_LOAD (1<<19)
#define ITCM_ENABLE (1<<18)
#define DTCM_LOAD (1<<17)
#define DTCM_ENABLE (1<<16)
#define DISABLE_TBIT (1<<15)
#define ROUND_ROBIT (1<<14)
#define ALT_VECTORS (1<<13)
#define ICACHE_ENABLE (1<<12)
#define BIG_ENDIAN (1<<7)
#define DCACHE_ENABLE (1<<2)
#define PROTECT_ENABLE (1<<0)
	
  // disable DTCM and protection unit
	mrc	p15, 0, r0, c1, c0, 0
	ldr r1,= ~(ITCM_ENABLE | DTCM_ENABLE | ICACHE_ENABLE | DCACHE_ENABLE | PROTECT_ENABLE)
	and r0,r0,r1
	ldr r1,=2_01111000 ; set SBO
	orr	r0,r0,r1
	ldr r1,= ITCM_ENABLE | DTCM_ENABLE
	orr	r0,r0,r1
	mcr	p15, 0, r0, c1, c0, 0
  
  mcr p15, 0, r1, c6, c0, 0 // disable protection unit 0  (def = 0)
  mcr p15, 0, r1, c6, c1, 0 // disable protection unit 1  (def = 0)
  mcr p15, 0, r1, c6, c2, 0 // disable protection unit 2  (def = 0)
  mcr p15, 0, r1, c6, c3, 0 // disable protection unit 3  (def = 0)
  mcr p15, 0, r1, c6, c4, 0 // disable protection unit 4  (def = ?)
  mcr p15, 0, r1, c6, c5, 0 // disable protection unit 5  (def = ?)
  mcr p15, 0, r1, c6, c6, 0 // disable protection unit 6  (def = ?)
  mcr p15, 0, r1, c6, c7, 0 // disable protection unit 7  (def = ?)
    
    mov r1, #0x0000000C // Use mov instead
	mcr p15, 0, r1, c9, c1, 1 // ITCM base  (def = 0x0000000C) ???
    
    mov r1, #0x00800000 // Use mov instead
    add r1, r1, #0x00A
    mcr p15, 0, r1, c9, c1, 0 //DTCM base  (def = 0x0080000A) ???
	
    mov r1, #0
    mcr p15, 0, r1, c5, c0, 3 // IAccess
    mcr p15, 0, r1, c5, c0, 2 // DAccess

    mov r1, #0x1F
	msr cpsr_cxsf, r1
	
	// clear ITCM
    mov r0, #0x00000000
    add r1,r0,#32*1024
    mov r2,#0
clearITCM_loop
	str r2,[r0],#4
	cmps r0,r1
	bne clearITCM_loop
	  
	// clear DTCM
   mov r0, #0x00800000
   add r1,r0,#16*1024
   mov r2,#0
clearDTCM_loop
	str r2,[r0],#4
	cmps r0,r1
	bne clearDTCM_loop

	str r11,[r10]
	
swireset	
  swi	0x00<<16
}

////////////////////////////////////////////--------------------

CODE_IN_ITCM asm void ITCM_resetMemory2_ARM9 (vu32 *pBootAddress,u32 BootAddress,u32 ClearMainMemory)
{
    
    mov r10,r0
    mov r11,r1
    mov r12,r2
    
    mov r1, #0
ITCM_outer_loop
    mov r0, #0
ITCM_inner_loop
    orr r2, r1, r0
    mcr p15, 0, r2, c7, c14, 2
    add r0, r0, #0x20
    cmp r0, #0x400
    bne ITCM_inner_loop
    add r1, r1, #0x40000000
    cmp r1, #0x0
    bne ITCM_outer_loop
      
     
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 0 // Flush ICache
    mcr p15, 0, r1, c7, c6, 0 // Flush DCache
    mcr p15, 0, r1, c7, c10, 4 // empty write buffer
    mcr p15, 0, r1, c3, c0, 0 // disable write buffer       (def = 0)
    
    MCR p15, 0, R1,c2,c0,0    
    MCR p15, 0, R1,c2,c0,1    

#define ITCM_LOAD (1<<19)
#define ITCM_ENABLE (1<<18)
#define DTCM_LOAD (1<<17)
#define DTCM_ENABLE (1<<16)
#define DISABLE_TBIT (1<<15)
#define ROUND_ROBIT (1<<14)
#define ALT_VECTORS (1<<13)
#define ICACHE_ENABLE (1<<12)
#define BIG_ENDIAN (1<<7)
#define DCACHE_ENABLE (1<<2)
#define PROTECT_ENABLE (1<<0)
    
  // disable DTCM and protection unit
    mrc p15, 0, r0, c1, c0, 0
    ldr r1,= ~(ITCM_ENABLE | DTCM_ENABLE | ICACHE_ENABLE | DCACHE_ENABLE | PROTECT_ENABLE)
    and r0,r0,r1
    ldr r1,=2_01111000 ; set SBO
    orr r0,r0,r1
    ldr r1,= ITCM_ENABLE | DTCM_ENABLE
    orr r0,r0,r1
    mcr p15, 0, r0, c1, c0, 0
  
    mcr p15, 0, r1, c6, c0, 0 // disable protection unit 0  (def = 0)
    mcr p15, 0, r1, c6, c1, 0 // disable protection unit 1  (def = 0)
    mcr p15, 0, r1, c6, c2, 0 // disable protection unit 2  (def = 0)
    mcr p15, 0, r1, c6, c3, 0 // disable protection unit 3  (def = 0)
    mcr p15, 0, r1, c6, c4, 0 // disable protection unit 4  (def = ?)
    mcr p15, 0, r1, c6, c5, 0 // disable protection unit 5  (def = ?)
    mcr p15, 0, r1, c6, c6, 0 // disable protection unit 6  (def = ?)
    mcr p15, 0, r1, c6, c7, 0 // disable protection unit 7  (def = ?)

    
    mov r1, #0x0000000C // Use mov instead
    mcr p15, 0, r1, c9, c1, 1 // ITCM base  (def = 0x0000000C) ???
    
    mov r1, #0x00800000 // Use mov instead
    add r1, r1, #0x00A
    mcr p15, 0, r1, c9, c1, 0 //DTCM base  (def = 0x0080000A) ???
    
    mov r1, #0
    mcr p15, 0, r1, c5, c0, 3 // IAccess
    mcr p15, 0, r1, c5, c0, 2 // DAccess
    mcr p15, 0, r1, c5, c0, 1 // IAccess
    mcr p15, 0, r1, c5, c0, 0 // DAccess

    mov r1, #0x1F
    msr cpsr_cxsf, r1

    // clear ITCM
     mov r0, #0x00000000
     add r1,r0,#32*1024
     mov r2,#0
clearITCM_loop_i
     str r2,[r0],#4
     cmps r0,r1
     bne clearITCM_loop_i
    
    // clear DTCM
    mov r0, #0x00800000
    add r1,r0,#16*1024
    mov r2,#0
clearDTCM_loop_I
    str r2,[r0],#4
    cmps r0,r1
    bne clearDTCM_loop_I

    MVN     R2,#0
    MOV     r1,#0
    LDR     R0, =0x803000
    STR     R1,[R0,#0xFFC]
    str     r2,[r0,#0xFF8]

    
    cmp     r12,#0
    beq     selfboot
    cmp		r12,#2
    beq		boot_slot2
                
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;特殊
    ldr     r3,=0x2330000
    stmia   r3,{r10,r11}
    ldr     r3,=0x2330800
    bx      r3

selfboot    
    str     r11,[r10]
                 
    ldr     r3,=0x2330000
    stmia   r3,{r10,r11}
    
    mov     r3,#0x04000000
    ldr     r2,=0xE880
    str     r2,[r3,#0x204]
    
    ldr     r4,=0xEF000000    //this is reset           
    ldr     r0,=0x027FFFC0
    ldr     r2,=0xe51ff004
    add     r3,r0,#4
    str     r2,[r0,#4]
    str     r3,[r0,#8]
    str     r4,[r0,#0xc]             
    bx      r3 
    
boot_slot2
	str     r11,[r10]
	             
	ldr     r3,=0x2330000
	stmia   r3,{r10,r11}
	
	mov     r3,#0x04000000
	ldr     r2,=0xE880
	str     r2,[r3,#0x204]
                
    ldr		r0,=0x027ffe04
    ldr		r1,=0xe59ff018
    str		r1,[r0]
    add		r2,r0,#0x20
    str		r0,[r2]
    ldr		r3,=0x27FF200
    ldr		r4,=0xFDEf54AA
    str		r4,[r3]
    bx		r0  		    
}

/*-------------------------------------------------------------------------
resetMemory1_ARM9
Clears the ARM9's DMA channels and resets video memory
Written by Darkain.
Modified by Chishm:
 * Changed MultiNDS specific stuff
--------------------------------------------------------------------------*/
CODE_IN_ITCM void MI_StopDma(uint32 dmaNo)
{
    
        vuint16   *dmaCntp = &((vuint16 *)0x40000b0)[dmaNo * 6 + 5];

        *dmaCntp &= ~(0x3A00);  //~((MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON) >> 16)
        *dmaCntp &= ~(0x8000) ; // ~(MI_DMA_ENABLE >> 16)

    //---- ARM9 must wait 2 cycle (load is 1/2 cycle)
    {
        vint32     dummy = dmaCntp[0];
    }
   {
        vint32     dummy = dmaCntp[0]; 
    }

    //---- for multiple DMA problem in DMA0
    if (dmaNo == 0)
    {
        vuint32   *p = (vuint32 *)((uint32)0x40000b0 + dmaNo * 12);
        *p = (vuint32)0;
        *(p + 1) = (vuint32)0;
        *(p + 2) = (vuint32)0x81400001;
    }
}

CODE_IN_ITCM /*static*/ void resetMemory1_ARM9 (void) 
{
    //这个会导致屏幕出现一些文字出现
	for (u32 i=0; i<32*1024; i+=4) {
	}

 	register int i;
  
	// clear out ARM9 DMA channels
	for (i=0; i<4; i++) {
	    MI_StopDma(i);        
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}
	//先关闭显示
    DISPLAY_CR = 0 ;
    SUB_DISPLAY_CR = 0 ;

	VRAM_A_CR = VRAM_A_MAIN_BG_0x06000000;
	VRAM_B_CR = VRAM_B_MAIN_BG_0x06020000;
	VRAM_C_CR = VRAM_C_MAIN_BG_0x06040000;
	VRAM_D_CR = VRAM_D_MAIN_BG_0x06060000;
  reset_MemSet32CPU(0, (void*)0x06000000,  128*4*1024);
	VRAM_A_CR = 0;
	VRAM_B_CR = 0;
	VRAM_C_CR = 0;
	VRAM_D_CR = 0;
	
	VRAM_E_CR = VRAM_E_MAIN_BG;
  reset_MemSet32CPU(0, (void*)0x06000000,  64*1024);
	VRAM_E_CR = 0;
	VRAM_F_CR = VRAM_F_MAIN_BG;
  reset_MemSet32CPU(0, (void*)0x06000000,  16*1024);
	VRAM_F_CR = 0;
	VRAM_G_CR = VRAM_G_MAIN_BG;
  reset_MemSet32CPU(0, (void*)0x06000000,  16*1024);
	VRAM_G_CR = 0;
	
	VRAM_H_CR = VRAM_H_SUB_BG;
  reset_MemSet32CPU(0, (void*)0x06200000,  32*1024);
	VRAM_H_CR = 0;
	VRAM_I_CR = VRAM_I_SUB_BG;
  reset_MemSet32CPU(0, (void*)0x06200000,  16*1024);
	VRAM_I_CR = 0;
	
    VRAM_CR = 0x80808080;
  reset_MemSet32CPU(0, PALETTE, 2*1024);
    PALETTE[0] = 0xFFFF;
  reset_MemSet32CPU(0, OAM,     2*1024);
  reset_MemSet32CPU(0, (void*)0x04000000, 0x58); //clear main display registers
  reset_MemSet32CPU(0, (void*)0x04001000, 0x58); //clear sub  display registers
   
	REG_DISPSTAT=0;
	DISPLAY_CR = 0 ;
	SUB_DISPLAY_CR = 0 ;
	
	VRAM_A_CR = 0;
	VRAM_B_CR = 0;
	VRAM_C_CR = 0;
	VRAM_D_CR = 0;
	VRAM_E_CR = 0;
	VRAM_F_CR = 0;
	VRAM_G_CR = 0;
	VRAM_H_CR = 0;
	VRAM_I_CR = 0;
	VRAM_CR   = 0x03000000;
    
	*(uint8*)0x04000247 = 3; 
 
	REG_POWERCNT  = 0x820F;
}
