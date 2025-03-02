## Auto-generated file, transpiled with RSPL
#define SCREEN_WIDTH 320
#define SCREEN_LINE_SIZE_IN 1280
#define SCREEN_LINE_SIZE_OUT 640
#define SCREEN_SIZE_BYTES_IN 307200
#define SP_STATUS_SIG3 0x400
#define SP_STATUS_SIG4 0x800
#include <rsp_queue.inc>

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
    RSPQ_DefineCommand FX_ApplyTex, 12
  RSPQ_EndOverlayHeader

  RSPQ_EmptySavedState

.bss
  TEMP_STATE_MEM_START:
    .align 4
    IMG_ROW_A: .ds.b 1280
    .align 4
    IMG_ROW_B: .ds.b 1280
    .align 4
    TEX_CACHE: .ds.b 128
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

FX_ApplyTex:
  lui $at, 0xFF
  ori $at, $at, 0xFFFF
  and $a2, $a2, $at
  lui $at, 0xFF
  ori $s5, $zero, %lo(IMG_ROW_B)
  ori $s6, $zero, %lo(IMG_ROW_A)
  ori $at, $at, 0xFFFF
  or $s7, $zero, $zero
  and $a0, $a0, $at
  or $s0, $zero, $a0
  ori $s4, $zero, %lo(IMG_ROW_A)
  LABEL_FX_ApplyTex_0001:
  xor $s6, $s6, $s5
  addiu $t0, $zero, 1280
  addiu $t0, $t0, -1
  xor $s5, $s6, $s5
  xor $s6, $s6, $s5
  or $t2, $zero, $zero
  or $s0, $zero, $a0
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s4, $zero, $s6
  or $s2, $zero, $s4
  addiu $t0, $zero, 15
  addiu $s3, $s4, 1280
  lui $sp, 0xFF
  or $s1, $zero, $s4
  lw $s0, -1($s1)
  ori $sp, $sp, 0xFFF0
  LABEL_FX_ApplyTex_0003:
  mfc0 $k1, COP0_DMA_BUSY
  bne $k1, $zero, LABEL_FX_ApplyTex_0003
  ori $s4, $zero, %lo(TEX_CACHE)
  LABEL_FX_ApplyTex_0005:
  andi $k0, $s0, 15
  and $s0, $s0, $sp
  beq $s7, $s0, LABEL_FX_ApplyTex_0007
  mtc0 $s4, COP0_DMA_SPADDR
  mtc0 $s0, COP0_DMA_RAMADDR
  DMA_START:
  or $s7, $zero, $s0
  mtc0 $t0, COP0_DMA_READ
  mfc0 $k1, COP0_DMA_BUSY
  LABEL_FX_ApplyTex_0008:
  bne $k1, $zero, LABEL_FX_ApplyTex_0008
  mfc0 $k1, COP0_DMA_BUSY
  LABEL_FX_ApplyTex_0007:
  addiu $s1, $s1, 4
  lw $t9, %lo(TEX_CACHE + 8)
  sll $k0, $k0, 1
  lw $s0, -1($s1)
  srlv $t9, $t9, $k0
  andi $t9, $t9, 6
  lhu $t8, %lo(TEX_CACHE)($t9)
  addiu $s2, $s2, 2
  bne $s1, $s3, LABEL_FX_ApplyTex_0005
  sh $t8, -2($s2)
  LABEL_FX_ApplyTex_0006:
  mtc0 $s6, COP0_DMA_SPADDR ## Barrier: 0x1
  mtc0 $a2, COP0_DMA_RAMADDR ## Barrier: 0x1
  addiu $a2, $a2, 640
  addiu $t0, $zero, 639
  addiu $a0, $a0, 1280
  bne $a0, $a1, LABEL_FX_ApplyTex_0001
  mtc0 $t0, COP0_DMA_WRITE ## Barrier: 0x1
  LABEL_FX_ApplyTex_0002:
  j RSPQ_Loop
  nop

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