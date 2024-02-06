
	PRESERVE8
    AREA      PatchNDSGame ,CODE,READONLY
    
	EXPORT    ITCM_DC_FlushAll
	EXPORT    ReadArm9To2000000
	
	EXPORT	  hybrid_Read512
	EXPORT	  SendError
	
	export	  bxr0
	export	  ResetEveryMemory
	;;-------------------------

	export	A9_ReadSram
	export	ds_WriteSram

bxr0
  	mov 	r12,r0
  	mov 	r0,r1
  	mov 	r1,r2
	bx		r12	
	
ResetEveryMemory
	stmfd	sp!,{r2-r12,lr}
	MOV		R1,#0
loc_6D4
    MOV     R0, #0
loc_6D8
    ORR     R2, R1, R0
    MCR     p15, 0, R2,c7,c14, 2
    ADD     R0, R0, #0x20
    CMP     R0, #0x400
    BNE     loc_6D8

    ADD     R1, R1, #0x40000000
    CMP     R1, #0
    BNE     loc_6D4

    MOV     R0, #0
    MCR     p15, 0, R0,c7,c5
    MCR     p15, 0, R0,c7,c6
    MCR     p15, 0, R0,c7,c10, 4
    MCR     p15, 0, R0,c3,c0
    MCR     p15, 0, R0,c2,c0
    MOV    	R0,#0x800000
    ADD		r0,r0,#0xA
    MCR     p15, 0, R0,c9,c1
    MOV     R0, #0xC
    MCR     p15, 0, R0,c9,c1, 1
    MOV     R0, #0x1F
    MSR     cpsr_cxsf, R0

	MVN		R12,#0
	MOV      r1,#0
    LDR     R0, =0x803000
    STR     R1, [R0,#0xFFC]
    STR     R12, [R0,#0xFF8]
    
resetMemory2_stage
    LDR     R3, =0x4000100
    MOV     R5, #0
    MOV     R0, #0
    SUB     R12, R3, #0x4C
    SUB     LR, R3, #0x50
    SUB     R2, R3, #0x48


ClearDMA
    ADD     R1, R0, R0,LSL#1
    STR     R5, [R2,R1,LSL#2]
    STR     R5, [LR,R1,LSL#2]
    STR     R5, [R12,R1,LSL#2]
    ADD     R1, R3, R0,LSL#2
    STRH    R5, [R1,#2]
    STRH    R5, [R1]
    ADD     R0, R0, #1
    CMP     R0, #4
    BLT     ClearDMA

    LDR     R0, =0x80808080
    MOV     R4, #0x4000000
    STR     R0, [R4,#0x240]
    LDR     R6, =0x27FFF60
    STR     R5, [R6]
    MVN     R0, #0xFFFFFFFF
    LDR     R6, =0x27FFF60
    STRH    R0, [R1]
    SUB     R1, R1, R0,LSL#1
    ADD     R2, R0, R0,LSR#21
    MOV     R0, R6
    BL      asmdmaFillWords

    STRH    R5, [R4,#4]
    STR     R5, [R4]
    STR     R5, [R7]
    STRB    R5, [R4,#0x240]
    STRB    R5, [R4,#0x241]
    STRB    R5, [R4,#0x242]
    STRB    R5, [R4,#0x243]
    STRB    R5, [R4,#0x244]
    STRB    R5, [R4,#0x245]
    STRB    R5, [R4,#0x246]
    STRB    R5, [R4,#0x248]
    STRB    R5, [R4,#0x249]
    MOV     R0, #0x3000000
    STR     R0, [R4,#0x240]
    
    LDR     R0, =0x820F
    ADD     R1, R4, #0x300
    STRH    R0, [R1,#4]
    MOV     R0, #3
    STRB    R0, [R4,#0x247]

	ldmfd	sp!,{r2-r12,pc}

asmdmaFillWords
    MOV     R3, #0x4000000
    STR     R0, [R3,#0xD4]
    STR     R1, [R3,#0xD8]
    MOV     R0, #0x85000000
    ORR     R0, R0, R2,LSR#2
    STR     R0, [R3,#0xDC]
waitDMA_Busy
    LDR     R0, [R3,#0xDC]
    TST     R0, #0x80000000
    BNE     waitDMA_Busy
    BX      LR
	;stmfd	sp!,{r0-r1}
	;ldr		r0,=0x27ffe60
	;ldr		r1,=0x5F7e00
	;str		r1,[r0]
	;ldmfd	sp!,{r0-r1}
	;bx	lr	
	;LTORG
ITCM_DC_FlushAll
	stmfd	sp!,{r0-r3}
	mov	r1, #0
outer_loop
	mov	r0, #0
inner_loop
	orr	r2, r1, r0			; generate segment and line address
	mcr	p15, 0, r2, c7, c14, 2		; clean and flush the line
	add	r0, r0, #32
	cmp	r0, #0x1000/4
	bne	inner_loop
	add	r1, r1, #0x40000000
	cmp	r1, #0
	bne	outer_loop
	ldmfd	sp!,{r0-r3}
	bx	lr	
;/*---------------------------------------------------------------------------------
;	memery set in ITCM ,为了启动游戏设置
;---------------------------------------------------------------------------------*/
rst_MemSet32CPU
;/* MEMCHK_SET(3,v,dst,len); */
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ReadArm9To2000000
	stmfd sp!,{r0-r12,lr}	
	    
;clear MainMemory
  	mov r0, #0x02000000
  	mov r1, #0x02300000
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

;//允许arm9操作GBA槽
	mov 	r12,#0x04000000
	ldr		r10,[r12,#0x204]
	bic		r11,r10,#0x880
	str		r11,[r12,#0x204]	
	
;// copy arm9 代码
	ldr 	r1,=0x27FFE00
	ldr 	r0,[r1,#0x20]
	ldr 	r3,[r1,#0x28]
	ldr 	r4,[r1,#0x2C]	
	mov 	r1,r3
Copy_arm9
	bl  	hybrid_Read512
	add 	r0,r0,#0x200
	add		r1,r1,#0x200
	subs 	r4,r4,#0x200
	bhi  	Copy_arm9
	
	ldmfd   sp!,{r0-r12,pc}	
;;********************************************
;r0,地址 r1 buffer ,混合模式读512数据
;;&*******************************************
hybrid_Read512
	stmfd	sp!,{r0-r7,lr}
	mov 	r1,r0
	MOV 	R0, R1,LSR#8
	ORR 	R0, R0, #0xB7000000
	MOV 	R1, R1,LSL#24
	stmfd	sp!,{r0-r1}
hybrid_loop1
 	orr   	r1,r1,#0x10000
	bl  	dscard_4byte
	ands  	r0,r0,#0xFF000000
	ldmfd	sp,{r0-r1}
  	bne   	hybrid_loop1
hybrid_quit
	ldmfd	sp!,{r0-r1}
	ldmfd	sp!,{r0-r7}
	bl		dscard_512bytes
	ldmfd	sp!,{pc}
;***********准备读4byte************	
;r0,r1,命令
dscard_4byte
	stmfd	sp!,{r2-r3,lr}
	bl		dscard_setop
	ldr   	r3,=0x040001A0
	ldr   	r2,=0x04100010
	ldr   	r1,=0xA7586000
	str		r1,[r3,#4]
read4_loop1
	ldr   	r1,[r3,#4]    
	ands	r1,r1,#0x800000
	beq  	read4_loop1
	ldr		r0,[r2]
	ldmfd	sp!,{r2-r3,pc}
	
;***********准备读512byte************	
;r0,地址 r1 buffer
dscard_512bytes
	stmfd	sp!,{r0-r7,lr}
	stmfd 	sp!,{r0-r1}	
	mov r1,r0
	MOV R0, R1,LSR#8
	ORR R0, R0, #0xB7000000
	MOV R1, R1,LSL#24
	bl  dscard_setop
	ldmfd	sp!,{r0-r1}
	
	ldr r7,=0xA1586000
	ldr r6,=0x040001a0
	ldr r5,=0x04100010
	str	r7,[r6,#4]
read_loop1
	ldr r7,[r6,#4]
	ands r3,r7,#0x800000
	beq read_loop2
	ldr r0,[r5]
	str	r0,[r1],#+4
read_loop2
	ands r3,r7,#0x80000000
	bne  read_loop1
	ldmfd	sp!,{r0-r7,pc}	
	
;***********准备写DScommand************	
;r0 -- 写入的高位地址 r1 写入的低位地址
dscard_setop
	stmfd 	sp!,{r11-r12}	
	ldr 	r12,=0x040001A0
op_loop
	ldr 	r11,[r12,#4]
	ands 	r11,r11,#0x80000000
	bne 	op_loop
	
	mov 	r11,#0xc0
	strb 	r11,[r12,#1]
		
	mov	 r11,r0,lsr#24
	strb r11,[r12,#8]
	mov	 r11,r0,lsr#16
	strb r11,[r12,#9]
	mov	 r11,r0,lsr#8
	strb r11,[r12,#0xA]
	strb r0,[r12,#0xB]	
	mov	 r11,r1,lsr#24
	strb r11,[r12,#0xC]
	mov	 r11,r1,lsr#16
	strb r11,[r12,#0xD]
	mov	 r11,r1,lsr#8
	strb r11,[r12,#0xE]
	strb r1,[r12,#0xF]
	ldmfd 	sp!,{r11-r12}	
	bx		lr

;/*---------------------------------------------------------------------------------
;	发送一些标志， void SendError(uint32 a)
;   以便于逻辑分析扑捉
;---------------------------------------------------------------------------------*/	
SendError
	stmfd	sp!,{lr}
	mov		r1,r0
	ldr		r0,=0xBB000000
	bl		dscard_4byte
	ldmfd	sp!,{pc}
;=============================================	
;;*********************************************************
;*　ARM指令的readSram
;;*********************************************************		
A9_ReadSram
	;r0  address to read
	stmfd	sp!,{r1-r2,lr}
	ldr		r2,=0x7FF
	and     r0,r0,r2
;//对于spi来说	
	ldr		r2,=0xB5010000
	lsr		r1,r0,#8
	ORR		r2,r1
	lsl		r1,r0,#24
	mov		r0,r2
	bl		dscard_4byte
	ldmfd	sp!,{r1-r2,lr}
	bx		lr
;//-------------------------------------------	
ds_WriteSram
	;@r0 address , r1  16bit data
	stmfd	sp!,{r1-r3,lr}
	;ldr		r2,=0xB8F20000
 IF WriteSRAM =1
	ldr		r2,=0xB4F20000   
 ELIF WriteSRAM =2
 	ldr		r2,=0xB4F30000
 ELIF WriteSRAM =3
 	ldr		r2,=0xB4F40000
 ENDIF
	lsr		r3,r0,#8
	ORR		r2,r3
	mov		r3,r1
	lsl		r1,r0,#24
	mov		r0,r2
	
	lsl		r2,r3,#16
	lsr		r2,r2,#24
	orr		r1,r2
	lsl		r3,r3,#24
	lsr		r3,r3,#16
	orr		r1,r3
	bl		dscard_4byte
	ldmfd	sp!,{r1-r3,pc}
;//--------------------------------------------
	 LTORG
;;#################################################################################
	END	
