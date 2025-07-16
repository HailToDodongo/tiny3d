## Auto-generated file, transpiled with RSPL
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_STRIDE (320 * 4)
#define BUFF_SIZE_RGBA16 (320 * 240 * 2)
#define BUFF_SIZE_RGBA32 (320 * 240 * 4)
#define BLUR_WIDTH 80
#define BLUR_HEIGHT 60
#define BLUR_STRIDE (80 * 4)
#define BLUR_STRIDE_PADDED (80 * 4 + 32)
#include <rsp_queue.inc>

.set noreorder
.set noat
.set nomacro

#undef zero
#undef at
#undef v0
#undef v1
#undef a0
#undef a1
#undef a2
#undef a3
#undef t0
#undef t1
#undef t2
#undef t3
#undef t4
#undef t5
#undef t6
#undef t7
#undef s0
#undef s1
#undef s2
#undef s3
#undef s4
#undef s5
#undef s6
#undef s7
#undef t8
#undef t9
#undef k0
#undef k1
#undef gp
#undef sp
#undef fp
#undef ra
.equ hex.$zero, 0
.equ hex.$at, 1
.equ hex.$v0, 2
.equ hex.$v1, 3
.equ hex.$a0, 4
.equ hex.$a1, 5
.equ hex.$a2, 6
.equ hex.$a3, 7
.equ hex.$t0, 8
.equ hex.$t1, 9
.equ hex.$t2, 10
.equ hex.$t3, 11
.equ hex.$t4, 12
.equ hex.$t5, 13
.equ hex.$t6, 14
.equ hex.$t7, 15
.equ hex.$s0, 16
.equ hex.$s1, 17
.equ hex.$s2, 18
.equ hex.$s3, 19
.equ hex.$s4, 20
.equ hex.$s5, 21
.equ hex.$s6, 22
.equ hex.$s7, 23
.equ hex.$t8, 24
.equ hex.$t9, 25
.equ hex.$k0, 26
.equ hex.$k1, 27
.equ hex.$gp, 28
.equ hex.$sp, 29
.equ hex.$fp, 30
.equ hex.$ra, 31
#define vco 0
#define vcc 1
#define vce 2

.data
  RSPQ_BeginOverlayHeader
    RSPQ_DefineCommand Cmd_HDRBloom, 16
    RSPQ_DefineCommand Cmd_Blur, 12
    RSPQ_DefineCommand Cmd_Downscale, 8
  RSPQ_EndOverlayHeader

  RSPQ_EmptySavedState

.bss
  TEMP_STATE_MEM_START:
    .align 2
    BUFF_IN_A: .ds.b 1280
    .align 2
    BUFF_IN_B: .ds.b 1280
    .align 3
    SAFE_SPACE_0: .ds.b 16
    .align 2
    BUFF_BLOOM: .ds.b 320
    .align 4
    SAFE_SPACE_1: .ds.b 16
    .align 4
    SAFE_SPACE_2: .ds.b 16
    .align 2
    BUFF_BLOOM_B: .ds.b 320
    .align 4
    SAFE_SPACE_3: .ds.b 16
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

