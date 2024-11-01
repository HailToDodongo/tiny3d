## Auto-generated file, transpiled with RSPL
#define RDP_CMD_PRIM 0x3A00'0000
#define RDP_CMD_DEPTH 0x2E
#define RDP_CMD_RECT 0x36
#define RDP_CMD_TEX_RECT 0x24
#define PARTICLE_INPUT_SIZE 16
#define PARTICLE_MAX_COUNT 164
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
    .align 4
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0
    STATE_MEM_END:
  RSPQ_EndSavedState

  TEMP_STATE_MEM_START:
    .align 4
    PARTICLE_BUFF: .ds.b 2624
    .align 4
    RDP_BUFF: .ds.b 528
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

loadCurrentMVPMat:
  ori $at, $zero, %lo(MATRIX_MVP)
  ldv $v27, 8, 0, $at
  ldv $v28, 0, 8, $at
  ldv $v28, 8, 8, $at
  ldv $v25, 0, 16, $at
  ldv $v25, 8, 16, $at
  ldv $v26, 0, 24, $at
  ldv $v26, 8, 24, $at
  ldv $v23, 0, 32, $at
  ldv $v23, 8, 32, $at
  ldv $v24, 0, 40, $at
  ldv $v24, 8, 40, $at
  ldv $v21, 0, 48, $at
  ldv $v21, 8, 48, $at
  ldv $v22, 0, 56, $at
  ldv $v22, 8, 56, $at
  jr $ra
  ldv $v27, 0, 0, $at
TPXCmd_SyncT3D:
  ori $t0, $zero, 191
  ori $s4, $zero, %lo(MATRIX_PROJ)
  or $t2, $zero, $zero
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a0
  ori $t0, $zero, 15
  ori $s4, $zero, %lo(SCREEN_SCALE_OFFSET)
  or $t2, $zero, $zero
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a1
  j RSPQ_Loop
  sh $a2, %lo(6 + NORM_SCALE_W)($zero)
