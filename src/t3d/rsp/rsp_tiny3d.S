## Auto-generated file, transpiled with RSPL
#define RSPQ_PROFILE_MANUAL_SLOT 1
#define VERT_BUFFER_COUNT 70
#define LIGHT_COUNT 8
#define RDPQ_TRIANGLE_CUSTOM_VTX 1
#define VTX_ATTR_XY 0x00
#define VTX_ATTR_Z 0x04
#define VTX_ATTR_CLIPFLAGS 0x06
#define VTX_ATTR_REJFLAGS 0x07
#define VTX_ATTR_RGBA 0x08
#define VTX_ATTR_ST 0x0C
#define VTX_ATTR_CLIPPOSi 0x10
#define VTX_ATTR_Wi 0x16
#define VTX_ATTR_CLIPPOSf 0x18
#define VTX_ATTR_Wf 0x1E
#define VTX_ATTR_INVWi 0x20
#define VTX_ATTR_INVWf 0x22
#define RDPQ_TRIANGLE_VTX1 a0
#define RDPQ_TRIANGLE_VTX2 a1
#define RDPQ_TRIANGLE_VTX3 a2
#define RDPQ_TRIANGLE_VTX1_DMEM 0,v1
#define RDPQ_TRIANGLE_VTX2_DMEM 4,v1
#define RDPQ_TRIANGLE_VTX3_DMEM 2,v1
#define TRI_SIZE 36
#define TRI_SIZE_2 72
#define TRI_SIZE_3 108
#define RDP_CMD_MAX_SIZE 176
#define LIGHT_SIZE 16
#define MATRIX_SIZE 0x40
#define DRAW_TRIS 1
#define DP_STATUS_END_VALID 512
#define RDPQ_DYNAMIC_BUFFER_SIZE 65536
#include <rsp_queue.inc>
#include <rdpq_macros.h>
#include "./rspq_triangle.inc"

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
    RSPQ_DefineCommand T3DCmd_TriDraw, 8
    RSPQ_DefineCommand T3DCmd_SetScreenSize, 16
    RSPQ_DefineCommand T3DCmd_MatrixStack, 8
    RSPQ_DefineCommand T3DCmd_SetWord, 8
    RSPQ_DefineCommand T3DCmd_VertLoad, 12
    RSPQ_DefineCommand T3DCmd_LightSet, 16
    RSPQ_DefineCommand T3DCmd_RenderMode, 8
    RSPQ_DefineCommand T3DCmd_MatProjectionSet, 4
    RSPQ_DefineCommand T3DCmd_SetFogRange, 8
    RSPQ_DefineCommand T3DCmd_SetFogState, 4
    RSPQ_DefineCommand T3DCmd_TriSync, 4
    RSPQ_DefineCommand T3DCmd_TriDraw_Strip, 8
    RSPQ_DefineCommand T3DCmd_TriDraw_Seq, 8
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 3
    MATRIX_PROJ: .ds.b 64
    .align 3
    MATRIX_MVP: .ds.b 64
    .align 3
    MATRIX_MV: .ds.b 64
    .align 3
    MATRIX_NORMAL: .ds.b 24
    .align 1
    FOG_SCALE_OFFSET: .half 0, 0, 32767, 0
    .align 4
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 4
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 1
    RDPQ_TRI_BUFF_OFFSET: .half 0
    .align 1
    SCREEN_UVGEN_SCALE: .half 0
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0
    .align 1
    CLIP_RA: .half 0
    .align 1
    UV_GEN_PARAMS: .half 0, 0
    .align 1
    COLOR_EXPOSURE: .half 128
    .align 2
    SEGMENT_TABLE: .word 0, 0, 0, 0, 0, 0, 0, 0
    .align 2
    COLOR_AMBIENT: .ds.b 8
    .align 2
    #if RSPQ_PROFILE
    LIGHT_DIR_COLOR: .ds.b 32
    _RSPQ_OVL_PROFILESLOT: .long 0, 0
    #else
    LIGHT_DIR_COLOR: .ds.b 112
    #endif
    .align 2
    TRI_COMMAND: .word 0
    .align 2
    MATRIX_STACK_PTR: .word 0
    FACE_CULLING: .byte 0
    FOG_STORE_OFFSET: .byte 72
    ACTIVE_LIGHT_SIZE: .byte 0
    _UNUSED_: .byte 0
    .align 1
    VERTEX_FX_FUNC: .half 0
    .align 1
    CLIP_CODE_SIZE: .half 0
    .align 2
    CLIP_CODE_ADDR: .word 0
    .align 2
    CLIP_CODE_ORG_ADDR: .word 0
    .align 3
    VERT_BUFFER: .ds.b 2520
    STATE_MEM_END:
  RSPQ_EndSavedState

.bss
  TEMP_STATE_MEM_START:
    .align 4
    CLIP_BUFFER_TMP: .ds.b 252
    .align 4
    CLIP_BUFFER_RESULT: .ds.b 288
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

.align 3
CLIPPING_CODE_TARGET:
  lw $s0, %lo(CLIP_CODE_ADDR + 0)
  lhu $t0, %lo(CLIP_CODE_SIZE + 0)
  addiu $t2, $zero, 12
  ori $ra, $zero, %lo(CLIPPING_CODE_TARGET)
  sb $t0, %lo(CLIP_CODE_ADDR)($zero)
  j DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
