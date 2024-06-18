## Auto-generated file, transpiled with RSPL
#define TRI_BUFFER_COUNT 70
#define LIGHT_COUNT 4
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
    RSPQ_DefineCommand T3DCmd_LightSet, 12
    RSPQ_DefineCommand T3DCmd_RenderMode, 8
    RSPQ_DefineCommand T3DCmd_SetUVGen, 8
    RSPQ_DefineCommand T3DCmd_MatProjectionSet, 4
    RSPQ_DefineCommand T3DCmd_LightCount, 4
    RSPQ_DefineCommand T3DCmd_SetFogRange, 8
    RSPQ_DefineCommand T3DCmd_SetFogState, 4
    RSPQ_DefineCommand T3DCmd_TriSync, 4
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 3
    MATRIX_PROJ: .ds.b 64
    .align 3
    MATRIX_MVP: .ds.b 64
    .align 3
    MATRIX_NORMAL: .ds.b 24
    TRI_BUFFER: .ds.b 2520
    .align 4
    CLIP_BUFFER_TMP: .ds.b 252
    .align 4
    CLIP_BUFFER_RESULT: .ds.b 288
    .align 4
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 4
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 1
    UV_GEN_PARAMS: .half 0, 0
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0, 0, 0, 0, 65535
    .align 2
    SEGMENT_TABLE: .word 0, 0, 0, 0, 0, 0, 0, 0
    .align 2
    COLOR_AMBIENT: .ds.b 8
    .align 2
    LIGHT_DIR_COLOR: .ds.b 64
    .align 2
    TRI_COMMAND: .word 0
    .align 2
    MATRIX_STACK_PTR: .word 0
    .align 1
    FOG_SCALE_OFFSET: .half 0, 0, 0, 32767
    .align 1
    RDPQ_TRI_BUFF_OFFSET: .half 0
    .align 1
    SCREEN_UVGEN_SCALE: .half 0
    UV_GEN_FUNCTION: .byte 0
    FACE_CULLING: .byte 0
    USE_FOG: .byte 0
    ACTIVE_LIGHT_SIZE: .byte 0
    STATE_MEM_END:
  RSPQ_EndSavedState

  TEMP_STATE_MEM_START:
    .align 2
    CLIP_CODE_ADDR: .word 0
    .align 2
    CLIP_CODE_ORG_ADDR: .word 0
    .align 1
    CLIP_CODE_SIZE: .half 0
    .align 1
    CLIP_RA: .ds.b 2
  TEMP_STATE_MEM_END:

.text

