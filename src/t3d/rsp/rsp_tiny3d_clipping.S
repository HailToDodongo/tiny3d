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
#define CLIP_BUFFER_XOR 0x100
#define CLIP_BUFFER_SIZE 252
#define CLIP_FAN_ASYNC_MAX_SIZE 145
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
    VERT_CONSTS: .half 32768, 32768, 0, 32768, 33023, 32767, 0, 0
    .align 4
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 3
    SCREEN_SCALE: .half 0, 0, 0, 0, 0, 0, 0, 0
    .align 1
    SCREEN_OFFSET: .half 0, 0, 16383, 33023
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
  ## hot-path cycles: 270
  ori $t2, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:431  |      ^ | u32 ptrBuff1 = CLIP_BUFFER_RESULT;
  srl $t4, $t4, 8                                    ## L:418  |      2 | clipMask >>= 8;
  lui $t1, 0x9669                                    ## L:427  |      3 | u32 parity = 0x96696996;
  addiu $t2, $t2, 252                                ## L:432  |      4 | ptrBuff1 += 252;
  ori $t1, $t1, 0x6996                               ## L:427  |      5 | u32 parity = 0x96696996;
  ldv $v02, 0, 8, $a0                                ## L:651  |      6 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  srlv $t1, $t1, $t4                                 ## L:428  |      7 | parity >>= clipMask;
  andi $t1, $t1, 1                                   ## L:429  |      8 | parity &= 1;
  sll $t1, $t1, 8                                    ## L:430  |      9 | parity <<= 8;
  ldv $v01, 0, 0, $a0                                ## L:650  |     10 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  subu $t2, $t2, $t1                                 ## L:433  |     11 | ptrBuff1 -= parity;
  ldv $v04, 0, 24, $a0                               ## L:653  |     12 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  or $t3, $zero, $t2                                 ## L:434  |     13 | u32 ptrWrite = ptrBuff1;
  addiu $t3, $t3, 65500                              ## L:649  |     14 | ptrDst -= 36;
  sdv $v01, 0, 0, $t3                                ## L:655  |     15 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v03, 0, 16, $a0                               ## L:652  |     16 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v02, 0, 8, $t3                                ## L:656  |     17 | store(tmp2.xyzw, ptrDst, 0x08);
  ldv $v02, 0, 8, $a1                                ## L:651  |     18 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  sdv $v04, 0, 24, $t3                               ## L:658  |     19 | store(tmp4.xyzw, ptrDst, 0x18);
  addiu $t6, $zero, 1                                ## L:444  |     20 | s32 planeIdx = 1;
  ldv $v01, 0, 0, $a1                                ## L:650  |     21 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  sdv $v03, 0, 16, $t3                               ## L:657  |     22 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v04, 0, 24, $a1                               ## L:653  |     23 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ldv $v03, 0, 16, $a1                               ## L:652  |     24 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  lbu $t7, 6($a0)                                    ## L:448  |     25 | u8 oldClip = load(ptrVertA, 0x06);
  addiu $t3, $t3, 65500                              ## L:649  |     26 | ptrDst -= 36;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:453  |     27 | normScaleWInv.x = load(NORM_SCALE_W).w;
  sdv $v04, 0, 24, $t3                               ## L:658  |     28 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v03, 0, 16, $t3                               ## L:657  |     29 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v01, 0, 0, $t3                                ## L:655  |     30 | store(tmp1.xyzw, ptrDst, 0x00);
  sdv $v02, 0, 8, $t3                                ## L:656  |     31 | store(tmp2.xyzw, ptrDst, 0x08);
  ldv $v02, 0, 8, $a2                                ## L:651  |     32 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  ldv $v01, 0, 0, $a2                                ## L:650  |     33 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v04, 0, 24, $a2                               ## L:653  |     34 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  addiu $t3, $t3, 65500                              ## L:649  |     35 | ptrDst -= 36;
  ldv $v03, 0, 16, $a2                               ## L:652  |     36 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:655  |     37 | store(tmp1.xyzw, ptrDst, 0x00);
  lsv $v01, 0, 6, $at                                ## L:453  |     38 | normScaleWInv.x = load(NORM_SCALE_W).w;
  sdv $v04, 0, 24, $t3                               ## L:658  |     39 | store(tmp4.xyzw, ptrDst, 0x18);
  ori $at, $zero, 0xFFFF                             ## L:455  |     40 | normScaleWInv.w = 0xFFFF;
  sb $t7, %lo(20 + RSPQ_SCRATCH_MEM)($zero)          ## L:449  |     41 | store(oldClip, RSPQ_SCRATCH_MEM, 20);
  sdv $v02, 0, 8, $t3                                ## L:656  |     42 | store(tmp2.xyzw, ptrDst, 0x08);
  vor $v01, $v00, $v01.e0                            ## L:454  |      ^ | normScaleWInv = normScaleWInv.x;
  mtc2 $at, $v01.e3                                  ## L:455  |     43 | normScaleWInv.w = 0xFFFF;
  sdv $v03, 0, 16, $t3                               ## L:657  |     44 | store(tmp3.xyzw, ptrDst, 0x10);
  vsubc $v03, $v00, $v30.e7                          ## L:462  |      ^ | vec16 uvFractMask = VZERO - 1;
  ori $at, $zero, 0xFFFF                             ## L:456  |     45 | normScaleWInv.W = 0xFFFF;
  mtc2 $at, $v01.e7                                  ## L:456  |     46 | normScaleWInv.W = 0xFFFF;
  ori $at, $zero, %lo(CLIPPING_PLANES)               ## L:458  |     47 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  llv $v02, 0, 0, $at                                ## L:458  |     48 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:463  |     49 | uvFractMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  ori $t5, $zero, %lo(CLIPPING_PLANES)               ## L:443  |     50 | u32 planePtr = CLIPPING_PLANES;
  addiu $t0, $zero, 108                              ## L:412  |     51 | arraySize = 108;
  sdv $v00, 0, 0, $at                                ## L:463  |     52 | uvFractMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  ldv $v03, 8, 0, $at                                ## L:463  |     53 | uvFractMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  vor $v02, $v00, $v02.e1                            ## L:459  |      ^ | guardBandScale = guardBandScale.y;
  LABEL_clipTriangle_0001:
  and $t7, $t4, $t6                                  ## L:469  |     54 | u32 planeActive = clipMask & planeIdx;
  beq $t7, $zero, LABEL_clipTriangle_0003            ## L:470  |     55 | if(planeActive)
  nop                                                ## L:470  |     56 | if(planeActive)
  vmudl $v05, $v00, $v31.e7                          ## L:478  |     57 | planeNorm >>= 8;
  lpv $v04, 0, 0, $t5                                ## L:475  |      ^ | vec32 planeNorm:sint = load_vec_s8(planePtr);
  or $t8, $zero, $t3                                 ## L:481  |     58 | u32 ptrLast = ptrWrite;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:476  |     59 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  lbu $t9, 6($t8)                                    ## L:487  |     60 | u8 lastClipCode   = load(ptrLast, 0x06);
  sdv $v04, 0, 0, $at                                ## L:476  |     61 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  ldv $v04, 8, 0, $at                                ## L:476  |     62 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  xori $t2, $t2, 256                                 ## L:491  |     63 | ptrBuff1 ^= 0x100;
  luv $v06, 0, 8, $t8                                ## L:483  |     64 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  ldv $v08, 0, 24, $t8                               ## L:484  |    *66 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  vmadm $v04, $v04, $v31.e7                          ## L:478  |      ^ | planeNorm >>= 8;
  llv $v07, 8, 12, $t8                               ## L:485  |     67 | lastPosUV:sint.XY = load(ptrLast, 0x0C).xy;
  vmadn $v05, $v00, $v00                             ## L:478  |      ^ | planeNorm >>= 8;
  ldv $v07, 0, 16, $t8                               ## L:484  |     68 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  xori $t8, $t2, 256                                 ## L:494  |     69 | u32 ptrRead = ptrBuff1 ^ 0x100;
  subu $k0, $t8, $t0                                 ## L:495  |     70 | u32 ptrReadEnd = ptrRead - arraySize;
  vand $v08, $v08, $v03.v                            ## L:486  |      ^ | lastPosUV:sfract &= uvFractMask;
  xor $t4, $t4, $t6                                  ## L:472  |     71 | clipMask ^= planeIdx;
  vmudl $v05, $v05, $v01.v                           ## L:479  |      ^ | planeNorm *= normScaleWInv:ufract;
  vmadm $v04, $v04, $v01.v                           ## L:479  |     72 | planeNorm *= normScaleWInv:ufract;
  or $t3, $zero, $t2                                 ## L:493  |      ^ | ptrWrite = ptrBuff1;
  vmadn $v05, $v00, $v00                             ## L:479  |     73 | planeNorm *= normScaleWInv:ufract;
  LABEL_clipTriangle_0004:
  addiu $t8, $t8, 65500                              ## L:500  |      ^ | ptrRead -= 36;
  lbu $k1, 6($t8)                                    ## L:506  |     74 | u8 clipCode = load(ptrRead, 0x06);
  llv $v09, 8, 12, $t8                               ## L:502  |     75 | currentPosUV:sint.XY = load(ptrRead, 0x0C).xy;
  ldv $v10, 0, 24, $t8                               ## L:501  |     76 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  nor $fp, $zero, $k1                                ## L:510  |     77 | u8 isInsideCurr = ~clipCode;
  xor $sp, $k1, $t9                                  ## L:509  |     78 | u8 exclIntersect = clipCode ^ lastClipCode;
  and $fp, $fp, $t6                                  ## L:512  |     79 | isInsideCurr &= planeIdx;
  and $sp, $sp, $t6                                  ## L:511  |     80 | exclIntersect &= planeIdx;
  ldv $v09, 0, 16, $t8                               ## L:501  |     81 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  vand $v10, $v10, $v03.v                            ## L:503  |      ^ | currentPosUV:sfract &= uvFractMask;
  beq $sp, $zero, LABEL_clipTriangle_0006            ## L:514  |     82 | if(exclIntersect)
  luv $v11, 0, 8, $t8                                ## L:504  |     83 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  and $t9, $t9, $t6                                  ## L:516  |     84 | lastClipCode &= planeIdx;
  beq $t9, $zero, LABEL_clipTriangle_0007            ## L:340  |     85 | if(newClipCode)
  nop                                                ## L:340  |     86 | if(newClipCode)
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:344  |     87 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v14, $v00, $v09                               ## L:343  |      ^ | vpos = pos;
  vsubc $v13, $v08, $v10.v                           ## L:342  |     88 | posUVDiff = posBase - pos;
  sdv $v07, 0, 0, $at                                ## L:344  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  sdv $v08, 0, 8, $at                                ## L:344  |     89 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vsub $v12, $v07, $v09.v                            ## L:342  |      ^ | posUVDiff = posBase - pos;
  ldv $v14, 8, 0, $at                                ## L:344  |     90 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v15, $v00, $v10                               ## L:343  |      ^ | vpos = pos;
  ldv $v15, 8, 8, $at                                ## L:344  |     91 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  beq $zero, $zero, LABEL_clipTriangle_0008          ## L:345  |     92 | colorDiff = colorBase - color;
  vsubc $v16, $v06, $v11.v                           ## L:345  |    *94 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0007:
  vor $v14, $v00, $v07                               ## L:348  |      1 | vpos = posBase;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:349  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  sdv $v09, 0, 0, $at                                ## L:349  |      2 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsubc $v13, $v10, $v08.v                           ## L:347  |      ^ | posUVDiff = pos - posBase;
  vor $v15, $v00, $v08                               ## L:348  |      3 | vpos = posBase;
  sdv $v10, 0, 8, $at                                ## L:349  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsub $v12, $v09, $v07.v                            ## L:347  |      4 | posUVDiff = pos - posBase;
  ldv $v14, 8, 0, $at                                ## L:349  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  ldv $v15, 8, 8, $at                                ## L:349  |      5 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsubc $v16, $v11, $v06.v                           ## L:350  |      ^ | colorDiff = color - colorBase;
  LABEL_clipTriangle_0008:
  vmudl $v17, $v15, $v05.v                           ## L:232  | *****************************************************************************************95 | res = a * b;
  vmadm $v17, $v14, $v05.v                           ## L:232  |     96 | res = a * b;
  vmadn $v18, $v15, $v04.v                           ## L:232  |     97 | res = a * b;
  vmadh $v17, $v14, $v04.v                           ## L:232  |     98 | res = a * b;
  vaddc $v18, $v18, $v18.q1                          ## L:233  |  **101 | res += res.yywwYYWW;
  vadd $v17, $v17, $v17.q1                           ## L:233  |    102 | res += res.yywwYYWW;
  vaddc $v18, $v18, $v18.h2                          ## L:234  |  **105 | res += res.zzzzZZZZ;
  vadd $v17, $v17, $v17.h2                           ## L:234  |    106 | res += res.zzzzZZZZ;
  vsubc $v20, $v18, $v18.e4                          ## L:354  |  **109 | vec32 fac = dot - dot.X;
  vsub $v19, $v17, $v17.e4                           ## L:354  |    110 | vec32 fac = dot - dot.X;
  vrcph $v19.e0, $v19.e0                             ## L:356  | ***114 | fac.x = invert_half(fac).x;
  vrcpl $v20.e0, $v20.e0                             ## L:356  |    115 | fac.x = invert_half(fac).x;
  vrcph $v19.e0, $v00.e0                             ## L:356  |    116 | fac.x = invert_half(fac).x;
  vaddc $v20, $v20, $v20.v                           ## L:357  |  **119 | fac += fac;
  vadd $v19, $v19, $v19.v                            ## L:357  |    120 | fac += fac;
  vmudl $v29, $v20, $v18.e0                          ## L:358  |  **123 | fac *= dot.x;
  vmadm $v29, $v19, $v18.e0                          ## L:358  |    124 | fac *= dot.x;
  vmadn $v20, $v20, $v17.e0                          ## L:358  |    125 | fac *= dot.x;
  vmadh $v19, $v19, $v17.e0                          ## L:358  |    126 | fac *= dot.x;
  mfc2 $s0, $v19.e0                                  ## L:360  | ***130 | s16 facX = fac:sint.x;
  beq $s0, $zero, LABEL_clipTriangle_0009            ## L:361  |  **133 | if(facX != 0) {
  nop                                                ## L:361  |    134 | if(facX != 0) {
  vlt $v29, $v19, $v30.e7                            ## L:365  |    135 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v20, $v20, $v01.e3                           ## L:365  |    136 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v19, $v00.e0                            ## L:366  |    137 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v20, $v20, $v00                              ## L:366  |  **140 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0009:
  beq $t9, $zero, LABEL_clipTriangle_000A            ## L:371  |      ^ | if(newClipCode) {
  vmudm $v16, $v16, $v20.e0                          ## L:369  | ***144 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v18, $v13, $v20.e0                          ## L:373  |    145 | dot = posUVDiff * fac:ufract.x;
  vmadm $v17, $v12, $v20.e0                          ## L:373  |    146 | dot = posUVDiff * fac:ufract.x;
  vmadn $v18, $v00, $v00                             ## L:373  |    147 | dot = posUVDiff * fac:ufract.x;
  vmadn $v08, $v10, $v30.e7                          ## L:374  |    148 | resPosUV = pos +* 1;
  vmadh $v07, $v09, $v30.e7                          ## L:374  |    149 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000B          ## L:375  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v06, $v11, $v16.v                           ## L:375  |   *151 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_000A:
  vmudl $v18, $v13, $v20.e0                          ## L:377  |      1 | dot = posUVDiff * fac:ufract.x;
  vmadm $v17, $v12, $v20.e0                          ## L:377  |      2 | dot = posUVDiff * fac:ufract.x;
  vmadn $v18, $v00, $v00                             ## L:377  |      3 | dot = posUVDiff * fac:ufract.x;
  vmadn $v08, $v08, $v30.e7                          ## L:378  |      4 | resPosUV = posBase +* 1;
  vmadh $v07, $v07, $v30.e7                          ## L:378  |      5 | resPosUV = posBase +* 1;
  vaddc $v06, $v06, $v16.v                           ## L:379  |      6 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000B:
  beq $t4, $zero, LABEL_clipTriangle_000C            ## L:384  | *************************************************************************************************************************************************152 | if(remainingPlanes)
  nop                                                ## L:384  |    153 | if(remainingPlanes)
  vmudl $v13, $v08, $v01.v                           ## L:386  |    154 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v12, $v07, $v01.v                           ## L:386  |    155 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadn $v13, $v00, $v00                             ## L:386  |    156 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmudm $v15, $v02, $v08.h3                          ## L:387  |    157 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadh $v14, $v02, $v07.h3                          ## L:387  |    158 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmadn $v15, $v00, $v00                             ## L:387  |    159 | vec32 clipPlaneW = guardBandScale * resPosUV.wwwwWWWW;
  vmov $v12.e2, $v12.e3                              ## L:389  |    160 | clipBase.z = clipBase.w;
  vmov $v13.e2, $v13.e3                              ## L:389  |    161 | clipBase.z = clipBase.w;
  vch $v29, $v12, $v14.e3                            ## L:390  |  **164 | newClipCode = clip(clipBase, clipPlaneW.w);
  vcl $v29, $v13, $v15.e3                            ## L:390  |    165 | newClipCode = clip(clipBase, clipPlaneW.w);
  cfc2 $t9, $vcc                                     ## L:390  |    166 | newClipCode = clip(clipBase, clipPlaneW.w);
  andi $t9, $t9, 771                                 ## L:257  |  **169 | res = clipCode & 0b0000'0011'0000'0011;
  srl $s0, $t9, 5                                    ## L:258  |    170 | u32 tmp = res >> 5;
  or $t9, $t9, $s0                                   ## L:259  |    171 | res |= tmp;
  LABEL_clipTriangle_000C:
  addiu $t3, $t3, 65500                              ## L:672  |    172 | ptrWrite -= 36;
  sb $t9, 6($t3)                                     ## L:676  |    173 | store(lastClipCode:u8,   ptrWrite, 0x06);
  sdv $v08, 0, 24, $t3                               ## L:677  |    174 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  suv $v06, 0, 8, $t3                                ## L:673  |    175 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  slv $v07, 8, 12, $t3                               ## L:674  |    176 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v07, 0, 16, $t3                               ## L:677  |    177 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  LABEL_clipTriangle_0006:
  or $t9, $zero, $k1                                 ## L:526  |    178 | lastClipCode = clipCode;
  vor $v08, $v00, $v10                               ## L:525  |      ^ | lastPosUV = currentPosUV;
  vor $v06, $v00, $v11                               ## L:527  |    179 | lastColor = currentColor;
  beq $fp, $zero, LABEL_clipTriangle_000D            ## L:529  |      ^ | if(isInsideCurr) {
  vor $v07, $v00, $v09                               ## L:525  |    180 | lastPosUV = currentPosUV;
  addiu $t3, $t3, 65500                              ## L:672  |    181 | ptrWrite -= 36;
  sdv $v08, 0, 24, $t3                               ## L:677  |    182 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  suv $v06, 0, 8, $t3                                ## L:673  |    183 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  sb $t9, 6($t3)                                     ## L:676  |    184 | store(lastClipCode:u8,   ptrWrite, 0x06);
  sdv $v07, 0, 16, $t3                               ## L:677  |    185 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  slv $v07, 8, 12, $t3                               ## L:674  |    186 | @Barrier("uv") store(lastPosUV:sint.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  LABEL_clipTriangle_000D:
  bne $t8, $k0, LABEL_clipTriangle_0004              ## L:677  |    187 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  nop                                                ## L:677  |    188 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  LABEL_clipTriangle_0005:
  subu $t0, $t2, $t3                                 ## L:535  |    189 | arraySize = ptrBuff1 - ptrWrite;
  beq $t0, $zero, CLIP_RETURN                        ## L:536  |    190 | if(arraySize == 0)goto CLIP_RETURN;
  nop                                                ## L:536  |    191 | if(arraySize == 0)goto CLIP_RETURN;
  LABEL_clipTriangle_0003:
  addiu $at, $zero, 32                               ## L:540  |    192 | planePtr += 0x04;
  sll $t6, $t6, 1                                    ## L:539  |    193 | planeIdx <<= 1;
  bne $t6, $at, LABEL_clipTriangle_0001              ## L:540  |    194 | planePtr += 0x04;
  addiu $t5, $t5, 4                                  ## L:540  |    195 | planePtr += 0x04;
  CLIP_END:
  or $t7, $zero, $t3                                 ## L:548  |    196 | u32 ptrConvA = ptrWrite;
  LABEL_clipTriangle_000F:
  addiu $t9, $t7, 36                                 ## L:553  |    197 | u32 ptrConvB = ptrConvA + 36;
  bne $t9, $t2, LABEL_clipTriangle_0011              ## L:554  |    198 | if(ptrConvB == ptrBuff1) {
  lhu $t8, %lo(RSPQ_SCRATCH_MEM + 20)                ## L:552  |    199 | u16 oldClipFlag = load(RSPQ_SCRATCH_MEM, 20);
  or $t9, $zero, $t7                                 ## L:555  |    200 | ptrConvB = ptrConvA;
  LABEL_clipTriangle_0011:
  ldv $v08, 0, 16, $t7                               ## L:568  |    201 | vec32 posClip = load(ptrConvA, 0x10).xyzw;
  ldv $v08, 8, 16, $t9                               ## L:569  |    202 | posClip.XYZW = load(ptrConvB, 0x10).xyzw;
  ldv $v09, 0, 24, $t7                               ## L:568  |    203 | vec32 posClip = load(ptrConvA, 0x10).xyzw;
  ldv $v09, 8, 24, $t9                               ## L:569  |    204 | posClip.XYZW = load(ptrConvB, 0x10).xyzw;
  ori $at, $zero, %lo(SCREEN_SCALE)                  ## L:558  |    205 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v04, 0, 0, $at                                ## L:558  |    206 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v04, 8, 0, $at                                ## L:558  |    207 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcph $v10.e3, $v08.e3                             ## L:572  |      ^ | invWRaw.w = invert_half(posClip).w;
  vrcpl $v11.e3, $v09.e3                             ## L:572  |    208 | invWRaw.w = invert_half(posClip).w;
  ldv $v05, 0, 8, $at                                ## L:558  |      ^ | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcph $v10.e3, $v08.e7                             ## L:573  |    209 | invWRaw.W = invert_half(posClip).W;
  ldv $v05, 8, 8, $at                                ## L:558  |      ^ | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcpl $v11.e7, $v09.e7                             ## L:573  |    210 | invWRaw.W = invert_half(posClip).W;
  ori $at, $zero, %lo(SCREEN_OFFSET)                 ## L:559  |      ^ | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v06, 0, 0, $at                                ## L:559  |    211 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v06, 8, 0, $at                                ## L:559  |    212 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  vrcph $v10.e7, $v00.e7                             ## L:573  |      ^ | invWRaw.W = invert_half(posClip).W;
  vmudl $v13, $v09, $v11.h3                          ## L:577  |   *214 | invW:sfract = posClip * invWRaw.wwwwWWWW;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:560  |      ^ | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v07, 8, 0, $at                                ## L:560  |    215 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmadm $v13, $v08, $v11.h3                          ## L:577  |      ^ | invW:sfract = posClip * invWRaw.wwwwWWWW;
  vmadn $v13, $v09, $v10.h3                          ## L:577  |    216 | invW:sfract = posClip * invWRaw.wwwwWWWW;
  ldv $v07, 0, 0, $at                                ## L:560  |      ^ | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  andi $t8, $t8, 57344                               ## L:565  | ***220 | oldClipFlag = oldClipFlag & ((0b111'00000) << 8);
  vsubc $v14, $v07, $v13.h3                          ## L:578  |      ^ | vec16 nrD = normScaleW:ufract - invW:ufract.wwwwWWWW;
  ori $t8, $t8, 255                                  ## L:566  |    221 | oldClipFlag |= 0xFF;
  vaddc $v13, $v11, $v11.v                           ## L:579  |      ^ | invW = invWRaw + invWRaw;
  vadd $v12, $v10, $v10.v                            ## L:579  |    222 | invW = invWRaw + invWRaw;
  vmudl $v13, $v13, $v14.h0                          ## L:580  |  **225 | invW *= nrD:ufract.xxxxXXXX;
  vmadm $v12, $v12, $v14.h0                          ## L:580  |    226 | invW *= nrD:ufract.xxxxXXXX;
  vmadn $v13, $v00, $v00                             ## L:580  |    227 | invW *= nrD:ufract.xxxxXXXX;
  ssv $v13, 6, 34, $t7                               ## L:583  | ***231 | store(invW.w, ptrConvA, 0x20);
  vmudl $v29, $v09, $v13.h3                          ## L:587  |      ^ | invW = posClip * invW.wwwwWWWW;
  vmadm $v29, $v08, $v13.h3                          ## L:587  |    232 | invW = posClip * invW.wwwwWWWW;
  ssv $v13, 14, 34, $t9                              ## L:584  |      ^ | store(invW.W, ptrConvB, 0x20);
  vmadn $v13, $v09, $v12.h3                          ## L:587  |    233 | invW = posClip * invW.wwwwWWWW;
  ssv $v12, 14, 32, $t9                              ## L:584  |      ^ | store(invW.W, ptrConvB, 0x20);
  ssv $v12, 6, 32, $t7                               ## L:583  |    234 | store(invW.w, ptrConvA, 0x20);
  vmadh $v12, $v08, $v12.h3                          ## L:587  |      ^ | invW = posClip * invW.wwwwWWWW;
  addiu $t7, $t7, 72                                 ## L:602  |  **237 | ptrConvA += 72;
  vmudl $v08, $v13, $v05.v                           ## L:588  |      ^ | posClip = invW * screenSize;
  vmadm $v08, $v12, $v05.v                           ## L:588  |    238 | posClip = invW * screenSize;
  vmadn $v09, $v13, $v04.v                           ## L:588  |    239 | posClip = invW * screenSize;
  sh $t8, -66($t7)                                   ## L:596  |      ^ | store(oldClipFlag, ptrConvA, 0x06);
  vmadh $v08, $v12, $v04.v                           ## L:588  |    240 | posClip = invW * screenSize;
  sltu $at, $t7, $t2                                 ## L:602  |      ^ | ptrConvA += 72;
  vmadh $v08, $v06, $v30.e7                          ## L:589  |    241 | posClip:sint = screenOffset:sint +* 1;
  sh $t8, 6($t9)                                     ## L:600  |      ^ | store(oldClipFlag, ptrConvB, 0x06);
  vge $v12, $v08, $v00.e0                            ## L:592  | ***245 | vec16 zClamp = posClip:sint >= 0;
  slv $v08, 0, -72, $t7                              ## L:594  |      ^ | store(posClip:sint.xy, ptrConvA, 0x00);
  slv $v08, 8, 0, $t9                                ## L:598  |    246 | store(posClip:sint.XY, ptrConvB, 0x00);
  ssv $v12, 4, -68, $t7                              ## L:595  |  **249 | store(zClamp.z, ptrConvA, 0x04);
  bne $at, $zero, LABEL_clipTriangle_000F            ## L:602  |    250 | ptrConvA += 72;
  ssv $v12, 12, 4, $t9                               ## L:599  |    251 | store(zClamp.Z, ptrConvB, 0x04);
  LABEL_clipTriangle_0010:
  addiu $s5, $t3, 36                                 ## L:612  |    252 | u32<$s5> ptrVertInEnd = ptrWrite + 36;
  addiu $s4, $t2, 65500                              ## L:610  |    253 | u32<$s4> ptrVert0 = ptrBuff1 - 36;
  or $s6, $zero, $s4                                 ## L:611  |    254 | u32<$s6> ptrVertIn = ptrVert0;
  addiu $s0, $t0, 65391                              ## L:613  |    255 | s32<$s0> fanWait = arraySize - 145;
  LABEL_clipTriangle_0012:
  addiu $s6, $s6, 65500                              ## L:617  |    256 | ptrVertIn -= 36;
  or $a0, $zero, $s4                                 ## L:619  |    257 | ptrVertA = ptrVert0;
  or $a1, $zero, $s6                                 ## L:620  |    258 | ptrVertB = ptrVertIn;
  jal RDPQ_Triangle_Send_Async                       ## L:624  |    259 | RDPQ_Triangle_Send_Async(ptrVertA, ptrVertB, ptrVertC, faceCull); ## Args: $a0, $a1, $a2, $v0
  addiu $a2, $s6, 65500                              ## L:621  |    260 | ptrVertC = ptrVertIn - 36;
  bgez $s0, RDPQ_Triangle_Send_End                   ## L:625  |    261 | if(fanWait >= 0) {
  ori $ra, $zero, RDPQ_Triangle_Clip                 ## L:559  |    262 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  RDPQ_Triangle_Clip:
  bne $s6, $s5, LABEL_clipTriangle_0012              ## L:628  |    263 | RDPQ_Triangle_Clip:
  nop                                                ## L:628  |    264 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $t7, %lo(CLIP_RA + 0)                          ## L:636  |    265 | u16 tmp = load(CLIP_RA);
  j $t7                                              ## L:639  |  **268 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:638  |   *270 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);

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