T3DCmd_VertLoad:
  srl $s4, $a2, 16                                   ## L:331  |      ^ | u32<$s4> prt3d = addressInOut >> 16;
  srl $s6, $a1, 24                                   ## L:278  |      2 | u32 segment = addrIn >> 24;
  andi $t0, $a0, 65535                               ## L:333  |      3 | u32<$t0> copySize = bufferSize & 0xFFFF;
  lw $s6, %lo(SEGMENT_TABLE)($s6)                    ## L:279  |      4 | segment = load(segment, SEGMENT_TABLE);
  addu $s7, $s4, $t0                                 ## L:334  |      5 | u32 ptr3dEnd = prt3d + copySize;
  addiu $t0, $t0, -1                                 ## L:338  |      6 | dma_in_async(prt3d, dmaAddrRDRAM, copySize);
  or $t2, $zero, $zero                               ## L:338  |      7 | dma_in_async(prt3d, dmaAddrRDRAM, copySize);
  jal DMAExec                                        ## L:338  |      8 | dma_in_async(prt3d, dmaAddrRDRAM, copySize); ## Args: $t0, $t1, $s0, $s4, $t2
  addu $s0, $a1, $s6                                 ## L:280  |    *10 | addrOut = addrIn + segment;
  ori $s6, $zero, %lo(MATRIX_NORMAL)                 ## L:270  |     11 | u16 address = MATRIX_NORMAL;
  ldv $v19, 0, 8, $s6                                ## L:272  |     12 | mat1 = load(address, 0x08).xyzwxyzw;
  ldv $v18, 0, 16, $s6                               ## L:273  |     13 | mat2 = load(address, 0x10).xyzwxyzw;
  ldv $v19, 8, 8, $s6                                ## L:272  |     14 | mat1 = load(address, 0x08).xyzwxyzw;
  ldv $v20, 0, 0, $s6                                ## L:271  |     15 | mat0 = load(address, 0x00).xyzwxyzw;
  ldv $v20, 8, 0, $s6                                ## L:271  |     16 | mat0 = load(address, 0x00).xyzwxyzw;
  jal loadCurrentMVPMat                              ## L:345  |     17 | loadCurrentMVPMat(mat0, mat1, mat2, mat3); ## Args: $v27, $v25, $v23, $v21
  ldv $v18, 8, 16, $s6                               ## L:273  |    *19 | mat2 = load(address, 0x10).xyzwxyzw;
  ori $at, $zero, %lo(NORMAL_MASK_SHIFT)             ## L:347  |     20 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v16, 0, 8, $at                                ## L:348  |     21 | vec16 normShift = load(NORMAL_MASK_SHIFT, 0x08).xyzwxyzw;
  ldv $v17, 0, 0, $at                                ## L:347  |     22 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v17, 8, 0, $at                                ## L:347  |     23 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v16, 8, 8, $at                                ## L:348  |     24 | vec16 normShift = load(NORMAL_MASK_SHIFT, 0x08).xyzwxyzw;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:350  |     25 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  llv $v15, 0, 0, $at                                ## L:350  |     26 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:353  |     27 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v12, 0, 8, $at                                ## L:357  |     28 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  andi $s6, $a2, 65535                               ## L:363  |     29 | u32 ptrBuffA = addressInOut & 0xFFFF;
  addiu $s5, $s6, 65500                              ## L:364  |     30 | u32 ptrBuffB = ptrBuffA - 36;
  lbu $s2, %lo(ACTIVE_LIGHT_SIZE + 0)                ## L:368  |     31 | u8 ptrLightEnd = load(ACTIVE_LIGHT_SIZE);
  ldv $v12, 8, 8, $at                                ## L:357  |     32 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  ldv $v13, 0, 0, $at                                ## L:353  |     33 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v13, 8, 0, $at                                ## L:353  |     34 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ori $at, $zero, %lo(FOG_SCALE_OFFSET)              ## L:358  |     35 | vec16 fogScaleOffset = load(FOG_SCALE_OFFSET).xyzw;
  vor $v15, $v00, $v15.e1                            ## L:351  |      ^ | guardBandScale = guardBandScale.y;
  ldv $v11, 0, 0, $at                                ## L:358  |     36 | vec16 fogScaleOffset = load(FOG_SCALE_OFFSET).xyzw;
  vmudl $v14, $v00, $v31.e3                          ## L:355  |      ^ | screenSize >>= 4;
  lbu $k0, %lo(TRI_COMMAND + 0)                      ## L:384  |     37 | u8 isUnlit = load(TRI_COMMAND);
  lbu $s0, %lo(CLIP_CODE_ORG_ADDR + 0)               ## L:388  |     38 | u8<$s0> baseTileMask = load(CLIP_CODE_ORG_ADDR);
  vmadm $v13, $v13, $v31.e3                          ## L:355  |      ^ | screenSize >>= 4;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:361  |     39 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmadn $v14, $v00, $v00                             ## L:355  |      ^ | screenSize >>= 4;
  ldv $v10, 0, 0, $at                                ## L:361  |     40 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v10, 8, 0, $at                                ## L:361  |     41 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ori $at, $zero, %lo(UV_GEN_PARAMS)                 ## L:373  |     42 | uvGenArgs.XYZW = load(UV_GEN_PARAMS).xyzw;
  ldv $v09, 8, 0, $at                                ## L:373  |     43 | uvGenArgs.XYZW = load(UV_GEN_PARAMS).xyzw;
  addiu $s6, $s6, 65464                              ## L:379  |     44 | ptrBuffA -= 72;
  vmov $v11.e6, $v11.e2                              ## L:359  |      ^ | fogScaleOffset.Z = fogScaleOffset.z;
  ori $at, $zero, %lo(SCREEN_UVGEN_SCALE)            ## L:374  |     45 | uvGenArgs.W = load(SCREEN_UVGEN_SCALE).x;
  lbu $s1, %lo(FOG_STORE_OFFSET + 0)                 ## L:378  |     46 | u8 fogStoreOffset = load(FOG_STORE_OFFSET);
  addiu $s2, $s2, %lo(LIGHT_DIR_COLOR)               ## L:369  |     47 | ptrLightEnd += LIGHT_DIR_COLOR;
  lsv $v09, 14, 0, $at                               ## L:374  |     48 | uvGenArgs.W = load(SCREEN_UVGEN_SCALE).x;
  ori $at, $zero, %lo(UV_GEN_PARAMS)                 ## L:375  |     49 | uvGenArgs.xy = load(UV_GEN_PARAMS).xy;
  llv $v09, 0, 0, $at                                ## L:375  |     50 | uvGenArgs.xy = load(UV_GEN_PARAMS).xy;
  jal DMAWaitIdle                                    ## L:392  |     51 | dma_await();
  llv $v09, 4, 0, $at                                ## L:376  |    *53 | uvGenArgs.zw = load(UV_GEN_PARAMS).xy;
  lqv $v08, 0, 0, $s4                                ## L:395  |     54 | vec16 pos = load(prt3d, 0x00);
  vand $v07, $v17, $v08.h3                           ## L:396  |  ***58 | vec16 norm = normMask & pos.wwwwWWWW;
  vmudn $v07, $v07, $v16.v                           ## L:397  |  ***62 | norm *= normShift;
  LOOP_START:
  vor $v03, $v00, $v11.e2                            ## L:410  |     63 | vec16 lightColor = fogScaleOffset.z;
  lbu $a2, 19($s4)                                   ## L:413  |      ^ | baseTile = load(prt3d, 0x13);
  vmulf $v29, $v20, $v07.h0                          ## L:424  |   **66 | VTEMP:sfract = matN0:sfract  * norm.xxxxXXXX;
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR)               ## L:431  |      ^ | ptrLight = LIGHT_DIR_COLOR;
  vmacf $v29, $v19, $v07.h1                          ## L:425  |     67 | VTEMP:sfract = matN1:sfract +* norm.yyyyYYYY;
  lbu $t8, 15($s3)                                   ## L:435  |      ^ | u8 isPointLight = load(ptrLight, 15);
  vmacf $v07, $v18, $v07.h2                          ## L:426  |     68 | norm         = matN2:sfract +* norm.zzzzZZZZ;
  vmudn $v06, $v28, $v08.h0                          ## L:212  |     69 | out = mat0  * vec.xxxxXXXX;
  addiu $s5, $s5, 72                                 ## L:419  |      ^ | ptrBuffB += 72;
  vmadh $v05, $v27, $v08.h0                          ## L:212  |     70 | out = mat0  * vec.xxxxXXXX;
  and $a2, $a2, $s0                                  ## L:414  |      ^ | baseTile &= baseTileMask;
  vmadn $v06, $v26, $v08.h1                          ## L:213  |     71 | out = mat1 +* vec.yyyyYYYY;
  lhu $k1, %lo(VERTEX_FX_FUNC + 0)                   ## L:420  |      ^ | vertexFxFunc = load(VERTEX_FX_FUNC);
  vmadh $v05, $v25, $v08.h1                          ## L:213  |     72 | out = mat1 +* vec.yyyyYYYY;
  lpv $v02, 0, 8, $s3                                ## L:434  |      ^ | vec16 lightDirVec  = load_vec_s8(ptrLight, 8);
  vmadn $v06, $v24, $v08.h2                          ## L:214  |     73 | out = mat2 +* vec.zzzzZZZZ;
  or $t9, $zero, $zero                               ## L:416  |      ^ | u8 mvMatrixLoaded = 0;
  addiu $s6, $s6, 72                                 ## L:418  |     74 | ptrBuffA += 72;
  vmadh $v05, $v23, $v08.h2                          ## L:214  |      ^ | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v06, $v22, $v30.e7                          ## L:215  |     75 | out = mat3 +* 1;
  bne $k0, $zero, LIGHT_LOOP_END                     ## L:438  |      ^ | if(isUnlit)goto LIGHT_LOOP_END;
  vmadh $v05, $v21, $v30.e7                          ## L:215  |    *77 | out = mat3 +* 1;
  beq $s3, $s2, LIGHT_LOOP_END                       ## L:441  |     78 | if(ptrLight != ptrLightEnd)
  luv $v03, 0, -8, $s3                               ## L:439  |    *80 | lightColor = load_vec_u8(ptrLight, -8);
  LABEL_T3DCmd_VertLoad_0003:
  vmulf $v01, $v07, $v02.v                           ## L:445  |     81 | vec16 lightDirScale = norm:sfract * lightDirVec:sfract;
  luv $v04, 0, 0, $s3                                ## L:446  |      ^ | color = load_vec_u8(ptrLight);
  bne $t8, $zero, pointLight                         ## L:448  |     82 | if(isPointLight) {
  ori $ra, $zero, LABEL_T3DCmd_VertLoad_0005         ## L:964  |    *84 | dma_out(dmaDest, addrRDRAM, 0x40);
  LABEL_T3DCmd_VertLoad_0005:
  addiu $s3, $s3, 16                                 ## L:462  |     85 | ptrLight += 16;
  vmulu $v29, $v04, $v01.h0                          ## L:456  |      ^ | VTEMP = color:ufract  * lightDirScale:ufract.xxxxXXXX;
  vmacu $v29, $v04, $v01.h1                          ## L:457  |     86 | VTEMP = color:ufract +* lightDirScale:ufract.yyyyYYYY;
  lbu $t8, 15($s3)                                   ## L:464  |      ^ | isPointLight =  load(ptrLight, 15);
  vmacu $v29, $v04, $v01.h2                          ## L:458  |     87 | VTEMP = color:ufract +* lightDirScale:ufract.zzzzZZZZ;
  lpv $v02, 0, 8, $s3                                ## L:463  |      ^ | lightDirVec = load_vec_s8(ptrLight, 8);
  bne $s3, $s2, LABEL_T3DCmd_VertLoad_0003           ## L:464  |     88 | isPointLight =  load(ptrLight, 15);
  vadd $v03, $v03, $v29.v                            ## L:460  |  ***92 | lightColor:sint += VTEMP;
  LIGHT_LOOP_END:
  vmudm $v02, $v15, $v06.h3                          ## L:470  |     93 | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  luv $v04, 0, 16, $s4                               ## L:472  |      ^ | color = load_vec_u8(prt3d, 0x10);
  ori $ra, $zero, LABEL_T3DCmd_VertLoad_0006         ## L:964  |     94 | dma_out(dmaDest, addrRDRAM, 0x40);
  vmadh $v01, $v15, $v05.h3                          ## L:470  |      ^ | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  vmadn $v02, $v00, $v00                             ## L:470  |     95 | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  bne $t9, $zero, loadCurrentMVPMat                  ## L:475  |      ^ | if(mvMatrixLoaded) {
  addu $fp, $s6, $s1                                 ## L:473  |    *97 | fogA = ptrBuffA + fogStoreOffset;
  LABEL_T3DCmd_VertLoad_0006:
  vch $v29, $v05, $v05.h3                            ## L:483  |     98 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v06, $v06.h3                            ## L:483  |     99 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:483  |    100 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  vch $v29, $v05, $v01                               ## L:484  |      ^ | u32 clipCodes = clip(posClip, clipPlaneW);
  vcl $v29, $v06, $v02                               ## L:484  |    101 | u32 clipCodes = clip(posClip, clipPlaneW);
  addu $sp, $s5, $s1                                 ## L:541  |      ^ | fogB = ptrBuffB + fogStoreOffset;
  vmulf $v04, $v04, $v03.v                           ## L:499  |    102 | color:sfract *= lightColor:ufract;
  lqv $v08, 0, 32, $s4                               ## L:565  |      ^ | pos = load(prt3d, 0x20);
  vmudl $v06, $v06, $v10.v                           ## L:501  |    103 | posClip *= normScaleW:ufract;
  cfc2 $t3, $vcc                                     ## L:484  |      ^ | u32 clipCodes = clip(posClip, clipPlaneW);
  vmadm $v05, $v05, $v10.v                           ## L:501  |    104 | posClip *= normScaleW:ufract;
  srl $t5, $t4, 4                                    ## L:493  |      ^ | rejCodesB = rejCodes >> 4;
  andi $t5, $t5, 1799                                ## L:253  |    105 | res = clipCode & 0b0000'0111'0000'0111;
  vmadn $v06, $v00, $v00                             ## L:501  |      ^ | posClip *= normScaleW:ufract;
  srl $t7, $t3, 4                                    ## L:486  |    106 | clipCodeB = clipCodes >> 4;
  andi $t6, $t4, 1799                                ## L:253  |    107 | res = clipCode & 0b0000'0111'0000'0111;
  vmudm $v04, $v04, $v31.e6                          ## L:503  |      ^ | color >>= 7;
  sdv $v05, 0, 16, $s6                               ## L:511  |    108 | store(posClip.xyzw, ptrBuffA, 0x10);
  vrcph $v13.e3, $v05.e3                             ## L:517  |      ^ | screenSize.w = invert_half(posClip).w;
  sdv $v06, 0, 24, $s6                               ## L:511  |    109 | store(posClip.xyzw, ptrBuffA, 0x10);
  vrcpl $v14.e3, $v06.e3                             ## L:517  |      ^ | screenSize.w = invert_half(posClip).w;
  sdv $v06, 8, 24, $s5                               ## L:512  |    110 | store(posClip.XYZW, ptrBuffB, 0x10);
  vrcph $v13.e3, $v05.e7                             ## L:518  |      ^ | screenSize.W = invert_half(posClip).W;
  andi $t8, $t3, 775                                 ## L:245  |    111 | res = clipCode & 0b0000'0011'0000'0111;
  vrcpl $v14.e7, $v06.e7                             ## L:518  |      ^ | screenSize.W = invert_half(posClip).W;
  vrcph $v13.e7, $v00.e7                             ## L:518  |    112 | screenSize.W = invert_half(posClip).W;
  srl $t2, $t8, 5                                    ## L:246  |      ^ | u32 tmp = res >> 5;
  vmudh $v04, $v04, $v09.e6                          ## L:504  |    113 | color:sint *= uvGenArgs:sint.Z;
  sdv $v05, 8, 16, $s5                               ## L:512  |      ^ | store(posClip.XYZW, ptrBuffB, 0x10);
  or $t8, $t8, $t2                                   ## L:247  |    114 | res |= tmp;
  vaddc $v03, $v06, $v11.e1                          ## L:529  |      ^ | fog:ufract = posClip + fogScaleOffset:ufract.y;
  vadd $v02, $v05, $v11.e0                           ## L:530  |    115 | fog:sint   = posClip + fogScaleOffset.x;
  andi $t7, $t7, 775                                 ## L:245  |      ^ | res = clipCode & 0b0000'0011'0000'0111;
  srl $t2, $t7, 5                                    ## L:246  |    116 | u32 tmp = res >> 5;
  vmudl $v29, $v06, $v14.h3                          ## L:532  |      ^ | posClip *= screenSize.wwwwWWWW;
  vmadm $v29, $v05, $v14.h3                          ## L:532  |    117 | posClip *= screenSize.wwwwWWWW;
  or $t8, $t8, $a2                                   ## L:490  |      ^ | clipCodeA |= baseTile;
  suv $v04, 0, 8, $s6                                ## L:506  |    118 | @Barrier("color-fog") store_vec_u8(color.x, ptrBuffA, 0x08); ## Barrier: 0x1
  vmadn $v06, $v06, $v13.h3                          ## L:532  |      ^ | posClip *= screenSize.wwwwWWWW;
  ssv $v14, 6, 34, $s6                               ## L:522  |    119 | store(screenSize.w, ptrBuffA, 0x20);
  vmadh $v05, $v05, $v13.h3                          ## L:532  |      ^ | posClip *= screenSize.wwwwWWWW;
  vmudh $v02, $v02, $v11.e3                          ## L:534  |    120 | fog:sint *= fogScaleOffset:sint.w;
  ssv $v14, 14, 34, $s5                              ## L:523  |      ^ | store(screenSize.W, ptrBuffB, 0x20);
  ssv $v13, 6, 32, $s6                               ## L:522  |    121 | store(screenSize.w, ptrBuffA, 0x20);
  or $t7, $t7, $t2                                   ## L:247  |    122 | res |= tmp;
  vmudl $v29, $v06, $v14.v                           ## L:536  |      ^ | posClip *= screenSize;
  srl $t2, $t6, 5                                    ## L:254  |    123 | u32 tmp = res >> 5;
  vmadm $v29, $v05, $v14.v                           ## L:536  |      ^ | posClip *= screenSize;
  suv $v04, 4, 8, $s5                                ## L:507  |    124 | @Barrier("color-fog") store_vec_u8(color.X, ptrBuffB, 0x08); ## Barrier: 0x1
  vmadn $v06, $v06, $v13.v                           ## L:536  |      ^ | posClip *= screenSize;
  vmadh $v05, $v05, $v13.v                           ## L:536  |    125 | posClip *= screenSize;
  or $t7, $t7, $a2                                   ## L:491  |      ^ | clipCodeB |= baseTile;
  vsub $v29, $v11, $v02.h2                           ## L:538  |    126 | VTEMP:sint = fogScaleOffset - fog:sint.zzzzZZZZ;
  ssv $v13, 14, 32, $s5                              ## L:523  |      ^ | store(screenSize.W, ptrBuffB, 0x20);
  nor $t6, $t6, $t2                                  ## L:255  |    127 | res = res ~| tmp;
  srl $t2, $t5, 5                                    ## L:254  |    128 | u32 tmp = res >> 5;
  vmadh $v05, $v12, $v30.e7                          ## L:545  |      ^ | posClip:sint = screenOffset:sint +* 1;
  ldv $v03, 0, 24, $s4                               ## L:551  |    129 | vec16 uv = load(prt3d, 0x18).xyzw;
  addiu $s4, $s4, 32                                 ## L:570  |    130 | prt3d += 0x20;
  nor $t5, $t5, $t2                                  ## L:255  |    131 | res = res ~| tmp;
  vmudn $v29, $v29, $v30.e6                          ## L:547  |      ^ | VTEMP <<= 1;
  sdv $v05, 0, 0, $s6                                ## L:553  |    132 | @Barrier("pos-cc") store(posClip:sint.xyzw, ptrBuffA, 0x00); ## Barrier: 0x2
  vor $v02, $v00, $v07                               ## L:566  |      ^ | vec16 oldNorm = norm;
  vand $v07, $v17, $v08.h3                           ## L:567  |    133 | norm = normMask & pos.wwwwWWWW;
  sdv $v05, 8, 0, $s5                                ## L:554  |      ^ | @Barrier("pos-cc") store(posClip:sint.XYZW, ptrBuffB, 0x00); ## Barrier: 0x2
  sb $t8, 6($s6)                                     ## L:557  |    134 | @Barrier("pos-cc") store(clipCodeA:u8, ptrBuffA, 0x06); ## Barrier: 0x2
  sb $t6, 7($s6)                                     ## L:558  |    135 | @Barrier("pos-cc") store(rejCodesA:u8, ptrBuffA, 0x07); ## Barrier: 0x2
  sb $t7, 6($s5)                                     ## L:560  |    136 | @Barrier("pos-cc") store(clipCodeB:u8, ptrBuffB, 0x06); ## Barrier: 0x2
  ssv $v29, 4, 0, $fp                                ## L:548  |    137 | @Barrier("color-fog") store(VTEMP:sint.z, fogA); ## Barrier: 0x1
  vmudn $v07, $v07, $v16.v                           ## L:568  |      ^ | norm *= normShift;
  sb $t5, 7($s5)                                     ## L:561  |    138 | @Barrier("pos-cc") store(rejCodesB:u8, ptrBuffB, 0x07); ## Barrier: 0x2
  vaddc $v03, $v03, $v09.v                           ## L:563  |      ^ | uv += uvGenArgs;
  jal $k1                                            ## L:572  |    139 | vertexFxFunc();
  ssv $v29, 12, 0, $sp                               ## L:549  |   *141 | @Barrier("color-fog") store(VTEMP:sint.Z, fogB); ## Barrier: 0x1
