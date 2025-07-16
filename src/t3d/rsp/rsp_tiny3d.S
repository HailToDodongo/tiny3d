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
  andi $t0, $a0, 65535 ## L:366 | u32<$t0> copySize = bufferSize & 0xFFFF;
  srl $s6, $a1, 24 ## L:330 | u32 segment = addrIn >> 24;
  lw $s6, %lo(SEGMENT_TABLE)($s6) ## L:331 | segment = load(segment, SEGMENT_TABLE);
  srl $s4, $a2, 16 ## L:364 | u32<$s4> prt3d = addressInOut >> 16;
  or $t2, $zero, $zero ## L:371 | dma_in_async(prt3d, dmaAddrRDRAM, copySize);
  addu $s7, $s4, $t0 ## L:367 | u32 ptr3dEnd = prt3d + copySize;
  addu $s0, $a1, $s6 ## L:332 | addrOut = addrIn + segment;
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2 ## L:371 | dma_in_async(prt3d, dmaAddrRDRAM, copySize);
  addiu $t0, $t0, -1 ## L:371 | dma_in_async(prt3d, dmaAddrRDRAM, copySize);
  ori $s6, $zero, %lo(MATRIX_NORMAL) ## L:322 | u16 address = MATRIX_NORMAL;
  ldv $v18, 0, 16, $s6 ## L:325 | mat2 = load(address, 0x10).xyzwxyzw;
  ldv $v20, 0, 0, $s6 ## L:323 | mat0 = load(address, 0x00).xyzwxyzw;
  ldv $v18, 8, 16, $s6 ## L:325 | mat2 = load(address, 0x10).xyzwxyzw;
  ldv $v19, 0, 8, $s6 ## L:324 | mat1 = load(address, 0x08).xyzwxyzw;
  ldv $v20, 8, 0, $s6 ## L:323 | mat0 = load(address, 0x00).xyzwxyzw;
  jal loadCurrentMVPMat ## Args: $v27, $v25, $v23, $v21 ## L:378 | loadCurrentMVPMat(mat0, mat1, mat2, mat3);
  ldv $v19, 8, 8, $s6 ## L:324 | mat1 = load(address, 0x08).xyzwxyzw;
  lbu $s1, %lo(FOG_STORE_OFFSET + 0) ## L:409 | u8 fogStoreOffset = load(FOG_STORE_OFFSET);
  ori $at, $zero, %lo(NORMAL_MASK_SHIFT) ## L:380 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v17, 0, 0, $at ## L:380 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v17, 8, 0, $at ## L:380 | vec16 normMask = load(NORMAL_MASK_SHIFT, 0x00).xyzwxyzw;
  ldv $v16, 0, 8, $at ## L:381 | vec16 normShift = load(NORMAL_MASK_SHIFT, 0x08).xyzwxyzw;
  ldv $v16, 8, 8, $at ## L:381 | vec16 normShift = load(NORMAL_MASK_SHIFT, 0x08).xyzwxyzw;
  ori $at, $zero, %lo(CLIPPING_PLANES) ## L:383 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  andi $s6, $a2, 65535 ## L:396 | u32 ptrBuffA = addressInOut & 0xFFFF;
  llv $v15, 0, 0, $at ## L:383 | vec16 guardBandScale = load(CLIPPING_PLANES).xy;
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET) ## L:386 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v13, 0, 0, $at ## L:386 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v12, 0, 8, $at ## L:390 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  addiu $s5, $s6, 65500 ## L:397 | u32 ptrBuffB = ptrBuffA - 36;
  ldv $v13, 8, 0, $at ## L:386 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v12, 8, 8, $at ## L:390 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  vor $v15, $v00, $v15.e1 ## L:384 | guardBandScale = guardBandScale.y;
  lbu $s2, %lo(ACTIVE_LIGHT_SIZE + 0) ## L:401 | u8 ptrLightEnd = load(ACTIVE_LIGHT_SIZE);
  ori $at, $zero, %lo(FOG_SCALE_OFFSET) ## L:391 | vec16 fogScaleOffset = load(FOG_SCALE_OFFSET).xyzw;
  addiu $s6, $s6, 65464 ## L:410 | ptrBuffA -= 72;
  ldv $v11, 0, 0, $at ## L:391 | vec16 fogScaleOffset = load(FOG_SCALE_OFFSET).xyzw;
  lbu $k0, %lo(TRI_COMMAND + 0) ## L:415 | u8 isUnlit = load(TRI_COMMAND);
  ori $at, $zero, %lo(NORM_SCALE_W) ## L:394 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v10, 0, 0, $at ## L:394 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmudl $v14, $v00, $v31.e3 ## L:388 | screenSize >>= 4;
  ldv $v10, 8, 0, $at ## L:394 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmadm $v13, $v13, $v31.e3 ## L:388 | screenSize >>= 4;
  ori $at, $zero, %lo(UV_GEN_PARAMS) ## L:405 | vec16 uvGenArgs = load(UV_GEN_PARAMS).xyzw;
  ldv $v09, 0, 0, $at ## L:405 | vec16 uvGenArgs = load(UV_GEN_PARAMS).xyzw;
  llv $v09, 8, 0, $at ## L:406 | uvGenArgs.XY = load(UV_GEN_PARAMS).xy;
  vmadn $v14, $v00, $v00 ## L:388 | screenSize >>= 4;
  vmov $v11.e6, $v11.e2 ## L:392 | fogScaleOffset.Z = fogScaleOffset.z;
  ori $at, $zero, %lo(SCREEN_UVGEN_SCALE) ## L:407 | uvGenArgs.W = load(SCREEN_UVGEN_SCALE).x;
  lsv $v09, 14, 0, $at ## L:407 | uvGenArgs.W = load(SCREEN_UVGEN_SCALE).x;
  jal DMAWaitIdle ## L:418 | dma_await();
  addiu $s2, $s2, %lo(LIGHT_DIR_COLOR) ## L:402 | ptrLightEnd += LIGHT_DIR_COLOR;
  lqv $v08, 0, 0, $s4 ## L:421 | vec16 pos = load(prt3d, 0x00);
  ori $at, $zero, %lo(CLIPPING_PLANES) ## L:425 | pos.w = load(CLIPPING_PLANES, 4).x;
  vand $v07, $v17, $v08.h3 ## L:422 | vec16 norm = normMask & pos.wwwwWWWW;
  vmudn $v07, $v07, $v16.v ## L:423 | norm *= normShift;
  lsv $v08, 6, 4, $at ## L:425 | pos.w = load(CLIPPING_PLANES, 4).x;
  lsv $v08, 14, 4, $at ## L:426 | pos.W = load(CLIPPING_PLANES, 4).x;
  LOOP_START:
  # emux_trace_start # inline-ASM ## L:432 | asm("emux_trace_start");
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR) ## L:456 | ptrLight = LIGHT_DIR_COLOR;
  or $t9, $zero, $zero ## L:441 | u8 mvMatrixLoaded = 0;
  vor $v03, $v00, $v11.e2 ## L:439 | vec16 lightColor = fogScaleOffset.z;
  OP_NORM_MAT0:
  vmulf $v29, $v20, $v07.h0 ## L:449 | VTEMP:sfract = matN0:sfract  * norm.xxxxXXXX;
  addiu $s5, $s5, 72 ## L:444 | ptrBuffB += 72;
  OP_NORM_MAT1:
  vmacf $v29, $v19, $v07.h1 ## L:450 | VTEMP:sfract = matN1:sfract +* norm.yyyyYYYY;
  OP_NORM_MAT2:
  vmacf $v07, $v18, $v07.h2 ## L:451 | norm         = matN2:sfract +* norm.zzzzZZZZ;
  addiu $s6, $s6, 72 ## L:443 | ptrBuffA += 72;
  lhu $k1, %lo(VERTEX_FX_FUNC + 0) ## L:445 | vertexFxFunc = load(VERTEX_FX_FUNC);
  vmudn $v06, $v28, $v08.h0 ## L:200 | out = mat0  * vec.xxxxXXXX;
  vmadh $v05, $v27, $v08.h0 ## L:200 | out = mat0  * vec.xxxxXXXX;
  vmadn $v06, $v26, $v08.h1 ## L:201 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v05, $v25, $v08.h1 ## L:201 | out = mat1 +* vec.yyyyYYYY;
  lbu $t8, 15($s3) ## L:460 | u8 isPointLight = load(ptrLight, 15);
  vmadn $v06, $v24, $v08.h2 ## L:202 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v05, $v23, $v08.h2 ## L:202 | out = mat2 +* vec.zzzzZZZZ;
  lpv $v02, 0, 8, $s3 ## L:459 | vec16 lightDirVec  = load_vec_s8(ptrLight, 8);
  vmadn $v06, $v22, $v08.h3 ## L:203 | out = mat3 +* vec.wwwwWWWW;
  bne $k0, $zero, LIGHT_LOOP_END ## L:463 | if(isUnlit)goto LIGHT_LOOP_END;
  vmadh $v05, $v21, $v08.h3 ## L:203 | out = mat3 +* vec.wwwwWWWW;
  beq $s3, $s2, LIGHT_LOOP_END ## L:466 | if(ptrLight != ptrLightEnd)
  luv $v03, 0, -8, $s3 ## L:464 | lightColor = load_vec_u8(ptrLight, -8);
  LABEL_T3DCmd_VertLoad_0006:
  vmulf $v01, $v07, $v02.v ## L:470 | vec16 lightDirScale = norm:sfract * lightDirVec:sfract;
  luv $v04, 0, 0, $s3 ## L:471 | color = load_vec_u8(ptrLight);
  bne $t8, $zero, pointLight ## L:473 | if(isPointLight) {
  ori $ra, $zero, LABEL_T3DCmd_VertLoad_0008 ## L:946 |
  LABEL_T3DCmd_VertLoad_0008:
  vmulu $v29, $v04, $v01.h0 ## L:481 | VTEMP = color:ufract  * lightDirScale:ufract.xxxxXXXX;
  vmacu $v29, $v04, $v01.h1 ## L:482 | VTEMP = color:ufract +* lightDirScale:ufract.yyyyYYYY;
  vmacu $v29, $v04, $v01.h2 ## L:483 | VTEMP = color:ufract +* lightDirScale:ufract.zzzzZZZZ;
  addiu $s3, $s3, 16 ## L:487 | ptrLight += 16;
  lbu $t8, 15($s3) ## L:489 | isPointLight =  load(ptrLight, 15);
  lpv $v02, 0, 8, $s3 ## L:488 | lightDirVec = load_vec_s8(ptrLight, 8);
  bne $s3, $s2, LABEL_T3DCmd_VertLoad_0006 ## L:489 | isPointLight =  load(ptrLight, 15);
  vadd $v03, $v03, $v29.v ## L:485 | lightColor:sint += VTEMP;
  LIGHT_LOOP_END:
  vmudm $v02, $v15, $v06.h3 ## L:495 | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  luv $v04, 0, 16, $s4 ## L:497 | color = load_vec_u8(prt3d, 0x10);
  vmadh $v01, $v15, $v05.h3 ## L:495 | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  addu $fp, $s6, $s1 ## L:498 | fogA = ptrBuffA + fogStoreOffset;
  vmadn $v02, $v00, $v00 ## L:495 | vec32 clipPlaneW = guardBandScale * posClip.wwwwWWWW;
  bne $t9, $zero, loadCurrentMVPMat ## L:500 | if(mvMatrixLoaded) {
  ori $ra, $zero, LABEL_T3DCmd_VertLoad_0009 ## L:946 |
  LABEL_T3DCmd_VertLoad_0009:
  vch $v29, $v05, $v05.h3 ## L:508 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v06, $v06.h3 ## L:508 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  addu $sp, $s5, $s1 ## L:563 | fogB = ptrBuffB + fogStoreOffset;
  cfc2 $t4, $vcc ## L:508 | u32 rejCodes = clip(posClip, posClip.wwwwWWWW);
  vch $v29, $v05, $v01 ## L:509 | u32 clipCodes = clip(posClip, clipPlaneW);
  vcl $v29, $v06, $v02 ## L:509 | u32 clipCodes = clip(posClip, clipPlaneW);
  vmulf $v04, $v04, $v03.v ## L:521 | color:sfract *= lightColor:ufract;
  lqv $v08, 0, 32, $s4 ## L:591 | pos = load(prt3d, 0x20);
  vmudl $v06, $v06, $v10.v ## L:523 | posClip *= normScaleW:ufract;
  cfc2 $t3, $vcc ## L:509 | u32 clipCodes = clip(posClip, clipPlaneW);
  srl $t5, $t4, 4 ## L:515 | rejCodesB = rejCodes >> 4;
  vmadm $v05, $v05, $v10.v ## L:523 | posClip *= normScaleW:ufract;
  vmadn $v06, $v00, $v00 ## L:523 | posClip *= normScaleW:ufract;
  andi $t6, $t4, 1799 ## L:295 | res = clipCode & 0b0000'0111'0000'0111;
  vmudm $v04, $v04, $v31.e6 ## L:525 | color >>= 7;
  andi $t8, $t3, 1799 ## L:287 | res = clipCode & 0b0000'0111'0000'0111;
  srl $t2, $t8, 5 ## L:288 | u32 tmp = res >> 5;
  srl $t7, $t3, 4 ## L:511 | clipCodeB = clipCodes >> 4;
  or $t8, $t8, $t2 ## L:289 | res |= tmp;
  andi $t7, $t7, 1799 ## L:287 | res = clipCode & 0b0000'0111'0000'0111;
  srl $t2, $t7, 5 ## L:288 | u32 tmp = res >> 5;
  vmudh $v04, $v04, $v09.e2 ## L:526 | color:sint *= uvGenArgs:sint.z;
  vrcph $v13.e3, $v05.e3 ## L:539 | screenSize.w = invert_half(posClip).w;
  or $t7, $t7, $t2 ## L:289 | res |= tmp;
  vrcpl $v14.e3, $v06.e3 ## L:539 | screenSize.w = invert_half(posClip).w;
  sdv $v06, 0, 24, $s6 ## L:533 | store(posClip.xyzw, ptrBuffA, 0x10);
  vrcph $v13.e3, $v05.e7 ## L:540 | screenSize.W = invert_half(posClip).W;
  srl $t2, $t6, 5 ## L:296 | u32 tmp = res >> 5;
  vrcpl $v14.e7, $v06.e7 ## L:540 | screenSize.W = invert_half(posClip).W;
  nor $t6, $t6, $t2 ## L:297 | res = res ~| tmp;
  vaddc $v03, $v06, $v11.e1 ## L:551 | fog:ufract = posClip + fogScaleOffset:ufract.y;
  sdv $v05, 8, 16, $s5 ## L:534 | store(posClip.XYZW, ptrBuffB, 0x10);
  vrcph $v13.e7, $v00.e7 ## L:540 | screenSize.W = invert_half(posClip).W;
  andi $t6, $t6, 255 ## L:584 | rejCodesA &= 0xFF;
  vadd $v02, $v05, $v11.e0 ## L:552 | fog:sint   = posClip + fogScaleOffset.x;
  sdv $v06, 8, 24, $s5 ## L:534 | store(posClip.XYZW, ptrBuffB, 0x10);
  sll $t8, $t8, 8 ## L:583 | clipCodeA <<= 8;
  vmudl $v29, $v06, $v14.h3 ## L:554 | posClip *= screenSize.wwwwWWWW;
  sdv $v05, 0, 16, $s6 ## L:533 | store(posClip.xyzw, ptrBuffA, 0x10);
  vmadm $v29, $v05, $v14.h3 ## L:554 | posClip *= screenSize.wwwwWWWW;
  vmadn $v06, $v06, $v13.h3 ## L:554 | posClip *= screenSize.wwwwWWWW;
  ldv $v03, 0, 24, $s4 ## L:577 | vec16 uv = load(prt3d, 0x18).xyzw;
  vmadh $v05, $v05, $v13.h3 ## L:554 | posClip *= screenSize.wwwwWWWW;
  suv $v04, 0, 8, $s6 ## Barrier: 0x1 ## L:528 | @Barrier("color-fog") store_vec_u8(color.x, ptrBuffA, 0x08);
  vmudh $v02, $v02, $v11.e3 ## L:556 | fog:sint *= fogScaleOffset:sint.w;
  or $t8, $t8, $t6 ## L:585 | clipCodeA |= rejCodesA;
  suv $v04, 4, 8, $s5 ## Barrier: 0x1 ## L:529 | @Barrier("color-fog") store_vec_u8(color.X, ptrBuffB, 0x08);
  ssv $v14, 6, 34, $s6 ## L:544 | store(screenSize.w, ptrBuffA, 0x20);
  vmudl $v29, $v06, $v14.v ## L:558 | posClip *= screenSize;
  ssv $v13, 14, 32, $s5 ## L:545 | store(screenSize.W, ptrBuffB, 0x20);
  vmadm $v29, $v05, $v14.v ## L:558 | posClip *= screenSize;
  vmadn $v06, $v06, $v13.v ## L:558 | posClip *= screenSize;
  andi $t5, $t5, 1799 ## L:295 | res = clipCode & 0b0000'0111'0000'0111;
  srl $t2, $t5, 5 ## L:296 | u32 tmp = res >> 5;
  vmadh $v05, $v05, $v13.v ## L:558 | posClip *= screenSize;
  vsub $v29, $v11, $v02.h2 ## L:560 | VTEMP:sint = fogScaleOffset - fog:sint.zzzzZZZZ;
  ssv $v14, 14, 34, $s5 ## L:545 | store(screenSize.W, ptrBuffB, 0x20);
  nor $t5, $t5, $t2 ## L:297 | res = res ~| tmp;
  vmadh $v05, $v12, $v30.e7 ## L:575 | posClip:sint = screenOffset:sint +* 1;
  vor $v02, $v00, $v07 ## L:592 | vec16 oldNorm = norm;
  ssv $v13, 6, 32, $s6 ## L:544 | store(screenSize.w, ptrBuffA, 0x20);
  addiu $s4, $s4, 32 ## L:598 | prt3d += 0x20;
  vand $v07, $v17, $v08.h3 ## L:593 | norm = normMask & pos.wwwwWWWW;
  sfv $v29.e0, 0, $fp ## Barrier: 0x3 ## L:569 | asm_op("sfv", VTEMP:sint.x, 0, fogA);
  sfv $v29.e4, 0, $sp ## Barrier: 0x3 ## L:573 | asm_op("sfv", VTEMP:sint.X, 0, fogB);
  sdv $v05, 0, 0, $s6 ## Barrier: 0x2 ## L:579 | @Barrier("pos-cc") store(posClip:sint.xyzw, ptrBuffA, 0x00);
  sdv $v05, 8, 0, $s5 ## Barrier: 0x2 ## L:580 | @Barrier("pos-cc") store(posClip:sint.XYZW, ptrBuffB, 0x00);
  vmudn $v07, $v07, $v16.v ## L:594 | norm *= normShift;
  sh $t8, 6($s6) ## Barrier: 0x2 ## L:586 | @Barrier("pos-cc") store(clipCodeA:u16, ptrBuffA, 0x06);
  sb $t7, 6($s5) ## Barrier: 0x2 ## L:588 | @Barrier("pos-cc") store(clipCodeB:u8, ptrBuffB, 0x06);
  vmov $v08.e3, $v30.e7 ## L:596 | pos.w = 1;
  vmov $v08.e7, $v30.e7 ## L:597 | pos.W = 1;
  jal $k1 ## L:600 | vertexFxFunc();
  sb $t5, 7($s5) ## Barrier: 0x2 ## L:589 | @Barrier("pos-cc") store(rejCodesB:u8, ptrBuffB, 0x07);
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
  bne $t9, $zero, LABEL_pointLight_0001
  lhu $t7, 14($s3)
  ori $at, $zero, %lo(MATRIX_MV)
  ldv $v27, 0, 0, $at
  ldv $v28, 0, 8, $at
  ldv $v26, 0, 24, $at
  ldv $v28, 8, 8, $at
  ldv $v25, 0, 16, $at
  ldv $v27, 8, 0, $at
  ldv $v25, 8, 16, $at
  ldv $v21, 0, 48, $at
  vmudn $v28, $v28, $v08.h0
  ldv $v26, 8, 24, $at
  ldv $v24, 0, 40, $at
  ldv $v24, 8, 40, $at
  ldv $v23, 0, 32, $at
  vmadh $v27, $v27, $v08.h0
  ldv $v23, 8, 32, $at
  vmadn $v28, $v26, $v08.h1
  ldv $v22, 0, 56, $at
  ldv $v22, 8, 56, $at
  vmadh $v27, $v25, $v08.h1
  ldv $v21, 8, 48, $at
  vmadn $v28, $v24, $v08.h2
  addiu $t9, $zero, 1
  vmadh $v27, $v23, $v08.h2
  vmadn $v28, $v22, $v08.h3
  vmadh $v27, $v21, $v08.h3
  vmudl $v28, $v28, $v31.e3
  vmadm $v27, $v27, $v31.e3
  vmadn $v28, $v00, $v00
  LABEL_pointLight_0001:
  ldv $v23, 0, 8, $s3
  ldv $v23, 8, 8, $s3
  vmudm $v23, $v23, $v31.e7
  vmadn $v24, $v00, $v00
  andi $t7, $t7, 32767
  vsubc $v24, $v24, $v28.v
  sll $t7, $t7, 13
  srl $at, $t7, 16
  vsub $v23, $v23, $v27.v
  vmudl $v29, $v24, $v24.v
  vmadm $v29, $v23, $v24.v
  vmadn $v22, $v24, $v23.v
  vmadh $v21, $v23, $v23.v
  vaddc $v26, $v22, $v22.h1
  vadd $v25, $v21, $v21.h1
  vaddc $v22, $v22, $v26.h2
  vadd $v21, $v21, $v25.h2
  vrsqh $v21.e0, $v21.e0
  vrsql $v22.e0, $v22.e0
  vrsqh $v21.e0, $v21.e4
  mtc2 $at, $v25.e3
  vrsql $v22.e4, $v22.e4
  mtc2 $t7, $v26.e3
  vrsqh $v21.e4, $v00.e0
  lbu $t7, 14($s3)
  vmudl $v29, $v22, $v26.e3
  sra $t7, $t7, 7
  vmadm $v29, $v21, $v26.e3
  vmadn $v29, $v22, $v25.e3
  bne $t7, $zero, LABEL_pointLight_0002
  vmadh $v01, $v21, $v25.e3
  vmudn $v22, $v22, $v31.e7
  vmadh $v21, $v21, $v31.e7
  vmulf $v01, $v01, $v01.v
  vmudl $v29, $v24, $v22.h0
  vmadm $v29, $v23, $v22.h0
  vmadn $v29, $v24, $v21.h0
  vmadh $v08, $v23, $v21.h0
  vmulf $v08, $v08, $v01.h0
  jr $ra
  vmulf $v01, $v08, $v07.v
  LABEL_pointLight_0002:
  vmulf $v01, $v01, $v01.v
  vmov $v01.e5, $v00.e0
  vmov $v01.e1, $v00.e0
  vmov $v01.e2, $v00.e0
  jr $ra
  vmov $v01.e6, $v00.e0
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
  sra $s6, $a2, 16
  vxor $v28, $v00, $v00.e0
  mtc2 $s6, $v27.e0
  sh $a3, %lo(6 + NORM_SCALE_W)($zero)
  sw $a1, %lo(8 + SCREEN_SCALE_OFFSET)($zero)
  sw $a2, %lo(0 + SCREEN_SCALE_OFFSET)($zero)
  vrcph $v27.e0, $v27.e0
  andi $a0, $a0, 15
  vrcpl $v28.e0, $v28.e0
  sb $a0, %lo(3 + CLIPPING_PLANES)($zero)
  ori $s6, $zero, %lo(RSPQ_Loop)
  vrcph $v27.e0, $v00.e0
  vmudm $v26, $v28, $v31.e1
  subu $s7, $zero, $a0
  sh $s6, %lo(CLIP_RA)($zero)
  sb $s7, %lo(19 + CLIPPING_PLANES)($zero)
  sb $s7, %lo(15 + CLIPPING_PLANES)($zero)
  sw $a3, %lo(4 + SCREEN_SCALE_OFFSET)($zero)
  vaddc $v26, $v26, $v30.e7
  sb $a0, %lo(7 + CLIPPING_PLANES)($zero)
  ori $at, $zero, %lo(SCREEN_UVGEN_SCALE)
  j RSPQ_Loop
  ssv $v26, 0, 0, $at
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
T3DCmd_TriDraw_End:
  j RSPQ_Loop
  sh $s7, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)
  RDPQ_Triangle_Clip:
  jal RDPQ_Triangle_Send_End
  sh $s4, %lo(18 + RSPQ_SCRATCH_MEM)($zero)
  j CLIPPING_CODE_TARGET
  ori $s4, $zero, %lo(CLIPPING_CODE_TARGET)
