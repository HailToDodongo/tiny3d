## Auto-generated file, transpiled with RSPL
#define TRI_BUFFER_COUNT 70
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
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand T3DCmd_VertLoad, 12
    RSPQ_DefineCommand T3DCmd_LightSet, 12
    RSPQ_DefineCommand T3DCmd_RenderMode, 8
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand T3DCmd_MatProjectionSet, 4
    RSPQ_DefineCommand T3DCmd_LightCount, 4
    RSPQ_DefineCommand T3DCmd_SetFogRange, 8
    RSPQ_DefineCommand T3DCmd_SetFogState, 4
    RSPQ_DefineCommand RDPQ_Triangle_Send_End, 4
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 3
    MATRIX_PROJ: .ds.b 64
    .align 3
    MATRIX_MVP: .ds.b 64
    .align 3
    MATRIX_NORMAL: .ds.b 32
    TRI_BUFFER: .ds.b 2520
    .align 4
    CLIP_BUFFER_TMP: .ds.b 252
    .align 4
    CLIP_BUFFER_RESULT: .ds.b 288
    .align 3
    SCREEN_SCALE_OFFSET: .ds.b 16
    .align 3
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 4
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 1
    NORM_SCALE_W: .half 65535, 65535, 65535, 1191, 1191, 1191, 1191, 65535
    .align 2
    COLOR_AMBIENT: .ds.b 8
    .align 2
    LIGHT_DIR_COLOR: .ds.b 128
    .align 2
    TRI_COMMAND: .word 0
    .align 2
    MATRIX_STACK_PTR: .word 0
    .align 1
    FOG_SCALE_OFFSET: .half 0, 0, 0, 32767
    .align 1
    CURRENT_MAT_ADDR: .half 0
    .align 1
    RDPQ_TRI_BUFF_OFFSET: .half 0
    USE_REJECT: .byte 0
    FACE_CULLING: .byte 0
    USE_FOG: .byte 0
    ACTIVE_LIGHT_SIZE: .byte 0
    STATE_MEM_END:
  RSPQ_EndSavedState

.text

