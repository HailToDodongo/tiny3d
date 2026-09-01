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
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 4
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1, 0, -1
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
    LIGHT_DIR_COLOR: .ds.b 112
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
    CLIP_BUFFER_RESULT: .ds.b 252
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

.align 3
clipTriangle:
  ## hot-path cycles: 229
  ldv $v01, 0, 0, $a0                                ## L:575  |      ^ | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v03, 0, 16, $a0                               ## L:577  |      2 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ldv $v04, 0, 24, $a0                               ## L:578  |      3 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ldv $v02, 0, 8, $a0                                ## L:576  |      4 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ori $t3, $zero, %lo(CLIP_BUFFER_TMP)               ## L:419  |      5 | u32 ptrWrite = CLIP_BUFFER_TMP;
  ori $t1, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:417  |      6 | u32 ptrBuff0 = CLIP_BUFFER_RESULT;
  sdv $v04, 0, 24, $t3                               ## L:583  |      7 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v02, 0, 8, $t3                                ## L:581  |      8 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v01, 0, 0, $t3                                ## L:580  |      9 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v04, 0, 24, $a1                               ## L:578  |     10 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  sdv $v03, 0, 16, $t3                               ## L:582  |     11 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v03, 0, 16, $a1                               ## L:577  |     12 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ldv $v01, 0, 0, $a1                                ## L:575  |     13 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v02, 0, 8, $a1                                ## L:576  |     14 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  addiu $t3, $t3, 36                                 ## L:585  |     15 | ptrDst += 36;
  ori $t2, $zero, %lo(CLIP_BUFFER_TMP)               ## L:418  |     16 | u32 ptrBuff1 = CLIP_BUFFER_TMP;
  sdv $v03, 0, 16, $t3                               ## L:582  |     17 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v02, 0, 8, $t3                                ## L:581  |     18 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v04, 0, 24, $t3                               ## L:583  |     19 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v01, 0, 0, $t3                                ## L:580  |     20 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v02, 0, 8, $a2                                ## L:576  |     21 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v03, 0, 16, $a2                               ## L:577  |     22 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ldv $v01, 0, 0, $a2                                ## L:575  |     23 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v04, 0, 24, $a2                               ## L:578  |     24 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t3, $t3, 36                                 ## L:585  |     25 | ptrDst += 36;
  addiu $t0, $zero, 108                              ## L:413  |     26 | arraySize = 108;
  sdv $v02, 0, 8, $t3                                ## L:581  |     27 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v01, 0, 0, $t3                                ## L:580  |     28 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v03, 0, 16, $t3                               ## L:582  |     29 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v04, 0, 24, $t3                               ## L:583  |     30 | store(tmp4.xyzw, ptrDst, 0x18);
  addiu $t3, $t3, 36                                 ## L:585  |     31 | ptrDst += 36;
  POST_VTX_COPY:
  lsv $v01, 2, 42, $t3                               ## L:438  |     32 | codes.y = load(ptrWrite, 42).x;
  lsv $v01, 4, 78, $t3                               ## L:439  |     33 | codes.z = load(ptrWrite, 78).x;
  lsv $v01, 0, 6, $t3                                ## L:437  |     34 | codes.x = load(ptrWrite, 6).x;
  vxor $v02, $v01, $v30.e4                           ## L:440  |  ***38 | vec16 wBit = codes ^ 0b1000;
  lbu $t6, 6($a0)                                    ## L:456  |      ^ | u8 oldClip = load(ptrVertA, 0x06);
  sb $t6, %lo(20 + RSPQ_SCRATCH_MEM)($zero)          ## L:457  |   **41 | store(oldClip, RSPQ_SCRATCH_MEM, 20);
  vand $v03, $v01, $v31.e5                           ## L:441  |      ^ | vec16 oldBit = codes & 0b100'0000'0000;
  vand $v02, $v02, $v30.e4                           ## L:442  |     42 | wBit &= 0b1000;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:461  |      ^ | normScaleWInv.x = load(NORM_SCALE_W).w;
  ori $t4, $zero, %lo(CLIPPING_PLANES)               ## L:451  |   **45 | u32 planePtr = CLIPPING_PLANES;
  vxor $v01, $v01, $v03.v                            ## L:443  |      ^ | codes ^= oldBit;
  vmudn $v02, $v02, $v30.e0                          ## L:444  |     46 | wBit <<= 7;
  addiu $t5, $zero, 1                                ## L:452  |      ^ | s32 planeIdx = 1;
  vor $v01, $v01, $v02.v                             ## L:445  |  ***50 | codes |= wBit;
  ssv $v01, 0, 6, $t3                                ## L:446  |  ***54 | store(codes.x, ptrWrite, 6);
  ssv $v01, 4, 78, $t3                               ## L:448  |     55 | store(codes.z, ptrWrite, 78);
  ssv $v01, 2, 42, $t3                               ## L:447  |     56 | store(codes.y, ptrWrite, 42);
  lsv $v01, 0, 6, $at                                ## L:461  |     57 | normScaleWInv.x = load(NORM_SCALE_W).w;
  vor $v01, $v00, $v01.e0                            ## L:462  |  ***61 | normScaleWInv = normScaleWInv.x;
  ori $at, $zero, 0xFFFF                             ## L:463  |      ^ | normScaleWInv.w = 0xFFFF;
  mtc2 $at, $v01.e3                                  ## L:463  |     62 | normScaleWInv.w = 0xFFFF;
  ori $at, $zero, 0xFFFF                             ## L:464  |     63 | normScaleWInv.W = 0xFFFF;
  mtc2 $at, $v01.e7                                  ## L:464  |    *65 | normScaleWInv.W = 0xFFFF;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:466  |     66 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  llv $v02, 0, 0, $at                                ## L:466  |     67 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  vor $v02, $v00, $v02.e1                            ## L:467  |  ***71 | guardBandScale = guardBandScale.y;
  LABEL_clipTriangle_0001:
  addiu $t6, $t3, 65500                              ## L:473  |      ^ | u32 ptrLast = ptrWrite - 36;
  ldv $v05, 0, 24, $t6                               ## L:476  |     72 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  xor $t1, $t1, $t2                                  ## L:482  |     73 | swap(ptrBuff0, ptrBuff1);
  xor $t2, $t1, $t2                                  ## L:482  |     74 | swap(ptrBuff0, ptrBuff1);
  xor $t1, $t1, $t2                                  ## L:482  |     75 | swap(ptrBuff0, ptrBuff1);
  luv $v03, 0, 8, $t6                                ## L:475  |     76 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  ldv $v04, 0, 16, $t6                               ## L:476  |     77 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  lbu $t7, 6($t6)                                    ## L:478  |     78 | u8 lastClipCode   = load(ptrLast, 0x06);
  addu $t8, $t1, $t0                                 ## L:486  |     79 | u32 ptrReadEnd = ptrBuff0 + arraySize;
  llv $v04, 8, 12, $t6                               ## L:477  |     80 | lastPosUV:sint.XY = load(ptrLast, 0x0C).xy;
  or $t6, $zero, $t1                                 ## L:485  |     81 | u32 ptrRead = ptrBuff0;
  or $t3, $zero, $t2                                 ## L:484  |     82 | ptrWrite = ptrBuff1;
  LABEL_clipTriangle_0003:
  llv $v06, 8, 12, $t6                               ## L:492  |     83 | currentPosUV:sint.XY = load(ptrRead, 0x0C).xy;
  lbu $t9, 6($t6)                                    ## L:495  |     84 | u8 clipCode = load(ptrRead, 0x06);
  ldv $v06, 0, 16, $t6                               ## L:491  |     85 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  ldv $v07, 0, 24, $t6                               ## L:491  |     86 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  xor $k0, $t9, $t7                                  ## L:498  |     87 | u8 exclIntersect = clipCode ^ lastClipCode;
  and $k0, $k0, $t5                                  ## L:500  |     88 | exclIntersect &= planeIdx;
  nor $k1, $zero, $t9                                ## L:499  |     89 | u8 isInsideCurr = ~clipCode;
  and $k1, $k1, $t5                                  ## L:501  |     90 | isInsideCurr &= planeIdx;
  beq $k0, $zero, LABEL_clipTriangle_0005            ## L:503  |     91 | if(exclIntersect)
  luv $v08, 0, 8, $t6                                ## L:493  |     92 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  and $t7, $t7, $t5                                  ## L:505  |     93 | lastClipCode &= planeIdx;
  lpv $v14, 0, 0, $t4                                ## L:329  |     94 | vec32 planeNorm:sint = load_vec_s8(planePtr);
  beq $t7, $zero, LABEL_clipTriangle_0006            ## L:334  |     95 | if(newClipCode)
  nop                                                ## L:334  |     96 | if(newClipCode)
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:336  |     97 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:337  |      ^ | planeNorm:sfract = 0;
  sdv $v14, 0, 0, $at                                ## L:336  |     98 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v07                               ## L:340  |      ^ | vpos = pos;
  vsubc $v10, $v05, $v07.v                           ## L:339  |     99 | posUVDiff = posBase - pos;
  ldv $v14, 8, 0, $at                                ## L:336  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsub $v09, $v04, $v06.v                            ## L:339  |    100 | posUVDiff = posBase - pos;
  sdv $v04, 0, 0, $at                                ## L:341  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v06                               ## L:340  |    101 | vpos = pos;
  vmudl $v15, $v15, $v31.e7                          ## L:342  |    102 | planeNorm >>= 8;
  sdv $v05, 0, 8, $at                                ## L:341  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadm $v14, $v14, $v31.e7                          ## L:342  |    103 | planeNorm >>= 8;
  ldv $v11, 8, 0, $at                                ## L:341  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:342  |    104 | planeNorm >>= 8;
  ldv $v12, 8, 8, $at                                ## L:341  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  beq $zero, $zero, LABEL_clipTriangle_0007          ## L:344  |    105 | colorDiff = colorBase - color;
  vsubc $v13, $v03, $v08.v                           ## L:344  |   *107 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0006:
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:346  |      1 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v04                               ## L:350  |      ^ | vpos = posBase;
  sdv $v14, 0, 0, $at                                ## L:346  |      2 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:347  |      ^ | planeNorm:sfract = 0;
  vsubc $v10, $v07, $v05.v                           ## L:349  |      3 | posUVDiff = pos - posBase;
  ldv $v14, 8, 0, $at                                ## L:346  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v05                               ## L:350  |      4 | vpos = posBase;
  sdv $v06, 0, 0, $at                                ## L:351  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsub $v09, $v06, $v04.v                            ## L:349  |      5 | posUVDiff = pos - posBase;
  vmudl $v15, $v15, $v31.e7                          ## L:352  |      6 | planeNorm >>= 8;
  vmadm $v14, $v14, $v31.e7                          ## L:352  |      7 | planeNorm >>= 8;
  sdv $v07, 0, 8, $at                                ## L:351  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:352  |      8 | planeNorm >>= 8;
  ldv $v11, 8, 0, $at                                ## L:351  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsubc $v13, $v08, $v03.v                           ## L:354  |      9 | colorDiff = color - colorBase;
  ldv $v12, 8, 8, $at                                ## L:351  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  LABEL_clipTriangle_0007:
  vmudl $v15, $v15, $v01.v                           ## L:357  | **************************************************************************************************108 | planeNorm *= normScaleWInv:ufract;
  vmadm $v14, $v14, $v01.v                           ## L:357  |    109 | planeNorm *= normScaleWInv:ufract;
  vmadn $v15, $v00, $v00                             ## L:357  |    110 | planeNorm *= normScaleWInv:ufract;
  vmudl $v29, $v12, $v15.v                           ## L:225  | ***114 | res = a * b;
  vmadm $v29, $v11, $v15.v                           ## L:225  |    115 | res = a * b;
  vmadn $v17, $v12, $v14.v                           ## L:225  |    116 | res = a * b;
  vmadh $v16, $v11, $v14.v                           ## L:225  |    117 | res = a * b;
  vaddc $v17, $v17, $v17.q1                          ## L:226  |  **120 | res += res.yywwYYWW;
  vadd $v16, $v16, $v16.q1                           ## L:226  |    121 | res += res.yywwYYWW;
  vaddc $v17, $v17, $v17.h2                          ## L:227  |  **124 | res += res.zzzzZZZZ;
  vadd $v16, $v16, $v16.h2                           ## L:227  |    125 | res += res.zzzzZZZZ;
  vsubc $v19, $v17, $v17.e4                          ## L:360  |  **128 | vec32 fac = dot - dot.X;
  vsub $v18, $v16, $v16.e4                           ## L:360  |    129 | vec32 fac = dot - dot.X;
  vrcph $v18.e0, $v18.e0                             ## L:362  | ***133 | fac.x = invert_half(fac).x;
  vrcpl $v19.e0, $v19.e0                             ## L:362  |    134 | fac.x = invert_half(fac).x;
  vrcph $v18.e0, $v00.e0                             ## L:362  |    135 | fac.x = invert_half(fac).x;
  vaddc $v19, $v19, $v19.v                           ## L:363  |  **138 | fac += fac;
  vadd $v18, $v18, $v18.v                            ## L:363  |    139 | fac += fac;
  vmudl $v29, $v19, $v17.e0                          ## L:364  |  **142 | fac *= dot.x;
  vmadm $v29, $v18, $v17.e0                          ## L:364  |    143 | fac *= dot.x;
  vmadn $v19, $v19, $v16.e0                          ## L:364  |    144 | fac *= dot.x;
  vmadh $v18, $v18, $v16.e0                          ## L:364  |    145 | fac *= dot.x;
  mfc2 $sp, $v18.e0                                  ## L:366  | ***149 | s16 facX = fac:sint.x;
  beq $sp, $zero, LABEL_clipTriangle_0008            ## L:367  |  **152 | if(facX != 0) {
  nop                                                ## L:367  |    153 | if(facX != 0) {
  vlt $v29, $v18, $v30.e7                            ## L:371  |    154 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v19, $v19, $v01.e3                           ## L:371  |    155 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v18, $v00.e0                            ## L:372  |    156 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v19, $v19, $v00                              ## L:372  |  **159 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0008:
  beq $t7, $zero, LABEL_clipTriangle_0009            ## L:377  |      ^ | if(newClipCode) {
  vmudm $v13, $v13, $v19.e0                          ## L:375  | ***163 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v17, $v10, $v19.e0                          ## L:379  |    164 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:379  |    165 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:379  |    166 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v07, $v30.e7                          ## L:380  |    167 | resPosUV = pos +* 1;
  vmadh $v04, $v06, $v30.e7                          ## L:380  |    168 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000A          ## L:381  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v03, $v08, $v13.v                           ## L:381  |   *170 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_0009:
  vmudl $v17, $v10, $v19.e0                          ## L:383  |      1 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:383  |      2 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:383  |      3 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v05, $v30.e7                          ## L:384  |      4 | resPosUV = posBase +* 1;
  vmadh $v04, $v04, $v30.e7                          ## L:384  |      5 | resPosUV = posBase +* 1;
  vaddc $v03, $v03, $v13.v                           ## L:385  |      6 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000A:
  sdv $v05, 0, 24, $t3                               ## L:603  | ********************************************************************************************************************************************************************171 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  vmudl $v10, $v05, $v01.v                           ## L:389  |      ^ | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v09, $v04, $v01.v                           ## L:389  |    172 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadn $v10, $v00, $v00                             ## L:389  |    173 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmudm $v12, $v02, $v05.h3                          ## L:390  |    174 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  suv $v03, 0, 8, $t3                                ## L:599  |      ^ | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  vmadh $v11, $v02, $v04.h3                          ## L:390  |    175 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadn $v12, $v00, $v00                             ## L:390  |    176 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmov $v09.e2, $v09.e3                              ## L:392  |    177 | clipBase.z = clipBase.w;
  vmov $v10.e2, $v10.e3                              ## L:392  |    178 | clipBase.z = clipBase.w;
  vch $v29, $v09, $v11.e3                            ## L:393  |  **181 | newClipCode = clip(clipBase, clipPlaneW.w);
  slv $v04, 8, 12, $t3                               ## L:600  |      ^ | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v04, 0, 16, $t3                               ## L:603  |    182 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  vcl $v29, $v10, $v12.e3                            ## L:393  |      ^ | newClipCode = clip(clipBase, clipPlaneW.w);
  cfc2 $t7, $vcc                                     ## L:393  |    183 | newClipCode = clip(clipBase, clipPlaneW.w);
  addiu $t3, $t3, 36                                 ## L:604  |    184 | ptrWrite += 36;
  andi $t7, $t7, 775                                 ## L:250  |   *186 | res = clipCode & 0b0000'0011'0000'0111;
  srl $sp, $t7, 5                                    ## L:251  |    187 | u32 tmp = res >> 5;
  or $t7, $t7, $sp                                   ## L:252  |    188 | res |= tmp;
  sb $t7, -30($t3)                                   ## L:602  |    189 | store(lastClipCode:u8,   ptrWrite, 0x06);
  LABEL_clipTriangle_0005:
  vor $v03, $v00, $v08                               ## L:516  |      ^ | lastColor = currentColor;
  or $t7, $zero, $t9                                 ## L:515  |    190 | lastClipCode = clipCode;
  vor $v04, $v00, $v06                               ## L:514  |      ^ | lastPosUV = currentPosUV;
  beq $k1, $zero, LABEL_clipTriangle_000B            ## L:518  |    191 | if(isInsideCurr) {
  vor $v05, $v00, $v07                               ## L:514  |    192 | lastPosUV = currentPosUV;
  suv $v03, 0, 8, $t3                                ## L:599  |    193 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  slv $v04, 8, 12, $t3                               ## L:600  |    194 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v04, 0, 16, $t3                               ## L:603  |    195 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  sdv $v05, 0, 24, $t3                               ## L:603  |    196 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  addiu $t3, $t3, 36                                 ## L:604  |    197 | ptrWrite += 36;
  sb $t7, -30($t3)                                   ## L:602  |    198 | store(lastClipCode:u8,   ptrWrite, 0x06);
  LABEL_clipTriangle_000B:
  addiu $t6, $t6, 36                                 ## L:522  |    199 | ptrRead += 36;
  bne $t6, $t8, LABEL_clipTriangle_0003              ## L:522  |    200 | ptrRead += 36;
  nop                                                ## L:522  |    201 | ptrRead += 36;
  LABEL_clipTriangle_0004:
  subu $t0, $t3, $t2                                 ## L:527  |    202 | arraySize = ptrWrite - ptrBuff1;
  beq $t0, $zero, CLIP_RETURN                        ## L:529  |    203 | if(arraySize == 0)goto CLIP_RETURN;
  sll $t5, $t5, 1                                    ## L:526  |    204 | planeIdx <<= 1;
  addiu $at, $zero, 32                               ## L:531  |    205 | planePtr += 0x04;
  bne $t5, $at, LABEL_clipTriangle_0001              ## L:531  |    206 | planePtr += 0x04;
  addiu $t4, $t4, 4                                  ## L:531  |    207 | planePtr += 0x04;
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:536  |    208 | u32<$s6> ptrVertIn = CLIP_BUFFER_RESULT;
  addu $s5, $s6, $t0                                 ## L:537  |    209 | u32<$s5> ptrVertInEnd = ptrVertIn + arraySize;
  bgezal $zero, triToScreenSpace                     ## L:542  |    210 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s5, $s5, 65464                              ## L:538  |    211 | ptrVertInEnd -= 72;
  bgezal $zero, triToScreenSpace                     ## L:544  |    212 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:543  |    213 | ptrVertIn += 72;
  LABEL_clipTriangle_000D:
  bgezal $zero, triToScreenSpace                     ## L:550  |    214 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:549  |    215 | ptrVertIn += 72;
  addiu $a2, $s6, 36                                 ## L:554  |    216 | ptrVertC = ptrVertIn + 36;
  or $a1, $zero, $s6                                 ## L:553  |    217 | ptrVertB = ptrVertIn;
  jal RDPQ_Triangle_Send_Async                       ## L:557  |    218 | RDPQ_Triangle_Send_Async(ptrVertA, ptrVertB, ptrVertC, faceCull); ## Args: $a0, $a1, $a2, $v0
  ori $a0, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:552  |    219 | ptrVertA = CLIP_BUFFER_RESULT;
  jal RDPQ_Triangle_Send_End                         ## L:558  |    220 | RDPQ_Triangle_Send_End();
  nop                                                ## L:558  |    221 | RDPQ_Triangle_Send_End();
  RDPQ_Triangle_Clip:
  bne $s6, $s5, LABEL_clipTriangle_000D              ## L:559  |    222 | RDPQ_Triangle_Clip:
  nop                                                ## L:559  |    223 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $t6, %lo(CLIP_RA + 0)                          ## L:296  |    224 | u16 tmp = load(CLIP_RA);
  j $t6                                              ## L:299  |  **227 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:298  |   *229 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);