Cmd_HDRBloom:
  addiu $at, $zero, 31744                            ## L:72   |      ^ | VCONST_RGB.x = 0b0'11111'00000'00000;
  mtc2 $at, $v28.e0                                  ## L:72   |      2 | VCONST_RGB.x = 0b0'11111'00000'00000;
  lui $at, 0x04                                      ## L:74   |      3 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  ori $at, $at, 0xB000                               ## L:74   |      4 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:74   |      5 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addiu $at, $zero, 8192                             ## L:82   |      6 | bloomFactorA:sfract.X = 0.25;
  vxor $v26, $v00, $v00.e0                           ## L:81   |      ^ | vec16 bloomFactorA = 0;
  mtc2 $at, $v26.e4                                  ## L:82   |      7 | bloomFactorA:sfract.X = 0.25;
  ori $s6, $zero, %lo(BUFF_IN_A)                     ## L:75   |      8 | u16 buffDMEM = BUFF_IN_A;
  or $s5, $zero, $zero                               ## L:78   |      9 | u8 bloomPhase = 0;
  srl $s4, $a3, 16                                   ## L:86   |     10 | u16 factorU16 = factor >> 16;
  vxor $v27, $v00, $v00.e0                           ## L:79   |      ^ | vec16 bloomFactorY = 0;
  vmov $v26.e5, $v26.e4                              ## L:83   |     11 | bloomFactorA.Y = bloomFactorA.X;
  vmov $v26.e6, $v26.e4                              ## L:84   |  ***15 | bloomFactorA.Z = bloomFactorA.X;
  LABEL_Cmd_HDRBloom_0001:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:91   |      ^ | RA = get_dma_busy();
  mtc2 $s4, $v26.e3                                  ## L:93   |     16 | bloomFactorA.w = factorU16;
  or $s3, $zero, $zero                               ## L:92   |     17 | sumRGB = 0;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0001            ## L:94   |     18 | bloomFactorA.W = factor:u16;
  mtc2 $a3, $v26.e7                                  ## L:94   |    *20 | bloomFactorA.W = factor:u16;
  LABEL_Cmd_HDRBloom_0003:
  or $s1, $zero, $s6                                 ## L:101  |     21 | u32 ptrDMEM = buffDMEM;
  addiu $s2, $zero, 1279                             ## L:100  |     22 | u32 dmaSize = (320 * 4) - 1;
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:32   |     23 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |     24 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  ori $s0, $zero, %lo(BUFF_BLOOM)                    ## L:105  |     25 | u16 dmemBloom = BUFF_BLOOM;
  bne $s5, $zero, LABEL_Cmd_HDRBloom_0005            ## L:110  |     26 | if(bloomPhase == 0) {
  mtc0 $s2, COP0_DMA_READ                            ## L:34   |    *28 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0006:
  addiu $s5, $zero, 4                                ## L:114  |     29 | bloomPhase = 4;
  mfc0 $ra, COP0_DMA_BUSY                            ## L:113  |     30 | RA = get_dma_busy();
  addiu $fp, $zero, 640                              ## L:116  |     31 | bloomDmaSize = (80 * 4)*2;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0006            ## L:116  |    *33 | bloomDmaSize = (80 * 4)*2;
  vsubc $v27, $v00, $v31.e2                          ## L:115  |    *35 | bloomFactorY = VZERO - 0x2000;
  LABEL_Cmd_HDRBloom_0007:
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:32   |     36 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR                         ## L:33   |     37 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a2, $a2, 320                                ## L:120  |     38 | ptrBloom += (80 * 4);
  mtc0 $fp, COP0_DMA_READ                            ## L:34   |    *40 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0005:
  addiu $a0, $a0, 1280                               ## L:124  |     41 | ptrIn += (320 * 4);
  vaddc $v27, $v27, $v31.e2                          ## L:129  |      ^ | bloomFactorY += 0x2000;
  addiu $s5, $s5, 65535                              ## L:123  |     42 | bloomPhase -= 1;
  LABEL_Cmd_HDRBloom_0008:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:132  |     43 | RA = get_dma_busy();
  addiu $k1, $s1, 1280                               ## L:135  |     44 | dmemInEnd = ptrDMEM + 320*4;
  or $fp, $zero, $s1                                 ## L:134  |     45 | dmemIn = ptrDMEM;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0008            ## L:135  |     46 | dmemInEnd = ptrDMEM + 320*4;
  ori $sp, $zero, %lo(BUFF_IN_B)                     ## L:133  |    *48 | dmemOut = BUFF_IN_B;
  LABEL_Cmd_HDRBloom_0009:
  luv $v21, 0, 320, $s0                              ## L:149  |     49 | bloom0Next = load_vec_u8(dmemBloom, 320);
  luv $v25, 0, 0, $s0                                ## L:148  |     50 | bloom0 = load_vec_u8(dmemBloom, 0);
  luv $v20, 0, 328, $s0                              ## L:152  |     51 | bloom2Next = load_vec_u8(dmemBloom, 328);
  luv $v23, 0, 8, $s0                                ## L:151  |     52 | bloom2 = load_vec_u8(dmemBloom, 8);
  luv $v13, 0, 0, $fp                                ## L:159  |     53 | vec16 pixel0 = load_vec_u8(dmemIn, 0);
  ori $k0, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:141  |     54 | u16 tmpMemA = RSPQ_SCRATCH_MEM;
  vsubc $v15, $v21, $v25.v                           ## L:156  |      ^ | vec16 diffY_A = bloom0Next - bloom0;
  addiu $s0, $s0, 8                                  ## L:154  |    *56 | dmemBloom += 8;
  vsubc $v14, $v20, $v23.v                           ## L:157  |      ^ | vec16 diffY_B = bloom2Next - bloom2;
  addiu $t8, $k0, 2                                  ## L:143  |     57 | u16 tmpMemC = tmpMemA + 2;
  vmudm $v29, $v13, $v26.e7                          ## L:55   |      ^ | asm_op("vmudm", VTEMP, pixel, bloomFactorA.W);
  addiu $t9, $k0, 1                                  ## L:142  |     58 | u16 tmpMemB = tmpMemA + 1;
  vmadh $v13, $v13, $v26.e3                          ## L:56   |      ^ | asm_op("vmadh", pixel, pixel, bloomFactorA.w);
  LABEL_Cmd_HDRBloom_000A:
  vmudn $v19, $v18, $v30.e6                          ## L:170  |     59 | pixelOut = pixelR:uint * 2;
  lbu $t5, 10($k0)                                   ## L:235  |      ^ | tmp2 = load(tmpMemA, 10);
  vmadl $v19, $v17, $v31.e3                          ## L:171  |     60 | asm_op("vmadl", pixelOut, pixelG, VSHIFT8.w);
  luv $v11, 0, 16, $fp                               ## L:165  |      ^ | vec16 pixel2 = load_vec_u8(dmemIn, 16);
  vmadl $v19, $v16, $v30.e0                          ## L:172  |     61 | asm_op("vmadl", pixelOut, pixelB, VSHIFT.x);
  luv $v10, 0, 24, $fp                               ## L:166  |      ^ | vec16 pixel3 = load_vec_u8(dmemIn, 24);
  vmudh $v25, $v25, $v30.e7                          ## L:175  |     62 | bloom0:sint = bloom0:sint * 1;
  vmacf $v25, $v15, $v27.v                           ## L:176  |     63 | bloom0:sfract = diffY_A:sfract +* bloomFactorY:sfract;
  luv $v12, 0, 8, $fp                                ## L:164  |      ^ | vec16 pixel1 = load_vec_u8(dmemIn, 8);
  lbu $t7, 8($k0)                                    ## L:233  |     64 | tmp0 = load(tmpMemA, 8);
  vmudh $v23, $v23, $v30.e7                          ## L:181  |      ^ | bloom2:sint = bloom2:sint * 1;
  vmacf $v23, $v14, $v27.v                           ## L:182  |     65 | bloom2:sfract = diffY_B:sfract +* bloomFactorY:sfract;
  lbu $t6, 9($k0)                                    ## L:234  |      ^ | tmp1 = load(tmpMemA, 9);
  vmudm $v29, $v12, $v26.e7                          ## L:55   |    *67 | asm_op("vmudm", VTEMP, pixel, bloomFactorA.W);
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:190  |      ^ | @Barrier("tmp_0") store(bloom0, RSPQ_SCRATCH_MEM, 0); ## Barrier: 0x2
  sqv $v25, 0, 0, $at                                ## L:190  |     68 | @Barrier("tmp_0") store(bloom0, RSPQ_SCRATCH_MEM, 0); ## Barrier: 0x2
  vmadh $v12, $v12, $v26.e3                          ## L:56   |      ^ | asm_op("vmadh", pixel, pixel, bloomFactorA.w);
  sdv $v23, 0, 24, $at                               ## L:191  |     69 | @Barrier("tmp_2") store(bloom2.xyzw, RSPQ_SCRATCH_MEM, 24); ## Barrier: 0x4
  vor $v20, $v00, $v23                               ## L:193  |      ^ | @Barrier("tmp_2") bloom2Next = bloom2; ## Barrier: 0x4
  vor $v23, $v00, $v25                               ## L:194  |     70 | @Barrier("tmp_0") bloom2 = bloom0; ## Barrier: 0x2
  ldv $v25, 8, 0, $at                                ## L:196  |      ^ | @Barrier("tmp_0") bloom0.XYZW = load(RSPQ_SCRATCH_MEM, 0).xyzw; ## Barrier: 0x2
  ldv $v23, 0, 8, $at                                ## L:197  |     71 | @Barrier("tmp_0") bloom2.xyzw = load(RSPQ_SCRATCH_MEM, 8).xyzw; ## Barrier: 0x2
  vmudm $v29, $v11, $v26.e7                          ## L:55   |      ^ | asm_op("vmudm", VTEMP, pixel, bloomFactorA.W);
  vmadh $v11, $v11, $v26.e3                          ## L:56   |   **74 | asm_op("vmadh", pixel, pixel, bloomFactorA.w);
  sqv $v19, 0, -16, $sp                              ## L:179  |      ^ | store(pixelOut, dmemOut, -16);
  vsubc $v09, $v23, $v25.v                           ## L:205  |     75 | vec16 diffX_A = bloom2 - bloom0;
  vmudm $v29, $v10, $v26.e7                          ## L:55   |     76 | asm_op("vmudm", VTEMP, pixel, bloomFactorA.W);
  ldv $v20, 8, 24, $at                               ## L:198  |      ^ | @Barrier("tmp_2") bloom2Next.XYZW = load(RSPQ_SCRATCH_MEM, 24).xyzw; ## Barrier: 0x4
  lbu $t3, 25($k0)                                   ## L:237  |     77 | tmp4 = load(tmpMemA, 25);
  vmadh $v10, $v10, $v26.e3                          ## L:56   |      ^ | asm_op("vmadh", pixel, pixel, bloomFactorA.w);
  vmudh $v24, $v25, $v30.e7                          ## L:212  |     78 | bloom1:sint = bloom0:sint * 1;
  luv $v25, 0, 0, $s0                                ## L:273  |      ^ | bloom0 = load_vec_u8(dmemBloom, 0);
  addu $t1, $t7, $t6                                 ## L:239  |     79 | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  vmacf $v08, $v09, $v26.v                           ## L:213  |      ^ | diffX_A_first:sfract = diffX_A:sfract +* bloomFactorA:sfract;
  vmacf $v24, $v09, $v31.e1                          ## L:214  |     80 | bloom1:sfract = diffX_A:sfract +* 0x4000;
  addiu $sp, $sp, 16                                 ## L:283  |      ^ | dmemOut += 16;
  vmadh $v12, $v12, $v30.e7                          ## L:215  |     81 | pixel1:sint = pixel1:sint +* 1;
  vsubc $v06, $v20, $v23.v                           ## L:217  |     82 | vec16 diffX_B = bloom2Next - bloom2;
  lbu $t4, 24($k0)                                   ## L:236  |      ^ | tmp3 = load(tmpMemA, 24);
  luv $v20, 0, 328, $s0                              ## L:275  |     83 | bloom2Next = load_vec_u8(dmemBloom, 328);
  vadd $v13, $v08, $v13.v                            ## L:220  |      ^ | pixel0:sint = diffX_A_first:sint + pixel0:sint;
  vmudh $v22, $v23, $v30.e7                          ## L:222  |     84 | bloom3:sint = bloom2:sint * 1;
  lbu $t2, 26($k0)                                   ## L:238  |      ^ | tmp5 = load(tmpMemA, 26);
  vmacf $v07, $v06, $v26.v                           ## L:223  |    *86 | diffX_B_first:sfract = diffX_B:sfract +* bloomFactorA:sfract;
  addu $t1, $t1, $t5                                 ## L:239  |      ^ | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  addu $t1, $t1, $t4                                 ## L:239  |     87 | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  vmacf $v22, $v06, $v31.e1                          ## L:224  |      ^ | bloom3:sfract = diffX_B:sfract +* 0x4000;
  suv $v13, 0, 0, $k0                                ## L:243  |     88 | @Barrier("rgba16") store_vec_u8(pixel0, tmpMemA, 0); ## Barrier: 0x8
  suv $v12, 0, 8, $k0                                ## L:244  |     89 | @Barrier("rgba16") store_vec_u8(pixel1, tmpMemA, 8); ## Barrier: 0x8
  vmadh $v10, $v10, $v30.e7                          ## L:225  |      ^ | pixel3:sint = pixel3:sint +* 1;
  vadd $v11, $v07, $v11.v                            ## L:227  |     90 | pixel2:sint = diffX_B_first:sint + pixel2:sint;
  addu $t1, $t1, $t3                                 ## L:239  |      ^ | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  luv $v13, 0, 32, $fp                               ## L:248  |     91 | pixel0 = load_vec_u8(dmemIn, 32);
  addu $t1, $t1, $t2                                 ## L:239  |     92 | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  addu $s3, $s3, $t1                                 ## L:239  |     93 | sumRGB += tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
  suv $v11, 0, 16, $k0                               ## L:245  |     94 | @Barrier("rgba16") store_vec_u8(pixel2, tmpMemA, 16); ## Barrier: 0x8
  suv $v10, 0, 24, $k0                               ## L:246  |     95 | @Barrier("rgba16") store_vec_u8(pixel3, tmpMemA, 24); ## Barrier: 0x8
  lfv $v18, 0, 0, $k0                                ## L:255  |     96 | asm_op("lfv", pixelR, 0,  0, tmpMemA); ## Barrier: 0x8
  lfv $v18, 8, 16, $k0                               ## L:257  |     97 | asm_op("lfv", pixelR, 8, 16, tmpMemA); ## Barrier: 0x8
  lfv $v17, 0, 0, $t9                                ## L:261  |     98 | asm_op("lfv", pixelG, 0,  0, tmpMemB); ## Barrier: 0x8
  luv $v23, 0, 8, $s0                                ## L:274  |     99 | bloom2 = load_vec_u8(dmemBloom, 8);
  vmudm $v29, $v13, $v26.e7                          ## L:55   |      ^ | asm_op("vmudm", VTEMP, pixel, bloomFactorA.W);
  vmadh $v13, $v13, $v26.e3                          ## L:56   |    100 | asm_op("vmadh", pixel, pixel, bloomFactorA.w);
  lfv $v17, 8, 16, $t9                               ## L:263  |      ^ | asm_op("lfv", pixelG, 8, 16, tmpMemB); ## Barrier: 0x8
  addiu $fp, $fp, 24                                 ## L:282  |    101 | dmemIn += 24;
  addiu $s0, $s0, 8                                  ## L:277  |    102 | dmemBloom += 8;
  lfv $v16, 0, 0, $t8                                ## L:267  |    103 | asm_op("lfv", pixelB, 0,  0, tmpMemC); ## Barrier: 0x8
  vand $v18, $v18, $v28.e0                           ## L:258  |      ^ | pixelR &= VCONST_RGB.x;
  vand $v17, $v17, $v28.e0                           ## L:264  |    104 | pixelG &= VCONST_RGB.x;
  lfv $v16, 8, 16, $t8                               ## L:269  |      ^ | asm_op("lfv", pixelB, 8, 16, tmpMemC); ## Barrier: 0x8
  vor $v15, $v00, $v14                               ## L:279  |    105 | diffY_A = diffY_B;
  bne $fp, $k1, LABEL_Cmd_HDRBloom_000A              ## L:283  |      ^ | dmemOut += 16;
  vsubc $v14, $v20, $v23.v                           ## L:280  |   *107 | diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_000B:
  vmudn $v19, $v18, $v30.e6                          ## L:287  |    108 | pixelOut = pixelR:uint * 2;
  addiu $s1, $sp, 64896                              ## L:295  |      ^ | ptrDMEM = dmemOut - 320 * 2;
  vmadl $v19, $v17, $v31.e3                          ## L:288  |    109 | asm_op("vmadl", pixelOut, pixelG, VSHIFT8.w);
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:40   |      ^ | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  vmadl $v19, $v16, $v30.e0                          ## L:289  |    110 | asm_op("vmadl", pixelOut, pixelB, VSHIFT.x);
  slv $v00, 0, -8, $sp                               ## L:292  |      ^ | store(VZERO.xy, dmemOut, -8);
  addiu $t7, $zero, 639                              ## L:294  |    111 | u32 dmaSize = (320 * 2) - 1;
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:41   |    112 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $t7, COP0_DMA_WRITE                           ## L:42   |    113 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  addiu $a1, $a1, 640                                ## L:298  |    114 | ptrOut += (320 * 2);
  bne $a0, $s7, LABEL_Cmd_HDRBloom_0003              ## L:298  |    115 | ptrOut += (320 * 2);
  sdv $v19, 0, -16, $sp                              ## L:291  |   *117 | store(pixelOut.xyzw, dmemOut, -16);
  LABEL_Cmd_HDRBloom_0004:
  sw $s3, 0($s6)                                     ## L:302  |    118 | store(sumRGB, buffDMEM, 0);
  mtc0 $s6, COP0_DMA_SPADDR                          ## L:40   |    119 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:41   |    120 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $zero, 7                                ## L:303  |    121 | u16 dmaSize = 8 - 1;
  j RSPQ_Loop                                        ## L:43   |    122 | }
  mtc0 $s2, COP0_DMA_WRITE                           ## L:42   |   *124 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