T3DCmd_VertLoad:
  andi $t0, $a0, 65535
  srl $s4, $a2, 16
  or $t2, $zero, $zero
  or $s0, $zero, $a1
  addu $s7, $s4, $t0
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  ori $at, $zero, %lo(COLOR_AMBIENT)
  ori $s6, $zero, %lo(MATRIX_MVP)
  luv $v17, 0, 0, $at
  ori $at, $zero, %lo(NORMAL_MASK_SHIFT)
  ldv $v28, 0, 8, $s6
  ldv $v16, 0, 0, $at
  ldv $v25, 0, 16, $s6
  ldv $v22, 0, 56, $s6
  ldv $v24, 0, 40, $s6
  ldv $v21, 0, 48, $s6
  ldv $v16, 8, 0, $at
  ldv $v26, 0, 24, $s6
  ldv $v15, 0, 8, $at
  ldv $v21, 8, 48, $s6
  ldv $v23, 0, 32, $s6
  lbu $s2, %lo(ACTIVE_LIGHT_SIZE + 0)
  ldv $v26, 8, 24, $s6
  ldv $v15, 8, 8, $at
  ori $at, $zero, %lo(CLIPPING_PLANES)
  llv $v14, 0, 0, $at
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR)
  ldv $v23, 8, 32, $s6
  ldv $v22, 8, 56, $s6
  ldv $v25, 8, 16, $s6
  ldv $v28, 8, 8, $s6
  ldv $v24, 8, 40, $s6
  ldv $v27, 0, 0, $s6
  ldv $v27, 8, 0, $s6
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v13, 0, 0, $at
  ori $s6, $zero, %lo(MATRIX_NORMAL)
  ldv $v18, 0, 16, $s6
  ldv $v12, 0, 8, $at
  ldv $v20, 0, 0, $s6
  ldv $v19, 0, 8, $s6
  ldv $v18, 8, 16, $s6
  ldv $v12, 8, 8, $at
  ldv $v19, 8, 8, $s6
  vor $v14, $v00, $v14.e1
  ldv $v20, 8, 0, $s6
  andi $s6, $a2, 65535
  ldv $v13, 8, 0, $at
  ori $at, $zero, %lo(FOG_SCALE_OFFSET)
  addiu $s5, $s6, 36
  ldv $v11, 0, 0, $at
  jal DMAWaitIdle
  addu $s2, $s2, $s3
  LABEL_0001:
  lqv $v07, 0, 0, $s4
  vand $v06, $v16, $v07.h3
  ori $at, $zero, %lo(CLIPPING_PLANES)
  lsv $v07, 6, 4, $at
  lsv $v07, 14, 4, $at
  vmudn $v06, $v06, $v15.v
  vmulf $v05, $v20, $v06.h0
  vmacf $v05, $v19, $v06.h1
  vmacf $v06, $v18, $v06.h2
  vmudn $v10, $v28, $v07.h0
  vmadh $v09, $v27, $v07.h0
  vmadn $v10, $v26, $v07.h1
  vmadh $v09, $v25, $v07.h1
  vmadn $v10, $v24, $v07.h2
  vmadh $v09, $v23, $v07.h2
  vmadn $v10, $v22, $v07.h3
  vmadh $v09, $v21, $v07.h3
  luv $v08, 0, 16, $s4
  vor $v07, $v00, $v17
  LABEL_0003:
  beq $s3, $s2, LABEL_0004
  nop
  lpv $v03, 0, 8, $s3
  luv $v04, 0, 0, $s3
  addiu $s3, $s3, 16
  vmulf $v02, $v06, $v03.v
  vadd $v01, $v02, $v02.h1
  vadd $v02, $v02, $v01.h2
  vmulu $v02, $v04, $v02.h0
  vadd $v07, $v07, $v02.v
  j LABEL_0003
  nop
  LABEL_0004:
  vmulf $v08, $v08, $v07.v
  vmudl $v29, $v00, $v10.h3
  ori $at, $zero, %lo(NORM_SCALE_W)
  ldv $v07, 0, 0, $at
  vmadm $v29, $v14, $v10.h3
  vmadn $v03, $v00, $v09.h3
  vmadh $v02, $v14, $v09.h3
  ori $s3, $zero, %lo(LIGHT_DIR_COLOR)
  vch $v29, $v09, $v02
  ldv $v07, 8, 0, $at
  vcl $v29, $v10, $v03
  cfc2 $k1, $vcc
  vch $v29, $v09, $v09.h3
  vcl $v29, $v10, $v10.h3
  vmudl $v10, $v10, $v07.v
  vmadm $v09, $v09, $v07.v
  andi $s1, $k1, 1799
  vmadn $v10, $v00, $v00
  srl $k0, $s1, 5
  srl $s0, $k1, 4
  or $s1, $s1, $k0
  sdv $v09, 8, 16, $s5
  sdv $v10, 8, 24, $s5
  sdv $v09, 0, 16, $s6
  vrcph $v09.e3, $v09.e3
  sdv $v10, 0, 24, $s6
  vrcpl $v10.e3, $v10.e3
  lbu $k1, %lo(USE_FOG + 0)
  vrcph $v09.e3, $v09.e7
  andi $s0, $s0, 1799
  vrcpl $v10.e7, $v10.e7
  vrcph $v09.e7, $v00.e7
  srl $k0, $s0, 5
  or $s0, $s0, $k0
  cfc2 $k0, $vcc
  vaddc $v06, $v10, $v11.e2
  vadd $v05, $v09, $v11.e1
  andi $fp, $k0, 1799
  srl $sp, $k0, 4
  vmudn $v06, $v06, $v11.e0
  vmadh $v05, $v05, $v11.e0
  srl $t9, $fp, 5
  vor $v04, $v00, $v11.e3
  nor $fp, $fp, $t9
  andi $sp, $sp, 1799
  srl $t9, $sp, 5
  nor $sp, $sp, $t9
  vsub $v05, $v04, $v05.v
  beq $k1, $zero, LABEL_0005
  nop
  vmov $v08.e3, $v05.e2
  vmov $v08.e7, $v05.e6
  LABEL_0005:
  ssv $v09, 6, 32, $s6
  vmudl $v29, $v10, $v10.h3
  ssv $v10, 6, 34, $s6
  vmadm $v29, $v09, $v10.h3
  ssv $v10, 14, 34, $s5
  vmadn $v10, $v10, $v09.h3
  ssv $v09, 14, 32, $s5
  vmadh $v09, $v09, $v09.h3
  vmudn $v10, $v10, $v13.v
  suv $v08, 0, 8, $s6 ## Barrier: 0x2
  vmadh $v09, $v09, $v13.v
  suv $v08, 4, 8, $s5 ## Barrier: 0x2
  ldv $v10, 0, 24, $s4
  vadd $v09, $v09, $v12.v
  addiu $s4, $s4, 32
  slv $v10, 0, 12, $s6 ## Barrier: 0x2
  slv $v10, 4, 12, $s5 ## Barrier: 0x2
  sdv $v09, 0, 0, $s6 ## Barrier: 0x1
  sdv $v09, 8, 0, $s5 ## Barrier: 0x1
  sb $s1, 6($s6) ## Barrier: 0x1
  sb $s0, 6($s5) ## Barrier: 0x1
  sb $fp, 7($s6) ## Barrier: 0x1
  sb $sp, 7($s5) ## Barrier: 0x1
  addiu $s5, $s5, 72
  bne $s4, $s7, LABEL_0001
  addiu $s6, $s6, 72
  LABEL_0002:
  j RSPQ_Loop
  nop
