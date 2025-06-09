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
  vxor $v28, $v00, $v31.e4                           ## L:60   |      ^ | vec16 VCONST_R = 1 << 11;
  jal DMAWaitIdle                                    ## L:63   |      ^ | dma_await();
  vxor $v27, $v00, $v30.e1                           ## L:61   |     *3 | vec16 VCONST_G = 1 << 6;
  lui $at, 0x04                                      ## L:65   |      4 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  vxor $v25, $v00, $v00.e0                           ## L:72   |      ^ | vec16 bloomFactorA = 0;
  vxor $v26, $v00, $v00.e0                           ## L:71   |      5 | vec16 bloomFactorY = 0;
  ori $at, $at, 0xB000                               ## L:65   |      ^ | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:65   |      6 | u32 ptrInEnd = ptrIn + (320 * 240 * 4);
  addiu $at, $zero, 8192                             ## L:73   |      7 | bloomFactorA:sfract.X = 0.25;
  mtc2 $at, $v25.e4                                  ## L:73   |      8 | bloomFactorA:sfract.X = 0.25;
  addiu $at, $zero, 16384                            ## L:80   |      9 | bloomFactorB:sfract.x = 0.5;
  mtc2 $at, $v24.e0                                  ## L:80   |    *11 | bloomFactorB:sfract.x = 0.5;
  addiu $at, $zero, 24576                            ## L:83   |     12 | bloomFactorB:sfract.X = 0.75;
  mtc2 $a3, $v25.e3                                  ## L:77   |   **15 | bloomFactorA.w = factor;
  vmov $v24.e2, $v24.e0                              ## L:82   |      ^ | bloomFactorB:sfract.z = bloomFactorB:sfract.x;
  mtc2 $at, $v24.e4                                  ## L:83   |     16 | bloomFactorB:sfract.X = 0.75;
  vmov $v24.e1, $v24.e0                              ## L:81   |  ***20 | bloomFactorB:sfract.y = bloomFactorB:sfract.x;
  or $s5, $zero, $zero                               ## L:67   |      ^ | u8 bloomPhase = 0;
  vmov $v25.e5, $v25.e4                              ## L:74   |     21 | bloomFactorA:sfract.Y = bloomFactorA:sfract.X;
  vmov $v24.e5, $v24.e4                              ## L:84   |   **24 | bloomFactorB:sfract.Y = bloomFactorB:sfract.X;
  ori $s6, $zero, %lo(BUFF_IN_A)                     ## L:66   |      ^ | u16 buffDMEM = BUFF_IN_A;
  vmov $v25.e6, $v25.e4                              ## L:75   |     25 | bloomFactorA:sfract.Z = bloomFactorA:sfract.X;
  vmov $v24.e6, $v24.e4                              ## L:85   |   **28 | bloomFactorB:sfract.Z = bloomFactorB:sfract.X;
  LABEL_Cmd_HDRBloom_0001:
  ori $s2, $zero, %lo(BUFF_BLOOM)                    ## L:93   |      ^ | u16 dmemBloom = BUFF_BLOOM;
  or $s3, $zero, $s6                                 ## L:89   |     29 | u32 ptrDMEM = buffDMEM;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:30   |     30 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:31   |     31 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $zero, 1279                             ## L:88   |     32 | u32 dmaSize = (320 * 4) - 1;
  addiu $s1, $zero, 640                              ## L:94   |     33 | u16 bloomDmaSize = (80 * 4)*2;
  bne $s5, $zero, LABEL_Cmd_HDRBloom_0003            ## L:96   |     34 | if(bloomPhase == 0) {
  mtc0 $s4, COP0_DMA_READ                            ## L:32   |    *36 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0004:
  addiu $s5, $zero, 4                                ## L:99   |     37 | bloomPhase = 4;
  mfc0 $ra, COP0_DMA_BUSY                            ## L:98   |     38 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0004            ## L:100  |   **41 | bloomFactorY = VZERO - 0x2000;
  vsubc $v26, $v00, $v31.e2                          ## L:100  |    *43 | bloomFactorY = VZERO - 0x2000;
  LABEL_Cmd_HDRBloom_0005:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:30   |     44 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR                         ## L:31   |     45 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a2, $a2, 320                                ## L:104  |     46 | ptrBloom += (80 * 4);
  mtc0 $s1, COP0_DMA_READ                            ## L:32   |    *48 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0003:
  vaddc $v26, $v26, $v31.e2                          ## L:115  |      ^ | bloomFactorY += 0x2000;
  addiu $a0, $a0, 1280                               ## L:108  |     49 | ptrIn += (320 * 4);
  ori $s0, $zero, %lo(SAFE_SPACE_3)                  ## L:110  |     50 | u16 tmpMem = SAFE_SPACE_3;
  addiu $s5, $s5, 65535                              ## L:106  |     51 | bloomPhase -= 1;
  LABEL_Cmd_HDRBloom_0006:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:118  |     52 | RA = get_dma_busy();
  or $fp, $zero, $s3                                 ## L:120  |     53 | dmemIn = ptrDMEM;
  ori $sp, $zero, %lo(BUFF_IN_B)                     ## L:119  |     54 | dmemOut = BUFF_IN_B;
  bne $ra, $zero, LABEL_Cmd_HDRBloom_0006            ## L:121  |     55 | dmemInEnd = dmemIn + 320*4;
  addiu $k1, $fp, 1280                               ## L:121  |    *57 | dmemInEnd = dmemIn + 320*4;
  LABEL_Cmd_HDRBloom_0007:
  luv $v19, 0, 320, $s2                              ## L:128  |     58 | bloom0Next = load_vec_u8(dmemBloom, 320);
  luv $v23, 0, 0, $s2                                ## L:127  |     59 | bloom0 = load_vec_u8(dmemBloom, 0);
  addiu $s2, $s2, 4                                  ## L:130  |     60 | dmemBloom += 4;
  luv $v21, 0, 0, $s2                                ## L:132  |     61 | bloom2 = load_vec_u8(dmemBloom, 0);
  luv $v18, 0, 320, $s2                              ## L:133  |     62 | bloom2Next = load_vec_u8(dmemBloom, 320);
  luv $v15, 0, 0, $fp                                ## L:138  |     63 | vec16 pixel0 = load_vec_u8(dmemIn, 0);
  vsubc $v17, $v19, $v23.v                           ## L:135  |      ^ | vec16 diffY_A = bloom0Next - bloom0;
  vsubc $v16, $v18, $v21.v                           ## L:136  |   **66 | vec16 diffY_B = bloom2Next - bloom2;
  LABEL_Cmd_HDRBloom_0008:
  vmudh $v23, $v23, $v30.e7                          ## L:148  |     67 | bloom0:sint = bloom0:sint * 1;
  luv $v14, 0, 8, $fp                                ## L:141  |      ^ | vec16 pixel1 = load_vec_u8(dmemIn, 8);
  luv $v12, 0, 24, $fp                               ## L:143  |     68 | vec16 pixel3 = load_vec_u8(dmemIn, 24);
  vmacf $v23, $v17, $v26.v                           ## L:149  |      ^ | bloom0:sfract = diffY_A:sfract +* bloomFactorY:sfract;
  luv $v13, 0, 16, $fp                               ## L:142  |     69 | vec16 pixel2 = load_vec_u8(dmemIn, 16);
  vmudm $v15, $v15, $v31.e5                          ## L:151  |      ^ | pixel0 >>= 6;
  vmudh $v21, $v21, $v30.e7                          ## L:153  |     70 | bloom2:sint = bloom2:sint * 1;
  vmacf $v21, $v16, $v26.v                           ## L:154  |     71 | bloom2:sfract = diffY_B:sfract +* bloomFactorY:sfract;
  addiu $s2, $s2, 4                                  ## L:145  |      ^ | dmemBloom += 4;
  sqv $v23, 0, 0, $s0                                ## L:161  |     72 | @Barrier("tmp_0") store(bloom0, tmpMem, 0); ## Barrier: 0x4
  vor $v19, $v00, $v23                               ## L:160  |      ^ | @Barrier("tmp_1") bloom0Next = bloom0; ## Barrier: 0x2
  vmudh $v15, $v15, $v25.e3                          ## L:176  |     73 | pixel0:sint *= bloomFactorA.w;
  ldv $v23, 8, 0, $s0                                ## L:162  |      ^ | @Barrier("tmp_1") @Barrier("tmp_0") bloom0.XYZW = load(tmpMem, 0).xyzw; ## Barrier: 0x8
  vmudm $v14, $v14, $v31.e5                          ## L:165  |     74 | pixel1 >>= 6;
  ldv $v19, 0, 8, $s0                                ## L:163  |      ^ | @Barrier("tmp_0") bloom0Next.xyzw = load(tmpMem, 0).XYZW; ## Barrier: 0x4
  vmudm $v13, $v13, $v31.e5                          ## L:166  |     75 | pixel2 >>= 6;
  vor $v18, $v00, $v21                               ## L:169  |    *77 | @Barrier("tmp_3") bloom2Next = bloom2; ## Barrier: 0x10
  sqv $v21, 0, 16, $s0                               ## L:170  |      ^ | @Barrier("tmp_2") store(bloom2, tmpMem, 16); ## Barrier: 0x20
  ldv $v18, 0, 24, $s0                               ## L:172  |     78 | @Barrier("tmp_2") bloom2Next.xyzw = load(tmpMem, 16).XYZW; ## Barrier: 0x20
  vsubc $v11, $v19, $v23.v                           ## L:174  |      ^ | vec16 diffX_A = bloom0Next - bloom0;
  ldv $v21, 8, 16, $s0                               ## L:171  |     79 | @Barrier("tmp_3") @Barrier("tmp_2") bloom2.XYZW = load(tmpMem, 16).xyzw; ## Barrier: 0x40
  vmudm $v12, $v12, $v31.e5                          ## L:167  |      ^ | pixel3 >>= 6;
  vmudh $v14, $v14, $v25.e3                          ## L:177  |     80 | pixel1:sint *= bloomFactorA.w;
  luv $v19, 0, 320, $s2                              ## L:221  |      ^ | bloom0Next = load_vec_u8(dmemBloom, 320);
  vmudh $v22, $v23, $v30.e7                          ## L:179  |     81 | bloom1:sint = bloom0:sint * 1;
  vmacf $v22, $v11, $v24.v                           ## L:180  |     82 | bloom1:sfract = diffX_A:sfract +* bloomFactorB:sfract;
  vsubc $v10, $v18, $v21.v                           ## L:182  |     83 | vec16 diffX_B = bloom2Next - bloom2;
  vmudh $v23, $v23, $v30.e7                          ## L:184  |     84 | bloom0:sint = bloom0:sint * 1;
  vmacf $v23, $v11, $v25.v                           ## L:185  |     85 | bloom0:sfract = diffX_A:sfract +* bloomFactorA:sfract;
  vmudh $v20, $v21, $v30.e7                          ## L:187  |     86 | bloom3:sint = bloom2:sint * 1;
  vmacf $v20, $v10, $v24.v                           ## L:188  |     87 | bloom3:sfract = diffX_B:sfract +* bloomFactorB:sfract;
  vmudh $v21, $v21, $v30.e7                          ## L:190  |     88 | bloom2:sint = bloom2:sint * 1;
  vmacf $v21, $v10, $v25.v                           ## L:191  |     89 | bloom2:sfract = diffX_B:sfract +* bloomFactorA:sfract;
  addiu $fp, $fp, 32                                 ## L:231  |      ^ | dmemIn += 32;
  vmudh $v13, $v13, $v25.e3                          ## L:193  |     90 | pixel2:sint *= bloomFactorA.w;
  vmudh $v12, $v12, $v25.e3                          ## L:194  |     91 | pixel3:sint *= bloomFactorA.w;
  vadd $v15, $v15, $v23.v                            ## L:196  |     92 | pixel0:sint += bloom0:sint;
  luv $v23, 0, 0, $s2                                ## L:220  |      ^ | bloom0 = load_vec_u8(dmemBloom, 0);
  vadd $v14, $v14, $v22.v                            ## L:197  |     93 | pixel1:sint += bloom1:sint;
  addiu $s2, $s2, 4                                  ## L:223  |      ^ | dmemBloom += 4;
  vadd $v13, $v13, $v21.v                            ## L:198  |     94 | pixel2:sint += bloom2:sint;
  luv $v21, 0, 0, $s2                                ## L:225  |      ^ | bloom2 = load_vec_u8(dmemBloom, 0);
  vadd $v12, $v12, $v20.v                            ## L:199  |     95 | pixel3:sint += bloom3:sint;
  vmudm $v15, $v15, $v30.e1                          ## L:201  |     96 | pixel0 >>= 10;
  vmudm $v14, $v14, $v30.e1                          ## L:202  |     97 | pixel1 >>= 10;
  vmudm $v13, $v13, $v30.e1                          ## L:203  |     98 | pixel2 >>= 10;
  vmudm $v12, $v12, $v30.e1                          ## L:204  |     99 | pixel3 >>= 10;
  vmudn $v09, $v15, $v30.e6                          ## L:53   |    100 | out = in * 2;
  luv $v18, 0, 320, $s2                              ## L:226  |      ^ | bloom2Next = load_vec_u8(dmemBloom, 320);
  vmadn $v09, $v27, $v15.h1                          ## L:54   |    101 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v09, $v28, $v15.h0                          ## L:55   |    102 | out = VCONST_R +* in.xxxxXXXX;
  vmudn $v08, $v14, $v30.e6                          ## L:53   |    103 | out = in * 2;
  luv $v15, 0, 0, $fp                                ## L:234  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  vmadn $v08, $v27, $v14.h1                          ## L:54   |    104 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v08, $v28, $v14.h0                          ## L:55   |    105 | out = VCONST_R +* in.xxxxXXXX;
  vmudn $v07, $v13, $v30.e6                          ## L:53   |    106 | out = in * 2;
  vmadn $v07, $v27, $v13.h1                          ## L:54   |    107 | out = VCONST_G +* in.yyyyYYYY;
  vmadn $v07, $v28, $v13.h0                          ## L:55   |    108 | out = VCONST_R +* in.xxxxXXXX;
  vmudn $v06, $v12, $v30.e6                          ## L:53   |    109 | out = in * 2;
  ssv $v08, 4, 4, $sp                                ## L:213  |      ^ | store(pixelOut1.z, dmemOut, 4);
  vmadn $v06, $v27, $v12.h1                          ## L:54   |    110 | out = VCONST_G +* in.yyyyYYYY;
  ssv $v09, 12, 2, $sp                               ## L:212  |      ^ | store(pixelOut0.Z, dmemOut, 2);
  ssv $v09, 4, 0, $sp                                ## L:211  |    111 | store(pixelOut0.z, dmemOut, 0);
  vmadn $v06, $v28, $v12.h0                          ## L:55   |      ^ | out = VCONST_R +* in.xxxxXXXX;
  ssv $v07, 4, 8, $sp                                ## L:215  |    112 | store(pixelOut2.z, dmemOut, 8);
  ssv $v07, 12, 10, $sp                              ## L:216  |    113 | store(pixelOut2.Z, dmemOut, 10);
  ssv $v08, 12, 6, $sp                               ## L:214  |    114 | store(pixelOut1.Z, dmemOut, 6);
  ssv $v06, 12, 14, $sp                              ## L:218  |    115 | store(pixelOut3.Z, dmemOut, 14);
  ssv $v06, 4, 12, $sp                               ## L:217  |    116 | store(pixelOut3.z, dmemOut, 12);
  vsubc $v17, $v19, $v23.v                           ## L:228  |      ^ | diffY_A = bloom0Next - bloom0;
  vsubc $v16, $v18, $v21.v                           ## L:229  |    117 | diffY_B = bloom2Next - bloom2;
  bne $fp, $k1, LABEL_Cmd_HDRBloom_0008              ## L:234  |      ^ | pixel0 = load_vec_u8(dmemIn, 0);
  addiu $sp, $sp, 16                                 ## L:232  |   *119 | dmemOut += 16;
  LABEL_Cmd_HDRBloom_0009:
  addiu $s3, $sp, 64896                              ## L:239  |    120 | ptrDMEM = dmemOut - 320 * 2;
  addiu $k0, $zero, 639                              ## L:238  |    121 | u32 dmaSize = (320 * 2) - 1;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:38   |    122 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:39   |    123 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a1, $a1, 640                                ## L:242  |    124 | ptrOut += (320 * 2);
  bne $a0, $s7, LABEL_Cmd_HDRBloom_0001              ## L:242  |    125 | ptrOut += (320 * 2);
  mtc0 $k0, COP0_DMA_WRITE                           ## L:40   |   *127 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  LABEL_Cmd_HDRBloom_0002:
  j RSPQ_Loop                                        ## L:243  |    128 | } while(ptrIn != ptrInEnd)
  nop                                                ## L:243  |   *130 | } while(ptrIn != ptrInEnd)
