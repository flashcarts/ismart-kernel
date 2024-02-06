    
    AREA	globals,CODE,READONLY

	EXPORT	CTF_DrawFont_Fast2_asm
CTF_DrawFont_Fast2_asm

;// void CTF_DrawFont_Fast2_asm(u32 *pcoltbl9,const u16 *pfontdata,u32 fontwidth,u16 *pdstbuf);

REG_pcoltbl9 RN r0
REG_pfontdata RN r1
REG_fontwidth RN r2
REG_pdstbuf RN r3

REG_curbits RN r4
REG_curbitscount RN r5
REG_dy RN r6
REG_dx RN r7
REG_mask_1ff RN r8
REG_stride RN r9

REG_tmp RN lr

ScreenWidth EQU 256
CTF_FontHeight EQU 12
needbitscount EQU 9

;//  bkpt 0 // デバッグ用ブレークポイント設定
  
  push {r4,r5,r6,r7,r8,r9,lr}
  
  mov REG_mask_1ff,#0x100
  orr REG_mask_1ff,#0xff
  
  mov REG_stride,#ScreenWidth*2
  sub REG_stride,REG_stride,REG_fontwidth,lsl #1
  
  ldr REG_curbits,[REG_pfontdata],#4
  mov REG_curbitscount,#32
  
  mov REG_dy,#CTF_FontHeight
df_dy_loopstart
  
  mov REG_dx,REG_fontwidth
df_dx_loopstart
  
df_readbits_start
  cmps REG_curbitscount,#needbitscount
  bhs df_readbits_end
  ldrh REG_tmp,[REG_pfontdata],#2
  orr REG_curbits,REG_curbits,REG_tmp,lsl REG_curbitscount
  add REG_curbitscount,#16
df_readbits_end
  
df_checkskip_start
  and REG_tmp,REG_curbits,#7
  cmps REG_tmp,#7
  bne df_decode_start
  add REG_pdstbuf,#2
  lsr REG_curbits,#3
  sub REG_curbitscount,#3
df_checkskip_end
  
df_dx_loopend_copy
  subs REG_dx,#1
  bne df_dx_loopstart
  
  add REG_pdstbuf,REG_stride
  
df_dy_loopend_copy
  subs REG_dy,#1
  bne df_dy_loopstart
  
df_loopout_copy
  pop {r4,r5,r6,r7,r8,r9,pc}
  
df_decode_start
  and REG_tmp,REG_curbits,REG_mask_1ff
  ldr REG_tmp,[REG_pcoltbl9,REG_tmp,lsl #2]
  lsr REG_curbits,#needbitscount
  sub REG_curbitscount,#needbitscount
  strh REG_tmp,[REG_pdstbuf],#2
df_decode_end

df_dx_loopend
  subs REG_dx,#1
  bne df_dx_loopstart
  
  add REG_pdstbuf,REG_stride
  
df_dy_loopend
  subs REG_dy,#1
  bne df_dy_loopstart
  
df_loopout
  pop {r4,r5,r6,r7,r8,r9,pc}
  
  END
