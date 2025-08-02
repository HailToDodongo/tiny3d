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
  ldv $v04, 0, 24, $a0                               ## L:531  |      ^ | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ldv $v02, 0, 8, $a0                                ## L:529  |      2 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ori $t3, $zero, %lo(CLIP_BUFFER_TMP)               ## L:400  |      3 | u32 ptrWrite = CLIP_BUFFER_TMP;
  ldv $v01, 0, 0, $a0                                ## L:528  |      4 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  sdv $v04, 0, 24, $t3                               ## L:536  |      5 | store(tmp4.xyzw, ptrDst, 0x18);
  ldv $v04, 0, 24, $a1                               ## L:531  |      6 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  sdv $v02, 0, 8, $t3                                ## L:534  |      7 | store(tmp2.xyzw, ptrDst, 0x08);
  ldv $v03, 0, 16, $a0                               ## L:530  |      8 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:533  |      9 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v01, 0, 0, $a1                                ## L:528  |     10 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ori $t4, $zero, %lo(CLIPPING_PLANES)               ## L:409  |     11 | u32 planePtr = CLIPPING_PLANES;
  ldv $v02, 0, 8, $a1                                ## L:529  |     12 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  sdv $v03, 0, 16, $t3                               ## L:535  |     13 | store(tmp3.xyzw, ptrDst, 0x10);
  addiu $t3, $t3, 36                                 ## L:538  |     14 | ptrDst += 36;
  sdv $v04, 0, 24, $t3                               ## L:536  |     15 | store(tmp4.xyzw, ptrDst, 0x18);
  ldv $v03, 0, 16, $a1                               ## L:530  |     16 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:413  |     17 | normScaleWInv.x = load(NORM_SCALE_W).w;
  ori $t2, $zero, %lo(CLIP_BUFFER_TMP)               ## L:399  |     18 | u32 ptrBuff1 = CLIP_BUFFER_TMP;
  sdv $v01, 0, 0, $t3                                ## L:533  |     19 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v02, 0, 8, $t3                                ## L:534  |     20 | store(tmp2.xyzw, ptrDst, 0x08);
  ldv $v01, 0, 0, $a2                                ## L:528  |     21 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  sdv $v03, 0, 16, $t3                               ## L:535  |     22 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v04, 0, 24, $a2                               ## L:531  |     23 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t3, $t3, 36                                 ## L:538  |     24 | ptrDst += 36;
  ldv $v03, 0, 16, $a2                               ## L:530  |     25 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:533  |     26 | store(tmp1.xyzw, ptrDst, 0x00);
  lsv $v01, 0, 6, $at                                ## L:413  |     27 | normScaleWInv.x = load(NORM_SCALE_W).w;
  ldv $v02, 0, 8, $a2                                ## L:529  |     28 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ori $at, $zero, 0xFFFF                             ## L:415  |     29 | normScaleWInv.w = 0xFFFF;
  ori $t1, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:398  |     30 | u32 ptrBuff0 = CLIP_BUFFER_RESULT;
  sdv $v03, 0, 16, $t3                               ## L:535  |     31 | store(tmp3.xyzw, ptrDst, 0x10);
  vor $v01, $v00, $v01.e0                            ## L:414  |      ^ | normScaleWInv = normScaleWInv.x;
  sdv $v02, 0, 8, $t3                                ## L:534  |     32 | store(tmp2.xyzw, ptrDst, 0x08);
  mtc2 $at, $v01.e7                                  ## L:416  |     33 | normScaleWInv.W = 0xFFFF;
  mtc2 $at, $v01.e3                                  ## L:415  |     34 | normScaleWInv.w = 0xFFFF;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:418  |     35 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  llv $v02, 0, 0, $at                                ## L:418  |     36 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  sdv $v04, 0, 24, $t3                               ## L:536  |     37 | store(tmp4.xyzw, ptrDst, 0x18);
  addiu $t5, $zero, 1                                ## L:410  |     38 | s32 planeIdx = 1;
  addiu $t3, $t3, 36                                 ## L:538  |     39 | ptrDst += 36;
  addiu $t0, $zero, 108                              ## L:394  |     40 | arraySize = 108;
  vor $v02, $v00, $v02.e1                            ## L:419  |      ^ | guardBandScale = guardBandScale.y;
  LABEL_clipTriangle_0001:
  xor $t1, $t1, $t2                                  ## L:434  |     41 | swap(ptrBuff0, ptrBuff1);
  addiu $t6, $t3, 65500                              ## L:425  |     42 | u32 ptrLast = ptrWrite - 36;
  lbu $t7, 6($t6)                                    ## L:430  |     43 | u8 lastClipCode   = load(ptrLast, 0x06);
  xor $t2, $t1, $t2                                  ## L:434  |     44 | swap(ptrBuff0, ptrBuff1);
  xor $t1, $t1, $t2                                  ## L:434  |     45 | swap(ptrBuff0, ptrBuff1);
  ldv $v04, 0, 16, $t6                               ## L:428  |     46 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  luv $v03, 0, 8, $t6                                ## L:427  |     47 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  ldv $v05, 0, 24, $t6                               ## L:428  |     48 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  llv $v04, 8, 12, $t6                               ## L:429  |     49 | lastPosUV:sint.XY = load(ptrLast, 0x0C).xy;
  or $t6, $zero, $t1                                 ## L:437  |     50 | u32 ptrRead = ptrBuff0;
  or $t3, $zero, $t2                                 ## L:436  |     51 | ptrWrite = ptrBuff1;
  addu $t8, $t1, $t0                                 ## L:438  |     52 | u32 ptrReadEnd = ptrBuff0 + arraySize;
  LABEL_clipTriangle_0003:
  lbu $t9, 6($t6)                                    ## L:447  |     53 | u8 clipCode = load(ptrRead, 0x06);
  luv $v08, 0, 8, $t6                                ## L:445  |     54 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  ldv $v06, 0, 16, $t6                               ## L:443  |     55 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  xor $k0, $t9, $t7                                  ## L:450  |     56 | u8 exclIntersect = clipCode ^ lastClipCode;
  nor $k1, $zero, $t9                                ## L:451  |     57 | u8 isInsideCurr = ~clipCode;
  and $k0, $k0, $t5                                  ## L:452  |     58 | exclIntersect &= planeIdx;
  ldv $v07, 0, 24, $t6                               ## L:443  |     59 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  llv $v06, 8, 12, $t6                               ## L:444  |     60 | currentPosUV:sint.XY = load(ptrRead, 0x0C).xy;
  beq $k0, $zero, LABEL_clipTriangle_0005            ## L:455  |     61 | if(exclIntersect)
  and $k1, $k1, $t5                                  ## L:453  |    *63 | isInsideCurr &= planeIdx;
  and $t7, $t7, $t5                                  ## L:457  |     64 | lastClipCode &= planeIdx;
  lpv $v14, 0, 0, $t4                                ## L:312  |     65 | vec32 planeNorm:sint = load_vec_s8(planePtr);
  beq $t7, $zero, LABEL_clipTriangle_0006            ## L:317  |     66 | if(newClipCode)
  nop                                                ## L:488  |    *68 | u32<$s6> ptrVertIn = CLIP_BUFFER_RESULT;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:319  |     69 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:320  |      ^ | planeNorm:sfract = 0;
  vor $v12, $v00, $v07                               ## L:323  |     70 | vpos = pos;
  sdv $v14, 0, 0, $at                                ## L:319  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsubc $v10, $v05, $v07.v                           ## L:322  |     71 | posUVDiff = posBase - pos;
  ldv $v14, 8, 0, $at                                ## L:319  |      ^ | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsub $v09, $v04, $v06.v                            ## L:322  |     72 | posUVDiff = posBase - pos;
  sdv $v04, 0, 0, $at                                ## L:324  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v06                               ## L:323  |    *74 | vpos = pos;
  sdv $v05, 0, 8, $at                                ## L:324  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmudl $v15, $v15, $v31.e7                          ## L:325  |     75 | planeNorm >>= 8;
  ldv $v11, 8, 0, $at                                ## L:324  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  ldv $v12, 8, 8, $at                                ## L:324  |     76 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vmadm $v14, $v14, $v31.e7                          ## L:325  |      ^ | planeNorm >>= 8;
  vmadn $v15, $v00, $v00                             ## L:325  |     77 | planeNorm >>= 8;
  beq $zero, $zero, LABEL_clipTriangle_0007          ## L:327  |      ^ | colorDiff = colorBase - color;
  vsubc $v13, $v03, $v08.v                           ## L:327  |    *79 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0006:
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:329  |     80 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vor $v11, $v00, $v04                               ## L:333  |      ^ | vpos = posBase;
  sdv $v14, 0, 0, $at                                ## L:329  |     81 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vxor $v15, $v00, $v00.e0                           ## L:330  |      ^ | planeNorm:sfract = 0;
  ldv $v14, 8, 0, $at                                ## L:329  |     82 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  vsubc $v10, $v07, $v05.v                           ## L:332  |      ^ | posUVDiff = pos - posBase;
  sdv $v06, 0, 0, $at                                ## L:334  |     83 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vor $v12, $v00, $v05                               ## L:333  |      ^ | vpos = posBase;
  vsub $v09, $v06, $v04.v                            ## L:332  |     84 | posUVDiff = pos - posBase;
  vmudl $v15, $v15, $v31.e7                          ## L:335  |     85 | planeNorm >>= 8;
  sdv $v07, 0, 8, $at                                ## L:334  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vmadm $v14, $v14, $v31.e7                          ## L:335  |     86 | planeNorm >>= 8;
  ldv $v11, 8, 0, $at                                ## L:334  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  ldv $v12, 8, 8, $at                                ## L:334  |     87 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vmadn $v15, $v00, $v00                             ## L:335  |      ^ | planeNorm >>= 8;
  vsubc $v13, $v08, $v03.v                           ## L:337  |     88 | colorDiff = color - colorBase;
  LABEL_clipTriangle_0007:
  vmudl $v15, $v15, $v01.v                           ## L:340  |   **91 | planeNorm *= normScaleWInv:ufract;
  vmadm $v14, $v14, $v01.v                           ## L:340  |     92 | planeNorm *= normScaleWInv:ufract;
  vmadn $v15, $v00, $v00                             ## L:340  |     93 | planeNorm *= normScaleWInv:ufract;
  vmudl $v29, $v12, $v15.v                           ## L:210  |  ***97 | res = a * b;
  vmadm $v29, $v11, $v15.v                           ## L:210  |     98 | res = a * b;
  vmadn $v17, $v12, $v14.v                           ## L:210  |     99 | res = a * b;
  vmadh $v16, $v11, $v14.v                           ## L:210  |    100 | res = a * b;
  vaddc $v17, $v17, $v17.q1                          ## L:211  |  **103 | res += res.yywwYYWW;
  vadd $v16, $v16, $v16.q1                           ## L:211  |    104 | res += res.yywwYYWW;
  vaddc $v17, $v17, $v17.h2                          ## L:212  |  **107 | res += res.zzzzZZZZ;
  vadd $v16, $v16, $v16.h2                           ## L:212  |    108 | res += res.zzzzZZZZ;
  vsubc $v19, $v17, $v17.e4                          ## L:343  |  **111 | vec32 fac = dot - dot.X;
  vsub $v18, $v16, $v16.e4                           ## L:343  |    112 | vec32 fac = dot - dot.X;
  vrcph $v18.e0, $v18.e0                             ## L:345  | ***116 | fac.x = invert_half(fac).x;
  vrcpl $v19.e0, $v19.e0                             ## L:345  |    117 | fac.x = invert_half(fac).x;
  vrcph $v18.e0, $v00.e0                             ## L:345  |    118 | fac.x = invert_half(fac).x;
  vaddc $v19, $v19, $v19.v                           ## L:346  |  **121 | fac += fac;
  vadd $v18, $v18, $v18.v                            ## L:346  |    122 | fac += fac;
  vmudl $v29, $v19, $v17.e0                          ## L:347  |  **125 | fac *= dot.x;
  vmadm $v29, $v18, $v17.e0                          ## L:347  |    126 | fac *= dot.x;
  vmadn $v19, $v19, $v16.e0                          ## L:347  |    127 | fac *= dot.x;
  vmadh $v18, $v18, $v16.e0                          ## L:347  |    128 | fac *= dot.x;
  mfc2 $sp, $v18.e0                                  ## L:349  | ***132 | s16 facX = fac:sint.x;
  beq $sp, $zero, LABEL_clipTriangle_0008            ## L:350  |  **135 | if(facX != 0) {
  nop                                                ## L:350  |   *137 | if(facX != 0) {
  vlt $v29, $v18, $v30.e7                            ## L:354  |    138 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v19, $v19, $v01.e3                           ## L:354  |    139 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v18, $v00.e0                            ## L:355  |    140 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v19, $v19, $v00                              ## L:355  |  **143 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0008:
  beq $t7, $zero, LABEL_clipTriangle_0009            ## L:360  |      ^ | if(newClipCode) {
  vmudm $v13, $v13, $v19.e0                          ## L:358  | ****148 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v17, $v10, $v19.e0                          ## L:362  |    149 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:362  |    150 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:362  |    151 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v07, $v30.e7                          ## L:363  |    152 | resPosUV = pos +* 1;
  vmadh $v04, $v06, $v30.e7                          ## L:363  |    153 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000A          ## L:364  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v03, $v08, $v13.v                           ## L:364  |   *155 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_0009:
  vmudl $v17, $v10, $v19.e0                          ## L:366  |    156 | dot = posUVDiff * fac:ufract.x;
  vmadm $v16, $v09, $v19.e0                          ## L:366  |    157 | dot = posUVDiff * fac:ufract.x;
  vmadn $v17, $v00, $v00                             ## L:366  |    158 | dot = posUVDiff * fac:ufract.x;
  vmadn $v05, $v05, $v30.e7                          ## L:367  |    159 | resPosUV = posBase +* 1;
  vmadh $v04, $v04, $v30.e7                          ## L:367  |    160 | resPosUV = posBase +* 1;
  vaddc $v03, $v03, $v13.v                           ## L:368  |    161 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000A:
  vmudl $v10, $v05, $v01.v                           ## L:372  |   *163 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v09, $v04, $v01.v                           ## L:372  |    164 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadn $v10, $v00, $v00                             ## L:372  |    165 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmudm $v12, $v02, $v05.h3                          ## L:373  |    166 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  sdv $v04, 0, 16, $t3                               ## L:556  |      ^ | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  vmadh $v11, $v02, $v04.h3                          ## L:373  |    167 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  suv $v03, 0, 8, $t3                                ## L:552  |      ^ | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  slv $v04, 8, 12, $t3                               ## L:553  |    168 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  vmadn $v12, $v00, $v00                             ## L:373  |      ^ | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vch $v29, $v09, $v11.e3                            ## L:375  |  **171 | newClipCode = clip(clipBase, clipPlaneW.w);
  vcl $v29, $v10, $v12.e3                            ## L:375  |    172 | newClipCode = clip(clipBase, clipPlaneW.w);
  sdv $v05, 0, 24, $t3                               ## L:556  |      ^ | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  cfc2 $t7, $vcc                                     ## L:375  |    173 | newClipCode = clip(clipBase, clipPlaneW.w);
  andi $t7, $t7, 1799                                ## L:233  |  **176 | res = clipCode & 0b0000'0111'0000'0111;
  srl $sp, $t7, 5                                    ## L:234  |    177 | u32 tmp = res >> 5;
  or $t7, $t7, $sp                                   ## L:235  |    178 | res |= tmp;
  sb $t7, 6($t3)                                     ## L:555  |    179 | store(lastClipCode:u8,   ptrWrite, 0x06);
  addiu $t3, $t3, 36                                 ## L:557  |    180 | ptrWrite += 36;
  LABEL_clipTriangle_0005:
  vor $v03, $v00, $v08                               ## L:468  |      ^ | lastColor = currentColor;
  or $t7, $zero, $t9                                 ## L:467  |    181 | lastClipCode = clipCode;
  vor $v04, $v00, $v06                               ## L:466  |      ^ | lastPosUV = currentPosUV;
  beq $k1, $zero, LABEL_clipTriangle_000B            ## L:470  |    182 | if(isInsideCurr) {
  vor $v05, $v00, $v07                               ## L:466  |   *184 | lastPosUV = currentPosUV;
  sb $t7, 6($t3)                                     ## L:555  |    185 | store(lastClipCode:u8,   ptrWrite, 0x06);
  sdv $v04, 0, 16, $t3                               ## L:556  |    186 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  suv $v03, 0, 8, $t3                                ## L:552  |    187 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  slv $v04, 8, 12, $t3                               ## L:553  |    188 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v05, 0, 24, $t3                               ## L:556  |    189 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  addiu $t3, $t3, 36                                 ## L:557  |    190 | ptrWrite += 36;
  LABEL_clipTriangle_000B:
  addiu $t6, $t6, 36                                 ## L:474  |    191 | ptrRead += 36;
  bne $t6, $t8, LABEL_clipTriangle_0003              ## L:474  |    192 | ptrRead += 36;
  nop                                                ## L:474  |   *194 | ptrRead += 36;
  LABEL_clipTriangle_0004:
  subu $t0, $t3, $t2                                 ## L:479  |    195 | arraySize = ptrWrite - ptrBuff1;
  beq $t0, $zero, CLIP_RETURN                        ## L:481  |    196 | if(arraySize == 0)goto CLIP_RETURN;
  sll $t5, $t5, 1                                    ## L:478  |   *198 | planeIdx <<= 1;
  addiu $at, $zero, 32                               ## L:483  |    199 | planePtr += 0x04;
  bne $t5, $at, LABEL_clipTriangle_0001              ## L:483  |    201 | planePtr += 0x04;
  addiu $t4, $t4, 4
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:488  |    204 | u32<$s6> ptrVertIn = CLIP_BUFFER_RESULT;
  addu $s5, $s6, $t0                                 ## L:489  |    205 | u32<$s5> ptrVertInEnd = ptrVertIn + arraySize;
  bgezal $zero, triToScreenSpace                     ## L:494  |    206 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s5, $s5, 65464                              ## L:490  |   *208 | ptrVertInEnd -= 72;
  bgezal $zero, triToScreenSpace                     ## L:496  |    209 | @Relative triToScreenSpace(ptrVertIn); ## Args: $s6
  addiu $s6, $s6, 72                                 ## L:495  |   *211 | ptrVertIn += 72;
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
  bne $s6, $s5, LABEL_clipTriangle_000D              ## L:512  |    224 | RDPQ_Triangle_Clip:
  nop                                                ## L:512  |   *226 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $t6, %lo(CLIP_RA + 0)                          ## L:279  |    227 | u16 tmp = load(CLIP_RA);
  j $t6                                              ## L:282  |  **230 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:281  |   *232 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);
