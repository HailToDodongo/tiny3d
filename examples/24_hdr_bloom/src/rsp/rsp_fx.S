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
  lui $at, 0x04                                      ## L:63   |      ^ | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  vxor $v27, $v00, $v30.e1                           ## L:61   |      ^ | vec16 VCONST_G = 1 << 6;
  ori $at, $at, 0xB000                               ## L:63   |      2 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:63   |      3 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addiu $at, $zero, 8192                             ## L:71   |      4 | bloomFactorA:sfract.X = 0.25;
  vxor $v25, $v00, $v00.e0                           ## L:70   |      ^ | vec16 bloomFactorA = 0;
  vxor $v28, $v00, $v31.e4                           ## L:60   |      5 | vec16 VCONST_R = 1 << 11;
  mtc2 $at, $v25.e4                                  ## L:71   |      ^ | bloomFactorA:sfract.X = 0.25;
  addiu $at, $zero, 16384                            ## L:78   |      6 | bloomFactorB:sfract.x = 0.5;
  mtc2 $at, $v24.e0                                  ## L:78   |     *8 | bloomFactorB:sfract.x = 0.5;
  addiu $at, $zero, 24576                            ## L:81   |      9 | bloomFactorB:sfract.X = 0.75;
  vmov $v25.e6, $v25.e4                              ## L:73   |      ^ | bloomFactorA:sfract.Z = bloomFactorA:sfract.X;
  vxor $v26, $v00, $v00.e0                           ## L:69   |     10 | vec16 bloomFactorY = 0;
  vmov $v24.e1, $v24.e0                              ## L:79   |    *12 | bloomFactorB:sfract.y = bloomFactorB:sfract.x;
  mtc2 $at, $v24.e4                                  ## L:81   |     13 | bloomFactorB:sfract.X = 0.75;
  ori $s6, $zero, %lo(BUFF_IN_A)                     ## L:64   |     14 | u16 buffDMEM = BUFF_IN_A;
  vmov $v25.e5, $v25.e4                              ## L:72   |      ^ | bloomFactorA:sfract.Y = bloomFactorA:sfract.X;
  vmov $v24.e2, $v24.e0                              ## L:80   |   **17 | bloomFactorB:sfract.z = bloomFactorB:sfract.x;
  or $s5, $zero, $zero                               ## L:65   |      ^ | u8 bloomPhase = 0;
  mtc2 $a3, $v25.e3                                  ## L:75   |  ***21 | bloomFactorA.w = factor;
  vmov $v24.e5, $v24.e4                              ## L:82   |      ^ | bloomFactorB:sfract.Y = bloomFactorB:sfract.X;
  jal DMAWaitIdle                                    ## L:85   |     22 | dma_await();
  vmov $v24.e6, $v24.e4                              ## L:83   |  ***26 | bloomFactorB:sfract.Z = bloomFactorB:sfract.X;
  LABEL_Cmd_HDRBloom_0001:
  or $s3, $zero, $s6                                 ## L:89   |     27 | u32 ptrDMEM = buffDMEM;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:30   |     28 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     29 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s1, $zero, 640                              ## L:94   |     30 | u16 bloomDmaSize = (80 * 4)*2;
  addiu $s4, $zero, 1279                             ## L:88   |     31 | u32 dmaSize = (320 * 4) - 1;
  mtc0 $s4, COP0_DMA_READ                            ## L:32   |     32 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  bne $s5, $zero, LABEL_Cmd_HDRBloom_0003            ## L:96   |     33 | if(bloomPhase == 0) {
  ori $s2, $zero, %lo(BUFF_BLOOM)                    ## L:93   |    *35 | u16 dmemBloom = BUFF_BLOOM;
  LABEL_Cmd_HDRBloom_0004:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:98   |     36 | RA = get_dma_busy();
  vsubc $v26, $v00, $v31.e2                          ## L:100  |      ^ | bloomFactorY = VZERO - 0x2000;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0004            ## L:100  |   **39 | bloomFactorY = VZERO - 0x2000;
  addiu $s5, $zero, 4                                ## L:99   |    *41 | bloomPhase = 4;
  LABEL_Cmd_HDRBloom_0005:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     42 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR                         ## L:31   |     43 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a2, $a2, 320                                ## L:104  |     44 | ptrBloom += (80 * 4);
  mtc0 $s1, COP0_DMA_READ                            ## L:32   |    *46 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0003:
  addiu $s5, $s5, 65535                              ## L:106  |     47 | bloomPhase -= 1;
  ori $s0, $zero, %lo(SAFE_SPACE_3)                  ## L:110  |     48 | u16 tmpMem = SAFE_SPACE_3;
  addiu $a0, $a0, 1280                               ## L:108  |     49 | ptrIn += (320 * 4);
  vaddc $v26, $v26, $v31.e2                          ## L:115  |      ^ | bloomFactorY += 0x2000;
  LABEL_Cmd_HDRBloom_0006:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:118  |     50 | RA = get_dma_busy();
  addiu $k1, $s3, 1280                               ## L:121  |     51 | dmemInEnd = ptrDMEM + 320*4;
  or $fp, $zero, $s3                                 ## L:120  |     52 | dmemIn = ptrDMEM;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0006            ## L:121  |     53 | dmemInEnd = ptrDMEM + 320*4;
  ori $sp, $zero, %lo(BUFF_IN_B)                     ## L:119  |    *55 | dmemOut = BUFF_IN_B;
  LABEL_Cmd_HDRBloom_0007:
  luv $v19, 0, 320, $s2                              ## L:130  |     56 | bloom0Next = load_vec_u8(dmemBloom, 320);
  luv $v23, 0, 0, $s2                                ## L:129  |     57 | bloom0 = load_vec_u8(dmemBloom, 0);
  addiu $s2, $s2, 4                                  ## L:132  |     58 | dmemBloom += 4;
  luv $v18, 0, 320, $s2                              ## L:135  |     59 | bloom2Next = load_vec_u8(dmemBloom, 320);
  luv $v21, 0, 0, $s2                                ## L:134  |     60 | bloom2 = load_vec_u8(dmemBloom, 0);
  luv $v11, 0, 0, $fp                                ## L:140  |     61 | vec16 pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v13, $v19, $v23.v                           ## L:137  |      ^ | vec16 diffY_A = bloom0Next - bloom0;
  vsubc $v12, $v18, $v21.v                           ## L:138  |   **64 | vec16 diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_0008:
  luv $v08, 0, 24, $fp                               ## L:145  |      ^ | vec16 pixel3 = load_vec_u8(dmemIn, 24);
  vmudh $v23, $v23, $v30.e7                          ## L:159  |     65 | bloom0:sint = bloom0:sint * 1;
  ssv $v14, 12, -2, $sp                              ## L:154  |      ^ | store(pixelOut3.Z, dmemOut, -2);
  vmacf $v23, $v13, $v26.v                           ## L:160  |     66 | bloom0:sfract = diffY_A:sfract +* bloomFactorY:sfract;
  vmudm $v11, $v11, $v31.e5                          ## L:162  |     67 | pixel0 >>= 6;
  luv $v10, 0, 8, $fp                                ## L:143  |      ^ | vec16 pixel1 = load_vec_u8(dmemIn, 8);
  vmudh $v21, $v21, $v30.e7                          ## L:164  |     68 | bloom2:sint = bloom2:sint * 1;
  ssv $v16, 12, -10, $sp                             ## L:150  |      ^ | store(pixelOut1.Z, dmemOut, -10);
  luv $v09, 0, 16, $fp                               ## L:144  |     69 | vec16 pixel2 = load_vec_u8(dmemIn, 16);
  sqv $v23, 0, 0, $s0                                ## L:172  |     70 | @Barrier("tmp_0") store(bloom0, tmpMem, 0); ## Barrier: 0x4
  vmacf $v21, $v12, $v26.v                           ## L:165  |      ^ | bloom2:sfract = diffY_B:sfract +* bloomFactorY:sfract;
  vor $v19, $v00, $v23                               ## L:171  |     71 | @Barrier("tmp_1") bloom0Next = bloom0; ## Barrier: 0x2
  ldv $v23, 8, 0, $s0                                ## L:173  |      ^ | @Barrier("tmp_1") @Barrier("tmp_0") bloom0.XYZW = load(tmpMem, 0).xyzw; ## Barrier: 0x8
  ldv $v19, 0, 8, $s0                                ## L:174  |     72 | @Barrier("tmp_0") bloom0Next.xyzw = load(tmpMem, 0).XYZW; ## Barrier: 0x4
  vmudm $v08, $v08, $v31.e5                          ## L:178  |      ^ | pixel3 >>= 6;
  sqv $v21, 0, 16, $s0                               ## L:181  |   **75 | @Barrier("tmp_2") store(bloom2, tmpMem, 16); ## Barrier: 0x20
  vor $v18, $v00, $v21                               ## L:180  |      ^ | @Barrier("tmp_3") bloom2Next = bloom2; ## Barrier: 0x10
  vsubc $v07, $v19, $v23.v                           ## L:185  |     76 | vec16 diffX_A = bloom0Next - bloom0;
  ldv $v18, 0, 24, $s0                               ## L:183  |      ^ | @Barrier("tmp_2") bloom2Next.xyzw = load(tmpMem, 16).XYZW; ## Barrier: 0x20
  addiu $s2, $s2, 4                                  ## L:156  |     77 | dmemBloom += 4;
  vmudm $v10, $v10, $v31.e5                          ## L:176  |      ^ | pixel1 >>= 6;
  ldv $v21, 8, 16, $s0                               ## L:182  |     78 | @Barrier("tmp_3") @Barrier("tmp_2") bloom2.XYZW = load(tmpMem, 16).xyzw; ## Barrier: 0x40
  vmudm $v09, $v09, $v31.e5                          ## L:177  |      ^ | pixel2 >>= 6;
  vmudh $v22, $v23, $v30.e7                          ## L:187  |     79 | bloom1:sint = bloom0:sint * 1;
  addiu $fp, $fp, 32                                 ## L:228  |      ^ | dmemIn += 32;
  vmacf $v22, $v07, $v24.v                           ## L:188  |     80 | bloom1:sfract = diffX_A:sfract +* bloomFactorB:sfract;
  vmadh $v10, $v10, $v25.e3                          ## L:189  |     81 | pixel1:sint = pixel1:sint +* bloomFactorA:sint.w;
  vsubc $v06, $v18, $v21.v                           ## L:191  |     82 | vec16 diffX_B = bloom2Next - bloom2;
  luv $v19, 0, 320, $s2                              ## L:218  |      ^ | bloom0Next = load_vec_u8(dmemBloom, 320);
  vmudh $v23, $v23, $v30.e7                          ## L:193  |     83 | bloom0:sint = bloom0:sint * 1;
  ssv $v17, 4, -16, $sp                              ## L:147  |      ^ | store(pixelOut0.z, dmemOut, -16);
  vmacf $v23, $v07, $v25.v                           ## L:194  |     84 | bloom0:sfract = diffX_A:sfract +* bloomFactorA:sfract;
  luv $v23, 0, 0, $s2                                ## L:217  |     85 | bloom0 = load_vec_u8(dmemBloom, 0);
  vmadh $v11, $v11, $v25.e3                          ## L:195  |      ^ | pixel0:sint = pixel0:sint +* bloomFactorA:sint.w;
  vmudh $v20, $v21, $v30.e7                          ## L:197  |     86 | bloom3:sint = bloom2:sint * 1;
  addiu $s2, $s2, 4                                  ## L:220  |      ^ | dmemBloom += 4;
  luv $v18, 0, 320, $s2                              ## L:223  |     87 | bloom2Next = load_vec_u8(dmemBloom, 320);
  vmacf $v20, $v06, $v24.v                           ## L:198  |      ^ | bloom3:sfract = diffX_B:sfract +* bloomFactorB:sfract;
  vmadh $v08, $v08, $v25.e3                          ## L:199  |     88 | pixel3:sint = pixel3:sint +* bloomFactorA:sint.w;
  vmudl $v11, $v11, $v30.e1                          ## L:201  |     89 | pixel0 >>>= 10;
  vmudh $v21, $v21, $v30.e7                          ## L:203  |     90 | bloom2:sint = bloom2:sint * 1;
  vmacf $v21, $v06, $v25.v                           ## L:204  |     91 | bloom2:sfract = diffX_B:sfract +* bloomFactorA:sfract;
  ssv $v17, 12, -14, $sp                             ## L:148  |      ^ | store(pixelOut0.Z, dmemOut, -14);
  luv $v21, 0, 0, $s2                                ## L:222  |     92 | bloom2 = load_vec_u8(dmemBloom, 0);
  vmadh $v09, $v09, $v25.e3                          ## L:205  |      ^ | pixel2:sint = pixel2:sint +* bloomFactorA:sint.w;
  vmudl $v10, $v10, $v30.e1                          ## L:207  |     93 | pixel1 >>>= 10;
  vmudn $v17, $v11, $v30.e6                          ## L:53   |     94 | out = in * 2;
  vmadn $v17, $v27, $v11.h1                          ## L:54   |     95 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v17, $v28, $v11.h0                          ## L:55   |     96 | out = VCONST_R +* in.xxxxXXXX;
  ssv $v16, 4, -12, $sp                              ## L:149  |      ^ | store(pixelOut1.z, dmemOut, -12);
  vmudl $v09, $v09, $v30.e1                          ## L:210  |     97 | pixel2 >>>= 10;
  vmudl $v08, $v08, $v30.e1                          ## L:211  |     98 | pixel3 >>>= 10;
  luv $v11, 0, 0, $fp                                ## L:231  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  vmudn $v16, $v10, $v30.e6                          ## L:53   |     99 | out = in * 2;
  vmadn $v16, $v27, $v10.h1                          ## L:54   |    100 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v16, $v28, $v10.h0                          ## L:55   |    101 | out = VCONST_R +* in.xxxxXXXX;
  ssv $v15, 12, -6, $sp                              ## L:152  |      ^ | store(pixelOut2.Z, dmemOut, -6);
  ssv $v15, 4, -8, $sp                               ## L:151  |    102 | store(pixelOut2.z, dmemOut, -8);
  vmudn $v15, $v09, $v30.e6                          ## L:53   |      ^ | out = in * 2;
  ssv $v14, 4, -4, $sp                               ## L:153  |    103 | store(pixelOut3.z, dmemOut, -4);
  vmadn $v15, $v27, $v09.h1                          ## L:54   |      ^ | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v15, $v28, $v09.h0                          ## L:55   |    104 | out = VCONST_R +* in.xxxxXXXX;
  addiu $sp, $sp, 16                                 ## L:229  |      ^ | dmemOut += 16;
  vmudn $v14, $v08, $v30.e6                          ## L:53   |    105 | out = in * 2;
  vmadn $v14, $v27, $v08.h1                          ## L:54   |    106 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v14, $v28, $v08.h0                          ## L:55   |    107 | out = VCONST_R +* in.xxxxXXXX;
  vsubc $v13, $v19, $v23.v                           ## L:225  |    108 | diffY_A = bloom0Next - bloom0;
  bne $fp, $k1, LABEL_Cmd_HDRBloom_0008              ## L:231  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v12, $v18, $v21.v                           ## L:226  |   *110 | diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_0009:
  slv $v00, 0, -8, $sp                               ## L:239  |    111 | store(VZERO.xy, dmemOut, -8);
  ssv $v16, 4, -12, $sp                              ## L:237  |    112 | store(pixelOut1.z, dmemOut, -12);
  addiu $s3, $sp, 64896                              ## L:242  |    113 | ptrDMEM = dmemOut - 320 * 2;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |    114 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    115 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a1, $a1, 640                                ## L:245  |    116 | ptrOut += (320 * 2);
  addiu $k0, $zero, 639                              ## L:241  |    117 | u32 dmaSize = (320 * 2) - 1;
  ssv $v16, 12, -10, $sp                             ## L:238  |    118 | store(pixelOut1.Z, dmemOut, -10);
  mtc0 $k0, COP0_DMA_WRITE                           ## L:40   |    119 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  ssv $v17, 4, -16, $sp                              ## L:235  |    120 | store(pixelOut0.z, dmemOut, -16);
  bne $a0, $s7, LABEL_Cmd_HDRBloom_0001              ## L:245  |    121 | ptrOut += (320 * 2);
  ssv $v17, 12, -14, $sp                             ## L:236  |   *123 | store(pixelOut0.Z, dmemOut, -14);
  LABEL_Cmd_HDRBloom_0002:
  j RSPQ_Loop                                        ## L:246  |    124 | } while(ptrIn != ptrInEnd)
  nop                                                ## L:246  |   *126 | } while(ptrIn != ptrInEnd)
