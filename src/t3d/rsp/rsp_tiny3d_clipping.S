## Auto-generated file, transpiled with RSPL
#define OVERLAY_CLIPPING 1
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
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0, 0, 0, 0, 65535
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

clipTriangle:
  addiu $t0, $zero, 108
  ori $t3, $zero, %lo(CLIP_BUFFER_TMP)
  or $s0, $zero, $a0
  ori $t2, $zero, %lo(CLIP_BUFFER_TMP)
  bgezal $zero, copyClippingVertex ## Args: $t3, $s0
  ori $t1, $zero, %lo(CLIP_BUFFER_RESULT)
  bgezal $zero, copyClippingVertex ## Args: $t3, $s0
  or $s0, $zero, $a1
  bgezal $zero, copyClippingVertex ## Args: $t3, $s0
  or $s0, $zero, $a2
  ori $t4, $zero, %lo(CLIPPING_PLANES)
  addiu $t5, $zero, 1
  LABEL_0001:
  addiu $t6, $t3, 65500
  luv $v01, 0, 8, $t6
  xor $t1, $t1, $t2
  ldv $v03, 0, 24, $t6
  xor $t2, $t1, $t2
  or $t3, $zero, $t2
  ldv $v02, 0, 16, $t6
  llv $v02, 8, 12, $t6
  xor $t1, $t1, $t2
  addu $t8, $t1, $t0
  lbu $t7, 6($t6)
  or $t6, $zero, $t1
  LABEL_0003:
  lbu $t9, 6($t6)
  ldv $v04, 0, 16, $t6
  luv $v06, 0, 8, $t6
  xor $k0, $t9, $t7
  and $k0, $k0, $t5
  ldv $v05, 0, 24, $t6
  nor $k1, $zero, $t9
  llv $v04, 8, 12, $t6
  beq $k0, $zero, LABEL_0005
  and $k1, $k1, $t5
  lpv $v11, 0, 0, $t4
  vsubc $v08, $v06, $v01.v
  ori $at, $zero, %lo(NORM_SCALE_W)
  vsubc $v10, $v05, $v03.v
  vsub $v09, $v04, $v02.v
  vmudl $v12, $v00, $v31.e7
  ldv $v07, 0, 8, $at
  vmadm $v11, $v11, $v31.e7
  ldv $v07, 8, 8, $at
  ori $at, $zero, %lo(RSPQ_SCRATCH_MEM) ## Barrier: 0x1
  sdv $v11, 0, 0, $at ## Barrier: 0x1
  vmadn $v12, $v00, $v00
  ldv $v11, 8, 0, $at ## Barrier: 0x1
  vmudl $v12, $v12, $v07.v
  sdv $v04, 0, 0, $at ## Barrier: 0x1
  vmadm $v11, $v11, $v07.v
  vmadn $v12, $v00, $v00
  sdv $v05, 0, 8, $at ## Barrier: 0x1
  vor $v13, $v00, $v02
  vor $v14, $v00, $v03
  ldv $v13, 8, 0, $at ## Barrier: 0x1
  ldv $v14, 8, 8, $at ## Barrier: 0x1
  vmudl $v29, $v14, $v12.v
  vmadm $v29, $v13, $v12.v
  vmadn $v16, $v14, $v11.v
  vmadh $v15, $v13, $v11.v
  vaddc $v16, $v16, $v16.q1
  vadd $v15, $v15, $v15.q1
  vaddc $v16, $v16, $v16.h2
  vadd $v15, $v15, $v15.h2
  vsubc $v18, $v16, $v16.e4
  vsub $v17, $v15, $v15.e4
  vrcph $v17.e0, $v17.e0
  vrcpl $v18.e0, $v18.e0
  vrcph $v17.e0, $v00.e0
  vaddc $v18, $v18, $v18.v
  vadd $v17, $v17, $v17.v
  vmudl $v29, $v18, $v16.e0
  vmadm $v29, $v17, $v16.e0
  vmadn $v18, $v18, $v15.e0
  vmadh $v17, $v17, $v15.e0
  vlt $v29, $v17, $v30.e7
  vmrg $v18, $v18, $v07.e3
  vge $v29, $v17, $v00.e0
  vmrg $v18, $v18, $v00
  vmudl $v16, $v10, $v18.e0
  vmadm $v15, $v09, $v18.e0
  vmadn $v16, $v00, $v00
  vmadn $v03, $v03, $v30.e7
  vmadh $v02, $v02, $v30.e7
  vmudm $v08, $v08, $v18.e0
  vaddc $v01, $v01, $v08.v
  vmudl $v09, $v03, $v07.v
  vmadm $v08, $v02, $v07.v
  vmadn $v09, $v00, $v00
  vch $v29, $v08, $v02.e3
  vcl $v29, $v09, $v03.e3
  cfc2 $t7, $vcc
  andi $t7, $t7, 1799
  srl $sp, $t7, 5
  bgezal $zero, emitClippedTri ## Args: $t3, $t7, $v02, $v01
  or $t7, $t7, $sp
  LABEL_0005:
  or $t7, $zero, $t9
  vor $v03, $v00, $v05
  vor $v02, $v00, $v04
  beq $k1, $zero, LABEL_0006
  vor $v01, $v00, $v06
  bgezal $zero, emitClippedTri ## Args: $t3, $t7, $v02, $v01
  nop
  LABEL_0006:
  addiu $t6, $t6, 36
  bne $t6, $t8, LABEL_0003
  nop
  LABEL_0004:
  subu $t0, $t3, $t2
  beq $t0, $zero, CLIP_RETURN
  sll $t5, $t5, 1
  addiu $at, $zero, 32
  bne $t5, $at, LABEL_0001
  addiu $t4, $t4, 4
  CLIP_END:
  ori $s6, $zero, %lo(CLIP_BUFFER_RESULT)
  sh $s6, %lo(0 + RSPQ_SCRATCH_MEM)($zero)
  addu $s5, $s6, $t0
  bgezal $zero, triToScreenSpace ## Args: $s6
  addiu $s5, $s5, 65464
  bgezal $zero, triToScreenSpace ## Args: $s6
  addiu $s6, $s6, 72
  LABEL_0008:
  bgezal $zero, triToScreenSpace ## Args: $s6
  addiu $s6, $s6, 72
  ori $sp, $zero, %lo(CLIP_AFTER_EMIT)
  addiu $a1, $s6, 36
  or $a2, $zero, $s6
  sh $a1, %lo(4 + RSPQ_SCRATCH_MEM)($zero)
  sh $a2, %lo(2 + RSPQ_SCRATCH_MEM)($zero)
  ori $a0, $zero, %lo(CLIP_BUFFER_RESULT)
  jal RDPQ_Triangle_Send_Async ## Args: $a0, $a1, $a2
  ori $v1, $zero, %lo(RSPQ_SCRATCH_MEM)
  jal RDPQ_Triangle_Send_End
  nop
  CLIP_AFTER_EMIT:
  bne $s6, $s5, LABEL_0008
  nop
  CLIP_RETURN:
  lhu $t6, %lo(CLIP_RA + 0)
  j $t6
  addiu $t6, $t6, 0
