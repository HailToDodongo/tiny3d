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
  ## hot-path cycles: 267
  ldv $v03, 0, 16, $a0                               ## L:649  |      ^ | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  lui $t1, 0x9669                                    ## L:422  |      2 | u32 parity = 0x96696996;
  srl $t4, $t4, 8                                    ## L:418  |      3 | clipMask >>= 8;
  ldv $v04, 0, 24, $a0                               ## L:650  |      4 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ori $t1, $t1, 0x6996                               ## L:422  |      5 | u32 parity = 0x96696996;
  ldv $v02, 0, 8, $a0                                ## L:648  |      6 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  srlv $t1, $t1, $t4                                 ## L:423  |      7 | parity >>= clipMask;
  ldv $v01, 0, 0, $a0                                ## L:647  |      8 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ori $t2, $zero, %lo(CLIP_BUFFER_RESULT)            ## L:426  |      9 | u32 ptrBuff1 = CLIP_BUFFER_RESULT;
  andi $t1, $t1, 1                                   ## L:424  |     10 | parity &= 1;
  addiu $t2, $t2, 252                                ## L:427  |     11 | ptrBuff1 += 252;
  sll $t1, $t1, 8                                    ## L:425  |     12 | parity <<= 8;
  subu $t2, $t2, $t1                                 ## L:428  |     13 | ptrBuff1 -= parity;
  or $t3, $zero, $t2                                 ## L:429  |     14 | u32 ptrWrite = ptrBuff1;
  addiu $t3, $t3, 65500                              ## L:646  |     15 | ptrDst -= 36;
  sdv $v02, 0, 8, $t3                                ## L:653  |     16 | store(tmp2.xyzw, ptrDst, 0x08);
  sdv $v03, 0, 16, $t3                               ## L:654  |     17 | store(tmp3.xyzw, ptrDst, 0x10);
  ldv $v02, 0, 8, $a1                                ## L:648  |     18 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  sdv $v04, 0, 24, $t3                               ## L:655  |     19 | store(tmp4.xyzw, ptrDst, 0x18);
  ldv $v03, 0, 16, $a1                               ## L:649  |     20 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:652  |     21 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v01, 0, 0, $a1                                ## L:647  |     22 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v04, 0, 24, $a1                               ## L:650  |     23 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:448  |     24 | normScaleWInv.x = load(NORM_SCALE_W).w;
  addiu $t3, $t3, 65500                              ## L:646  |     25 | ptrDst -= 36;
  sdv $v03, 0, 16, $t3                               ## L:654  |     26 | store(tmp3.xyzw, ptrDst, 0x10);
  sdv $v04, 0, 24, $t3                               ## L:655  |     27 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v02, 0, 8, $t3                                ## L:653  |     28 | store(tmp2.xyzw, ptrDst, 0x08);
  ldv $v04, 0, 24, $a2                               ## L:650  |     29 | vec16 tmp4 = load(ptrSrc, 0x18).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:652  |     30 | store(tmp1.xyzw, ptrDst, 0x00);
  ldv $v01, 0, 0, $a2                                ## L:647  |     31 | vec16 tmp1 = load(ptrSrc, 0x00).xyzw;
  ldv $v03, 0, 16, $a2                               ## L:649  |     32 | vec16 tmp3 = load(ptrSrc, 0x10).xyzw;
  addiu $t3, $t3, 65500                              ## L:646  |     33 | ptrDst -= 36;
  ldv $v02, 0, 8, $a2                                ## L:648  |     34 | vec16 tmp2 = load(ptrSrc, 0x08).xyzw;
  sdv $v01, 0, 0, $t3                                ## L:652  |     35 | store(tmp1.xyzw, ptrDst, 0x00);
  lsv $v01, 0, 6, $at                                ## L:448  |     36 | normScaleWInv.x = load(NORM_SCALE_W).w;
  lbu $t7, 6($a0)                                    ## L:443  |     37 | u8 oldClip = load(ptrVertA, 0x06);
  addiu $t6, $zero, 1                                ## L:439  |     38 | s32 planeIdx = 1;
  ori $at, $zero, 0xFFFF                             ## L:450  |     39 | normScaleWInv.w = 0xFFFF;
  ori $t5, $zero, %lo(CLIPPING_PLANES)               ## L:438  |     40 | u32 planePtr = CLIPPING_PLANES;
  vor $v01, $v00, $v01.e0                            ## L:449  |      ^ | normScaleWInv = normScaleWInv.x;
  mtc2 $at, $v01.e3                                  ## L:450  |     41 | normScaleWInv.w = 0xFFFF;
  sdv $v02, 0, 8, $t3                                ## L:653  |     42 | store(tmp2.xyzw, ptrDst, 0x08);
  vsubc $v02, $v00, $v30.e7                          ## L:456  |      ^ | vec16 uvIntMask = VZERO - 1;
  ori $at, $zero, 0xFFFF                             ## L:451  |     43 | normScaleWInv.W = 0xFFFF;
  sdv $v04, 0, 24, $t3                               ## L:655  |     44 | store(tmp4.xyzw, ptrDst, 0x18);
  sdv $v03, 0, 16, $t3                               ## L:654  |     45 | store(tmp3.xyzw, ptrDst, 0x10);
  mtc2 $at, $v01.e7                                  ## L:451  |     46 | normScaleWInv.W = 0xFFFF;
  vor $v03, $v00, $v00                               ## L:458  |      ^ | vec16 uvBias = VZERO;
  sb $t7, %lo(20 + RSPQ_SCRATCH_MEM)($zero)          ## L:444  |     47 | store(oldClip, RSPQ_SCRATCH_MEM, 20);
  addiu $t0, $zero, 108                              ## L:412  |     48 | arraySize = 108;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:457  |     49 | uvIntMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  sdv $v00, 0, 0, $at                                ## L:457  |     50 | uvIntMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  ldv $v02, 8, 0, $at                                ## L:457  |     51 | uvIntMask.XYZW = VZERO.xyzw; ## Barrier: 0x1
  vmov $v03.e4, $v31.e0                              ## L:459  |      ^ | uvBias.X = 0x8000;
  vmov $v03.e5, $v31.e0                              ## L:460  |     52 | uvBias.Y = 0x8000;
  LABEL_clipTriangle_0001:
  and $t7, $t4, $t6                                  ## L:466  |      ^ | u32 planeActive = clipMask & planeIdx;
  beq $t7, $zero, LABEL_clipTriangle_0003            ## L:467  |     53 | if(planeActive)
  nop                                                ## L:467  |     54 | if(planeActive)
  lpv $v04, 0, 0, $t5                                ## L:472  |     55 | vec32 planeNorm:sint = load_vec_s8(planePtr);
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:473  |     56 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  or $t8, $zero, $t3                                 ## L:478  |     57 | u32 ptrLast = ptrWrite;
  lbu $t9, 6($t8)                                    ## L:485  |     58 | u8 lastClipCode   = load(ptrLast, 0x06);
  sdv $v04, 0, 0, $at                                ## L:473  |     59 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  ldv $v04, 8, 0, $at                                ## L:473  |     60 | planeNorm:sint.XYZW = planeNorm:sint.xyzw; ## Barrier: 0x1
  luv $v06, 0, 8, $t8                                ## L:480  |     61 | vec16 lastColor   = load_vec_u8(ptrLast, 0x08);
  ldv $v08, 0, 24, $t8                               ## L:481  |     62 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  vmudl $v05, $v00, $v31.e7                          ## L:475  |      ^ | planeNorm >>= 8;
  llv $v08, 8, 12, $t8                               ## L:482  |     63 | lastPosUV:sfract.XY = load(ptrLast, 0x0C).xy;
  ldv $v07, 0, 16, $t8                               ## L:481  |     64 | vec32 lastPosUV   = load(ptrLast, 0x10).xyzw;
  vmadm $v04, $v04, $v31.e7                          ## L:475  |      ^ | planeNorm >>= 8;
  xori $t2, $t2, 256                                 ## L:489  |     65 | ptrBuff1 ^= 0x100;
  vmadn $v05, $v00, $v00                             ## L:475  |      ^ | planeNorm >>= 8;
  xori $t8, $t2, 256                                 ## L:492  |    *67 | u32 ptrRead = ptrBuff1 ^ 0x100;
  vxor $v08, $v08, $v03.v                            ## L:484  |      ^ | lastPosUV:sfract ^= uvBias;
  vand $v07, $v07, $v02.v                            ## L:483  |     68 | lastPosUV:sint &= uvIntMask;
  xor $t4, $t4, $t6                                  ## L:469  |      ^ | clipMask ^= planeIdx;
  subu $k0, $t8, $t0                                 ## L:493  |     69 | u32 ptrReadEnd = ptrRead - arraySize;
  vmudl $v05, $v05, $v01.v                           ## L:476  |      ^ | planeNorm *= normScaleWInv:ufract;
  or $t3, $zero, $t2                                 ## L:491  |     70 | ptrWrite = ptrBuff1;
  vmadm $v04, $v04, $v01.v                           ## L:476  |      ^ | planeNorm *= normScaleWInv:ufract;
  vmadn $v05, $v00, $v00                             ## L:476  |     71 | planeNorm *= normScaleWInv:ufract;
  LABEL_clipTriangle_0004:
  addiu $t8, $t8, 65500                              ## L:498  |      ^ | ptrRead -= 36;
  ldv $v10, 0, 24, $t8                               ## L:499  |     72 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  lbu $k1, 6($t8)                                    ## L:505  |     73 | u8 clipCode = load(ptrRead, 0x06);
  ldv $v09, 0, 16, $t8                               ## L:499  |     74 | vec32 currentPosUV   = load(ptrRead, 0x10).xyzw;
  llv $v10, 8, 12, $t8                               ## L:500  |     75 | currentPosUV:sfract.XY = load(ptrRead, 0x0C).xy;
  nor $fp, $zero, $k1                                ## L:509  |     76 | u8 isInsideCurr = ~clipCode;
  xor $sp, $k1, $t9                                  ## L:508  |     77 | u8 exclIntersect = clipCode ^ lastClipCode;
  luv $v11, 0, 8, $t8                                ## L:503  |     78 | vec16 currentColor   = load_vec_u8(ptrRead, 0x08);
  and $sp, $sp, $t6                                  ## L:510  |     79 | exclIntersect &= planeIdx;
  vxor $v10, $v10, $v03.v                            ## L:502  |      ^ | currentPosUV:sfract ^= uvBias;
  vand $v09, $v09, $v02.v                            ## L:501  |     80 | currentPosUV:sint &= uvIntMask;
  beq $sp, $zero, LABEL_clipTriangle_0006            ## L:513  |      ^ | if(exclIntersect)
  and $fp, $fp, $t6                                  ## L:511  |     81 | isInsideCurr &= planeIdx;
  and $t9, $t9, $t6                                  ## L:515  |     82 | lastClipCode &= planeIdx;
  beq $t9, $zero, LABEL_clipTriangle_0007            ## L:340  |     83 | if(newClipCode)
  nop                                                ## L:340  |     84 | if(newClipCode)
  vsubc $v13, $v08, $v10.v                           ## L:342  |     85 | posUVDiff = posBase - pos;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:344  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  sdv $v07, 0, 0, $at                                ## L:344  |     86 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v14, $v00, $v09                               ## L:343  |      ^ | vpos = pos;
  vsub $v12, $v07, $v09.v                            ## L:342  |     87 | posUVDiff = posBase - pos;
  sdv $v08, 0, 8, $at                                ## L:344  |      ^ | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  ldv $v14, 8, 0, $at                                ## L:344  |     88 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  vor $v15, $v00, $v10                               ## L:343  |      ^ | vpos = pos;
  ldv $v15, 8, 8, $at                                ## L:344  |     89 | vpos.XYZW = posBase.xyzw; ## Barrier: 0x1
  beq $zero, $zero, LABEL_clipTriangle_0008          ## L:345  |     90 | colorDiff = colorBase - color;
  vsubc $v16, $v06, $v11.v                           ## L:345  |    *92 | colorDiff = colorBase - color;
  LABEL_clipTriangle_0007:
  vsubc $v13, $v10, $v08.v                           ## L:347  |      1 | posUVDiff = pos - posBase;
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM)              ## L:349  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  sdv $v09, 0, 0, $at                                ## L:349  |      2 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsub $v12, $v09, $v07.v                            ## L:347  |      ^ | posUVDiff = pos - posBase;
  vor $v14, $v00, $v07                               ## L:348  |      3 | vpos = posBase;
  sdv $v10, 0, 8, $at                                ## L:349  |      ^ | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  ldv $v14, 8, 0, $at                                ## L:349  |      4 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vor $v15, $v00, $v08                               ## L:348  |      ^ | vpos = posBase;
  ldv $v15, 8, 8, $at                                ## L:349  |      5 | vpos.XYZW = pos.xyzw; ## Barrier: 0x1
  vsubc $v16, $v11, $v06.v                           ## L:350  |      ^ | colorDiff = color - colorBase;
  LABEL_clipTriangle_0008:
  vmudl $v17, $v15, $v05.v                           ## L:232  | ***************************************************************************************93 | res = a * b;
  vmadm $v17, $v14, $v05.v                           ## L:232  |     94 | res = a * b;
  vmadn $v18, $v15, $v04.v                           ## L:232  |     95 | res = a * b;
  vmadh $v17, $v14, $v04.v                           ## L:232  |     96 | res = a * b;
  vaddc $v18, $v18, $v18.q1                          ## L:233  |   **99 | res += res.yywwYYWW;
  vadd $v17, $v17, $v17.q1                           ## L:233  |    100 | res += res.yywwYYWW;
  vaddc $v18, $v18, $v18.h2                          ## L:234  |  **103 | res += res.zzzzZZZZ;
  vadd $v17, $v17, $v17.h2                           ## L:234  |    104 | res += res.zzzzZZZZ;
  vsubc $v20, $v18, $v18.e4                          ## L:354  |  **107 | vec32 fac = dot - dot.X;
  vsub $v19, $v17, $v17.e4                           ## L:354  |    108 | vec32 fac = dot - dot.X;
  vrcph $v19.e0, $v19.e0                             ## L:356  | ***112 | fac.x = invert_half(fac).x;
  vrcpl $v20.e0, $v20.e0                             ## L:356  |    113 | fac.x = invert_half(fac).x;
  vrcph $v19.e0, $v00.e0                             ## L:356  |    114 | fac.x = invert_half(fac).x;
  vaddc $v20, $v20, $v20.v                           ## L:357  |  **117 | fac += fac;
  vadd $v19, $v19, $v19.v                            ## L:357  |    118 | fac += fac;
  vmudl $v29, $v20, $v18.e0                          ## L:358  |  **121 | fac *= dot.x;
  vmadm $v29, $v19, $v18.e0                          ## L:358  |    122 | fac *= dot.x;
  vmadn $v20, $v20, $v17.e0                          ## L:358  |    123 | fac *= dot.x;
  vmadh $v19, $v19, $v17.e0                          ## L:358  |    124 | fac *= dot.x;
  mfc2 $s0, $v19.e0                                  ## L:360  | ***128 | s16 facX = fac:sint.x;
  beq $s0, $zero, LABEL_clipTriangle_0009            ## L:361  |  **131 | if(facX != 0) {
  nop                                                ## L:361  |    132 | if(facX != 0) {
  vlt $v29, $v19, $v30.e7                            ## L:365  |    133 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vmrg $v20, $v20, $v01.e3                           ## L:365  |    134 | fac:ufract = fac:sint < 1 ? fac:ufract : normScaleWInv:ufract.w;
  vge $v29, $v19, $v00.e0                            ## L:366  |    135 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  vmrg $v20, $v20, $v00                              ## L:366  |  **138 | fac:ufract = fac:sint >= 0 ? fac:ufract : VZERO;
  LABEL_clipTriangle_0009:
  beq $t9, $zero, LABEL_clipTriangle_000A            ## L:371  |      ^ | if(newClipCode) {
  vmudm $v16, $v16, $v20.e0                          ## L:369  | ***142 | colorDiff:sfract *= fac:sfract.x;
  vmudl $v18, $v13, $v20.e0                          ## L:373  |    143 | dot = posUVDiff * fac:ufract.x;
  vmadm $v17, $v12, $v20.e0                          ## L:373  |    144 | dot = posUVDiff * fac:ufract.x;
  vmadn $v18, $v00, $v00                             ## L:373  |    145 | dot = posUVDiff * fac:ufract.x;
  vmadn $v08, $v10, $v30.e7                          ## L:374  |    146 | resPosUV = pos +* 1;
  vmadh $v07, $v09, $v30.e7                          ## L:374  |    147 | resPosUV = pos +* 1;
  beq $zero, $zero, LABEL_clipTriangle_000B          ## L:375  |      ^ | resColor:uint = color + colorDiff;
  vaddc $v06, $v11, $v16.v                           ## L:375  |   *149 | resColor:uint = color + colorDiff;
  LABEL_clipTriangle_000A:
  vmudl $v18, $v13, $v20.e0                          ## L:377  |      1 | dot = posUVDiff * fac:ufract.x;
  vmadm $v17, $v12, $v20.e0                          ## L:377  |      2 | dot = posUVDiff * fac:ufract.x;
  vmadn $v18, $v00, $v00                             ## L:377  |      3 | dot = posUVDiff * fac:ufract.x;
  vmadn $v08, $v08, $v30.e7                          ## L:378  |      4 | resPosUV = posBase +* 1;
  vmadh $v07, $v07, $v30.e7                          ## L:378  |      5 | resPosUV = posBase +* 1;
  vaddc $v06, $v06, $v16.v                           ## L:379  |      6 | resColor:uint = colorBase + colorDiff;
  LABEL_clipTriangle_000B:
  beq $t4, $zero, LABEL_clipTriangle_000C            ## L:384  | ***********************************************************************************************************************************************150 | if(remainingPlanes)
  nop                                                ## L:384  |    151 | if(remainingPlanes)
  vmudl $v13, $v08, $v01.v                           ## L:387  |    152 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadm $v12, $v07, $v01.v                           ## L:387  |    153 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmadn $v13, $v00, $v00                             ## L:387  |    154 | vec32 clipBase = resPosUV * normScaleWInv:ufract;
  vmov $v12.e2, $v12.e3                              ## L:389  |  **157 | clipBase.z = clipBase.w;
  vmov $v13.e2, $v13.e3                              ## L:389  |    158 | clipBase.z = clipBase.w;
  vch $v29, $v12, $v07.e3                            ## L:390  |  **161 | newClipCode = clip(clipBase, resPosUV.w);
  vcl $v29, $v13, $v08.e3                            ## L:390  |    162 | newClipCode = clip(clipBase, resPosUV.w);
  cfc2 $t9, $vcc                                     ## L:390  |    163 | newClipCode = clip(clipBase, resPosUV.w);
  andi $t9, $t9, 771                                 ## L:257  |  **166 | res = clipCode & 0b0000'0011'0000'0011;
  srl $s0, $t9, 5                                    ## L:258  |    167 | u32 tmp = res >> 5;
  or $t9, $t9, $s0                                   ## L:259  |    168 | res |= tmp;
  LABEL_clipTriangle_000C:
  vxor $v12, $v08, $v03.v                            ## L:671  |      ^ | vec16 stOut = lastPosUV:sfract ^ uvBias;
  addiu $t3, $t3, 65500                              ## L:669  |    169 | ptrWrite -= 36;
  sdv $v07, 0, 16, $t3                               ## L:675  |    170 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  suv $v06, 0, 8, $t3                                ## L:670  |    171 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  sdv $v08, 0, 24, $t3                               ## L:675  |    172 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  slv $v12, 8, 12, $t3                               ## L:672  |    173 | @Barrier("uv") store(stOut.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sb $t9, 6($t3)                                     ## L:674  |    174 | store(lastClipCode:u8,   ptrWrite, 0x06);
  LABEL_clipTriangle_0006:
  vor $v08, $v00, $v10                               ## L:524  |      ^ | lastPosUV = currentPosUV;
  vor $v07, $v00, $v09                               ## L:524  |    175 | lastPosUV = currentPosUV;
  or $t9, $zero, $k1                                 ## L:525  |      ^ | lastClipCode = clipCode;
  beq $fp, $zero, LABEL_clipTriangle_000D            ## L:528  |    176 | if(isInsideCurr) {
  vor $v06, $v00, $v11                               ## L:526  |    177 | lastColor = currentColor;
  vxor $v12, $v08, $v03.v                            ## L:671  |    178 | vec16 stOut = lastPosUV:sfract ^ uvBias;
  addiu $t3, $t3, 65500                              ## L:669  |      ^ | ptrWrite -= 36;
  sb $t9, 6($t3)                                     ## L:674  |    179 | store(lastClipCode:u8,   ptrWrite, 0x06);
  sdv $v08, 0, 24, $t3                               ## L:675  |    180 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  suv $v06, 0, 8, $t3                                ## L:670  |    181 | @Barrier("uv") store_vec_u8(lastColor,  ptrWrite, 0x08); ## Barrier: 0x2
  slv $v12, 8, 12, $t3                               ## L:672  |    182 | @Barrier("uv") store(stOut.XY, ptrWrite, 0x0C); ## Barrier: 0x2
  sdv $v07, 0, 16, $t3                               ## L:675  |    183 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  LABEL_clipTriangle_000D:
  bne $t8, $k0, LABEL_clipTriangle_0004              ## L:675  |    184 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  nop                                                ## L:675  |    185 | store(lastPosUV.xyzw,    ptrWrite, 0x10);
  LABEL_clipTriangle_0005:
  subu $t0, $t2, $t3                                 ## L:534  |    186 | arraySize = ptrBuff1 - ptrWrite;
  beq $t0, $zero, CLIP_RETURN                        ## L:535  |    187 | if(arraySize == 0)goto CLIP_RETURN;
  nop                                                ## L:535  |    188 | if(arraySize == 0)goto CLIP_RETURN;
  LABEL_clipTriangle_0003:
  sll $t6, $t6, 1                                    ## L:538  |    189 | planeIdx <<= 1;
  addiu $at, $zero, 32                               ## L:539  |    190 | planePtr += 0x04;
  bne $t6, $at, LABEL_clipTriangle_0001              ## L:539  |    191 | planePtr += 0x04;
  addiu $t5, $t5, 4                                  ## L:539  |    192 | planePtr += 0x04;
  CLIP_END:
  or $t7, $zero, $t3                                 ## L:547  |    193 | u32 ptrConvA = ptrWrite;
  LABEL_clipTriangle_000F:
  addiu $t9, $t7, 36                                 ## L:552  |    194 | u32 ptrConvB = ptrConvA + 36;
  bne $t9, $t2, LABEL_clipTriangle_0011              ## L:553  |    195 | if(ptrConvB == ptrBuff1) {
  lhu $t8, %lo(RSPQ_SCRATCH_MEM + 20)                ## L:551  |    196 | u16 oldClipFlag = load(RSPQ_SCRATCH_MEM, 20);
  or $t9, $zero, $t7                                 ## L:554  |    197 | ptrConvB = ptrConvA;
  LABEL_clipTriangle_0011:
  ldv $v08, 0, 16, $t7                               ## L:567  |    198 | vec32 posClip = load(ptrConvA, 0x10).xyzw;
  ldv $v09, 8, 24, $t9                               ## L:568  |    199 | posClip.XYZW = load(ptrConvB, 0x10).xyzw;
  ldv $v08, 8, 16, $t9                               ## L:568  |    200 | posClip.XYZW = load(ptrConvB, 0x10).xyzw;
  ldv $v09, 0, 24, $t7                               ## L:567  |    201 | vec32 posClip = load(ptrConvA, 0x10).xyzw;
  ori $at, $zero, %lo(SCREEN_SCALE)                  ## L:557  |    202 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v05, 0, 8, $at                                ## L:557  |    203 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v04, 0, 0, $at                                ## L:557  |    204 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcph $v10.e3, $v08.e3                             ## L:571  |      ^ | invWRaw.w = invert_half(posClip).w;
  ldv $v04, 8, 0, $at                                ## L:557  |    205 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcpl $v11.e3, $v09.e3                             ## L:571  |      ^ | invWRaw.w = invert_half(posClip).w;
  ldv $v05, 8, 8, $at                                ## L:557  |    206 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  vrcph $v10.e3, $v08.e7                             ## L:572  |      ^ | invWRaw.W = invert_half(posClip).W;
  vrcpl $v11.e7, $v09.e7                             ## L:572  |    207 | invWRaw.W = invert_half(posClip).W;
  andi $t8, $t8, 57344                               ## L:564  |      ^ | oldClipFlag = oldClipFlag & ((0b111'00000) << 8);
  vrcph $v10.e7, $v00.e7                             ## L:572  |    208 | invWRaw.W = invert_half(posClip).W;
  ori $at, $zero, %lo(SCREEN_OFFSET)                 ## L:558  |      ^ | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v06, 8, 0, $at                                ## L:558  |    209 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v06, 0, 0, $at                                ## L:558  |    210 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:559  |    211 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmudl $v13, $v09, $v11.h3                          ## L:576  |      ^ | invW:sfract = posClip * invWRaw.wwwwWWWW;
  vmadm $v13, $v08, $v11.h3                          ## L:576  |    212 | invW:sfract = posClip * invWRaw.wwwwWWWW;
  ldv $v07, 0, 0, $at                                ## L:559  |      ^ | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmadn $v13, $v09, $v10.h3                          ## L:576  |    213 | invW:sfract = posClip * invWRaw.wwwwWWWW;
  ldv $v07, 8, 0, $at                                ## L:559  |      ^ | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ori $t8, $t8, 255                                  ## L:565  | ***217 | oldClipFlag |= 0xFF;
  vsubc $v14, $v07, $v13.h3                          ## L:577  |      ^ | vec16 nrD = normScaleW:ufract - invW:ufract.wwwwWWWW;
  sh $t8, 6($t9)                                     ## L:599  |    218 | store(oldClipFlag, ptrConvB, 0x06);
  vaddc $v13, $v11, $v11.v                           ## L:578  |      ^ | invW = invWRaw + invWRaw;
  vadd $v12, $v10, $v10.v                            ## L:578  |    219 | invW = invWRaw + invWRaw;
  vmudl $v13, $v13, $v14.h2                          ## L:579  |  **222 | invW *= nrD:ufract.zzzzZZZZ;
  vmadm $v12, $v12, $v14.h2                          ## L:579  |    223 | invW *= nrD:ufract.zzzzZZZZ;
  vmadn $v13, $v00, $v00                             ## L:579  |    224 | invW *= nrD:ufract.zzzzZZZZ;
  vmudl $v29, $v09, $v13.h3                          ## L:586  | ***228 | invW = posClip * invW.wwwwWWWW;
  ssv $v13, 14, 34, $t9                              ## L:583  |      ^ | store(invW.W, ptrConvB, 0x20);
  ssv $v12, 6, 32, $t7                               ## L:582  |    229 | store(invW.w, ptrConvA, 0x20);
  vmadm $v29, $v08, $v13.h3                          ## L:586  |      ^ | invW = posClip * invW.wwwwWWWW;
  ssv $v13, 6, 34, $t7                               ## L:582  |    230 | store(invW.w, ptrConvA, 0x20);
  vmadn $v13, $v09, $v12.h3                          ## L:586  |      ^ | invW = posClip * invW.wwwwWWWW;
  ssv $v12, 14, 32, $t9                              ## L:583  |    231 | store(invW.W, ptrConvB, 0x20);
  vmadh $v12, $v08, $v12.h3                          ## L:586  |      ^ | invW = posClip * invW.wwwwWWWW;
  vmudl $v08, $v13, $v05.v                           ## L:587  |  **234 | posClip = invW * screenSize;
  addiu $t7, $t7, 72                                 ## L:601  |      ^ | ptrConvA += 72;
  vmadm $v08, $v12, $v05.v                           ## L:587  |    235 | posClip = invW * screenSize;
  vmadn $v09, $v13, $v04.v                           ## L:587  |    236 | posClip = invW * screenSize;
  vmadh $v08, $v12, $v04.v                           ## L:587  |    237 | posClip = invW * screenSize;
  sh $t8, -66($t7)                                   ## L:595  |      ^ | store(oldClipFlag, ptrConvA, 0x06);
  sltu $at, $t7, $t2                                 ## L:601  |    238 | ptrConvA += 72;
  vmadh $v08, $v06, $v30.e7                          ## L:588  |      ^ | posClip:sint = screenOffset:sint +* 1;
  slv $v08, 0, -72, $t7                              ## L:593  | ***242 | store(posClip:sint.xy, ptrConvA, 0x00);
  vge $v12, $v08, $v00.e0                            ## L:591  |      ^ | vec16 zClamp = posClip:sint >= 0;
  slv $v08, 8, 0, $t9                                ## L:597  |    243 | store(posClip:sint.XY, ptrConvB, 0x00);
  ssv $v12, 4, -68, $t7                              ## L:594  |  **246 | store(zClamp.z, ptrConvA, 0x04);
  bne $at, $zero, LABEL_clipTriangle_000F            ## L:601  |    247 | ptrConvA += 72;
  ssv $v12, 12, 4, $t9                               ## L:598  |    248 | store(zClamp.Z, ptrConvB, 0x04);
  LABEL_clipTriangle_0010:
  addiu $s0, $t0, 65391                              ## L:610  |    249 | s32<$s0> fanWait = arraySize - 145;
  addiu $s4, $t2, 65500                              ## L:607  |    250 | u32<$s4> ptrVert0 = ptrBuff1 - 36;
  addiu $s5, $t3, 36                                 ## L:609  |    251 | u32<$s5> ptrVertInEnd = ptrWrite + 36;
  or $s6, $zero, $s4                                 ## L:608  |    252 | u32<$s6> ptrVertIn = ptrVert0;
  LABEL_clipTriangle_0012:
  or $a0, $zero, $s4                                 ## L:616  |    253 | ptrVertA = ptrVert0;
  addiu $s6, $s6, 65500                              ## L:614  |    254 | ptrVertIn -= 36;
  or $a1, $zero, $s6                                 ## L:617  |    255 | ptrVertB = ptrVertIn;
  jal RDPQ_Triangle_Send_Async                       ## L:621  |    256 | RDPQ_Triangle_Send_Async(ptrVertA, ptrVertB, ptrVertC, faceCull); ## Args: $a0, $a1, $a2, $v0
  addiu $a2, $s6, 65500                              ## L:618  |    257 | ptrVertC = ptrVertIn - 36;
  bgez $s0, RDPQ_Triangle_Send_End                   ## L:622  |    258 | if(fanWait >= 0) {
  ori $ra, $zero, RDPQ_Triangle_Clip                 ## L:560  |    259 | 
  RDPQ_Triangle_Clip:
  bne $s6, $s5, LABEL_clipTriangle_0012              ## L:625  |    260 | RDPQ_Triangle_Clip:
  nop                                                ## L:625  |    261 | RDPQ_Triangle_Clip:
  CLIP_RETURN:
  lhu $t7, %lo(CLIP_RA + 0)                          ## L:633  |    262 | u16 tmp = load(CLIP_RA);
  j $t7                                              ## L:636  |  **265 | goto tmp;
  lh $s4, %lo(RSPQ_SCRATCH_MEM + 18)                 ## L:635  |   *267 | s16<$s4> oldDMEM = load(RSPQ_SCRATCH_MEM, 18);

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