Cmd_Blur:
  addiu $s3, $zero, 1056                             ## L:252  |      ^ | u16 dmemInOffsetEnd = (80 * 4 + 32) * 3;
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:258  |      2 | u16 dmemInA = BUFF_IN_A;
  or $s0, $zero, $s2                                 ## L:261  |      3 | u16 lastLoadedDmemIn = dmemInA;
  addiu $s1, $s2, 1056                               ## L:259  |      4 | u32 dmemOutStart = dmemInA + ((80 * 4 + 32)*3);
  or $s4, $zero, $zero                               ## L:251  |      5 | u16 dmemInOffset = 0;
  addiu $s6, $zero, 60                               ## L:248  |      6 | u32 rowsLeft = 60;
  addiu $s7, $a0, 18880                              ## L:247  |      7 | u32 ptrRDRAMInEnd = ptrRDRAMIn + ((80 * 4) * (60 - 1));
  addiu $s5, $zero, 319                              ## L:250  |      8 | u16 dmaSize = (80 * 4) - 1;
  mtc2 $a2, $v28.e0                                  ## L:256  |      9 | factor.x = brightness;
  LABEL_Cmd_Blur_000D:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:54   |     10 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000D                ## L:54   |   **13 | RA = get_dma_busy();
  nop                                                ## L:54   |    *15 | RA = get_dma_busy();
  LABEL_Cmd_Blur_000E:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:38   |     16 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:39   |     17 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:268  |     18 | dmemInA += (80 * 4 + 32);
  mtc0 $s5, COP0_DMA_READ                            ## L:40   |    *20 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:38   |     21 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:39   |   **24 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s2, $s2, 352                                ## L:270  |     25 | dmemInA += (80 * 4 + 32);
  addiu $a0, $a0, 320                                ## L:272  |     26 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:40   |     27 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_000F:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:54   |     28 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_000F                ## L:54   |   **31 | RA = get_dma_busy();
  nop                                                ## L:54   |    *33 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0010:
  mtc0 $s2, COP0_DMA_SPADDR                          ## L:38   |     34 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:39   |     35 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:276  |     36 | ptrRDRAMIn += (80 * 4);
  mtc0 $s5, COP0_DMA_READ                            ## L:40   |    *38 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Blur_0011:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:54   |     39 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0011                ## L:54   |   **42 | RA = get_dma_busy();
  nop                                                ## L:54   |    *44 | RA = get_dma_busy();
  LABEL_Cmd_Blur_0012:
  lw $fp, 316($s2)                                   ## L:280  |     45 | u32 tmp1 = load(dmemInA, 316);
  sw $fp, 320($s2)                                   ## L:281  |   **48 | store(tmp1, dmemInA, 320);
  addiu $s2, $s2, 65184                              ## L:283  |     49 | dmemInA -= (80 * 4 + 32);
  lw $sp, 316($s2)                                   ## L:284  |     50 | u32 tmp2 = load(dmemInA, 316);
  sw $sp, 320($s2)                                   ## L:285  |   **53 | store(tmp2, dmemInA, 320);
  LABEL_Cmd_Blur_0013:
  ori $s2, $zero, %lo(BUFF_IN_A)                     ## L:293  |     54 | dmemInA = BUFF_IN_A;
  addiu $s2, $s2, 65532                              ## L:294  |     55 | dmemInA -= 4;
  addiu $fp, $s2, 704                                ## L:296  |     56 | u16 dmemInC = dmemInA + (80 * 4 + 32)*2;
  LABEL_Cmd_Blur_0015:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:301  |     57 | RA = get_dma_busy();
  bne $ra, $zero, LABEL_Cmd_Blur_0015                ## L:302  |   **60 | dmemOutCurr = dmemOutStart;
  or $sp, $zero, $s1                                 ## L:302  |    *62 | dmemOutCurr = dmemOutStart;
  LABEL_Cmd_Blur_0016:
  luv $v27, 0, 8, $s2                                ## L:308  |     63 | vec16 pixelA_00 = load_vec_u8(dmemInA, 8);
  luv $v24, 0, 8, $s2                                ## L:312  |     64 | vec16 pixelA_01 = load_vec_u8(dmemInA, 8);
  luv $v26, 0, 360, $s2                              ## L:309  |     65 | vec16 pixelA_10 = load_vec_u8(dmemInA, 360);
  luv $v23, 0, 360, $s2                              ## L:313  |     66 | vec16 pixelA_11 = load_vec_u8(dmemInA, 360);
  luv $v25, 0, 8, $fp                                ## L:310  |     67 | vec16 pixelA_20 = load_vec_u8(dmemInC, 8);
  lw $k0, 316($s0)                                   ## L:316  |     68 | u32 tmp1 = load(lastLoadedDmemIn, 316);
  sw $k0, 320($s0)                                   ## L:317  |   **71 | store(tmp1, lastLoadedDmemIn, 320);
  LABEL_Cmd_Blur_0017:
  vmulu $v15, $v27, $v28.e0                          ## L:236  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v22, 0, 8, $fp                                ## L:323  |     72 | vec16 pixelA_21 = load_vec_u8(dmemInC, 8);
  vmacu $v15, $v26, $v28.e0                          ## L:237  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v04, 0, 32, $fp                               ## L:341  |     73 | vec16 pixelD_21 = load_vec_u8(dmemInC, 32);
  vmacu $v15, $v25, $v28.e0                          ## L:238  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v15, $v24, $v28.e0                          ## L:240  |     74 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  vmacu $v15, $v23, $v28.e0                          ## L:241  |     75 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  addiu $k0, $s2, 352                                ## L:321  |      ^ | u16 dmemInB = dmemInA + (80 * 4 + 32);
  luv $v21, 0, 16, $s2                               ## L:325  |     76 | vec16 pixelB_01 = load_vec_u8(dmemInA, 16);
  vmacu $v15, $v22, $v28.e0                          ## L:242  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v20, 0, 16, $k0                               ## L:326  |     77 | vec16 pixelB_11 = load_vec_u8(dmemInB, 16);
  vmulu $v14, $v24, $v28.e0                          ## L:236  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v19, 0, 16, $fp                               ## L:327  |     78 | vec16 pixelB_21 = load_vec_u8(dmemInC, 16);
  vmacu $v14, $v23, $v28.e0                          ## L:237  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v14, $v22, $v28.e0                          ## L:238  |     79 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v01, 0, 40, $fp                               ## L:345  |      ^ | vec16 pixelE_21 = load_vec_u8(dmemInC, 40);
  vmacu $v14, $v21, $v28.e0                          ## L:240  |     80 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v18, 0, 48, $s2                               ## L:329  |      ^ | vec16 pixelF_01 = load_vec_u8(dmemInA, 48);
  vmacu $v14, $v20, $v28.e0                          ## L:241  |     81 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  addiu $sp, $sp, 64                                 ## L:366  |      ^ | dmemOutCurr += 64;
  vmacu $v14, $v19, $v28.e0                          ## L:242  |     82 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v09, 0, 24, $s2                               ## L:335  |      ^ | vec16 pixelC_01 = load_vec_u8(dmemInA, 24);
  luv $v08, 0, 24, $k0                               ## L:336  |     83 | vec16 pixelC_11 = load_vec_u8(dmemInB, 24);
  vmulu $v13, $v21, $v28.e0                          ## L:236  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v07, 0, 24, $fp                               ## L:337  |     84 | vec16 pixelC_21 = load_vec_u8(dmemInC, 24);
  vmacu $v13, $v20, $v28.e0                          ## L:237  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v17, 0, 48, $k0                               ## L:330  |     85 | vec16 pixelF_11 = load_vec_u8(dmemInB, 48);
  vmacu $v13, $v19, $v28.e0                          ## L:238  |      ^ | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v13, $v09, $v28.e0                          ## L:240  |     86 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v05, 0, 32, $k0                               ## L:340  |      ^ | vec16 pixelD_11 = load_vec_u8(dmemInB, 32);
  vmacu $v13, $v08, $v28.e0                          ## L:241  |     87 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v13, $v07, $v28.e0                          ## L:242  |     88 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v06, 0, 32, $s2                               ## L:339  |      ^ | vec16 pixelD_01 = load_vec_u8(dmemInA, 32);
  vmulu $v12, $v09, $v28.e0                          ## L:236  |     89 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v12, $v08, $v28.e0                          ## L:237  |     90 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v03, 0, 40, $s2                               ## L:343  |      ^ | vec16 pixelE_01 = load_vec_u8(dmemInA, 40);
  vmacu $v12, $v07, $v28.e0                          ## L:238  |     91 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v08, 0, 56, $k0                               ## L:356  |      ^ | vec16 pixelG_11 = load_vec_u8(dmemInB, 56);
  vmacu $v12, $v06, $v28.e0                          ## L:240  |     92 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v07, 0, 56, $fp                               ## L:357  |      ^ | vec16 pixelG_21 = load_vec_u8(dmemInC, 56);
  vmacu $v12, $v05, $v28.e0                          ## L:241  |     93 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v12, $v04, $v28.e0                          ## L:242  |     94 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v09, 0, 56, $s2                               ## L:355  |      ^ | vec16 pixelG_01 = load_vec_u8(dmemInA, 56);
  vmulu $v11, $v06, $v28.e0                          ## L:236  |     95 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  luv $v02, 0, 40, $k0                               ## L:344  |      ^ | vec16 pixelE_11 = load_vec_u8(dmemInB, 40);
  luv $v16, 0, 48, $fp                               ## L:331  |     96 | vec16 pixelF_21 = load_vec_u8(dmemInC, 48);
  vmacu $v11, $v05, $v28.e0                          ## L:237  |      ^ | sum:ufract = pixel10:ufract +* factor:ufract.x;
  vmacu $v11, $v04, $v28.e0                          ## L:238  |     97 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  luv $v06, 0, 64, $s2                               ## L:359  |      ^ | vec16 pixelH_01 = load_vec_u8(dmemInA, 64);
  vmacu $v11, $v03, $v28.e0                          ## L:240  |     98 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  addiu $s2, $s2, 64                                 ## L:377  |      ^ | dmemInA += 64;
  vmacu $v11, $v02, $v28.e0                          ## L:241  |     99 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  luv $v05, 0, 64, $k0                               ## L:360  |      ^ | vec16 pixelH_11 = load_vec_u8(dmemInB, 64);
  luv $v04, 0, 64, $fp                               ## L:361  |    100 | vec16 pixelH_21 = load_vec_u8(dmemInC, 64);
  vmacu $v11, $v01, $v28.e0                          ## L:242  |      ^ | sum:ufract = pixel21:ufract +* factor:ufract.x;
  addiu $fp, $fp, 64                                 ## L:379  |    101 | dmemInC += 64;
  vmulu $v10, $v03, $v28.e0                          ## L:236  |      ^ | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v10, $v02, $v28.e0                          ## L:237  |    102 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  luv $v24, 0, 8, $s2                                ## L:385  |      ^ | pixelA_01 = load_vec_u8(dmemInA, 8);
  vmacu $v10, $v01, $v28.e0                          ## L:238  |    103 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  addiu $k0, $k0, 64                                 ## L:378  |      ^ | dmemInB += 64;
  luv $v23, 0, 8, $k0                                ## L:386  |    104 | pixelA_11 = load_vec_u8(dmemInB, 8);
  vmacu $v10, $v18, $v28.e0                          ## L:240  |      ^ | sum:ufract = pixel01:ufract +* factor:ufract.x;
  luv $v26, 0, 0, $k0                                ## L:382  |    105 | pixelA_10 = load_vec_u8(dmemInB, 0);
  vmacu $v10, $v17, $v28.e0                          ## L:241  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v10, $v16, $v28.e0                          ## L:242  |    106 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  vmulu $v03, $v18, $v28.e0                          ## L:236  |    107 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v03, $v17, $v28.e0                          ## L:237  |    108 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  suv $v11, 0, -32, $sp                              ## L:372  |      ^ | store_vec_u8(sum4, dmemOutCurr, -32);
  vmacu $v03, $v16, $v28.e0                          ## L:238  |    109 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  vmacu $v03, $v09, $v28.e0                          ## L:240  |    110 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  suv $v10, 0, -24, $sp                              ## L:373  |      ^ | store_vec_u8(sum5, dmemOutCurr, -24);
  suv $v14, 0, -56, $sp                              ## L:369  |    111 | store_vec_u8(sum1, dmemOutCurr, -56);
  vmacu $v03, $v08, $v28.e0                          ## L:241  |      ^ | sum:ufract = pixel11:ufract +* factor:ufract.x;
  vmacu $v03, $v07, $v28.e0                          ## L:242  |    112 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  luv $v25, 0, 0, $fp                                ## L:383  |      ^ | pixelA_20 = load_vec_u8(dmemInC, 0);
  vmulu $v02, $v09, $v28.e0                          ## L:236  |    113 | sum:ufract = pixel00:ufract *  factor:ufract.x;
  vmacu $v02, $v08, $v28.e0                          ## L:237  |    114 | sum:ufract = pixel10:ufract +* factor:ufract.x;
  addiu $k1, $s1, 320                                ## L:388  |      ^ | dmemOutEnd = dmemOutStart + (80 * 4);
  vmacu $v02, $v07, $v28.e0                          ## L:238  |    115 | sum:ufract = pixel20:ufract +* factor:ufract.x;
  suv $v15, 0, -64, $sp                              ## L:368  |      ^ | store_vec_u8(sum0, dmemOutCurr, -64);
  vmacu $v02, $v06, $v28.e0                          ## L:240  |    116 | sum:ufract = pixel01:ufract +* factor:ufract.x;
  suv $v12, 0, -40, $sp                              ## L:371  |      ^ | store_vec_u8(sum3, dmemOutCurr, -40);
  vmacu $v02, $v05, $v28.e0                          ## L:241  |    117 | sum:ufract = pixel11:ufract +* factor:ufract.x;
  suv $v03, 0, -16, $sp                              ## L:374  |      ^ | store_vec_u8(sum6, dmemOutCurr, -16);
  vmacu $v02, $v04, $v28.e0                          ## L:242  |    118 | sum:ufract = pixel21:ufract +* factor:ufract.x;
  addiu $a1, $a1, 64                                 ## L:389  |      ^ | ptrRDRAMOut += 64;
  luv $v27, 0, 0, $s2                                ## L:381  |    119 | pixelA_00 = load_vec_u8(dmemInA, 0);
  suv $v13, 0, -48, $sp                              ## L:370  |    120 | store_vec_u8(sum2, dmemOutCurr, -48);
  bne $sp, $k1, LABEL_Cmd_Blur_0017                  ## L:389  |    121 | ptrRDRAMOut += 64;
  suv $v02, 0, -8, $sp                               ## L:375  |   *123 | store_vec_u8(sum7, dmemOutCurr, -8);
  LABEL_Cmd_Blur_0018:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:46   |    124 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:47   |    125 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $s5, COP0_DMA_WRITE                           ## L:48   |  **128 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  addiu $s0, $s4, %lo(BUFF_IN_A)                     ## L:217  |    129 | lastLoadedDmemIn = dmemInOffset + BUFF_IN_A;
  mtc0 $s0, COP0_DMA_SPADDR                          ## L:38   |   *131 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:39   |    132 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s4, $s4, 352                                ## L:219  |    133 | dmemInOffset += (80 * 4 + 32);
  beq $a0, $s7, LABEL_Cmd_Blur_0019                  ## L:221  |    134 | if(ptrRDRAMIn != ptrRDRAMInEnd) {
  mtc0 $s5, COP0_DMA_READ                            ## L:40   |   *136 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  addiu $a0, $a0, 320                                ## L:222  |    137 | ptrRDRAMIn += (80 * 4);
  LABEL_Cmd_Blur_0019:
  bne $s4, $s3, LABEL_Cmd_Blur_001A                  ## L:225  |    138 | if(dmemInOffset == dmemInOffsetEnd) {
  nop                                                ## L:225  |   *140 | if(dmemInOffset == dmemInOffsetEnd) {
  or $s4, $zero, $zero                               ## L:226  |    141 | dmemInOffset = 0;
  LABEL_Cmd_Blur_001A:
  addiu $s6, $s6, 65535                              ## L:395  |    142 | rowsLeft -= 1;
  bne $s6, $zero, LABEL_Cmd_Blur_0013                ## L:395  |    143 | rowsLeft -= 1;
  nop                                                ## L:395  |   *145 | rowsLeft -= 1;
  LABEL_Cmd_Blur_0014:
  j RSPQ_Loop                                        ## L:396  |    146 | 
  nop                                                ## L:396  |   *148 | 