Cmd_Blur:
  or $s4, $zero, $zero                               ## L:374  |      ^ | u16 dmemInOffset = 0;
  mtc2 $a2, $v29.e0                                  ## L:377  |      2 | VTEMP.x = thresholdBrightness;
  srl $a2, $a2, 16                                   ## L:378  |      3 | thresholdBrightness >>= 16;
  addiu $s3, $zero, 1056                             ## L:375  |      4 | u16 dmemInOffsetEnd = (80 * 4 + 32) * 3;
  addiu $s6, $zero, 60                               ## L:371  |      5 | u32 rowsLeft = 60;
  addiu $s5, $zero, 319                              ## L:373  |      6 | u16 dmaSize = (80 * 4) - 1;
  addiu $s7, $a0, 18880                              ## L:370  |      7 | u32 ptrRDRAMInEnd = ptrRDRAMIn + ((80 * 4) * (60 - 1));
  mtc2 $a2, $v29.e1                                  ## L:379  |      8 | VTEMP.y = thresholdBrightness;
  LABEL_Cmd_Blur_000C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:386  |      9 | RA = get_dma_busy();
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:387  |     10 | dmemInA = BUFF_IN_A;
  addiu $s1, $s2, 1072                               ## L:388  |     11 | dmemOutStart = dmemInA + ((80 * 4 + 32)*3) + 16;
  bne $ra, $zero, LABEL_Cmd_Blur_000C                ## L:389  |     12 | lastLoadedDmemIn = dmemInA;
  or $s0, $zero, $s2                                 ## L:389  |    *14 | lastLoadedDmemIn = dmemInA;
  LABEL_Cmd_Blur_000D:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:32   |     15 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |     16 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:395  |     17 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:34   |    *19 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000E:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:48   |     20 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000E                ## L:48   |   **23 | RA = get_dma_busy();
  nop                                                ## L:48   |    *25 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000F:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:32   |     26 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |     27 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:399  |     28 | dmemInA += (80 * 4 + 32);
  addiu $a0, $a0, 320                                ## L:400  |     29 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:34   |     30 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0010:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:48   |     31 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0010                ## L:48   |   **34 | RA = get_dma_busy();
  nop                                                ## L:48   |    *36 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0011:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:32   |     37 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |     38 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:404  |     39 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:34   |    *41 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0012:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:48   |     42 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0012                ## L:48   |   **45 | RA = get_dma_busy();
  nop                                                ## L:48   |    *47 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0013:
  lw $fp, 316($s2)                                   ## L:408  |     48 | u32 tmp1 = load(dmemInA, 316);
  vxor $v27, $v00, $v00.e0                           ## L:423  |      ^ | vec16 maskG = 0; maskG.y = 1;
  vxor $v26, $v00, $v00.e0                           ## L:424  |     49 | vec16 maskB = 0; maskB.z = 1;
  vxor $v28, $v00, $v00.e0                           ## L:422  |     50 | vec16 maskR = 0; maskR.x = 1;
  vmov $v28.e0, $v30.e7                              ## L:422  |     51 | vec16 maskR = 0; maskR.x = 1;
  vmov $v26.e2, $v30.e7                              ## L:424  |     52 | vec16 maskB = 0; maskB.z = 1;
  sw $fp, 320($s2)                                   ## L:409  |      ^ | store(tmp1, dmemInA, 320);
  addiu $s2, $s2, 65184                              ## L:411  |     53 | dmemInA -= (80 * 4 + 32);
  vxor $v25, $v00, $v00.e0                           ## L:426  |      ^ | vec16 maskR1 = 0; maskR1.X = 1;
  vmov $v27.e1, $v30.e7                              ## L:423  |     54 | vec16 maskG = 0; maskG.y = 1;
  lw $sp, 316($s2)                                   ## L:412  |      ^ | u32 tmp2 = load(dmemInA, 316);
  vxor $v23, $v00, $v00.e0                           ## L:428  |     55 | vec16 maskB1 = 0; maskB1.Z = 1;
  vxor $v24, $v00, $v00.e0                           ## L:427  |     56 | vec16 maskG1 = 0; maskG1.Y = 1;
  addiu $fp, $zero, 960                              ## L:420  |      ^ | u16 thresholdStride = (80 * 4) * 3;
  vmov $v25.e4, $v30.e7                              ## L:426  |     57 | vec16 maskR1 = 0; maskR1.X = 1;
  sw $sp, 320($s2)                                   ## L:413  |      ^ | store(tmp2, dmemInA, 320);
  ori $sp, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:430  |     58 | u16 temp = RSPQ_SCRATCH_MEM;
  vmov $v24.e5, $v30.e7                              ## L:427  |      ^ | vec16 maskG1 = 0; maskG1.Y = 1;
  vmov $v23.e6, $v30.e7                              ## L:428  |     59 | vec16 maskB1 = 0; maskB1.Z = 1;
  LABEL_Cmd_Blur_0016:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:437  |      ^ | RA = get_dma_busy();
  addiu $t9, $zero, 352                              ## L:442  |     60 | dmemInBOffset = (80 * 4 + 32);
  or $k1, $zero, $s1                                 ## L:441  |     61 | dmemOutCurr = dmemOutStart;
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:438  |     62 | dmemInA = BUFF_IN_A;
  bne $ra, $zero, LABEL_Cmd_Blur_0016                ## L:442  |     63 | dmemInBOffset = (80 * 4 + 32);
  addiu $s2, $s2, 4                                  ## L:439  |    *65 | dmemInA += 4;
  LABEL_Cmd_Blur_0017:
  vxor $v22, $v00, $v00.e0                           ## L:448  |     66 | sumA = 0;
  beq $a2, $zero, LABEL_Cmd_Blur_0018                ## L:450  |      ^ | if(thresholdBrightness)
  addiu $t8, $s2, 704                                ## L:445  |    *68 | u16 dmemInC = dmemInA + (80 * 4 + 32)*2;
  or $t7, $zero, $s0                                 ## L:453  |     69 | u16 ptrClamp = lastLoadedDmemIn;
  LABEL_Cmd_Blur_0019:
  luv $v18, 0, 0, $t7                                ## L:460  |     70 | vec16 p0 = load_vec_u8(ptrClamp, 0);
  luv $v17, 0, 8, $t7                                ## L:461  |     71 | vec16 p1 = load_vec_u8(ptrClamp, 8);
  luv $v09, 0, 72, $t7                               ## L:469  |     72 | vec16 p9 = load_vec_u8(ptrClamp, 72);
  luv $v14, 0, 32, $t7                               ## L:464  |    *74 | vec16 p4 = load_vec_u8(ptrClamp, 32);
  vlt $v19, $v18, $v29.e1                            ## L:358  |      ^ | tmp = pixel < VTEMP.y;
  luv $v16, 0, 16, $t7                               ## L:462  |     75 | vec16 p2 = load_vec_u8(ptrClamp, 16);
  vmrg $v18, $v00, $v18                              ## L:359  |      ^ | pixel = select(VZERO, pixel);
  luv $v12, 0, 48, $t7                               ## L:466  |     76 | vec16 p6 = load_vec_u8(ptrClamp, 48);
  vlt $v19, $v17, $v29.e1                            ## L:358  |      ^ | tmp = pixel < VTEMP.y;
  vmrg $v17, $v00, $v17                              ## L:359  |     77 | pixel = select(VZERO, pixel);
  luv $v15, 0, 24, $t7                               ## L:463  |      ^ | vec16 p3 = load_vec_u8(ptrClamp, 24);
  vlt $v19, $v16, $v29.e1                            ## L:358  |    *79 | tmp = pixel < VTEMP.y;
  luv $v11, 0, 56, $t7                               ## L:467  |      ^ | vec16 p7 = load_vec_u8(ptrClamp, 56);
  vmrg $v16, $v00, $v16                              ## L:359  |     80 | pixel = select(VZERO, pixel);
  vlt $v19, $v15, $v29.e1                            ## L:358  |     81 | tmp = pixel < VTEMP.y;
  luv $v13, 0, 40, $t7                               ## L:465  |      ^ | vec16 p5 = load_vec_u8(ptrClamp, 40);
  vmrg $v15, $v00, $v15                              ## L:359  |     82 | pixel = select(VZERO, pixel);
  vlt $v19, $v14, $v29.e1                            ## L:358  |     83 | tmp = pixel < VTEMP.y;
  vmrg $v14, $v00, $v14                              ## L:359  |     84 | pixel = select(VZERO, pixel);
  luv $v10, 0, 64, $t7                               ## L:468  |      ^ | vec16 p8 = load_vec_u8(ptrClamp, 64);
  vlt $v19, $v13, $v29.e1                            ## L:358  |     85 | tmp = pixel < VTEMP.y;
  addiu $fp, $zero, 320                              ## L:457  |      ^ | thresholdStride = (80 * 4);
  vmrg $v13, $v00, $v13                              ## L:359  |     86 | pixel = select(VZERO, pixel);
  vlt $v19, $v12, $v29.e1                            ## L:358  |     87 | tmp = pixel < VTEMP.y;
  suv $v16, 0, 16, $t7                               ## L:484  |      ^ | store_vec_u8(p2, ptrClamp, 16);
  suv $v14, 0, 32, $t7                               ## L:486  |     88 | store_vec_u8(p4, ptrClamp, 32);
  vmrg $v12, $v00, $v12                              ## L:359  |      ^ | pixel = select(VZERO, pixel);
  vlt $v19, $v11, $v29.e1                            ## L:358  |     89 | tmp = pixel < VTEMP.y;
  suv $v15, 0, 24, $t7                               ## L:485  |      ^ | store_vec_u8(p3, ptrClamp, 24);
  suv $v18, 0, 0, $t7                                ## L:482  |     90 | store_vec_u8(p0, ptrClamp, 0);
  vmrg $v11, $v00, $v11                              ## L:359  |      ^ | pixel = select(VZERO, pixel);
  vlt $v19, $v10, $v29.e1                            ## L:358  |     91 | tmp = pixel < VTEMP.y;
  vmrg $v10, $v00, $v10                              ## L:359  |     92 | pixel = select(VZERO, pixel);
  addu $t6, $s0, $fp                                 ## L:458  |      ^ | ptrClampEnd = lastLoadedDmemIn + thresholdStride;
  suv $v13, 0, 40, $t7                               ## L:487  |     93 | store_vec_u8(p5, ptrClamp, 40);
  vlt $v19, $v09, $v29.e1                            ## L:358  |      ^ | tmp = pixel < VTEMP.y;
  suv $v12, 0, 48, $t7                               ## L:488  |     94 | store_vec_u8(p6, ptrClamp, 48);
  vmrg $v09, $v00, $v09                              ## L:359  |      ^ | pixel = select(VZERO, pixel);
  suv $v17, 0, 8, $t7                                ## L:483  |     95 | store_vec_u8(p1, ptrClamp, 8);
  suv $v11, 0, 56, $t7                               ## L:489  |     96 | store_vec_u8(p7, ptrClamp, 56);
  suv $v10, 0, 64, $t7                               ## L:490  |     97 | store_vec_u8(p8, ptrClamp, 64);
  suv $v09, 0, 72, $t7                               ## L:491  |     98 | store_vec_u8(p9, ptrClamp, 72);
  addiu $t7, $t7, 80                                 ## L:493  |     99 | ptrClamp += 80;
  bne $t7, $t6, LABEL_Cmd_Blur_0019                  ## L:493  |    100 | ptrClamp += 80;
  nop                                                ## L:493  |   *102 | ptrClamp += 80;
  LABEL_Cmd_Blur_0018:
  luv $v11, 0, 0, $t8                                ## L:516  |    103 | vec16 pixel_y2_x0 = load_vec_u8(dmemInC, 0);
  luv $v12, 0, 360, $s2                              ## L:514  |    104 | vec16 pixel_y1_x2 = load_vec_u8(dmemInA, 360);
  lw $t7, 316($s0)                                   ## L:499  |    105 | u32 tmp1 = load(lastLoadedDmemIn, 316);
  luv $v14, 0, 8, $s2                                ## L:511  |    106 | vec16 pixel_y0_x2  = load_vec_u8(dmemInA, 8);
  addiu $a1, $a1, 320                                ## L:505  |    107 | ptrRDRAMOut += (80 * 4);
  luv $v15, 0, 0, $s2                                ## L:510  |    108 | vec16 pixel_y0_x0  = load_vec_u8(dmemInA, 0);
  sw $t7, 320($s0)                                   ## L:500  |    109 | store(tmp1, lastLoadedDmemIn, 320);
  luv $v13, 0, 352, $s2                              ## L:513  |    110 | vec16 pixel_y1_x0 = load_vec_u8(dmemInA, 352);
  luv $v10, 0, 8, $t8                                ## L:517  |    111 | vec16 pixel_y2_x2 = load_vec_u8(dmemInC, 8);
  addiu $k0, $s1, 320                                ## L:506  |    112 | dmemOutEnd = dmemOutStart + (80 * 4);
  LABEL_Cmd_Blur_001B:
  addiu $s2, $s2, 16                                 ## L:560  |    113 | dmemInA += 16;
  vmulu $v20, $v14, $v29.e0                          ## L:527  |      ^ | sumC:ufract = pixel_y0_x2:ufract  * VTEMP:ufract.x;
  vmacu $v20, $v12, $v29.e0                          ## L:528  |    114 | sumC:ufract = pixel_y1_x2:ufract +* VTEMP:ufract.x;
  addiu $t8, $t8, 16                                 ## L:561  |      ^ | dmemInC += 16;
  vmacu $v20, $v10, $v29.e0                          ## L:529  |    115 | sumC:ufract = pixel_y2_x2:ufract +* VTEMP:ufract.x;
  suv $v16, 0, -8, $k1                               ## L:521  |      ^ | store_vec_u8(res1, dmemOutCurr, -8);
  suv $v17, 0, -16, $k1                              ## L:520  |    116 | store_vec_u8(res0, dmemOutCurr, -16);
  vmulu $v21, $v15, $v29.e0                          ## L:534  |      ^ | sumB:ufract = pixel_y0_x0:ufract  * VTEMP:ufract.x;
  luv $v14, 0, 8, $s2                                ## L:565  |    117 | pixel_y0_x2 = load_vec_u8(dmemInA, 8);
  vmacu $v21, $v13, $v29.e0                          ## L:535  |      ^ | sumB:ufract = pixel_y1_x0:ufract +* VTEMP:ufract.x;
  vmacu $v21, $v11, $v29.e0                          ## L:536  |    118 | sumB:ufract = pixel_y2_x0:ufract +* VTEMP:ufract.x;
  luv $v11, 0, 0, $t8                                ## L:570  |      ^ | pixel_y2_x0 = load_vec_u8(dmemInC, 0);
  vmadh $v17, $v28, $v22.e4                          ## L:543  |    119 | res0:sint = maskR:sint +* sumA:sint.X;
  addiu $k1, $k1, 16                                 ## L:573  |      ^ | dmemOutCurr += 16;
  vmadh $v17, $v27, $v22.e5                          ## L:544  |    120 | res0:sint = maskG:sint +* sumA:sint.Y;
  luv $v13, 0, 352, $s2                              ## L:567  |      ^ | pixel_y1_x0 = load_vec_u8(dmemInA, 352);
  sdv $v20, 0, 8, $sp                                ## L:531  |    121 | @Barrier("group2") store(sumC.xyzw, temp, 8); ## Barrier: 0x2
  vmadh $v17, $v26, $v22.e6                          ## L:545  |      ^ | res0:sint = maskB:sint +* sumA:sint.Z;
  vmadh $v17, $v25, $v21.e0                          ## L:547  |    122 | res0:sint = maskR1:sint +* sumB:sint.x;
  luv $v15, 0, 0, $s2                                ## L:564  |      ^ | pixel_y0_x0 = load_vec_u8(dmemInA, 0);
  sdv $v21, 8, 0, $sp                                ## L:538  |    123 | @Barrier("group2") store(sumB.XYZW, temp, 0); ## Barrier: 0x2
  vmadh $v17, $v24, $v21.e1                          ## L:548  |      ^ | res0:sint = maskG1:sint +* sumB:sint.y;
  lqv $v09, 0, 0, $sp                                ## L:554  |    124 | @Barrier("group2") group2 = load(temp, 0); ## Barrier: 0x2
  vmadh $v17, $v23, $v21.e2                          ## L:549  |      ^ | res0:sint = maskB1:sint +* sumB:sint.z;
  luv $v12, 0, 360, $s2                              ## L:568  |    125 | pixel_y1_x2 = load_vec_u8(dmemInA, 360);
  vmadh $v17, $v22, $v30.e7                          ## L:551  |      ^ | res0:sint = sumA:sint +* 1;
  luv $v10, 0, 8, $t8                                ## L:571  |    126 | pixel_y2_x2 = load_vec_u8(dmemInC, 8);
  vmudh $v22, $v20, $v30.e7                          ## L:556  |      ^ | sumA:sint = sumC:sint * 1;
  vmadh $v16, $v21, $v30.e7                          ## L:557  |    127 | res1:sint = sumB:sint +* 1;
  bne $k1, $k0, LABEL_Cmd_Blur_001B                  ## L:573  |      ^ | dmemOutCurr += 16;
  vmadh $v16, $v09, $v30.e7                          ## L:558  |   *129 | res1:sint = group2:sint +* 1;
  LABEL_Cmd_Blur_001C:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:40   |    130 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:41   |    131 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s6, $s6, 65535                              ## L:581  |    132 | rowsLeft -= 1;
  suv $v17, 0, -16, $k1                              ## L:576  |   *134 | store_vec_u8(res0, dmemOutCurr, -16);
  suv $v16, 0, -8, $k1                               ## L:577  |    135 | store_vec_u8(res1, dmemOutCurr, -8);
  mtc0 $s5, COP0_DMA_WRITE                           ## L:42   |    136 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_001D:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:584  |    137 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_001D                ## L:585  |  **140 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  addiu $s0, $s4, %lo(BUFF_IN_A)                     ## L:585  |   *142 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  LABEL_Cmd_Blur_001E:
  beq $s6, $zero, RSPQ_Loop                          ## L:588  |    143 | if(rowsLeft == 0)exit;
  nop                                                ## L:588  |   *145 | if(rowsLeft == 0)exit;
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:32   |    146 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |    147 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $s4, 352                                ## L:592  |    148 | dmemInOffset += (80 * 4 + 32);
  beq $a0, $s7, LABEL_Cmd_Blur_0020                  ## L:594  |    149 | if(ptrRDRAMIn != ptrRDRAMInEnd) {
  mtc0 $s5, COP0_DMA_READ                            ## L:34   |   *151 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:595  |    152 | ptrRDRAMIn += (80 * 4);
  LABEL_Cmd_Blur_0020:
  bne $s4, $s3, LABEL_Cmd_Blur_0016                  ## L:598  |    153 | if(dmemInOffset == dmemInOffsetEnd) {
  nop                                                ## L:598  |   *155 | if(dmemInOffset == dmemInOffsetEnd) {
  or $s4, $zero, $zero                               ## L:599  |    156 | dmemInOffset = 0;
  LABEL_Cmd_Blur_0021:
  j LABEL_Cmd_Blur_0016                              ## L:599  |    157 | dmemInOffset = 0;
  nop                                                ## L:599  |   *159 | dmemInOffset = 0;
