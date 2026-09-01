## Auto-generated file, transpiled with RSPL
#define OVERLAY_CLIPPING 1
#define VERT_BUFFER_COUNT 70
#define LIGHT_COUNT 7
#define RSPQ_PROFILE_MANUAL_SLOT 1
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
#define DRAW_TRIS 1
#define DP_STATUS_END_VALID 512
#define RDPQ_DYNAMIC_BUFFER_SIZE 65536
#define MASK_W 0b1000
#define MASK_Z 0b100'0000'0000
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
    RSPQ_DefineCommand RSPQ_Loop, 8
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
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
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 3
    SCREEN_SCALE: .half 0, 0, 0, 0, 0, 0, 0, 0
    .align 1
    SCREEN_OFFSET: .half 0, 0, 16383, 255
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
    COLOR_AMBIENT: .word 4294967295, 4294967295
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
    #if RSPQ_PROFILE
      CLIP_BUFFER_RESULT: .ds.b 236
    #else
      CLIP_BUFFER_RESULT: .ds.b 252
    #endif
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

.align 3
clipTriangle:
  ## hot-path cycles: 229
  ldv $v01, 0, 0, $a0                                ## L:578  |      ^ | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v02, 0, 8, $a0                                ## L:579  |      2 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v03, 0, 16, $a0                               ## L:580  |      3 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ori $t3, $zero, %lo(CLIP_BUFFER_TMP)               ## L:422  |      4 | u32 ptrWrite = CLIP_BUFFER_TMP;
  ldv $v04, 0, 24, $a0                               ## L:581  |      5 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  sdv $v02, 0, 8, $t3                                ## L:584  |      6 | store(tmp2.xyzw, ptrDst, 0x08);
  ori $t1, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:420  |      7 | u32 ptrBuff0 = CLIP_BUFFER_RESULT;
  sdv $v01, 0, 0, $t3                                ## L:583  |      8 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v02, 0, 8, $a1                                ## L:579  |      9 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  sdv $v03, 0, 16, $t3                               ## L:585  |     10 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v03, 0, 16, $a1                               ## L:580  |     11 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v04, 0, 24, $t3                               ## L:586  |     12 | store(tmp4.xyzw, ptrDst, 0x18);
  ldv $v01, 0, 0, $a1                                ## L:578  |     13 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v04, 0, 24, $a1                               ## L:581  |     14 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t3, $t3, 36                                 ## L:588  |     15 | ptrDst += 36;
  ori $t2, $zero, %lo(CLIP_BUFFER_TMP)               ## L:421  |     16 | u32 ptrBuff1 = CLIP_BUFFER_TMP;
  sdv $v03, 0, 16, $t3                               ## L:585  |     17 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v01, 0, 0, $t3                                ## L:583  |     18 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v02, 0, 8, $t3                                ## L:584  |     19 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v04, 0, 24, $t3                               ## L:586  |     20 | store(tmp4.xyzw, ptrDst, 0x18);
  ldv $v01, 0, 0, $a2                                ## L:578  |     21 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v03, 0, 16, $a2                               ## L:580  |     22 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ldv $v02, 0, 8, $a2                                ## L:579  |     23 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v04, 0, 24, $a2                               ## L:581  |     24 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t0, $zero, 108                              ## L:416  |     25 | arraySize = 108;
  addiu $t3, $t3, 36                                 ## L:588  |     26 | ptrDst += 36;
  sdv $v01, 0, 0, $t3                                ## L:583  |     27 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v04, 0, 24, $t3                               ## L:586  |     28 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v02, 0, 8, $t3                                ## L:584  |     29 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v03, 0, 16, $t3                               ## L:585  |     30 | store(tmp3.xyzw, ptrDst, 0x10);
  addiu $t3, $t3, 36                                 ## L:588  |     31 | ptrDst += 36;
  POST_VTX_COPY:
  lsv $v01, 2, 42, $t3                               ## L:441  |     32 | codes.y = load(ptrWrite, 42).x;
  lsv $v01, 4, 78, $t3                               ## L:442  |     33 | codes.z = load(ptrWrite, 78).x;
  lsv $v01, 0, 6, $t3                                ## L:440  |     34 | codes.x = load(ptrWrite, 6).x;
  lbu $t6, 6($a0)                                    ## L:459  |  ***38 | u8 oldClip = load(ptrVertA, 0x06);
  vxor $v02, $v01, $v30.e4                           ## L:443  |      ^ | vec16 wBit = codes ^ 0b1000;
  addiu $t5, $zero, 1                                ## L:455  |     39 | s32 planeIdx = 1;
  vand $v03, $v01, $v31.e5                           ## L:444  |      ^ | vec16 oldBit = codes & 0b100'0000'0000;
  vand $v02, $v02, $v30.e4                           ## L:445  |   **42 | wBit &= 0b1000;
  ori $t4, $zero, %lo(CLIPPING_PLANES)               ## L:454  |      ^ | u32 planePtr = CLIPPING_PLANES;
  vxor $v01, $v01, $v03.v                            ## L:446  |     43 | codes ^= oldBit;
  vmudn $v02, $v02, $v30.e0                          ## L:447  |   **46 | wBit <<= 7;
  sb $t6, %lo(20 + RSPQ_SCRATCH_MEM)($zero)          ## L:460  |      ^ | store(oldClip, RSPQ_SCRATCH_MEM, 20);
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:464  |  ***50 | normScaleWInv.x = load(NORM_SCALE_W).w;
  vor $v01, $v01, $v02.v                             ## L:448  |      ^ | codes |= wBit;
  ssv $v01, 0, 6, $t3                                ## L:449  |  ***54 | store(codes.x, ptrWrite, 6);
  ssv $v01, 4, 78, $t3                               ## L:451  |     55 | store(codes.z, ptrWrite, 78);
  ssv $v01, 2, 42, $t3                               ## L:450  |     56 | store(codes.y, ptrWrite, 42);
  lsv $v01, 0, 6, $at                                ## L:464  |     57 | normScaleWInv.x = load(NORM_SCALE_W).w;
  vor $v01, $v00, $v01.e0                            ## L:465  |  ***61 | normScaleWInv = normScaleWInv.x;
  ori $at, $zero, 0xFFFF                             ## L:466  |      ^ | normScaleWInv.w = 0xFFFF;
  mtc2 $at, $v01.e3                                  ## L:466  |     62 | normScaleWInv.w = 0xFFFF;
  ori $at, $zero, 0xFFFF                             ## L:467  |     63 | normScaleWInv.W = 0xFFFF;
  mtc2 $at, $v01.e7                                  ## L:467  |    *65 | normScaleWInv.W = 0xFFFF;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:469  |     66 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  llv $v02, 0, 0, $at                                ## L:469  |     67 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  vor $v02, $v00, $v02.e1                            ## L:470  |  ***71 | guardBandScale = guardBandScale.y;
  LABEL_clipTriangle_0001:
  xor $t1, $t1, $t2                                  ## L:485  |      ^ | swap(ptrBuff0, ptrBuff1);
  addiu $t6, $t3, 65500                              ## L:476  |     72 | u32 ptrLast = ptrWrite - 36;
  llv $v04, 8, 12, $t6                               ## L:480  |     73 | lastPosUV:sint.XY = load(ptrLast, 0x0C).xy;
  ldv $v05, 0, 24, $t6                               ## L:479  |     74 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  xor $t2, $t1, $t2                                  ## L:485  |     75 | swap(ptrBuff0, ptrBuff1);
  xor $t1, $t1, $t2                                  ## L:485  |     76 | swap(ptrBuff0, ptrBuff1);
  luv $v03, 0, 8, $t6                                ## L:478  |     77 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  or $t3, $zero, $t2                                 ## L:487  |     78 | ptrWrite = ptrBuff1;
  ldv $v04, 0, 16, $t6                               ## L:479  |     79 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  lbu $t7, 6($t6)                                    ## L:481  |     80 | u8 lastClipCode   = load(ptrLast, 0x06);
  addu $t8, $t1, $t0                                 ## L:489  |     81 | u32 ptrReadEnd = ptrBuff0 + arraySize;
  or $t6, $zero, $t1                                 ## L:488  |     82 | u32 ptrRead = ptrBuff0;
  LABEL_clipTriangle_0003:
  ldv $v06, 0, 16, $t6                               ## L:494  |     83 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  lbu $t9, 6($t6)                                    ## L:498  |     84 | u8 clipCode = load(ptrRead, 0x06);
  llv $v06, 8, 12, $t6                               ## L:495  |     85 | currentPosUV:sint.XY = load(ptrRead, 0x0C).xy;
  luv $v08, 0, 8, $t6                                ## L:496  |     86 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  xor $k0, $t9, $t7                                  ## L:501  |     87 | u8 exclIntersect = clipCode ^ lastClipCode;
  ldv $v07, 0, 24, $t6                               ## L:494  |     88 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  nor $k1, $zero, $t9                                ## L:502  |     89 | u8 isInsideCurr = ~clipCode;
  and $k0, $k0, $t5                                  ## L:503  |     90 | exclIntersect &= planeIdx;
  beq $k0, $zero, LABEL_clipTriangle_0005            ## L:506  |     91 | if(exclIntersect)
  and $k1, $k1, $t5                                  ## L:504  |     92 | isInsideCurr &= planeIdx;
  and $t7, $t7, $t5                                  ## L:508  |     93 | lastClipCode &= planeIdx;
  lpv $v14, 0, 0, $t4                                ## L:332  |     94 | vec32 planeNorm:sint = load_vec_s8(planePtr);
  beq $t7, $zero, LABEL_clipTriangle_0006            ## L:337  |     95 | if(newClipCode)
  nop                                                ## L:337  |     96 | if(newClipCode)
  vxor $v15, $v00, $v00.e0                           ## L:340  |     97 | planeNorm:sfract = 0;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:339  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  sdv $v14, 0, 0, $at                                ## L:339  |     98 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsubc $v10, $v05, $v07.v                           ## L:342  |      ^ | posUVDiff = posBase - pos;
  ldv $v14, 8, 0, $at                                ## L:339  |     99 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v07                               ## L:343  |      ^ | vpos = pos;
  vor $v11, $v00, $v06                               ## L:343  |    100 | vpos = pos;
  sdv $v04, 0, 0, $at                                ## L:344  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vsub $v09, $v04, $v06.v                            ## L:342  |    101 | posUVDiff = posBase - pos;
  vmudl $v15, $v15, $v31.e7                          ## L:345  |    102 | planeNorm >>= 8;
  sdv $v05, 0, 8, $at                                ## L:344  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  ldv $v11, 8, 0, $at                                ## L:344  |    103 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadm $v14, $v14, $v31.e7                          ## L:345  |      ^ | planeNorm >>= 8;
  ldv $v12, 8, 8, $at                                ## L:344  |    104 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:345  |      ^ | planeNorm >>= 8;
  beq $zero, $zero, LABEL_clipTriangle_0007          ## L:347  |    105 | colorDiff = colorBase - color;
  vsubc $v13, $v03, $v08.v                           ## L:347  |   *107 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0006:
  vsubc $v10, $v07, $v05.v                           ## L:352  |      1 | posUVDiff = pos - posBase;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:349  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:350  |      2 | planeNorm:sfract = 0;
  sdv $v14, 0, 0, $at                                ## L:349  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v05                               ## L:353  |      3 | vpos = posBase;
  ldv $v14, 8, 0, $at                                ## L:349  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v04                               ## L:353  |      4 | vpos = posBase;
  sdv $v06, 0, 0, $at                                ## L:354  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsub $v09, $v06, $v04.v                            ## L:352  |      5 | posUVDiff = pos - posBase;
  vmudl $v15, $v15, $v31.e7                          ## L:355  |      6 | planeNorm >>= 8;
  sdv $v07, 0, 8, $at                                ## L:354  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  ldv $v11, 8, 0, $at                                ## L:354  |      7 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vmadm $v14, $v14, $v31.e7                          ## L:355  |      ^ | planeNorm >>= 8;
  ldv $v12, 8, 8, $at                                ## L:354  |      8 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:355  |      ^ | planeNorm >>= 8;
  vsubc $v13, $v08, $v03.v                           ## L:357  |      9 | colorDiff = color - colorBase;
  LABEL_clipTriangle_0007:
  vmudl $v15, $v15, $v01.v                           ## L:360  | **************************************************************************************************108 | planeNorm *= normScaleWInv:ufract;
  vmadm $v14, $v14, $v01.v                           ## L:360  |    109 | planeNorm *= normScaleWInv:ufract;
  vmadn $v15, $v00, $v00                             ## L:360  |    110 | planeNorm *= normScaleWInv:ufract;
  vmudl $v29, $v12, $v15.v                           ## L:228  | ***114 | res = a * b;
  vmadm $v29, $v11, $v15.v                           ## L:228  |    115 | res = a * b;
  vmadn $v17, $v12, $v14.v                           ## L:228  |    116 | res = a * b;
  vmadh $v16, $v11, $v14.v                           ## L:228  |    117 | res = a * b;
  vaddc $v17, $v17, $v17.q1                          ## L:229  |  **120 | res += res.yywwYYWW;
  vadd $v16, $v16, $v16.q1                           ## L:229  |    121 | res += res.yywwYYWW;
  vaddc $v17, $v17, $v17.h2                          ## L:230  |  **124 | res += res.zzzzZZZZ;
  vadd $v16, $v16, $v16.h2                           ## L:230  |    125 | res += res.zzzzZZZZ;
  vsubc $v19, $v17, $v17.e4                          ## L:363  |  **128 | vec32 fac = dot - dot.X;
  vsub $v18, $v16, $v16.e4                           ## L:363  |    129 | vec32 fac = dot - dot.X;
  vrcph $v18.e0, $v18.e0                             ## L:365  | ***133 | fac.x = invert_half(fac).x;
  vrcpl $v19.e0, $v19.e0                             ## L:365  |    134 | fac.x = invert_half(fac).x;
  vrcph $v18.e0, $v00.e0                             ## L:365  |    135 | fac.x = invert_half(fac).x;
  vaddc $v19, $v19, $v19.v                           ## L:366  |  **138 | fac += fac;
  vadd $v18, $v18, $v18.v                            ## L:366  |    139 | fac += fac;
  vmudl $v29, $v19, $v17.e0                          ## L:367  |  **142 | fac *= dot.x;
  vmadm $v29, $v18, $v17.e0                          ## L:367  |    143 | fac *= dot.x;
  vmadn $v19, $v19, $v16.e0                          ## L:367  |    144 | fac *= dot.x;
  vmadh $v18, $v18, $v16.e0                          ## L:367  |    145 | fac *= dot.x;
  mfc2 $sp, $v18.e0                                  ## L:369  | ***149 | s16 facX = fac:sint.x;
  beq $sp, $zero, LABEL_clipTriangle_0008            ## L:370  |  **152 | if(facX != 0) {
  nop                                                ## L:370  |    153 | if(facX != 0) {
  vlt $v29, $v18, $v30.e7                            ## L:374  |    154 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v19, $v19, $v01.e3                           ## L:374  |    155 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v18, $v00.e0                            ## L:375  |    156 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v19, $v19, $v00                              ## L:375  |  **159 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0008:
  beq $t7, $zero, LABEL_clipTriangle_0009            ## L:380  |      ^ | if(newClipCode) {
  vmudm $v13, $v13, $v19.e0                          ## L:378  | ***163 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v17, $v10, $v19.e0                          ## L:382  |    164 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:382  |    165 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:382  |    166 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v07, $v30.e7                          ## L:383  |    167 | resPosUV = pos +* 1;
  vmadh $v04, $v06, $v30.e7                          ## L:383  |    168 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000A          ## L:384  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v03, $v08, $v13.v                           ## L:384  |   *170 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_0009:
  vmudl $v17, $v10, $v19.e0                          ## L:386  |      1 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:386  |      2 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:386  |      3 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v05, $v30.e7                          ## L:387  |      4 | resPosUV = posBase +* 1;
  vmadh $v04, $v04, $v30.e7                          ## L:387  |      5 | resPosUV = posBase +* 1;
  vaddc $v03, $v03, $v13.v                           ## L:388  |      6 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000A:
  vmudl $v10, $v05, $v01.v                           ## L:392  | ********************************************************************************************************************************************************************171 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v09, $v04, $v01.v                           ## L:392  |    172 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  sdv $v04, 0, 16, $t3                               ## L:606  |      ^ | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  sdv $v05, 0, 24, $t3                               ## L:606  |    173 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  vmadn $v10, $v00, $v00                             ## L:392  |      ^ | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmudm $v12, $v02, $v05.h3                          ## L:393  |    174 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  suv $v03, 0, 8, $t3                                ## L:602  |      ^ | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  addiu $t3, $t3, 36                                 ## L:607  |    175 | ptrWrite += 36;
  vmadh $v11, $v02, $v04.h3                          ## L:393  |      ^ | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadn $v12, $v00, $v00                             ## L:393  |    176 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  slv $v04, 8, -24, $t3                              ## L:603  |      ^ | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  vmov $v09.e2, $v09.e3                              ## L:395  |    177 | clipBase.z = clipBase.w;
  vmov $v10.e2, $v10.e3                              ## L:395  |    178 | clipBase.z = clipBase.w;
  vch $v29, $v09, $v11.e3                            ## L:396  |  **181 | newClipCode = clip(clipBase, clipPlaneW.w);
  vcl $v29, $v10, $v12.e3                            ## L:396  |    182 | newClipCode = clip(clipBase, clipPlaneW.w);
  cfc2 $t7, $vcc                                     ## L:396  |    183 | newClipCode = clip(clipBase, clipPlaneW.w);
  andi $t7, $t7, 775                                 ## L:253  |  **186 | res = clipCode & 0b0000'0011'0000'0111;
  srl $sp, $t7, 5                                    ## L:254  |    187 | u32 tmp = res >> 5;
  or $t7, $t7, $sp                                   ## L:255  |    188 | res |= tmp;
  sb $t7, -30($t3)                                   ## L:605  |    189 | store(lastClipCode:u8,   ptrWrite, 0x06);
  LABEL_clipTriangle_0005:
  vor $v04, $v00, $v06                               ## L:517  |      ^ | lastPosUV = currentPosUV;
  vor $v03, $v00, $v08                               ## L:519  |    190 | lastColor = currentColor;
  or $t7, $zero, $t9                                 ## L:518  |      ^ | lastClipCode = clipCode;
  beq $k1, $zero, LABEL_clipTriangle_000B            ## L:521  |    191 | if(isInsideCurr) {
  vor $v05, $v00, $v07                               ## L:517  |    192 | lastPosUV = currentPosUV;
  sdv $v04, 0, 16, $t3                               ## L:606  |    193 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  sb $t7, 6($t3)                                     ## L:605  |    194 | store(lastClipCode:u8,   ptrWrite, 0x06);
  suv $v03, 0, 8, $t3                                ## L:602  |    195 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  sdv $v05, 0, 24, $t3                               ## L:606  |    196 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  addiu $t3, $t3, 36                                 ## L:607  |    197 | ptrWrite += 36;
  slv $v04, 8, -24, $t3                              ## L:603  |    198 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  LABEL_clipTriangle_000B:
  addiu $t6, $t6, 36                                 ## L:525  |    199 | ptrRead += 36;
  bne $t6, $t8, LABEL_clipTriangle_0003              ## L:525  |    200 | ptrRead += 36;
  nop                                                ## L:525  |    201 | ptrRead += 36;
  LABEL_clipTriangle_0004:
  subu $t0, $t3, $t2                                 ## L:530  |    202 | arraySize = ptrWrite - ptrBuff1;
  beq $t0, $zero, CLIP_RETURN                        ## L:532  |    203 | if(arraySize == 0)goto CLIP_RETURN;
  sll $t5, $t5, 1                                    ## L:529  |    204 | planeIdx <<= 1;
  addiu $at, $zero, 32                               ## L:534  |    205 | planePtr += 0x04;
  bne $t5, $at, LABEL_clipTriangle_0001              ## L:534  |    206 | planePtr += 0x04;
  addiu $t4, $t4, 4                                  ## L:534  |    207 | planePtr += 0x04;
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:539  |    208 | u32<$s6> ptrVertIn = CLIP_BUFFER_RESULT;
  addu $s5, $s6, $t0                                 ## L:540  |    209 | u32<$s5> ptrVertInEnd = ptrVertIn + arraySize;
  bgezal $zero, triToScreenSpace                     ## L:545  |    210 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s5, $s5, 65464                              ## L:541  |    211 | ptrVertInEnd -= 72;
  bgezal $zero, triToScreenSpace                     ## L:547  |    212 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:546  |    213 | ptrVertIn += 72;
  LABEL_clipTriangle_000D:
  bgezal $zero, triToScreenSpace                     ## L:553  |    214 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:552  |    215 | ptrVertIn += 72;
  ori $a0, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:555  |    216 | ptrVertA = CLIP_BUFFER_RESULT;
  addiu $a2, $s6, 36                                 ## L:557  |    217 | ptrVertC = ptrVertIn + 36;
  jal RDPQ_Triangle_Send_Async                       ## L:560  |    218 | RDPQ_Triangle_Send_Async(ptrVertA, ptrVertB, ptrVertC, faceCull); ## Args: $a0, $a1, $a2, $v0
  or $a1, $zero, $s6                                 ## L:556  |    219 | ptrVertB = ptrVertIn;
  jal RDPQ_Triangle_Send_End                         ## L:561  |    220 | RDPQ_Triangle_Send_End();
  nop                                                ## L:561  |    221 | RDPQ_Triangle_Send_End();
  RDPQ_Triangle_Clip:
  bne $s6, $s5, LABEL_clipTriangle_000D              ## L:562  |    222 | RDPQ_Triangle_Clip:
  nop                                                ## L:562  |    223 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $t6, %lo(CLIP_RA + 0)                          ## L:299  |    224 | u16 tmp = load(CLIP_RA);
  j $t6                                              ## L:302  |  **227 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:301  |   *229 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);