.align 3
T3DCmd_VertLoad:
  CLIPPING_CODE_TARGET:
  andi $t0, $a0, 65535
  srl $s6, $a1, 24
  srl $s4, $a2, 16
  or $t2, $zero, $zero
  lw $s6, %lo(SEGMENT_TABLE)($s6)
  addu $s7, $s4, $t0
  addiu $t0, $t0, -1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addu $s0, $a1, $s6
  ori $s6, $zero, %lo(MATRIX_MVP)
  ori $at, $zero, %lo(NORMAL_MASK_SHIFT)
  ldv $v21, 0, 48, $s6
  ldv $v25, 0, 16, $s6
  ldv $v21, 8, 48, $s6
  ldv $v17, 0, 0, $at
  ldv $v24, 0, 40, $s6
  ldv $v17, 8, 0, $at
  ldv $v16, 0, 8, $at
  ldv $v26, 0, 24, $s6
  ldv $v27, 0, 0, $s6
  ldv $v16, 8, 8, $at
  ori $at, $zero, %lo(CLIPPING_PLANES)
  ldv $v28, 0, 8, $s6
  llv $v15, 0, 0, $at
  ldv $v22, 0, 56, $s6
  ldv $v25, 8, 16, $s6
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v13, 0, 0, $at
  vor $v15, $v00, $v15.e1
  ldv $v12, 0, 8, $at
  ldv $v12, 8, 8, $at
  ldv $v13, 8, 0, $at
  ldv $v24, 8, 40, $s6
  ldv $v26, 8, 24, $s6
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR)
  ldv $v23, 0, 32, $s6
  ldv $v27, 8, 0, $s6
  ldv $v22, 8, 56, $s6
  ldv $v28, 8, 8, $s6
  ldv $v23, 8, 32, $s6
  ori $s6, $zero, %lo(MATRIX_NORMAL)
  ori $at, $zero, %lo(FOG_SCALE_OFFSET)
  vmudl $v14, $v00, $v31.e3
  ldv $v19, 0, 8, $s6
  ldv $v11, 0, 0, $at
  lbu $s2, %lo(ACTIVE_LIGHT_SIZE + 0)
  ori $at, $zero, %lo(NORM_SCALE_W)
  ldv $v10, 0, 0, $at
  ldv $v10, 8, 0, $at
  ldv $v20, 0, 0, $s6
  ori $at, $zero, %lo(UV_GEN_PARAMS)
  llv $v09, 0, 0, $at
  llv $v09, 8, 0, $at
  ldv $v19, 8, 8, $s6
  ori $at, $zero, %lo(SCREEN_UVGEN_SCALE)
  vmadm $v13, $v13, $v31.e3
  addu $s2, $s2, $s3
  ldv $v18, 0, 16, $s6
  ldv $v20, 8, 0, $s6
  lsv $v09, 6, 0, $at
  ldv $v18, 8, 16, $s6
  vmadn $v14, $v00, $v00
  andi $s6, $a2, 65535
  jal DMAWaitIdle
  addiu $s5, $s6, 36
  lqv $v08, 0, 0, $s4
  vand $v07, $v17, $v08.h3
  vmudn $v07, $v07, $v16.v
  LOOP_START:
  ori $at, $zero, %lo(CLIPPING_PLANES)
  lsv $v08, 6, 4, $at
  lsv $v08, 14, 4, $at
  lpv $v03, 0, 8, $s3
  vmulf $v06, $v20, $v07.h0
  luv $v04, 0, 16, $s4
  vmacf $v06, $v19, $v07.h1
  lbu $s1, %lo(UV_GEN_FUNCTION + 0)
  vmacf $v07, $v18, $v07.h2
  vmudn $v06, $v28, $v08.h0
  vmadh $v05, $v27, $v08.h0
  vmadn $v06, $v26, $v08.h1
  vmadh $v05, $v25, $v08.h1
  ori $at, $zero, %lo(COLOR_AMBIENT)
  vmadn $v06, $v24, $v08.h2
  vmadh $v05, $v23, $v08.h2
  lbu $fp, %lo(USE_FOG + 0)
  vmadn $v06, $v22, $v08.h3
  vmadh $v05, $v21, $v08.h3
  luv $v08, 0, 0, $at
  LABEL_0003:
  beq $s3, $s2, LABEL_0004
  vmulf $v02, $v07, $v03.v
  vadd $v01, $v02, $v02.h1
  vadd $v02, $v02, $v01.h2
  luv $v01, 0, 0, $s3
  addiu $s3, $s3, 16
  lpv $v03, 0, 8, $s3
  vmulu $v02, $v01, $v02.h0
  j LABEL_0003
  vadd $v08, $v08, $v02.v
  LABEL_0004:
  vmulf $v04, $v04, $v08.v
  vmudl $v29, $v00, $v06.h3
  vmadm $v29, $v15, $v06.h3
  vmadn $v03, $v00, $v05.h3
  vmadh $v02, $v15, $v05.h3
  vch $v29, $v05, $v02
  vcl $v29, $v06, $v03
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR)
  cfc2 $t8, $vcc
  vch $v29, $v05, $v05.h3
  vcl $v29, $v06, $v06.h3
  vmudl $v06, $v06, $v10.v
  vmadm $v05, $v05, $v10.v
  andi $sp, $t8, 1799
  srl $t7, $sp, 5
  srl $k1, $t8, 4
  vmadn $v06, $v00, $v00
  or $sp, $sp, $t7
  sdv $v05, 8, 16, $s5
  sdv $v05, 0, 16, $s6
  vrcph $v05.e3, $v05.e3
  sdv $v06, 8, 24, $s5
  sdv $v06, 0, 24, $s6
  vrcpl $v06.e3, $v06.e3
  andi $k1, $k1, 1799
  srl $t7, $k1, 5
  vrcph $v05.e3, $v05.e7
  vrcpl $v06.e7, $v06.e7
  or $k1, $k1, $t7
  cfc2 $t7, $vcc
  vrcph $v05.e7, $v00.e7
  vaddc $v03, $v06, $v11.e2
  srl $t9, $t7, 4
  andi $k0, $t7, 1799
  vadd $v02, $v05, $v11.e1
  andi $t9, $t9, 1799
  srl $t6, $k0, 5
  vmudn $v03, $v03, $v11.e0
  vmadh $v02, $v02, $v11.e0
  nor $k0, $k0, $t6
  vor $v01, $v00, $v11.e3
  srl $t6, $t9, 5
  nor $t9, $t9, $t6
  vsub $v02, $v01, $v02.v
  beq $fp, $zero, LABEL_0005
  nop
  vmov $v04.e3, $v02.e2
  vmov $v04.e7, $v02.e6
  LABEL_0005:
  ssv $v05, 14, 32, $s5
  ssv $v06, 6, 34, $s6
  vmudl $v29, $v06, $v06.h3
  vmadm $v29, $v05, $v06.h3
  ssv $v06, 14, 34, $s5
  ssv $v05, 6, 32, $s6
  vmadn $v06, $v06, $v05.h3
  vmadh $v05, $v05, $v05.h3
  suv $v04, 4, 8, $s5
  vmudl $v29, $v06, $v14.v
  ldv $v03, 0, 24, $s4
  vmadm $v29, $v05, $v14.v
  addiu $s4, $s4, 32
  vmadn $v06, $v06, $v13.v
  vmadh $v05, $v05, $v13.v
  lqv $v08, 0, 0, $s4
  vadd $v05, $v05, $v12.v
  suv $v04, 0, 8, $s6
  vor $v02, $v00, $v07
  vand $v07, $v17, $v08.h3
  sdv $v05, 0, 0, $s6 ## Barrier: 0x1
  sdv $v05, 8, 0, $s5 ## Barrier: 0x1
  sb $sp, 6($s6) ## Barrier: 0x1
  sb $k1, 6($s5) ## Barrier: 0x1
  sb $k0, 7($s6) ## Barrier: 0x1
  vmudn $v07, $v07, $v16.v
  bne $s1, $zero, LABEL_0006
  sb $t9, 7($s5) ## Barrier: 0x1
  slv $v03, 4, 12, $s5
  slv $v03, 0, 12, $s6
  addiu $s6, $s6, 72
  bne $s4, $s7, LOOP_START
  addiu $s5, $s5, 72
  j RSPQ_Loop
  nop
  LABEL_0006:
  vsubc $v01, $v05, $v12.v
  vmudn $v01, $v01, $v09.e3
  vmulf $v03, $v02, $v09.v
  vmulf $v01, $v01, $v02.h2
  vaddc $v03, $v03, $v09.e0
  addiu $s6, $s6, 72
  vmulf $v01, $v01, $v09.e0
  vaddc $v03, $v03, $v01.v
  addiu $s5, $s5, 72
  slv $v03, 8, -60, $s5
  bne $s4, $s7, LOOP_START
  slv $v03, 0, -60, $s6
  LABEL_0002:
  j RSPQ_Loop
  nop