VertexFX_None:
  slv $v03, 0, 12, $s6
  # emux_trace_stop
  bne $s4, $s7, LOOP_START
  slv $v03, 4, 12, $s5
  j RSPQ_Loop

#if RSPQ_PROFILE == 1
  nop
  VertexFX_Spherical:
  VertexFX_CelShadeColor:
  VertexFX_CelShadeAlpha:
  VertexFX_Outline:
  j VertexFX_None
  nop
#endif

#if RSPQ_PROFILE == 0
VertexFX_Spherical:
  vge $v05, $v05, $v00.e0 ## L:607
  vsubc $v04, $v05, $v12.v ## L:608
  vlt $v04, $v04, $v12 ## L:609
  vmulf $v03, $v02, $v09.v ## L:610
  vmudn $v04, $v04, $v09.e7 ## L:612
  vaddc $v03, $v03, $v09.e0 ## L:616
  vmulf $v04, $v04, $v02.h2 ## L:613
  vmulf $v04, $v04, $v09.e0 ## L:614
  vaddc $v03, $v03, $v04.v ## L:617
  slv $v03, 0, 12, $s6 ## L:619
  bne $s4, $s7, LOOP_START ## L:585
  slv $v03, 8, 12, $s5 ## L:620
  j RSPQ_Loop ## L:585