triToScreenSpace:
  ## hot-path cycles: 32
  ldv $v07, 0, 16, $s6                               ## L:630  |      ^ | vec32 posClip = load(ptr, 0x10).xyzw;
  ldv $v08, 0, 24, $s6                               ## L:630  |      2 | vec32 posClip = load(ptr, 0x10).xyzw;
  ori $at, $zero, %lo(SCREEN_SCALE)                  ## L:617  |      3 | vec32 screenSize = load(SCREEN_SCALE).xyzw;
  vxor $v03, $v00, $v30.e7                           ## L:619  |      ^ | vec16 vecOne = 1;
  ldv $v02, 0, 8, $at                                ## L:617  |      4 | vec32 screenSize = load(SCREEN_SCALE).xyzw;
  ldv $v01, 0, 0, $at                                ## L:617  |      5 | vec32 screenSize = load(SCREEN_SCALE).xyzw;
  vrcph $v05.e3, $v07.e3                             ## L:631  |      ^ | invW.w = invert_half(posClip).w;
  lhu $t0, %lo(RSPQ_SCRATCH_MEM + 20)                ## L:623  |      6 | u16<$t0> oldClipFlag = load(RSPQ_SCRATCH_MEM, 20);
  vrcpl $v06.e3, $v08.e3                             ## L:631  |      ^ | invW.w = invert_half(posClip).w;
  ori $at, $zero, %lo(SCREEN_OFFSET)                 ## L:627  |      7 | vec16 screenOffset = load(SCREEN_OFFSET).xyzw;
  vrcph $v05.e3, $v00.e3                             ## L:631  |      ^ | invW.w = invert_half(posClip).w;
  ssv $v06, 6, 34, $s6                               ## L:632  |   **10 | store(invW.w, ptr, 0x20);
  vmudl $v29, $v08, $v06.e3                          ## L:634  |      ^ | posClip *= invW.w;
  vmadm $v29, $v07, $v06.e3                          ## L:634  |     11 | posClip *= invW.w;
  vmadn $v08, $v08, $v05.e3                          ## L:634  |     12 | posClip *= invW.w;
  vmadh $v07, $v07, $v05.e3                          ## L:634  |     13 | posClip *= invW.w;
  ldv $v04, 0, 0, $at                                ## L:627  |      ^ | vec16 screenOffset = load(SCREEN_OFFSET).xyzw;
  andi $t0, $t0, 57344                               ## L:624  |   **16 | oldClipFlag = oldClipFlag & ((0b111'00000) << 8);
  vmudl $v29, $v08, $v02.v                           ## L:635  |      ^ | posClip *= screenSize;
  ori $t0, $t0, 255                                  ## L:625  |     17 | oldClipFlag |= 0xFF;
  vmadm $v29, $v07, $v02.v                           ## L:635  |      ^ | posClip *= screenSize;
  vmadn $v08, $v08, $v01.v                           ## L:635  |     18 | posClip *= screenSize;
  vmadh $v07, $v07, $v01.v                           ## L:635  |     19 | posClip *= screenSize;
  ssv $v05, 6, 32, $s6                               ## L:632  |      ^ | store(invW.w, ptr, 0x20);
  vmadh $v07, $v04, $v30.e7                          ## L:637  |     20 | posClip:sint = screenOffset:sint +* 1;
  vge $v09, $v07, $v00.e0                            ## L:638  |  ***24 | vec16 zClamp = posClip:sint >= 0;
  sdv $v07, 0, 0, $s6                                ## L:640  |      ^ | @Barrier("cc") store(posClip:sint.xyzw, ptr, 0x00); ## Barrier: 0x4
  ssv $v09, 4, 4, $s6                                ## L:641  |  ***28 | @Barrier("cc") store(zClamp.z, ptr, 0x04); ## Barrier: 0x4
  sh $t0, 6($s6)                                     ## L:642  |     29 | @Barrier("cc") store(oldClipFlag, ptr, 0x06); ## Barrier: 0x4
  jr $ra                                             ## L:645  |     30 | }
  addiu $s6, $s6, 65500                              ## L:644  |    *32 | ptr -= 36;
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