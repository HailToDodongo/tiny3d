## Auto-generated file, transpiled with RSPL
#define RDP_CMD_PRIM 0x3A00'0000
#define RDP_CMD_DEPTH 0x2E
#define RDP_CMD_RECT 0x36
#define RDP_CMD_TEX_RECT 0x24
#define PARTICLE_INPUT_SIZE 16
#define PARTICLE_MAX_COUNT 344
#define RDP_FLUSH_SIZE 528
#define RDP_POS_MASK 0b1111'1111'1111
#include <rsp_queue.inc>
#include <rsp_rdpq.inc>
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
    RSPQ_DefineCommand TPXCmd_SyncT3D, 12
    RSPQ_DefineCommand TPXCmd_DrawColor, 8
    RSPQ_DefineCommand T3DCmd_MatrixStack, 8
    RSPQ_DefineCommand TPXCmd_SetDMEM, 8
    RSPQ_DefineCommand TPXCmd_DrawTextured, 8
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 3
    MATRIX_PROJ: .ds.b 64
    .align 3
    MATRIX_MVP: .ds.b 64
    .align 4
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0
    .align 2
    MATRIX_STACK_PTR: .word 0
    .align 1
    PARTICLE_SCALE: .half 32767, 32767
    .align 1
    TEX_OFFSET: .half 0
    .align 1
    TILE_COUNT: .half 0
    .align 1
    BASE_SIZE: .half 128
    STATE_MEM_END:
  RSPQ_EndSavedState

.bss
  TEMP_STATE_MEM_START:
    .align 4
    PARTICLE_BUFF: .ds.b 2752
    .align 4
    RDP_BUFF: .ds.b 528
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

TPXCmd_SyncT3D:
  ori $t0, $zero, 127
  or $t2, $zero, $zero
  or $s0, $zero, $a0
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(MATRIX_PROJ)
  ori $t0, $zero, 63
  addiu $a0, $a0, 128
  or $s0, $zero, $a0
  or $t2, $zero, $zero
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(RDP_BUFF)
  ori $t0, $zero, 63
  addiu $t2, $zero, -32768
  lw $s0, %lo(MATRIX_STACK_PTR + 0)
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s4, $zero, %lo(RDP_BUFF)
  ori $t0, $zero, 15
  sh $a2, %lo(6 + NORM_SCALE_W)($zero)
  ori $s4, $zero, %lo(SCREEN_SCALE_OFFSET)
  or $s0, $zero, $a1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t2, $zero, 12
  j RSPQ_Loop
  nop
