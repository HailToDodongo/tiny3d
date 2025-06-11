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
    RSPQ_DefineCommand Cmd_Downscale, 12
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
  vxor $v28, $v00, $v31.e4                           ## L:60   |      ^ | vec16 VCONST_R = 1 << 11;
  jal DMAWaitIdle                                    ## L:63   |      ^ | dma_await();
  vxor $v27, $v00, $v30.e1                           ## L:61   |     *3 | vec16 VCONST_G = 1 << 6;
  vxor $v26, $v00, $v00.e0                           ## L:71   |      4 | vec16 bloomFactorY = 0;
  lui $at, 0x04                                      ## L:65   |      ^ | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  vxor $v25, $v00, $v00.e0                           ## L:72   |      5 | vec16 bloomFactorA = 0;
  ori $at, $at, 0xB000                               ## L:65   |      ^ | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:65   |      6 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addiu $at, $zero, 8192                             ## L:73   |      7 | bloomFactorA:sfract.X = 0.25;
  mtc2 $at, $v25.e4                                  ## L:73   |      8 | bloomFactorA:sfract.X = 0.25;
  addiu $at, $zero, 16384                            ## L:80   |      9 | bloomFactorB:sfract.x = 0.5;
  mtc2 $at, $v24.e0                                  ## L:80   |    *11 | bloomFactorB:sfract.x = 0.5;
  addiu $at, $zero, 24576                            ## L:83   |     12 | bloomFactorB:sfract.X = 0.75;
  vmov $v25.e5, $v25.e4                              ## L:74   |      ^ | bloomFactorA:sfract.Y = bloomFactorA:sfract.X;
  or $s5, $zero, $zero                               ## L:67   |   **15 | u8 bloomPhase = 0;
  vmov $v24.e2, $v24.e0                              ## L:82   |      ^ | bloomFactorB:sfract.z = bloomFactorB:sfract.x;
  mtc2 $at, $v24.e4                                  ## L:83   |     16 | bloomFactorB:sfract.X = 0.75;
  vmov $v25.e6, $v25.e4                              ## L:75   |      ^ | bloomFactorA:sfract.Z = bloomFactorA:sfract.X;
  mtc2 $a3, $v25.e3                                  ## L:77   |  ***20 | bloomFactorA.w = factor;
  vmov $v24.e1, $v24.e0                              ## L:81   |      ^ | bloomFactorB:sfract.y = bloomFactorB:sfract.x;
  vmov $v24.e5, $v24.e4                              ## L:84   |  ***24 | bloomFactorB:sfract.Y = bloomFactorB:sfract.X;
  ori $s6, $zero, %lo(BUFF_IN_A)                     ## L:66   |      ^ | u16 buffDMEM = BUFF_IN_A;
  vmov $v24.e6, $v24.e4                              ## L:85   |  ***28 | bloomFactorB:sfract.Z = bloomFactorB:sfract.X;
  LABEL_Cmd_HDRBloom_0001:
  or $s3, $zero, $s6                                 ## L:89   |      ^ | u32 ptrDMEM = buffDMEM;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:30   |     29 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  addiu $s4, $zero, 1279                             ## L:88   |     30 | u32 dmaSize = (320 * 4) - 1;
  addiu $s1, $zero, 640                              ## L:94   |     31 | u16 bloomDmaSize = (80 * 4)*2;
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     32 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $s4, COP0_DMA_READ                            ## L:32   |     33 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  bne $s5, $zero, LABEL_Cmd_HDRBloom_0003            ## L:96   |     34 | if(bloomPhase == 0) {
  ori $s2, $zero, %lo(BUFF_BLOOM)                    ## L:93   |    *36 | u16 dmemBloom = BUFF_BLOOM;
  LABEL_Cmd_HDRBloom_0004:
  vsubc $v26, $v00, $v31.e2                          ## L:100  |     37 | bloomFactorY = VZERO - 0x2000;
  mfc0 $ra, COP0_DMA_BUSY                            ## L:98   |      ^ | RA = get_dma_busy();
  addiu $s5, $zero, 4                                ## L:99   |     38 | bloomPhase = 4;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0004            ## L:100  |    *40 | bloomFactorY = VZERO - 0x2000;
  nop                                                ## L:605  |    *42 | store_vec_u8(resB, dmemOut, -8);
  LABEL_Cmd_HDRBloom_0005:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     43 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR                         ## L:31   |     44 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a2, $a2, 320                                ## L:104  |     45 | ptrBloom += (80 * 4);
  mtc0 $s1, COP0_DMA_READ                            ## L:32   |    *47 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0003:
  addiu $a0, $a0, 1280                               ## L:108  |     48 | ptrIn += (320 * 4);
  ori $s0, $zero, %lo(SAFE_SPACE_3)                  ## L:110  |     49 | u16 tmpMem = SAFE_SPACE_3;
  addiu $s5, $s5, 65535                              ## L:106  |     50 | bloomPhase -= 1;
  vaddc $v26, $v26, $v31.e2                          ## L:115  |      ^ | bloomFactorY += 0x2000;
  LABEL_Cmd_HDRBloom_0006:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:118  |     51 | RA = get_dma_busy();
  or $fp, $zero, $s3                                 ## L:120  |     52 | dmemIn = ptrDMEM;
  addiu $k1, $fp, 1280                               ## L:121  |     53 | dmemInEnd = dmemIn + 320*4;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0006            ## L:121  |     54 | dmemInEnd = dmemIn + 320*4;
  ori $sp, $zero, %lo(BUFF_IN_B)                     ## L:119  |    *56 | dmemOut = BUFF_IN_B;
  LABEL_Cmd_HDRBloom_0007:
  luv $v23, 0, 0, $s2                                ## L:129  |     57 | bloom0 = load_vec_u8(dmemBloom, 0);
  luv $v19, 0, 320, $s2                              ## L:130  |     58 | bloom0Next = load_vec_u8(dmemBloom, 320);
  addiu $s2, $s2, 4                                  ## L:132  |     59 | dmemBloom += 4;
  luv $v21, 0, 0, $s2                                ## L:134  |     60 | bloom2 = load_vec_u8(dmemBloom, 0);
  luv $v18, 0, 320, $s2                              ## L:135  |     61 | bloom2Next = load_vec_u8(dmemBloom, 320);
  luv $v11, 0, 0, $fp                                ## L:140  |     62 | vec16 pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v13, $v19, $v23.v                           ## L:137  |      ^ | vec16 diffY_A = bloom0Next - bloom0;
  vsubc $v12, $v18, $v21.v                           ## L:138  |   **65 | vec16 diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_0008:
  luv $v10, 0, 8, $fp                                ## L:143  |      ^ | vec16 pixel1 = load_vec_u8(dmemIn, 8);
  ssv $v15, 4, -8, $sp                               ## L:151  |     66 | store(pixelOut2.z, dmemOut, -8);
  vmudh $v23, $v23, $v30.e7                          ## L:159  |      ^ | bloom0:sint = bloom0:sint * 1;
  luv $v09, 0, 16, $fp                               ## L:144  |     67 | vec16 pixel2 = load_vec_u8(dmemIn, 16);
  vmacf $v23, $v13, $v26.v                           ## L:160  |      ^ | bloom0:sfract = diffY_A:sfract +* bloomFactorY:sfract;
  luv $v08, 0, 24, $fp                               ## L:145  |     68 | vec16 pixel3 = load_vec_u8(dmemIn, 24);
  vmudm $v11, $v11, $v31.e5                          ## L:162  |      ^ | pixel0 >>= 6;
  addiu $fp, $fp, 32                                 ## L:243  |     69 | dmemIn += 32;
  vmudh $v21, $v21, $v30.e7                          ## L:164  |      ^ | bloom2:sint = bloom2:sint * 1;
  vmacf $v21, $v12, $v26.v                           ## L:165  |     70 | bloom2:sfract = diffY_B:sfract +* bloomFactorY:sfract;
  vor $v19, $v00, $v23                               ## L:171  |     71 | @Barrier("tmp_1") bloom0Next = bloom0; ## Barrier: 0x2
  sqv $v23, 0, 0, $s0                                ## L:172  |      ^ | @Barrier("tmp_0") store(bloom0, tmpMem, 0); ## Barrier: 0x4
  ldv $v19, 0, 8, $s0                                ## L:174  |     72 | @Barrier("tmp_0") bloom0Next.xyzw = load(tmpMem, 0).XYZW; ## Barrier: 0x4
  ldv $v23, 8, 0, $s0                                ## L:173  |     73 | @Barrier("tmp_1") @Barrier("tmp_0") bloom0.XYZW = load(tmpMem, 0).xyzw; ## Barrier: 0x8
  vmudm $v08, $v08, $v31.e5                          ## L:178  |      ^ | pixel3 >>= 6;
  vmudm $v10, $v10, $v31.e5                          ## L:176  |     74 | pixel1 >>= 6;
  vor $v18, $v00, $v21                               ## L:180  |     75 | @Barrier("tmp_3") bloom2Next = bloom2; ## Barrier: 0x10
  vmudm $v09, $v09, $v31.e5                          ## L:177  |     76 | pixel2 >>= 6;
  sqv $v21, 0, 16, $s0                               ## L:181  |      ^ | @Barrier("tmp_2") store(bloom2, tmpMem, 16); ## Barrier: 0x20
  ldv $v18, 0, 24, $s0                               ## L:183  |     77 | @Barrier("tmp_2") bloom2Next.xyzw = load(tmpMem, 16).XYZW; ## Barrier: 0x20
  vsubc $v07, $v19, $v23.v                           ## L:185  |      ^ | vec16 diffX_A = bloom0Next - bloom0;
  vmudh $v11, $v11, $v25.e3                          ## L:187  |     78 | pixel0:sint *= bloomFactorA.w;
  ldv $v21, 8, 16, $s0                               ## L:182  |      ^ | @Barrier("tmp_3") @Barrier("tmp_2") bloom2.XYZW = load(tmpMem, 16).xyzw; ## Barrier: 0x40
  vmudh $v10, $v10, $v25.e3                          ## L:188  |     79 | pixel1:sint *= bloomFactorA.w;
  vmudh $v22, $v23, $v30.e7                          ## L:190  |     80 | bloom1:sint = bloom0:sint * 1;
  addiu $s2, $s2, 4                                  ## L:156  |      ^ | dmemBloom += 4;
  ssv $v14, 4, -4, $sp                               ## L:153  |     81 | store(pixelOut3.z, dmemOut, -4);
  vmacf $v22, $v07, $v24.v                           ## L:191  |      ^ | bloom1:sfract = diffX_A:sfract +* bloomFactorB:sfract;
  vsubc $v06, $v18, $v21.v                           ## L:193  |     82 | vec16 diffX_B = bloom2Next - bloom2;
  vmudh $v23, $v23, $v30.e7                          ## L:195  |     83 | bloom0:sint = bloom0:sint * 1;
  ssv $v14, 12, -2, $sp                              ## L:154  |      ^ | store(pixelOut3.Z, dmemOut, -2);
  luv $v19, 0, 320, $s2                              ## L:233  |     84 | bloom0Next = load_vec_u8(dmemBloom, 320);
  vmacf $v23, $v07, $v25.v                           ## L:196  |      ^ | bloom0:sfract = diffX_A:sfract +* bloomFactorA:sfract;
  vmudh $v20, $v21, $v30.e7                          ## L:198  |     85 | bloom3:sint = bloom2:sint * 1;
  vmacf $v20, $v06, $v24.v                           ## L:199  |     86 | bloom3:sfract = diffX_B:sfract +* bloomFactorB:sfract;
  vmudh $v21, $v21, $v30.e7                          ## L:201  |     87 | bloom2:sint = bloom2:sint * 1;
  vmacf $v21, $v06, $v25.v                           ## L:202  |     88 | bloom2:sfract = diffX_B:sfract +* bloomFactorA:sfract;
  vmudh $v09, $v09, $v25.e3                          ## L:204  |     89 | pixel2:sint *= bloomFactorA.w;
  ssv $v17, 4, -16, $sp                              ## L:147  |      ^ | store(pixelOut0.z, dmemOut, -16);
  vmudh $v08, $v08, $v25.e3                          ## L:205  |     90 | pixel3:sint *= bloomFactorA.w;
  vadd $v11, $v11, $v23.v                            ## L:207  |     91 | pixel0:sint += bloom0:sint;
  ssv $v16, 12, -10, $sp                             ## L:150  |      ^ | store(pixelOut1.Z, dmemOut, -10);
  vadd $v10, $v10, $v22.v                            ## L:208  |     92 | pixel1:sint += bloom1:sint;
  luv $v23, 0, 0, $s2                                ## L:232  |      ^ | bloom0 = load_vec_u8(dmemBloom, 0);
  addiu $s2, $s2, 4                                  ## L:235  |     93 | dmemBloom += 4;
  vadd $v09, $v09, $v21.v                            ## L:209  |      ^ | pixel2:sint += bloom2:sint;
  luv $v21, 0, 0, $s2                                ## L:237  |     94 | bloom2 = load_vec_u8(dmemBloom, 0);
  vadd $v08, $v08, $v20.v                            ## L:210  |      ^ | pixel3:sint += bloom3:sint;
  vmudm $v09, $v09, $v30.e1                          ## L:214  |   **97 | pixel2 >>= 10;
  vmudm $v08, $v08, $v30.e1                          ## L:215  |     98 | pixel3 >>= 10;
  ssv $v15, 12, -6, $sp                              ## L:152  |      ^ | store(pixelOut2.Z, dmemOut, -6);
  ssv $v16, 4, -12, $sp                              ## L:149  |     99 | store(pixelOut1.z, dmemOut, -12);
  vmudm $v11, $v11, $v30.e1                          ## L:212  |      ^ | pixel0 >>= 10;
  vmudm $v10, $v10, $v30.e1                          ## L:213  |    100 | pixel1 >>= 10;
  vmudn $v15, $v09, $v30.e6                          ## L:53   |    101 | out = in * 2;
  vmadn $v15, $v27, $v09.h1                          ## L:54   |    102 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v15, $v28, $v09.h0                          ## L:55   |    103 | out = VCONST_R +* in.xxxxXXXX;
  vmudn $v14, $v08, $v30.e6                          ## L:53   |    104 | out = in * 2;
  ssv $v17, 12, -14, $sp                             ## L:148  |      ^ | store(pixelOut0.Z, dmemOut, -14);
  vmadn $v14, $v27, $v08.h1                          ## L:54   |    105 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v14, $v28, $v08.h0                          ## L:55   |    106 | out = VCONST_R +* in.xxxxXXXX;
  vmudn $v17, $v11, $v30.e6                          ## L:53   |    107 | out = in * 2;
  vmadn $v17, $v27, $v11.h1                          ## L:54   |    108 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v17, $v28, $v11.h0                          ## L:55   |    109 | out = VCONST_R +* in.xxxxXXXX;
  addiu $sp, $sp, 16                                 ## L:244  |      ^ | dmemOut += 16;
  vmudn $v16, $v10, $v30.e6                          ## L:53   |    110 | out = in * 2;
  luv $v18, 0, 320, $s2                              ## L:238  |      ^ | bloom2Next = load_vec_u8(dmemBloom, 320);
  luv $v11, 0, 0, $fp                                ## L:246  |    111 | pixel0 = load_vec_u8(dmemIn, 0);
  vmadn $v16, $v27, $v10.h1                          ## L:54   |      ^ | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v16, $v28, $v10.h0                          ## L:55   |    112 | out = VCONST_R +* in.xxxxXXXX;
  vsubc $v13, $v19, $v23.v                           ## L:240  |    113 | diffY_A = bloom0Next - bloom0;
  bne $fp, $k1, LABEL_Cmd_HDRBloom_0008              ## L:246  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v12, $v18, $v21.v                           ## L:241  |   *115 | diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_0009:
  ssv $v16, 12, -10, $sp                             ## L:253  |    116 | store(pixelOut1.Z, dmemOut, -10);
  slv $v00, 0, -8, $sp                               ## L:254  |    117 | store(VZERO.xy, dmemOut, -8);
  ssv $v16, 4, -12, $sp                              ## L:252  |    118 | store(pixelOut1.z, dmemOut, -12);
  addiu $s3, $sp, 64896                              ## L:257  |    119 | ptrDMEM = dmemOut - 320 * 2;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |    120 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    121 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a1, $a1, 640                                ## L:260  |    122 | ptrOut += (320 * 2);
  addiu $k0, $zero, 639                              ## L:256  |    123 | u32 dmaSize = (320 * 2) - 1;
  mtc0 $k0, COP0_DMA_WRITE                           ## L:40   |    124 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  ssv $v17, 12, -14, $sp                             ## L:251  |    125 | store(pixelOut0.Z, dmemOut, -14);
  bne $a0, $s7, LABEL_Cmd_HDRBloom_0001              ## L:260  |    126 | ptrOut += (320 * 2);
  ssv $v17, 4, -16, $sp                              ## L:250  |   *128 | store(pixelOut0.z, dmemOut, -16);
  LABEL_Cmd_HDRBloom_0002:
  j RSPQ_Loop                                        ## L:261  |    129 | } while(ptrIn != ptrInEnd)
  nop                                                ## L:261  |   *131 | } while(ptrIn != ptrInEnd)