copyClippingVertex:
  ldv $v04, 0, 24, $s0
  ldv $v01, 0, 0, $s0
  ldv $v02, 0, 8, $s0
  ldv $v03, 0, 16, $s0
  sdv $v04, 0, 24, $t3
  sdv $v03, 0, 16, $t3
  sdv $v01, 0, 0, $t3
  sdv $v02, 0, 8, $t3
  jr $ra
  addiu $t3, $t3, 36
emitClippedTri:
  suv $v01, 0, 8, $t3 ## Barrier: 0x2
  sdv $v02, 0, 16, $t3
  sdv $v03, 0, 24, $t3
  slv $v02, 8, 12, $t3 ## Barrier: 0x2
  sb $t7, 6($t3)
  jr $ra
  addiu $t3, $t3, 36
triToScreenSpace:
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v01, 0, 0, $at
  ldv $v04, 0, 16, $s6
  ldv $v05, 0, 24, $s6
  vmudl $v02, $v00, $v31.e3
  ldv $v02, 0, 8, $at
  vmadm $v01, $v01, $v31.e3
  vmadn $v02, $v00, $v00
  vrcph $v04.e3, $v04.e3
  vrcpl $v05.e3, $v05.e3
  vrcph $v04.e3, $v00.e3
  vmudl $v29, $v05, $v05.e3
  ldv $v03, 0, 8, $at
  ssv $v05, 6, 34, $s6
  vmadm $v29, $v04, $v05.e3
  ssv $v04, 6, 32, $s6
  vmadn $v05, $v05, $v04.e3
  vmadh $v04, $v04, $v04.e3
  vmudl $v29, $v05, $v02.v
  vmadm $v29, $v04, $v02.v
  vmadn $v05, $v05, $v01.v
  vmadh $v04, $v04, $v01.v
  vadd $v04, $v04, $v03.v
  sdv $v04, 0, 0, $s6 ## Barrier: 0x4
  ssv $v03, 6, 6, $s6 ## Barrier: 0x4
  jr $ra
  addiu $s6, $s6, 65500

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