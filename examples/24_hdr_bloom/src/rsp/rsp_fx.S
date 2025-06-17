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
  vxor $v26, $v00, $v00.e0                           ## L:64   |      ^ | vec16 bloomFactorA = 0;
  addiu $at, $zero, 31744                            ## L:56   |      ^ | VCONST_RGB.x = 0b0'11111'00000'00000;
  vxor $v27, $v00, $v00.e0                           ## L:62   |      2 | vec16 bloomFactorY = 0;
  mtc2 $at, $v28.e0                                  ## L:56   |      ^ | VCONST_RGB.x = 0b0'11111'00000'00000;
  lui $at, 0x04                                      ## L:58   |      3 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  ori $at, $at, 0xB000                               ## L:58   |      4 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:58   |      5 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addiu $at, $zero, 8192                             ## L:65   |      6 | bloomFactorA:sfract.X = 0.25;
  mtc2 $at, $v26.e4                                  ## L:65   |      7 | bloomFactorA:sfract.X = 0.25;
  addiu $at, $zero, 16384                            ## L:72   |      8 | bloomFactorB:sfract.x = 0.5;
  mtc2 $at, $v25.e0                                  ## L:72   |    *10 | bloomFactorB:sfract.x = 0.5;
  addiu $at, $zero, 24576                            ## L:75   |     11 | bloomFactorB:sfract.X = 0.75;
  vmov $v26.e5, $v26.e4                              ## L:66   |      ^ | bloomFactorA.Y = bloomFactorA.X;
  or $s5, $zero, $zero                               ## L:60   |     12 | u8 bloomPhase = 0;
  ori $s6, $zero, %lo(BUFF_IN_A)                     ## L:59   |    *14 | u16 buffDMEM = BUFF_IN_A;
  vmov $v25.e1, $v25.e0                              ## L:73   |      ^ | bloomFactorB.y = bloomFactorB.x;
  mtc2 $at, $v25.e4                                  ## L:75   |     15 | bloomFactorB:sfract.X = 0.75;
  vmov $v26.e6, $v26.e4                              ## L:67   |      ^ | bloomFactorA.Z = bloomFactorA.X;
  mtc2 $a3, $v26.e3                                  ## L:69   |  ***19 | bloomFactorA.w = factor;
  vmov $v25.e2, $v25.e0                              ## L:74   |      ^ | bloomFactorB.z = bloomFactorB.x;
  LABEL_Cmd_HDRBloom_0001:
  vmov $v25.e5, $v25.e4                              ## L:81   |  ***23 | bloomFactorB.Y = bloomFactorB.X;
  mfc0 $ra, COP0_DMA_BUSY                            ## L:80   |      ^ | RA = get_dma_busy();
  or $s4, $zero, $zero                               ## L:83   |     24 | sumRGB = 0;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0001            ## L:83   |    *26 | sumRGB = 0;
  vmov $v25.e6, $v25.e4                              ## L:82   |    *28 | bloomFactorB.Z = bloomFactorB.X;
  LABEL_Cmd_HDRBloom_0003:
  or $s2, $zero, $s6                                 ## L:88   |     29 | u32 ptrDMEM = buffDMEM;
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:32   |     30 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:33   |     31 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s3, $zero, 1279                             ## L:87   |     32 | u32 dmaSize = (320 * 4) - 1;
  ori $s1, $zero, %lo(BUFF_BLOOM)                    ## L:92   |     33 | u16 dmemBloom = BUFF_BLOOM;
  bne $s5, $zero, LABEL_Cmd_HDRBloom_0005            ## L:94   |     34 | if(bloomPhase == 0) {
  mtc0 $s3, COP0_DMA_READ                            ## L:34   |    *36 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0006:
  addiu $s0, $zero, 640                              ## L:100  |     37 | bloomDmaSize = (80 * 4)*2;
  mfc0 $ra, COP0_DMA_BUSY                            ## L:97   |     38 | RA = get_dma_busy();
  addiu $s5, $zero, 4                                ## L:98   |     39 | bloomPhase = 4;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0006            ## L:100  |    *41 | bloomDmaSize = (80 * 4)*2;
  vsubc $v27, $v00, $v31.e2                          ## L:99   |    *43 | bloomFactorY = VZERO - 0x2000;
  LABEL_Cmd_HDRBloom_0007:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:32   |     44 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR                         ## L:33   |     45 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a2, $a2, 320                                ## L:104  |     46 | ptrBloom += (80 * 4);
  mtc0 $s0, COP0_DMA_READ                            ## L:34   |    *48 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0005:
  addiu $a0, $a0, 1280                               ## L:108  |     49 | ptrIn += (320 * 4);
  vaddc $v27, $v27, $v31.e2                          ## L:115  |      ^ | bloomFactorY += 0x2000;
  ori $s0, $zero, %lo(SAFE_SPACE_3)                  ## L:110  |     50 | u16 tmpMem = SAFE_SPACE_3;
  addiu $s5, $s5, 65535                              ## L:106  |     51 | bloomPhase -= 1;
  LABEL_Cmd_HDRBloom_0008:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:118  |     52 | RA = get_dma_busy();
  ori $sp, $zero, %lo(BUFF_IN_B)                     ## L:119  |     53 | dmemOut = BUFF_IN_B;
  addiu $k1, $s2, 1280                               ## L:121  |     54 | dmemInEnd = ptrDMEM + 320*4;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0008            ## L:121  |     55 | dmemInEnd = ptrDMEM + 320*4;
  or $fp, $zero, $s2                                 ## L:120  |    *57 | dmemIn = ptrDMEM;
  LABEL_Cmd_HDRBloom_0009:
  luv $v19, 0, 328, $s1                              ## L:138  |     58 | bloom2Next = load_vec_u8(dmemBloom, 328);
  luv $v20, 0, 320, $s1                              ## L:135  |     59 | bloom0Next = load_vec_u8(dmemBloom, 320);
  luv $v24, 0, 0, $s1                                ## L:134  |     60 | bloom0 = load_vec_u8(dmemBloom, 0);
  luv $v22, 0, 8, $s1                                ## L:137  |     61 | bloom2 = load_vec_u8(dmemBloom, 8);
  ori $k0, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:127  |     62 | u16 tmpMemA = RSPQ_SCRATCH_MEM;
  addiu $t9, $k0, 1                                  ## L:128  |     63 | u16 tmpMemB = tmpMemA + 1;
  luv $v12, 0, 0, $fp                                ## L:145  |     64 | vec16 pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v14, $v20, $v24.v                           ## L:142  |      ^ | vec16 diffY_A = bloom0Next - bloom0;
  vsubc $v13, $v19, $v22.v                           ## L:143  |     65 | vec16 diffY_B = bloom2Next - bloom2;
  addiu $s1, $s1, 8                                  ## L:140  |      ^ | dmemBloom += 8;
  addiu $t8, $k0, 2                                  ## L:129  |     66 | u16 tmpMemC = tmpMemA + 2;
  LABEL_Cmd_HDRBloom_000A:
  vmudn $v18, $v17, $v30.e6                          ## L:152  |      ^ | pixelOut = pixelR:uint * 2;
  luv $v10, 0, 16, $fp                               ## L:149  |     67 | vec16 pixel2 = load_vec_u8(dmemIn, 16);
  vmadl $v18, $v16, $v31.e3                          ## L:153  |      ^ | asm_op("vmadl", pixelOut, pixelG, VSHIFT8.w);
  vmadl $v18, $v15, $v30.e0                          ## L:154  |     68 | asm_op("vmadl", pixelOut, pixelB, VSHIFT.x);
  lbu $t7, %lo(RSPQ_SCRATCH_MEM + 0)                 ## L:211  |      ^ | tmp0 = load(RSPQ_SCRATCH_MEM, 0);
  lbu $t3, %lo(RSPQ_SCRATCH_MEM + 5)                 ## L:215  |     69 | tmp4 = load(RSPQ_SCRATCH_MEM, 5);
  vmudh $v24, $v24, $v30.e7                          ## L:159  |      ^ | bloom0:sint = bloom0:sint * 1;
  luv $v09, 0, 24, $fp                               ## L:150  |     70 | vec16 pixel3 = load_vec_u8(dmemIn, 24);
  vmacf $v24, $v14, $v27.v                           ## L:160  |      ^ | bloom0:sfract = diffY_A:sfract +* bloomFactorY:sfract;
  luv $v11, 0, 8, $fp                                ## L:148  |     71 | vec16 pixel1 = load_vec_u8(dmemIn, 8);
  vmudm $v12, $v12, $v31.e5                          ## L:162  |      ^ | pixel0 >>= 6;
  vmudh $v22, $v22, $v30.e7                          ## L:164  |     72 | bloom2:sint = bloom2:sint * 1;
  lbu $t6, %lo(RSPQ_SCRATCH_MEM + 1)                 ## L:212  |      ^ | tmp1 = load(RSPQ_SCRATCH_MEM, 1);
  lbu $t4, %lo(RSPQ_SCRATCH_MEM + 4)                 ## L:214  |     73 | tmp3 = load(RSPQ_SCRATCH_MEM, 4);
  vmacf $v22, $v13, $v27.v                           ## L:165  |      ^ | bloom2:sfract = diffY_B:sfract +* bloomFactorY:sfract;
  sqv $v24, 0, 0, $s0                                ## L:171  |   **76 | @Barrier("tmp_0") store(bloom0, tmpMem, 0); ## Barrier: 0x2
  sdv $v22, 0, 24, $s0                               ## L:172  |     77 | @Barrier("tmp_2") store(bloom2.xyzw, tmpMem, 24); ## Barrier: 0x4
  vor $v19, $v00, $v22                               ## L:174  |      ^ | @Barrier("tmp_2") bloom2Next = bloom2; ## Barrier: 0x4
  vor $v22, $v00, $v24                               ## L:175  |     78 | @Barrier("tmp_0") bloom2 = bloom0; ## Barrier: 0x2
  ldv $v24, 8, 0, $s0                                ## L:177  |      ^ | @Barrier("tmp_0") bloom0.XYZW = load(tmpMem, 0).xyzw; ## Barrier: 0x2
  ldv $v22, 0, 8, $s0                                ## L:178  |     79 | @Barrier("tmp_0") bloom2.xyzw = load(tmpMem, 8).xyzw; ## Barrier: 0x2
  vmudm $v09, $v09, $v31.e5                          ## L:183  |      ^ | pixel3 >>= 6;
  lbu $t5, %lo(RSPQ_SCRATCH_MEM + 2)                 ## L:213  |     80 | tmp2 = load(RSPQ_SCRATCH_MEM, 2);
  vmudm $v11, $v11, $v31.e5                          ## L:181  |      ^ | pixel1 >>= 6;
  vsubc $v08, $v22, $v24.v                           ## L:187  |   **83 | vec16 diffX_A = bloom2 - bloom0;
  sqv $v18, 0, -16, $sp                              ## L:156  |      ^ | store(pixelOut, dmemOut, -16);
  ldv $v19, 8, 24, $s0                               ## L:179  |     84 | @Barrier("tmp_2") bloom2Next.XYZW = load(tmpMem, 24).xyzw; ## Barrier: 0x4
  addiu $fp, $fp, 32                                 ## L:268  |     85 | dmemIn += 32;
  vmudm $v10, $v10, $v31.e5                          ## L:182  |      ^ | pixel2 >>= 6;
  lbu $t2, %lo(RSPQ_SCRATCH_MEM + 6)                 ## L:216  |     86 | tmp5 = load(RSPQ_SCRATCH_MEM, 6);
  vmudh $v23, $v24, $v30.e7                          ## L:189  |      ^ | bloom1:sint = bloom0:sint * 1;
  vmacf $v23, $v08, $v25.v                           ## L:190  |     87 | bloom1:sfract = diffX_A:sfract +* bloomFactorB:sfract;
  vmadh $v11, $v11, $v26.e3                          ## L:191  |     88 | pixel1:sint = pixel1:sint +* bloomFactorA:sint.w;
  addu $t5, $t5, $t4                                 ## L:218  |      ^ | tmp2 += tmp3;
  addu $t3, $t3, $t2                                 ## L:219  |     89 | tmp4 += tmp5;
  vsubc $v07, $v19, $v22.v                           ## L:193  |      ^ | vec16 diffX_B = bloom2Next - bloom2;
  vmudh $v24, $v24, $v30.e7                          ## L:195  |     90 | bloom0:sint = bloom0:sint * 1;
  addiu $sp, $sp, 16                                 ## L:269  |      ^ | dmemOut += 16;
  luv $v19, 0, 328, $s1                              ## L:261  |     91 | bloom2Next = load_vec_u8(dmemBloom, 328);
  vmacf $v24, $v08, $v26.v                           ## L:196  |      ^ | bloom0:sfract = diffX_A:sfract +* bloomFactorA:sfract;
  vmadh $v12, $v12, $v26.e3                          ## L:197  |     92 | pixel0:sint = pixel0:sint +* bloomFactorA:sint.w;
  luv $v24, 0, 0, $s1                                ## L:259  |      ^ | bloom0 = load_vec_u8(dmemBloom, 0);
  vmudh $v21, $v22, $v30.e7                          ## L:199  |     93 | bloom3:sint = bloom2:sint * 1;
  vmacf $v21, $v07, $v25.v                           ## L:200  |     94 | bloom3:sfract = diffX_B:sfract +* bloomFactorB:sfract;
  vmadh $v09, $v09, $v26.e3                          ## L:201  |     95 | pixel3:sint = pixel3:sint +* bloomFactorA:sint.w;
  addu $t7, $t7, $t6                                 ## L:217  |      ^ | tmp0 += tmp1;
  addu $t1, $t7, $t5                                 ## L:220  |     96 | sumRGB += tmp0 + tmp2 + tmp4;
  vmudh $v22, $v22, $v30.e7                          ## L:203  |      ^ | bloom2:sint = bloom2:sint * 1;
  suv $v12, 0, 0, $k0                                ## L:224  |     97 | @Barrier("rgba16") store_vec_u8(pixel0, tmpMemA, 0); ## Barrier: 0x8
  vmacf $v22, $v07, $v26.v                           ## L:204  |      ^ | bloom2:sfract = diffX_B:sfract +* bloomFactorA:sfract;
  luv $v22, 0, 8, $s1                                ## L:260  |     98 | bloom2 = load_vec_u8(dmemBloom, 8);
  vmadh $v10, $v10, $v26.e3                          ## L:205  |      ^ | pixel2:sint = pixel2:sint +* bloomFactorA:sint.w;
  vmov $v15.e0, $v12.e2                              ## L:244  |     99 | pixelB.x = pixel0.z;
  addu $t1, $t1, $t3                                 ## L:220  |      ^ | sumRGB += tmp0 + tmp2 + tmp4;
  vmov $v15.e2, $v11.e2                              ## L:246  |    100 | pixelB.z = pixel1.z;
  vmov $v15.e6, $v09.e2                              ## L:250  |    101 | pixelB.Z = pixel3.z;
  suv $v11, 0, 8, $k0                                ## L:225  |      ^ | @Barrier("rgba16") store_vec_u8(pixel1, tmpMemA, 8); ## Barrier: 0x8
  vmov $v15.e5, $v10.e6                              ## L:249  |    102 | pixelB.Y = pixel2.Z;
  suv $v10, 0, 16, $k0                               ## L:226  |      ^ | @Barrier("rgba16") store_vec_u8(pixel2, tmpMemA, 16); ## Barrier: 0x8
  suv $v09, 0, 24, $k0                               ## L:227  |    103 | @Barrier("rgba16") store_vec_u8(pixel3, tmpMemA, 24); ## Barrier: 0x8
  vmov $v15.e3, $v11.e6                              ## L:247  |      ^ | pixelB.w = pixel1.Z;
  vmov $v15.e4, $v10.e2                              ## L:248  |    104 | pixelB.X = pixel2.z;
  lfv $v17, 0, 0, $k0                                ## L:233  |      ^ | asm_op("lfv", pixelR, 0,  0, tmpMemA); ## Barrier: 0x8
  lfv $v17, 8, 16, $k0                               ## L:235  |    105 | asm_op("lfv", pixelR, 8, 16, tmpMemA); ## Barrier: 0x8
  vmov $v15.e7, $v09.e6                              ## L:251  |      ^ | pixelB.W = pixel3.Z;
  lfv $v16, 0, 0, $t9                                ## L:239  |    106 | asm_op("lfv", pixelG, 0,  0, tmpMemB); ## Barrier: 0x8
  lfv $v16, 8, 16, $t9                               ## L:241  |    107 | asm_op("lfv", pixelG, 8, 16, tmpMemB); ## Barrier: 0x8
  vmov $v15.e1, $v12.e6                              ## L:245  |      ^ | pixelB.y = pixel0.Z;
  luv $v12, 0, 0, $fp                                ## L:271  |    108 | pixel0 = load_vec_u8(dmemIn, 0);
  vor $v14, $v00, $v13                               ## L:265  |      ^ | diffY_A = diffY_B;
  addiu $s1, $s1, 8                                  ## L:263  |    109 | dmemBloom += 8;
  addu $s4, $s4, $t1                                 ## L:220  |    110 | sumRGB += tmp0 + tmp2 + tmp4;
  vand $v17, $v17, $v28.e0                           ## L:236  |      ^ | pixelR &= VCONST_RGB.x;
  vand $v16, $v16, $v28.e0                           ## L:242  |    111 | pixelG &= VCONST_RGB.x;
  bne $fp, $k1, LABEL_Cmd_HDRBloom_000A              ## L:271  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v13, $v19, $v22.v                           ## L:266  |   *113 | diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_000B:
  vmudn $v18, $v17, $v30.e6                          ## L:275  |    114 | pixelOut = pixelR:uint * 2;
  addiu $s2, $sp, 64896                              ## L:283  |      ^ | ptrDMEM = dmemOut - 320 * 2;
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:40   |    115 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  vmadl $v18, $v16, $v31.e3                          ## L:276  |      ^ | asm_op("vmadl", pixelOut, pixelG, VSHIFT8.w);
  vmadl $v18, $v15, $v30.e0                          ## L:277  |    116 | asm_op("vmadl", pixelOut, pixelB, VSHIFT.x);
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:41   |      ^ | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $t7, $zero, 639                              ## L:282  |    117 | u32 dmaSize = (320 * 2) - 1;
  addiu $a1, $a1, 640                                ## L:286  |    118 | ptrOut += (320 * 2);
  mtc0 $t7, COP0_DMA_WRITE                           ## L:42   |    119 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  sdv $v18, 0, -16, $sp                              ## L:279  |    120 | store(pixelOut.xyzw, dmemOut, -16);
  bne $a0, $s7, LABEL_Cmd_HDRBloom_0003              ## L:286  |    121 | ptrOut += (320 * 2);
  slv $v00, 0, -8, $sp                               ## L:280  |   *123 | store(VZERO.xy, dmemOut, -8);
  LABEL_Cmd_HDRBloom_0004:
  sw $s4, 0($s6)                                     ## L:290  |    124 | store(sumRGB, buffDMEM, 0);
  mtc0 $s6, COP0_DMA_SPADDR                          ## L:40   |    125 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:41   |    126 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s3, $zero, 7                                ## L:291  |    127 | u16 dmaSize = 8 - 1;
  j RSPQ_Loop                                        ## L:43   |    128 | }
  mtc0 $s3, COP0_DMA_WRITE                           ## L:42   |   *130 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