Cmd_Blur:
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:295  |      ^ | u16 dmemInA = BUFF_IN_A;
  addiu $s3, $zero, 1056                             ## L:289  |      2 | u16 dmemInOffsetEnd = (80 * 4 + 32) * 3;
  or $s4, $zero, $zero                               ## L:288  |      3 | u16 dmemInOffset = 0;
  mtc2 $a2, $v28.e0                                  ## L:293  |      4 | factor.x = brightness;
  addiu $s5, $zero, 319                              ## L:287  |      5 | u16 dmaSize = (80 * 4) - 1;
  addiu $s6, $zero, 60                               ## L:285  |      6 | u32 rowsLeft = 60;
  or $s0, $zero, $s2                                 ## L:298  |      7 | u16 lastLoadedDmemIn = dmemInA;
  addiu $s7, $a0, 18880                              ## L:284  |      8 | u32 ptrRDRAMInEnd = ptrRDRAMIn + ((80 * 4) * (60 - 1));
  addiu $s1, $s2, 1056                               ## L:296  |      9 | u32 dmemOutStart = dmemInA + ((80 * 4 + 32)*3);
  LABEL_Cmd_Blur_000A:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     10 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000A                ## L:46   |   **13 | RA = get_dma_busy();
  nop                                                ## L:46   |    *15 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000B:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     16 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     17 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |   **20 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:305  |     21 | dmemInA += (80 * 4 + 32);
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |    *23 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     24 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:309  |     25 | ptrRDRAMIn += (80 * 4);
  addiu $s2, $s2, 352                                ## L:307  |     26 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |     27 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     28 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000C                ## L:46   |   **31 | RA = get_dma_busy();
  nop                                                ## L:46   |    *33 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000D:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     34 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     35 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:313  |     36 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |    *38 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000E:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:46   |     39 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000E                ## L:46   |   **42 | RA = get_dma_busy();
  nop                                                ## L:46   |    *44 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000F:
  lw $fp, 316($s2)                                   ## L:317  |     45 | u32 tmp1 = load(dmemInA, 316);
  sw $fp, 320($s2)                                   ## L:318  |   **48 | store(tmp1, dmemInA, 320);
  addiu $s2, $s2, 65184                              ## L:320  |     49 | dmemInA -= (80 * 4 + 32);
  lw $sp, 316($s2)                                   ## L:321  |     50 | u32 tmp2 = load(dmemInA, 316);
  sw $sp, 320($s2)                                   ## L:322  |   **53 | store(tmp2, dmemInA, 320);
  LABEL_Cmd_Blur_0010:
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:330  |     54 | dmemInA = BUFF_IN_A;
  addiu $s2, $s2, 65532                              ## L:331  |     55 | dmemInA -= 4;
  addiu $fp, $s2, 700                                ## L:333  |     56 | u16 dmemInC = dmemInA + (80 * 4 + 32)*2 - 4;
  LABEL_Cmd_Blur_0012:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:339  |     57 | RA = get_dma_busy();
  addiu $k0, $zero, 352                              ## L:341  |     58 | dmemInBOffset = (80 * 4 + 32);
  bne $ra, $zero, LABEL_Cmd_Blur_0012                ## L:341  |    *60 | dmemInBOffset = (80 * 4 + 32);
  or $sp, $zero, $s1                                 ## L:340  |    *62 | dmemOutCurr = dmemOutStart;
  LABEL_Cmd_Blur_0013:
  luv $v23, 0, 360, $s2                              ## L:352  |     63 | vec16 pixelA_11 = load_vec_u8(dmemInA, 360);
  luv $v26, 0, 360, $s2                              ## L:348  |     64 | vec16 pixelA_10 = load_vec_u8(dmemInA, 360);
  luv $v24, 0, 8, $s2                                ## L:351  |     65 | vec16 pixelA_01 = load_vec_u8(dmemInA, 8);
  luv $v25, 0, 8, $fp                                ## L:349  |     66 | vec16 pixelA_20 = load_vec_u8(dmemInC, 8);
  luv $v27, 0, 8, $s2                                ## L:347  |     67 | vec16 pixelA_00 = load_vec_u8(dmemInA, 8);
  lw $t9, 316($s0)                                   ## L:355  |     68 | u32 tmp1 = load(lastLoadedDmemIn, 316);
  sw $t9, 320($s0)                                   ## L:356  |   **71 | store(tmp1, lastLoadedDmemIn, 320);
  LABEL_Cmd_Blur_0014:
  vmulu $v15, $v27, $v28.e0                          ## L:273  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v22, 0, 8, $fp                                ## L:363  |     72 | vec16 pixelA_21 = load_vec_u8(dmemInC, 8);
  vmacu $v15, $v26, $v28.e0                          ## L:274  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v19, 0, 16, $fp                               ## L:367  |     73 | vec16 pixelB_21 = load_vec_u8(dmemInC, 16);
  vmacu $v15, $v25, $v28.e0                          ## L:275  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v15, $v24, $v28.e0                          ## L:277  |     74 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v21, 0, 16, $s2                               ## L:365  |      ^ | vec16 pixelB_01 = load_vec_u8(dmemInA, 16);
  addu $t9, $s2, $k0                                 ## L:360  |     75 | u16 dmemInB = dmemInA + dmemInBOffset;
  vmacu $v15, $v23, $v28.e0                          ## L:278  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v15, $v22, $v28.e0                          ## L:279  |     76 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v26, 0, 64, $t9                               ## L:400  |      ^ | pixelA_10 = load_vec_u8(dmemInB, 64);
  vmulu $v14, $v24, $v28.e0                          ## L:273  |     77 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v20, 0, 16, $t9                               ## L:366  |      ^ | vec16 pixelB_11 = load_vec_u8(dmemInB, 16);
  vmacu $v14, $v23, $v28.e0                          ## L:274  |     78 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v06, 0, 32, $s2                               ## L:379  |      ^ | vec16 pixelD_01 = load_vec_u8(dmemInA, 32);
  vmacu $v14, $v22, $v28.e0                          ## L:275  |     79 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v05, 0, 32, $t9                               ## L:380  |      ^ | vec16 pixelD_11 = load_vec_u8(dmemInB, 32);
  vmacu $v14, $v21, $v28.e0                          ## L:277  |     80 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v08, 0, 24, $t9                               ## L:376  |      ^ | vec16 pixelC_11 = load_vec_u8(dmemInB, 24);
  luv $v04, 0, 32, $fp                               ## L:381  |     81 | vec16 pixelD_21 = load_vec_u8(dmemInC, 32);
  vmacu $v14, $v20, $v28.e0                          ## L:278  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v14, $v19, $v28.e0                          ## L:279  |     82 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v09, 0, 24, $s2                               ## L:375  |      ^ | vec16 pixelC_01 = load_vec_u8(dmemInA, 24);
  vmulu $v13, $v21, $v28.e0                          ## L:273  |     83 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v07, 0, 24, $fp                               ## L:377  |      ^ | vec16 pixelC_21 = load_vec_u8(dmemInC, 24);
  addiu $a1, $a1, 64                                 ## L:425  |     84 | ptrRDRAMOut += 64;
  vmacu $v13, $v20, $v28.e0                          ## L:274  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v13, $v19, $v28.e0                          ## L:275  |     85 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v01, 0, 40, $fp                               ## L:385  |      ^ | vec16 pixelE_21 = load_vec_u8(dmemInC, 40);
  luv $v03, 0, 40, $s2                               ## L:383  |     86 | vec16 pixelE_01 = load_vec_u8(dmemInA, 40);
  vmacu $v13, $v09, $v28.e0                          ## L:277  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v13, $v08, $v28.e0                          ## L:278  |     87 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v27, 0, 64, $s2                               ## L:399  |      ^ | pixelA_00 = load_vec_u8(dmemInA, 64);
  luv $v02, 0, 40, $t9                               ## L:384  |     88 | vec16 pixelE_11 = load_vec_u8(dmemInB, 40);
  vmacu $v13, $v07, $v28.e0                          ## L:279  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v17, 0, 48, $t9                               ## L:370  |     89 | vec16 pixelF_11 = load_vec_u8(dmemInB, 48);
  vmulu $v12, $v09, $v28.e0                          ## L:273  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v12, $v08, $v28.e0                          ## L:274  |     90 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v09, 0, 56, $s2                               ## L:395  |      ^ | vec16 pixelG_01 = load_vec_u8(dmemInA, 56);
  vmacu $v12, $v07, $v28.e0                          ## L:275  |     91 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v12, $v06, $v28.e0                          ## L:277  |     92 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v07, 0, 56, $fp                               ## L:397  |      ^ | vec16 pixelG_21 = load_vec_u8(dmemInC, 56);
  vmacu $v12, $v05, $v28.e0                          ## L:278  |     93 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v12, $v04, $v28.e0                          ## L:279  |     94 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v18, 0, 48, $s2                               ## L:369  |      ^ | vec16 pixelF_01 = load_vec_u8(dmemInA, 48);
  addiu $s2, $s2, 64                                 ## L:403  |     95 | dmemInA += 64;
  vmulu $v11, $v06, $v28.e0                          ## L:273  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v11, $v05, $v28.e0                          ## L:274  |     96 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v16, 0, 48, $fp                               ## L:371  |      ^ | vec16 pixelF_21 = load_vec_u8(dmemInC, 48);
  vmacu $v11, $v04, $v28.e0                          ## L:275  |     97 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v11, $v03, $v28.e0                          ## L:277  |     98 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v11, $v02, $v28.e0                          ## L:278  |     99 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  addiu $sp, $sp, 64                                 ## L:410  |      ^ | dmemOutCurr += 64;
  vmacu $v11, $v01, $v28.e0                          ## L:279  |    100 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  suv $v14, 0, -56, $sp                              ## L:413  |      ^ | store_vec_u8(sum1, dmemOutCurr, -56);
  vmulu $v10, $v03, $v28.e0                          ## L:273  |    101 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v10, $v02, $v28.e0                          ## L:274  |    102 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  suv $v15, 0, -64, $sp                              ## L:412  |      ^ | store_vec_u8(sum0, dmemOutCurr, -64);
  vmacu $v10, $v01, $v28.e0                          ## L:275  |    103 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v08, 0, 56, $t9                               ## L:396  |      ^ | vec16 pixelG_11 = load_vec_u8(dmemInB, 56);
  vmacu $v10, $v18, $v28.e0                          ## L:277  |    104 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v10, $v17, $v28.e0                          ## L:278  |    105 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v10, $v16, $v28.e0                          ## L:279  |    106 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v25, 0, 64, $fp                               ## L:401  |      ^ | pixelA_20 = load_vec_u8(dmemInC, 64);
  luv $v24, 0, 8, $s2                                ## L:421  |    107 | pixelA_01 = load_vec_u8(dmemInA, 8);
  vmulu $v06, $v18, $v28.e0                          ## L:273  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v06, $v17, $v28.e0                          ## L:274  |    108 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v06, $v16, $v28.e0                          ## L:275  |    109 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  addiu $t9, $t9, 64                                 ## L:404  |      ^ | dmemInB += 64;
  luv $v23, 0, 8, $t9                                ## L:422  |    110 | pixelA_11 = load_vec_u8(dmemInB, 8);
  vmacu $v06, $v09, $v28.e0                          ## L:277  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v06, $v08, $v28.e0                          ## L:278  |    111 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v11, 0, -32, $sp                              ## L:416  |      ^ | store_vec_u8(sum4, dmemOutCurr, -32);
  vmacu $v06, $v07, $v28.e0                          ## L:279  |    112 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v05, $v09, $v28.e0                          ## L:273  |    113 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  addiu $fp, $fp, 64                                 ## L:405  |      ^ | dmemInC += 64;
  vmacu $v05, $v08, $v28.e0                          ## L:274  |    114 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  suv $v13, 0, -48, $sp                              ## L:414  |      ^ | store_vec_u8(sum2, dmemOutCurr, -48);
  vmacu $v05, $v07, $v28.e0                          ## L:275  |    115 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  xori $k0, $k0, 4                                   ## L:361  |      ^ | dmemInBOffset ^= 4;
  suv $v10, 0, -24, $sp                              ## L:417  |    116 | store_vec_u8(sum5, dmemOutCurr, -24);
  vmacu $v05, $v27, $v28.e0                          ## L:277  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  suv $v06, 0, -16, $sp                              ## L:418  |    117 | store_vec_u8(sum6, dmemOutCurr, -16);
  vmacu $v05, $v26, $v28.e0                          ## L:278  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v12, 0, -40, $sp                              ## L:415  |    118 | store_vec_u8(sum3, dmemOutCurr, -40);
  vmacu $v05, $v25, $v28.e0                          ## L:279  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  addiu $k1, $s1, 320                                ## L:424  |    119 | dmemOutEnd = dmemOutStart + (80 * 4);
  bne $sp, $k1, LABEL_Cmd_Blur_0014                  ## L:425  |    120 | ptrRDRAMOut += 64;
  suv $v05, 0, -8, $sp                               ## L:419  |  **123 | store_vec_u8(sum7, dmemOutCurr, -8);
  LABEL_Cmd_Blur_0015:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:38   |    124 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    125 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |  **128 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0016:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:433  |    129 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0016                ## L:434  |  **132 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  addiu $s0, $s4, %lo(BUFF_IN_A)                     ## L:434  |   *134 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  LABEL_Cmd_Blur_0017:
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:30   |    135 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |    136 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $s4, 352                                ## L:439  |    137 | dmemInOffset += (80 * 4 + 32);
  beq $a0, $s7, LABEL_Cmd_Blur_0018                  ## L:441  |    138 | if(ptrRDRAMIn != ptrRDRAMInEnd) {
  mtc0 $s5, COP0_DMA_READ                            ## L:32   |   *140 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:442  |    141 | ptrRDRAMIn += (80 * 4);
  LABEL_Cmd_Blur_0018:
  bne $s4, $s3, LABEL_Cmd_Blur_0019                  ## L:445  |    142 | if(dmemInOffset == dmemInOffsetEnd) {
  nop                                                ## L:445  |   *144 | if(dmemInOffset == dmemInOffsetEnd) {
  or $s4, $zero, $zero                               ## L:446  |    145 | dmemInOffset = 0;
  LABEL_Cmd_Blur_0019:
  addiu $s6, $s6, 65535                              ## L:449  |    146 | rowsLeft -= 1;
  bne $s6, $zero, LABEL_Cmd_Blur_0010                ## L:449  |    147 | rowsLeft -= 1;
  nop                                                ## L:449  |   *149 | rowsLeft -= 1;
  LABEL_Cmd_Blur_0011:
  j RSPQ_Loop                                        ## L:450  |    150 | 
  nop                                                ## L:450  |   *152 | 