TPXCmd_DrawColor:
  or $s0, $zero, $a1
  ori $s4, $zero, %lo(PARTICLE_BUFF)
  andi $t0, $a0, 65535
  addu $s7, $s4, $t0
  or $t2, $zero, $zero
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  ori $at, $zero, %lo(MATRIX_MVP)
  ldv $v26, 0, 24, $at
  ldv $v26, 8, 24, $at
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)
  ldv $v24, 0, 40, $at
  ldv $v28, 0, 8, $at
  srl $t4, $s2, 12
  ldv $v21, 0, 48, $at
  ldv $v22, 0, 56, $at
  ldv $v24, 8, 40, $at
  vmudl $v20, $v00, $v31.e3
  ldv $v23, 0, 32, $at
  ldv $v27, 0, 0, $at
  ldv $v22, 8, 56, $at
  ldv $v21, 8, 48, $at
  ldv $v25, 0, 16, $at
  or $s6, $zero, $s4
  ldv $v25, 8, 16, $at
  mtc2 $s2, $v15.e1
  ldv $v28, 8, 8, $at
  ldv $v23, 8, 32, $at
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)
  ldv $v27, 8, 0, $at
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  lui $t7, 0x3A00
  ori $s4, $zero, %lo(RDP_BUFF)
  mtc2 $t4, $v15.e0
  mtc2 $t4, $v15.e4
  ldv $v19, 0, 0, $at
  ldv $v18, 0, 8, $at
  srl $t4, $s1, 12
  ldv $v18, 8, 8, $at
  mtc2 $s2, $v15.e5
  ldv $v19, 8, 0, $at
  ori $at, $zero, %lo(NORM_SCALE_W)
  ldv $v17, 0, 0, $at
  mtc2 $s1, $v16.e1
  ldv $v17, 8, 0, $at
  addiu $at, $zero, 4095
  vmadm $v19, $v19, $v31.e3
  ori $s3, $zero, %lo(RDP_BUFF)
  vmadn $v20, $v00, $v00
  mtc2 $at, $v15.e3
  mtc2 $t4, $v16.e0
  ori $at, $zero, %lo(PARTICLE_SCALE)
  ori $s2, $zero, %lo(RDP_BUFF)
  addiu $s5, $s3, 528
  mtc2 $t4, $v16.e4
  vand $v15, $v15, $v15.e3
  mtc2 $s1, $v16.e5
  llv $v13, 0, 0, $at
  llv $v13, 8, 0, $at
  addiu $a0, $zero, 46
  addiu $s1, $s5, 24
  vand $v16, $v16, $v15.e3
  LABEL_0001:
  sw $t7, 0($s2)
  sb $a0, 8($s2)
  sh $zero, 14($s2)
  addiu $s2, $s2, 24
  bne $s2, $s1, LABEL_0001
  nop
  LABEL_0002:
  jal DMAWaitIdle
  nop
  lpv $v10, 0, 0, $s6
  vmulf $v14, $v13, $v10.h3
  vmudm $v10, $v10, $v31.e7
  vmov $v10.e3, $v30.e7
  vmov $v10.e7, $v30.e7
  vxor $v09, $v00, $v30.e7
  vmudn $v12, $v28, $v10.h0
  vmadh $v11, $v27, $v10.h0
  vmadn $v12, $v26, $v10.h1
  vmadh $v11, $v25, $v10.h1
  vmadn $v12, $v24, $v10.h2
  vmadh $v11, $v23, $v10.h2
  vmadn $v12, $v22, $v10.h3
  vmadh $v11, $v21, $v10.h3
  vch $v29, $v11, $v11.h3
  vcl $v29, $v12, $v12.h3
  cfc2 $t4, $vcc
  LABEL_0003:
  vmudl $v12, $v12, $v17.v
  ori $at, $zero, %lo(BASE_SIZE)
  vmadm $v11, $v11, $v17.v
  ldv $v03, 0, 8, $s6
  vmadn $v12, $v00, $v00
  vrcph $v07.e3, $v11.e3
  andi $t5, $t4, 16448
  vrcpl $v08.e3, $v12.e3
  addiu $s6, $s6, 16
  vrcph $v07.e3, $v11.e7
  vrcpl $v08.e7, $v12.e7
  vrcph $v07.e7, $v00.e7
  lsv $v11, 6, 0, $at
  vmov $v12.e3, $v00.e0
  lsv $v11, 14, 0, $at
  vmov $v12.e7, $v00.e0
  addiu $at, $zero, 3
  vmudl $v29, $v12, $v08.h3
  vmadm $v29, $v11, $v08.h3
  vmadn $v12, $v12, $v07.h3
  addiu $t2, $zero, 54
  vmadh $v11, $v11, $v07.h3
  vmulf $v14, $v14, $v11.h3
  vmudl $v29, $v12, $v20.v
  vmadm $v29, $v11, $v20.v
  slv $v03, 0, 4, $s3
  vmadn $v08, $v12, $v19.v
  andi $t1, $t4, 1028
  vmadh $v07, $v11, $v19.v
  vmadh $v06, $v18, $v09.v
  vmadh $v05, $v09, $v14.v
  vsubc $v10, $v06, $v14.v
  vlt $v05, $v05, $v15
  vge $v10, $v10, $v16
  ssv $v06, 4, 12, $s3
  mfc2 $sp, $v05.e5
  mfc2 $k1, $v10.e0
  mfc2 $s2, $v10.e1
  andi $sp, $sp, 4095
  sll $k1, $k1, 12
  mfc2 $fp, $v05.e1
  andi $s2, $s2, 4095
  or $s2, $s2, $k1
  mfc2 $k1, $v10.e4
  mfc2 $s1, $v10.e5
  andi $fp, $fp, 4095
  sll $k1, $k1, 12
  andi $s1, $s1, 4095
  sw $s2, 4 + 16($s3)
  or $s1, $s1, $k1
  vlt $v04, $v10, $v05
  mfc2 $k1, $v05.e0
  cfc2 $t4, $vcc
  sll $k1, $k1, 12
  or $fp, $fp, $k1
  andi $a2, $t4, 3
  lpv $v10, 0, 0, $s6
  sw $fp, 0 + 16($s3) ## Barrier: 0x1
  or $t1, $t1, $a2
  mfc2 $k1, $v05.e4
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  andi $a2, $t4, 48
  vmulf $v14, $v13, $v10.h3
  sll $k1, $k1, 12
  vmudm $v10, $v10, $v31.e7
  or $sp, $sp, $k1
  vmov $v10.e3, $v30.e7
  bne $t1, $at, LABEL_0005
  or $t5, $t5, $a2
  addiu $s3, $s3, 24
  LABEL_0005:
  addiu $at, $zero, 48
  vmov $v10.e7, $v30.e7
  bne $t5, $at, LABEL_0006
  nop
  slv $v03, 4, 4, $s3
  sw $sp, 0 + 16($s3) ## Barrier: 0x1
  ssv $v06, 12, 12, $s3
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  sw $s1, 4 + 16($s3)
  addiu $s3, $s3, 24
  LABEL_0006:
  vmudn $v12, $v28, $v10.h0
  vmadh $v11, $v27, $v10.h0
  vmadn $v12, $v26, $v10.h1
  vmadh $v11, $v25, $v10.h1
  sltu $at, $s3, $s5
  vmadn $v12, $v24, $v10.h2
  vmadh $v11, $v23, $v10.h2
  vmadn $v12, $v22, $v10.h3
  bne $at, $zero, LABEL_0007
  vmadh $v11, $v21, $v10.h3
  jal RDPQ_Send ## Args: $s4, $s3
  nop
  or $s3, $zero, $s4
  LABEL_0007:
  vch $v29, $v11, $v11.h3
  vcl $v29, $v12, $v12.h3
  bne $s6, $s7, LABEL_0003
  cfc2 $t4, $vcc
  LABEL_0004:
  j RDPQ_Send
  ori $ra, $zero, %lo(RSPQ_Loop)
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
T3DCmd_MatrixStack:
  andi $s7, $a0, 1
  andi $s6, $a0, 2
  lh $a0, %lo(RSPQ_DMEM_BUFFER  -7)($gp)
  lw $s5, %lo(MATRIX_STACK_PTR + 0)
  addu $s5, $s5, $a0
  bne $s6, $zero, RSPQ_Loop
  sw $s5, %lo(MATRIX_STACK_PTR)($zero)
  bgez $a0, LABEL_T3DCmd_MatrixStack_0009
  ori $s4, $zero, %lo(RDP_BUFF)
  or $a1, $zero, $s5
  LABEL_T3DCmd_MatrixStack_0009:
  lui $at, 0xFF
  ori $t0, $zero, 63
  ori $at, $at, 0xFFFF
  addiu $t2, $zero, 12
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  and $s0, $a1, $at
  beq $s7, $zero, LABEL_T3DCmd_MatrixStack_000A
  nop
  ori $t0, $zero, 63
  ori $s4, $zero, %lo(PARTICLE_BUFF)
  addiu $s0, $s5, 65472
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t2, $zero, 12
  ori $s4, $zero, %lo(RDP_BUFF)
  ori $s3, $zero, %lo(PARTICLE_BUFF)
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s2, $zero, %lo(RDP_BUFF)
  LABEL_T3DCmd_MatrixStack_000A:
  ori $t0, $zero, 63
  addiu $t2, $zero, -32756
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $s5
  ori $s2, $zero, %lo(MATRIX_MVP)
  ori $s4, $zero, %lo(RDP_BUFF)
  jal mulMat4Mat4 ## Args: $s2, $s3, $s4
  ori $s3, $zero, %lo(MATRIX_PROJ)
  j RSPQ_Loop
  nop