copyClippingVertex:
  ldv $v04, 0, 24, $s0
  ldv $v03, 0, 16, $s0
  ldv $v02, 0, 8, $s0
  ldv $v01, 0, 0, $s0
  sdv $v02, 0, 8, $s5
  sdv $v04, 0, 24, $s5
  sdv $v03, 0, 16, $s5
  sdv $v01, 0, 0, $s5
  jr $ra
  addiu $s5, $s5, 36
emitClippedTri:
  sdv $v26, 0, 16, $s5
  suv $v28, 0, 8, $s5 ## Barrier: 0x2
  slv $v26, 8, 12, $s5 ## Barrier: 0x2
  sdv $v27, 0, 24, $s5
  sb $s1, 6($s5)
  jr $ra
  addiu $s5, $s5, 36
triToScreenSpace:
  ldv $v03, 0, 16, $s6
  ldv $v04, 0, 24, $s6
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v01, 0, 0, $at
  vrcph $v03.e3, $v03.e3
  vrcpl $v04.e3, $v04.e3
  vrcph $v03.e3, $v00.e3
  addiu $t0, $zero, 255
  ssv $v04, 6, 34, $s6
  vmudl $v29, $v04, $v04.e3
  ssv $v03, 6, 32, $s6
  vmadm $v29, $v03, $v04.e3
  vmadn $v04, $v04, $v03.e3
  ldv $v02, 0, 8, $at
  vmadh $v03, $v03, $v03.e3
  vmudn $v04, $v04, $v01.v
  vmadh $v03, $v03, $v01.v
  vadd $v03, $v03, $v02.v
  sdv $v03, 0, 0, $s6 ## Barrier: 0x4
  sh $t0, 6($s6) ## Barrier: 0x4
  jr $ra
  addiu $s6, $s6, 65500