VertexFX_CelShadeColor:
  vge $v04, $v04, $v04
  vge $v04, $v04, $v04
  vmov $v03.e0, $v04.e0
  vmov $v03.e2, $v04.e4
  slv $v03, 4, 12, $s5
  bne $s4, $s7, LOOP_START
  slv $v03, 0, 12, $s6
  j RSPQ_Loop
VertexFX_CelShadeAlpha:
  vmov $v03.e0, $v04.e3
  vmov $v03.e2, $v04.e7
  slv $v03, 4, 12, $s5
  bne $s4, $s7, LOOP_START
  slv $v03, 0, 12, $s6
  j RSPQ_Loop
VertexFX_Outline:
  vmulf $v29, $v02, $v09.v
  vmadh $v05, $v05, $v30.e7
  slv $v05, 8, 0, $s5
  bne $s4, $s7, LOOP_START
  slv $v05, 0, 0, $s6
  j RSPQ_Loop
  nop
#endif

pointLight:
  bne $t9, $zero, LABEL_pointLight_000C              ## L:683  |      ^ | if(!mvMatrixLoaded) {
  lhu $t7, 14($s3)                                   ## L:682  |     *3 | lightSize:u16 = load(ptrLight, 14);
  ori $at, $zero, %lo(MATRIX_MV)                     ## L:260  |      4 | mat0 = load(MATRIX_MV, 0x00).xyzwxyzw;
  ldv $v26, 0, 24, $at                               ## L:261  |      5 | mat1 = load(MATRIX_MV, 0x10).xyzwxyzw;
  ldv $v26, 8, 24, $at                               ## L:261  |      6 | mat1 = load(MATRIX_MV, 0x10).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:260  |      7 | mat0 = load(MATRIX_MV, 0x00).xyzwxyzw;
  ldv $v28, 0, 8, $at                                ## L:260  |      8 | mat0 = load(MATRIX_MV, 0x00).xyzwxyzw;
  ldv $v23, 0, 32, $at                               ## L:262  |      9 | mat2 = load(MATRIX_MV, 0x20).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:262  |     10 | mat2 = load(MATRIX_MV, 0x20).xyzwxyzw;
  ldv $v28, 8, 8, $at                                ## L:260  |     11 | mat0 = load(MATRIX_MV, 0x00).xyzwxyzw;
  ldv $v27, 8, 0, $at                                ## L:260  |     12 | mat0 = load(MATRIX_MV, 0x00).xyzwxyzw;
  ldv $v21, 0, 48, $at                               ## L:263  |     13 | mat3 = load(MATRIX_MV, 0x30).xyzwxyzw;
  ldv $v24, 8, 40, $at                               ## L:262  |     14 | mat2 = load(MATRIX_MV, 0x20).xyzwxyzw;
  ldv $v25, 0, 16, $at                               ## L:261  |     15 | mat1 = load(MATRIX_MV, 0x10).xyzwxyzw;
  vmudn $v28, $v28, $v08.h0                          ## L:212  |      ^ | out = mat0  * vec.xxxxXXXX;
  ldv $v25, 8, 16, $at                               ## L:261  |     16 | mat1 = load(MATRIX_MV, 0x10).xyzwxyzw;
  ldv $v23, 8, 32, $at                               ## L:262  |     17 | mat2 = load(MATRIX_MV, 0x20).xyzwxyzw;
  ldv $v22, 0, 56, $at                               ## L:263  |     18 | mat3 = load(MATRIX_MV, 0x30).xyzwxyzw;
  vmadh $v27, $v27, $v08.h0                          ## L:212  |      ^ | out = mat0  * vec.xxxxXXXX;
  ldv $v22, 8, 56, $at                               ## L:263  |     19 | mat3 = load(MATRIX_MV, 0x30).xyzwxyzw;
  vmadn $v28, $v26, $v08.h1                          ## L:213  |      ^ | out = mat1 +* vec.yyyyYYYY;
  ldv $v21, 8, 48, $at                               ## L:263  |     20 | mat3 = load(MATRIX_MV, 0x30).xyzwxyzw;
  vmadh $v27, $v25, $v08.h1                          ## L:213  |      ^ | out = mat1 +* vec.yyyyYYYY;
  vmadn $v28, $v24, $v08.h2                          ## L:214  |     21 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v27, $v23, $v08.h2                          ## L:214  |     22 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v28, $v22, $v30.e7                          ## L:215  |     23 | out = mat3 +* 1;
  vmadh $v27, $v21, $v30.e7                          ## L:215  |     24 | out = mat3 +* 1;
  vmudl $v28, $v28, $v31.e3                          ## L:689  |   **27 | vertPos >>= 4;
  vmadm $v27, $v27, $v31.e3                          ## L:689  |     28 | vertPos >>= 4;
  addiu $t9, $zero, 1                                ## L:690  |      ^ | mvMatrixLoaded = 1;
  vmadn $v28, $v00, $v00                             ## L:689  |     29 | vertPos >>= 4;
  LABEL_pointLight_000C:
  ldv $v23, 0, 8, $s3                                ## L:697  |      ^ | ptPos:sint.xyzw = load(ptrLight, 8).xyzw;
  ldv $v23, 8, 8, $s3                                ## L:698  |     30 | ptPos:sint.XYZW = load(ptrLight, 8).xyzw;
  andi $t7, $t7, 32767                               ## L:693  |  ***34 | lightSize &= 0x7FFF;
  vmudm $v23, $v23, $v31.e7                          ## L:700  |      ^ | ptPos = ptPos:sint >> 8;
  vmadn $v24, $v00, $v00                             ## L:700  |     35 | ptPos = ptPos:sint >> 8;
  sll $t7, $t7, 13                                   ## L:694  |      ^ | lightSize <<= 13;
  vsubc $v24, $v24, $v28.v                           ## L:701  |  ***39 | ptPos = ptPos - vertPos;
  vsub $v23, $v23, $v27.v                            ## L:701  |     40 | ptPos = ptPos - vertPos;
  vmudl $v29, $v24, $v24.v                           ## L:704  |   **43 | lightVertDot = ptPos * ptPos;
  vmadm $v29, $v23, $v24.v                           ## L:704  |     44 | lightVertDot = ptPos * ptPos;
  vmadn $v22, $v24, $v23.v                           ## L:704  |     45 | lightVertDot = ptPos * ptPos;
  vmadh $v21, $v23, $v23.v                           ## L:704  |     46 | lightVertDot = ptPos * ptPos;
  vaddc $v26, $v22, $v22.h1                          ## L:705  |   **49 | mat1         = lightVertDot + lightVertDot.yyyyYYYY;
  vadd $v25, $v21, $v21.h1                           ## L:705  |     50 | mat1         = lightVertDot + lightVertDot.yyyyYYYY;
  vaddc $v22, $v22, $v26.h2                          ## L:706  |   **53 | lightVertDot = lightVertDot + mat1.zzzzZZZZ;
  mtc2 $t7, $v26.e3                                  ## L:708  |      ^ | mat1.w = lightSize;
  vadd $v21, $v21, $v25.h2                           ## L:706  |     54 | lightVertDot = lightVertDot + mat1.zzzzZZZZ;
  srl $at, $t7, 16                                   ## L:708  |      ^ | mat1.w = lightSize;
  vrsqh $v21.e0, $v21.e0                             ## L:714  |  ***58 | lightVertDot.x = invert_half_sqrt(lightVertDot).x;
  lbu $t7, 14($s3)                                   ## L:710  |      ^ | lightSize:u8 = load(ptrLight, 14);
  vrsql $v22.e0, $v22.e0                             ## L:714  |     59 | lightVertDot.x = invert_half_sqrt(lightVertDot).x;
  mtc2 $at, $v25.e3                                  ## L:708  |      ^ | mat1.w = lightSize;
  sra $t7, $t7, 7                                    ## L:711  |   **62 | lightSize >>= 7;
  vrsqh $v21.e0, $v21.e4                             ## L:715  |      ^ | lightVertDot.X = invert_half_sqrt(lightVertDot).X;
  vrsql $v22.e4, $v22.e4                             ## L:715  |     63 | lightVertDot.X = invert_half_sqrt(lightVertDot).X;
  vrsqh $v21.e4, $v00.e0                             ## L:715  |     64 | lightVertDot.X = invert_half_sqrt(lightVertDot).X;
  vmudl $v29, $v22, $v26.e3                          ## L:716  |   **67 | lightRes = lightVertDot * mat1.w;
  vmadm $v29, $v21, $v26.e3                          ## L:716  |     68 | lightRes = lightVertDot * mat1.w;
  vmadn $v29, $v22, $v25.e3                          ## L:716  |     69 | lightRes = lightVertDot * mat1.w;
  bne $t7, $zero, LABEL_pointLight_000D              ## L:718  |      ^ | if(!lightSize) {
  vmadh $v01, $v21, $v25.e3                          ## L:716  |    *71 | lightRes = lightVertDot * mat1.w;
  vmudn $v22, $v22, $v31.e7                          ## L:721  |     72 | lightVertDot *= 256;
  vmadh $v21, $v21, $v31.e7                          ## L:721  |     73 | lightVertDot *= 256;
  vmulf $v01, $v01, $v01.v                           ## L:723  |    *75 | lightRes:sfract *= lightRes:sfract;
  vmudl $v29, $v24, $v22.h0                          ## L:725  |     76 | pos:sint = ptPos * lightVertDot.xxxxXXXX;
  vmadm $v29, $v23, $v22.h0                          ## L:725  |     77 | pos:sint = ptPos * lightVertDot.xxxxXXXX;
  vmadn $v29, $v24, $v21.h0                          ## L:725  |     78 | pos:sint = ptPos * lightVertDot.xxxxXXXX;
  vmadh $v08, $v23, $v21.h0                          ## L:725  |     79 | pos:sint = ptPos * lightVertDot.xxxxXXXX;
  vmulf $v08, $v08, $v01.h0                          ## L:726  |  ***83 | pos:sfract *= lightRes:sfract.xxxxXXXX;
  jr $ra
  vmulf $v01, $v08, $v07.v                           ## L:729  |  ***87 | lightRes = pos:sfract * norm:sfract;
  LABEL_pointLight_000D:
  vmulf $v01, $v01, $v01.v                           ## L:732  |    *91 | lightRes:sfract *= lightRes:sfract;
  vmov $v01.e5, $v00.e0                              ## L:734  |     92 | lightRes.Y = 0; lightRes.Z = 0;
  vmov $v01.e6, $v00.e0                              ## L:734  |     93 | lightRes.Y = 0; lightRes.Z = 0;
  vmov $v01.e2, $v00.e0                              ## L:733  |     94 | lightRes.y = 0; lightRes.z = 0;
  jr $ra                                             ## L:735  |      ^ | }
  vmov $v01.e1, $v00.e0                              ## L:733  |     95 | lightRes.y = 0; lightRes.z = 0;