TPXCmd_DrawColor:
  or $t2, $zero, $zero
  ori $s4, $zero, %lo(PARTICLE_BUFF)
  andi $t0, $a0, 65535
  addiu $s4, $s4, 8
  addu $s7, $s4, $t0
  or $s0, $zero, $a1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1
  or $s6, $zero, $s4
  ori $s3, $zero, %lo(RDP_BUFF)
  addiu $s5, $s3, 528
  jal loadCurrentMVPMat ## Args: $v27, $v25, $v23, $v21
  ori $s4, $zero, %lo(RDP_BUFF)
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v18, 0, 8, $at
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)
  ldv $v18, 8, 8, $at
  ldv $v19, 0, 0, $at
  srl $t4, $s2, 12
  mtc2 $t4, $v15.e0
  mtc2 $s2, $v15.e1
  ldv $v19, 8, 0, $at
  ori $at, $zero, %lo(NORM_SCALE_W)
  mtc2 $t4, $v15.e4
  mtc2 $s2, $v15.e5
  srl $t4, $s1, 12
  vmudl $v20, $v00, $v31.e3
  ldv $v17, 0, 0, $at
  mtc2 $s1, $v16.e1
  vmadm $v19, $v19, $v31.e3
  ldv $v17, 8, 0, $at
  addiu $at, $zero, 4095
  mtc2 $s1, $v16.e5
  mtc2 $at, $v15.e3
  mtc2 $t4, $v16.e4
  vmadn $v20, $v00, $v00
  vand $v15, $v15, $v15.e3
  mtc2 $t4, $v16.e0
  vand $v16, $v16, $v15.e3
  jal DMAWaitIdle
  vxor $v13, $v00, $v00.e0
  lpv $v10, 0, 0, $s6
  vmudm $v10, $v10, $v31.e7
  vor $v14, $v00, $v10
  vmov $v10.e3, $v30.e7
  vmov $v10.e7, $v30.e7
  LABEL_0001:
  vmudn $v12, $v28, $v10.h0
  vmadh $v11, $v27, $v10.h0
  addiu $a0, $zero, 46
  vmadn $v12, $v26, $v10.h1
  vmadh $v11, $v25, $v10.h1
  vmadn $v12, $v24, $v10.h2
  vmadh $v11, $v23, $v10.h2
  vmadn $v12, $v22, $v10.h3
  vmadh $v11, $v21, $v10.h3
  addiu $at, $zero, 3
  vch $v29, $v11, $v11.h3
  vcl $v29, $v12, $v12.h3
  vmudl $v12, $v12, $v17.v
  vmadm $v11, $v11, $v17.v
  vmadn $v12, $v00, $v00
  ldv $v04, 0, 8, $s6
  vrcph $v08.e3, $v11.e3
  vrcpl $v09.e3, $v12.e3
  vrcph $v08.e3, $v11.e7
  vmov $v11.e3, $v14.e3
  addiu $s6, $s6, 16
  vrcpl $v09.e7, $v12.e7
  vmov $v12.e3, $v00.e3
  vrcph $v08.e7, $v00.e7
  vmov $v11.e7, $v14.e7
  vmov $v12.e7, $v00.e7
  cfc2 $t4, $vcc
  vmudl $v29, $v12, $v09.h3
  vmadm $v29, $v11, $v09.h3
  vmadn $v12, $v12, $v08.h3
  vmadh $v11, $v11, $v08.h3
  vmudl $v29, $v12, $v20.v
  vmadm $v29, $v11, $v20.v
  sb $a0, 0 + 8($s3)
  lui $t7, 0x3A00
  vmadn $v09, $v12, $v19.v
  vmadh $v08, $v11, $v19.v
  andi $t5, $t4, 16448
  andi $t1, $t4, 1028
  vaddc $v07, $v08, $v18.v
  vaddc $v06, $v07, $v11.h3
  slv $v04, 0, 4, $s3
  vsubc $v10, $v07, $v11.h3
  vlt $v06, $v06, $v15
  vge $v10, $v10, $v16
  sw $t7, ($s3)
  addiu $t2, $zero, 54
  mfc2 $fp, $v06.e1
  mfc2 $s2, $v10.e1
  mfc2 $s1, $v10.e5
  mfc2 $k1, $v10.e0
  andi $fp, $fp, 4095
  andi $s2, $s2, 4095
  sll $k1, $k1, 12
  or $s2, $s2, $k1
  mfc2 $k1, $v10.e4
  vlt $v05, $v10, $v06
  mfc2 $sp, $v06.e5
  andi $s1, $s1, 4095
  sll $k1, $k1, 12
  or $s1, $s1, $k1
  mfc2 $k1, $v06.e0
  vmov $v07.e3, $v00.e0
  cfc2 $t4, $vcc
  lpv $v10, 0, 0, $s6
  sll $k1, $k1, 12
  or $fp, $fp, $k1
  andi $sp, $sp, 4095
  mfc2 $k1, $v06.e4
  andi $a2, $t4, 3
  or $t1, $t1, $a2
  andi $a2, $t4, 48
  vmudm $v10, $v10, $v31.e7
  sw $fp, 0 + 16($s3) ## Barrier: 0x1
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  slv $v07, 4, 12, $s3
  or $t5, $t5, $a2
  sll $k1, $k1, 12
  vor $v14, $v00, $v10
  sw $s2, 4 + 16($s3)
  vmov $v10.e3, $v30.e7
  vmov $v10.e7, $v30.e7
  bne $t1, $at, LABEL_0003
  or $sp, $sp, $k1
  addiu $s3, $s3, 24
  LABEL_0003:
  vmov $v07.e7, $v00.e0
  addiu $at, $zero, 48
  bne $t5, $at, LABEL_0004
  nop
  slv $v04, 4, 4, $s3
  sw $s1, 4 + 16($s3)
  sb $a0, 0 + 8($s3)
  sw $sp, 0 + 16($s3) ## Barrier: 0x1
  slv $v07, 12, 12, $s3
  sb $t2, 0 + 16($s3) ## Barrier: 0x1
  sw $t7, ($s3)
  addiu $s3, $s3, 24
  LABEL_0004:
  sltu $at, $s3, $s5
  bne $at, $zero, LABEL_0005
  nop
  jal RDPQ_Send ## Args: $s4, $s3
  nop
  or $s3, $zero, $s4
  LABEL_0005:
  bne $s6, $s7, LABEL_0001
  nop
  LABEL_0002:
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