T3DCmd_TriDraw:
  srl $a2, $a1, 16
  addiu $v1, $gp, %lo(RSPQ_DMEM_BUFFER)  -6
  jal RDPQ_Triangle_Send_Async ## Args: $a0, $a1, $a2
  ori $sp, $zero, %lo(RSPQ_Loop)
  bne $v1, $zero, LABEL_0006
  lhu $s7, %lo(RDPQ_TRI_BUFF_OFFSET + 0)
  xori $s7, $s7, 176
  j RSPQ_Loop
  sh $s7, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)
  LABEL_0006:
  lhu $a2, %lo(RSPQ_DMEM_BUFFER  -2)($gp)
  jal RDPQ_Triangle_Send_End
  lhu $a1, %lo(RSPQ_DMEM_BUFFER  -4)($gp)
  ori $s5, $zero, %lo(CLIP_BUFFER_TMP)
  ori $s6, $zero, %lo(CLIP_BUFFER_TMP)
  ori $s7, $zero, %lo(CLIP_BUFFER_RESULT)
  lhu $a0, %lo(RSPQ_DMEM_BUFFER  -6)($gp)
  addiu $t0, $zero, 108
  jal copyClippingVertex ## Args: $s5, $s0
  or $s0, $zero, $a0
  jal copyClippingVertex ## Args: $s5, $s0
  or $s0, $zero, $a1
  jal copyClippingVertex ## Args: $s5, $s0
  or $s0, $zero, $a2
  ori $s4, $zero, %lo(CLIPPING_PLANES)
  addiu $s3, $zero, 1
  LABEL_0007:
  addiu $s2, $s5, 65500
  xor $s7, $s7, $s6
  ldv $v26, 0, 16, $s2
  ldv $v27, 0, 24, $s2
  lbu $s1, 6($s2)
  luv $v28, 0, 8, $s2
  xor $s6, $s7, $s6
  or $s5, $zero, $s6
  llv $v26, 8, 12, $s2
  xor $s7, $s7, $s6
  addu $s0, $s7, $t0
  or $s2, $zero, $s7
  LABEL_0009:
  lbu $fp, 6($s2)
  ldv $v24, 0, 16, $s2
  llv $v24, 8, 12, $s2
  nor $k1, $zero, $fp
  xor $sp, $fp, $s1
  luv $v23, 0, 8, $s2
  and $sp, $sp, $s3
  ldv $v25, 0, 24, $s2
  beq $sp, $zero, LABEL_000B
  and $k1, $k1, $s3
  ori $at, $zero, %lo(NORM_SCALE_W)
  vsubc $v21, $v23, $v28.v
  vsubc $v20, $v25, $v27.v
  ldv $v22, 0, 8, $at
  lpv $v17, 0, 0, $s4
  vsub $v19, $v24, $v26.v
  ldv $v22, 8, 8, $at
  vxor $v18, $v00, $v00.e0
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM) ## Barrier: 0x8
  vmudl $v18, $v18, $v31.e7
  vmadm $v17, $v17, $v31.e7
  vmadn $v18, $v00, $v00
  sdv $v17, 0, 0, $at ## Barrier: 0x8
  ldv $v17, 8, 0, $at ## Barrier: 0x8
  sdv $v24, 0, 0, $at ## Barrier: 0x8
  vmudl $v18, $v18, $v22.v
  vmadm $v17, $v17, $v22.v
  vmadn $v18, $v00, $v00
  sdv $v25, 0, 8, $at ## Barrier: 0x8
  vor $v15, $v00, $v26
  vor $v16, $v00, $v27
  ldv $v15, 8, 0, $at ## Barrier: 0x8
  ldv $v16, 8, 8, $at ## Barrier: 0x8
  vmudl $v29, $v16, $v18.v
  vmadm $v29, $v15, $v18.v
  vmadn $v14, $v16, $v17.v
  vmadh $v13, $v15, $v17.v
  vaddc $v14, $v14, $v14.q1
  vadd $v13, $v13, $v13.q1
  vaddc $v14, $v14, $v14.h2
  vadd $v13, $v13, $v13.h2
  vsubc $v12, $v14, $v14.e4
  vsub $v11, $v13, $v13.e4
  vrcph $v11.e0, $v11.e0
  vrcpl $v12.e0, $v12.e0
  vrcph $v11.e0, $v00.e0
  vaddc $v12, $v12, $v12.v
  vadd $v11, $v11, $v11.v
  vmudl $v29, $v12, $v14.e0
  vmadm $v29, $v11, $v14.e0
  vmadn $v12, $v12, $v13.e0
  vmadh $v11, $v11, $v13.e0
  vlt $v29, $v11, $v30.e7
  vmrg $v12, $v12, $v22.e3
  vge $v29, $v11, $v00.e0
  vmrg $v12, $v12, $v00
  vmudl $v14, $v20, $v12.e0
  vmadm $v13, $v19, $v12.e0
  vmadn $v14, $v00, $v00
  vmadn $v27, $v27, $v30.e7
  vmadh $v26, $v26, $v30.e7
  vmudm $v21, $v21, $v12.e0
  vaddc $v28, $v28, $v21.v
  vmudl $v21, $v27, $v22.v
  vmadm $v20, $v26, $v22.v
  vmadn $v21, $v00, $v00
  vch $v29, $v20, $v26.e3
  vcl $v29, $v21, $v27.e3
  cfc2 $s1, $vcc
  andi $s1, $s1, 1799
  srl $k0, $s1, 5
  jal emitClippedTri ## Args: $s5, $s1, $v26, $v28
  or $s1, $s1, $k0
  LABEL_000B:
  vor $v26, $v00, $v24
  ori $ra, $zero, LABEL_000C
  vor $v28, $v00, $v23
  or $s1, $zero, $fp
  bne $k1, $zero, emitClippedTri
  vor $v27, $v00, $v25
  LABEL_000C:
  addiu $s2, $s2, 36
  bne $s2, $s0, LABEL_0009
  nop
  LABEL_000A:
  subu $t0, $s5, $s6
  beq $t0, $zero, RSPQ_Loop
  sll $s3, $s3, 1
  addiu $at, $zero, 32
  bne $s3, $at, LABEL_0007
  addiu $s4, $s4, 4
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)
  addu $s5, $s6, $t0
  addiu $s5, $s5, 65464
  jal triToScreenSpace ## Args: $s6
  sh $s6, %lo(0 + RSPQ_SCRATCH_MEM)($zero)
  jal triToScreenSpace ## Args: $s6
  addiu $s6, $s6, 72
  LABEL_000E:
  jal triToScreenSpace ## Args: $s6
  addiu $s6, $s6, 72
  ori $v1, $zero, %lo(RSPQ_SCRATCH_MEM)
  addiu $a1, $s6, 36
  ori $a0, $zero, %lo(CLIP_BUFFER_RESULT)
  ori $sp, $zero, %lo(CLIP_AFTER_EMIT)
  or $a2, $zero, $s6
  sh $a2, %lo(2 + RSPQ_SCRATCH_MEM)($zero)
  jal RDPQ_Triangle_Send_Async ## Args: $a0, $a1, $a2
  sh $a1, %lo(4 + RSPQ_SCRATCH_MEM)($zero)
  jal RDPQ_Triangle_Send_End
  nop
  CLIP_AFTER_EMIT:
  bne $s6, $s5, LABEL_000E
  nop
  LABEL_000F:
  j RSPQ_Loop
  nop