Cmd_Downscale:
  addiu $a1, $a1, 65216                              ## L:541  |      ^ | prtRDRAMOut -= (80 * 4);
  lui $at, 0x04                                      ## L:539  |      2 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  ori $s4, $zero, %lo(BUFF_BLOOM)                    ## L:553  |      3 | u16 dmemOutA = BUFF_BLOOM;
  ori $at, $at, 0xB000                               ## L:539  |      4 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  lui $s6, 0x5000                                    ## L:543  |      5 | u16 dmaSizeIn = (
  addiu $s5, $zero, 319                              ## L:548  |      6 | u16 dmaSizeOut = (80 * 4) - 1;
  ori $s6, $s6, 0x14FF                               ## L:543  |      7 | u16 dmaSizeIn = (
  ori $s3, $zero, %lo(BUFF_BLOOM_B)                  ## L:554  |      8 | u16 dmemOutB = BUFF_BLOOM_B;
  addu $s7, $a0, $at                                 ## L:539  |      9 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addiu $at, $zero, 4096                             ## L:551  |     10 | sumFactor:sfract.x = 0.125;
  mtc2 $at, $v28.e0                                  ## L:551  |     11 | sumFactor:sfract.x = 0.125;
  LABEL_Cmd_Downscale_0024:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:562  |     12 | RA = get_dma_busy();
  ori $s1, $zero, %lo(BUFF_IN_A)                     ## L:563  |     13 | dmemInA = BUFF_IN_A;
  bne $ra, $zero, LABEL_Cmd_Downscale_0024           ## L:564  |    *15 | dmemInB = BUFF_IN_B;
  ori $s0, $zero, %lo(BUFF_IN_B)                     ## L:564  |    *17 | dmemInB = BUFF_IN_B;
  LABEL_Cmd_Downscale_0025:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:30   |     18 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     19 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 5120                               ## L:570  |     20 | ptrRDRAMIn += (320 * 4) * 4;
  mtc0 $s6, COP0_DMA_READ                            ## L:32   |    *22 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Downscale_0026:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:573  |     23 | RA = get_dma_busy();
  or $s2, $zero, $s4                                 ## L:575  |     24 | dmemOut = dmemOutA;
  bne $ra, $zero, LABEL_Cmd_Downscale_0026           ## L:576  |    *26 | dmemOutEnd = dmemOut + (80 * 4);
  addiu $fp, $s2, 320                                ## L:576  |    *28 | dmemOutEnd = dmemOut + (80 * 4);
  LABEL_Cmd_Downscale_0027:
  addiu $s1, $s1, 1280                               ## L:584  |     29 | dmemInA += 1280;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     30 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     31 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  lqv $v27, 0, -32, $s1                              ## L:587  |     32 | vec16 a = load(dmemInA, -32);
  addiu $s0, $s0, 1280                               ## L:585  |     33 | dmemInB += 1280;
  xor $s4, $s4, $s3                                  ## L:580  |     34 | swap(dmemOutA, dmemOutB);
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |     35 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  sqv $v27, 0, -16, $s1                              ## L:588  |     36 | store(a, dmemInA, -16);
  addiu $s0, $s0, 64256                              ## L:591  |     37 | dmemInB -= 1280;
  xor $s3, $s4, $s3                                  ## L:580  |     38 | swap(dmemOutA, dmemOutB);
  xor $s4, $s4, $s3                                  ## L:580  |     39 | swap(dmemOutA, dmemOutB);
  addiu $s1, $s1, 64256                              ## L:590  |     40 | dmemInA -= 1280;
  LABEL_Cmd_Downscale_0028:
  vadd $v27, $v25, $v21.v                            ## L:601  |      ^ | resA:sint = p0_AB:sint + p1_AB:sint;
  luv $v25, 0, 0, $s1                                ## L:616  |     41 | p0_AB = load_vec_u8(dmemInA, 0);
  luv $v24, 0, 8, $s1                                ## L:617  |     42 | p0_CD = load_vec_u8(dmemInA, 8);
  vadd $v26, $v17, $v13.v                            ## L:602  |      ^ | resB:sint = p2_AB:sint + p3_AB:sint;
  vlt $v29, $v27, $v28.e1                            ## L:605  |    *44 | resA = resA < sumFactor.y ? VZERO : resA;
  luv $v22, 0, 8, $s0                                ## L:619  |      ^ | p0_GH = load_vec_u8(dmemInB, 8);
  luv $v23, 0, 0, $s0                                ## L:618  |     45 | p0_EF = load_vec_u8(dmemInB, 0);
  vmrg $v27, $v00, $v27                              ## L:605  |      ^ | resA = resA < sumFactor.y ? VZERO : resA;
  luv $v21, 0, 16, $s1                               ## L:621  |     46 | p1_AB = load_vec_u8(dmemInA, 16);
  vlt $v29, $v26, $v28.e1                            ## L:606  |      ^ | resB = resB < sumFactor.y ? VZERO : resB;
  luv $v20, 0, 24, $s1                               ## L:622  |     47 | p1_CD = load_vec_u8(dmemInA, 24);
  vmrg $v26, $v00, $v26                              ## L:606  |      ^ | resB = resB < sumFactor.y ? VZERO : resB;
  luv $v19, 0, 16, $s0                               ## L:623  |     48 | p1_EF = load_vec_u8(dmemInB, 16);
  vmulf $v25, $v25, $v28.e0                          ## L:524  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v13, 0, 48, $s1                               ## L:631  |     49 | p3_AB = load_vec_u8(dmemInA, 48);
  vmacf $v25, $v24, $v28.e0                          ## L:525  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v18, 0, 24, $s0                               ## L:624  |     50 | p1_GH = load_vec_u8(dmemInB, 24);
  vmacf $v25, $v23, $v28.e0                          ## L:526  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v16, 0, 40, $s1                               ## L:627  |     51 | p2_CD = load_vec_u8(dmemInA, 40);
  vmacf $v25, $v22, $v28.e0                          ## L:527  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v17, 0, 32, $s1                               ## L:626  |     52 | p2_AB = load_vec_u8(dmemInA, 32);
  vmulf $v21, $v21, $v28.e0                          ## L:524  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v15, 0, 32, $s0                               ## L:628  |     53 | p2_EF = load_vec_u8(dmemInB, 32);
  vmacf $v21, $v20, $v28.e0                          ## L:525  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v21, $v19, $v28.e0                          ## L:526  |     54 | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v12, 0, 56, $s1                               ## L:632  |      ^ | p3_CD = load_vec_u8(dmemInA, 56);
  vmacf $v21, $v18, $v28.e0                          ## L:527  |     55 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v14, 0, 40, $s0                               ## L:629  |      ^ | p2_GH = load_vec_u8(dmemInB, 40);
  addiu $a1, $a1, 16                                 ## L:661  |     56 | prtRDRAMOut += 16;
  vmulf $v17, $v17, $v28.e0                          ## L:524  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v11, 0, 48, $s0                               ## L:633  |     57 | p3_EF = load_vec_u8(dmemInB, 48);
  vmacf $v17, $v16, $v28.e0                          ## L:525  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  suv $v27, 0, -16, $s2                              ## L:608  |     58 | store_vec_u8(resA, dmemOut, -16);
  vmacf $v17, $v15, $v28.e0                          ## L:526  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  vmacf $v17, $v14, $v28.e0                          ## L:527  |     59 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v10, 0, 56, $s0                               ## L:634  |      ^ | p3_GH = load_vec_u8(dmemInB, 56);
  vmulf $v13, $v13, $v28.e0                          ## L:524  |     60 | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v13, $v12, $v28.e0                          ## L:525  |     61 | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v13, $v11, $v28.e0                          ## L:526  |     62 | a:sfract = c:sfract +* sumFactor:sfract.x;
  sdv $v25, 8, 0, $s1                                ## L:646  |      ^ | @Barrier("tmp0") store(p0_AB.XYZW, dmemInA, 0); ## Barrier: 0x2
  vmacf $v13, $v10, $v28.e0                          ## L:527  |     63 | a:sfract = d:sfract +* sumFactor:sfract.x;
  suv $v26, 0, -8, $s2                               ## L:609  |      ^ | store_vec_u8(resB, dmemOut, -8);
  vmov $v25.e4, $v21.e0                              ## L:647  |     64 | @Barrier("tmp0") p0_AB.X = p1_AB.x; ## Barrier: 0x2
  sdv $v17, 8, 8, $s1                                ## L:652  |      ^ | @Barrier("tmp1") store(p2_AB.XYZW, dmemInA, 8); ## Barrier: 0x4
  vmov $v25.e5, $v21.e1                              ## L:648  |     65 | @Barrier("tmp0") p0_AB.Y = p1_AB.y; ## Barrier: 0x2
  addiu $s2, $s2, 16                                 ## L:658  |      ^ | dmemOut += 16;
  vmov $v17.e4, $v13.e0                              ## L:653  |    *67 | @Barrier("tmp1") p2_AB.X = p3_AB.x; ## Barrier: 0x4
  vmov $v17.e5, $v13.e1                              ## L:654  |     68 | @Barrier("tmp1") p2_AB.Y = p3_AB.y; ## Barrier: 0x4
  addiu $s0, $s0, 64                                 ## L:660  |      ^ | dmemInB += 64;
  vmov $v17.e6, $v13.e2                              ## L:655  |     69 | @Barrier("tmp1") p2_AB.Z = p3_AB.z; ## Barrier: 0x4
  ldv $v13, 0, 8, $s1                                ## L:656  |      ^ | @Barrier("tmp1") p3_AB.xyzw = load(dmemInA, 8).xyzw; ## Barrier: 0x4
  vmov $v25.e6, $v21.e2                              ## L:649  |     70 | @Barrier("tmp0") p0_AB.Z = p1_AB.z; ## Barrier: 0x2
  ldv $v21, 0, 0, $s1                                ## L:650  |      ^ | @Barrier("tmp0") p1_AB.xyzw = load(dmemInA, 0).xyzw; ## Barrier: 0x2
  bne $s2, $fp, LABEL_Cmd_Downscale_0028             ## L:661  |     71 | prtRDRAMOut += 16;
  addiu $s1, $s1, 64                                 ## L:659  |    *73 | dmemInA += 64;
  LABEL_Cmd_Downscale_0029:
  vadd $v27, $v25, $v21.v                            ## L:665  |     74 | resA:sint = p0_AB:sint + p1_AB:sint;
  vadd $v26, $v17, $v13.v                            ## L:666  |     75 | resB:sint = p2_AB:sint + p3_AB:sint;
  suv $v26, 0, -8, $s2                               ## L:669  |  ***79 | store_vec_u8(resB, dmemOut, -8);
  bne $a0, $s7, LABEL_Cmd_Downscale_0024             ## L:669  |     80 | store_vec_u8(resB, dmemOut, -8);
  suv $v27, 0, -16, $s2                              ## L:668  |    *82 | store_vec_u8(resA, dmemOut, -16);
  LABEL_Cmd_Downscale_0023:
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     83 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     84 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  j RSPQ_Loop                                        ## L:41   |     85 | }
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |   **88 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1

OVERLAY_CODE_END:

#define zero $0
#define v0 $2
#define v1 $3
#define a0 $4
#define a1 $5
#define a2 $6
#define a3 $7
#define t0 $8
#define t1 $9
#define t2 $10
#define t3 $11
#define t4 $12
#define t5 $13
#define t6 $14
#define t7 $15
#define s0 $16
#define s1 $17
#define s2 $18
#define s3 $19
#define s4 $20
#define s5 $21
#define s6 $22
#define s7 $23
#define t8 $24
#define t9 $25
#define k0 $26
#define k1 $27
#define gp $28
#define sp $29
#define fp $30
#define ra $31

.set at
.set macro