triToScreenSpace:
  ## hot-path cycles: 35
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:614  |      ^ | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v01, 0, 0, $at                                ## L:614  |      2 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v08, 0, 24, $s6                               ## L:629  |      3 | vec32 posClip = load(ptr, 0x10).xyzw;
  ldv $v07, 0, 16, $s6                               ## L:629  |      4 | vec32 posClip = load(ptr, 0x10).xyzw;
  vmudl $v02, $v00, $v31.e7                          ## L:616  |      ^ | screenSize >>= 8;
  vmadm $v01, $v01, $v31.e7                          ## L:616  |     *6 | screenSize >>= 8;
  ldv $v04, 0, 8, $at                                ## L:626  |      ^ | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzw;
  vmadn $v02, $v00, $v00                             ## L:616  |      7 | screenSize >>= 8;
  vrcph $v05.e3, $v07.e3                             ## L:630  |      8 | invW.w = invert_half(posClip).w;
  lhu $t0, %lo(RSPQ_SCRATCH_MEM + 20)                ## L:622  |      ^ | u16<$t0> oldClipFlag = load(RSPQ_SCRATCH_MEM, 20);
  vrcpl $v06.e3, $v08.e3                             ## L:630  |      9 | invW.w = invert_half(posClip).w;
  vxor $v03, $v00, $v30.e7                           ## L:618  |     10 | vec16 vecOne = 1;
  vrcph $v05.e3, $v00.e3                             ## L:630  |     11 | invW.w = invert_half(posClip).w;
  andi $t0, $t0, 57344                               ## L:623  |      ^ | oldClipFlag = oldClipFlag & ((0b111'00000) << 8);
  vmudl $v29, $v08, $v06.e3                          ## L:633  |    *13 | posClip *= invW.w;
  ssv $v06, 6, 34, $s6                               ## L:631  |      ^ | store(invW.w, ptr, 0x20);
  ori $t0, $t0, 255                                  ## L:624  |     14 | oldClipFlag |= 0xFF;
  vmadm $v29, $v07, $v06.e3                          ## L:633  |      ^ | posClip *= invW.w;
  vmadn $v08, $v08, $v05.e3                          ## L:633  |     15 | posClip *= invW.w;
  vmadh $v07, $v07, $v05.e3                          ## L:633  |     16 | posClip *= invW.w;
  vmudl $v29, $v08, $v02.v                           ## L:634  |   **19 | posClip *= screenSize;
  vmadm $v29, $v07, $v02.v                           ## L:634  |     20 | posClip *= screenSize;
  vmadn $v08, $v08, $v01.v                           ## L:634  |     21 | posClip *= screenSize;
  vmadh $v07, $v07, $v01.v                           ## L:634  |     22 | posClip *= screenSize;
  ssv $v05, 6, 32, $s6                               ## L:631  |      ^ | store(invW.w, ptr, 0x20);
  vmadh $v07, $v04, $v30.e7                          ## L:636  |     23 | posClip:sint = screenOffset:sint +* 1;
  vge $v09, $v07, $v00.e0                            ## L:637  |  ***27 | vec16 zClamp = posClip:sint >= 0;
  sdv $v07, 0, 0, $s6                                ## L:639  |      ^ | @Barrier("cc") store(posClip:sint.xyzw, ptr, 0x00); ## Barrier: 0x4
  ssv $v09, 4, 4, $s6                                ## L:640  |  ***31 | @Barrier("cc") store(zClamp.z, ptr, 0x04); ## Barrier: 0x4
  sh $t0, 6($s6)                                     ## L:641  |     32 | @Barrier("cc") store(oldClipFlag, ptr, 0x06); ## Barrier: 0x4
  jr $ra                                             ## L:644  |     33 | }
  addiu $s6, $s6, 65500                              ## L:643  |    *35 | ptr -= 36;

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