T3DCmd_TriDraw_Strip:
  addiu $t2, $zero, 12
  andi $s4, $a1, 65535
  srl $t0, $a1, 16
  addiu $t0, $t0, -1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a0
  lbu $v0, %lo(FACE_CULLING + 0)
  ori $ra, $zero, %lo(STRIP_LOOP_NEXT)
  addu $s2, $s4, $t0
  addiu $s2, $s2, 65533
  addiu $s4, $s4, 65534
  xori $v0, $v0, 1
  sh $ra, %lo(CLIP_RA)($zero)
  sb $v0, %lo(16 + RSPQ_SCRATCH_MEM)($zero)
  STRIP_LOOP_NEW:
  lbu $v0, %lo(RSPQ_SCRATCH_MEM + 16)
  addiu $s4, $s4, 2
  STRIP_LOOP_NEXT:
  beq $s4, $s2, STRIP_LOOP_END
  xori $v0, $v0, 1
  lh $a1, 4($s4)
  lhu $a0, 0($s4)
  lhu $a2, 2($s4)
  bltz $a1, STRIP_LOOP_NEW
  addiu $s4, $s4, 2
  addiu $v1, $s4, 65534
  jal RDPQ_Triangle_Send_Async ## Args: $a0, $a1, $a2, $sp, $v0
  ori $sp, $zero, %lo(STRIP_LOOP_NEXT)
  j STRIP_LOOP_NEXT
  sh $s7, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)
  STRIP_LOOP_END:
  ori $sp, $zero, %lo(RSPQ_Loop)
  j RSPQ_Loop
  sh $sp, %lo(CLIP_RA)($zero)
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