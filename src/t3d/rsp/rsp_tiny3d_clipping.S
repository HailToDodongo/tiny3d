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
clipTriangle:
  ldv $v02, 0, 8, $a0                                ## L:536  |      ^ | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v01, 0, 0, $a0                                ## L:535  |      2 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v04, 0, 24, $a0                               ## L:538  |      3 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ldv $v03, 0, 16, $a0                               ## L:537  |      4 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ori $t3, $zero, %lo(CLIP_BUFFER_TMP)               ## L:402  |      5 | u32 ptrWrite = CLIP_BUFFER_TMP;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:421  |      6 | normScaleWInv.x = load(NORM_SCALE_W).w;
  sdv $v01, 0, 0, $t3                                ## L:540  |      7 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v02, 0, 8, $t3                                ## L:541  |      8 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v04, 0, 24, $t3                               ## L:543  |      9 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v03, 0, 16, $t3                               ## L:542  |     10 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v01, 0, 0, $a1                                ## L:535  |     11 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v03, 0, 16, $a1                               ## L:537  |     12 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ldv $v02, 0, 8, $a1                                ## L:536  |     13 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v04, 0, 24, $a1                               ## L:538  |     14 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t3, $t3, 36                                 ## L:545  |     15 | ptrDst += 36;
  ori $t1, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:400  |     16 | u32 ptrBuff0 = CLIP_BUFFER_RESULT;
  sdv $v03, 0, 16, $t3                               ## L:542  |     17 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v04, 0, 24, $t3                               ## L:543  |     18 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v02, 0, 8, $t3                                ## L:541  |     19 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v01, 0, 0, $t3                                ## L:540  |     20 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v04, 0, 24, $a2                               ## L:538  |     21 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ldv $v01, 0, 0, $a2                                ## L:535  |     22 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ori $t4, $zero, %lo(CLIPPING_PLANES)               ## L:411  |     23 | u32 planePtr = CLIPPING_PLANES;
  addiu $t3, $t3, 36                                 ## L:545  |     24 | ptrDst += 36;
  ldv $v03, 0, 16, $a2                               ## L:537  |     25 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:540  |     26 | store(tmp1.xyzw, ptrDst, 0x00);
  lbu $t6, 6($a0)                                    ## L:416  |     27 | u8 oldClip = load(ptrVertA, 0x06);
  lsv $v01, 0, 6, $at                                ## L:421  |     28 | normScaleWInv.x = load(NORM_SCALE_W).w;
  ldv $v02, 0, 8, $a2                                ## L:536  |     29 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  addiu $t5, $zero, 1                                ## L:412  |     30 | s32 planeIdx = 1;
  ori $at, $zero, 0xFFFF                             ## L:423  |     31 | normScaleWInv.w = 0xFFFF;
  sdv $v04, 0, 24, $t3                               ## L:543  |     32 | store(tmp4.xyzw, ptrDst, 0x18);
  vor $v01, $v00, $v01.e0                            ## L:422  |      ^ | normScaleWInv = normScaleWInv.x;
  mtc2 $at, $v01.e3                                  ## L:423  |     33 | normScaleWInv.w = 0xFFFF;
  mtc2 $at, $v01.e7                                  ## L:424  |     34 | normScaleWInv.W = 0xFFFF;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:426  |     35 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  ori $t2, $zero, %lo(CLIP_BUFFER_TMP)               ## L:401  |     36 | u32 ptrBuff1 = CLIP_BUFFER_TMP;
  sdv $v02, 0, 8, $t3                                ## L:541  |     37 | store(tmp2.xyzw, ptrDst, 0x08);
  llv $v02, 0, 0, $at                                ## L:426  |     38 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  sb $t6, %lo(20 + RSPQ_SCRATCH_MEM)($zero)          ## L:417  |     39 | store(oldClip, RSPQ_SCRATCH_MEM, 20);
  addiu $t0, $zero, 108                              ## L:396  |     40 | arraySize = 108;
  sdv $v03, 0, 16, $t3                               ## L:542  |     41 | store(tmp3.xyzw, ptrDst, 0x10);
  addiu $t3, $t3, 36                                 ## L:545  |     42 | ptrDst += 36;
  vor $v02, $v00, $v02.e1                            ## L:427  |      ^ | guardBandScale = guardBandScale.y;
  LABEL_clipTriangle_0001:
  xor $t1, $t1, $t2                                  ## L:442  |     43 | swap(ptrBuff0, ptrBuff1);
  xor $t2, $t1, $t2                                  ## L:442  |     44 | swap(ptrBuff0, ptrBuff1);
  addiu $t6, $t3, 65500                              ## L:433  |     45 | u32 ptrLast = ptrWrite - 36;
  ldv $v04, 0, 16, $t6                               ## L:436  |     46 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  xor $t1, $t1, $t2                                  ## L:442  |     47 | swap(ptrBuff0, ptrBuff1);
  luv $v03, 0, 8, $t6                                ## L:435  |     48 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  lbu $t7, 6($t6)                                    ## L:438  |     49 | u8 lastClipCode   = load(ptrLast, 0x06);
  ldv $v05, 0, 24, $t6                               ## L:436  |     50 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  or $t3, $zero, $t2                                 ## L:444  |     51 | ptrWrite = ptrBuff1;
  llv $v04, 8, 12, $t6                               ## L:437  |     52 | lastPosUV:sint.XY = load(ptrLast, 0x0C).xy;
  addu $t8, $t1, $t0                                 ## L:446  |     53 | u32 ptrReadEnd = ptrBuff0 + arraySize;
  or $t6, $zero, $t1                                 ## L:445  |     54 | u32 ptrRead = ptrBuff0;
  LABEL_clipTriangle_0003:
  lbu $t9, 6($t6)                                    ## L:455  |     55 | u8 clipCode = load(ptrRead, 0x06);
  luv $v08, 0, 8, $t6                                ## L:453  |     56 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  ldv $v07, 0, 24, $t6                               ## L:451  |     57 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  nor $k1, $zero, $t9                                ## L:459  |     58 | u8 isInsideCurr = ~clipCode;
  xor $k0, $t9, $t7                                  ## L:458  |     59 | u8 exclIntersect = clipCode ^ lastClipCode;
  ldv $v06, 0, 16, $t6                               ## L:451  |     60 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  and $k0, $k0, $t5                                  ## L:460  |     61 | exclIntersect &= planeIdx;
  and $k1, $k1, $t5                                  ## L:461  |     62 | isInsideCurr &= planeIdx;
  beq $k0, $zero, LABEL_clipTriangle_0005            ## L:463  |     63 | if(exclIntersect)
  llv $v06, 8, 12, $t6                               ## L:452  |    *65 | currentPosUV:sint.XY = load(ptrRead, 0x0C).xy;
  lpv $v14, 0, 0, $t4                                ## L:313  |     66 | vec32 planeNorm:sint = load_vec_s8(planePtr);
  and $t7, $t7, $t5                                  ## L:465  |     67 | lastClipCode &= planeIdx;
  beq $t7, $zero, LABEL_clipTriangle_0006            ## L:318  |     68 | if(newClipCode)
  nop                                                ## L:318  |    *70 | if(newClipCode)
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:320  |     71 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsubc $v10, $v05, $v07.v                           ## L:323  |      ^ | posUVDiff = posBase - pos;
  sdv $v14, 0, 0, $at                                ## L:320  |     72 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:321  |      ^ | planeNorm:sfract = 0;
  vsub $v09, $v04, $v06.v                            ## L:323  |     73 | posUVDiff = posBase - pos;
  ldv $v14, 8, 0, $at                                ## L:320  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  sdv $v04, 0, 0, $at                                ## L:325  |     74 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v07                               ## L:324  |      ^ | vpos = pos;
  sdv $v05, 0, 8, $at                                ## L:325  |    *76 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v06                               ## L:324  |      ^ | vpos = pos;
  ldv $v11, 8, 0, $at                                ## L:325  |     77 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmudl $v15, $v15, $v31.e7                          ## L:326  |      ^ | planeNorm >>= 8;
  vmadm $v14, $v14, $v31.e7                          ## L:326  |     78 | planeNorm >>= 8;
  ldv $v12, 8, 8, $at                                ## L:325  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:326  |     79 | planeNorm >>= 8;
  beq $zero, $zero, LABEL_clipTriangle_0007          ## L:328  |      ^ | colorDiff = colorBase - color;
  vsubc $v13, $v03, $v08.v                           ## L:328  |    *81 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0006:
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:330  |     82 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v04                               ## L:334  |      ^ | vpos = posBase;
  sdv $v14, 0, 0, $at                                ## L:330  |     83 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:331  |      ^ | planeNorm:sfract = 0;
  vor $v12, $v00, $v05                               ## L:334  |     84 | vpos = posBase;
  ldv $v14, 8, 0, $at                                ## L:330  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsubc $v10, $v07, $v05.v                           ## L:333  |     85 | posUVDiff = pos - posBase;
  sdv $v06, 0, 0, $at                                ## L:335  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsub $v09, $v06, $v04.v                            ## L:333  |     86 | posUVDiff = pos - posBase;
  vmudl $v15, $v15, $v31.e7                          ## L:336  |     87 | planeNorm >>= 8;
  vmadm $v14, $v14, $v31.e7                          ## L:336  |     88 | planeNorm >>= 8;
  vmadn $v15, $v00, $v00                             ## L:336  |     89 | planeNorm >>= 8;
  sdv $v07, 0, 8, $at                                ## L:335  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsubc $v13, $v08, $v03.v                           ## L:338  |     90 | colorDiff = color - colorBase;
  ldv $v11, 8, 0, $at                                ## L:335  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  ldv $v12, 8, 8, $at                                ## L:335  |   **93 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  LABEL_clipTriangle_0007:
  vmudl $v15, $v15, $v01.v                           ## L:341  |      ^ | planeNorm *= normScaleWInv:ufract;
  vmadm $v14, $v14, $v01.v                           ## L:341  |     94 | planeNorm *= normScaleWInv:ufract;
  vmadn $v15, $v00, $v00                             ## L:341  |     95 | planeNorm *= normScaleWInv:ufract;
  vmudl $v29, $v12, $v15.v                           ## L:212  |  ***99 | res = a * b;
  vmadm $v29, $v11, $v15.v                           ## L:212  |    100 | res = a * b;
  vmadn $v17, $v12, $v14.v                           ## L:212  |    101 | res = a * b;
  vmadh $v16, $v11, $v14.v                           ## L:212  |    102 | res = a * b;
  vaddc $v17, $v17, $v17.q1                          ## L:213  |  **105 | res += res.yywwYYWW;
  vadd $v16, $v16, $v16.q1                           ## L:213  |    106 | res += res.yywwYYWW;
  vaddc $v17, $v17, $v17.h2                          ## L:214  |  **109 | res += res.zzzzZZZZ;
  vadd $v16, $v16, $v16.h2                           ## L:214  |    110 | res += res.zzzzZZZZ;
  vsubc $v19, $v17, $v17.e4                          ## L:344  |  **113 | vec32 fac = dot - dot.X;
  vsub $v18, $v16, $v16.e4                           ## L:344  |    114 | vec32 fac = dot - dot.X;
  vrcph $v18.e0, $v18.e0                             ## L:346  | ***118 | fac.x = invert_half(fac).x;
  vrcpl $v19.e0, $v19.e0                             ## L:346  |    119 | fac.x = invert_half(fac).x;
  vrcph $v18.e0, $v00.e0                             ## L:346  |    120 | fac.x = invert_half(fac).x;
  vaddc $v19, $v19, $v19.v                           ## L:347  |  **123 | fac += fac;
  vadd $v18, $v18, $v18.v                            ## L:347  |    124 | fac += fac;
  vmudl $v29, $v19, $v17.e0                          ## L:348  |  **127 | fac *= dot.x;
  vmadm $v29, $v18, $v17.e0                          ## L:348  |    128 | fac *= dot.x;
  vmadn $v19, $v19, $v16.e0                          ## L:348  |    129 | fac *= dot.x;
  vmadh $v18, $v18, $v16.e0                          ## L:348  |    130 | fac *= dot.x;
  mfc2 $sp, $v18.e0                                  ## L:350  | ***134 | s16 facX = fac:sint.x;
  beq $sp, $zero, LABEL_clipTriangle_0008            ## L:351  |  **137 | if(facX != 0) {
  nop                                                ## L:351  |   *139 | if(facX != 0) {
  vlt $v29, $v18, $v30.e7                            ## L:355  |    140 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v19, $v19, $v01.e3                           ## L:355  |    141 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v18, $v00.e0                            ## L:356  |    142 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v19, $v19, $v00                              ## L:356  |  **145 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0008:
  beq $t7, $zero, LABEL_clipTriangle_0009            ## L:361  |      ^ | if(newClipCode) {
  vmudm $v13, $v13, $v19.e0                          ## L:359  | ****150 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v17, $v10, $v19.e0                          ## L:363  |    151 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:363  |    152 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:363  |    153 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v07, $v30.e7                          ## L:364  |    154 | resPosUV = pos +* 1;
  vmadh $v04, $v06, $v30.e7                          ## L:364  |    155 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000A          ## L:365  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v03, $v08, $v13.v                           ## L:365  |   *157 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_0009:
  vmudl $v17, $v10, $v19.e0                          ## L:367  |    158 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:367  |    159 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:367  |    160 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v05, $v30.e7                          ## L:368  |    161 | resPosUV = posBase +* 1;
  vmadh $v04, $v04, $v30.e7                          ## L:368  |    162 | resPosUV = posBase +* 1;
  vaddc $v03, $v03, $v13.v                           ## L:369  |    163 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000A:
  vmudl $v10, $v05, $v01.v                           ## L:373  |   *165 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v09, $v04, $v01.v                           ## L:373  |    166 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadn $v10, $v00, $v00                             ## L:373  |    167 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  sdv $v04, 0, 16, $t3                               ## L:563  |      ^ | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  vmudm $v12, $v02, $v05.h3                          ## L:374  |    168 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadh $v11, $v02, $v04.h3                          ## L:374  |    169 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadn $v12, $v00, $v00                             ## L:374  |    170 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  suv $v03, 0, 8, $t3                                ## L:559  |      ^ | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  vch $v29, $v09, $v11.e3                            ## L:376  |  **173 | newClipCode = clip(clipBase, clipPlaneW.w);
  vcl $v29, $v10, $v12.e3                            ## L:376  |    174 | newClipCode = clip(clipBase, clipPlaneW.w);
  slv $v04, 8, 12, $t3                               ## L:560  |      ^ | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  cfc2 $t7, $vcc                                     ## L:376  |    175 | newClipCode = clip(clipBase, clipPlaneW.w);
  sdv $v05, 0, 24, $t3                               ## L:563  |    176 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  andi $t7, $t7, 775                                 ## L:236  |   *178 | res = clipCode & 0b0000'0011'0000'0111;
  srl $sp, $t7, 5                                    ## L:237  |    179 | u32 tmp = res >> 5;
  or $t7, $t7, $sp                                   ## L:238  |    180 | res |= tmp;
  sb $t7, 6($t3)                                     ## L:562  |    181 | store(lastClipCode:u8,   ptrWrite, 0x06);
  addiu $t3, $t3, 36                                 ## L:564  |    182 | ptrWrite += 36;
  LABEL_clipTriangle_0005:
  vor $v03, $v00, $v08                               ## L:476  |      ^ | lastColor = currentColor;
  vor $v05, $v00, $v07                               ## L:474  |    183 | lastPosUV = currentPosUV;
  or $t7, $zero, $t9                                 ## L:475  |      ^ | lastClipCode = clipCode;
  beq $k1, $zero, LABEL_clipTriangle_000B            ## L:478  |    184 | if(isInsideCurr) {
  vor $v04, $v00, $v06                               ## L:474  |   *186 | lastPosUV = currentPosUV;
  suv $v03, 0, 8, $t3                                ## L:559  |    187 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  sb $t7, 6($t3)                                     ## L:562  |    188 | store(lastClipCode:u8,   ptrWrite, 0x06);
  sdv $v05, 0, 24, $t3                               ## L:563  |    189 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  slv $v04, 8, 12, $t3                               ## L:560  |    190 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v04, 0, 16, $t3                               ## L:563  |    191 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  addiu $t3, $t3, 36                                 ## L:564  |    192 | ptrWrite += 36;
  LABEL_clipTriangle_000B:
  addiu $t6, $t6, 36                                 ## L:482  |    193 | ptrRead += 36;
  bne $t6, $t8, LABEL_clipTriangle_0003              ## L:482  |    194 | ptrRead += 36;
  LABEL_clipTriangle_0004:
  subu $t0, $t3, $t2                                 ## L:487  |    197 | arraySize = ptrWrite - ptrBuff1;
  beq $t0, $zero, CLIP_RETURN                        ## L:489  |    198 | if(arraySize == 0)goto CLIP_RETURN;
  sll $t5, $t5, 1                                    ## L:486  |   *200 | planeIdx <<= 1;
  addiu $at, $zero, 32                               ## L:491  |    201 | planePtr += 0x04;
  bne $t5, $at, LABEL_clipTriangle_0001              ## L:491  |    202 | planePtr += 0x04;
  addiu $t4, $t4, 4                                  ## L:491  |   *204 | planePtr += 0x04;
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:496  |    205 | u32<$s6> ptrVertIn = CLIP_BUFFER_RESULT;
  addu $s5, $s6, $t0                                 ## L:497  |    206 | u32<$s5> ptrVertInEnd = ptrVertIn + arraySize;
  bgezal $zero, triToScreenSpace                     ## L:502  |    207 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s5, $s5, 65464                              ## L:498  |   *209 | ptrVertInEnd -= 72;
  bgezal $zero, triToScreenSpace                     ## L:504  |    210 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:503  |   *212 | ptrVertIn += 72;
  LABEL_clipTriangle_000D:
  bgezal $zero, triToScreenSpace                     ## L:502  |    212 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:501  |   *214 | ptrVertIn += 72;
  or $a1, $zero, $s6                                 ## L:505  |    215 | ptrVertB = ptrVertIn;
  ori $a0, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:504  |    217 | ptrVertA = CLIP_BUFFER_RESULT;
  jal RDPQ_Triangle_Send_Async                       ## L:510  |    218 | RDPQ_Triangle_Send_Async(ptrVertA, ptrVertB, ptrVertC, cullDest, faceCull); ## Args: $a0, $a1, $a2, $sp, $v0
  addiu $a2, $s6, 36                                 ## L:506  |   *220 | ptrVertC = ptrVertIn + 36;
  j RDPQ_Triangle_Send_End                           ## L:511  |    221 | RDPQ_Triangle_Send_End();
  ori $ra, $zero, %lo(RDPQ_Triangle_Clip)
  RDPQ_Triangle_Clip:
  bne $s6, $s5, LABEL_clipTriangle_000D              ## L:519  |    224 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $at, %lo(CLIP_RA + 0)                          ## L:280  |    227 | u16 tmp = load(CLIP_RA);
  j $at                                              ## L:283  |  **230 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:282  |   *232 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);
