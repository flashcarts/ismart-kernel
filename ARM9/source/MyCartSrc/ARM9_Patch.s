;;#################################################################################
	AREA    ARM9_Patch ,CODE,READONLY

	EXPORT  ReplaceIRQ_start_arm9
	EXPORT  ReplaceIRQ_end_arm9

	EXPORT  quit_irq
	CODE16
ReplaceIRQ_start_arm9
	push	{r7}
	push	{r0-r6,lr}
	ldr		r7,=0x4000000
	ldrh	r6,[r7,#6]
	cmp		r6,#0xa0
	bhi		check_key
	b		quit_irq 
check_key	
	ldr		r7,=0x4000130
	ldrh	r6,[r7]
	cmp		r6,#0xFB	;@L+R+
	beq		SoftReset_PROC
	b		quit_irq
	
SoftReset_PROC
	;需要将ARM7 ARM9全部copy，然后再启动
	ldr		r6,=0x4000200
	str		r6,[r6,#8]
	ldr		r5,=0xe000
	strh	r5,[r6,#4]
	
	;从PSRAM中读取软复位代码，然后复位
	;ldr		r7,=0x77000;旧的spram地址
	;mov 	r7,#0;sram的地址
	ldr 	r7,=0x17F000   ;软复位的代码放到这个位置
	ldr		r5,=0x2340000
	;ldr		r4,=0x1000
	ldr		r4,=0x600
	ldr		r3,=0x200
copy_ARM9	
	mov		r0,r7
	mov		r1,r5
	bl		arm9_ReadFlash
	add		r5,r3
	add		r7,r3
	sub		r4,r3
	bne		copy_ARM9
	
	;blx		startshow
	ldr		r7,=0x2340000
	bx		r7
;------------------------
	code32	
startshow
	ldr r1,=0x20100
	mov r3,#0x4000000
	str r1,[r3]	
	mov r1,#0x80
	str r1,[r3,#0x240]	
	ldr r1,=0x03e0  ;白
	mov r3,#0x6800000	
	mov r6,#0x46     ;70 ;高
	ldr r2,=0x7aa6   ;白框的起始地址
loop111	
	mov r5,#0x5a ;90 ;宽
loop222	
	strh r1,[r3,r2]
	add r2,#0x2
	subs r5,#1
	bne loop222
	add r2,#0x14c  ;下一行
	subs r6,#1
	bne loop111
	b .
	code16
;---------------------------	
arm9_ReadFlash	
	;r0  address to read , r1 buffer
	push	{r2-r7,lr}
	push	{r0-r1}
	;ldr		r2,=0x7ff
	;and     r0,r0,r2
	ldr		r2,=0xB5000000
	lsr		r1,r0,#8
	ORR		r2,r1
	lsl		r1,r0,#24
	mov		r0,r2
	bl		arm9reset_SetRomOP
	pop		{r0-r1}
	bl		IRQ_arm7_Read512
	pop		{r2-r7,pc}

;r0，r1, 为传递的参数
IRQ_arm7_Read512	
;r0 null r1,buffer
	push	{r2-r7,lr}
	ldr 	r7,=0xA1586000
	ldr 	r6,=0x040001a0
	ldr 	r5,=0x04100010
	str		r7,[r6,#4]
	mov		r3,#8
	lsl		r3,#20
IRQ_ARM7_read_loop1
	ldr 	r7,[r6,#4]
	tst 	r3,r7
	beq 	IRQ_ARM7_read_loop2
	ldr 	r0,[r5]
	stmia	r1!,{r0}
IRQ_ARM7_read_loop2
	lsr		r7,#31
	bne  	IRQ_ARM7_read_loop1
	pop		{r2-r7,pc}
		
arm9reset_SetRomOP 	 ;r0,r1 传送的参数
	ldr 	r3,=0x040001A0
arm9reset_oploop
	ldr 	r4,[r3,#4]
	lsr 	r4,#31
	bne 	arm9reset_oploop
	mov 	r4,#0xc0
	strb 	r4,[r3,#1]
	lsr	 	r4,r0,#24
	strb 	r4,[r3,#8]
	lsr	 	r4,r0,#16
	strb 	r4,[r3,#9]
	lsr	 	r4,r0,#8
	strb 	r4,[r3,#10]
	strb 	r0,[r3,#11]
	
	lsr	 	r4,r1,#24
	strb 	r4,[r3,#12]
	lsr	 	r4,r1,#16
	strb 	r4,[r3,#13]
	lsr	 	r4,r1,#8
	strb 	r4,[r3,#14]
	strb 	r1,[r3,#15]
	bx  	lr

quit_irq
	pop		{r0-r6}
	pop		{r7}		;;恢复LR
	mov		lr,r7
	ldr		r7,=0x1FFA040
	mov		r12,r7
	pop		{r7}
	bx		r12
	ALIGN 4	
	LTORG	
ReplaceIRQ_end_arm9


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	END