loadCurrentMVPMat:
  ori $at, $zero, %lo(MATRIX_MVP)
  ldv $v28, 8, 8, $at
  ldv $v22, 8, 56, $at
  ldv $v28, 0, 8, $at
  ldv $v25, 8, 16, $at
  ldv $v22, 0, 56, $at
  ldv $v21, 0, 48, $at
  ldv $v23, 0, 32, $at
  ldv $v24, 8, 40, $at
  ldv $v23, 8, 32, $at
  ldv $v26, 8, 24, $at
  ldv $v27, 0, 0, $at
  ldv $v24, 0, 40, $at
  ldv $v27, 8, 0, $at
  ldv $v26, 0, 24, $at
  ldv $v25, 0, 16, $at
  jr $ra
  ldv $v21, 8, 48, $at
.align 3
T3DCmd_SetScreenSize:
  srl $s7, $a0, 8                                    ## L:742  |      ^ | u16 uvgenScaleFactor = guardBandFactor >> 8;
  sh $a3, %lo(6 + NORM_SCALE_W)($zero)               ## L:740  |      2 | store(depthAndWScale:s16, NORM_SCALE_W, 6);
  andi $a0, $a0, 15                                  ## L:746  |      3 | guardBandFactor &= 0xF;
  subu $s6, $zero, $a0                               ## L:747  |      4 | s8 guardBandFactorNeg = ZERO - guardBandFactor;
  sb $s6, %lo(19 + CLIPPING_PLANES)($zero)           ## L:753  |      5 | store(guardBandFactorNeg, CLIPPING_PLANES, 19);
  sh $s7, %lo(SCREEN_UVGEN_SCALE)($zero)             ## L:743  |      6 | store(uvgenScaleFactor, SCREEN_UVGEN_SCALE);
  sb $a0, %lo(7 + CLIPPING_PLANES)($zero)            ## L:750  |      7 | store(guardBandFactor,    CLIPPING_PLANES, 7);
  sb $a0, %lo(3 + CLIPPING_PLANES)($zero)            ## L:749  |      8 | store(guardBandFactor,    CLIPPING_PLANES, 3);
  sb $s6, %lo(15 + CLIPPING_PLANES)($zero)           ## L:752  |      9 | store(guardBandFactorNeg, CLIPPING_PLANES, 15);
  sw $a3, %lo(4 + SCREEN_SCALE_OFFSET)($zero)        ## L:736  |     10 | store(depthAndWScale:u32, SCREEN_SCALE_OFFSET, 0x04);
  sw $a2, %lo(0 + SCREEN_SCALE_OFFSET)($zero)        ## L:737  |     11 | store(screenScaleXY, SCREEN_SCALE_OFFSET, 0x00);
  j RSPQ_Loop                                        ## L:754  |     12 | }
  sw $a1, %lo(8 + SCREEN_SCALE_OFFSET)($zero)        ## L:738  |    *14 | store(screenOffsetXY, SCREEN_SCALE_OFFSET, 0x08);