T3DCmd_SetScreenSize:
  sra $s6, $a2, 16
  mtc2 $s6, $v27.e0
  sh $a3, %lo(6 + NORM_SCALE_W)($zero)
  vxor $v28, $v00, $v00.e0
  sw $a1, %lo(8 + SCREEN_SCALE_OFFSET)($zero)
  andi $a0, $a0, 15
  vrcph $v27.e0, $v27.e0
  vrcpl $v28.e0, $v28.e0
  sb $a0, %lo(7 + CLIPPING_PLANES)($zero)
  subu $s7, $zero, $a0
  sw $a3, %lo(4 + SCREEN_SCALE_OFFSET)($zero)
  vrcph $v27.e0, $v00.e0
  ori $at, $zero, %lo(SCREEN_UVGEN_SCALE)
  sh $a3, %lo(12 + NORM_SCALE_W)($zero)
  vmudm $v26, $v28, $v31.e1
  sb $s7, %lo(19 + CLIPPING_PLANES)($zero)
  sb $s7, %lo(15 + CLIPPING_PLANES)($zero)
  sh $a3, %lo(8 + NORM_SCALE_W)($zero)
  sh $a3, %lo(10 + NORM_SCALE_W)($zero)
  vaddc $v26, $v26, $v30.e7
  sb $a0, %lo(3 + CLIPPING_PLANES)($zero)
  sw $a2, %lo(0 + SCREEN_SCALE_OFFSET)($zero)
  j RSPQ_Loop
  ssv $v26, 0, 0, $at
T3DCmd_SetFogRange:
  sh $a0, %lo(0 + FOG_SCALE_OFFSET)($zero)
  j RSPQ_Loop
  sw $a1, %lo(2 + FOG_SCALE_OFFSET)($zero)
T3DCmd_SetFogState:
  j RSPQ_Loop
  sb $a0, %lo(USE_FOG)($zero)