triToScreenSpace:
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:570  |      ^ | vec32 screenSize = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v01, 0, 0, $at                                ## L:570  |      2 | vec32 screenSize = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v02, 0, 8, $at                                ## L:570  |      3 | vec32 screenSize = load(SCREEN_SCALE_OFFSET).xyzw;
  ldv $v04, 0, 16, $s6                               ## L:576  |      4 | vec32 posClip = load(ptr, 0x10).xyzw;
  ldv $v05, 0, 24, $s6                               ## L:576  |      5 | vec32 posClip = load(ptr, 0x10).xyzw;
  vmudl $v02, $v00, $v31.e3                          ## L:572  |      ^ | screenSize >>= 4;
  vmadm $v01, $v01, $v31.e3                          ## L:572  |      6 | screenSize >>= 4;
  vmadn $v02, $v00, $v00                             ## L:572  |      7 | screenSize >>= 4;
  vrcph $v04.e3, $v04.e3                             ## L:577  |      8 | posClip.w = invert_half(posClip).w;
  vrcpl $v05.e3, $v05.e3                             ## L:577  |      9 | posClip.w = invert_half(posClip).w;
  vrcph $v04.e3, $v00.e3                             ## L:577  |     10 | posClip.w = invert_half(posClip).w;
  vmudl $v29, $v05, $v05.e3                          ## L:580  |   **13 | posClip *= posClip.w;
  ssv $v05, 6, 34, $s6                               ## L:578  |      ^ | store(posClip.w, ptr, 0x20);
  vmadm $v29, $v04, $v05.e3                          ## L:580  |     14 | posClip *= posClip.w;
  ssv $v04, 6, 32, $s6                               ## L:578  |      ^ | store(posClip.w, ptr, 0x20);
  vmadn $v05, $v05, $v04.e3                          ## L:580  |     15 | posClip *= posClip.w;
  vmadh $v04, $v04, $v04.e3                          ## L:580  |     16 | posClip *= posClip.w;
  vmudl $v29, $v05, $v02.v                           ## L:581  |   **19 | posClip *= screenSize;
  vmadm $v29, $v04, $v02.v                           ## L:581  |     20 | posClip *= screenSize;
  vmadn $v05, $v05, $v01.v                           ## L:581  |     21 | posClip *= screenSize;
  vmadh $v04, $v04, $v01.v                           ## L:581  |     22 | posClip *= screenSize;
  ldv $v03, 0, 8, $at                                ## L:574  |      ^ | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzw;
  vadd $v04, $v04, $v03.v                            ## L:582  |  ***26 | posClip:sint += screenOffset;
  sdv $v04, 0, 0, $s6                                ## L:584  |  ***30 | @Barrier("cc") store(posClip:sint.xyzw, ptr, 0x00); ## Barrier: 0x1
  ssv $v03, 6, 6, $s6                                ## L:587  |     31 | @Barrier("cc") store(screenOffset.w, ptr, 0x06); ## Barrier: 0x1
  jr $ra                                             ## L:590  |     32 | }
  addiu $s6, $s6, 65500                              ## L:589  |    *34 | ptr -= 36;
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