Cmd_Downscale:
  lui $at, 0x04                                      ## L:419  |      ^ | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  lui $s6, 0x5000                                    ## L:423  |      2 | u16 dmaSizeIn = (
  addiu $s5, $zero, 319                              ## L:428  |      3 | u16 dmaSizeOut = (80 * 4) - 1;
  ori $s6, $s6, 0x14FF                               ## L:423  |      4 | u16 dmaSizeIn = (
  ori $at, $at, 0xB000                               ## L:419  |      5 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addu $s7, $a0, $at                                 ## L:419  |      6 | u32 ptrRDRAMInEnd = ptrRDRAMIn + (320 * 240 * 4);
  addiu $a1, $a1, 65216                              ## L:421  |      7 | prtRDRAMOut -= (80 * 4);
  ori $s3, $zero, %lo(BUFF_BLOOM_B)                  ## L:434  |      8 | u16 dmemOutB = BUFF_BLOOM_B;
  ori $s4, $zero, %lo(BUFF_BLOOM)                    ## L:433  |      9 | u16 dmemOutA = BUFF_BLOOM;
  addiu $at, $zero, 4096                             ## L:431  |     10 | sumFactor:sfract.x = 0.125;
  mtc2 $at, $v28.e0                                  ## L:431  |     11 | sumFactor:sfract.x = 0.125;
  LABEL_Cmd_Downscale_001D:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:442  |     12 | RA = get_dma_busy();
  ori $s0, $zero, %lo(BUFF_IN_B)                     ## L:444  |     13 | dmemInB = BUFF_IN_B;
  ori $s1, $zero, %lo(BUFF_IN_A)                     ## L:443  |     14 | dmemInA = BUFF_IN_A;
  bne $ra, $zero, LABEL_Cmd_Downscale_001D           ## L:444  |     15 | dmemInB = BUFF_IN_B;
  nop                                                ## L:568  |    *17 | 
  LABEL_Cmd_Downscale_001E:
  mtc0 $s1, COP0_DMA_SPADDR                          ## L:38   |     18 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:39   |     19 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $a0, $a0, 5120                               ## L:450  |     20 | ptrRDRAMIn += (320 * 4) * 4;
  mtc0 $s6, COP0_DMA_READ                            ## L:40   |    *22 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  LABEL_Cmd_Downscale_001F:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:453  |     23 | RA = get_dma_busy();
  or $s2, $zero, $s4                                 ## L:455  |     24 | dmemOut = dmemOutA;
  bne $ra, $zero, LABEL_Cmd_Downscale_001F           ## L:456  |    *26 | dmemOutEnd = dmemOut + (80 * 4);
  addiu $fp, $s2, 320                                ## L:456  |    *28 | dmemOutEnd = dmemOut + (80 * 4);
  LABEL_Cmd_Downscale_0020:
  addiu $s0, $s0, 1280                               ## L:465  |     29 | dmemInB += 1280;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:46   |     30 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  xor $s4, $s4, $s3                                  ## L:460  |     31 | swap(dmemOutA, dmemOutB);
  xor $s3, $s4, $s3                                  ## L:460  |     32 | swap(dmemOutA, dmemOutB);
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:47   |     33 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  addiu $s1, $s1, 1280                               ## L:464  |     34 | dmemInA += 1280;
  lqv $v27, 0, -32, $s1                              ## L:467  |     35 | vec16 a = load(dmemInA, -32);
  xor $s4, $s4, $s3                                  ## L:460  |     36 | swap(dmemOutA, dmemOutB);
  addiu $s0, $s0, 64256                              ## L:471  |     37 | dmemInB -= 1280;
  mtc0 $s5, COP0_DMA_WRITE                           ## L:48   |     38 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1
  sqv $v27, 0, -16, $s1                              ## L:468  |     39 | store(a, dmemInA, -16);
  addiu $s1, $s1, 64256                              ## L:470  |     40 | dmemInA -= 1280;
  LABEL_Cmd_Downscale_0021:
  luv $v23, 0, 0, $s0                                ## L:494  |     41 | p0_EF = load_vec_u8(dmemInB, 0);
  vadd $v27, $v25, $v21.v                            ## L:481  |      ^ | resA:sint = p0_AB:sint + p1_AB:sint;
  luv $v25, 0, 0, $s1                                ## L:492  |     42 | p0_AB = load_vec_u8(dmemInA, 0);
  luv $v24, 0, 8, $s1                                ## L:493  |     43 | p0_CD = load_vec_u8(dmemInA, 8);
  vadd $v26, $v17, $v13.v                            ## L:482  |      ^ | resB:sint = p2_AB:sint + p3_AB:sint;
  luv $v17, 0, 32, $s1                               ## L:502  |     44 | p2_AB = load_vec_u8(dmemInA, 32);
  luv $v22, 0, 8, $s0                                ## L:495  |     45 | p0_GH = load_vec_u8(dmemInB, 8);
  luv $v21, 0, 16, $s1                               ## L:497  |     46 | p1_AB = load_vec_u8(dmemInA, 16);
  vmulf $v25, $v25, $v28.e0                          ## L:404  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v20, 0, 24, $s1                               ## L:498  |     47 | p1_CD = load_vec_u8(dmemInA, 24);
  vmacf $v25, $v24, $v28.e0                          ## L:405  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v25, $v23, $v28.e0                          ## L:406  |     48 | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v19, 0, 16, $s0                               ## L:499  |      ^ | p1_EF = load_vec_u8(dmemInB, 16);
  vmacf $v25, $v22, $v28.e0                          ## L:407  |     49 | a:sfract = d:sfract +* sumFactor:sfract.x;
  luv $v18, 0, 24, $s0                               ## L:500  |      ^ | p1_GH = load_vec_u8(dmemInB, 24);
  luv $v14, 0, 40, $s0                               ## L:505  |     50 | p2_GH = load_vec_u8(dmemInB, 40);
  vmulf $v21, $v21, $v28.e0                          ## L:404  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v16, 0, 40, $s1                               ## L:503  |     51 | p2_CD = load_vec_u8(dmemInA, 40);
  vmacf $v21, $v20, $v28.e0                          ## L:405  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  vmacf $v21, $v19, $v28.e0                          ## L:406  |     52 | a:sfract = c:sfract +* sumFactor:sfract.x;
  luv $v15, 0, 32, $s0                               ## L:504  |      ^ | p2_EF = load_vec_u8(dmemInB, 32);
  luv $v10, 0, 56, $s0                               ## L:510  |     53 | p3_GH = load_vec_u8(dmemInB, 56);
  vmacf $v21, $v18, $v28.e0                          ## L:407  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  vmulf $v17, $v17, $v28.e0                          ## L:404  |     54 | a:sfract = a:sfract * sumFactor:sfract.x;
  luv $v13, 0, 48, $s1                               ## L:507  |      ^ | p3_AB = load_vec_u8(dmemInA, 48);
  vmacf $v17, $v16, $v28.e0                          ## L:405  |     55 | a:sfract = b:sfract +* sumFactor:sfract.x;
  luv $v12, 0, 56, $s1                               ## L:508  |      ^ | p3_CD = load_vec_u8(dmemInA, 56);
  luv $v11, 0, 48, $s0                               ## L:509  |     56 | p3_EF = load_vec_u8(dmemInB, 48);
  vmacf $v17, $v15, $v28.e0                          ## L:406  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  vmacf $v17, $v14, $v28.e0                          ## L:407  |     57 | a:sfract = d:sfract +* sumFactor:sfract.x;
  addiu $s0, $s0, 64                                 ## L:536  |      ^ | dmemInB += 64;
  addiu $a1, $a1, 16                                 ## L:537  |     58 | prtRDRAMOut += 16;
  vmulf $v13, $v13, $v28.e0                          ## L:404  |      ^ | a:sfract = a:sfract * sumFactor:sfract.x;
  suv $v27, 0, -16, $s2                              ## L:484  |     59 | store_vec_u8(resA, dmemOut, -16);
  vmacf $v13, $v12, $v28.e0                          ## L:405  |      ^ | a:sfract = b:sfract +* sumFactor:sfract.x;
  suv $v26, 0, -8, $s2                               ## L:485  |     60 | store_vec_u8(resB, dmemOut, -8);
  vmacf $v13, $v11, $v28.e0                          ## L:406  |      ^ | a:sfract = c:sfract +* sumFactor:sfract.x;
  sdv $v17, 8, 8, $s1                                ## L:528  |     61 | @Barrier("tmp1") store(p2_AB.XYZW, dmemInA, 8); ## Barrier: 0x4
  vmacf $v13, $v10, $v28.e0                          ## L:407  |      ^ | a:sfract = d:sfract +* sumFactor:sfract.x;
  sdv $v25, 8, 0, $s1                                ## L:522  |     62 | @Barrier("tmp0") store(p0_AB.XYZW, dmemInA, 0); ## Barrier: 0x2
  vmov $v25.e4, $v21.e0                              ## L:523  |      ^ | @Barrier("tmp0") p0_AB.X = p1_AB.x; ## Barrier: 0x2
  vmov $v25.e5, $v21.e1                              ## L:524  |     63 | @Barrier("tmp0") p0_AB.Y = p1_AB.y; ## Barrier: 0x2
  vmov $v25.e6, $v21.e2                              ## L:525  |     64 | @Barrier("tmp0") p0_AB.Z = p1_AB.z; ## Barrier: 0x2
  vmov $v17.e4, $v13.e0                              ## L:529  |     65 | @Barrier("tmp1") p2_AB.X = p3_AB.x; ## Barrier: 0x4
  addiu $s2, $s2, 16                                 ## L:534  |      ^ | dmemOut += 16;
  ldv $v21, 0, 0, $s1                                ## L:526  |     66 | @Barrier("tmp0") p1_AB.xyzw = load(dmemInA, 0).xyzw; ## Barrier: 0x2
  vmov $v17.e5, $v13.e1                              ## L:530  |      ^ | @Barrier("tmp1") p2_AB.Y = p3_AB.y; ## Barrier: 0x4
  vmov $v17.e6, $v13.e2                              ## L:531  |     67 | @Barrier("tmp1") p2_AB.Z = p3_AB.z; ## Barrier: 0x4
  ldv $v13, 0, 8, $s1                                ## L:532  |      ^ | @Barrier("tmp1") p3_AB.xyzw = load(dmemInA, 8).xyzw; ## Barrier: 0x4
  bne $s2, $fp, LABEL_Cmd_Downscale_0021             ## L:537  |     68 | prtRDRAMOut += 16;
  addiu $s1, $s1, 64                                 ## L:535  |    *70 | dmemInA += 64;
  LABEL_Cmd_Downscale_0022:
  vadd $v27, $v25, $v21.v                            ## L:541  |     71 | resA:sint = p0_AB:sint + p1_AB:sint;
  vadd $v26, $v17, $v13.v                            ## L:542  |     72 | resB:sint = p2_AB:sint + p3_AB:sint;
  suv $v26, 0, -8, $s2                               ## L:545  |  ***76 | store_vec_u8(resB, dmemOut, -8);
  bne $a0, $s7, LABEL_Cmd_Downscale_001D             ## L:545  |     77 | store_vec_u8(resB, dmemOut, -8);
  suv $v27, 0, -16, $s2                              ## L:544  |    *79 | store_vec_u8(resA, dmemOut, -16);
  LABEL_Cmd_Downscale_001C:
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:46   |     80 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a1, COP0_DMA_RAMADDR                         ## L:47   |     81 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  j RSPQ_Loop                                        ## L:49   |     82 | }
  mtc0 $s5, COP0_DMA_WRITE                           ## L:48   |   **85 | @Barrier("DMA") set_dma_write(size); ## Barrier: 0x1

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