T3DCmd_SetFogRange:
  sw $a1, %lo(0 + FOG_SCALE_OFFSET)($zero) ## L:616
  j RSPQ_Loop ## L:617
  sh $a0, %lo(6 + FOG_SCALE_OFFSET)($zero) ## L:615
T3DCmd_SetFogState:
  j RSPQ_Loop
  sb $a0, %lo(FOG_STORE_OFFSET)($zero)
T3DCmd_LightSet:
  beq $a2, $zero, LABEL_000A
  sw $a1, 0($a0)
  sw $a3, 12($a0)
  sw $a2, 8($a0)
  LABEL_000A:
  j RSPQ_Loop
  sw $a1, 4($a0)
T3DCmd_RenderMode:
  lbu $s7, %lo(RDPQ_OTHER_MODES + 0)
  sb $a0, %lo(FACE_CULLING)($zero)
  andi $s7, $s7, 56
  or $a1, $a1, $s7
  j RSPQ_Loop
  sw $a1, %lo(TRI_COMMAND)($zero)
mulMat4Mat4:
  ldv $v01, 0, 0, $s3
  ldv $v10, 0, 8, $s4
  ldv $v02, 0, 8, $s3
  ldv $v09, 0, 0, $s4
  ldv $v02, 8, 8, $s3
  ldv $v10, 8, 24, $s4
  ldv $v01, 8, 0, $s3
  ldv $v09, 8, 16, $s4
  ldv $v03, 0, 16, $s3
  ldv $v05, 0, 32, $s3
  vmudl $v29, $v02, $v10.h0
  ldv $v04, 0, 24, $s3
  ldv $v06, 0, 40, $s3
  ldv $v06, 8, 40, $s3
  ldv $v04, 8, 24, $s3
  vmadm $v29, $v01, $v10.h0
  ldv $v03, 8, 16, $s3
  vmadn $v14, $v02, $v09.h0
  ldv $v07, 0, 48, $s3
  ldv $v08, 0, 56, $s3
  vmadh $v13, $v01, $v09.h0
  vmadl $v29, $v04, $v10.h1
  ldv $v05, 8, 32, $s3
  vmadm $v29, $v03, $v10.h1
  ldv $v11, 0, 32, $s4
  ldv $v12, 0, 40, $s4
  vmadn $v14, $v04, $v09.h1
  vmadh $v13, $v03, $v09.h1
  ldv $v08, 8, 56, $s3
  vmadl $v29, $v06, $v10.h2
  ldv $v12, 8, 56, $s4
  ldv $v07, 8, 48, $s3
  vmadm $v29, $v05, $v10.h2
  vmadn $v14, $v06, $v09.h2
  ldv $v11, 8, 48, $s4
  vmadh $v13, $v05, $v09.h2
  vmadl $v29, $v08, $v10.h3
  vmadm $v29, $v07, $v10.h3
  vmadn $v10, $v08, $v09.h3
  vmadh $v09, $v07, $v09.h3
  vmudl $v29, $v02, $v12.h0
  vmadm $v29, $v01, $v12.h0
  vmadn $v14, $v02, $v11.h0
  vmadh $v13, $v01, $v11.h0
  vmadl $v29, $v04, $v12.h1
  vmadm $v29, $v03, $v12.h1
  sdv $v09, 0, 0, $s2
  vmadn $v14, $v04, $v11.h1
  vmadh $v13, $v03, $v11.h1
  vmadl $v29, $v06, $v12.h2
  sdv $v10, 8, 24, $s2
  sdv $v09, 8, 16, $s2
  vmadm $v29, $v05, $v12.h2
  vmadn $v14, $v06, $v11.h2
  vmadh $v13, $v05, $v11.h2
  vmadl $v29, $v08, $v12.h3
  vmadm $v29, $v07, $v12.h3
  vmadn $v12, $v08, $v11.h3
  vmadh $v11, $v07, $v11.h3
  sdv $v10, 0, 8, $s2
  sdv $v12, 0, 40, $s2
  sdv $v11, 0, 32, $s2
  sdv $v12, 8, 56, $s2
  jr $ra
  sdv $v11, 8, 48, $s2
