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
  or $s0, $zero, $a1                                 ## L:850  |      ^ | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  andi $t0, $a0, 65535                               ## L:834  |      2 | u32<$t0> dmaSize = dataSize & 0xFFFF;
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:833  |      3 | u16<$s4> dmaDmem = PARTICLE_BUFF;
  addu $s7, $s4, $t0                                 ## L:849  |      4 | u32 ptrInEnd = dmaDmem + dmaSize;
  addiu $t0, $t0, -1                                 ## L:850  |      5 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  jal DMAExec                                        ## L:850  |      6 | dma_in_async(dmaDmem, rdramAddr, dmaSize); ## Args: $t0, $t1, $s0, $s4, $t2
  or $t2, $zero, $zero                               ## L:850  |     *8 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  vmudl $v20, $v00, $v31.e3                          ## L:865  |      9 | screenSize >>= 4;
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)                 ## L:1213 |      ^ | u32 extMax = load(RDPQ_SCISSOR_RECT, 4);
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)                 ## L:1214 |     10 | u32 extMin = load(RDPQ_SCISSOR_RECT, 0);
  ori $at, $zero, %lo(MATRIX_MVP)                    ## L:858  |     11 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v21, 0, 48, $at                               ## L:861  |     12 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  mtc2 $s2, $v15.e5                                  ## L:1218 |     13 | screenMax.y = extMax; screenMax.Y = extMax;
  ldv $v23, 0, 32, $at                               ## L:860  |     14 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v28, 0, 8, $at                                ## L:858  |     15 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v22, 0, 56, $at                               ## L:861  |     16 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v26, 0, 24, $at                               ## L:859  |     17 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ori $s3, $zero, %lo(RDP_BUFF)                      ## L:855  |     18 | u16<$s3> dmaDmemEnd = RDP_BUFF;
  ldv $v25, 0, 16, $at                               ## L:859  |     19 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v25, 8, 16, $at                               ## L:859  |     20 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:860  |     21 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v21, 8, 48, $at                               ## L:861  |     22 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v26, 8, 24, $at                               ## L:859  |     23 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v28, 8, 8, $at                                ## L:858  |     24 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:858  |     25 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v24, 8, 40, $at                               ## L:860  |     26 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v23, 8, 32, $at                               ## L:860  |     27 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v27, 8, 0, $at                                ## L:858  |     28 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v22, 8, 56, $at                               ## L:861  |     29 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:863  |     30 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v18, 0, 8, $at                                ## L:867  |     31 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  mtc2 $s1, $v16.e5                                  ## L:1222 |     32 | screenMin.y = extMin; screenMin.Y = extMin;
  ldv $v18, 8, 8, $at                                ## L:867  |     33 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  srl $t4, $s2, 12                                   ## L:1216 |     34 | temp1 = extMax >> 12;
  ldv $v19, 0, 0, $at                                ## L:863  |     35 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  mtc2 $s1, $v16.e1                                  ## L:1222 |     36 | screenMin.y = extMin; screenMin.Y = extMin;
  ldv $v19, 8, 0, $at                                ## L:863  |     37 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:868  |     38 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  addiu $s5, $s3, 528                                ## L:856  |     39 | u16 dmaDmemFlush = dmaDmemEnd + 528;
  mtc2 $s2, $v15.e1                                  ## L:1218 |     40 | screenMax.y = extMax; screenMax.Y = extMax;
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:881  |     41 | u16 buffRdp = RDP_BUFF;
  ldv $v17, 0, 0, $at                                ## L:868  |     42 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  mtc2 $t4, $v15.e4                                  ## L:1217 |     43 | screenMax.x = temp1;  screenMax.X = temp1;
  ldv $v17, 8, 0, $at                                ## L:868  |     44 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  lui $t7, 0x3A00                                    ## L:883  |     45 | cmdRdpColor = 0x3A00'0000;
  addiu $at, $zero, 4095                             ## L:1212 |     46 | screenMax.w = 0b1111'1111'1111;
  mtc2 $at, $v15.e3                                  ## L:1212 |     47 | screenMax.w = 0b1111'1111'1111;
  mtc2 $t4, $v15.e0                                  ## L:1217 |     48 | screenMax.x = temp1;  screenMax.X = temp1;
  ori $at, $zero, %lo(PARTICLE_SCALE)                ## L:875  |     49 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  addiu $a0, $zero, 46                               ## L:884  |     50 | cmdRdpDepth = 0x2E;
  srl $t4, $s1, 12                                   ## L:1220 |     51 | temp1 = extMin >> 12;
  vmadm $v19, $v19, $v31.e3                          ## L:865  |      ^ | screenSize >>= 4;
  vmadn $v20, $v00, $v00                             ## L:865  |     52 | screenSize >>= 4;
  mtc2 $t4, $v16.e0                                  ## L:1221 |      ^ | screenMin.x = temp1;  screenMin.X = temp1;
  mtc2 $t4, $v16.e4                                  ## L:1221 |     53 | screenMin.x = temp1;  screenMin.X = temp1;
  vand $v15, $v15, $v15.e3                           ## L:1224 |      ^ | screenMax &= screenMax.w;
  llv $v13, 0, 0, $at                                ## L:875  |     54 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  llv $v13, 8, 0, $at                                ## L:876  |     55 | globalPartSize.XY = load(PARTICLE_SCALE).xy;
  addiu $s1, $s5, 24                                 ## L:882  |     56 | u16 buffRdpEnd = dmaDmemFlush + 24;
  or $s6, $zero, $s4                                 ## L:852  |     57 | u32 ptrIn = dmaDmem;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:854  |     58 | dmaDmem = RDP_BUFF;
  vand $v16, $v16, $v15.e3                           ## L:1225 |      ^ | screenMin &= screenMax.w;
  LABEL_TPXCmd_DrawColor_0001:
  sh $zero, 14($s2)                                  ## L:888  |     59 | store(ZERO:u16, buffRdp, 0x0E);
  sw $t7, 0($s2)                                     ## L:886  |     60 | store(cmdRdpColor, buffRdp, 0);
  sb $a0, 8($s2)                                     ## L:887  |     61 | store(cmdRdpDepth, buffRdp, 8);
  addiu $s2, $s2, 24                                 ## L:889  |     62 | buffRdp += 24;
  bne $s2, $s1, LABEL_TPXCmd_DrawColor_0001          ## L:889  |     63 | buffRdp += 24;
  nop                                                ## L:889  |    *65 | buffRdp += 24;
  LABEL_TPXCmd_DrawColor_0002:
  jal DMAWaitIdle                                    ## L:893  |     66 | dma_await();
  nop                                                ## L:893  |    *68 | dma_await();
  bgez $a1, LABEL_TPXCmd_DrawColor_0003              ## L:900  |     69 | if(rdramAddr < 0) {
  vxor $v10, $v00, $v30.e7                           ## L:897  |    *71 | const vec16 vecOne = 1;
  lqv $v09, 0, 0, $s6                                ## L:466  |     72 | vec16 posStart = load(ptrIn, 0x00);
  vmulf $v14, $v13, $v09.h3                          ## L:467  |  ***76 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudn $v12, $v28, $v09.h0                          ## L:93   |     77 | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:93   |     78 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:94   |     79 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:94   |     80 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:95   |     81 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:95   |     82 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:96   |     83 | out = mat3 +* 1;
  vmadh $v11, $v21, $v30.e7                          ## L:96   |     84 | out = mat3 +* 1;
  vch $v29, $v11, $v11.h3                            ## L:475  |  ***88 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:475  |     89 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:475  |     90 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0005:
  vmudl $v12, $v12, $v17.v                           ## L:486  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v11, $v11, $v17.v                           ## L:486  |     91 | posClip *= normScaleW:ufract;
  ldv $v03, 0, 16, $s6                               ## L:531  |      ^ | vec16 color = load(ptrIn, 16).xyzw;
  vmadn $v12, $v00, $v00                             ## L:486  |     92 | posClip *= normScaleW:ufract;
  vrcph $v07.e3, $v11.e3                             ## L:488  |   **95 | invW.w = invert_half(posClip).w;
  andi $t1, $t4, 1028                                ## L:482  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  vrcpl $v08.e3, $v12.e3                             ## L:488  |     96 | invW.w = invert_half(posClip).w;
  addiu $t2, $zero, 54                               ## L:535  |      ^ | cmdRdpRect = 0x36;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:492  |     97 | posClip:sint.w = load(BASE_SIZE).x;
  vrcph $v07.e3, $v11.e7                             ## L:489  |      ^ | invW.W = invert_half(posClip).W;
  vrcpl $v08.e7, $v12.e7                             ## L:489  |     98 | invW.W = invert_half(posClip).W;
  vmov $v12.e3, $v00.e0                              ## L:494  |     99 | posClip:sfract.w = 0;
  vmov $v12.e7, $v00.e0                              ## L:495  |    100 | posClip:sfract.W = 0;
  lsv $v11, 6, 0, $at                                ## L:492  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  lsv $v11, 14, 0, $at                               ## L:493  |    101 | posClip:sint.W = load(BASE_SIZE).x;
  vrcph $v07.e7, $v00.e7                             ## L:489  |      ^ | invW.W = invert_half(posClip).W;
  andi $t5, $t4, 16448                               ## L:483  |  **104 | clipB = temp1 & 0b0100'0000'0100'0000;
  vmudl $v29, $v12, $v08.h3                          ## L:498  |      ^ | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v11, $v08.h3                          ## L:498  |    105 | posClip *= invW.wwwwWWWW;
  vmadn $v12, $v12, $v07.h3                          ## L:498  |    106 | posClip *= invW.wwwwWWWW;
  vmadh $v11, $v11, $v07.h3                          ## L:498  |    107 | posClip *= invW.wwwwWWWW;
  slv $v03, 0, 4, $s3                                ## L:543  |      ^ | store(color.xy, dmaDmemEnd, 4);
  vmulf $v14, $v14, $v11.h3                          ## L:502  | ***111 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v12, $v20.v                           ## L:505  |    112 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v11, $v20.v                           ## L:505  |    113 | vec32 posScreen = posClip * screenSize;
  vmadn $v08, $v12, $v19.v                           ## L:505  |    114 | vec32 posScreen = posClip * screenSize;
  vmadh $v07, $v11, $v19.v                           ## L:505  |    115 | vec32 posScreen = posClip * screenSize;
  vmadh $v06, $v18, $v10.v                           ## L:506  |    116 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v05, $v10, $v14.v                           ## L:509  |    117 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v09, $v06, $v14.v                           ## L:510  |  **120 | posStart = posCenter - localPartSize:sint;
  vlt $v05, $v05, $v15                               ## L:513  |    121 | posEnd = min(posEnd, screenMax);
  addiu $at, $zero, 3                                ## L:567  |      ^ | if(clipA == 0b0000'0011) {
  ssv $v06, 4, 12, $s3                               ## L:547  |  **124 | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  vge $v09, $v09, $v16                               ## L:514  |      ^ | posStart = max(posStart, screenMin);
  mfc2 $fp, $v05.e1                                  ## L:1191 |    125 | outA = pos.y;
  mfc2 $sp, $v05.e5                                  ## L:1197 |    126 | outB = pos.Y;
  andi $fp, $fp, 4095                                ## L:1192 |   *128 | outA &= 0b1111'1111'1111;
  mfc2 $s2, $v09.e1                                  ## L:1191 |    129 | outA = pos.y;
  mfc2 $k1, $v09.e0                                  ## L:1193 |    130 | u32 tmp = pos.x;
  andi $sp, $sp, 4095                                ## L:1198 |    131 | outB &= 0b1111'1111'1111;
  andi $s2, $s2, 4095                                ## L:1192 |    132 | outA &= 0b1111'1111'1111;
  mfc2 $s1, $v09.e5                                  ## L:1197 |    133 | outB = pos.Y;
  vlt $v04, $v09, $v05                               ## L:520  |      ^ | vec16 extend = posStart < posEnd;
  sll $k1, $k1, 12                                   ## L:1194 |    134 | tmp <<= 12;
  or $s2, $s2, $k1                                   ## L:1195 |    135 | outA |= tmp;
  andi $s1, $s1, 4095                                ## L:1198 |    136 | outB &= 0b1111'1111'1111;
  mfc2 $k1, $v09.e4                                  ## L:1199 |    137 | tmp = pos.X;
  sw $s2, 4 + 16($s3)                                ## L:551  |    138 | store(posA, dmaDmemEnd, 0x04, 16);
  ldv $v09, 0, 24, $s6                               ## L:558  |    139 | posStart.xyzw = load(ptrIn, 24).xyzw;
  sll $k1, $k1, 12                                   ## L:1200 |    140 | tmp <<= 12;
  or $s1, $s1, $k1                                   ## L:1201 |    141 | outB |= tmp;
  mfc2 $k1, $v05.e0                                  ## L:1193 |    142 | u32 tmp = pos.x;
  cfc2 $t4, $vcc                                     ## L:521  |    143 | temp1 = get_vcc();
  ldv $v09, 8, 32, $s6                               ## L:559  |    144 | posStart.XYZW = load(ptrIn, 24).XYZW;
  sll $k1, $k1, 12                                   ## L:1194 |    145 | tmp <<= 12;
  or $fp, $fp, $k1                                   ## L:1195 |    146 | outA |= tmp;
  sw $fp, 0 + 16($s3)                                ## L:552  |    147 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  andi $a2, $t4, 3                                   ## L:523  |    148 | temp0 = temp1 & 0b0000'0011;
  mfc2 $k1, $v05.e4                                  ## L:1199 |    149 | tmp = pos.X;
  or $t1, $t1, $a2                                   ## L:524  |    150 | clipA |= temp0;
  andi $a2, $t4, 48                                  ## L:526  |    151 | temp0 = temp1 & 0b0011'0000;
  vmulf $v14, $v13, $v09.h3                          ## L:560  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  sll $k1, $k1, 12                                   ## L:1200 |    152 | tmp <<= 12;
  sb $t2, 0 + 16($s3)                                ## L:553  |    153 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  or $t5, $t5, $a2                                   ## L:527  |    154 | clipB |= temp0;
  bne $t1, $at, LABEL_TPXCmd_DrawColor_0007          ## L:567  |    155 | if(clipA == 0b0000'0011) {
  or $sp, $sp, $k1                                   ## L:1201 |   *157 | outB |= tmp;
  addiu $s3, $s3, 24                                 ## L:568  |    158 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_0007:
  addiu $at, $zero, 48                               ## L:578  |    159 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawColor_0008          ## L:578  |    160 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 24                                 ## L:572  |   *162 | ptrIn += 24;
  sw $s1, 4 + 16($s3)                                ## L:582  |    163 | store(posB, dmaDmemEnd, 0x04, 16);
  ssv $v06, 12, 12, $s3                              ## L:580  |    164 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  sw $sp, 0 + 16($s3)                                ## L:583  |    165 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:584  |    166 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v03, 4, 4, $s3                                ## L:579  |    167 | store(color.zw, dmaDmemEnd, 4);
  addiu $s3, $s3, 24                                 ## L:586  |    168 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_0008:
  vmudn $v12, $v28, $v09.h0                          ## L:93   |      ^ | out = mat0  * vec.xxxxXXXX;
  sltu $at, $s3, $s5                                 ## L:593  |    169 | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v27, $v09.h0                          ## L:93   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:94   |    170 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:94   |    171 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:95   |    172 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:95   |    173 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:96   |    174 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawColor_0009        ## L:593  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v21, $v30.e7                          ## L:96   |   *176 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:594  |    177 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:594  |   *179 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:595  |    180 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawColor_0009:
  vch $v29, $v11, $v11.h3                            ## L:598  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:598  |    181 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawColor_0005          ## L:598  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:598  |   *183 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0006:
  beq $zero, $zero, LABEL_TPXCmd_DrawColor_0004      ## L:598  |    184 | temp1 = clip(posClip, posClip.wwwwWWWW);
  nop                                                ## L:598  |   *186 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0003:
  lpv $v09, 0, 0, $s6                                ## L:116  |    187 | vec16 posStart = load_vec_s8(ptrIn, 0x00);
  vmulf $v14, $v13, $v09.h3                          ## L:117  | ***191 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v09, $v09, $v31.e7                          ## L:118  |    192 | posStart >>= 8;
  vmudn $v12, $v28, $v09.h0                          ## L:93   | ***196 | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:93   |    197 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:94   |    198 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:94   |    199 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:95   |    200 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:95   |    201 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:96   |    202 | out = mat3 +* 1;
  vmadh $v11, $v21, $v30.e7                          ## L:96   |    203 | out = mat3 +* 1;
  vch $v29, $v11, $v11.h3                            ## L:122  | ***207 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:122  |    208 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:122  |    209 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_000A:
  vmudl $v12, $v12, $v17.v                           ## L:133  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v11, $v11, $v17.v                           ## L:133  |    210 | posClip *= normScaleW:ufract;
  vmadn $v12, $v00, $v00                             ## L:133  |    211 | posClip *= normScaleW:ufract;
  ldv $v03, 0, 8, $s6                                ## L:180  |      ^ | vec16 color = load(ptrIn, 8).xyzw;
  andi $t1, $t4, 1028                                ## L:129  |    212 | clipA = temp1 & 0b0000'0100'0000'0100;
  addiu $t2, $zero, 54                               ## L:182  |   *214 | cmdRdpRect = 0x36;
  vrcph $v07.e3, $v11.e3                             ## L:135  |      ^ | invW.w = invert_half(posClip).w;
  vrcpl $v08.e3, $v12.e3                             ## L:135  |    215 | invW.w = invert_half(posClip).w;
  vrcph $v07.e3, $v11.e7                             ## L:136  |    216 | invW.W = invert_half(posClip).W;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:139  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcpl $v08.e7, $v12.e7                             ## L:136  |    217 | invW.W = invert_half(posClip).W;
  lsv $v11, 6, 0, $at                                ## L:139  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vmov $v12.e3, $v00.e0                              ## L:141  |    218 | posClip:sfract.w = 0;
  lsv $v11, 14, 0, $at                               ## L:140  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  addiu $at, $zero, 3                                ## L:214  |    219 | if(clipA == 0b0000'0011) {
  vmov $v12.e7, $v00.e0                              ## L:142  |      ^ | posClip:sfract.W = 0;
  vrcph $v07.e7, $v00.e7                             ## L:136  |    220 | invW.W = invert_half(posClip).W;
  vmudl $v29, $v12, $v08.h3                          ## L:145  |  **223 | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v11, $v08.h3                          ## L:145  |    224 | posClip *= invW.wwwwWWWW;
  vmadn $v12, $v12, $v07.h3                          ## L:145  |    225 | posClip *= invW.wwwwWWWW;
  vmadh $v11, $v11, $v07.h3                          ## L:145  |    226 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v11.h3                          ## L:149  | ***230 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  andi $t5, $t4, 16448                               ## L:130  |      ^ | clipB = temp1 & 0b0100'0000'0100'0000;
  vmudl $v29, $v12, $v20.v                           ## L:152  |    231 | vec32 posScreen = posClip * screenSize;
  slv $v03, 0, 4, $s3                                ## L:190  |      ^ | store(color.xy, dmaDmemEnd, 4);
  vmadm $v29, $v11, $v20.v                           ## L:152  |    232 | vec32 posScreen = posClip * screenSize;
  vmadn $v08, $v12, $v19.v                           ## L:152  |    233 | vec32 posScreen = posClip * screenSize;
  vmadh $v07, $v11, $v19.v                           ## L:152  |    234 | vec32 posScreen = posClip * screenSize;
  vmadh $v06, $v18, $v10.v                           ## L:153  |    235 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v05, $v10, $v14.v                           ## L:156  |    236 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v09, $v06, $v14.v                           ## L:157  |  **239 | posStart = posCenter - localPartSize:sint;
  vlt $v05, $v05, $v15                               ## L:160  |    240 | posEnd = min(posEnd, screenMax);
  vge $v09, $v09, $v16                               ## L:161  |  **243 | posStart = max(posStart, screenMin);
  ssv $v06, 4, 12, $s3                               ## L:194  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  mfc2 $fp, $v05.e1                                  ## L:1191 |    244 | outA = pos.y;
  mfc2 $sp, $v05.e5                                  ## L:1197 |    245 | outB = pos.Y;
  andi $fp, $fp, 4095                                ## L:1192 |   *247 | outA &= 0b1111'1111'1111;
  mfc2 $k1, $v09.e0                                  ## L:1193 |    248 | u32 tmp = pos.x;
  mfc2 $s2, $v09.e1                                  ## L:1191 |    249 | outA = pos.y;
  andi $sp, $sp, 4095                                ## L:1198 |    250 | outB &= 0b1111'1111'1111;
  vlt $v04, $v09, $v05                               ## L:167  |      ^ | vec16 extend = posStart < posEnd;
  sll $k1, $k1, 12                                   ## L:1194 |    251 | tmp <<= 12;
  mfc2 $s1, $v09.e5                                  ## L:1197 |    252 | outB = pos.Y;
  andi $s2, $s2, 4095                                ## L:1192 |    253 | outA &= 0b1111'1111'1111;
  or $s2, $s2, $k1                                   ## L:1195 |    254 | outA |= tmp;
  mfc2 $k1, $v09.e4                                  ## L:1199 |    255 | tmp = pos.X;
  cfc2 $t4, $vcc                                     ## L:168  |    256 | temp1 = get_vcc();
  lpv $v09, 0, 16, $s6                               ## L:209  |    257 | posStart = load_vec_s8(ptrIn, 16);
  sll $k1, $k1, 12                                   ## L:1200 |    258 | tmp <<= 12;
  andi $s1, $s1, 4095                                ## L:1198 |    259 | outB &= 0b1111'1111'1111;
  andi $a2, $t4, 3                                   ## L:170  |    260 | temp0 = temp1 & 0b0000'0011;
  or $s1, $s1, $k1                                   ## L:1201 |    261 | outB |= tmp;
  mfc2 $k1, $v05.e0                                  ## L:1193 |    262 | u32 tmp = pos.x;
  vmulf $v14, $v13, $v09.h3                          ## L:210  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v09, $v09, $v31.e7                          ## L:211  |    263 | posStart >>= 8;
  or $t1, $t1, $a2                                   ## L:171  |      ^ | clipA |= temp0;
  andi $a2, $t4, 48                                  ## L:173  |    264 | temp0 = temp1 & 0b0011'0000;
  sll $k1, $k1, 12                                   ## L:1194 |    265 | tmp <<= 12;
  or $fp, $fp, $k1                                   ## L:1195 |    266 | outA |= tmp;
  mfc2 $k1, $v05.e4                                  ## L:1199 |    267 | tmp = pos.X;
  sw $s2, 4 + 16($s3)                                ## L:198  |    268 | store(posA, dmaDmemEnd, 0x04, 16);
  or $t5, $t5, $a2                                   ## L:174  |    269 | clipB |= temp0;
  sw $fp, 0 + 16($s3)                                ## L:199  |    270 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:200  |    271 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sll $k1, $k1, 12                                   ## L:1200 |    272 | tmp <<= 12;
  bne $t1, $at, LABEL_TPXCmd_DrawColor_000C          ## L:214  |    273 | if(clipA == 0b0000'0011) {
  or $sp, $sp, $k1                                   ## L:1201 |   *275 | outB |= tmp;
  addiu $s3, $s3, 24                                 ## L:215  |    276 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_000C:
  addiu $at, $zero, 48                               ## L:225  |    277 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawColor_000D          ## L:225  |    278 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 16                                 ## L:221  |   *280 | ptrIn += 16;
  ssv $v06, 12, 12, $s3                              ## L:227  |    281 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  slv $v03, 4, 4, $s3                                ## L:226  |    282 | store(color.zw, dmaDmemEnd, 4);
  sw $sp, 0 + 16($s3)                                ## L:230  |    283 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sw $s1, 4 + 16($s3)                                ## L:229  |    284 | store(posB, dmaDmemEnd, 0x04, 16);
  sb $t2, 0 + 16($s3)                                ## L:231  |    285 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  addiu $s3, $s3, 24                                 ## L:233  |    286 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_000D:
  vmudn $v12, $v28, $v09.h0                          ## L:93   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:93   |    287 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:94   |    288 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:94   |    289 | out = mat1 +* vec.yyyyYYYY;
  sltu $at, $s3, $s5                                 ## L:240  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadn $v12, $v24, $v09.h2                          ## L:95   |    290 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:95   |    291 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:96   |    292 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawColor_000E        ## L:240  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v21, $v30.e7                          ## L:96   |   *294 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:241  |    295 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:241  |   *297 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:242  |    298 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawColor_000E:
  vch $v29, $v11, $v11.h3                            ## L:245  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:245  |    299 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawColor_000A          ## L:245  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:245  |   *301 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0004:
  j RDPQ_Send                                        ## L:908  |    302 | goto RDPQ_Send;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:907  |    303 | RA = RSPQ_Loop;
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
  or $s0, $zero, $a1                                 ## L:1097 |      ^ | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  andi $t0, $a0, 65535                               ## L:1081 |      2 | u32<$t0> dmaSize = dataSize & 0xFFFF;
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:1080 |      3 | u16<$s4> dmaDmem = PARTICLE_BUFF;
  addu $s7, $s4, $t0                                 ## L:1096 |      4 | u32 ptrInEnd = dmaDmem + dmaSize;
  or $t2, $zero, $zero                               ## L:1097 |      5 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  jal DMAExec                                        ## L:1097 |      6 | dma_in_async(dmaDmem, rdramAddr, dmaSize); ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1                                 ## L:1097 |     *8 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  ori $s3, $zero, %lo(RDP_BUFF)                      ## L:1102 |      9 | u16<$s3> dmaDmemEnd = RDP_BUFF;
  addiu $s5, $s3, 480                                ## L:1103 |     10 | u16 dmaDmemFlush = dmaDmemEnd + 480;
  ori $at, $zero, %lo(MATRIX_MVP)                    ## L:1105 |     11 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:1107 |     12 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:1105 |     13 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v28, 0, 8, $at                                ## L:1105 |     14 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v27, 8, 0, $at                                ## L:1105 |     15 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)                 ## L:1213 |     16 | u32 extMin = load(RDPQ_SCISSOR_RECT, 0);
  ldv $v24, 8, 40, $at                               ## L:1107 |     17 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  or $s6, $zero, $s4                                 ## L:1099 |     18 | u32 ptrIn = dmaDmem;
  ldv $v21, 0, 48, $at                               ## L:1108 |     19 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v28, 8, 8, $at                                ## L:1105 |     20 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v22, 0, 56, $at                               ## L:1108 |     21 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v25, 0, 16, $at                               ## L:1106 |     22 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v26, 0, 24, $at                               ## L:1106 |     23 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v21, 8, 48, $at                               ## L:1108 |     24 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v23, 0, 32, $at                               ## L:1107 |     25 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v23, 8, 32, $at                               ## L:1107 |     26 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)                 ## L:1212 |     27 | u32 extMax = load(RDPQ_SCISSOR_RECT, 4);
  ldv $v22, 8, 56, $at                               ## L:1108 |     28 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v26, 8, 24, $at                               ## L:1106 |     29 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v25, 8, 16, $at                               ## L:1106 |     30 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:1110 |     31 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v18, 0, 8, $at                                ## L:1114 |     32 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  mtc2 $s1, $v16.e5                                  ## L:1221 |     33 | screenMin.y = extMin; screenMin.Y = extMin;
  ldv $v18, 8, 8, $at                                ## L:1114 |     34 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  ldv $v19, 0, 0, $at                                ## L:1110 |     35 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v19, 8, 0, $at                                ## L:1110 |     36 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:1115 |     37 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  srl $t4, $s2, 12                                   ## L:1215 |     38 | temp1 = extMax >> 12;
  mtc2 $s2, $v15.e1                                  ## L:1217 |     39 | screenMax.y = extMax; screenMax.Y = extMax;
  mtc2 $t4, $v15.e4                                  ## L:1216 |     40 | screenMax.x = temp1;  screenMax.X = temp1;
  ldv $v17, 0, 0, $at                                ## L:1115 |     41 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v17, 8, 0, $at                                ## L:1115 |     42 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  lui $t7, 0x3A00                                    ## L:1130 |     43 | cmdRdpColor = 0x3A00'0000;
  addiu $at, $zero, 4095                             ## L:1211 |     44 | screenMax.w = 0b1111'1111'1111;
  mtc2 $at, $v15.e3                                  ## L:1211 |     45 | screenMax.w = 0b1111'1111'1111;
  mtc2 $s2, $v15.e5                                  ## L:1217 |     46 | screenMax.y = extMax; screenMax.Y = extMax;
  addiu $a0, $zero, 46                               ## L:1131 |     47 | cmdRdpDepth = 0x2E;
  ori $at, $zero, %lo(PARTICLE_SCALE)                ## L:1122 |     48 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  vmudl $v20, $v00, $v31.e3                          ## L:1112 |      ^ | screenSize >>= 4;
  mtc2 $t4, $v15.e0                                  ## L:1216 |     49 | screenMax.x = temp1;  screenMax.X = temp1;
  vmadm $v19, $v19, $v31.e3                          ## L:1112 |      ^ | screenSize >>= 4;
  vmadn $v20, $v00, $v00                             ## L:1112 |     50 | screenSize >>= 4;
  mtc2 $s1, $v16.e1                                  ## L:1221 |      ^ | screenMin.y = extMin; screenMin.Y = extMin;
  llv $v13, 0, 0, $at                                ## L:1122 |     51 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  srl $t4, $s1, 12                                   ## L:1219 |    *53 | temp1 = extMin >> 12;
  vand $v15, $v15, $v15.e3                           ## L:1223 |      ^ | screenMax &= screenMax.w;
  mtc2 $t4, $v16.e0                                  ## L:1220 |     54 | screenMin.x = temp1;  screenMin.X = temp1;
  mtc2 $t4, $v16.e4                                  ## L:1220 |     55 | screenMin.x = temp1;  screenMin.X = temp1;
  addiu $s1, $s5, 32                                 ## L:1129 |     56 | u16 buffRdpEnd = dmaDmemFlush + 32;
  llv $v13, 8, 0, $at                                ## L:1123 |     57 | globalPartSize.XY = load(PARTICLE_SCALE).xy;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1101 |     58 | dmaDmem = RDP_BUFF;
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:1128 |     59 | u16 buffRdp = RDP_BUFF;
  vand $v16, $v16, $v15.e3                           ## L:1224 |      ^ | screenMin &= screenMax.w;
  LABEL_TPXCmd_DrawTextured_0013:
  sb $a0, 8($s2)                                     ## L:1134 |     60 | store(cmdRdpDepth, buffRdp, 8);
  sh $zero, 14($s2)                                  ## L:1135 |     61 | store(ZERO:u16, buffRdp, 0x0E);
  sw $t7, 0($s2)                                     ## L:1133 |     62 | store(cmdRdpColor, buffRdp, 0);
  addiu $s2, $s2, 32                                 ## L:1136 |     63 | buffRdp += 32;
  bne $s2, $s1, LABEL_TPXCmd_DrawTextured_0013       ## L:1136 |     64 | buffRdp += 32;
  nop                                                ## L:1136 |    *66 | buffRdp += 32;
  LABEL_TPXCmd_DrawTextured_0014:
  vxor $v12, $v00, $v00.e0                           ## L:1143 |     67 | vec16 texMirrorMask = 0;
  lhu $s1, %lo(TILE_COUNT + 0)                       ## L:1147 |      ^ | u16 tiles = load(TILE_COUNT);
  vmov $v12.e5, $v30.e7                              ## L:1150 |     68 | texMirrorMask.Y = 1;
  vmov $v12.e1, $v30.e7                              ## L:1151 |    *70 | texMirrorMask.y = 1;
  addu $fp, $s1, $s1                                 ## L:1148 |      ^ | u16 tilesEnd = tiles + tiles;
  mtc2 $fp, $v12.e3                                  ## L:1153 |     71 | texMirrorMask.w = tilesEnd;
  mtc2 $s1, $v12.e2                                  ## L:1152 |     72 | texMirrorMask.z = tiles;
  addiu $fp, $fp, 65535                              ## L:1155 |     73 | tilesEnd -= 1;
  vmov $v12.e7, $v31.e7                              ## L:1158 |      ^ | texMirrorMask.W = 0x100;
  mtc2 $fp, $v12.e4                                  ## L:1157 |    *75 | texMirrorMask.X = tilesEnd;
  mtc2 $fp, $v12.e0                                  ## L:1156 |     76 | texMirrorMask.x = tilesEnd;
  vxor $v11, $v00, $v00.e0                           ## L:1144 |      ^ | vec16 texMirrorCompare = 0;
  vmudn $v12, $v12, $v12.e7                          ## L:1159 |  ***80 | texMirrorMask = texMirrorMask * texMirrorMask.W;
  lh $s2, %lo(TEX_OFFSET + 0)                        ## L:1141 |      ^ | s16 texOffset = load(TEX_OFFSET);
  vsubc $v29, $v12, $v30.e7                          ## L:1161 |  ***84 | VTEMP = texMirrorMask - 1;
  vmov $v12.e3, $v29.e3                              ## L:1162 |  ***88 | texMirrorMask.w = VTEMP.w;
  jal DMAWaitIdle                                    ## L:1166 |      ^ | dma_await();
  vor $v11, $v00, $v29.e2                            ## L:1163 |    *90 | texMirrorCompare = VTEMP.z;
  bgez $a1, LABEL_TPXCmd_DrawTextured_0015           ## L:1171 |     91 | if(rdramAddr < 0) {
  nop                                                ## L:1171 |    *93 | if(rdramAddr < 0) {
  vxor $v07, $v00, $v30.e7                           ## L:614  |     94 | const vec16 vecOne = 1;
  lqv $v08, 0, 0, $s6                                ## L:606  |      ^ | vec16 posStart = load(ptrIn, 0x00);
  vmulf $v14, $v13, $v08.h3                          ## L:607  |  ***98 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudn $v10, $v28, $v08.h0                          ## L:92   |     99 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |    100 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    101 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    102 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    103 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    104 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    105 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:95   |    106 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:619  | ***110 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:619  |    111 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:619  |    112 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0017:
  vmudl $v10, $v10, $v17.v                           ## L:630  |      ^ | posClip *= normScaleW:ufract;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:636  |    113 | posClip:sint.w = load(BASE_SIZE).x;
  vmadm $v09, $v09, $v17.v                           ## L:630  |      ^ | posClip *= normScaleW:ufract;
  vmadn $v10, $v00, $v00                             ## L:630  |    114 | posClip *= normScaleW:ufract;
  vrcph $v05.e3, $v09.e3                             ## L:632  |  **117 | invW.w = invert_half(posClip).w;
  vrcpl $v06.e3, $v10.e3                             ## L:632  |    118 | invW.w = invert_half(posClip).w;
  andi $t5, $t4, 16448                               ## L:627  |      ^ | clipB = temp1 & 0b0100'0000'0100'0000;
  vrcph $v05.e3, $v09.e7                             ## L:633  |    119 | invW.W = invert_half(posClip).W;
  lsv $v09, 6, 0, $at                                ## L:636  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  andi $t1, $t4, 1028                                ## L:626  |    120 | clipA = temp1 & 0b0000'0100'0000'0100;
  vrcpl $v06.e7, $v10.e7                             ## L:633  |      ^ | invW.W = invert_half(posClip).W;
  lsv $v09, 14, 0, $at                               ## L:637  |    121 | posClip:sint.W = load(BASE_SIZE).x;
  vmov $v10.e3, $v00.e0                              ## L:638  |      ^ | posClip:sfract.w = 0;
  lb $t4, 15($s6)                                    ## L:675  |    122 | temp1:s8 = load(ptrIn, 15);
  vmov $v10.e7, $v00.e0                              ## L:639  |      ^ | posClip:sfract.W = 0;
  vrcph $v05.e7, $v00.e7                             ## L:633  |    123 | invW.W = invert_half(posClip).W;
  lb $a2, 7($s6)                                     ## L:674  |      ^ | temp0:s8 = load(ptrIn, 7);
  addu $t4, $t4, $s2                                 ## L:681  |   *125 | temp1 += texOffset; temp1 <<= 3;
  addu $a2, $a2, $s2                                 ## L:680  |    126 | temp0 += texOffset; temp0 <<= 3;
  vmudl $v29, $v10, $v06.h3                          ## L:642  |      ^ | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v09, $v06.h3                          ## L:642  |    127 | posClip *= invW.wwwwWWWW;
  sll $t4, $t4, 3                                    ## L:681  |      ^ | temp1 += texOffset; temp1 <<= 3;
  vmadn $v10, $v10, $v05.h3                          ## L:642  |    128 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:642  |    129 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:645  | ***133 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:648  |    134 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:648  |    135 | vec32 posScreen = posClip * screenSize;
  vmadn $v04, $v10, $v19.v                           ## L:648  |    136 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:648  |    137 | vec32 posScreen = posClip * screenSize;
  vmadh $v02, $v18, $v07.v                           ## L:649  |    138 | vec16 posCenter = screenOffset:sint +* vecOne;
  sll $a2, $a2, 3                                    ## L:680  |      ^ | temp0 += texOffset; temp0 <<= 3;
  vmadh $v04, $v07, $v14.v                           ## L:654  |    139 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v08, $v02, $v14.v                           ## L:655  |  **142 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:659  |    143 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:660  | ***147 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:660  |    148 | invW.x = invert_half(localPartSize).x;
  vrcp $v06.e1, $v14.e1                              ## L:661  |    149 | invW.y = invert_half(localPartSize).y;
  vrcph $v05.e1, $v14.e1                             ## L:661  |    150 | invW.y = invert_half(localPartSize).y;
  vrcp $v06.e4, $v14.e4                              ## L:662  |    151 | invW.X = invert_half(localPartSize).X;
  vrcph $v05.e4, $v14.e4                             ## L:662  |    152 | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:663  |    153 | invW.Y = invert_half(localPartSize).Y;
  vrcph $v05.e5, $v14.e5                             ## L:663  |    154 | invW.Y = invert_half(localPartSize).Y;
  vmudh $v01, $v08, $v05.v                           ## L:668  | ***158 | vec16 uvStart = posStart * invW:sint;
  vmudm $v01, $v01, $v31.e6                          ## L:669  | ***162 | uvStart >>= 7;
  vor $v03, $v00, $v05                               ## L:664  |    163 | vec16 uvDelta = invW:sint;
  vxor $v05, $v00, $v00.e0                           ## L:694  |    164 | vec16 texOffsetTotal = 0;
  ssv $v02, 4, 12, $s3                               ## L:749  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  mtc2 $t4, $v05.e4                                  ## L:696  |    165 | texOffsetTotal.X = temp1:s16;
  mtc2 $a2, $v05.e0                                  ## L:695  |    166 | texOffsetTotal.x = temp0:s16;
  vlt $v01, $v01, $v00.e0                            ## L:670  |      ^ | uvStart = uvStart < 0;
  vsubc $v06, $v00, $v01.v                           ## L:692  | ***170 | vec16 uvStartNeg = VZERO - uvStart;
  vand $v05, $v05, $v12.e3                           ## L:697  |    171 | texOffsetTotal &= texMirrorMask.w;
  vge $v29, $v11, $v05.h0                            ## L:700  | ***175 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vmrg $v01, $v06, $v01                              ## L:700  |    176 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  slv $v03, 0, 28, $s3                               ## L:758  |      ^ | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vlt $v04, $v04, $v15                               ## L:703  |    177 | posEnd = min(posEnd, screenMax);
  addiu $t2, $zero, 36                               ## L:737  |      ^ | cmdRdpRect = 0x24;
  vge $v08, $v08, $v16                               ## L:704  |    178 | posStart = max(posStart, screenMin);
  vaddc $v01, $v01, $v05.v                           ## L:706  |   *180 | uvStart += texOffsetTotal;
  addiu $at, $zero, 3                                ## L:771  |      ^ | if(clipA == 0b0000'0011) {
  mfc2 $k1, $v04.e5                                  ## L:1196 |    181 | outB = pos.Y;
  mfc2 $sp, $v04.e1                                  ## L:1190 |    182 | outA = pos.y;
  mfc2 $k0, $v08.e0                                  ## L:1192 |  **185 | u32 tmp = pos.x;
  vsubc $v06, $v12, $v01.v                           ## L:711  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  mfc2 $s1, $v08.e1                                  ## L:1190 |    186 | outA = pos.y;
  andi $k1, $k1, 4095                                ## L:1197 |    187 | outB &= 0b1111'1111'1111;
  mfc2 $fp, $v08.e5                                  ## L:1196 |   *189 | outB = pos.Y;
  vaddc $v06, $v06, $v12.e2                          ## L:712  |      ^ | uvStartNeg += texMirrorMask.z;
  vge $v29, $v11, $v05.h0                            ## L:714  |    190 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  ldv $v05, 0, 16, $s6                               ## L:732  |      ^ | vec16 color = load(ptrIn, 16).xyzw;
  sll $k0, $k0, 12                                   ## L:1193 |    191 | tmp <<= 12;
  andi $s1, $s1, 4095                                ## L:1191 |    192 | outA &= 0b1111'1111'1111;
  or $s1, $s1, $k0                                   ## L:1194 |    193 | outA |= tmp;
  mfc2 $k0, $v08.e4                                  ## L:1198 |    194 | tmp = pos.X;
  andi $fp, $fp, 4095                                ## L:1197 |    195 | outB &= 0b1111'1111'1111;
  vmrg $v01, $v01, $v06                              ## L:714  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  vlt $v06, $v08, $v04                               ## L:721  |    196 | vec16 extend = posStart < posEnd;
  ldv $v08, 0, 24, $s6                               ## L:762  |      ^ | posStart.xyzw = load(ptrIn, 24).xyzw;
  sll $k0, $k0, 12                                   ## L:1199 |    197 | tmp <<= 12;
  or $fp, $fp, $k0                                   ## L:1200 |    198 | outB |= tmp;
  mfc2 $k0, $v04.e0                                  ## L:1192 |    199 | u32 tmp = pos.x;
  cfc2 $t4, $vcc                                     ## L:722  |    200 | temp1 = get_vcc();
  andi $sp, $sp, 4095                                ## L:1191 |    201 | outA &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1193 |    202 | tmp <<= 12;
  or $sp, $sp, $k0                                   ## L:1194 |    203 | outA |= tmp;
  ldv $v08, 8, 32, $s6                               ## L:763  |    204 | posStart.XYZW = load(ptrIn, 24).XYZW;
  mfc2 $k0, $v04.e4                                  ## L:1198 |    205 | tmp = pos.X;
  andi $a2, $t4, 3                                   ## L:724  |    206 | temp0 = temp1 & 0b0000'0011;
  or $t1, $t1, $a2                                   ## L:725  |    207 | clipA |= temp0;
  slv $v01, 0, 24, $s3                               ## L:757  |    208 | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  vmulf $v14, $v13, $v08.h3                          ## L:764  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  sw $sp, 0 + 16($s3)                                ## L:755  |    209 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  andi $a2, $t4, 48                                  ## L:727  |    210 | temp0 = temp1 & 0b0011'0000;
  sb $t2, 0 + 16($s3)                                ## L:756  |    211 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v05, 0, 4, $s3                                ## L:745  |    212 | store(color.xy, dmaDmemEnd, 4);
  sll $k0, $k0, 12                                   ## L:1199 |    213 | tmp <<= 12;
  or $k1, $k1, $k0                                   ## L:1200 |    214 | outB |= tmp;
  sw $s1, 4 + 16($s3)                                ## L:754  |    215 | store(posA, dmaDmemEnd, 0x04, 16);
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_0019       ## L:771  |    216 | if(clipA == 0b0000'0011) {
  or $t5, $t5, $a2                                   ## L:728  |   *218 | clipB |= temp0;
  addiu $s3, $s3, 32                                 ## L:772  |    219 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_0019:
  addiu $at, $zero, 48                               ## L:782  |    220 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001A       ## L:782  |    221 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 24                                 ## L:776  |   *223 | ptrIn += 24;
  sw $fp, 4 + 16($s3)                                ## L:786  |    224 | store(posB, dmaDmemEnd, 0x04, 16);
  slv $v05, 4, 4, $s3                                ## L:783  |    225 | store(color.zw, dmaDmemEnd, 4);
  sw $k1, 0 + 16($s3)                                ## L:787  |    226 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:788  |    227 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v03, 8, 28, $s3                               ## L:791  |    228 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  ssv $v02, 12, 12, $s3                              ## L:784  |    229 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  slv $v01, 8, 24, $s3                               ## L:790  |    230 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  addiu $s3, $s3, 32                                 ## L:793  |    231 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001A:
  vmudn $v10, $v28, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |    232 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    233 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    234 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    235 | out = mat2 +* vec.zzzzZZZZ;
  sltu $at, $s3, $s5                                 ## L:800  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    236 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    237 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_001B     ## L:800  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:95   |   *239 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:801  |    240 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:801  |   *242 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:802  |    243 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_001B:
  vch $v29, $v09, $v09.h3                            ## L:805  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:805  |    244 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_0017       ## L:805  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:805  |   *246 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0018:
  beq $zero, $zero, LABEL_TPXCmd_DrawTextured_0016   ## L:805  |    247 | temp1 = clip(posClip, posClip.wwwwWWWW);
  nop                                                ## L:805  |   *249 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0015:
  vxor $v07, $v00, $v30.e7                           ## L:261  |    250 | const vec16 vecOne = 1;
  lpv $v08, 0, 0, $s6                                ## L:256  |      ^ | vec16 posStart = load_vec_s8(ptrIn, 0x00);
  vmulf $v14, $v13, $v08.h3                          ## L:257  | ***254 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v08, $v08, $v31.e7                          ## L:258  |    255 | posStart >>= 8;
  vmudn $v10, $v28, $v08.h0                          ## L:92   | ***259 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |    260 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    261 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    262 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    263 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    264 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    265 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:95   |    266 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:266  | ***270 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:266  |    271 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:266  |    272 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_001C:
  vmudl $v10, $v10, $v17.v                           ## L:277  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v09, $v09, $v17.v                           ## L:277  |    273 | posClip *= normScaleW:ufract;
  vmadn $v10, $v00, $v00                             ## L:277  |    274 | posClip *= normScaleW:ufract;
  lb $a2, 11($s6)                                    ## L:324  |      ^ | temp0:s8 = load(ptrIn, 0x0B);
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:283  |  **277 | posClip:sint.w = load(BASE_SIZE).x;
  vrcph $v05.e3, $v09.e3                             ## L:279  |      ^ | invW.w = invert_half(posClip).w;
  vrcpl $v06.e3, $v10.e3                             ## L:279  |    278 | invW.w = invert_half(posClip).w;
  vrcph $v05.e3, $v09.e7                             ## L:280  |    279 | invW.W = invert_half(posClip).W;
  vrcpl $v06.e7, $v10.e7                             ## L:280  |    280 | invW.W = invert_half(posClip).W;
  vmov $v10.e7, $v00.e0                              ## L:286  |    281 | posClip:sfract.W = 0;
  lsv $v09, 6, 0, $at                                ## L:283  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  lsv $v09, 14, 0, $at                               ## L:284  |    282 | posClip:sint.W = load(BASE_SIZE).x;
  vmov $v10.e3, $v00.e0                              ## L:285  |      ^ | posClip:sfract.w = 0;
  vrcph $v05.e7, $v00.e7                             ## L:280  |    283 | invW.W = invert_half(posClip).W;
  addu $a2, $a2, $s2                                 ## L:327  |      ^ | temp0 += texOffset; temp0 <<= 3;
  vmudl $v29, $v10, $v06.h3                          ## L:289  |  **286 | posClip *= invW.wwwwWWWW;
  sll $a2, $a2, 3                                    ## L:327  |      ^ | temp0 += texOffset; temp0 <<= 3;
  vmadm $v29, $v09, $v06.h3                          ## L:289  |    287 | posClip *= invW.wwwwWWWW;
  vmadn $v10, $v10, $v05.h3                          ## L:289  |    288 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:289  |    289 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:292  | ***293 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:295  |    294 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:295  |    295 | vec32 posScreen = posClip * screenSize;
  vmadn $v04, $v10, $v19.v                           ## L:295  |    296 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:295  |    297 | vec32 posScreen = posClip * screenSize;
  vmadh $v02, $v18, $v07.v                           ## L:296  |    298 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v04, $v07, $v14.v                           ## L:301  |    299 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v08, $v02, $v14.v                           ## L:302  |  **302 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:306  |    303 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:307  | ***307 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:307  |    308 | invW.x = invert_half(localPartSize).x;
  addiu $at, $zero, 3                                ## L:418  |      ^ | if(clipA == 0b0000'0011) {
  vrcp $v06.e1, $v14.e1                              ## L:308  |    309 | invW.y = invert_half(localPartSize).y;
  andi $t5, $t4, 16448                               ## L:274  |      ^ | clipB = temp1 & 0b0100'0000'0100'0000;
  vrcph $v05.e1, $v14.e1                             ## L:308  |    310 | invW.y = invert_half(localPartSize).y;
  vrcp $v06.e4, $v14.e4                              ## L:309  |    311 | invW.X = invert_half(localPartSize).X;
  ssv $v02, 4, 12, $s3                               ## L:396  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  andi $t1, $t4, 1028                                ## L:273  |    312 | clipA = temp1 & 0b0000'0100'0000'0100;
  vrcph $v05.e4, $v14.e4                             ## L:309  |      ^ | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:310  |    313 | invW.Y = invert_half(localPartSize).Y;
  vrcph $v05.e5, $v14.e5                             ## L:310  |    314 | invW.Y = invert_half(localPartSize).Y;
  vmudh $v01, $v08, $v05.v                           ## L:315  | ***318 | vec16 uvStart = posStart * invW:sint;
  vmudm $v01, $v01, $v31.e6                          ## L:316  | ***322 | uvStart >>= 7;
  lb $t4, 15($s6)                                    ## L:325  |      ^ | temp1:s8 = load(ptrIn, 0x0F);
  addu $t4, $t4, $s2                                 ## L:328  | ***326 | temp1 += texOffset; temp1 <<= 3;
  vlt $v01, $v01, $v00.e0                            ## L:317  |      ^ | uvStart = uvStart < 0;
  vor $v03, $v00, $v05                               ## L:311  |    327 | vec16 uvDelta = invW:sint;
  vxor $v05, $v00, $v00.e0                           ## L:341  |    328 | vec16 texOffsetTotal = 0;
  sll $t4, $t4, 3                                    ## L:328  |      ^ | temp1 += texOffset; temp1 <<= 3;
  mtc2 $t4, $v05.e4                                  ## L:343  |    329 | texOffsetTotal.X = temp1:s16;
  mtc2 $a2, $v05.e0                                  ## L:342  |    330 | texOffsetTotal.x = temp0:s16;
  vsubc $v06, $v00, $v01.v                           ## L:339  |      ^ | vec16 uvStartNeg = VZERO - uvStart;
  vand $v05, $v05, $v12.e3                           ## L:344  | ***334 | texOffsetTotal &= texMirrorMask.w;
  addiu $t2, $zero, 36                               ## L:384  |      ^ | cmdRdpRect = 0x24;
  vge $v29, $v11, $v05.h0                            ## L:347  | ***338 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  slv $v03, 0, 28, $s3                               ## L:405  |      ^ | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vmrg $v01, $v06, $v01                              ## L:347  |    339 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vlt $v04, $v04, $v15                               ## L:350  |    340 | posEnd = min(posEnd, screenMax);
  vge $v08, $v08, $v16                               ## L:351  |    341 | posStart = max(posStart, screenMin);
  vaddc $v01, $v01, $v05.v                           ## L:353  |  **344 | uvStart += texOffsetTotal;
  mfc2 $sp, $v04.e1                                  ## L:1190 |      ^ | outA = pos.y;
  mfc2 $s1, $v08.e1                                  ## L:1190 |    345 | outA = pos.y;
  mfc2 $k0, $v08.e0                                  ## L:1192 |  **348 | u32 tmp = pos.x;
  vsubc $v06, $v12, $v01.v                           ## L:358  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  mfc2 $fp, $v08.e5                                  ## L:1196 |    349 | outB = pos.Y;
  sll $k0, $k0, 12                                   ## L:1193 |   *351 | tmp <<= 12;
  mfc2 $k1, $v04.e5                                  ## L:1196 |    352 | outB = pos.Y;
  andi $s1, $s1, 4095                                ## L:1191 |    353 | outA &= 0b1111'1111'1111;
  vaddc $v06, $v06, $v12.e2                          ## L:359  |      ^ | uvStartNeg += texMirrorMask.z;
  vge $v29, $v11, $v05.h0                            ## L:361  |    354 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  andi $sp, $sp, 4095                                ## L:1191 |      ^ | outA &= 0b1111'1111'1111;
  or $s1, $s1, $k0                                   ## L:1194 |    355 | outA |= tmp;
  ldv $v05, 0, 8, $s6                                ## L:381  |    356 | vec16 color = load(ptrIn, 8).xyzw;
  mfc2 $k0, $v08.e4                                  ## L:1198 |    357 | tmp = pos.X;
  andi $k1, $k1, 4095                                ## L:1197 |    358 | outB &= 0b1111'1111'1111;
  vmrg $v01, $v01, $v06                              ## L:361  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  vlt $v06, $v08, $v04                               ## L:368  |    359 | vec16 extend = posStart < posEnd;
  andi $fp, $fp, 4095                                ## L:1197 |      ^ | outB &= 0b1111'1111'1111;
  lpv $v08, 0, 16, $s6                               ## L:413  |    360 | posStart = load_vec_s8(ptrIn, 16);
  sll $k0, $k0, 12                                   ## L:1199 |    361 | tmp <<= 12;
  or $fp, $fp, $k0                                   ## L:1200 |    362 | outB |= tmp;
  slv $v01, 0, 24, $s3                               ## L:404  |    363 | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  mfc2 $k0, $v04.e0                                  ## L:1192 |    364 | u32 tmp = pos.x;
  cfc2 $t4, $vcc                                     ## L:369  |    365 | temp1 = get_vcc();
  sll $k0, $k0, 12                                   ## L:1193 |   *367 | tmp <<= 12;
  or $sp, $sp, $k0                                   ## L:1194 |    368 | outA |= tmp;
  slv $v05, 0, 4, $s3                                ## L:392  |    369 | store(color.xy, dmaDmemEnd, 4);
  mfc2 $k0, $v04.e4                                  ## L:1198 |    370 | tmp = pos.X;
  sw $s1, 4 + 16($s3)                                ## L:401  |    371 | store(posA, dmaDmemEnd, 0x04, 16);
  andi $a2, $t4, 3                                   ## L:371  |    372 | temp0 = temp1 & 0b0000'0011;
  vmulf $v14, $v13, $v08.h3                          ## L:414  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  sw $sp, 0 + 16($s3)                                ## L:402  |    373 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  vmudm $v08, $v08, $v31.e7                          ## L:415  |      ^ | posStart >>= 8;
  sll $k0, $k0, 12                                   ## L:1199 |    374 | tmp <<= 12;
  or $t1, $t1, $a2                                   ## L:372  |    375 | clipA |= temp0;
  or $k1, $k1, $k0                                   ## L:1200 |    376 | outB |= tmp;
  andi $a2, $t4, 48                                  ## L:374  |    377 | temp0 = temp1 & 0b0011'0000;
  sb $t2, 0 + 16($s3)                                ## L:403  |    378 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  or $t5, $t5, $a2                                   ## L:375  |    379 | clipB |= temp0;
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_001E       ## L:418  |    380 | if(clipA == 0b0000'0011) {
  nop                                                ## L:1052 |   *382 | 
  addiu $s3, $s3, 32                                 ## L:419  |    383 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001E:
  addiu $at, $zero, 48                               ## L:429  |    384 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001F       ## L:429  |    385 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 16                                 ## L:425  |   *387 | ptrIn += 16;
  slv $v05, 4, 4, $s3                                ## L:430  |    388 | store(color.zw, dmaDmemEnd, 4);
  slv $v01, 8, 24, $s3                               ## L:437  |    389 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  ssv $v02, 12, 12, $s3                              ## L:431  |    390 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  sw $k1, 0 + 16($s3)                                ## L:434  |    391 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:435  |    392 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v03, 8, 28, $s3                               ## L:438  |    393 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  sw $fp, 4 + 16($s3)                                ## L:433  |    394 | store(posB, dmaDmemEnd, 0x04, 16);
  addiu $s3, $s3, 32                                 ## L:440  |    395 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001F:
  vmudn $v10, $v28, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  sltu $at, $s3, $s5                                 ## L:447  |    396 | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v27, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    397 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    398 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    399 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    400 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    401 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_0020     ## L:447  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:95   |   *403 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:448  |    404 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:448  |   *406 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:449  |    407 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_0020:
  vch $v29, $v09, $v09.h3                            ## L:452  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:452  |    408 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_001C       ## L:452  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:452  |   *410 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0016:
  j RDPQ_Send                                        ## L:1179 |    411 | goto RDPQ_Send;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:1178 |   *413 | RA = RSPQ_Loop;

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