T3DCmd_LightSet:
  beq $a2, $zero, LABEL_0008
  sw $a1, 0($a0)
  sw $a2, 8($a0)
  sw $a2, 12($a0)
  LABEL_0008:
  j RSPQ_Loop
  sw $a1, 4($a0)
T3DCmd_LightCount:
  j RSPQ_Loop
  sb $a0, %lo(ACTIVE_LIGHT_SIZE)($zero)
T3DCmd_RenderMode:
  lbu $s7, %lo(RDPQ_OTHER_MODES + 0)
  sb $a0, %lo(FACE_CULLING)($zero)
  andi $s7, $s7, 56
  or $a1, $a1, $s7
  j RSPQ_Loop
  sw $a1, %lo(TRI_COMMAND)($zero)
mulMat4Mat4:
  ldv $v01, 0, 0, $s3
  ldv $v02, 0, 8, $s3
  ldv $v10, 0, 8, $s4
  ldv $v03, 0, 16, $s3
  ldv $v01, 8, 0, $s3
  ldv $v04, 0, 24, $s3
  ldv $v02, 8, 8, $s3
  ldv $v05, 0, 32, $s3
  ldv $v09, 0, 0, $s4
  ldv $v10, 8, 24, $s4
  ldv $v09, 8, 16, $s4
  ldv $v04, 8, 24, $s3
  ldv $v03, 8, 16, $s3
  ldv $v07, 0, 48, $s3
  vmudl $v29, $v02, $v10.h0
  ldv $v08, 0, 56, $s3
  vmadm $v29, $v01, $v10.h0
  vmadn $v14, $v02, $v09.h0
  ldv $v06, 0, 40, $s3
  ldv $v07, 8, 48, $s3
  vmadh $v13, $v01, $v09.h0
  vmadl $v29, $v04, $v10.h1
  ldv $v06, 8, 40, $s3
  ldv $v05, 8, 32, $s3
  vmadm $v29, $v03, $v10.h1
  vmadn $v14, $v04, $v09.h1
  ldv $v12, 0, 40, $s4
  ldv $v08, 8, 56, $s3
  vmadh $v13, $v03, $v09.h1
  vmadl $v29, $v06, $v10.h2
  vmadm $v29, $v05, $v10.h2
  ldv $v11, 0, 32, $s4
  vmadn $v14, $v06, $v09.h2
  ldv $v12, 8, 56, $s4
  vmadh $v13, $v05, $v09.h2
  vmadl $v29, $v08, $v10.h3
  vmadm $v29, $v07, $v10.h3
  vmadn $v10, $v08, $v09.h3
  ldv $v11, 8, 48, $s4
  vmadh $v09, $v07, $v09.h3
  vmudl $v29, $v02, $v12.h0
  vmadm $v29, $v01, $v12.h0
  vmadn $v14, $v02, $v11.h0
  vmadh $v13, $v01, $v11.h0
  vmadl $v29, $v04, $v12.h1
  vmadm $v29, $v03, $v12.h1
  vmadn $v14, $v04, $v11.h1
  vmadh $v13, $v03, $v11.h1
  vmadl $v29, $v06, $v12.h2
  sdv $v09, 8, 16, $s2
  vmadm $v29, $v05, $v12.h2
  vmadn $v14, $v06, $v11.h2
  vmadh $v13, $v05, $v11.h2
  vmadl $v29, $v08, $v12.h3
  vmadm $v29, $v07, $v12.h3
  vmadn $v12, $v08, $v11.h3
  vmadh $v11, $v07, $v11.h3
  sdv $v09, 0, 0, $s2
  sdv $v10, 0, 8, $s2
  sdv $v10, 8, 24, $s2
  sdv $v12, 8, 56, $s2
  sdv $v11, 0, 32, $s2
  sdv $v11, 8, 48, $s2
  jr $ra
  sdv $v12, 0, 40, $s2