triToScreenSpace:
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:574  |      ^ | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v01, 0, 0, $at                                ## L:574  |      2 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v07, 0, 16, $s6                               ## L:589  |      3 | vec32 posClip = load(ptr, 0x10).xyzw;
  vmudl $v02, $v00, $v31.e3                          ## L:576  |      ^ | screenSize >>= 4;
  ldv $v08, 0, 24, $s6                               ## L:589  |      4 | vec32 posClip = load(ptr, 0x10).xyzw;
  lhu $t0, %lo(RSPQ_SCRATCH_MEM + 20)                ## L:582  |      5 | u16<$t0> oldClipFlag = load(RSPQ_SCRATCH_MEM, 20);
  ldv $v04, 0, 8, $at                                ## L:586  |      6 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzw;
  vmadm $v01, $v01, $v31.e3                          ## L:576  |      ^ | screenSize >>= 4;
  vmadn $v02, $v00, $v00                             ## L:576  |      7 | screenSize >>= 4;
  vrcph $v05.e3, $v07.e3                             ## L:590  |      8 | invW.w = invert_half(posClip).w;
  vrcpl $v06.e3, $v08.e3                             ## L:590  |      9 | invW.w = invert_half(posClip).w;
  vrcph $v05.e3, $v00.e3                             ## L:590  |     10 | invW.w = invert_half(posClip).w;
  vxor $v03, $v00, $v30.e7                           ## L:578  |     11 | vec16 vecOne = 1;
  vmudl $v29, $v08, $v06.e3                          ## L:593  |    *13 | posClip *= invW.w;
  vmadm $v29, $v07, $v06.e3                          ## L:593  |     14 | posClip *= invW.w;
  vmadn $v08, $v08, $v05.e3                          ## L:593  |     15 | posClip *= invW.w;
  vmadh $v07, $v07, $v05.e3                          ## L:593  |     16 | posClip *= invW.w;
  vmudl $v29, $v08, $v02.v                           ## L:594  |   **19 | posClip *= screenSize;
  vmadm $v29, $v07, $v02.v                           ## L:594  |     20 | posClip *= screenSize;
  andi $t0, $t0, 57344                               ## L:583  |      ^ | oldClipFlag = oldClipFlag & ((0b111'00000) << 8);
  vmadn $v08, $v08, $v01.v                           ## L:594  |     21 | posClip *= screenSize;
  ssv $v06, 6, 34, $s6                               ## L:591  |      ^ | store(invW.w, ptr, 0x20);
  ori $t0, $t0, 255                                  ## L:584  |     22 | oldClipFlag |= 0xFF;
  vmadh $v07, $v07, $v01.v                           ## L:594  |      ^ | posClip *= screenSize;
  ssv $v05, 6, 32, $s6                               ## L:591  |     23 | store(invW.w, ptr, 0x20);
  vmadh $v07, $v04, $v30.e7                          ## L:596  |      ^ | posClip:sint = screenOffset:sint +* 1;
  sdv $v07, 0, 0, $s6                                ## L:598  |  ***27 | @Barrier("cc") store(posClip:sint.xyzw, ptr, 0x00); ## Barrier: 0x1
  sh $t0, 6($s6)                                     ## L:599  |     28 | @Barrier("cc") store(oldClipFlag, ptr, 0x06); ## Barrier: 0x1
  jr $ra                                             ## L:602  |     29 | }
  addiu $s6, $s6, 65500                              ## L:601  |    *31 | ptr -= 36;
T3DCmd_TriDraw_End:
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