T3DCmd_SetScreenSize:
  andi $a0, $a0, 15
  srl $s7, $a3, 16
  sh $a3, %lo(12 + SCREEN_SCALE_OFFSET)($zero)
  subu $s6, $zero, $a0
  sw $a2, %lo(0 + SCREEN_SCALE_OFFSET)($zero)
  sh $s7, %lo(6 + NORM_SCALE_W)($zero)
  sw $a1, %lo(8 + SCREEN_SCALE_OFFSET)($zero)
  sh $s7, %lo(10 + NORM_SCALE_W)($zero)
  sb $s6, %lo(19 + CLIPPING_PLANES)($zero)
  sh $s7, %lo(8 + NORM_SCALE_W)($zero)
  sb $s6, %lo(15 + CLIPPING_PLANES)($zero)
  sh $s7, %lo(12 + NORM_SCALE_W)($zero)
  sb $a0, %lo(7 + CLIPPING_PLANES)($zero)
  sb $a0, %lo(3 + CLIPPING_PLANES)($zero)
  srl $s7, $s7, 1
  j RSPQ_Loop
  sh $s7, %lo(4 + SCREEN_SCALE_OFFSET)($zero)
T3DCmd_SetFogRange:
  sw $a1, %lo(2 + FOG_SCALE_OFFSET)($zero)
  j RSPQ_Loop
  sh $a0, %lo(0 + FOG_SCALE_OFFSET)($zero)
T3DCmd_SetFogState:
  j RSPQ_Loop
  sb $a0, %lo(USE_FOG)($zero)