Cmd_Blur:
  addiu $s5, $zero, 319                              ## L:300  |      ^ | u16 dmaSize = (80 * 4) - 1;
  mtc2 $a2, $v28.e0                                  ## L:306  |      2 | factor.x = thresholdBrightness;
  addiu $s6, $zero, 60                               ## L:298  |      3 | u32 rowsLeft = 60;
  or $s4, $zero, $zero                               ## L:301  |      4 | u16 dmemInOffset = 0;
  addiu $s3, $zero, 1056                             ## L:302  |      5 | u16 dmemInOffsetEnd = (80 * 4 + 32) * 3;
  addiu $s7, $a0, 18880                              ## L:297  |      6 | u32 ptrRDRAMInEnd = ptrRDRAMIn + ((80 * 4) * (60 - 1));
  srl $a2, $a2, 16                                   ## L:307  |      7 | thresholdBrightness >>= 16;
  mtc2 $a2, $v28.e1                                  ## L:308  |      8 | factor.y = thresholdBrightness;
  LABEL_Cmd_Blur_000C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:315  |      9 | RA = get_dma_busy();
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:316  |     10 | dmemInA = BUFF_IN_A;
  or $s0, $zero, $s2                                 ## L:318  |     11 | lastLoadedDmemIn = dmemInA;
  bne $ra, $zero, LABEL_Cmd_Blur_000C                ## L:318  |     12 | lastLoadedDmemIn = dmemInA;
  addiu $s1, $s2, 1056                               ## L:317  |    *14 | dmemOutStart = dmemInA + ((80 * 4 + 32)*3);
  LABEL_Cmd_Blur_000D:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     15 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     16 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:324  |     17 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |    *19 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000E:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     20 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000E                ## L:46   |   **23 | RA = get_dma_busy();
  nop                                                ## L:46   |    *25 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000F:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     26 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     27 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:329  |     28 | ptrRDRAMIn += (80 * 4);
  addiu $s2, $s2, 352                                ## L:328  |     29 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |     30 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0010:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     31 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0010                ## L:46   |   **34 | RA = get_dma_busy();
  nop                                                ## L:46   |    *36 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0011:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     37 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     38 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:333  |     39 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |    *41 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0012:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     42 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0012                ## L:46   |   **45 | RA = get_dma_busy();
  nop                                                ## L:46   |    *47 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0013:
  lw $fp, 316($s2)                                   ## L:337  |     48 | u32 tmp1 = load(dmemInA, 316);
  sw $fp, 320($s2)                                   ## L:338  |   **51 | store(tmp1, dmemInA, 320);
  addiu $s2, $s2, 65184                              ## L:340  |     52 | dmemInA -= (80 * 4 + 32);
  lw $sp, 316($s2)                                   ## L:341  |     53 | u32 tmp2 = load(dmemInA, 316);
  addiu $fp, $zero, 960                              ## L:349  |     54 | u16 thresholdStride = (80 * 4) * 3;
  sw $sp, 320($s2)                                   ## L:342  |    *56 | store(tmp2, dmemInA, 320);
  LABEL_Cmd_Blur_0014:
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:352  |     57 | dmemInA = BUFF_IN_A;
  addiu $s2, $s2, 65532                              ## L:353  |     58 | dmemInA -= 4;
  LABEL_Cmd_Blur_0016:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:359  |     59 | RA = get_dma_busy();
  or $sp, $zero, $s1                                 ## L:360  |     60 | dmemOutCurr = dmemOutStart;
  bne $ra, $zero, LABEL_Cmd_Blur_0016                ## L:361  |    *62 | dmemInBOffset = (80 * 4 + 32);
  addiu $k0, $zero, 352                              ## L:361  |    *64 | dmemInBOffset = (80 * 4 + 32);
  LABEL_Cmd_Blur_0017:
  beq $a2, $zero, LABEL_Cmd_Blur_0018                ## L:366  |     65 | if(thresholdBrightness)
  addiu $t9, $s2, 700                                ## L:364  |    *67 | u16 dmemInC = dmemInA + (80 * 4 + 32)*2 - 4;
  or $t8, $zero, $s0                                 ## L:368  |     68 | u16 ptrClamp = lastLoadedDmemIn;
  addu $t7, $s0, $fp                                 ## L:369  |     69 | u16 ptrClampEnd = lastLoadedDmemIn + thresholdStride;
  LABEL_Cmd_Blur_0019:
  luv $v27, 0, 0, $t8                                ## L:372  |     70 | vec16 p0 = load_vec_u8(ptrClamp, 0);
  luv $v25, 0, 16, $t8                               ## L:374  |     71 | vec16 p2 = load_vec_u8(ptrClamp, 16);
  luv $v26, 0, 8, $t8                                ## L:373  |     72 | vec16 p1 = load_vec_u8(ptrClamp, 8);
  luv $v21, 0, 48, $t8                               ## L:378  |     73 | vec16 p6 = load_vec_u8(ptrClamp, 48);
  luv $v24, 0, 24, $t8                               ## L:375  |     74 | vec16 p3 = load_vec_u8(ptrClamp, 24);
  vlt $v29, $v27, $v28.e1                            ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v22, 0, 40, $t8                               ## L:377  |     75 | vec16 p5 = load_vec_u8(ptrClamp, 40);
  vmrg $v27, $v00, $v27                              ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v19, 0, 64, $t8                               ## L:380  |     76 | vec16 p8 = load_vec_u8(ptrClamp, 64);
  vlt $v29, $v26, $v28.e1                            ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v23, 0, 32, $t8                               ## L:376  |     77 | vec16 p4 = load_vec_u8(ptrClamp, 32);
  vmrg $v26, $v00, $v26                              ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v25, $v28.e1                            ## L:292  |     78 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v25, $v00, $v25                              ## L:292  |     79 | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v20, 0, 56, $t8                               ## L:379  |      ^ | vec16 p7 = load_vec_u8(ptrClamp, 56);
  suv $v27, 0, 0, $t8                                ## L:394  |     80 | store_vec_u8(p0, ptrClamp, 0);
  vlt $v29, $v24, $v28.e1                            ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v24, $v00, $v24                              ## L:292  |     81 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v23, $v28.e1                            ## L:292  |     82 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v23, $v00, $v23                              ## L:292  |     83 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v26, 0, 8, $t8                                ## L:395  |      ^ | store_vec_u8(p1, ptrClamp, 8);
  vlt $v29, $v22, $v28.e1                            ## L:292  |     84 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v22, $v00, $v22                              ## L:292  |     85 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v25, 0, 16, $t8                               ## L:396  |      ^ | store_vec_u8(p2, ptrClamp, 16);
  vlt $v29, $v21, $v28.e1                            ## L:292  |     86 | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v18, 0, 72, $t8                               ## L:381  |      ^ | vec16 p9 = load_vec_u8(ptrClamp, 72);
  vmrg $v21, $v00, $v21                              ## L:292  |     87 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v20, $v28.e1                            ## L:292  |     88 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v20, $v00, $v20                              ## L:292  |     89 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v23, 0, 32, $t8                               ## L:398  |      ^ | store_vec_u8(p4, ptrClamp, 32);
  vlt $v29, $v19, $v28.e1                            ## L:292  |     90 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v24, 0, 24, $t8                               ## L:397  |      ^ | store_vec_u8(p3, ptrClamp, 24);
  suv $v21, 0, 48, $t8                               ## L:400  |     91 | store_vec_u8(p6, ptrClamp, 48);
  vmrg $v19, $v00, $v19                              ## L:292  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v18, $v28.e1                            ## L:292  |     92 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v18, $v00, $v18                              ## L:292  |     93 | pixel = pixel < factor.y ? VZERO : pixel;
  addiu $fp, $zero, 320                              ## L:371  |      ^ | thresholdStride = (80 * 4);
  suv $v20, 0, 56, $t8                               ## L:401  |     94 | store_vec_u8(p7, ptrClamp, 56);
  suv $v19, 0, 64, $t8                               ## L:402  |     95 | store_vec_u8(p8, ptrClamp, 64);
  suv $v22, 0, 40, $t8                               ## L:399  |     96 | store_vec_u8(p5, ptrClamp, 40);
  suv $v18, 0, 72, $t8                               ## L:403  |     97 | store_vec_u8(p9, ptrClamp, 72);
  addiu $t8, $t8, 80                                 ## L:405  |     98 | ptrClamp += 80;
  bne $t8, $t7, LABEL_Cmd_Blur_0019                  ## L:405  |     99 | ptrClamp += 80;
  nop                                                ## L:405  |   *101 | ptrClamp += 80;
  LABEL_Cmd_Blur_0018:
  vxor $v26, $v00, $v00.e0                           ## L:413  |    102 | vec16 pixelA_10 = 0;
  lw $t8, 316($s0)                                   ## L:420  |      ^ | u32 tmp1 = load(lastLoadedDmemIn, 316);
  luv $v23, 0, 360, $s2                              ## L:417  |    103 | vec16 pixelA_11 = load_vec_u8(dmemInA, 360);
  luv $v24, 0, 8, $s2                                ## L:416  |    104 | vec16 pixelA_01 = load_vec_u8(dmemInA, 8);
  luv $v27, 0, 8, $s2                                ## L:412  |    105 | vec16 pixelA_00 = load_vec_u8(dmemInA, 8);
  luv $v25, 0, 8, $t9                                ## L:414  |    106 | vec16 pixelA_20 = load_vec_u8(dmemInC, 8);
  sw $t8, 320($s0)                                   ## L:421  |  **109 | store(tmp1, lastLoadedDmemIn, 320);
  LABEL_Cmd_Blur_001B:
  vmulu $v15, $v27, $v28.e0                          ## L:281  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v22, 0, 8, $t9                                ## L:428  |    110 | vec16 pixelA_21 = load_vec_u8(dmemInC, 8);
  vmacu $v15, $v26, $v28.e0                          ## L:282  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v15, $v25, $v28.e0                          ## L:283  |    111 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v21, 0, 16, $s2                               ## L:430  |      ^ | vec16 pixelB_01 = load_vec_u8(dmemInA, 16);
  vmacu $v15, $v24, $v28.e0                          ## L:285  |    112 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v19, 0, 16, $t9                               ## L:432  |      ^ | vec16 pixelB_21 = load_vec_u8(dmemInC, 16);
  addu $t8, $s2, $k0                                 ## L:425  |    113 | u16 dmemInB = dmemInA + dmemInBOffset;
  vmacu $v15, $v23, $v28.e0                          ## L:286  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v07, 0, 24, $t9                               ## L:442  |    114 | vec16 pixelC_21 = load_vec_u8(dmemInC, 24);
  vmacu $v15, $v22, $v28.e0                          ## L:287  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v20, 0, 16, $t8                               ## L:431  |    115 | vec16 pixelB_11 = load_vec_u8(dmemInB, 16);
  vmulu $v14, $v24, $v28.e0                          ## L:281  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v14, $v23, $v28.e0                          ## L:282  |    116 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v09, 0, 24, $s2                               ## L:440  |      ^ | vec16 pixelC_01 = load_vec_u8(dmemInA, 24);
  vmacu $v14, $v22, $v28.e0                          ## L:283  |    117 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v05, 0, 32, $t8                               ## L:445  |      ^ | vec16 pixelD_11 = load_vec_u8(dmemInB, 32);
  vmacu $v14, $v21, $v28.e0                          ## L:285  |    118 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v14, $v20, $v28.e0                          ## L:286  |    119 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v03, 0, 40, $s2                               ## L:448  |      ^ | vec16 pixelE_01 = load_vec_u8(dmemInA, 40);
  addiu $sp, $sp, 64                                 ## L:475  |    120 | dmemOutCurr += 64;
  vmacu $v14, $v19, $v28.e0                          ## L:287  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v08, 0, 24, $t8                               ## L:441  |    121 | vec16 pixelC_11 = load_vec_u8(dmemInB, 24);
  vmulu $v13, $v21, $v28.e0                          ## L:281  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v06, 0, 32, $s2                               ## L:444  |    122 | vec16 pixelD_01 = load_vec_u8(dmemInA, 32);
  vmacu $v13, $v20, $v28.e0                          ## L:282  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v04, 0, 32, $t9                               ## L:446  |    123 | vec16 pixelD_21 = load_vec_u8(dmemInC, 32);
  vmacu $v13, $v19, $v28.e0                          ## L:283  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v13, $v09, $v28.e0                          ## L:285  |    124 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v27, 0, 64, $s2                               ## L:464  |      ^ | pixelA_00 = load_vec_u8(dmemInA, 64);
  luv $v01, 0, 40, $t9                               ## L:450  |    125 | vec16 pixelE_21 = load_vec_u8(dmemInC, 40);
  vmacu $v13, $v08, $v28.e0                          ## L:286  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v17, 0, 48, $t8                               ## L:435  |    126 | vec16 pixelF_11 = load_vec_u8(dmemInB, 48);
  vmacu $v13, $v07, $v28.e0                          ## L:287  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v26, 0, 64, $t8                               ## L:465  |    127 | pixelA_10 = load_vec_u8(dmemInB, 64);
  vmulu $v12, $v09, $v28.e0                          ## L:281  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v09, 0, 56, $s2                               ## L:460  |    128 | vec16 pixelG_01 = load_vec_u8(dmemInA, 56);
  vmacu $v12, $v08, $v28.e0                          ## L:282  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v12, $v07, $v28.e0                          ## L:283  |    129 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v02, 0, 40, $t8                               ## L:449  |      ^ | vec16 pixelE_11 = load_vec_u8(dmemInB, 40);
  luv $v08, 0, 56, $t8                               ## L:461  |    130 | vec16 pixelG_11 = load_vec_u8(dmemInB, 56);
  vmacu $v12, $v06, $v28.e0                          ## L:285  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v12, $v05, $v28.e0                          ## L:286  |    131 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v18, 0, 48, $s2                               ## L:434  |      ^ | vec16 pixelF_01 = load_vec_u8(dmemInA, 48);
  vmacu $v12, $v04, $v28.e0                          ## L:287  |    132 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v11, $v06, $v28.e0                          ## L:281  |    133 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v16, 0, 48, $t9                               ## L:436  |      ^ | vec16 pixelF_21 = load_vec_u8(dmemInC, 48);
  addiu $s2, $s2, 64                                 ## L:468  |    134 | dmemInA += 64;
  vmacu $v11, $v05, $v28.e0                          ## L:282  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v07, 0, 56, $t9                               ## L:462  |    135 | vec16 pixelG_21 = load_vec_u8(dmemInC, 56);
  vmacu $v11, $v04, $v28.e0                          ## L:283  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  suv $v12, 0, -40, $sp                              ## L:480  |    136 | store_vec_u8(sum3, dmemOutCurr, -40);
  vmacu $v11, $v03, $v28.e0                          ## L:285  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v24, 0, 8, $s2                                ## L:486  |    137 | pixelA_01 = load_vec_u8(dmemInA, 8);
  vmacu $v11, $v02, $v28.e0                          ## L:286  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v13, 0, -48, $sp                              ## L:479  |    138 | store_vec_u8(sum2, dmemOutCurr, -48);
  vmacu $v11, $v01, $v28.e0                          ## L:287  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  xori $k0, $k0, 4                                   ## L:426  |    139 | dmemInBOffset ^= 4;
  vmulu $v10, $v03, $v28.e0                          ## L:281  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v10, $v02, $v28.e0                          ## L:282  |    140 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v10, $v01, $v28.e0                          ## L:283  |    141 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  suv $v14, 0, -56, $sp                              ## L:478  |      ^ | store_vec_u8(sum1, dmemOutCurr, -56);
  vmacu $v10, $v18, $v28.e0                          ## L:285  |    142 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v25, 0, 64, $t9                               ## L:466  |      ^ | pixelA_20 = load_vec_u8(dmemInC, 64);
  vmacu $v10, $v17, $v28.e0                          ## L:286  |    143 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v15, 0, -64, $sp                              ## L:477  |      ^ | store_vec_u8(sum0, dmemOutCurr, -64);
  vmacu $v10, $v16, $v28.e0                          ## L:287  |    144 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v06, $v18, $v28.e0                          ## L:281  |    145 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v06, $v17, $v28.e0                          ## L:282  |    146 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  addiu $t9, $t9, 64                                 ## L:470  |      ^ | dmemInC += 64;
  vmacu $v06, $v16, $v28.e0                          ## L:283  |    147 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  addiu $k1, $s1, 320                                ## L:489  |      ^ | dmemOutEnd = dmemOutStart + (80 * 4);
  vmacu $v06, $v09, $v28.e0                          ## L:285  |    148 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  suv $v10, 0, -24, $sp                              ## L:482  |      ^ | store_vec_u8(sum5, dmemOutCurr, -24);
  vmacu $v06, $v08, $v28.e0                          ## L:286  |    149 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v06, $v07, $v28.e0                          ## L:287  |    150 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v05, $v09, $v28.e0                          ## L:281  |    151 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  addiu $t8, $t8, 64                                 ## L:469  |      ^ | dmemInB += 64;
  vmacu $v05, $v08, $v28.e0                          ## L:282  |    152 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  suv $v11, 0, -32, $sp                              ## L:481  |      ^ | store_vec_u8(sum4, dmemOutCurr, -32);
  vmacu $v05, $v07, $v28.e0                          ## L:283  |    153 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v05, $v27, $v28.e0                          ## L:285  |    154 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  addiu $a1, $a1, 64                                 ## L:490  |      ^ | ptrRDRAMOut += 64;
  vmacu $v05, $v26, $v28.e0                          ## L:286  |    155 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v23, 0, 8, $t8                                ## L:487  |      ^ | pixelA_11 = load_vec_u8(dmemInB, 8);
  suv $v06, 0, -16, $sp                              ## L:483  |    156 | store_vec_u8(sum6, dmemOutCurr, -16);
  vmacu $v05, $v25, $v28.e0                          ## L:287  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  bne $sp, $k1, LABEL_Cmd_Blur_001B                  ## L:490  |    157 | ptrRDRAMOut += 64;
  suv $v05, 0, -8, $sp                               ## L:484  | ***161 | store_vec_u8(sum7, dmemOutCurr, -8);
  LABEL_Cmd_Blur_001C:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:38   |    162 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    163 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s6, $s6, 65535                              ## L:496  |    164 | rowsLeft -= 1;
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |   *166 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_001D:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:499  |    167 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_001D                ## L:500  |  **170 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  addiu $s0, $s4, %lo(BUFF_IN_A)                     ## L:500  |   *172 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  LABEL_Cmd_Blur_001E:
  beq $s6, $zero, RSPQ_Loop                          ## L:503  |    173 | if(rowsLeft == 0)exit;
  nop                                                ## L:503  |   *175 | if(rowsLeft == 0)exit;
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:30   |    176 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |    177 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $s4, 352                                ## L:507  |    178 | dmemInOffset += (80 * 4 + 32);
  beq $a0, $s7, LABEL_Cmd_Blur_0020                  ## L:509  |    179 | if(ptrRDRAMIn != ptrRDRAMInEnd) {
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |   *181 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:510  |    182 | ptrRDRAMIn += (80 * 4);
  LABEL_Cmd_Blur_0020:
  bne $s4, $s3, LABEL_Cmd_Blur_0014                  ## L:513  |    183 | if(dmemInOffset == dmemInOffsetEnd) {
  nop                                                ## L:513  |   *185 | if(dmemInOffset == dmemInOffsetEnd) {
  or $s4, $zero, $zero                               ## L:514  |    186 | dmemInOffset = 0;
  LABEL_Cmd_Blur_0021:
  j LABEL_Cmd_Blur_0014                              ## L:514  |    187 | dmemInOffset = 0;
  nop                                                ## L:514  |   *189 | dmemInOffset = 0;
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