Cmd_Downscale:
  ori $s3, $zero, %lo(BUFF_BLOOM_B)                  ## L:492  |      ^ | u16 dmemOutB = BUFF_BLOOM_B;
  addiu $s5, $zero, 319                              ## L:484  |      2 | u16 dmaSizeOut = (80 * 4) - 1;
  lui $at, 0x04                                      ## L:475  |      3 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  lui $s6, 0x5000                                    ## L:479  |      4 | u16 dmaSizeIn = (
  addiu $a1, $a1, 65216                              ## L:477  |      5 | prtRDRAMOut -= (80 * 4);
  ori $s6, $s6, 0x14FF                               ## L:479  |      6 | u16 dmaSizeIn = (
  mtc2 $a2, $v28.e1                                  ## L:489  |      7 | sumFactor.y = threshold;
  ori $at, $at, 0xB000                               ## L:475  |      8 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  ori $s4, $zero, %lo(BUFF_BLOOM)                    ## L:491  |      9 | u16 dmemOutA = BUFF_BLOOM;
  addu $s7, $a0, $at                                 ## L:475  |     10 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addiu $at, $zero, 4096                             ## L:487  |     11 | sumFactor:sfract.x = 0.125;
  mtc2 $at, $v28.e0                                  ## L:487  |     12 | sumFactor:sfract.x = 0.125;
  LABEL_Cmd_Downscale_001A:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:500  |     13 | RA = get_dma_busy();
  ori $s1, $zero, %lo(BUFF_IN_A)                     ## L:501  |     14 | dmemInA = BUFF_IN_A;
  ori $s0, $zero, %lo(BUFF_IN_B)                     ## L:502  |     15 | dmemInB = BUFF_IN_B;
  bne $ra, $zero, LABEL_Cmd_Downscale_001A           ## L:502  |     16 | dmemInB = BUFF_IN_B;
  nop                                                ## L:603  |    *18 | resA:sint = p0_AB:sint + p1_AB:sint;
  LABEL_Cmd_Downscale_001B:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:30   |     19 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     20 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 5120                               ## L:508  |     21 | ptrRDRAMIn += (320 * 4) * 4;
  mtc0 $s6, COP0_DMA_READ                            ## L:32   |    *23 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Downscale_001C:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:511  |     24 | RA = get_dma_busy();
  or $s2, $zero, $s4                                 ## L:513  |     25 | dmemOut = dmemOutA;
  bne $ra, $zero, LABEL_Cmd_Downscale_001C           ## L:514  |    *27 | dmemOutEnd = dmemOut + (80 * 4);
  addiu $fp, $s2, 320                                ## L:514  |    *29 | dmemOutEnd = dmemOut + (80 * 4);
  LABEL_Cmd_Downscale_001D:
  addiu $s1, $s1, 1280                               ## L:522  |     30 | dmemInA += 1280;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     31 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     32 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s0, $s0, 1280                               ## L:523  |     33 | dmemInB += 1280;
  xor $s4, $s4, $s3                                  ## L:518  |     34 | swap(dmemOutA, dmemOutB);
  xor $s3, $s4, $s3                                  ## L:518  |     35 | swap(dmemOutA, dmemOutB);
  lqv $v27, 0, -32, $s1                              ## L:525  |     36 | vec16 a = load(dmemInA, -32);
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |     37 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  xor $s4, $s4, $s3                                  ## L:518  |     38 | swap(dmemOutA, dmemOutB);
  addiu $s0, $s0, 64256                              ## L:529  |     39 | dmemInB -= 1280;
  sqv $v27, 0, -16, $s1                              ## L:526  |     40 | store(a, dmemInA, -16);
  addiu $s1, $s1, 64256                              ## L:528  |     41 | dmemInA -= 1280;
  LABEL_Cmd_Downscale_001E:
  vadd $v27, $v25, $v21.v                            ## L:539  |      ^ | resA:sint = p0_AB:sint + p1_AB:sint;
  luv $v25, 0, 0, $s1                                ## L:554  |     42 | p0_AB = load_vec_u8(dmemInA, 0);
  luv $v24, 0, 8, $s1                                ## L:555  |     43 | p0_CD = load_vec_u8(dmemInA, 8);
  vadd $v26, $v17, $v13.v                            ## L:540  |      ^ | resB:sint = p2_AB:sint + p3_AB:sint;
  luv $v17, 0, 32, $s1                               ## L:564  |     44 | p2_AB = load_vec_u8(dmemInA, 32);
  luv $v22, 0, 8, $s0                                ## L:557  |     45 | p0_GH = load_vec_u8(dmemInB, 8);
  vlt $v29, $v27, $v28.e1                            ## L:543  |      ^ | resA = resA < sumFactor.y ? VZERO : resA;
  vmrg $v27, $v00, $v27                              ## L:543  |     46 | resA = resA < sumFactor.y ? VZERO : resA;
  luv $v13, 0, 48, $s1                               ## L:569  |      ^ | p3_AB = load_vec_u8(dmemInA, 48);
  luv $v23, 0, 0, $s0                                ## L:556  |     47 | p0_EF = load_vec_u8(dmemInB, 0);
  vlt $v29, $v26, $v28.e1                            ## L:544  |      ^ | resB = resB < sumFactor.y ? VZERO : resB;
  vmrg $v26, $v00, $v26                              ## L:544  |     48 | resB = resB < sumFactor.y ? VZERO : resB;
  luv $v21, 0, 16, $s1                               ## L:559  |      ^ | p1_AB = load_vec_u8(dmemInA, 16);
  vmulf $v25, $v25, $v28.e0                          ## L:460  |     49 | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v19, 0, 16, $s0                               ## L:561  |      ^ | p1_EF = load_vec_u8(dmemInB, 16);
  luv $v20, 0, 24, $s1                               ## L:560  |     50 | p1_CD = load_vec_u8(dmemInA, 24);
  vmacf $v25, $v24, $v28.e0                          ## L:461  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v18, 0, 24, $s0                               ## L:562  |     51 | p1_GH = load_vec_u8(dmemInB, 24);
  vmacf $v25, $v23, $v28.e0                          ## L:462  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v16, 0, 40, $s1                               ## L:565  |     52 | p2_CD = load_vec_u8(dmemInA, 40);
  vmacf $v25, $v22, $v28.e0                          ## L:463  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v12, 0, 56, $s1                               ## L:570  |     53 | p3_CD = load_vec_u8(dmemInA, 56);
  vmulf $v21, $v21, $v28.e0                          ## L:460  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v21, $v20, $v28.e0                          ## L:461  |     54 | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v15, 0, 32, $s0                               ## L:566  |      ^ | p2_EF = load_vec_u8(dmemInB, 32);
  luv $v14, 0, 40, $s0                               ## L:567  |     55 | p2_GH = load_vec_u8(dmemInB, 40);
  vmacf $v21, $v19, $v28.e0                          ## L:462  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  vmacf $v21, $v18, $v28.e0                          ## L:463  |     56 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v10, 0, 56, $s0                               ## L:572  |      ^ | p3_GH = load_vec_u8(dmemInB, 56);
  luv $v11, 0, 48, $s0                               ## L:571  |     57 | p3_EF = load_vec_u8(dmemInB, 48);
  vmulf $v17, $v17, $v28.e0                          ## L:460  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  vmacf $v17, $v16, $v28.e0                          ## L:461  |     58 | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v17, $v15, $v28.e0                          ## L:462  |     59 | a:sfract = c:sfract +* sumFactor:sfract.x;
  addiu $s0, $s0, 64                                 ## L:598  |      ^ | dmemInB += 64;
  vmacf $v17, $v14, $v28.e0                          ## L:463  |     60 | a:sfract = d:sfract +* sumFactor:sfract.x;
  suv $v27, 0, -16, $s2                              ## L:546  |      ^ | store_vec_u8(resA, dmemOut, -16);
  vmulf $v13, $v13, $v28.e0                          ## L:460  |     61 | a:sfract = a:sfract * sumFactor:sfract.x;
  addiu $a1, $a1, 16                                 ## L:599  |      ^ | prtRDRAMOut += 16;
  suv $v26, 0, -8, $s2                               ## L:547  |     62 | store_vec_u8(resB, dmemOut, -8);
  vmacf $v13, $v12, $v28.e0                          ## L:461  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v13, $v11, $v28.e0                          ## L:462  |     63 | a:sfract = c:sfract +* sumFactor:sfract.x;
  vmacf $v13, $v10, $v28.e0                          ## L:463  |     64 | a:sfract = d:sfract +* sumFactor:sfract.x;
  sdv $v17, 8, 8, $s1                                ## L:590  |      ^ | @Barrier("tmp1") store(p2_AB.XYZW, dmemInA, 8); ## Barrier: 0x4
  sdv $v25, 8, 0, $s1                                ## L:584  |     65 | @Barrier("tmp0") store(p0_AB.XYZW, dmemInA, 0); ## Barrier: 0x2
  vmov $v25.e4, $v21.e0                              ## L:585  |      ^ | @Barrier("tmp0") p0_AB.X = p1_AB.x; ## Barrier: 0x2
  vmov $v25.e5, $v21.e1                              ## L:586  |     66 | @Barrier("tmp0") p0_AB.Y = p1_AB.y; ## Barrier: 0x2
  vmov $v25.e6, $v21.e2                              ## L:587  |     67 | @Barrier("tmp0") p0_AB.Z = p1_AB.z; ## Barrier: 0x2
  ldv $v21, 0, 0, $s1                                ## L:588  |      ^ | @Barrier("tmp0") p1_AB.xyzw = load(dmemInA, 0).xyzw; ## Barrier: 0x2
  addiu $s2, $s2, 16                                 ## L:596  |     68 | dmemOut += 16;
  vmov $v17.e4, $v13.e0                              ## L:591  |      ^ | @Barrier("tmp1") p2_AB.X = p3_AB.x; ## Barrier: 0x4
  vmov $v17.e5, $v13.e1                              ## L:592  |     69 | @Barrier("tmp1") p2_AB.Y = p3_AB.y; ## Barrier: 0x4
  vmov $v17.e6, $v13.e2                              ## L:593  |     70 | @Barrier("tmp1") p2_AB.Z = p3_AB.z; ## Barrier: 0x4
  ldv $v13, 0, 8, $s1                                ## L:594  |      ^ | @Barrier("tmp1") p3_AB.xyzw = load(dmemInA, 8).xyzw; ## Barrier: 0x4
  bne $s2, $fp, LABEL_Cmd_Downscale_001E             ## L:599  |     71 | prtRDRAMOut += 16;
  addiu $s1, $s1, 64                                 ## L:597  |    *73 | dmemInA += 64;
  LABEL_Cmd_Downscale_001F:
  vadd $v27, $v25, $v21.v                            ## L:603  |     74 | resA:sint = p0_AB:sint + p1_AB:sint;
  vadd $v26, $v17, $v13.v                            ## L:604  |     75 | resB:sint = p2_AB:sint + p3_AB:sint;
  vlt $v29, $v27, $v28.e1                            ## L:606  |   **78 | resA = resA < sumFactor.y ? VZERO : resA;
  vmrg $v27, $v00, $v27                              ## L:606  |     79 | resA = resA < sumFactor.y ? VZERO : resA;
  vlt $v29, $v26, $v28.e1                            ## L:607  |     80 | resB = resB < sumFactor.y ? VZERO : resB;
  vmrg $v26, $v00, $v26                              ## L:607  |     81 | resB = resB < sumFactor.y ? VZERO : resB;
  suv $v26, 0, -8, $s2                               ## L:610  |  ***85 | store_vec_u8(resB, dmemOut, -8);
  bne $a0, $s7, LABEL_Cmd_Downscale_001A             ## L:610  |     86 | store_vec_u8(resB, dmemOut, -8);
  suv $v27, 0, -16, $s2                              ## L:609  |    *88 | store_vec_u8(resA, dmemOut, -16);
  LABEL_Cmd_Downscale_0019:
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |     89 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |     90 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  j RSPQ_Loop                                        ## L:41   |     91 | }
  mtc0 $s5, COP0_DMA_WRITE                           ## L:40   |   **94 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1

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