normalizeMatrixVector:
  ldv $v02, 0, 8, $s4
  ldv $v01, 0, 0, $s4
  vmudl $v29, $v02, $v02.v
  vmadm $v29, $v01, $v02.v
  vmadn $v04, $v02, $v01.v
  addiu $s4, $s4, 16
  vmadh $v03, $v01, $v01.v
  vaddc $v06, $v04, $v04.h1
  vadd $v05, $v03, $v03.h1
  vaddc $v04, $v04, $v06.h2
  vadd $v03, $v03, $v05.h2
  vrsqh $v03.e3, $v03.e0
  vrsql $v04.e3, $v04.e0
  vrsqh $v03.e3, $v00.e0
  vmudl $v04, $v04, $v30.e0
  vmadm $v03, $v03, $v30.e0
  vmadn $v04, $v00, $v00
  vmudl $v29, $v02, $v04.h3
  vmadm $v29, $v01, $v04.h3
  vmadn $v02, $v02, $v03.h3
  vmadh $v01, $v01, $v03.h3
  vadd $v02, $v02, $v02.v
  sdv $v02, 0, 0, $t1
  jr $ra
  addiu $t1, $t1, 8
T3DCmd_MatrixStack:
  andi $s7, $a0, 1
  andi $s6, $a0, 2
  lh $a0, %lo(RSPQ_DMEM_BUFFER  -7)($gp)
  lw $s5, %lo(MATRIX_STACK_PTR + 0)
  addu $s5, $s5, $a0
  bne $s6, $zero, RSPQ_Loop
  sw $s5, %lo(MATRIX_STACK_PTR)($zero)
  srl $s3, $a1, 24
  lw $s3, %lo(SEGMENT_TABLE)($s3)
  addu $a1, $a1, $s3
  bgez $a0, LABEL_000C
  ori $s4, $zero, %lo(MATRIX_MV)
  or $a1, $zero, $s5
  LABEL_000C:
  lui $at, 0xFF
  ori $at, $at, 0xFFFF
  ori $t0, $zero, 63
  addiu $t2, $zero, 12
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  and $s0, $a1, $at
  beq $s7, $zero, LABEL_000D
  ori $t1, $zero, %lo(MATRIX_NORMAL)
  addiu $s0, $s5, 65472
  ori $t0, $zero, 63
  addiu $t2, $zero, 12
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(CLIP_BUFFER_RESULT)
  ori $s3, $zero, %lo(CLIP_BUFFER_RESULT)
  ori $s4, $zero, %lo(MATRIX_MV)
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s2, $zero, %lo(MATRIX_MV)
  LABEL_000D:
  ori $t0, $zero, 63
  addiu $t2, $zero, -32756
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $s5
  jal normalizeMatrixVector ## Args: $s4, $t1
  ori $s2, $zero, %lo(MATRIX_MVP)
  jal normalizeMatrixVector ## Args: $s4, $t1
  ori $s3, $zero, %lo(MATRIX_PROJ)
  jal normalizeMatrixVector ## Args: $s4, $t1
  nop
  ori $ra, $zero, %lo(RSPQ_Loop)
  j mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s4, $zero, %lo(MATRIX_MV)
T3DCmd_MatProjectionSet:
  addiu $t2, $zero, 12
  ori $t0, $zero, 63
  or $s0, $zero, $a0
  j DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(MATRIX_PROJ)
T3DCmd_SetWord:
  andi $s7, $a0, 32768
  bne $s7, $zero, RSPQ_Loop
  sh $a1, ($a0)
  j RSPQ_Loop
  sw $a1, ($a0)
  RDPQ_Triangle_Clip:
  sh $ra, %lo(CLIP_RA)
  jal RDPQ_Triangle_Send_End
  sh $s4, %lo(18 + RSPQ_SCRATCH_MEM)($zero)
  j CLIPPING_CODE_TARGET
  ori $s4, $zero, %lo(CLIPPING_CODE_TARGET)
