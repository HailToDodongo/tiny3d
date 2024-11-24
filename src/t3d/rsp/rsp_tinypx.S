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
  ori $s4, $zero, %lo(PARTICLE_BUFF)
  andi $t0, $a0, 65535
  addu $s7, $s4, $t0
  or $t2, $zero, $zero
  addiu $t0, $t0, -1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a1
  vmudl $v20, $v00, $v31.e3
  ori $at, $zero, %lo(MATRIX_MVP)
  ldv $v24, 0, 40, $at
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)
  ldv $v28, 0, 8, $at
  ldv $v27, 0, 0, $at
  or $s6, $zero, $s4
  ldv $v26, 0, 24, $at
  ldv $v22, 0, 56, $at
  ldv $v28, 8, 8, $at
  lui $t7, 0x3A00
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)
  mtc2 $s2, $v15.e1
  ldv $v21, 0, 48, $at
  ldv $v25, 0, 16, $at
  ldv $v23, 0, 32, $at
  srl $t4, $s2, 12
  ldv $v25, 8, 16, $at
  ldv $v24, 8, 40, $at
  ldv $v26, 8, 24, $at
  ldv $v22, 8, 56, $at
  ldv $v27, 8, 0, $at
  ldv $v21, 8, 48, $at
  ldv $v23, 8, 32, $at
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ori $s3, $zero, %lo(RDP_BUFF)
  mtc2 $s1, $v16.e1
  mtc2 $t4, $v15.e4
  ldv $v19, 0, 0, $at
  ldv $v19, 8, 0, $at
  ldv $v18, 0, 8, $at
  addiu $a0, $zero, 46
  ldv $v18, 8, 8, $at
  mtc2 $s2, $v15.e5
  vmadm $v19, $v19, $v31.e3
  ori $at, $zero, %lo(NORM_SCALE_W)
  ori $s4, $zero, %lo(RDP_BUFF)
  mtc2 $t4, $v15.e0
  vmadn $v20, $v00, $v00
  ldv $v17, 0, 0, $at
  ldv $v17, 8, 0, $at
  ori $s2, $zero, %lo(RDP_BUFF)
  addiu $at, $zero, 4095
  mtc2 $s1, $v16.e5
  mtc2 $at, $v15.e3
  ori $at, $zero, %lo(PARTICLE_SCALE)
  srl $t4, $s1, 12
  mtc2 $t4, $v16.e0
  mtc2 $t4, $v16.e4
  vand $v15, $v15, $v15.e3
  llv $v13, 0, 0, $at
  llv $v13, 8, 0, $at
  addiu $s5, $s3, 528
  addiu $s1, $s5, 24
  vand $v16, $v16, $v15.e3
  LABEL_TPXCmd_DrawColor_0001:
  sh $zero, 14($s2)
  sw $t7, 0($s2)
  sb $a0, 8($s2)
  addiu $s2, $s2, 24
  bne $s2, $s1, LABEL_TPXCmd_DrawColor_0001
  nop
  LABEL_TPXCmd_DrawColor_0002:
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
  LABEL_TPXCmd_DrawColor_0003:
  vmudl $v12, $v12, $v17.v
  vmadm $v11, $v11, $v17.v
  vmadn $v12, $v00, $v00
  vrcph $v07.e3, $v11.e3
  vrcpl $v08.e3, $v12.e3
  vrcph $v07.e3, $v11.e7
  vrcpl $v08.e7, $v12.e7
  vmov $v12.e3, $v00.e0
  vmov $v11.e3, $v30.e0
  addiu $at, $zero, 3
  vrcph $v07.e7, $v00.e7
  vmov $v11.e7, $v30.e0
  vmov $v12.e7, $v00.e0
  vmudl $v29, $v12, $v08.h3
  vmadm $v29, $v11, $v08.h3
  vmadn $v12, $v12, $v07.h3
  vmadh $v11, $v11, $v07.h3
  vmulf $v14, $v14, $v11.h3
  ldv $v03, 0, 8, $s6
  vmudl $v29, $v12, $v20.v
  addiu $t2, $zero, 54
  vmadm $v29, $v11, $v20.v
  vmadn $v08, $v12, $v19.v
  vmadh $v07, $v11, $v19.v
  vmadh $v06, $v18, $v09.v
  andi $t1, $t4, 1028
  vmadh $v05, $v09, $v14.v
  vsubc $v10, $v06, $v14.v
  addiu $s6, $s6, 16
  slv $v03, 0, 4, $s3
  vlt $v05, $v05, $v15
  ssv $v06, 4, 12, $s3
  mfc2 $sp, $v05.e5
  vge $v10, $v10, $v16
  mfc2 $fp, $v05.e1
  andi $t5, $t4, 16448
  mfc2 $k1, $v10.e0
  mfc2 $s2, $v10.e1
  andi $sp, $sp, 4095
  sll $k1, $k1, 12
  mfc2 $s1, $v10.e5
  vlt $v04, $v10, $v05
  andi $s2, $s2, 4095
  or $s2, $s2, $k1
  mfc2 $k1, $v10.e4
  cfc2 $t4, $vcc
  andi $s1, $s1, 4095
  sll $k1, $k1, 12
  or $s1, $s1, $k1
  mfc2 $k1, $v05.e0
  andi $a2, $t4, 3
  lpv $v10, 0, 0, $s6
  sll $k1, $k1, 12
  andi $fp, $fp, 4095
  or $fp, $fp, $k1
  mfc2 $k1, $v05.e4
  vmulf $v14, $v13, $v10.h3
  sw $fp, 0 + 16($s3) ## Barrier: 0x1
  or $t1, $t1, $a2
  vmudm $v10, $v10, $v31.e7
  sw $s2, 4 + 16($s3)
  vmov $v10.e3, $v30.e7
  andi $a2, $t4, 48
  sll $k1, $k1, 12
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  or $sp, $sp, $k1
  bne $t1, $at, LABEL_TPXCmd_DrawColor_0005
  or $t5, $t5, $a2
  addiu $s3, $s3, 24
  LABEL_TPXCmd_DrawColor_0005:
  vmov $v10.e7, $v30.e7
  addiu $at, $zero, 48
  bne $t5, $at, LABEL_TPXCmd_DrawColor_0006
  nop
  sw $sp, 0 + 16($s3) ## Barrier: 0x1
  ssv $v06, 12, 12, $s3
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  sw $s1, 4 + 16($s3)
  slv $v03, 4, 4, $s3
  addiu $s3, $s3, 24
  LABEL_TPXCmd_DrawColor_0006:
  vmudn $v12, $v28, $v10.h0
  vmadh $v11, $v27, $v10.h0
  vmadn $v12, $v26, $v10.h1
  sltu $at, $s3, $s5
  vmadh $v11, $v25, $v10.h1
  vmadn $v12, $v24, $v10.h2
  vmadh $v11, $v23, $v10.h2
  vmadn $v12, $v22, $v10.h3
  bne $at, $zero, LABEL_TPXCmd_DrawColor_0007
  vmadh $v11, $v21, $v10.h3
  jal RDPQ_Send ## Args: $s4, $s3
  nop
  or $s3, $zero, $s4
  LABEL_TPXCmd_DrawColor_0007:
  vch $v29, $v11, $v11.h3
  vcl $v29, $v12, $v12.h3
  bne $s6, $s7, LABEL_TPXCmd_DrawColor_0003
  cfc2 $t4, $vcc
  LABEL_TPXCmd_DrawColor_0004:
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