    AREA     BOOT ,CODE,READONLY
	CODE32
	EXPORT JmpGBA,Switch2Sytem,bxr0
bxr0	;@r0 ﾌｪｵﾘﾖｷ
	bx  r0
JmpGBA
	mov 	r2,#0x40
	ldr 	r12,=0x11b8
	bx  	r12
Switch2Sytem
	mov		r12,#0x1F
	msr		CPSR_cxsf,r12
	bx		lr	
	END
