;//this file come from woodrpg
	PRESERVE8
    AREA      ARM7_Patch,CODE,READONLY
    
    EXPORT  arm7_interrup

	EXPORT	  AR_start
	EXPORT	  AR_end
	EXPORT	  fix_data
	EXPORT    is_softReset
	EXPORT 	  b_arm7Reset_wait
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;arm7 patch
	CODE32
arm7_interrup	
	stmfd   sp!,{r0-r7,lr}
checkVBLANK
	mov		r4, #0x4000000	;only interpret AR code in V-BLANK interrupt
	ldr		r0, [r4, #0x214]
	tst		r0, #0x00000001
	beq		PATCH_exit

	adr		r0, entryCounter
	ldr		r1, [r0]
	cmp		r1, #1	;run code per 2 frames
	addcc	r1, #1
	strcc	r1, [r0]
	bcc		PATCH_exit
	
	mov	r1, #0
	str	r1, [r0]

	str	r1, [r4, #0x208]  ;interrupt OFF

check_key_for_reset	
	;mov		r12, #0x2800000
	;ldr		r1,[r12,#-0xA0]  ;0x27fff60 1:reset ON
	adr		r12,is_softReset
	ldr		r1,[r12]
	cmp		r1,#1
	bne		SoftReset_skip
	add		r1, r4, #0x130
	ldrh	r6,[r1]
	cmp		r6,#0xFB	;@L+R+Select
	bne		SoftReset_skip
b_arm7Reset_wait
	ldr		r0,=0x12345678  ;这个有两种情况,1.无即时攻略时:bx os_resetsystem
	bx		r0				;			  2.bleq arm7Reset_wait
SoftReset_skip
	bl		AR_start
PATCH_return
	mov	r1, #1
	str	r1, [r4, #0x208]

PATCH_exit
	ldmfd   sp!,{r0-r7,lr}
	mov	r12, #0x2800000
	ldr	pc, [r12, #-0x98]  ;0x27FFF68 arm7 return address
entryCounter
	dcd	0x00000000
is_softReset
	dcd	0x00000000

;;================================================	
;

; CHEAT ENGINE	

; R12:	CodeTableStart
; R11:	Value
; R10:	Code
; R9:	Offset Register
; R8:	Execution Status
; R7:	Dx loop start
; R6:	Dx repeat amount
; R5:	Dx data
; R4:	Dx execution status
; R0-R3 are generic registers for the code processor to use
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	code32
AR_start
	stmfd	sp!, {r0-r12, lr}

	adr	r12, codeTableStart
	mov	r9, #0x00000000	; offset register
	mov	r8, #0x00000000	; execution status. bit0: 1 = no exec, 0 = exec. allows nested ifs 32 deep
	mov	r7, #0x00000000	; Dx loop start
	mov	r6, #0x00000000	; Dx repeat amount
	mov	r5, #0x00000000	; DX data
	mov	r4, #0x00000000	; Dx execution status

; increment counter
	;ldrh	r0, counter_value
	;add	r0, r0, #1
	;strh	r0, counter_value
	;modify by cpy
	adr		r0,counter_value
	ldr		r10,[r0]
	add		r10, r10, #1
	str		r10,[r0]

;	r0-r3 are generic registers for the code processor to use
;	0xCF000000 0x00000000 indicates the end of the code list
	
main_loop
	ldmia	r12!, {r10, r11}	; load a code
	cmp	r10, #0xCF000000
	beq	AR_return
	
	mov	r0, r10, lsr #28
	cmp	r0, #0xE
	beq	patch_code
	cmp	r0, #0xD
	beq	dx_codes
	cmp	r0, #0xC
	beq	type_c
	
; check execution status
	tst	r8, #0x00000001
	bne	main_loop
	
; check code group
	cmp	r0, #0x3
	blt	raw_write
	cmp	r0, #0x6
	ble	if_32bit
	cmp	r0, #0xB
	blt	if_16bit_mask
	beq	offset_load
	b	mem_copy_code
	
counter_value
	DCD	0x00000000
	
;;;;;;;;;;;;;;;;
; type 0-2 

raw_write
	cmp	r0, #0x1
	bic	r10, r10, #0xf0000000
	strlt	r11, [r10, r9]	; type 0
	streqh	r11, [r10, r9]	; type 1
	strgtb	r11, [r10, r9]	; type 2
	b	main_loop
	
;;;;;;;;;;;;;;;;
; type 3-6 
; r0 still contains the code type

if_32bit
	bic	r1, r10, #0xf0000001
	tst	r10, #0x00000001
	addne	r1, r1, r9	; add offset to the address if the lowest bit is set
	cmp	r1, #0x00000000
	moveq	r1, r9	; if address is 0, set address to offset
	ldr	r1, [r1]	; load word from [0XXXXXXX]
	mov	r8, r8, lsl #1	; push execution status
	cmp	r0, #0x3
	beq	if_32bit_bhi	; type 3
	cmp	r0, #0x5
	blt	if_32bit_bcc	; type 4
	beq	if_32bit_beq	; type 5
	; fall through to if_32bit_bne
	
; type 6
if_32bit_bne
	cmp	r11, r1
	orreq	r8, r8, #0x00000001
	b	main_loop

; type 3
if_32bit_bhi
	cmp	r11, r1
	orrls	r8, r8, #0x00000001
	b	main_loop

; type 4
if_32bit_bcc
	cmp	r11, r1
	orrcs	r8, r8, #0x00000001
	b	main_loop

; type 5
if_32bit_beq
	cmp	r11, r1
	orrne	r8, r8, #0x00000001
	b	main_loop


;;;;;;;;;;;;;;;;
; type 7-A 
; r0 still contains the code type

if_16bit_mask
	bic	r1, r10, #0xf0000001
	tst	r10, #0x00000001
	addne	r1, r1, r9	; add offset to the address if the lowest bit is set
	cmp	r1, #0x00000000
	moveq	r1, r9	; if address is 0, set address to offset
	ldrh	r1, [r1]	; load halfword from [0XXXXXXX]
	mov	r2, r11, lsr #16	; bit mask
	mov	r3, r11, lsl #16	; compare data
	mov	r3, r3, lsr #16
	bic	r1, r1, r2	; clear any bit that is set in the mask
	
	mov	r8, r8, lsl #1	; push execution status
	cmp	r0, #0x7
	beq	if_16bit_bhi
	cmp	r0, #0x9
	blt	if_16bit_bcc
	beq	if_16bit_beq
	; fall through to if_16bit_bne
	
; ?XXXXXXX ZZZZYYYY
; r1 is [0XXXXXXX]
; r2 is ZZZZ or bitmask
; r3 is YYYY
	
; type A
if_16bit_bne
	cmp	r3, r1
	orreq	r8, r8, #0x00000001
	b	main_loop

; type 7
if_16bit_bhi
	cmp	r3, r1
	orrls	r8, r8, #0x00000001
	b	main_loop

; type 8
if_16bit_bcc
	cmp	r3, r1
	orrcs	r8, r8, #0x00000001
	b	main_loop

; type 9
if_16bit_beq
	cmp	r3, r1
	orrne	r8, r8, #0x00000001
	b	main_loop

;;;;;;;;;;;;;;;;
; type B

offset_load
	bic	r10, r10, #0xf0000000
	ldr	r9, [r10, r9]
	b	main_loop

;;;;;;;;;;;;;;;;
; type C
type_c 
	mov	r0, r10, lsr #24
	cmp	r0, #0xC1
	beq	exec_remote_function
	cmp	r0, #0xC2
	beq	exec_custom_function
	
	tst	r8, #0x00000001	; make sure execution is enabled
	bne	main_loop
	
	cmp	r0, #0xC0
	beq	loop_start
	cmp	r0, #0xC5
	beq	execution_counter	; type C5

offset_data_store	; type C4
	sublt	r9, r12, #0x08	; Offset = Two words back from current code pointer (ie, word at C4000000 code)

save_offset	; type C6
	strgt	r9, [r11]	; Save offset to [XXXXXXXX] in C6000000 XXXXXXXX
	b	main_loop
	
loop_start
	mov	r4, r8	; execution status
	mov	r6, r11	; loop repeat amount
	mov	r7, r12	; loop start
	b	main_loop
	
exec_remote_function
	and	r0, r10, #0x00000007
	ldmia	r12, {r0, r1, r2, r3}	; load arguments
	add	r12, r12, r0	; move code pointer to where it should be
	tst	r8, #0x00000001
	bne	align_cheat_list	; execution disabled
	stmdb	sp!, {r12}
	adr	lr, exec_function_return
	bx	r11
	
exec_custom_function
	and	r0, r10, #0x00000001	; thumb mode?
	add	r0, r0, r12	; custom function location
	add	r12, r12, r11
	tst	r8, #0x00000001
	bne	align_cheat_list	; execution disabled
	stmdb	sp!, {r12}
	adr	lr, exec_function_return
	bx	r0

exec_function_return
	ldmia	sp!, {r12}
	b	align_cheat_list

execution_counter
	adr	r0, counter_value
	ldrh	r0, [r0]	; counter_value is out of immediate range for ldrh
	mov	r1, r11, lsl #16	; upper 16bit = mask, lower 16 = compare value
	and	r0, r0, r1, lsr #16	; mask off counter
	mov	r8, r8, lsl #1	; push execution status
	cmp	r0, r11, lsr #16	; Compare against compare value component of code
	orrne	r8, r8, #0x00000001	; set execution to false
	b	main_loop
;;;;;;;;;;;;;;;;
; type D

dx_codes
	mov	r0, r10, lsr #24
	cmp	r0, #0xD3
	bcs	dx_conditional_exec

	cmp	r0, #0xD1
	bgt	end_loop_clear
	beq	end_loop_no_clear
	; fall through to end_if
	
; type D0
end_if	
	mov	r8, r8, lsr #1	; pop exec status
	b	main_loop
	
; type D2
end_loop_clear
	mov	r8, r4	; restore the old execution status
	cmp	r6, #0	; end of a loop?
	; no
	subgt	r6, r6, #1
	movgt	r12, r7	; load the Dx loop start
	bgt	main_loop
	; yes
	mov	r5, #0	; clear Dx data register
	mov	r8, #0	; clear execution status
	mov	r9, #0	; clear offset
	b	main_loop

; type D1
end_loop_no_clear
	mov	r8, r4	; restore the old execution status
	cmp	r6, #0	; end of a loop?
	; no
	subgt	r6, r6, #1
	movgt	r12, r7	; load the Dx loop start
	; yes
	b	main_loop

dx_conditional_exec
	tst	r8, #0x00000001
	bne	main_loop

	cmp	r0, #0xD6
	bcs	dx_write
	
	cmp	r0, #0xD4
; type D3 - Offset Set
	movlt	r9, r11
	
; type D4 - 
	beq	dx_data_op
	
; type D5 - Dx Data set
	movgt	r5, r11
	b	main_loop
	
dx_write
	cmp	r0, #0xD9
	bcs	dx_load
	
	cmp	r0, #0xD7
; type D6 - Dx Data write 32 bits
	strlt	r5, [r11, r9]
	addlt	r9, r9, #4
; type D7 - Dx Data write 16 bits
	streqh	r5, [r11, r9]
	addeq	r9, r9, #2
; type D8 - Dx Data write 8 bits
	strgtb	r5, [r11, r9]
	addgt	r9, r9, #1
	b	main_loop

dx_load
	cmp	r0, #0xDC
	bcs	dx_offset_add_addr
	
	cmp	r0, #0xDA
; type D9 - Dx Data load 32 bits
	ldrlt	r5, [r11, r9]
; type DA - Dx Data load 16 bits
	ldreqh	r5, [r11, r9]
; type DB - Dx Data load 8 bits
	ldrgtb	r5, [r11, r9]
	b	main_loop
	
; type DC
dx_offset_add_addr
	;ldr	r9, [r11, r9]
	add	r9, r9, r11 ;should be this?
	b	main_loop

; type D4
dx_data_op
	and	r0, r10, #0x000000ff
	cmp	r0, #0x01
; type D4000000 - add, use negative values for sub
	addlt	r5, r5, r11
; type D4000001 - or
	orreq	r5, r5, r11
	ble	main_loop

	cmp	r0, #0x03
; type D4000002 - and
	andlt	r5, r5, r11
; type D4000003 - xor
	eoreq	r5, r5, r11
	ble	main_loop
	
	cmp	r0, #0x05
; type D4000004 - lsl
	movlt	r5, r5, lsl r11
; type D4000005 - lsr
	moveq	r5, r5, lsr r11
	ble	main_loop
	
	cmp	r0, #0x07
; type D4000006 - ror
	movlt	r5, r5, ror r11
; type D4000007 - asr
	moveq	r5, r5, asr r11
	ble	main_loop
	
	cmp	r0, #0x09
; type D4000008 - mul
	mullt	r5, r11, r5

	b	main_loop


;;;;;;;;;;;;;;;;
; type E

patch_code
	; check execution status
	tst	r8, #0x00000001
	addne	r12, r12, r11	; skip the data section
	bne	align_cheat_list
	
	bic	r10, r10, #0xf0000000	; destination address
	add	r10, r10, r9	; add offset to dest
	; r11 is bytes remaining
	; r12 is source

patch_code_word_loop
	cmp	r11, #4
	blt	patch_code_byte_loop
	ldr	r1, [r12], #4
	str	r1, [r10], #4
	sub	r11, r11, #4
	b	patch_code_word_loop
	
patch_code_byte_loop
	cmp	r11, #1
	blt	align_cheat_list	; patch_code_end
	ldrb	r1, [r12], #1
	strb	r1, [r10], #1
	sub	r11, r11, #1
	b	patch_code_byte_loop

align_cheat_list
	add	r12, r12, #0x7
	bic	r12, r12, #0x00000007	; round up to nearest multiple of 8
	b	main_loop


;;;;;;;;;;;;;;;;
; type F

mem_copy_code
	bic	r10, r10, #0xf0000000	; destination address
	; r11 is bytes remaining
	mov	r2, r9	; source address
	
mem_copy_code_word_loop
	cmp	r11, #4
	ble	mem_copy_code_byte_loop
	ldr	r1, [r2], #4
	str	r1, [r10], #4
	sub	r11, r11, #4
	b	mem_copy_code_word_loop
	
mem_copy_code_byte_loop
	cmp	r11, #1
	blt	main_loop	; mem_copy_code_end
	ldrb	r1, [r2], #1
	strb	r1, [r10], #1
	sub	r11, r11, #1
	b	mem_copy_code_byte_loop
	

;;;;;;;;;;;;;;;;

AR_return
	ldmfd	sp!, {r0-r12, lr}
	bx	lr
	;ldr		r12,=0x37FB458
	;bx		r12
	;mov	r12, #0x2800000
	;ldr	pc, [r12, #-0x98] 
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
fix_data
	ALIGN 4	
	LTORG
AR_end	
;--------------------------------			
codeTableStart
	DCD 	 0xCF000000
	DCD 	 0x00000000
	END