normalizeMatrixVector:
  ldv $v02, 0, 8, $s4
  ldv $v01, 0, 0, $s4
  vmudl $v29, $v02, $v02.v
  vmadm $v29, $v01, $v02.v
  vmadn $v04, $v02, $v01.v
  vmadh $v03, $v01, $v01.v
  vaddc $v06, $v04, $v04.h1
  vadd $v05, $v03, $v03.h1
  vaddc $v04, $v04, $v06.h2
  addiu $s4, $s4, 16
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
  andi $s6, $a0, 2
  lw $s5, %lo(MATRIX_STACK_PTR + 0)
  andi $s7, $a0, 1
  lh $a0, %lo(RSPQ_DMEM_BUFFER  -7)($gp)
  addu $s5, $s5, $a0
  bne $s6, $zero, RSPQ_Loop
  sw $s5, %lo(MATRIX_STACK_PTR)($zero)
  srl $s3, $a1, 24
  slt $at, $a0, $zero
  lw $s3, %lo(SEGMENT_TABLE)($s3)
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  beq $at, $zero, LABEL_000A
  addu $a1, $a1, $s3
  or $a1, $zero, $s5
  LABEL_000A:
  lui $at, 0xFF
  addiu $t2, $zero, 12
  ori $at, $at, 0xFFFF
  and $s0, $a1, $at
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $t0, $zero, 63
  beq $s7, $zero, LABEL_000B
  ori $t1, $zero, %lo(MATRIX_NORMAL)
  addiu $t2, $zero, 12
  addiu $s0, $s5, 65472
  ori $s4, $zero, %lo(CLIP_BUFFER_RESULT)
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $t0, $zero, 63
  ori $s2, $zero, %lo(CLIP_BUFFER_TMP)
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s3, $zero, %lo(CLIP_BUFFER_RESULT)
  LABEL_000B:
  or $s0, $zero, $s5
  ori $t0, $zero, 63
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t2, $zero, -32756
  jal normalizeMatrixVector ## Args: $s4, $t1
  ori $s2, $zero, %lo(MATRIX_MVP)
  jal normalizeMatrixVector ## Args: $s4, $t1
  ori $s3, $zero, %lo(MATRIX_PROJ)
  jal normalizeMatrixVector ## Args: $s4, $t1
  nop
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  j RSPQ_Loop
  nop
T3DCmd_MatProjectionSet:
  lui $at, 0xFF
  ori $t0, $zero, 63
  addiu $t2, $zero, 12
  ori $s4, $zero, %lo(MATRIX_PROJ)
  ori $at, $at, 0xFFFF
  j DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  and $s0, $a0, $at
T3DCmd_SetWord:
  j RSPQ_Loop
  sw $a1, ($a0)
T3DCmd_SetUVGen:
  sb $a0, %lo(UV_GEN_FUNCTION)($zero)
  j RSPQ_Loop
  sw $a1, %lo(UV_GEN_PARAMS)($zero)
T3DCmd_TriDraw:
  srl $a2, $a1, 16
  ori $sp, $zero, %lo(RSPQ_Loop)
  jal RDPQ_Triangle_Send_Async ## Args: $a0, $a1, $a2
  addiu $v1, $gp, %lo(RSPQ_DMEM_BUFFER)  -6
  lhu $s7, %lo(RDPQ_TRI_BUFF_OFFSET + 0)
  bne $v1, $zero, LABEL_0001
  nop
  xori $s7, $s7, 176
  j RSPQ_Loop
  sh $s7, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)
  LABEL_0001:
  lhu $a2, %lo(RSPQ_DMEM_BUFFER  -2)($gp)
  jal RDPQ_Triangle_Send_End
  lhu $a1, %lo(RSPQ_DMEM_BUFFER  -4)($gp)
  lbu $s7, %lo(CLIP_CODE_ADDR + 0)
  lhu $a0, %lo(RSPQ_DMEM_BUFFER  -6)($gp)
  bne $s7, $zero, DO_CLIPPING
  nop
  lhu $t0, %lo(CLIP_CODE_SIZE + 0)
  addiu $t2, $zero, 12
  ori $s4, $zero, %lo(CLIPPING_CODE_TARGET)
  sb $t0, %lo(CLIP_CODE_ADDR)($zero)
  lw $s0, %lo(CLIP_CODE_ADDR + 0)
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  DO_CLIPPING:
  ori $ra, $zero, %lo(RSPQ_Loop)
  j CLIPPING_CODE_TARGET
  sh $ra, %lo(CLIP_RA)($zero)
T3DCmd_TriSync:
  lbu $s7, %lo(CLIP_CODE_ADDR + 0)
  beq $s7, $zero, RDPQ_Triangle_Send_End
  ori $s4, $zero, %lo(CLIPPING_CODE_TARGET)
  lhu $t0, %lo(CLIP_CODE_SIZE + 0)
  lw $s0, %lo(CLIP_CODE_ORG_ADDR + 0)
  or $t2, $zero, $zero
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  sb $zero, %lo(CLIP_CODE_ADDR)($zero)
  j RDPQ_Triangle_Send_End
  ori $ra, $zero, %lo(RSPQ_Loop)

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