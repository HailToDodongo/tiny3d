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
  andi $t0, $a0, 65535                               ## L:1081 |      ^ | u32<$t0> dmaSize = dataSize & 0xFFFF;
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:1080 |      2 | u16<$s4> dmaDmem = PARTICLE_BUFF;
  addu $s7, $s4, $t0                                 ## L:1096 |      3 | u32 ptrInEnd = dmaDmem + dmaSize;
  or $t2, $zero, $zero                               ## L:1097 |      4 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  or $s0, $zero, $a1                                 ## L:1097 |      5 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  jal DMAExec                                        ## L:1097 |      6 | dma_in_async(dmaDmem, rdramAddr, dmaSize); ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $t0, $t0, -1                                 ## L:1097 |     *8 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  vmudl $v20, $v00, $v31.e3                          ## L:1112 |      9 | screenSize >>= 4;
  ori $at, $zero, %lo(MATRIX_MVP)                    ## L:1105 |      ^ | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v28, 0, 8, $at                                ## L:1105 |     10 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v23, 0, 32, $at                               ## L:1107 |     11 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v25, 0, 16, $at                               ## L:1106 |     12 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:1107 |     13 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v21, 0, 48, $at                               ## L:1108 |     14 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v22, 0, 56, $at                               ## L:1108 |     15 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)                 ## L:1212 |     16 | u32 extMax = load(RDPQ_SCISSOR_RECT, 4);
  ldv $v28, 8, 8, $at                                ## L:1105 |     17 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:1105 |     18 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)                 ## L:1213 |     19 | u32 extMin = load(RDPQ_SCISSOR_RECT, 0);
  ldv $v25, 8, 16, $at                               ## L:1106 |     20 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v21, 8, 48, $at                               ## L:1108 |     21 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v26, 0, 24, $at                               ## L:1106 |     22 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  lui $t7, 0x3A00                                    ## L:1130 |     23 | cmdRdpColor = 0x3A00'0000;
  ldv $v27, 8, 0, $at                                ## L:1105 |     24 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v23, 8, 32, $at                               ## L:1107 |     25 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  srl $t4, $s2, 12                                   ## L:1215 |     26 | temp1 = extMax >> 12;
  ldv $v22, 8, 56, $at                               ## L:1108 |     27 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  mtc2 $t4, $v15.e0                                  ## L:1216 |     28 | screenMax.x = temp1;  screenMax.X = temp1;
  ldv $v26, 8, 24, $at                               ## L:1106 |     29 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v24, 8, 40, $at                               ## L:1107 |     30 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)           ## L:1110 |     31 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v19, 0, 0, $at                                ## L:1110 |     32 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  mtc2 $s2, $v15.e1                                  ## L:1217 |     33 | screenMax.y = extMax; screenMax.Y = extMax;
  ldv $v19, 8, 0, $at                                ## L:1110 |     34 | vec32 screenSize:sint = load(SCREEN_SCALE_OFFSET).xyzwxyzw;
  ldv $v18, 0, 8, $at                                ## L:1114 |     35 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  ldv $v18, 8, 8, $at                                ## L:1114 |     36 | vec16 screenOffset = load(SCREEN_SCALE_OFFSET, 0x08).xyzwxyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:1115 |     37 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v17, 0, 0, $at                                ## L:1115 |     38 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  vmadm $v19, $v19, $v31.e3                          ## L:1112 |      ^ | screenSize >>= 4;
  mtc2 $s1, $v16.e1                                  ## L:1221 |     39 | screenMin.y = extMin; screenMin.Y = extMin;
  addiu $a0, $zero, 46                               ## L:1131 |     40 | cmdRdpDepth = 0x2E;
  ldv $v17, 8, 0, $at                                ## L:1115 |     41 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  mtc2 $t4, $v15.e4                                  ## L:1216 |     42 | screenMax.x = temp1;  screenMax.X = temp1;
  addiu $at, $zero, 4095                             ## L:1211 |     43 | screenMax.w = 0b1111'1111'1111;
  or $s6, $zero, $s4                                 ## L:1099 |     44 | u32 ptrIn = dmaDmem;
  mtc2 $s1, $v16.e5                                  ## L:1221 |     45 | screenMin.y = extMin; screenMin.Y = extMin;
  mtc2 $at, $v15.e3                                  ## L:1211 |     46 | screenMax.w = 0b1111'1111'1111;
  ori $at, $zero, %lo(PARTICLE_SCALE)                ## L:1122 |     47 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  llv $v13, 0, 0, $at                                ## L:1122 |     48 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  mtc2 $s2, $v15.e5                                  ## L:1217 |     49 | screenMax.y = extMax; screenMax.Y = extMax;
  srl $t4, $s1, 12                                   ## L:1219 |     50 | temp1 = extMin >> 12;
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:1128 |     51 | u16 buffRdp = RDP_BUFF;
  mtc2 $t4, $v16.e4                                  ## L:1220 |     52 | screenMin.x = temp1;  screenMin.X = temp1;
  vmadn $v20, $v00, $v00                             ## L:1112 |      ^ | screenSize >>= 4;
  mtc2 $t4, $v16.e0                                  ## L:1220 |     53 | screenMin.x = temp1;  screenMin.X = temp1;
  vand $v15, $v15, $v15.e3                           ## L:1223 |      ^ | screenMax &= screenMax.w;
  llv $v13, 8, 0, $at                                ## L:1123 |     54 | globalPartSize.XY = load(PARTICLE_SCALE).xy;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1101 |     55 | dmaDmem = RDP_BUFF;
  ori $s3, $zero, %lo(RDP_BUFF)                      ## L:1102 |     56 | u16<$s3> dmaDmemEnd = RDP_BUFF;
  addiu $s5, $s3, 480                                ## L:1103 |     57 | u16 dmaDmemFlush = dmaDmemEnd + 480;
  addiu $s1, $s5, 32                                 ## L:1129 |     58 | u16 buffRdpEnd = dmaDmemFlush + 32;
  vand $v16, $v16, $v15.e3                           ## L:1224 |      ^ | screenMin &= screenMax.w;
  LABEL_TPXCmd_DrawTextured_0013:
  sh $zero, 14($s2)                                  ## L:1135 |     59 | store(ZERO:u16, buffRdp, 0x0E);
  sb $a0, 8($s2)                                     ## L:1134 |     60 | store(cmdRdpDepth, buffRdp, 8);
  sw $t7, 0($s2)                                     ## L:1133 |     61 | store(cmdRdpColor, buffRdp, 0);
  addiu $s2, $s2, 32                                 ## L:1136 |     62 | buffRdp += 32;
  bne $s2, $s1, LABEL_TPXCmd_DrawTextured_0013       ## L:1136 |     63 | buffRdp += 32;
  nop                                                ## L:1136 |    *65 | buffRdp += 32;
  LABEL_TPXCmd_DrawTextured_0014:
  lhu $s1, %lo(TILE_COUNT + 0)                       ## L:1147 |     66 | u16 tiles = load(TILE_COUNT);
  vxor $v12, $v00, $v00.e0                           ## L:1143 |      ^ | vec16 texMirrorMask = 0;
  vmov $v12.e5, $v30.e7                              ## L:1150 |     67 | texMirrorMask.Y = 1;
  vmov $v12.e1, $v30.e7                              ## L:1151 |    *69 | texMirrorMask.y = 1;
  addu $fp, $s1, $s1                                 ## L:1148 |      ^ | u16 tilesEnd = tiles + tiles;
  mtc2 $s1, $v12.e2                                  ## L:1152 |     70 | texMirrorMask.z = tiles;
  mtc2 $fp, $v12.e3                                  ## L:1153 |     71 | texMirrorMask.w = tilesEnd;
  addiu $fp, $fp, 65535                              ## L:1155 |     72 | tilesEnd -= 1;
  vmov $v12.e7, $v31.e7                              ## L:1158 |      ^ | texMirrorMask.W = 0x100;
  mtc2 $fp, $v12.e4                                  ## L:1157 |    *74 | texMirrorMask.X = tilesEnd;
  mtc2 $fp, $v12.e0                                  ## L:1156 |     75 | texMirrorMask.x = tilesEnd;
  vxor $v11, $v00, $v00.e0                           ## L:1144 |      ^ | vec16 texMirrorCompare = 0;
  vmudn $v12, $v12, $v12.e7                          ## L:1159 |  ***79 | texMirrorMask = texMirrorMask * texMirrorMask.W;
  vsubc $v29, $v12, $v30.e7                          ## L:1161 |  ***83 | VTEMP = texMirrorMask - 1;
  lh $s2, %lo(TEX_OFFSET + 0)                        ## L:1141 |      ^ | s16 texOffset = load(TEX_OFFSET);
  vmov $v12.e3, $v29.e3                              ## L:1162 |  ***87 | texMirrorMask.w = VTEMP.w;
  jal DMAWaitIdle                                    ## L:1166 |      ^ | dma_await();
  vor $v11, $v00, $v29.e2                            ## L:1163 |    *89 | texMirrorCompare = VTEMP.z;
  bgez $a1, LABEL_TPXCmd_DrawTextured_0015           ## L:1171 |     90 | if(rdramAddr < 0) {
  nop                                                ## L:1171 |    *92 | if(rdramAddr < 0) {
  lqv $v08, 0, 0, $s6                                ## L:606  |     93 | vec16 posStart = load(ptrIn, 0x00);
  vxor $v07, $v00, $v30.e7                           ## L:614  |      ^ | const vec16 vecOne = 1;
  vmulf $v14, $v13, $v08.h3                          ## L:607  |  ***97 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudn $v10, $v28, $v08.h0                          ## L:92   |     98 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |     99 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    100 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    101 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    102 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    103 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    104 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:95   |    105 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:619  | ***109 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:619  |    110 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:619  |    111 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0017:
  vmudl $v10, $v10, $v17.v                           ## L:630  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v09, $v09, $v17.v                           ## L:630  |    112 | posClip *= normScaleW:ufract;
  lb $a2, 7($s6)                                     ## L:674  |      ^ | temp0:s8 = load(ptrIn, 7);
  addiu $t2, $zero, 36                               ## L:737  |    113 | cmdRdpRect = 0x24;
  vmadn $v10, $v00, $v00                             ## L:630  |      ^ | posClip *= normScaleW:ufract;
  addu $a2, $a2, $s2                                 ## L:680  |   *115 | temp0 += texOffset; temp0 <<= 3;
  andi $t5, $t4, 16448                               ## L:627  |    116 | clipB = temp1 & 0b0100'0000'0100'0000;
  vrcph $v05.e3, $v09.e3                             ## L:632  |      ^ | invW.w = invert_half(posClip).w;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:636  |    117 | posClip:sint.w = load(BASE_SIZE).x;
  vrcpl $v06.e3, $v10.e3                             ## L:632  |      ^ | invW.w = invert_half(posClip).w;
  sll $a2, $a2, 3                                    ## L:680  |    118 | temp0 += texOffset; temp0 <<= 3;
  vrcph $v05.e3, $v09.e7                             ## L:633  |      ^ | invW.W = invert_half(posClip).W;
  vrcpl $v06.e7, $v10.e7                             ## L:633  |    119 | invW.W = invert_half(posClip).W;
  vmov $v10.e3, $v00.e0                              ## L:638  |    120 | posClip:sfract.w = 0;
  lsv $v09, 6, 0, $at                                ## L:636  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  lsv $v09, 14, 0, $at                               ## L:637  |    121 | posClip:sint.W = load(BASE_SIZE).x;
  vmov $v10.e7, $v00.e0                              ## L:639  |      ^ | posClip:sfract.W = 0;
  addiu $at, $zero, 3                                ## L:771  |    122 | if(clipA == 0b0000'0011) {
  vrcph $v05.e7, $v00.e7                             ## L:633  |      ^ | invW.W = invert_half(posClip).W;
  vmudl $v29, $v10, $v06.h3                          ## L:642  |  **125 | posClip *= invW.wwwwWWWW;
  andi $t1, $t4, 1028                                ## L:626  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  lb $t4, 15($s6)                                    ## L:675  |    126 | temp1:s8 = load(ptrIn, 15);
  vmadm $v29, $v09, $v06.h3                          ## L:642  |      ^ | posClip *= invW.wwwwWWWW;
  vmadn $v10, $v10, $v05.h3                          ## L:642  |    127 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:642  |    128 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:645  | ***132 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:648  |    133 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:648  |    134 | vec32 posScreen = posClip * screenSize;
  vmadn $v04, $v10, $v19.v                           ## L:648  |    135 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:648  |    136 | vec32 posScreen = posClip * screenSize;
  vmadh $v02, $v18, $v07.v                           ## L:649  |    137 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v04, $v07, $v14.v                           ## L:654  |    138 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v08, $v02, $v14.v                           ## L:655  |  **141 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:659  |    142 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:660  | ***146 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:660  |    147 | invW.x = invert_half(localPartSize).x;
  vrcp $v06.e1, $v14.e1                              ## L:661  |    148 | invW.y = invert_half(localPartSize).y;
  vrcph $v05.e1, $v14.e1                             ## L:661  |    149 | invW.y = invert_half(localPartSize).y;
  addu $t4, $t4, $s2                                 ## L:681  |      ^ | temp1 += texOffset; temp1 <<= 3;
  vrcp $v06.e4, $v14.e4                              ## L:662  |    150 | invW.X = invert_half(localPartSize).X;
  vrcph $v05.e4, $v14.e4                             ## L:662  |    151 | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:663  |    152 | invW.Y = invert_half(localPartSize).Y;
  vrcph $v05.e5, $v14.e5                             ## L:663  |    153 | invW.Y = invert_half(localPartSize).Y;
  vmudh $v01, $v08, $v05.v                           ## L:668  | ***157 | vec16 uvStart = posStart * invW:sint;
  vor $v03, $v00, $v05                               ## L:664  |    158 | vec16 uvDelta = invW:sint;
  sll $t4, $t4, 3                                    ## L:681  |      ^ | temp1 += texOffset; temp1 <<= 3;
  vmudm $v01, $v01, $v31.e6                          ## L:669  |  **161 | uvStart >>= 7;
  vxor $v05, $v00, $v00.e0                           ## L:694  |    162 | vec16 texOffsetTotal = 0;
  mtc2 $t4, $v05.e4                                  ## L:696  |  **165 | texOffsetTotal.X = temp1:s16;
  vlt $v01, $v01, $v00.e0                            ## L:670  |      ^ | uvStart = uvStart < 0;
  mtc2 $a2, $v05.e0                                  ## L:695  |    166 | texOffsetTotal.x = temp0:s16;
  slv $v03, 0, 28, $s3                               ## L:758  |  **169 | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vsubc $v06, $v00, $v01.v                           ## L:692  |      ^ | vec16 uvStartNeg = VZERO - uvStart;
  vand $v05, $v05, $v12.e3                           ## L:697  |    170 | texOffsetTotal &= texMirrorMask.w;
  vge $v29, $v11, $v05.h0                            ## L:700  | ***174 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vmrg $v01, $v06, $v01                              ## L:700  |    175 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vlt $v04, $v04, $v15                               ## L:703  |    176 | posEnd = min(posEnd, screenMax);
  vge $v08, $v08, $v16                               ## L:704  |    177 | posStart = max(posStart, screenMin);
  ssv $v02, 4, 12, $s3                               ## L:749  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  mfc2 $s1, $v08.e1                                  ## L:1190 | ***181 | outA = pos.y;
  vaddc $v01, $v01, $v05.v                           ## L:706  |      ^ | uvStart += texOffsetTotal;
  mfc2 $k0, $v08.e0                                  ## L:1192 |    182 | u32 tmp = pos.x;
  andi $s1, $s1, 4095                                ## L:1191 |   *184 | outA &= 0b1111'1111'1111;
  mfc2 $fp, $v08.e5                                  ## L:1196 |    185 | outB = pos.Y;
  sll $k0, $k0, 12                                   ## L:1193 |    186 | tmp <<= 12;
  vsubc $v06, $v12, $v01.v                           ## L:711  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  or $s1, $s1, $k0                                   ## L:1194 |    187 | outA |= tmp;
  mfc2 $k0, $v08.e4                                  ## L:1198 |    188 | tmp = pos.X;
  mfc2 $sp, $v04.e1                                  ## L:1190 |    189 | outA = pos.y;
  andi $fp, $fp, 4095                                ## L:1197 |    190 | outB &= 0b1111'1111'1111;
  vaddc $v06, $v06, $v12.e2                          ## L:712  |      ^ | uvStartNeg += texMirrorMask.z;
  sll $k0, $k0, 12                                   ## L:1199 |    191 | tmp <<= 12;
  or $fp, $fp, $k0                                   ## L:1200 |    192 | outB |= tmp;
  mfc2 $k0, $v04.e0                                  ## L:1192 |    193 | u32 tmp = pos.x;
  vge $v29, $v11, $v05.h0                            ## L:714  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  vmrg $v01, $v01, $v06                              ## L:714  |    194 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  mfc2 $k1, $v04.e5                                  ## L:1196 |      ^ | outB = pos.Y;
  vlt $v06, $v08, $v04                               ## L:721  |    195 | vec16 extend = posStart < posEnd;
  andi $sp, $sp, 4095                                ## L:1191 |      ^ | outA &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1193 |    196 | tmp <<= 12;
  ldv $v05, 0, 16, $s6                               ## L:732  |    197 | vec16 color = load(ptrIn, 16).xyzw;
  sw $s1, 4 + 16($s3)                                ## L:754  |    198 | store(posA, dmaDmemEnd, 0x04, 16);
  andi $k1, $k1, 4095                                ## L:1197 |    199 | outB &= 0b1111'1111'1111;
  slv $v01, 0, 24, $s3                               ## L:757  |    200 | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  or $sp, $sp, $k0                                   ## L:1194 |    201 | outA |= tmp;
  sw $sp, 0 + 16($s3)                                ## L:755  |    202 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  cfc2 $t4, $vcc                                     ## L:722  |    203 | temp1 = get_vcc();
  mfc2 $k0, $v04.e4                                  ## L:1198 |    204 | tmp = pos.X;
  ldv $v08, 0, 24, $s6                               ## L:762  |    205 | posStart.xyzw = load(ptrIn, 24).xyzw;
  andi $a2, $t4, 3                                   ## L:724  |    206 | temp0 = temp1 & 0b0000'0011;
  ldv $v08, 8, 32, $s6                               ## L:763  |    207 | posStart.XYZW = load(ptrIn, 24).XYZW;
  sll $k0, $k0, 12                                   ## L:1199 |    208 | tmp <<= 12;
  or $k1, $k1, $k0                                   ## L:1200 |    209 | outB |= tmp;
  slv $v05, 0, 4, $s3                                ## L:745  |    210 | store(color.xy, dmaDmemEnd, 4);
  or $t1, $t1, $a2                                   ## L:725  |    211 | clipA |= temp0;
  vmulf $v14, $v13, $v08.h3                          ## L:764  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  andi $a2, $t4, 48                                  ## L:727  |    212 | temp0 = temp1 & 0b0011'0000;
  or $t5, $t5, $a2                                   ## L:728  |    213 | clipB |= temp0;
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_0019       ## L:771  |    214 | if(clipA == 0b0000'0011) {
  sb $t2, 0 + 16($s3)                                ## L:756  |   *216 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  addiu $s3, $s3, 32                                 ## L:772  |    217 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_0019:
  addiu $at, $zero, 48                               ## L:782  |    218 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001A       ## L:782  |    219 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 24                                 ## L:776  |   *221 | ptrIn += 24;
  sw $k1, 0 + 16($s3)                                ## L:787  |    222 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v01, 8, 24, $s3                               ## L:790  |    223 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  sw $fp, 4 + 16($s3)                                ## L:786  |    224 | store(posB, dmaDmemEnd, 0x04, 16);
  slv $v03, 8, 28, $s3                               ## L:791  |    225 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  sb $t2, 0 + 16($s3)                                ## L:788  |    226 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v05, 4, 4, $s3                                ## L:783  |    227 | store(color.zw, dmaDmemEnd, 4);
  ssv $v02, 12, 12, $s3                              ## L:784  |    228 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  addiu $s3, $s3, 32                                 ## L:793  |    229 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001A:
  vmudn $v10, $v28, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |    230 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    231 | out = mat1 +* vec.yyyyYYYY;
  sltu $at, $s3, $s5                                 ## L:800  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    232 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    233 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    234 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    235 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_001B     ## L:800  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:95   |   *237 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:801  |    238 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:801  |   *240 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:802  |    241 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_001B:
  vch $v29, $v09, $v09.h3                            ## L:805  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:805  |    242 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_0017       ## L:805  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:805  |   *244 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0018:
  beq $zero, $zero, LABEL_TPXCmd_DrawTextured_0016   ## L:805  |    245 | temp1 = clip(posClip, posClip.wwwwWWWW);
  nop                                                ## L:805  |   *247 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0015:
  vxor $v07, $v00, $v30.e7                           ## L:261  |    248 | const vec16 vecOne = 1;
  lpv $v08, 0, 0, $s6                                ## L:256  |      ^ | vec16 posStart = load_vec_s8(ptrIn, 0x00);
  vmulf $v14, $v13, $v08.h3                          ## L:257  | ***252 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v08, $v08, $v31.e7                          ## L:258  |    253 | posStart >>= 8;
  vmudn $v10, $v28, $v08.h0                          ## L:92   | ***257 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:92   |    258 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    259 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    260 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    261 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    262 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    263 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:95   |    264 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:266  | ***268 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:266  |    269 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:266  |    270 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_001C:
  vmudl $v10, $v10, $v17.v                           ## L:277  |      ^ | posClip *= normScaleW:ufract;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:283  |    271 | posClip:sint.w = load(BASE_SIZE).x;
  vmadm $v09, $v09, $v17.v                           ## L:277  |      ^ | posClip *= normScaleW:ufract;
  lb $a2, 11($s6)                                    ## L:324  |    272 | temp0:s8 = load(ptrIn, 0x0B);
  vmadn $v10, $v00, $v00                             ## L:277  |      ^ | posClip *= normScaleW:ufract;
  andi $t5, $t4, 16448                               ## L:274  |    273 | clipB = temp1 & 0b0100'0000'0100'0000;
  addu $a2, $a2, $s2                                 ## L:327  |   *275 | temp0 += texOffset; temp0 <<= 3;
  vrcph $v05.e3, $v09.e3                             ## L:279  |      ^ | invW.w = invert_half(posClip).w;
  andi $t1, $t4, 1028                                ## L:273  |    276 | clipA = temp1 & 0b0000'0100'0000'0100;
  vrcpl $v06.e3, $v10.e3                             ## L:279  |      ^ | invW.w = invert_half(posClip).w;
  sll $a2, $a2, 3                                    ## L:327  |    277 | temp0 += texOffset; temp0 <<= 3;
  vrcph $v05.e3, $v09.e7                             ## L:280  |      ^ | invW.W = invert_half(posClip).W;
  vrcpl $v06.e7, $v10.e7                             ## L:280  |    278 | invW.W = invert_half(posClip).W;
  lb $t4, 15($s6)                                    ## L:325  |      ^ | temp1:s8 = load(ptrIn, 0x0F);
  vmov $v10.e3, $v00.e0                              ## L:285  |    279 | posClip:sfract.w = 0;
  lsv $v09, 6, 0, $at                                ## L:283  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vmov $v10.e7, $v00.e0                              ## L:286  |    280 | posClip:sfract.W = 0;
  lsv $v09, 14, 0, $at                               ## L:284  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  vrcph $v05.e7, $v00.e7                             ## L:280  |    281 | invW.W = invert_half(posClip).W;
  addu $t4, $t4, $s2                                 ## L:328  |      ^ | temp1 += texOffset; temp1 <<= 3;
  sll $t4, $t4, 3                                    ## L:328  |  **284 | temp1 += texOffset; temp1 <<= 3;
  vmudl $v29, $v10, $v06.h3                          ## L:289  |      ^ | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v09, $v06.h3                          ## L:289  |    285 | posClip *= invW.wwwwWWWW;
  vmadn $v10, $v10, $v05.h3                          ## L:289  |    286 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:289  |    287 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:292  | ***291 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:295  |    292 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:295  |    293 | vec32 posScreen = posClip * screenSize;
  vmadn $v04, $v10, $v19.v                           ## L:295  |    294 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:295  |    295 | vec32 posScreen = posClip * screenSize;
  vmadh $v02, $v18, $v07.v                           ## L:296  |    296 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v04, $v07, $v14.v                           ## L:301  |    297 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v08, $v02, $v14.v                           ## L:302  |  **300 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:306  |    301 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:307  | ***305 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:307  |    306 | invW.x = invert_half(localPartSize).x;
  vrcp $v06.e1, $v14.e1                              ## L:308  |    307 | invW.y = invert_half(localPartSize).y;
  vrcph $v05.e1, $v14.e1                             ## L:308  |    308 | invW.y = invert_half(localPartSize).y;
  vrcp $v06.e4, $v14.e4                              ## L:309  |    309 | invW.X = invert_half(localPartSize).X;
  vrcph $v05.e4, $v14.e4                             ## L:309  |    310 | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:310  |    311 | invW.Y = invert_half(localPartSize).Y;
  vrcph $v05.e5, $v14.e5                             ## L:310  |    312 | invW.Y = invert_half(localPartSize).Y;
  vmudh $v01, $v08, $v05.v                           ## L:315  | ***316 | vec16 uvStart = posStart * invW:sint;
  vor $v03, $v00, $v05                               ## L:311  |    317 | vec16 uvDelta = invW:sint;
  vxor $v05, $v00, $v00.e0                           ## L:341  |    318 | vec16 texOffsetTotal = 0;
  vmudm $v01, $v01, $v31.e6                          ## L:316  |   *320 | uvStart >>= 7;
  mtc2 $a2, $v05.e0                                  ## L:342  |      ^ | texOffsetTotal.x = temp0:s16;
  vlt $v01, $v01, $v00.e0                            ## L:317  | ***324 | uvStart = uvStart < 0;
  mtc2 $t4, $v05.e4                                  ## L:343  |      ^ | texOffsetTotal.X = temp1:s16;
  ssv $v02, 4, 12, $s3                               ## L:396  |    325 | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  slv $v03, 0, 28, $s3                               ## L:405  |  **328 | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vsubc $v06, $v00, $v01.v                           ## L:339  |      ^ | vec16 uvStartNeg = VZERO - uvStart;
  vand $v05, $v05, $v12.e3                           ## L:344  |    329 | texOffsetTotal &= texMirrorMask.w;
  vge $v29, $v11, $v05.h0                            ## L:347  | ***333 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vmrg $v01, $v06, $v01                              ## L:347  |    334 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  addiu $at, $zero, 3                                ## L:418  |      ^ | if(clipA == 0b0000'0011) {
  vlt $v04, $v04, $v15                               ## L:350  |    335 | posEnd = min(posEnd, screenMax);
  vge $v08, $v08, $v16                               ## L:351  |    336 | posStart = max(posStart, screenMin);
  addiu $t2, $zero, 36                               ## L:384  |      ^ | cmdRdpRect = 0x24;
  vaddc $v01, $v01, $v05.v                           ## L:353  |  **339 | uvStart += texOffsetTotal;
  mfc2 $sp, $v04.e1                                  ## L:1190 |      ^ | outA = pos.y;
  mfc2 $s1, $v08.e1                                  ## L:1190 |    340 | outA = pos.y;
  mfc2 $k0, $v08.e0                                  ## L:1192 |  **343 | u32 tmp = pos.x;
  mfc2 $fp, $v08.e5                                  ## L:1196 |    344 | outB = pos.Y;
  andi $s1, $s1, 4095                                ## L:1191 |    345 | outA &= 0b1111'1111'1111;
  vsubc $v06, $v12, $v01.v                           ## L:358  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  sll $k0, $k0, 12                                   ## L:1193 |    346 | tmp <<= 12;
  or $s1, $s1, $k0                                   ## L:1194 |    347 | outA |= tmp;
  mfc2 $k0, $v08.e4                                  ## L:1198 |    348 | tmp = pos.X;
  andi $sp, $sp, 4095                                ## L:1191 |    349 | outA &= 0b1111'1111'1111;
  vaddc $v06, $v06, $v12.e2                          ## L:359  |      ^ | uvStartNeg += texMirrorMask.z;
  andi $fp, $fp, 4095                                ## L:1197 |    350 | outB &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1199 |    351 | tmp <<= 12;
  or $fp, $fp, $k0                                   ## L:1200 |    352 | outB |= tmp;
  mfc2 $k0, $v04.e0                                  ## L:1192 |    353 | u32 tmp = pos.x;
  vge $v29, $v11, $v05.h0                            ## L:361  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  vmrg $v01, $v01, $v06                              ## L:361  |    354 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  mfc2 $k1, $v04.e5                                  ## L:1196 |      ^ | outB = pos.Y;
  vlt $v06, $v08, $v04                               ## L:368  |    355 | vec16 extend = posStart < posEnd;
  ldv $v05, 0, 8, $s6                                ## L:381  |      ^ | vec16 color = load(ptrIn, 8).xyzw;
  sll $k0, $k0, 12                                   ## L:1193 |    356 | tmp <<= 12;
  or $sp, $sp, $k0                                   ## L:1194 |    357 | outA |= tmp;
  cfc2 $t4, $vcc                                     ## L:369  |    358 | temp1 = get_vcc();
  mfc2 $k0, $v04.e4                                  ## L:1198 |    359 | tmp = pos.X;
  lpv $v08, 0, 16, $s6                               ## L:413  |    360 | posStart = load_vec_s8(ptrIn, 16);
  andi $a2, $t4, 3                                   ## L:371  |    361 | temp0 = temp1 & 0b0000'0011;
  or $t1, $t1, $a2                                   ## L:372  |    362 | clipA |= temp0;
  slv $v05, 0, 4, $s3                                ## L:392  |    363 | store(color.xy, dmaDmemEnd, 4);
  sw $sp, 0 + 16($s3)                                ## L:402  |    364 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:403  |    365 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sw $s1, 4 + 16($s3)                                ## L:401  |    366 | store(posA, dmaDmemEnd, 0x04, 16);
  andi $k1, $k1, 4095                                ## L:1197 |    367 | outB &= 0b1111'1111'1111;
  andi $a2, $t4, 48                                  ## L:374  |    368 | temp0 = temp1 & 0b0011'0000;
  sll $k0, $k0, 12                                   ## L:1199 |    369 | tmp <<= 12;
  vmulf $v14, $v13, $v08.h3                          ## L:414  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v08, $v08, $v31.e7                          ## L:415  |    370 | posStart >>= 8;
  slv $v01, 0, 24, $s3                               ## L:404  |      ^ | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  or $t5, $t5, $a2                                   ## L:375  |    371 | clipB |= temp0;
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_001E       ## L:418  |    372 | if(clipA == 0b0000'0011) {
  or $k1, $k1, $k0                                   ## L:1200 |   *374 | outB |= tmp;
  addiu $s3, $s3, 32                                 ## L:419  |    375 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001E:
  addiu $at, $zero, 48                               ## L:429  |    376 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001F       ## L:429  |    377 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 16                                 ## L:425  |   *379 | ptrIn += 16;
  sw $fp, 4 + 16($s3)                                ## L:433  |    380 | store(posB, dmaDmemEnd, 0x04, 16);
  sw $k1, 0 + 16($s3)                                ## L:434  |    381 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v03, 8, 28, $s3                               ## L:438  |    382 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  slv $v05, 4, 4, $s3                                ## L:430  |    383 | store(color.zw, dmaDmemEnd, 4);
  sb $t2, 0 + 16($s3)                                ## L:435  |    384 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  ssv $v02, 12, 12, $s3                              ## L:431  |    385 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  slv $v01, 8, 24, $s3                               ## L:437  |    386 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  addiu $s3, $s3, 32                                 ## L:440  |    387 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001F:
  vmudn $v10, $v28, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  sltu $at, $s3, $s5                                 ## L:447  |    388 | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v27, $v08.h0                          ## L:92   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:93   |    389 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:93   |    390 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:94   |    391 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:94   |    392 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:95   |    393 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_0020     ## L:447  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:95   |   *395 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:448  |    396 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:448  |   *398 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:449  |    399 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_0020:
  vch $v29, $v09, $v09.h3                            ## L:452  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:452  |    400 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_001C       ## L:452  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:452  |   *402 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0016:
  j RDPQ_Send                                        ## L:1179 |    403 | goto RDPQ_Send;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:1178 |   *405 | RA = RSPQ_Loop;

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