T3DCmd_LightSet:
  beq $a2, $zero, LABEL_0010
  sw $a1, 0($a0)
  sw $a2, 12($a0)
  sw $a2, 8($a0)
  LABEL_0010:
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
  ldv $v01, 8, 0, $s3
  ldv $v02, 0, 8, $s3
  ldv $v02, 8, 8, $s3
  ldv $v10, 0, 8, $s4
  ldv $v05, 0, 32, $s3
  ldv $v04, 0, 24, $s3
  ldv $v05, 8, 32, $s3
  ldv $v09, 0, 0, $s4
  ldv $v10, 8, 24, $s4
  ldv $v09, 8, 16, $s4
  ldv $v03, 0, 16, $s3
  ldv $v03, 8, 16, $s3
  ldv $v04, 8, 24, $s3
  vmudl $v29, $v02, $v10.h0
  vmadm $v29, $v01, $v10.h0
  ldv $v11, 0, 32, $s4
  vmadn $v14, $v02, $v09.h0
  ldv $v08, 0, 56, $s3
  vmadh $v13, $v01, $v09.h0
  ldv $v06, 0, 40, $s3
  ldv $v06, 8, 40, $s3
  vmadl $v29, $v04, $v10.h1
  vmadm $v29, $v03, $v10.h1
  vmadn $v14, $v04, $v09.h1
  ldv $v07, 0, 48, $s3
  vmadh $v13, $v03, $v09.h1
  ldv $v07, 8, 48, $s3
  ldv $v08, 8, 56, $s3
  vmadl $v29, $v06, $v10.h2
  ldv $v12, 0, 40, $s4
  vmadm $v29, $v05, $v10.h2
  vmadn $v14, $v06, $v09.h2
  vmadh $v13, $v05, $v09.h2
  vmadl $v29, $v08, $v10.h3
  ldv $v12, 8, 56, $s4
  vmadm $v29, $v07, $v10.h3
  ldv $v11, 8, 48, $s4
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
  sdv $v10, 0, 8, $s2
  vmadm $v29, $v05, $v12.h2
  vmadn $v14, $v06, $v11.h2
  vmadh $v13, $v05, $v11.h2
  vmadl $v29, $v08, $v12.h3
  vmadm $v29, $v07, $v12.h3
  sdv $v10, 8, 24, $s2
  sdv $v09, 8, 16, $s2
  vmadn $v12, $v08, $v11.h3
  vmadh $v11, $v07, $v11.h3
  sdv $v12, 0, 40, $s2
  sdv $v11, 0, 32, $s2
  sdv $v11, 8, 48, $s2
  jr $ra
  sdv $v12, 8, 56, $s2
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
  andi $s6, $a1, 2
  lw $s5, %lo(MATRIX_STACK_PTR + 0)
  andi $s7, $a1, 1
  sra $a1, $a1, 16
  addu $s5, $s5, $a1
  bne $s6, $zero, RSPQ_Loop
  sw $s5, %lo(MATRIX_STACK_PTR)($zero)
  slt $at, $a1, $zero
  beq $at, $zero, LABEL_0012
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  or $a0, $zero, $s5
  LABEL_0012:
  lui $at, 0xFF
  addiu $t2, $zero, 12
  ori $t0, $zero, 63
  ori $at, $at, 0xFFFF
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  and $s0, $a0, $at
  beq $s7, $zero, LABEL_0013
  ori $t1, $zero, %lo(MATRIX_NORMAL)
  addiu $s0, $s5, 65472
  ori $t0, $zero, 63
  addiu $t2, $zero, 12
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(CLIP_BUFFER_RESULT)
  ori $s2, $zero, %lo(CLIP_BUFFER_TMP)
  ori $s3, $zero, %lo(CLIP_BUFFER_RESULT)
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  LABEL_0013:
  ori $t0, $zero, 63
  or $s0, $zero, $s5
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
  addiu $t2, $zero, 12
  ori $s4, $zero, %lo(MATRIX_PROJ)
  ori $at, $at, 0xFFFF
  ori $t0, $zero, 63
  j DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  and $s0, $a0, $at

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
#include "./rspq_triangle.inc"