T3DCmd_TriDraw_Strip:
  LABEL_T3DCmd_TriDraw_Strip_0014:
  mfc0 $ra, COP0_DMA_BUSY                            ## L:1047 |      1 | RA = get_dma_busy();
  andi $t0, $a1, 65535                               ## L:1048 |      2 | copySize = countDmem & 0xFFFF;
  sra $s4, $a1, 16                                   ## L:1049 |      3 | prtDmem = countDmem >> 16;
  bne $ra, $zero, LABEL_T3DCmd_TriDraw_Strip_0014    ## L:1050 |      4 | prtDmemEnd = prtDmem + copySize;
  addu $s2, $s4, $t0                                 ## L:1050 |     *6 | prtDmemEnd = prtDmem + copySize;
  LABEL_T3DCmd_TriDraw_Strip_0015:
  mtc0 $s4, COP0_DMA_SPADDR                          ## L:223  |      7 | @Barrier("DMA") set_dma_addr_rsp(addrDMEM); ## Barrier: 0x1
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:224  |      8 | @Barrier("DMA") set_dma_addr_rdram(addrRDRAM); ## Barrier: 0x1
  mtc0 $t0, COP0_DMA_READ                            ## L:225  |   **11 | @Barrier("DMA") set_dma_read(size); ## Barrier: 0x1
  T3DCmd_TriDraw_Strip_PostDMA:
  addiu $s2, $s2, 65533                              ## L:1065 |     12 | prtDmemEnd -= 3;
  addiu $s4, $s4, 65534                              ## L:1071 |     13 | prtDmem -= 2;
  LABEL_T3DCmd_TriDraw_Strip_0016:
  mfc0 $v1, COP0_DMA_BUSY                            ## L:1074 |     14 | vertAddr = get_dma_busy();
  bne $v1, $zero, LABEL_T3DCmd_TriDraw_Strip_0016    ## L:1074 |   **17 | vertAddr = get_dma_busy();
  nop                                                ## L:1074 |    *19 | vertAddr = get_dma_busy();
  STRIP_LOOP_NEW:
  lbu $v0, %lo(FACE_CULLING + 0)                     ## L:1078 |     20 | faceCull = load(FACE_CULLING);
  addiu $s4, $s4, 2                                  ## L:1079 |     21 | prtDmem += 2;
  STRIP_LOOP_NEXT:
  beq $s4, $s2, STRIP_LOOP_END                       ## L:1082 |     22 | if(prtDmem == prtDmemEnd)goto STRIP_LOOP_END;
  xori $v0, $v0, 1                                   ## L:1081 |    *24 | faceCull ^= 1;
  lh $a1, 4($s4)                                     ## L:1085 |     25 | idx1:s16 = load(prtDmem, 4);
  lhu $a0, 2($s4)                                    ## L:1084 |     26 | idx0     = load(prtDmem, 2);
  lhu $a2, 0($s4)                                    ## L:1086 |     27 | idx2     = load(prtDmem, 0);
  bltz $a1, STRIP_LOOP_NEW                           ## L:1089 |     28 | if(idx1:s16 < 0)goto STRIP_LOOP_NEW;
  addiu $s4, $s4, 2                                  ## L:1088 |    *30 | prtDmem += 2;
  j RDPQ_Triangle_Send_Async                         ## L:1093 |     31 | goto RDPQ_Triangle_Send_Async;
  ori $ra, $zero, %lo(STRIP_LOOP_NEXT)               ## L:1092 |    *33 | RA = STRIP_LOOP_NEXT;
  STRIP_LOOP_END:
  bgez $s2, RSPQ_Loop                                ## L:1101 |     34 | if(prtDmemEnd >= 0)goto RSPQ_Loop;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:1098 |    *36 | RA = RSPQ_Loop;
T3DCmd_TriSync:
  lbu $s7, %lo(CLIP_CODE_ADDR + 0)
  beq $s7, $zero, RDPQ_Triangle_Send_End
  ori $s4, $zero, %lo(CLIPPING_CODE_TARGET)
  lhu $t0, %lo(CLIP_CODE_SIZE + 0)
  or $t2, $zero, $zero
  lw $s0, %lo(CLIP_CODE_ORG_ADDR + 0)
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  sb $zero, %lo(CLIP_CODE_ADDR)($zero)
  j RDPQ_Triangle_Send_End
  ori $ra, $zero, %lo(RSPQ_Loop)
T3DCmd_TriDraw_Seq:
  srl $s1, $a1, 16                                   ## L:1137 |      ^ | u16<$s1> isQuad = endIndex >> 16;
  andi $s4, $a1, 65535                               ## L:1133 |      2 | u8<$s4> baseVtxEnd = endIndex & 0xFFFF;
  lbu $v0, %lo(FACE_CULLING + 0)                     ## L:1136 |      3 | u8<$v0> faceCull = load(FACE_CULLING);
  andi $s2, $a0, 65535                               ## L:1134 |      4 | u16<$s2> baseVtx = sizeAndIndex & 0xFFFF;
  addiu $v1, $s1, 108                                ## L:1138 |      5 | u16<$v1> vtxIncr = isQuad + (36 * 3);
  SEQ_LOOP_NEXT:
  beq $s2, $s4, SEQ_LOOP_END                         ## L:1146 |      6 | if(baseVtx == baseVtxEnd)goto SEQ_LOOP_END;
  addiu $a0, $s2, 72                                 ## L:1145 |     *8 | idx0 = baseVtx + (36 * 2);
  or $a2, $zero, $s2                                 ## L:1148 |      9 | idx2 = baseVtx;
  addiu $a1, $s2, 36                                 ## L:1147 |     10 | idx1 = baseVtx + (36 * 1);
  ori $ra, $zero, %lo(SEQ_LOOP_NEXT)                 ## L:1151 |     11 | RA = SEQ_LOOP_NEXT;
  j RDPQ_Triangle_Send_Async                         ## L:1152 |     12 | goto RDPQ_Triangle_Send_Async;
  addu $s2, $s2, $v1                                 ## L:1149 |    *14 | baseVtx += vtxIncr;
  SEQ_LOOP_END:
  beq $s1, $zero, T3DCmd_TriSync                     ## L:1157 |     15 | if(isQuad) {
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:1156 |    *17 | RA = RSPQ_Loop;
  lhu $s2, %lo(RSPQ_DMEM_BUFFER  -6)($gp)            ## L:1160 |     18 | baseVtx:u16 = load_arg(2);
  xori $v0, $v0, 1                                   ## L:1159 |     19 | faceCull ^= 1;
  addiu $s4, $s4, 36                                 ## L:1162 |     20 | baseVtxEnd += 36;
  addiu $s2, $s2, 36                                 ## L:1161 |     21 | baseVtx += 36;
  j SEQ_LOOP_NEXT                                    ## L:1163 |     22 | goto SEQ_LOOP_NEXT;
  or $s1, $zero, $zero                               ## L:1158 |    *24 | isQuad = 0;

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