Cmd_Blur:
  addiu $s3, $zero, 1056                             ## L:291  |      ^ | u16 dmemInOffsetEnd = (80 * 4 + 32) * 3;
  mtc2 $a2, $v28.e0                                  ## L:295  |      2 | factor.x = thresholdBrightness;
  srl $a2, $a2, 16                                   ## L:296  |      3 | thresholdBrightness >>= 16;
  addiu $s5, $zero, 319                              ## L:289  |      4 | u16 dmaSize = (80 * 4) - 1;
  addiu $s6, $zero, 60                               ## L:287  |      5 | u32 rowsLeft = 60;
  addiu $s7, $a0, 18880                              ## L:286  |      6 | u32 ptrRDRAMInEnd = ptrRDRAMIn + ((80 * 4) * (60 - 1));
  or $s4, $zero, $zero                               ## L:290  |      7 | u16 dmemInOffset = 0;
  mtc2 $a2, $v28.e1                                  ## L:297  |      8 | factor.y = thresholdBrightness;
  LABEL_Cmd_Blur_000A:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:304  |      9 | RA = get_dma_busy();
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:305  |     10 | dmemInA = BUFF_IN_A;
  addiu $s1, $s2, 1056                               ## L:306  |     11 | dmemOutStart = dmemInA + ((80 * 4 + 32)*3);
  bne $ra, $zero, LABEL_Cmd_Blur_000A                ## L:307  |     12 | lastLoadedDmemIn = dmemInA;
  or $s0, $zero, $s2                                 ## L:307  |    *14 | lastLoadedDmemIn = dmemInA;
  LABEL_Cmd_Blur_000B:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     15 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     16 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:313  |     17 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |    *19 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     20 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000C                ## L:46   |   **23 | RA = get_dma_busy();
  nop                                                ## L:46   |    *25 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000D:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     26 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     27 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:317  |     28 | dmemInA += (80 * 4 + 32);
  addiu $a0, $a0, 320                                ## L:319  |     29 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |     30 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000E:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     31 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000E                ## L:46   |   **34 | RA = get_dma_busy();
  nop                                                ## L:46   |    *36 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000F:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     37 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     38 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:323  |     39 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |    *41 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0010:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     42 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0010                ## L:46   |   **45 | RA = get_dma_busy();
  nop                                                ## L:46   |    *47 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0011:
  lw $fp, 316($s2)                                   ## L:327  |     48 | u32 tmp1 = load(dmemInA, 316);
  sw $fp, 320($s2)                                   ## L:328  |   **51 | store(tmp1, dmemInA, 320);
  addiu $s2, $s2, 65184                              ## L:330  |     52 | dmemInA -= (80 * 4 + 32);
  lw $sp, 316($s2)                                   ## L:331  |     53 | u32 tmp2 = load(dmemInA, 316);
  addiu $fp, $zero, 960                              ## L:339  |     54 | u16 thresholdStride = (80 * 4) * 3;
  sw $sp, 320($s2)                                   ## L:332  |    *56 | store(tmp2, dmemInA, 320);
  LABEL_Cmd_Blur_0012:
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:342  |     57 | dmemInA = BUFF_IN_A;
  addiu $s2, $s2, 65532                              ## L:343  |     58 | dmemInA -= 4;
  LABEL_Cmd_Blur_0014:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:349  |     59 | RA = get_dma_busy();
  or $sp, $zero, $s1                                 ## L:350  |     60 | dmemOutCurr = dmemOutStart;
  bne $ra, $zero, LABEL_Cmd_Blur_0014                ## L:351  |    *62 | dmemInBOffset = (80 * 4 + 32);
  addiu $k0, $zero, 352                              ## L:351  |    *64 | dmemInBOffset = (80 * 4 + 32);
  LABEL_Cmd_Blur_0015:
  beq $a2, $zero, LABEL_Cmd_Blur_0016                ## L:356  |     65 | if(thresholdBrightness)
  addiu $t9, $s2, 700                                ## L:354  |    *67 | u16 dmemInC = dmemInA + (80 * 4 + 32)*2 - 4;
  addu $t7, $s0, $fp                                 ## L:359  |     68 | u16 ptrClampEnd = lastLoadedDmemIn + thresholdStride;
  or $t8, $zero, $s0                                 ## L:358  |     69 | u16 ptrClamp = lastLoadedDmemIn;
  LABEL_Cmd_Blur_0017:
  luv $v27, 0, 0, $t8                                ## L:362  |     70 | vec16 p0 = load_vec_u8(ptrClamp, 0);
  luv $v25, 0, 16, $t8                               ## L:364  |     71 | vec16 p2 = load_vec_u8(ptrClamp, 16);
  luv $v26, 0, 8, $t8                                ## L:363  |     72 | vec16 p1 = load_vec_u8(ptrClamp, 8);
  luv $v23, 0, 32, $t8                               ## L:366  |     73 | vec16 p4 = load_vec_u8(ptrClamp, 32);
  luv $v21, 0, 48, $t8                               ## L:368  |     74 | vec16 p6 = load_vec_u8(ptrClamp, 48);
  vlt $v29, $v27, $v28.e1                            ## L:281  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v27, $v00, $v27                              ## L:281  |     75 | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v24, 0, 24, $t8                               ## L:365  |      ^ | vec16 p3 = load_vec_u8(ptrClamp, 24);
  vlt $v29, $v26, $v28.e1                            ## L:281  |     76 | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v20, 0, 56, $t8                               ## L:369  |      ^ | vec16 p7 = load_vec_u8(ptrClamp, 56);
  luv $v22, 0, 40, $t8                               ## L:367  |     77 | vec16 p5 = load_vec_u8(ptrClamp, 40);
  vmrg $v26, $v00, $v26                              ## L:281  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v19, 0, 64, $t8                               ## L:370  |     78 | vec16 p8 = load_vec_u8(ptrClamp, 64);
  vlt $v29, $v25, $v28.e1                            ## L:281  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  luv $v18, 0, 72, $t8                               ## L:371  |     79 | vec16 p9 = load_vec_u8(ptrClamp, 72);
  vmrg $v25, $v00, $v25                              ## L:281  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v24, $v28.e1                            ## L:281  |     80 | pixel = pixel < factor.y ? VZERO : pixel;
  addiu $fp, $zero, 320                              ## L:361  |      ^ | thresholdStride = (80 * 4);
  vmrg $v24, $v00, $v24                              ## L:281  |     81 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v23, $v28.e1                            ## L:281  |     82 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v27, 0, 0, $t8                                ## L:384  |      ^ | store_vec_u8(p0, ptrClamp, 0);
  vmrg $v23, $v00, $v23                              ## L:281  |     83 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v22, $v28.e1                            ## L:281  |     84 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v22, $v00, $v22                              ## L:281  |     85 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v25, 0, 16, $t8                               ## L:386  |      ^ | store_vec_u8(p2, ptrClamp, 16);
  vlt $v29, $v21, $v28.e1                            ## L:281  |     86 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v24, 0, 24, $t8                               ## L:387  |      ^ | store_vec_u8(p3, ptrClamp, 24);
  suv $v23, 0, 32, $t8                               ## L:388  |     87 | store_vec_u8(p4, ptrClamp, 32);
  vmrg $v21, $v00, $v21                              ## L:281  |      ^ | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v20, $v28.e1                            ## L:281  |     88 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v20, $v00, $v20                              ## L:281  |     89 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v19, $v28.e1                            ## L:281  |     90 | pixel = pixel < factor.y ? VZERO : pixel;
  vmrg $v19, $v00, $v19                              ## L:281  |     91 | pixel = pixel < factor.y ? VZERO : pixel;
  vlt $v29, $v18, $v28.e1                            ## L:281  |     92 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v26, 0, 8, $t8                                ## L:385  |      ^ | store_vec_u8(p1, ptrClamp, 8);
  vmrg $v18, $v00, $v18                              ## L:281  |     93 | pixel = pixel < factor.y ? VZERO : pixel;
  suv $v22, 0, 40, $t8                               ## L:389  |      ^ | store_vec_u8(p5, ptrClamp, 40);
  suv $v20, 0, 56, $t8                               ## L:391  |     94 | store_vec_u8(p7, ptrClamp, 56);
  suv $v21, 0, 48, $t8                               ## L:390  |     95 | store_vec_u8(p6, ptrClamp, 48);
  suv $v19, 0, 64, $t8                               ## L:392  |     96 | store_vec_u8(p8, ptrClamp, 64);
  suv $v18, 0, 72, $t8                               ## L:393  |     97 | store_vec_u8(p9, ptrClamp, 72);
  addiu $t8, $t8, 80                                 ## L:395  |     98 | ptrClamp += 80;
  bne $t8, $t7, LABEL_Cmd_Blur_0017                  ## L:395  |     99 | ptrClamp += 80;
  nop                                                ## L:395  |   *101 | ptrClamp += 80;
  LABEL_Cmd_Blur_0016:
  vxor $v26, $v00, $v00.e0                           ## L:403  |    102 | vec16 pixelA_10 = 0;
  luv $v27, 0, 8, $s2                                ## L:402  |      ^ | vec16 pixelA_00 = load_vec_u8(dmemInA, 8);
  luv $v24, 0, 8, $s2                                ## L:406  |    103 | vec16 pixelA_01 = load_vec_u8(dmemInA, 8);
  lw $t8, 316($s0)                                   ## L:410  |    104 | u32 tmp1 = load(lastLoadedDmemIn, 316);
  luv $v23, 0, 360, $s2                              ## L:407  |    105 | vec16 pixelA_11 = load_vec_u8(dmemInA, 360);
  luv $v25, 0, 8, $t9                                ## L:404  |    106 | vec16 pixelA_20 = load_vec_u8(dmemInC, 8);
  sw $t8, 320($s0)                                   ## L:411  |  **109 | store(tmp1, lastLoadedDmemIn, 320);
  LABEL_Cmd_Blur_0019:
  vmulu $v15, $v27, $v28.e0                          ## L:270  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v22, 0, 8, $t9                                ## L:418  |    110 | vec16 pixelA_21 = load_vec_u8(dmemInC, 8);
  vmacu $v15, $v26, $v28.e0                          ## L:271  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v15, $v25, $v28.e0                          ## L:272  |    111 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v06, 0, 32, $s2                               ## L:434  |      ^ | vec16 pixelD_01 = load_vec_u8(dmemInA, 32);
  addu $t8, $s2, $k0                                 ## L:415  |    112 | u16 dmemInB = dmemInA + dmemInBOffset;
  vmacu $v15, $v24, $v28.e0                          ## L:274  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v21, 0, 16, $s2                               ## L:420  |    113 | vec16 pixelB_01 = load_vec_u8(dmemInA, 16);
  vmacu $v15, $v23, $v28.e0                          ## L:275  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v20, 0, 16, $t8                               ## L:421  |    114 | vec16 pixelB_11 = load_vec_u8(dmemInB, 16);
  vmacu $v15, $v22, $v28.e0                          ## L:276  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v14, $v24, $v28.e0                          ## L:270  |    115 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v19, 0, 16, $t9                               ## L:422  |      ^ | vec16 pixelB_21 = load_vec_u8(dmemInC, 16);
  addiu $k1, $s1, 320                                ## L:479  |    116 | dmemOutEnd = dmemOutStart + (80 * 4);
  vmacu $v14, $v23, $v28.e0                          ## L:271  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v09, 0, 24, $s2                               ## L:430  |    117 | vec16 pixelC_01 = load_vec_u8(dmemInA, 24);
  vmacu $v14, $v22, $v28.e0                          ## L:272  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v14, $v21, $v28.e0                          ## L:274  |    118 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v04, 0, 32, $t9                               ## L:436  |      ^ | vec16 pixelD_21 = load_vec_u8(dmemInC, 32);
  luv $v07, 0, 24, $t9                               ## L:432  |    119 | vec16 pixelC_21 = load_vec_u8(dmemInC, 24);
  vmacu $v14, $v20, $v28.e0                          ## L:275  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v14, $v19, $v28.e0                          ## L:276  |    120 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v01, 0, 40, $t9                               ## L:440  |      ^ | vec16 pixelE_21 = load_vec_u8(dmemInC, 40);
  vmulu $v13, $v21, $v28.e0                          ## L:270  |    121 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v08, 0, 24, $t8                               ## L:431  |      ^ | vec16 pixelC_11 = load_vec_u8(dmemInB, 24);
  vmacu $v13, $v20, $v28.e0                          ## L:271  |    122 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v13, $v19, $v28.e0                          ## L:272  |    123 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v13, $v09, $v28.e0                          ## L:274  |    124 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v13, $v08, $v28.e0                          ## L:275  |    125 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v02, 0, 40, $t8                               ## L:439  |      ^ | vec16 pixelE_11 = load_vec_u8(dmemInB, 40);
  vmacu $v13, $v07, $v28.e0                          ## L:276  |    126 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v18, 0, 48, $s2                               ## L:424  |      ^ | vec16 pixelF_01 = load_vec_u8(dmemInA, 48);
  vmulu $v12, $v09, $v28.e0                          ## L:270  |    127 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v05, 0, 32, $t8                               ## L:435  |      ^ | vec16 pixelD_11 = load_vec_u8(dmemInB, 32);
  vmacu $v12, $v08, $v28.e0                          ## L:271  |    128 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v25, 0, 64, $t9                               ## L:456  |      ^ | pixelA_20 = load_vec_u8(dmemInC, 64);
  vmacu $v12, $v07, $v28.e0                          ## L:272  |    129 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v16, 0, 48, $t9                               ## L:426  |      ^ | vec16 pixelF_21 = load_vec_u8(dmemInC, 48);
  vmacu $v12, $v06, $v28.e0                          ## L:274  |    130 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v03, 0, 40, $s2                               ## L:438  |      ^ | vec16 pixelE_01 = load_vec_u8(dmemInA, 40);
  vmacu $v12, $v05, $v28.e0                          ## L:275  |    131 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v26, 0, 64, $t8                               ## L:455  |      ^ | pixelA_10 = load_vec_u8(dmemInB, 64);
  vmacu $v12, $v04, $v28.e0                          ## L:276  |    132 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v17, 0, 48, $t8                               ## L:425  |      ^ | vec16 pixelF_11 = load_vec_u8(dmemInB, 48);
  luv $v08, 0, 56, $t8                               ## L:451  |    133 | vec16 pixelG_11 = load_vec_u8(dmemInB, 56);
  vmulu $v11, $v06, $v28.e0                          ## L:270  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  addiu $t8, $t8, 64                                 ## L:459  |    134 | dmemInB += 64;
  vmacu $v11, $v05, $v28.e0                          ## L:271  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v23, 0, 8, $t8                                ## L:477  |    135 | pixelA_11 = load_vec_u8(dmemInB, 8);
  vmacu $v11, $v04, $v28.e0                          ## L:272  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v11, $v03, $v28.e0                          ## L:274  |    136 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v07, 0, 56, $t9                               ## L:452  |      ^ | vec16 pixelG_21 = load_vec_u8(dmemInC, 56);
  addiu $t9, $t9, 64                                 ## L:460  |    137 | dmemInC += 64;
  vmacu $v11, $v02, $v28.e0                          ## L:275  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v11, $v01, $v28.e0                          ## L:276  |    138 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  addiu $sp, $sp, 64                                 ## L:465  |      ^ | dmemOutCurr += 64;
  vmulu $v10, $v03, $v28.e0                          ## L:270  |    139 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v10, $v02, $v28.e0                          ## L:271  |    140 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  suv $v13, 0, -48, $sp                              ## L:469  |      ^ | store_vec_u8(sum2, dmemOutCurr, -48);
  suv $v14, 0, -56, $sp                              ## L:468  |    141 | store_vec_u8(sum1, dmemOutCurr, -56);
  vmacu $v10, $v01, $v28.e0                          ## L:272  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v09, 0, 56, $s2                               ## L:450  |    142 | vec16 pixelG_01 = load_vec_u8(dmemInA, 56);
  vmacu $v10, $v18, $v28.e0                          ## L:274  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v10, $v17, $v28.e0                          ## L:275  |    143 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v15, 0, -64, $sp                              ## L:467  |      ^ | store_vec_u8(sum0, dmemOutCurr, -64);
  vmacu $v10, $v16, $v28.e0                          ## L:276  |    144 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v27, 0, 64, $s2                               ## L:454  |      ^ | pixelA_00 = load_vec_u8(dmemInA, 64);
  vmulu $v06, $v18, $v28.e0                          ## L:270  |    145 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  addiu $s2, $s2, 64                                 ## L:458  |      ^ | dmemInA += 64;
  luv $v24, 0, 8, $s2                                ## L:476  |    146 | pixelA_01 = load_vec_u8(dmemInA, 8);
  vmacu $v06, $v17, $v28.e0                          ## L:271  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v06, $v16, $v28.e0                          ## L:272  |    147 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v06, $v09, $v28.e0                          ## L:274  |    148 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  addiu $a1, $a1, 64                                 ## L:480  |      ^ | ptrRDRAMOut += 64;
  vmacu $v06, $v08, $v28.e0                          ## L:275  |    149 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v12, 0, -40, $sp                              ## L:470  |      ^ | store_vec_u8(sum3, dmemOutCurr, -40);
  suv $v11, 0, -32, $sp                              ## L:471  |    150 | store_vec_u8(sum4, dmemOutCurr, -32);
  vmacu $v06, $v07, $v28.e0                          ## L:276  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v05, $v09, $v28.e0                          ## L:270  |    151 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v05, $v08, $v28.e0                          ## L:271  |    152 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v05, $v07, $v28.e0                          ## L:272  |    153 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  xori $k0, $k0, 4                                   ## L:416  |      ^ | dmemInBOffset ^= 4;
  vmacu $v05, $v27, $v28.e0                          ## L:274  |    154 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v05, $v26, $v28.e0                          ## L:275  |    155 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v10, 0, -24, $sp                              ## L:472  |      ^ | store_vec_u8(sum5, dmemOutCurr, -24);
  vmacu $v05, $v25, $v28.e0                          ## L:276  |    156 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  suv $v06, 0, -16, $sp                              ## L:473  |      ^ | store_vec_u8(sum6, dmemOutCurr, -16);
  bne $sp, $k1, LABEL_Cmd_Blur_0019                  ## L:480  |    157 | ptrRDRAMOut += 64;
  suv $v05, 0, -8, $sp                               ## L:474  | ***161 | store_vec_u8(sum7, dmemOutCurr, -8);
  LABEL_Cmd_Blur_001A:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:38   |    162 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    163 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s6, $s6, 65535                              ## L:486  |    164 | rowsLeft -= 1;
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |   *166 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_001B:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:489  |    167 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_001B                ## L:490  |  **170 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  addiu $s0, $s4, %lo(BUFF_IN_A)                     ## L:490  |   *172 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  LABEL_Cmd_Blur_001C:
  beq $s6, $zero, RSPQ_Loop                          ## L:493  |    173 | if(rowsLeft == 0)exit;
  nop                                                ## L:493  |   *175 | if(rowsLeft == 0)exit;
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:30   |    176 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |    177 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $s4, 352                                ## L:497  |    178 | dmemInOffset += (80 * 4 + 32);
  beq $a0, $s7, LABEL_Cmd_Blur_001E                  ## L:499  |    179 | if(ptrRDRAMIn != ptrRDRAMInEnd) {
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |   *181 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:500  |    182 | ptrRDRAMIn += (80 * 4);
  LABEL_Cmd_Blur_001E:
  bne $s4, $s3, LABEL_Cmd_Blur_0012                  ## L:503  |    183 | if(dmemInOffset == dmemInOffsetEnd) {
  nop                                                ## L:503  |   *185 | if(dmemInOffset == dmemInOffsetEnd) {
  or $s4, $zero, $zero                               ## L:504  |    186 | dmemInOffset = 0;
  LABEL_Cmd_Blur_001F:
  j LABEL_Cmd_Blur_0012                              ## L:504  |    187 | dmemInOffset = 0;
  nop                                                ## L:504  |   *189 | dmemInOffset = 0;