TPXCmd_SetDMEM:
  andi $s7, $a0, 32768
  bne $s7, $zero, RSPQ_Loop
  sh $a1, ($a0)
  j RSPQ_Loop
  sw $a1, ($a0)
TPXCmd_DrawTextured:
  andi $t0, $a0, 65535
  or $s0, $zero, $a1
  ori $s4, $zero, %lo(PARTICLE_BUFF)
  addu $s7, $s4, $t0
  addiu $t0, $t0, -1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $t2, $zero, $zero
  ori $at, $zero, %lo(MATRIX_MVP)
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)
  ldv $v21, 0, 48, $at
  ldv $v27, 0, 0, $at
  ldv $v24, 0, 40, $at
  ldv $v21, 8, 48, $at
  ldv $v25, 0, 16, $at
  ldv $v22, 0, 56, $at
  ldv $v26, 0, 24, $at
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)
  ldv $v28, 0, 8, $at
  ldv $v25, 8, 16, $at
  ldv $v28, 8, 8, $at
  srl $t4, $s2, 12
  ldv $v27, 8, 0, $at
  or $s6, $zero, $s4
  ldv $v23, 0, 32, $at
  ldv $v22, 8, 56, $at
  ldv $v24, 8, 40, $at
  ldv $v23, 8, 32, $at
  ldv $v26, 8, 24, $at
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v19, 0, 0, $at
  lui $t7, 0x3A00
  ori $s4, $zero, %lo(RDP_BUFF)
  mtc2 $s2, $v15.e1
  mtc2 $s1, $v16.e1
  ldv $v19, 8, 0, $at
  ldv $v18, 0, 8, $at
  vmudl $v20, $v00, $v31.e3
  ldv $v18, 8, 8, $at
  ori $at, $zero, %lo(NORM_SCALE_W)
  ldv $v17, 0, 0, $at
  mtc2 $s1, $v16.e5
  ldv $v17, 8, 0, $at
  addiu $at, $zero, 4095
  ori $s3, $zero, %lo(RDP_BUFF)
  mtc2 $t4, $v15.e0
  mtc2 $t4, $v15.e4
  addiu $s5, $s3, 480
  srl $t4, $s1, 12
  vmadm $v19, $v19, $v31.e3
  mtc2 $s2, $v15.e5
  mtc2 $at, $v15.e3
  ori $s2, $zero, %lo(RDP_BUFF)
  ori $at, $zero, %lo(PARTICLE_SCALE)
  mtc2 $t4, $v16.e4
  vmadn $v20, $v00, $v00
  mtc2 $t4, $v16.e0
  vand $v15, $v15, $v15.e3
  llv $v13, 0, 0, $at
  llv $v13, 8, 0, $at
  addiu $s1, $s5, 32
  addiu $a0, $zero, 46
  vand $v16, $v16, $v15.e3
  LABEL_000C:
  sw $t7, 0($s2)
  sh $zero, 14($s2)
  sb $a0, 8($s2)
  addiu $s2, $s2, 32
  bne $s2, $s1, LABEL_000C
  nop
  LABEL_000D:
  lhu $s1, %lo(TILE_COUNT + 0)
  vxor $v12, $v00, $v00.e0
  vmov $v12.e1, $v30.e7
  vxor $v11, $v00, $v00.e0
  vmov $v12.e5, $v30.e7
  addu $fp, $s1, $s1
  mtc2 $fp, $v12.e3
  mtc2 $s1, $v12.e2
  addiu $fp, $fp, 65535
  vmov $v12.e7, $v31.e7
  mtc2 $fp, $v12.e0
  mtc2 $fp, $v12.e4
  lh $s2, %lo(TEX_OFFSET + 0)
  vmudn $v12, $v12, $v12.e7
  vsubc $v29, $v12, $v30.e7
  vmov $v12.e3, $v29.e3
  jal DMAWaitIdle
  vor $v11, $v00, $v29.e2
  lpv $v08, 0, 0, $s6
  vmulf $v14, $v13, $v08.h3
  vmudm $v08, $v08, $v31.e7
  vmov $v08.e7, $v30.e7
  vmov $v08.e3, $v30.e7
  vxor $v07, $v00, $v30.e7
  vmudn $v10, $v28, $v08.h0
  vmadh $v09, $v27, $v08.h0
  vmadn $v10, $v26, $v08.h1
  vmadh $v09, $v25, $v08.h1
  vmadn $v10, $v24, $v08.h2
  vmadh $v09, $v23, $v08.h2
  vmadn $v10, $v22, $v08.h3
  vmadh $v09, $v21, $v08.h3
  vch $v29, $v09, $v09.h3
  vcl $v29, $v10, $v10.h3
  cfc2 $t4, $vcc
  LABEL_000E:
  vmudl $v10, $v10, $v17.v
  vmadm $v09, $v09, $v17.v
  ori $at, $zero, %lo(BASE_SIZE)
  vmadn $v10, $v00, $v00
  vrcph $v05.e3, $v09.e3
  andi $t1, $t4, 1028
  vrcpl $v06.e3, $v10.e3
  vrcph $v05.e3, $v09.e7
  vrcpl $v06.e7, $v10.e7
  lsv $v09, 6, 0, $at
  lsv $v09, 14, 0, $at
  vmov $v10.e3, $v00.e0
  andi $t5, $t4, 16448
  vrcph $v05.e7, $v00.e7
  vmov $v10.e7, $v00.e0
  vmudl $v29, $v10, $v06.h3
  lb $a2, 11($s6)
  vmadm $v29, $v09, $v06.h3
  vmadn $v10, $v10, $v05.h3
  vmadh $v09, $v09, $v05.h3
  vmulf $v14, $v14, $v09.h3
  vmudl $v29, $v10, $v20.v
  vmadm $v29, $v09, $v20.v
  vmadn $v04, $v10, $v19.v
  vmadh $v03, $v09, $v19.v
  vmadh $v02, $v18, $v07.v
  lb $t4, 15($s6)
  vmadh $v04, $v07, $v14.v
  vsubc $v08, $v02, $v14.v
  addu $t4, $t4, $s2
  addu $a2, $a2, $s2
  vmudn $v14, $v14, $v30.e6
  vrcp $v06.e0, $v14.e0
  vrcph $v05.e0, $v14.e0
  vrcp $v06.e1, $v14.e1
  vrcph $v05.e1, $v14.e1
  vrcp $v06.e4, $v14.e4
  vrcph $v05.e4, $v14.e4
  vrcp $v06.e5, $v14.e5
  vrcph $v05.e5, $v14.e5
  sll $a2, $a2, 3
  vor $v03, $v00, $v05
  vmudh $v01, $v08, $v05.v
  vmudm $v01, $v01, $v31.e6
  vxor $v05, $v00, $v00.e0
  slv $v03, 0, 28, $s3
  sll $t4, $t4, 3
  mtc2 $t4, $v05.e4
  mtc2 $a2, $v05.e0
  vlt $v01, $v01, $v00.e0
  addiu $t2, $zero, 36
  vsubc $v06, $v00, $v01.v
  vand $v05, $v05, $v12.e3
  vge $v29, $v11, $v05.h0
  vmrg $v01, $v06, $v01
  vlt $v04, $v04, $v15
  vge $v08, $v08, $v16
  addiu $at, $zero, 3
  vaddc $v01, $v01, $v05.v
  ssv $v02, 4, 12, $s3
  mfc2 $k1, $v04.e5
  mfc2 $s1, $v08.e1
  andi $k1, $k1, 4095
  vsubc $v06, $v12, $v01.v
  mfc2 $k0, $v08.e0
  mfc2 $fp, $v08.e5
  andi $s1, $s1, 4095
  sll $k0, $k0, 12
  vaddc $v06, $v06, $v12.e2
  or $s1, $s1, $k0
  mfc2 $k0, $v08.e4
  vge $v29, $v11, $v05.h0
  mfc2 $sp, $v04.e1
  ldv $v05, 0, 8, $s6
  vmrg $v01, $v01, $v06
  andi $fp, $fp, 4095
  sll $k0, $k0, 12
  sw $s1, 4 + 16($s3)
  vlt $v06, $v08, $v04
  cfc2 $t4, $vcc
  addiu $s6, $s6, 16
  or $fp, $fp, $k0
  mfc2 $k0, $v04.e0
  andi $sp, $sp, 4095
  andi $a2, $t4, 3
  slv $v01, 0, 24, $s3
  lpv $v08, 0, 0, $s6
  sll $k0, $k0, 12
  or $sp, $sp, $k0
  mfc2 $k0, $v04.e4
  sw $sp, 0 + 16($s3) ## Barrier: 0x1
  or $t1, $t1, $a2
  slv $v05, 0, 4, $s3
  sll $k0, $k0, 12
  andi $a2, $t4, 48
  vmulf $v14, $v13, $v08.h3
  vmudm $v08, $v08, $v31.e7
  or $t5, $t5, $a2
  vmov $v08.e3, $v30.e7
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  bne $t1, $at, LABEL_0010
  or $k1, $k1, $k0
  addiu $s3, $s3, 32
  LABEL_0010:
  vmov $v08.e7, $v30.e7
  addiu $at, $zero, 48
  bne $t5, $at, LABEL_0011
  nop
  sw $fp, 4 + 16($s3)
  slv $v05, 4, 4, $s3
  sw $k1, 0 + 16($s3) ## Barrier: 0x1
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  slv $v03, 8, 28, $s3
  slv $v01, 8, 24, $s3
  ssv $v02, 12, 12, $s3
  addiu $s3, $s3, 32
  LABEL_0011:
  vmudn $v10, $v28, $v08.h0
  vmadh $v09, $v27, $v08.h0
  vmadn $v10, $v26, $v08.h1
  vmadh $v09, $v25, $v08.h1
  sltu $at, $s3, $s5
  vmadn $v10, $v24, $v08.h2
  vmadh $v09, $v23, $v08.h2
  vmadn $v10, $v22, $v08.h3
  bne $at, $zero, LABEL_0012
  vmadh $v09, $v21, $v08.h3
  jal RDPQ_Send ## Args: $s4, $s3
  nop
  or $s3, $zero, $s4
  LABEL_0012:
  vch $v29, $v09, $v09.h3
  vcl $v29, $v10, $v10.h3
  bne $s6, $s7, LABEL_000E
  cfc2 $t4, $vcc
  LABEL_000F:
  j RDPQ_Send
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