Cmd_Downscale:
  addiu $a1, $a1, 65216                              ## L:472  |      ^ | prtRDRAMOut -= (80 * 4);
  ori $s3, $zero, %lo(BUFF_BLOOM_B)                  ## L:485  |      2 | u16 dmemOutB = BUFF_BLOOM_B;
  lui $at, 0x04                                      ## L:470  |      3 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addiu $s5, $zero, 319                              ## L:479  |      4 | u16 dmaSizeOut = (80 * 4) - 1;
  ori $at, $at, 0xB000                               ## L:470  |      5 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:470  |      6 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addiu $at, $zero, 4096                             ## L:482  |      7 | sumFactor:sfract.x = 0.125;
  lui $s6, 0x5000                                    ## L:474  |      8 | u16 dmaSizeIn = (
  mtc2 $at, $v28.e0                                  ## L:482  |      9 | sumFactor:sfract.x = 0.125;
  ori $s6, $s6, 0x14FF                               ## L:474  |     10 | u16 dmaSizeIn = (
  ori $s4, $zero, %lo(BUFF_BLOOM)                    ## L:484  |     11 | u16 dmemOutA = BUFF_BLOOM;
  LABEL_Cmd_Downscale_001C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:493  |     12 | RA = get_dma_busy();
  ori $s1, $zero, %lo(BUFF_IN_A)                     ## L:494  |     13 | dmemInA = BUFF_IN_A;
  bne $ra, $zero, LABEL_Cmd_Downscale_001C           ## L:495  |    *15 | dmemInB = BUFF_IN_B;
  ori $s0, $zero, %lo(BUFF_IN_B)                     ## L:495  |    *17 | dmemInB = BUFF_IN_B;
  LABEL_Cmd_Downscale_001D:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:30   |     18 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     19 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 5120                               ## L:501  |     20 | ptrRDRAMIn += (320 * 4) * 4;
  mtc0 $s6, COP0_DMA_READ                            ## L:32   |    *22 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Downscale_001E:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:504  |     23 | RA = get_dma_busy();
  or $s2, $zero, $s4                                 ## L:506  |     24 | dmemOut = dmemOutA;
  bne $ra, $zero, LABEL_Cmd_Downscale_001E           ## L:507  |    *26 | dmemOutEnd = dmemOut + (80 * 4);
  addiu $fp, $s2, 320                                ## L:507  |    *28 | dmemOutEnd = dmemOut + (80 * 4);
  LABEL_Cmd_Downscale_001F:
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     29 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  xor $s4, $s4, $s3                                  ## L:511  |     30 | swap(dmemOutA, dmemOutB);
  addiu $s1, $s1, 1280                               ## L:515  |     31 | dmemInA += 1280;
  addiu $s0, $s0, 1280                               ## L:516  |     32 | dmemInB += 1280;
  lqv $v27, 0, -32, $s1                              ## L:518  |     33 | vec16 a = load(dmemInA, -32);
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     34 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  xor $s3, $s4, $s3                                  ## L:511  |     35 | swap(dmemOutA, dmemOutB);
  addiu $s0, $s0, 64256                              ## L:522  |     36 | dmemInB -= 1280;
  sqv $v27, 0, -16, $s1                              ## L:519  |     37 | store(a, dmemInA, -16);
  addiu $s1, $s1, 64256                              ## L:521  |     38 | dmemInA -= 1280;
  xor $s4, $s4, $s3                                  ## L:511  |     39 | swap(dmemOutA, dmemOutB);
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |     40 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_Downscale_0020:
  vadd $v27, $v25, $v21.v                            ## L:532  |      ^ | resA:sint = p0_AB:sint + p1_AB:sint;
  luv $v22, 0, 8, $s0                                ## L:550  |     41 | p0_GH = load_vec_u8(dmemInB, 8);
  luv $v25, 0, 0, $s1                                ## L:547  |     42 | p0_AB = load_vec_u8(dmemInA, 0);
  vadd $v26, $v17, $v13.v                            ## L:533  |      ^ | resB:sint = p2_AB:sint + p3_AB:sint;
  luv $v24, 0, 8, $s1                                ## L:548  |     43 | p0_CD = load_vec_u8(dmemInA, 8);
  luv $v23, 0, 0, $s0                                ## L:549  |     44 | p0_EF = load_vec_u8(dmemInB, 0);
  vlt $v29, $v27, $v28.e1                            ## L:536  |      ^ | resA = resA < sumFactor.y ? VZERO : resA;
  vmrg $v27, $v00, $v27                              ## L:536  |     45 | resA = resA < sumFactor.y ? VZERO : resA;
  luv $v21, 0, 16, $s1                               ## L:552  |      ^ | p1_AB = load_vec_u8(dmemInA, 16);
  vlt $v29, $v26, $v28.e1                            ## L:537  |     46 | resB = resB < sumFactor.y ? VZERO : resB;
  luv $v20, 0, 24, $s1                               ## L:553  |      ^ | p1_CD = load_vec_u8(dmemInA, 24);
  luv $v18, 0, 24, $s0                               ## L:555  |     47 | p1_GH = load_vec_u8(dmemInB, 24);
  vmrg $v26, $v00, $v26                              ## L:537  |      ^ | resB = resB < sumFactor.y ? VZERO : resB;
  luv $v11, 0, 48, $s0                               ## L:564  |     48 | p3_EF = load_vec_u8(dmemInB, 48);
  vmulf $v25, $v25, $v28.e0                          ## L:455  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v25, $v24, $v28.e0                          ## L:456  |     49 | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v17, 0, 32, $s1                               ## L:557  |      ^ | p2_AB = load_vec_u8(dmemInA, 32);
  vmacf $v25, $v23, $v28.e0                          ## L:457  |     50 | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v19, 0, 16, $s0                               ## L:554  |      ^ | p1_EF = load_vec_u8(dmemInB, 16);
  vmacf $v25, $v22, $v28.e0                          ## L:458  |     51 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v12, 0, 56, $s1                               ## L:563  |      ^ | p3_CD = load_vec_u8(dmemInA, 56);
  vmulf $v21, $v21, $v28.e0                          ## L:455  |     52 | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v21, $v20, $v28.e0                          ## L:456  |     53 | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v16, 0, 40, $s1                               ## L:558  |      ^ | p2_CD = load_vec_u8(dmemInA, 40);
  luv $v15, 0, 32, $s0                               ## L:559  |     54 | p2_EF = load_vec_u8(dmemInB, 32);
  vmacf $v21, $v19, $v28.e0                          ## L:457  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  vmacf $v21, $v18, $v28.e0                          ## L:458  |     55 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v14, 0, 40, $s0                               ## L:560  |      ^ | p2_GH = load_vec_u8(dmemInB, 40);
  vmulf $v17, $v17, $v28.e0                          ## L:455  |     56 | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v13, 0, 48, $s1                               ## L:562  |      ^ | p3_AB = load_vec_u8(dmemInA, 48);
  vmacf $v17, $v16, $v28.e0                          ## L:456  |     57 | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v17, $v15, $v28.e0                          ## L:457  |     58 | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v10, 0, 56, $s0                               ## L:565  |      ^ | p3_GH = load_vec_u8(dmemInB, 56);
  suv $v26, 0, -8, $s2                               ## L:540  |     59 | store_vec_u8(resB, dmemOut, -8);
  vmacf $v17, $v14, $v28.e0                          ## L:458  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  vmulf $v13, $v13, $v28.e0                          ## L:455  |     60 | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v13, $v12, $v28.e0                          ## L:456  |     61 | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v13, $v11, $v28.e0                          ## L:457  |     62 | a:sfract = c:sfract +* sumFactor:sfract.x;
  addiu $a1, $a1, 16                                 ## L:592  |      ^ | prtRDRAMOut += 16;
  sdv $v25, 8, 0, $s1                                ## L:577  |     63 | @Barrier("tmp0") store(p0_AB.XYZW, dmemInA, 0); ## Barrier: 0x2
  vmacf $v13, $v10, $v28.e0                          ## L:458  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  suv $v27, 0, -16, $s2                              ## L:539  |     64 | store_vec_u8(resA, dmemOut, -16);
  vmov $v25.e4, $v21.e0                              ## L:578  |      ^ | @Barrier("tmp0") p0_AB.X = p1_AB.x; ## Barrier: 0x2
  vmov $v25.e5, $v21.e1                              ## L:579  |     65 | @Barrier("tmp0") p0_AB.Y = p1_AB.y; ## Barrier: 0x2
  addiu $s0, $s0, 64                                 ## L:591  |      ^ | dmemInB += 64;
  vmov $v25.e6, $v21.e2                              ## L:580  |     66 | @Barrier("tmp0") p0_AB.Z = p1_AB.z; ## Barrier: 0x2
  sdv $v17, 8, 8, $s1                                ## L:583  |      ^ | @Barrier("tmp1") store(p2_AB.XYZW, dmemInA, 8); ## Barrier: 0x4
  vmov $v17.e4, $v13.e0                              ## L:584  |     67 | @Barrier("tmp1") p2_AB.X = p3_AB.x; ## Barrier: 0x4
  addiu $s2, $s2, 16                                 ## L:589  |      ^ | dmemOut += 16;
  vmov $v17.e5, $v13.e1                              ## L:585  |     68 | @Barrier("tmp1") p2_AB.Y = p3_AB.y; ## Barrier: 0x4
  ldv $v21, 0, 0, $s1                                ## L:581  |      ^ | @Barrier("tmp0") p1_AB.xyzw = load(dmemInA, 0).xyzw; ## Barrier: 0x2
  vmov $v17.e6, $v13.e2                              ## L:586  |     69 | @Barrier("tmp1") p2_AB.Z = p3_AB.z; ## Barrier: 0x4
  ldv $v13, 0, 8, $s1                                ## L:587  |      ^ | @Barrier("tmp1") p3_AB.xyzw = load(dmemInA, 8).xyzw; ## Barrier: 0x4
  bne $s2, $fp, LABEL_Cmd_Downscale_0020             ## L:592  |     70 | prtRDRAMOut += 16;
  addiu $s1, $s1, 64                                 ## L:590  |    *72 | dmemInA += 64;
  LABEL_Cmd_Downscale_0021:
  vadd $v27, $v25, $v21.v                            ## L:596  |     73 | resA:sint = p0_AB:sint + p1_AB:sint;
  vadd $v26, $v17, $v13.v                            ## L:597  |     74 | resB:sint = p2_AB:sint + p3_AB:sint;
  suv $v26, 0, -8, $s2                               ## L:600  |  ***78 | store_vec_u8(resB, dmemOut, -8);
  bne $a0, $s7, LABEL_Cmd_Downscale_001C             ## L:600  |     79 | store_vec_u8(resB, dmemOut, -8);
  suv $v27, 0, -16, $s2                              ## L:599  |    *81 | store_vec_u8(resA, dmemOut, -16);
  LABEL_Cmd_Downscale_001B:
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     82 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     83 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  j RSPQ_Loop                                        ## L:41   |     84 | }
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |   **87 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1

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