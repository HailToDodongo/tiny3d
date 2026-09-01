## Auto-generated file, transpiled with RSPL
#define RDP_CMD_PRIM 0x3A00'0000
#define RDP_CMD_DEPTH 0x2E
#define RDP_CMD_RECT 0x36
#define RDP_CMD_TEX_RECT 0x24
#define RDP_CMD_TEX_RECT_FLIP 0x25
#define PARTICLE_INPUT_SIZE_S8 16
#define PARTICLE_INPUT_SIZE_S16 24
#define PARTICLE_MAX_COUNT 344
#define RDP_FLUSH_SIZE 528
#define RDP_TEX_FLUSH_SIZE 480
#define RDP_POS_MASK 0b1111'1111'1111
#define LOOP_16BIT 1
#define MATRIX_TEMP_MUL PARTICLE_BUFF
#define MATRIX_MV RDP_BUFF
#define MATRIX_SIZE 0x40
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
    .align 3
    SCREEN_SCALE: .half 0, 0, 0, 0, 0, 0, 0, 0
    .align 1
    SCREEN_OFFSET: .half 0, 0, 16383, 255
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
  ## hot-path cycles: 25
  ori $t0, $zero, 127                                ## L:818  |      ^ | dma_in_async(MATRIX_PROJ, rdramMatrix, 128);
  ori $s4, $zero, %lo(MATRIX_PROJ)                   ## L:818  |      2 | dma_in_async(MATRIX_PROJ, rdramMatrix, 128);
  or $t2, $zero, $zero                               ## L:818  |      3 | dma_in_async(MATRIX_PROJ, rdramMatrix, 128);
  jal DMAExec                                        ## L:818  |      4 | dma_in_async(MATRIX_PROJ, rdramMatrix, 128); ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a0                                 ## L:818  |      5 | dma_in_async(MATRIX_PROJ, rdramMatrix, 128);
  addiu $a0, $a0, 128                                ## L:820  |      6 | rdramMatrix += 128;
  ori $t0, $zero, 63                                 ## L:821  |      7 | dma_in_async(RDP_BUFF, rdramMatrix, 64);
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:821  |      8 | dma_in_async(RDP_BUFF, rdramMatrix, 64);
  or $s0, $zero, $a0                                 ## L:821  |      9 | dma_in_async(RDP_BUFF, rdramMatrix, 64);
  jal DMAExec                                        ## L:821  |     10 | dma_in_async(RDP_BUFF, rdramMatrix, 64); ## Args: $t0, $t1, $s0, $s4, $t2
  or $t2, $zero, $zero                               ## L:821  |     11 | dma_in_async(RDP_BUFF, rdramMatrix, 64);
  ori $t0, $zero, 63                                 ## L:823  |     12 | dma_out_async(RDP_BUFF, addrRDRAM, 64);
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:823  |     13 | dma_out_async(RDP_BUFF, addrRDRAM, 64);
  addiu $t2, $zero, -32768                           ## L:823  |     14 | dma_out_async(RDP_BUFF, addrRDRAM, 64);
  jal DMAExec                                        ## L:823  |     15 | dma_out_async(RDP_BUFF, addrRDRAM, 64); ## Args: $t0, $t1, $s0, $s4, $t2
  lw $s0, %lo(MATRIX_STACK_PTR + 0)                  ## L:822  |     16 | u32<$s0> addrRDRAM = load(MATRIX_STACK_PTR);
  sh $a2, %lo(6 + NORM_SCALE_W)($zero)               ## L:825  |     17 | store(wNorm, NORM_SCALE_W, 6);
  or $s0, $zero, $a1                                 ## L:829  |     18 | dma_in(SCREEN_SCALE, rdramScreen, 24);
  ori $s4, $zero, %lo(SCREEN_SCALE)                  ## L:829  |     19 | dma_in(SCREEN_SCALE, rdramScreen, 24);
  addiu $t2, $zero, 12                               ## L:829  |     20 | dma_in(SCREEN_SCALE, rdramScreen, 24);
  jal DMAExec                                        ## L:829  |     21 | dma_in(SCREEN_SCALE, rdramScreen, 24); ## Args: $t0, $t1, $s0, $s4, $t2
  ori $t0, $zero, 23                                 ## L:829  |     22 | dma_in(SCREEN_SCALE, rdramScreen, 24);
  j RSPQ_Loop                                        ## L:830  |     23 | }
  nop                                                ## L:830  |    *25 | }
TPXCmd_DrawColor:
  ## hot-path cycles: 186
  andi $t0, $a0, 65535                               ## L:835  |      ^ | u32<$t0> dmaSize = dataSize & 0xFFFF;
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:834  |      2 | u16<$s4> dmaDmem = PARTICLE_BUFF;
  or $s0, $zero, $a1                                 ## L:851  |      3 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  addu $s7, $s4, $t0                                 ## L:850  |      4 | u32 ptrInEnd = dmaDmem + dmaSize;
  addiu $t0, $t0, -1                                 ## L:851  |      5 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  jal DMAExec                                        ## L:851  |      6 | dma_in_async(dmaDmem, rdramAddr, dmaSize); ## Args: $t0, $t1, $s0, $s4, $t2
  or $t2, $zero, $zero                               ## L:851  |      7 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  ori $at, $zero, %lo(MATRIX_MVP)                    ## L:859  |      8 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  or $s6, $zero, $s4                                 ## L:853  |      9 | u32 ptrIn = dmaDmem;
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)                 ## L:1210 |     10 | u32 extMax = load(RDPQ_SCISSOR_RECT, 4);
  ldv $v26, 0, 24, $at                               ## L:860  |     11 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v24, 8, 40, $at                               ## L:861  |     12 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v26, 8, 24, $at                               ## L:860  |     13 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  srl $t4, $s2, 12                                   ## L:1213 |     14 | temp1 = extMax >> 12;
  ldv $v27, 8, 0, $at                                ## L:859  |     15 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)                 ## L:1211 |     16 | u32 extMin = load(RDPQ_SCISSOR_RECT, 0);
  ori $s3, $zero, %lo(RDP_BUFF)                      ## L:856  |     17 | u16<$s3> dmaDmemEnd = RDP_BUFF;
  ldv $v28, 0, 8, $at                                ## L:859  |     18 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v21, 8, 48, $at                               ## L:862  |     19 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v22, 8, 56, $at                               ## L:862  |     20 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v28, 8, 8, $at                                ## L:859  |     21 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v23, 8, 32, $at                               ## L:861  |     22 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v25, 8, 16, $at                               ## L:860  |     23 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v21, 0, 48, $at                               ## L:862  |     24 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:859  |     25 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v25, 0, 16, $at                               ## L:860  |     26 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:861  |     27 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v23, 0, 32, $at                               ## L:861  |     28 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:855  |     29 | dmaDmem = RDP_BUFF;
  ldv $v22, 0, 56, $at                               ## L:862  |     30 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  mtc2 $t4, $v15.e4                                  ## L:1214 |     31 | screenMax.x = temp1;  screenMax.X = temp1;
  ori $at, $zero, %lo(SCREEN_SCALE)                  ## L:864  |     32 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v20, 8, 8, $at                                ## L:864  |     33 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  mtc2 $t4, $v15.e0                                  ## L:1214 |     34 | screenMax.x = temp1;  screenMax.X = temp1;
  ldv $v19, 0, 0, $at                                ## L:864  |     35 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v20, 0, 8, $at                                ## L:864  |     36 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  addiu $a0, $zero, 46                               ## L:883  |     37 | cmdRdpDepth = 0x2E;
  ldv $v19, 8, 0, $at                                ## L:864  |     38 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  mtc2 $s1, $v16.e1                                  ## L:1219 |     39 | screenMin.y = extMin; screenMin.Y = extMin;
  ori $at, $zero, %lo(SCREEN_OFFSET)                 ## L:866  |     40 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v18, 8, 0, $at                                ## L:866  |     41 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  addiu $s5, $s3, 528                                ## L:857  |     42 | u16 dmaDmemFlush = dmaDmemEnd + 528;
  ldv $v18, 0, 0, $at                                ## L:866  |     43 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  mtc2 $s1, $v16.e5                                  ## L:1219 |     44 | screenMin.y = extMin; screenMin.Y = extMin;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:867  |     45 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v17, 8, 0, $at                                ## L:867  |     46 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v17, 0, 0, $at                                ## L:867  |     47 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  srl $t4, $s1, 12                                   ## L:1217 |     48 | temp1 = extMin >> 12;
  addiu $at, $zero, 4095                             ## L:1209 |     49 | screenMax.w = 0b1111'1111'1111;
  mtc2 $at, $v15.e3                                  ## L:1209 |     50 | screenMax.w = 0b1111'1111'1111;
  mtc2 $t4, $v16.e0                                  ## L:1218 |     51 | screenMin.x = temp1;  screenMin.X = temp1;
  ori $at, $zero, %lo(PARTICLE_SCALE)                ## L:874  |     52 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  addiu $s1, $s5, 24                                 ## L:881  |     53 | u16 buffRdpEnd = dmaDmemFlush + 24;
  mtc2 $s2, $v15.e1                                  ## L:1215 |     54 | screenMax.y = extMax; screenMax.Y = extMax;
  mtc2 $s2, $v15.e5                                  ## L:1215 |     55 | screenMax.y = extMax; screenMax.Y = extMax;
  lui $t7, 0x3A00                                    ## L:882  |     56 | cmdRdpColor = 0x3A00'0000;
  llv $v13, 8, 0, $at                                ## L:875  |     57 | globalPartSize.XY = load(PARTICLE_SCALE).xy;
  mtc2 $t4, $v16.e4                                  ## L:1218 |     58 | screenMin.x = temp1;  screenMin.X = temp1;
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:880  |     59 | u16 buffRdp = RDP_BUFF;
  llv $v13, 0, 0, $at                                ## L:874  |     60 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  vand $v15, $v15, $v15.e3                           ## L:1221 |      ^ | screenMax &= screenMax.w;
  vand $v16, $v16, $v15.e3                           ## L:1222 |  ***64 | screenMin &= screenMax.w;
  LABEL_TPXCmd_DrawColor_0001:
  sb $a0, 8($s2)                                     ## L:886  |      ^ | store(cmdRdpDepth, buffRdp, 8);
  addiu $s2, $s2, 24                                 ## L:888  |     65 | buffRdp += 24;
  sw $t7, -24($s2)                                   ## L:885  |     66 | store(cmdRdpColor, buffRdp, 0);
  bne $s2, $s1, LABEL_TPXCmd_DrawColor_0001          ## L:888  |     67 | buffRdp += 24;
  sh $zero, -10($s2)                                 ## L:887  |     68 | store(ZERO:u16, buffRdp, 0x0E);
  LABEL_TPXCmd_DrawColor_0002:
  jal DMAWaitIdle                                    ## L:892  |     69 | dma_await();
  nop                                                ## L:892  |     70 | dma_await();
  bgez $a1, LABEL_TPXCmd_DrawColor_0003              ## L:899  |     71 | if(rdramAddr < 0) {
  vxor $v10, $v00, $v30.e7                           ## L:896  |     72 | const vec16 vecOne = 1;
  lqv $v09, 0, 0, $s6                                ## L:467  |     73 | vec16 posStart = load(ptrIn, 0x00);
  vmulf $v14, $v13, $v09.h3                          ## L:468  |  ***77 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudn $v12, $v28, $v09.h0                          ## L:94   |     78 | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:94   |     79 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:95   |     80 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:95   |     81 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:96   |     82 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:96   |     83 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:97   |     84 | out = mat3 +* 1;
  vmadh $v11, $v21, $v30.e7                          ## L:97   |     85 | out = mat3 +* 1;
  vch $v29, $v11, $v11.h3                            ## L:476  |  ***89 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:476  |     90 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:476  |     91 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0005:
  vmudl $v12, $v12, $v17.v                           ## L:487  |      ^ | posClip *= normScaleW:ufract;
  addiu $t2, $zero, 54                               ## L:536  |     92 | cmdRdpRect = 0x36;
  vmadm $v11, $v11, $v17.v                           ## L:487  |      ^ | posClip *= normScaleW:ufract;
  vmadn $v12, $v00, $v00                             ## L:487  |     93 | posClip *= normScaleW:ufract;
  vrcph $v07.e3, $v11.e3                             ## L:489  |   **96 | invW.w = invert_half(posClip).w;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:493  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcpl $v08.e3, $v12.e3                             ## L:489  |     97 | invW.w = invert_half(posClip).w;
  vrcph $v07.e3, $v11.e7                             ## L:490  |     98 | invW.W = invert_half(posClip).W;
  lsv $v11, 6, 0, $at                                ## L:493  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcpl $v08.e7, $v12.e7                             ## L:490  |     99 | invW.W = invert_half(posClip).W;
  vmov $v12.e7, $v00.e0                              ## L:496  |    100 | posClip:sfract.W = 0;
  vmov $v12.e3, $v00.e0                              ## L:495  |    101 | posClip:sfract.w = 0;
  vrcph $v07.e7, $v00.e7                             ## L:490  |    102 | invW.W = invert_half(posClip).W;
  lsv $v11, 14, 0, $at                               ## L:494  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  vmudl $v29, $v12, $v08.h3                          ## L:499  |  **105 | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v11, $v08.h3                          ## L:499  |    106 | posClip *= invW.wwwwWWWW;
  vmadn $v12, $v12, $v07.h3                          ## L:499  |    107 | posClip *= invW.wwwwWWWW;
  vmadh $v11, $v11, $v07.h3                          ## L:499  |    108 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v11.h3                          ## L:503  | ***112 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v12, $v20.v                           ## L:506  |    113 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v11, $v20.v                           ## L:506  |    114 | vec32 posScreen = posClip * screenSize;
  vmadn $v08, $v12, $v19.v                           ## L:506  |    115 | vec32 posScreen = posClip * screenSize;
  addiu $at, $zero, 3                                ## L:568  |      ^ | if(clipA == 0b0000'0011) {
  ldv $v03, 0, 16, $s6                               ## L:532  |    116 | vec16 color = load(ptrIn, 16).xyzw;
  vmadh $v07, $v11, $v19.v                           ## L:506  |      ^ | vec32 posScreen = posClip * screenSize;
  vmadh $v06, $v18, $v10.v                           ## L:507  |    117 | vec16 posCenter = screenOffset:sint +* vecOne;
  andi $t1, $t4, 1028                                ## L:483  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  andi $t5, $t4, 16448                               ## L:484  |    118 | clipB = temp1 & 0b0100'0000'0100'0000;
  vmadh $v05, $v10, $v14.v                           ## L:510  |      ^ | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v09, $v06, $v14.v                           ## L:511  |  **121 | posStart = posCenter - localPartSize:sint;
  ssv $v06, 4, 12, $s3                               ## L:548  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  slv $v03, 0, 4, $s3                                ## L:544  |    122 | store(color.xy, dmaDmemEnd, 4);
  vlt $v05, $v05, $v15                               ## L:514  |      ^ | posEnd = min(posEnd, screenMax);
  mfc2 $sp, $v05.e5                                  ## L:1194 | ***126 | outB = pos.Y;
  vge $v09, $v09, $v16                               ## L:515  |      ^ | posStart = max(posStart, screenMin);
  mfc2 $fp, $v05.e1                                  ## L:1188 |    127 | outA = pos.y;
  andi $sp, $sp, 4095                                ## L:1195 |   *129 | outB &= 0b1111'1111'1111;
  mfc2 $s2, $v09.e1                                  ## L:1188 |    130 | outA = pos.y;
  mfc2 $k1, $v09.e0                                  ## L:1190 |    131 | u32 tmp = pos.x;
  andi $fp, $fp, 4095                                ## L:1189 |    132 | outA &= 0b1111'1111'1111;
  andi $s2, $s2, 4095                                ## L:1189 |    133 | outA &= 0b1111'1111'1111;
  mfc2 $s1, $v09.e5                                  ## L:1194 |    134 | outB = pos.Y;
  sll $k1, $k1, 12                                   ## L:1191 |    135 | tmp <<= 12;
  or $s2, $s2, $k1                                   ## L:1192 |    136 | outA |= tmp;
  mfc2 $k1, $v09.e4                                  ## L:1196 |    137 | tmp = pos.X;
  vlt $v04, $v09, $v05                               ## L:521  |      ^ | vec16 extend = posStart < posEnd;
  sw $s2, 4 + 16($s3)                                ## L:552  |    138 | store(posA, dmaDmemEnd, 0x04, 16);
  andi $s1, $s1, 4095                                ## L:1195 |    139 | outB &= 0b1111'1111'1111;
  cfc2 $t4, $vcc                                     ## L:522  |    140 | temp1 = get_vcc();
  sll $k1, $k1, 12                                   ## L:1197 |    141 | tmp <<= 12;
  or $s1, $s1, $k1                                   ## L:1198 |    142 | outB |= tmp;
  andi $a2, $t4, 3                                   ## L:524  |    143 | temp0 = temp1 & 0b0000'0011;
  ldv $v09, 0, 24, $s6                               ## L:559  |    144 | posStart.xyzw = load(ptrIn, 24).xyzw;
  mfc2 $k1, $v05.e0                                  ## L:1190 |    145 | u32 tmp = pos.x;
  or $t1, $t1, $a2                                   ## L:525  |    146 | clipA |= temp0;
  ldv $v09, 8, 32, $s6                               ## L:560  |    147 | posStart.XYZW = load(ptrIn, 24).XYZW;
  sll $k1, $k1, 12                                   ## L:1191 |    148 | tmp <<= 12;
  or $fp, $fp, $k1                                   ## L:1192 |    149 | outA |= tmp;
  mfc2 $k1, $v05.e4                                  ## L:1196 |    150 | tmp = pos.X;
  andi $a2, $t4, 48                                  ## L:527  |    151 | temp0 = temp1 & 0b0011'0000;
  or $t5, $t5, $a2                                   ## L:528  |    152 | clipB |= temp0;
  sll $k1, $k1, 12                                   ## L:1197 |    153 | tmp <<= 12;
  vmulf $v14, $v13, $v09.h3                          ## L:561  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  sw $fp, 0 + 16($s3)                                ## L:553  |    154 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  or $sp, $sp, $k1                                   ## L:1198 |    155 | outB |= tmp;
  bne $t1, $at, LABEL_TPXCmd_DrawColor_0007          ## L:568  |    156 | if(clipA == 0b0000'0011) {
  sb $t2, 0 + 16($s3)                                ## L:554  |    157 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  addiu $s3, $s3, 24                                 ## L:569  |    158 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_0007:
  addiu $at, $zero, 48                               ## L:579  |    159 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawColor_0008          ## L:579  |    160 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 24                                 ## L:573  |    161 | ptrIn += 24;
  sw $sp, 0 + 16($s3)                                ## L:584  |    162 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:585  |    163 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v03, 4, 4, $s3                                ## L:580  |    164 | store(color.zw, dmaDmemEnd, 4);
  sw $s1, 4 + 16($s3)                                ## L:583  |    165 | store(posB, dmaDmemEnd, 0x04, 16);
  addiu $s3, $s3, 24                                 ## L:587  |    166 | dmaDmemEnd += 24;
  ssv $v06, 12, -12, $s3                             ## L:581  |    167 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  LABEL_TPXCmd_DrawColor_0008:
  vmudn $v12, $v28, $v09.h0                          ## L:94   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:94   |    168 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:95   |    169 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:95   |    170 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:96   |    171 | out = mat2 +* vec.zzzzZZZZ;
  sltu $at, $s3, $s5                                 ## L:594  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v23, $v09.h2                          ## L:96   |    172 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:97   |    173 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawColor_0009        ## L:594  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v21, $v30.e7                          ## L:97   |    174 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:595  |    175 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:595  |    176 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:596  |   *178 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawColor_0009:
  vch $v29, $v11, $v11.h3                            ## L:599  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:599  |    179 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawColor_0005          ## L:599  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:599  |    180 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0006:
  beq $zero, $zero, LABEL_TPXCmd_DrawColor_0004      ## L:599  |    181 | temp1 = clip(posClip, posClip.wwwwWWWW);
  nop                                                ## L:599  |   *183 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0003:
  lpv $v09, 0, 0, $s6                                ## L:117  |      1 | vec16 posStart = load_vec_s8(ptrIn, 0x00);
  vmulf $v14, $v13, $v09.h3                          ## L:118  |   ***5 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v09, $v09, $v31.e7                          ## L:119  |      6 | posStart >>= 8;
  vmudn $v12, $v28, $v09.h0                          ## L:94   |  ***10 | out = mat0  * vec.xxxxXXXX;
  vmadh $v11, $v27, $v09.h0                          ## L:94   |     11 | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:95   |     12 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:95   |     13 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:96   |     14 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:96   |     15 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:97   |     16 | out = mat3 +* 1;
  vmadh $v11, $v21, $v30.e7                          ## L:97   |     17 | out = mat3 +* 1;
  vch $v29, $v11, $v11.h3                            ## L:123  |  ***21 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:123  |     22 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:123  |     23 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_000A:
  vmudl $v12, $v12, $v17.v                           ## L:134  |      ^ | posClip *= normScaleW:ufract;
  ldv $v03, 0, 8, $s6                                ## L:181  |     24 | vec16 color = load(ptrIn, 8).xyzw;
  vmadm $v11, $v11, $v17.v                           ## L:134  |      ^ | posClip *= normScaleW:ufract;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:140  |     25 | posClip:sint.w = load(BASE_SIZE).x;
  vmadn $v12, $v00, $v00                             ## L:134  |      ^ | posClip *= normScaleW:ufract;
  vrcph $v07.e3, $v11.e3                             ## L:136  |   **28 | invW.w = invert_half(posClip).w;
  addiu $t2, $zero, 54                               ## L:183  |      ^ | cmdRdpRect = 0x36;
  vrcpl $v08.e3, $v12.e3                             ## L:136  |     29 | invW.w = invert_half(posClip).w;
  vrcph $v07.e3, $v11.e7                             ## L:137  |     30 | invW.W = invert_half(posClip).W;
  vrcpl $v08.e7, $v12.e7                             ## L:137  |     31 | invW.W = invert_half(posClip).W;
  lsv $v11, 14, 0, $at                               ## L:141  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  lsv $v11, 6, 0, $at                                ## L:140  |     32 | posClip:sint.w = load(BASE_SIZE).x;
  vmov $v12.e3, $v00.e0                              ## L:142  |      ^ | posClip:sfract.w = 0;
  addiu $at, $zero, 3                                ## L:215  |     33 | if(clipA == 0b0000'0011) {
  vmov $v12.e7, $v00.e0                              ## L:143  |      ^ | posClip:sfract.W = 0;
  vrcph $v07.e7, $v00.e7                             ## L:137  |     34 | invW.W = invert_half(posClip).W;
  vmudl $v29, $v12, $v08.h3                          ## L:146  |   **37 | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v11, $v08.h3                          ## L:146  |     38 | posClip *= invW.wwwwWWWW;
  vmadn $v12, $v12, $v07.h3                          ## L:146  |     39 | posClip *= invW.wwwwWWWW;
  vmadh $v11, $v11, $v07.h3                          ## L:146  |     40 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v11.h3                          ## L:150  |  ***44 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v12, $v20.v                           ## L:153  |     45 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v11, $v20.v                           ## L:153  |     46 | vec32 posScreen = posClip * screenSize;
  vmadn $v08, $v12, $v19.v                           ## L:153  |     47 | vec32 posScreen = posClip * screenSize;
  vmadh $v07, $v11, $v19.v                           ## L:153  |     48 | vec32 posScreen = posClip * screenSize;
  vmadh $v06, $v18, $v10.v                           ## L:154  |     49 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v05, $v10, $v14.v                           ## L:157  |     50 | vec16 posEnd = vecOne +* localPartSize:sint;
  andi $t1, $t4, 1028                                ## L:130  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  andi $t5, $t4, 16448                               ## L:131  |   **53 | clipB = temp1 & 0b0100'0000'0100'0000;
  vsubc $v09, $v06, $v14.v                           ## L:158  |      ^ | posStart = posCenter - localPartSize:sint;
  vlt $v05, $v05, $v15                               ## L:161  |     54 | posEnd = min(posEnd, screenMax);
  slv $v03, 0, 4, $s3                                ## L:191  |      ^ | store(color.xy, dmaDmemEnd, 4);
  vge $v09, $v09, $v16                               ## L:162  |   **57 | posStart = max(posStart, screenMin);
  ssv $v06, 4, 12, $s3                               ## L:195  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  mfc2 $fp, $v05.e1                                  ## L:1188 |     58 | outA = pos.y;
  mfc2 $sp, $v05.e5                                  ## L:1194 |     59 | outB = pos.Y;
  andi $fp, $fp, 4095                                ## L:1189 |    *61 | outA &= 0b1111'1111'1111;
  mfc2 $s2, $v09.e1                                  ## L:1188 |     62 | outA = pos.y;
  mfc2 $k1, $v09.e0                                  ## L:1190 |     63 | u32 tmp = pos.x;
  andi $sp, $sp, 4095                                ## L:1195 |     64 | outB &= 0b1111'1111'1111;
  andi $s2, $s2, 4095                                ## L:1189 |     65 | outA &= 0b1111'1111'1111;
  vlt $v04, $v09, $v05                               ## L:168  |      ^ | vec16 extend = posStart < posEnd;
  mfc2 $s1, $v09.e5                                  ## L:1194 |     66 | outB = pos.Y;
  sll $k1, $k1, 12                                   ## L:1191 |     67 | tmp <<= 12;
  or $s2, $s2, $k1                                   ## L:1192 |     68 | outA |= tmp;
  mfc2 $k1, $v09.e4                                  ## L:1196 |     69 | tmp = pos.X;
  cfc2 $t4, $vcc                                     ## L:169  |     70 | temp1 = get_vcc();
  andi $s1, $s1, 4095                                ## L:1195 |     71 | outB &= 0b1111'1111'1111;
  sll $k1, $k1, 12                                   ## L:1197 |     72 | tmp <<= 12;
  andi $a2, $t4, 3                                   ## L:171  |     73 | temp0 = temp1 & 0b0000'0011;
  or $s1, $s1, $k1                                   ## L:1198 |     74 | outB |= tmp;
  mfc2 $k1, $v05.e0                                  ## L:1190 |     75 | u32 tmp = pos.x;
  lpv $v09, 0, 16, $s6                               ## L:210  |     76 | posStart = load_vec_s8(ptrIn, 16);
  or $t1, $t1, $a2                                   ## L:172  |     77 | clipA |= temp0;
  sll $k1, $k1, 12                                   ## L:1191 |     78 | tmp <<= 12;
  or $fp, $fp, $k1                                   ## L:1192 |     79 | outA |= tmp;
  mfc2 $k1, $v05.e4                                  ## L:1196 |     80 | tmp = pos.X;
  sw $fp, 0 + 16($s3)                                ## L:200  |     81 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  andi $a2, $t4, 48                                  ## L:174  |     82 | temp0 = temp1 & 0b0011'0000;
  sw $s2, 4 + 16($s3)                                ## L:199  |     83 | store(posA, dmaDmemEnd, 0x04, 16);
  sll $k1, $k1, 12                                   ## L:1197 |     84 | tmp <<= 12;
  vmulf $v14, $v13, $v09.h3                          ## L:211  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  or $sp, $sp, $k1                                   ## L:1198 |     85 | outB |= tmp;
  vmudm $v09, $v09, $v31.e7                          ## L:212  |      ^ | posStart >>= 8;
  or $t5, $t5, $a2                                   ## L:175  |     86 | clipB |= temp0;
  bne $t1, $at, LABEL_TPXCmd_DrawColor_000C          ## L:215  |     87 | if(clipA == 0b0000'0011) {
  sb $t2, 0 + 16($s3)                                ## L:201  |    *89 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  addiu $s3, $s3, 24                                 ## L:216  |     90 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_000C:
  addiu $at, $zero, 48                               ## L:226  |     91 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawColor_000D          ## L:226  |     92 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 16                                 ## L:222  |    *94 | ptrIn += 16;
  sw $sp, 0 + 16($s3)                                ## L:231  |     95 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sb $t2, 0 + 16($s3)                                ## L:232  |     96 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sw $s1, 4 + 16($s3)                                ## L:230  |     97 | store(posB, dmaDmemEnd, 0x04, 16);
  slv $v03, 4, 4, $s3                                ## L:227  |     98 | store(color.zw, dmaDmemEnd, 4);
  ssv $v06, 12, 12, $s3                              ## L:228  |     99 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  addiu $s3, $s3, 24                                 ## L:234  |    100 | dmaDmemEnd += 24;
  LABEL_TPXCmd_DrawColor_000D:
  vmudn $v12, $v28, $v09.h0                          ## L:94   |      ^ | out = mat0  * vec.xxxxXXXX;
  sltu $at, $s3, $s5                                 ## L:241  |    101 | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v27, $v09.h0                          ## L:94   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadn $v12, $v26, $v09.h1                          ## L:95   |    102 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v11, $v25, $v09.h1                          ## L:95   |    103 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v12, $v24, $v09.h2                          ## L:96   |    104 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v11, $v23, $v09.h2                          ## L:96   |    105 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v12, $v22, $v30.e7                          ## L:97   |    106 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawColor_000E        ## L:241  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v11, $v21, $v30.e7                          ## L:97   |   *108 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:242  |    109 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:242  |   *111 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:243  |    112 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawColor_000E:
  vch $v29, $v11, $v11.h3                            ## L:246  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v12, $v12.h3                            ## L:246  |    113 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawColor_000A          ## L:246  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:246  |   *115 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawColor_0004:
  j RDPQ_Send                                        ## L:907  | ********************************************************************184 | goto RDPQ_Send;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:906  |   *186 | RA = RSPQ_Loop;
mulMat4Mat4:
  ## hot-path cycles: 50
  ldv $v02, 0, 8, $s3                                ## L:932  |      ^ | vec32 matL0 = load(addrMatL, 0x00).xyzwxyzw;
  ldv $v10, 8, 24, $s4                               ## L:940  |      2 | matR01.XYZW = load(addrMatR, 0x10).xyzw;
  ldv $v01, 8, 0, $s3                                ## L:932  |      3 | vec32 matL0 = load(addrMatL, 0x00).xyzwxyzw;
  ldv $v09, 8, 16, $s4                               ## L:940  |      4 | matR01.XYZW = load(addrMatR, 0x10).xyzw;
  ldv $v10, 0, 8, $s4                                ## L:939  |      5 | matR01.xyzw = load(addrMatR, 0x00).xyzw;
  ldv $v02, 8, 8, $s3                                ## L:932  |      6 | vec32 matL0 = load(addrMatL, 0x00).xyzwxyzw;
  ldv $v01, 0, 0, $s3                                ## L:932  |      7 | vec32 matL0 = load(addrMatL, 0x00).xyzwxyzw;
  ldv $v09, 0, 0, $s4                                ## L:939  |      8 | matR01.xyzw = load(addrMatR, 0x00).xyzw;
  ldv $v04, 0, 24, $s3                               ## L:933  |      9 | vec32 matL1 = load(addrMatL, 0x10).xyzwxyzw;
  ldv $v03, 8, 16, $s3                               ## L:933  |     10 | vec32 matL1 = load(addrMatL, 0x10).xyzwxyzw;
  vmudl $v29, $v02, $v10.h0                          ## L:945  |      ^ | tmp    = matL0  * matR01.xxxxXXXX;
  vmadm $v29, $v01, $v10.h0                          ## L:945  |     11 | tmp    = matL0  * matR01.xxxxXXXX;
  ldv $v04, 8, 24, $s3                               ## L:933  |      ^ | vec32 matL1 = load(addrMatL, 0x10).xyzwxyzw;
  vmadn $v14, $v02, $v09.h0                          ## L:945  |     12 | tmp    = matL0  * matR01.xxxxXXXX;
  ldv $v03, 0, 16, $s3                               ## L:933  |      ^ | vec32 matL1 = load(addrMatL, 0x10).xyzwxyzw;
  ldv $v06, 8, 40, $s3                               ## L:934  |     13 | vec32 matL2 = load(addrMatL, 0x20).xyzwxyzw;
  ldv $v06, 0, 40, $s3                               ## L:934  |     14 | vec32 matL2 = load(addrMatL, 0x20).xyzwxyzw;
  vmadh $v13, $v01, $v09.h0                          ## L:945  |      ^ | tmp    = matL0  * matR01.xxxxXXXX;
  vmadl $v29, $v04, $v10.h1                          ## L:946  |     15 | tmp    = matL1 +* matR01.yyyyYYYY;
  ldv $v05, 0, 32, $s3                               ## L:934  |      ^ | vec32 matL2 = load(addrMatL, 0x20).xyzwxyzw;
  vmadm $v29, $v03, $v10.h1                          ## L:946  |     16 | tmp    = matL1 +* matR01.yyyyYYYY;
  ldv $v05, 8, 32, $s3                               ## L:934  |      ^ | vec32 matL2 = load(addrMatL, 0x20).xyzwxyzw;
  vmadn $v14, $v04, $v09.h1                          ## L:946  |     17 | tmp    = matL1 +* matR01.yyyyYYYY;
  ldv $v08, 0, 56, $s3                               ## L:935  |      ^ | vec32 matL3 = load(addrMatL, 0x30).xyzwxyzw;
  ldv $v08, 8, 56, $s3                               ## L:935  |     18 | vec32 matL3 = load(addrMatL, 0x30).xyzwxyzw;
  vmadh $v13, $v03, $v09.h1                          ## L:946  |      ^ | tmp    = matL1 +* matR01.yyyyYYYY;
  ldv $v07, 0, 48, $s3                               ## L:935  |     19 | vec32 matL3 = load(addrMatL, 0x30).xyzwxyzw;
  vmadl $v29, $v06, $v10.h2                          ## L:947  |      ^ | tmp    = matL2 +* matR01.zzzzZZZZ;
  vmadm $v29, $v05, $v10.h2                          ## L:947  |     20 | tmp    = matL2 +* matR01.zzzzZZZZ;
  ldv $v07, 8, 48, $s3                               ## L:935  |      ^ | vec32 matL3 = load(addrMatL, 0x30).xyzwxyzw;
  vmadn $v14, $v06, $v09.h2                          ## L:947  |     21 | tmp    = matL2 +* matR01.zzzzZZZZ;
  ldv $v11, 8, 48, $s4                               ## L:942  |      ^ | matR23.XYZW = load(addrMatR, 0x30).xyzw;
  vmadh $v13, $v05, $v09.h2                          ## L:947  |     22 | tmp    = matL2 +* matR01.zzzzZZZZ;
  ldv $v12, 8, 56, $s4                               ## L:942  |      ^ | matR23.XYZW = load(addrMatR, 0x30).xyzw;
  vmadl $v29, $v08, $v10.h3                          ## L:948  |     23 | matR01 = matL3 +* matR01.wwwwWWWW;
  ldv $v12, 0, 40, $s4                               ## L:941  |      ^ | matR23.xyzw = load(addrMatR, 0x20).xyzw;
  vmadm $v29, $v07, $v10.h3                          ## L:948  |     24 | matR01 = matL3 +* matR01.wwwwWWWW;
  ldv $v11, 0, 32, $s4                               ## L:941  |      ^ | matR23.xyzw = load(addrMatR, 0x20).xyzw;
  vmadn $v10, $v08, $v09.h3                          ## L:948  |     25 | matR01 = matL3 +* matR01.wwwwWWWW;
  vmadh $v09, $v07, $v09.h3                          ## L:948  |     26 | matR01 = matL3 +* matR01.wwwwWWWW;
  vmudl $v29, $v02, $v12.h0                          ## L:950  |     27 | tmp    = matL0  * matR23.xxxxXXXX;
  vmadm $v29, $v01, $v12.h0                          ## L:950  |     28 | tmp    = matL0  * matR23.xxxxXXXX;
  vmadn $v14, $v02, $v11.h0                          ## L:950  |     29 | tmp    = matL0  * matR23.xxxxXXXX;
  vmadh $v13, $v01, $v11.h0                          ## L:950  |     30 | tmp    = matL0  * matR23.xxxxXXXX;
  vmadl $v29, $v04, $v12.h1                          ## L:951  |     31 | tmp    = matL1 +* matR23.yyyyYYYY;
  vmadm $v29, $v03, $v12.h1                          ## L:951  |     32 | tmp    = matL1 +* matR23.yyyyYYYY;
  sdv $v10, 8, 24, $s2                               ## L:956  |      ^ | store(matR01.XYZW, addrOut, 0x10);
  vmadn $v14, $v04, $v11.h1                          ## L:951  |     33 | tmp    = matL1 +* matR23.yyyyYYYY;
  vmadh $v13, $v03, $v11.h1                          ## L:951  |     34 | tmp    = matL1 +* matR23.yyyyYYYY;
  vmadl $v29, $v06, $v12.h2                          ## L:952  |     35 | tmp    = matL2 +* matR23.zzzzZZZZ;
  vmadm $v29, $v05, $v12.h2                          ## L:952  |     36 | tmp    = matL2 +* matR23.zzzzZZZZ;
  vmadn $v14, $v06, $v11.h2                          ## L:952  |     37 | tmp    = matL2 +* matR23.zzzzZZZZ;
  vmadh $v13, $v05, $v11.h2                          ## L:952  |     38 | tmp    = matL2 +* matR23.zzzzZZZZ;
  sdv $v09, 0, 0, $s2                                ## L:955  |      ^ | store(matR01.xyzw, addrOut, 0x00);
  vmadl $v29, $v08, $v12.h3                          ## L:953  |     39 | matR23 = matL3 +* matR23.wwwwWWWW;
  vmadm $v29, $v07, $v12.h3                          ## L:953  |     40 | matR23 = matL3 +* matR23.wwwwWWWW;
  vmadn $v12, $v08, $v11.h3                          ## L:953  |     41 | matR23 = matL3 +* matR23.wwwwWWWW;
  sdv $v10, 0, 8, $s2                                ## L:955  |      ^ | store(matR01.xyzw, addrOut, 0x00);
  sdv $v09, 8, 16, $s2                               ## L:956  |     42 | store(matR01.XYZW, addrOut, 0x10);
  vmadh $v11, $v07, $v11.h3                          ## L:953  |      ^ | matR23 = matL3 +* matR23.wwwwWWWW;
  sdv $v12, 8, 56, $s2                               ## L:958  |   **45 | store(matR23.XYZW, addrOut, 0x30);
  sdv $v11, 0, 32, $s2                               ## L:957  |     46 | store(matR23.xyzw, addrOut, 0x20);
  sdv $v12, 0, 40, $s2                               ## L:957  |     47 | store(matR23.xyzw, addrOut, 0x20);
  jr $ra                                             ## L:959  |     48 | }
  sdv $v11, 8, 48, $s2                               ## L:958  |    *50 | store(matR23.XYZW, addrOut, 0x30);
T3DCmd_MatrixStack:
  ## hot-path cycles: 40
  andi $s7, $a0, 1                                   ## L:1005 |      ^ | u16 doMultiply = stackAdvance & 0b01;
  andi $s6, $a0, 2                                   ## L:1006 |      2 | u16 onlyStackMove = stackAdvance & 0b10;
  lh $a0, %lo(RSPQ_DMEM_BUFFER  -7)($gp)             ## L:1008 |      3 | stackAdvance:s16 = load_arg(1);
  lw $s5, %lo(MATRIX_STACK_PTR + 0)                  ## L:1010 |      4 | s32 stackPtr = load(MATRIX_STACK_PTR);
  addu $s5, $s5, $a0                                 ## L:1011 |    **7 | stackPtr += stackAdvance;
  bne $s6, $zero, RSPQ_Loop                          ## L:1015 |      8 | if(onlyStackMove)exit;
  sw $s5, %lo(MATRIX_STACK_PTR)($zero)               ## L:1012 |      9 | store(stackPtr, MATRIX_STACK_PTR);
  bgez $a0, LABEL_T3DCmd_MatrixStack_0010            ## L:1020 |     10 | if(stackAdvance < 0)addressMat = stackPtr;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1016 |     11 | dmaDest = RDP_BUFF;
  or $a1, $zero, $s5                                 ## L:1020 |     12 | if(stackAdvance < 0)addressMat = stackPtr;
  LABEL_T3DCmd_MatrixStack_0010:
  lui $at, 0xFF                                      ## L:1021 |     13 | u32<$s0> addrRDRAM = addressMat & 0xFFFFFF;
  ori $at, $at, 0xFFFF                               ## L:1021 |     14 | u32<$s0> addrRDRAM = addressMat & 0xFFFFFF;
  and $s0, $a1, $at                                  ## L:1021 |     15 | u32<$s0> addrRDRAM = addressMat & 0xFFFFFF;
  addiu $t2, $zero, 12                               ## L:1023 |     16 | dma_in(dmaDest, addrRDRAM, 0x40);
  jal DMAExec                                        ## L:1023 |     17 | dma_in(dmaDest, addrRDRAM, 0x40); ## Args: $t0, $t1, $s0, $s4, $t2
  ori $t0, $zero, 63                                 ## L:1023 |     18 | dma_in(dmaDest, addrRDRAM, 0x40);
  beq $s7, $zero, LABEL_T3DCmd_MatrixStack_0011      ## L:1030 |     19 | if(doMultiply) {
  nop                                                ## L:1030 |     20 | if(doMultiply) {
  addiu $t2, $zero, 12                               ## L:1034 |     21 | dma_in(dmaDest, addrRDRAM, 0x40);
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:1032 |     22 | dmaDest = PARTICLE_BUFF;
  ori $t0, $zero, 63                                 ## L:1034 |     23 | dma_in(dmaDest, addrRDRAM, 0x40);
  jal DMAExec                                        ## L:1034 |     24 | dma_in(dmaDest, addrRDRAM, 0x40); ## Args: $t0, $t1, $s0, $s4, $t2
  addiu $s0, $s5, 65472                              ## L:1033 |     25 | addrRDRAM = stackPtr -  0x40;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1039 |     26 | dmaDest = RDP_BUFF;
  ori $s3, $zero, %lo(PARTICLE_BUFF)                 ## L:1038 |     27 | u32<$s3> mulLeft = PARTICLE_BUFF;
  jal mulMat4Mat4                                    ## L:1040 |     28 | mulMat4Mat4(mulDest, mulLeft, dmaDest); ## Args: $s2, $s3, $s4
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:1037 |     29 | u32<$s2> mulDest = RDP_BUFF;
  LABEL_T3DCmd_MatrixStack_0011:
  ori $t0, $zero, 63                                 ## L:1045 |     30 | dma_out(dmaDest, addrRDRAM, 0x40);
  addiu $t2, $zero, -32756                           ## L:1045 |     31 | dma_out(dmaDest, addrRDRAM, 0x40);
  jal DMAExec                                        ## L:1045 |     32 | dma_out(dmaDest, addrRDRAM, 0x40); ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $s5                                 ## L:1044 |     33 | addrRDRAM = stackPtr;
  ori $s2, $zero, %lo(MATRIX_MVP)                    ## L:1049 |     34 | u32<$s2> mulDest = MATRIX_MVP;
  ori $s3, $zero, %lo(MATRIX_PROJ)                   ## L:1053 |     35 | u32<$s3> mulLeft = MATRIX_PROJ;
  jal mulMat4Mat4                                    ## L:1063 |     36 | mulMat4Mat4(mulDest, mulLeft, mulRight); ## Args: $s2, $s3, $s4
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1062 |     37 | u32<$s4> mulRight = RDP_BUFF;
  j RSPQ_Loop                                        ## L:1064 |     38 | }
  nop                                                ## L:1064 |    *40 | }
TPXCmd_SetDMEM:
  ## hot-path cycles: 6
  andi $s7, $a0, 32768                               ## L:1069 |      ^ | u32 is16bit = addrDMEM & 0x8000;
  bne $s7, $zero, RSPQ_Loop                          ## L:1072 |      2 | if(is16bit)goto RSPQ_Loop;
  sh $a1, ($a0)                                      ## L:1070 |      3 | store(value:u16, addrDMEM);
  j RSPQ_Loop                                        ## L:1075 |      4 | }
  sw $a1, ($a0)                                      ## L:1074 |     *6 | store(value:u32, addrDMEM);
TPXCmd_DrawTextured:
  ## hot-path cycles: 246
  or $s0, $zero, $a1                                 ## L:1097 |      ^ | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  andi $t0, $a0, 65535                               ## L:1081 |      2 | u32<$t0> dmaSize = dataSize & 0xFFFF;
  ori $s4, $zero, %lo(PARTICLE_BUFF)                 ## L:1080 |      3 | u16<$s4> dmaDmem = PARTICLE_BUFF;
  addu $s7, $s4, $t0                                 ## L:1096 |      4 | u32 ptrInEnd = dmaDmem + dmaSize;
  addiu $t0, $t0, -1                                 ## L:1097 |      5 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  jal DMAExec                                        ## L:1097 |      6 | dma_in_async(dmaDmem, rdramAddr, dmaSize); ## Args: $t0, $t1, $s0, $s4, $t2
  or $t2, $zero, $zero                               ## L:1097 |      7 | dma_in_async(dmaDmem, rdramAddr, dmaSize);
  ori $at, $zero, %lo(MATRIX_MVP)                    ## L:1105 |      8 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v22, 8, 56, $at                               ## L:1108 |      9 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v26, 0, 24, $at                               ## L:1106 |     10 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  lw $s2, %lo(RDPQ_SCISSOR_RECT + 4)                 ## L:1210 |     11 | u32 extMax = load(RDPQ_SCISSOR_RECT, 4);
  ldv $v27, 8, 0, $at                                ## L:1105 |     12 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  lw $s1, %lo(RDPQ_SCISSOR_RECT + 0)                 ## L:1211 |     13 | u32 extMin = load(RDPQ_SCISSOR_RECT, 0);
  ldv $v24, 8, 40, $at                               ## L:1107 |     14 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v21, 0, 48, $at                               ## L:1108 |     15 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v22, 0, 56, $at                               ## L:1108 |     16 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v24, 0, 40, $at                               ## L:1107 |     17 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v28, 0, 8, $at                                ## L:1105 |     18 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v23, 0, 32, $at                               ## L:1107 |     19 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v21, 8, 48, $at                               ## L:1108 |     20 | vec32 mat3 = load(MATRIX_MVP, 0x30).xyzwxyzw;
  ldv $v25, 0, 16, $at                               ## L:1106 |     21 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v28, 8, 8, $at                                ## L:1105 |     22 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ldv $v26, 8, 24, $at                               ## L:1106 |     23 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ldv $v27, 0, 0, $at                                ## L:1105 |     24 | vec32 mat0 = load(MATRIX_MVP, 0x00).xyzwxyzw;
  ori $s3, $zero, %lo(RDP_BUFF)                      ## L:1102 |     25 | u16<$s3> dmaDmemEnd = RDP_BUFF;
  ldv $v23, 8, 32, $at                               ## L:1107 |     26 | vec32 mat2 = load(MATRIX_MVP, 0x20).xyzwxyzw;
  ldv $v25, 8, 16, $at                               ## L:1106 |     27 | vec32 mat1 = load(MATRIX_MVP, 0x10).xyzwxyzw;
  ori $at, $zero, %lo(SCREEN_SCALE)                  ## L:1110 |     28 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v19, 0, 0, $at                                ## L:1110 |     29 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v20, 8, 8, $at                                ## L:1110 |     30 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v20, 0, 8, $at                                ## L:1110 |     31 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ldv $v19, 8, 0, $at                                ## L:1110 |     32 | vec32 screenSize = load(SCREEN_SCALE).xyzwxyzw;
  ori $at, $zero, %lo(SCREEN_OFFSET)                 ## L:1112 |     33 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v18, 8, 0, $at                                ## L:1112 |     34 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ldv $v18, 0, 0, $at                                ## L:1112 |     35 | vec16 screenOffset = load(SCREEN_OFFSET).xyzwxyzw;
  ori $at, $zero, %lo(NORM_SCALE_W)                  ## L:1113 |     36 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  ldv $v17, 8, 0, $at                                ## L:1113 |     37 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  addiu $s5, $s3, 480                                ## L:1103 |     38 | u16 dmaDmemFlush = dmaDmemEnd + 480;
  ldv $v17, 0, 0, $at                                ## L:1113 |     39 | vec16 normScaleW = load(NORM_SCALE_W).xyzwxyzw;
  mtc2 $s2, $v15.e5                                  ## L:1215 |     40 | screenMax.y = extMax; screenMax.Y = extMax;
  srl $t4, $s2, 12                                   ## L:1213 |     41 | temp1 = extMax >> 12;
  addiu $at, $zero, 4095                             ## L:1209 |     42 | screenMax.w = 0b1111'1111'1111;
  mtc2 $t4, $v15.e0                                  ## L:1214 |     43 | screenMax.x = temp1;  screenMax.X = temp1;
  mtc2 $t4, $v15.e4                                  ## L:1214 |     44 | screenMax.x = temp1;  screenMax.X = temp1;
  addiu $a0, $zero, 46                               ## L:1129 |     45 | cmdRdpDepth = 0x2E;
  srl $t4, $s1, 12                                   ## L:1217 |     46 | temp1 = extMin >> 12;
  lui $t7, 0x3A00                                    ## L:1128 |     47 | cmdRdpColor = 0x3A00'0000;
  or $s6, $zero, $s4                                 ## L:1099 |     48 | u32 ptrIn = dmaDmem;
  mtc2 $at, $v15.e3                                  ## L:1209 |     49 | screenMax.w = 0b1111'1111'1111;
  mtc2 $s2, $v15.e1                                  ## L:1215 |     50 | screenMax.y = extMax; screenMax.Y = extMax;
  ori $at, $zero, %lo(PARTICLE_SCALE)                ## L:1120 |     51 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  llv $v13, 0, 0, $at                                ## L:1120 |     52 | globalPartSize.xy = load(PARTICLE_SCALE).xy;
  mtc2 $t4, $v16.e0                                  ## L:1218 |     53 | screenMin.x = temp1;  screenMin.X = temp1;
  ori $s2, $zero, %lo(RDP_BUFF)                      ## L:1126 |     54 | u16 buffRdp = RDP_BUFF;
  vand $v15, $v15, $v15.e3                           ## L:1221 |      ^ | screenMax &= screenMax.w;
  ori $s4, $zero, %lo(RDP_BUFF)                      ## L:1101 |     55 | dmaDmem = RDP_BUFF;
  mtc2 $s1, $v16.e5                                  ## L:1219 |     56 | screenMin.y = extMin; screenMin.Y = extMin;
  mtc2 $s1, $v16.e1                                  ## L:1219 |     57 | screenMin.y = extMin; screenMin.Y = extMin;
  addiu $s1, $s5, 32                                 ## L:1127 |     58 | u16 buffRdpEnd = dmaDmemFlush + 32;
  llv $v13, 8, 0, $at                                ## L:1121 |     59 | globalPartSize.XY = load(PARTICLE_SCALE).xy;
  mtc2 $t4, $v16.e4                                  ## L:1218 |     60 | screenMin.x = temp1;  screenMin.X = temp1;
  vand $v16, $v16, $v15.e3                           ## L:1222 |  ***64 | screenMin &= screenMax.w;
  LABEL_TPXCmd_DrawTextured_0013:
  addiu $s2, $s2, 32                                 ## L:1134 |      ^ | buffRdp += 32;
  sh $zero, -18($s2)                                 ## L:1133 |     65 | store(ZERO:u16, buffRdp, 0x0E);
  sb $a0, -24($s2)                                   ## L:1132 |     66 | store(cmdRdpDepth, buffRdp, 8);
  bne $s2, $s1, LABEL_TPXCmd_DrawTextured_0013       ## L:1134 |     67 | buffRdp += 32;
  sw $t7, -32($s2)                                   ## L:1131 |     68 | store(cmdRdpColor, buffRdp, 0);
  LABEL_TPXCmd_DrawTextured_0014:
  lhu $s1, %lo(TILE_COUNT + 0)                       ## L:1145 |     69 | u16 tiles = load(TILE_COUNT);
  vxor $v12, $v00, $v00.e0                           ## L:1141 |      ^ | vec16 texMirrorMask = 0;
  addu $fp, $s1, $s1                                 ## L:1146 |   **72 | u16 tilesEnd = tiles + tiles;
  vmov $v12.e7, $v31.e7                              ## L:1156 |      ^ | texMirrorMask.W = 0x100;
  mtc2 $s1, $v12.e2                                  ## L:1150 |     73 | texMirrorMask.z = tiles;
  mtc2 $fp, $v12.e3                                  ## L:1151 |     74 | texMirrorMask.w = tilesEnd;
  vmov $v12.e1, $v30.e7                              ## L:1149 |     75 | texMirrorMask.y = 1;
  addiu $fp, $fp, 65535                              ## L:1153 |      ^ | tilesEnd -= 1;
  vmov $v12.e5, $v30.e7                              ## L:1148 |     76 | texMirrorMask.Y = 1;
  mtc2 $fp, $v12.e4                                  ## L:1155 |     77 | texMirrorMask.X = tilesEnd;
  vxor $v11, $v00, $v00.e0                           ## L:1142 |      ^ | vec16 texMirrorCompare = 0;
  mtc2 $fp, $v12.e0                                  ## L:1154 |     78 | texMirrorMask.x = tilesEnd;
  vmudn $v12, $v12, $v12.e7                          ## L:1157 |  ***82 | texMirrorMask = texMirrorMask * texMirrorMask.W;
  vsubc $v29, $v12, $v30.e7                          ## L:1159 |  ***86 | VTEMP = texMirrorMask - 1;
  lh $s2, %lo(TEX_OFFSET + 0)                        ## L:1139 |      ^ | s16 texOffset = load(TEX_OFFSET);
  vmov $v12.e3, $v29.e3                              ## L:1160 |  ***90 | texMirrorMask.w = VTEMP.w;
  jal DMAWaitIdle                                    ## L:1164 |      ^ | dma_await();
  vor $v11, $v00, $v29.e2                            ## L:1161 |     91 | texMirrorCompare = VTEMP.z;
  bgez $a1, LABEL_TPXCmd_DrawTextured_0015           ## L:1169 |     92 | if(rdramAddr < 0) {
  nop                                                ## L:1169 |     93 | if(rdramAddr < 0) {
  lqv $v08, 0, 0, $s6                                ## L:608  |     94 | vec16 posStart = load(ptrIn, 0x00);
  vxor $v07, $v00, $v30.e7                           ## L:616  |      ^ | const vec16 vecOne = 1;
  vmulf $v14, $v13, $v08.h3                          ## L:609  |  ***98 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudn $v10, $v28, $v08.h0                          ## L:94   |     99 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:94   |    100 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:95   |    101 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:95   |    102 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:96   |    103 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:96   |    104 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:97   |    105 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:97   |    106 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:621  | ***110 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:621  |    111 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:621  |    112 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0017:
  vmudl $v10, $v10, $v17.v                           ## L:632  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v09, $v09, $v17.v                           ## L:632  |    113 | posClip *= normScaleW:ufract;
  lb $a2, 7($s6)                                     ## L:676  |      ^ | temp0:s8 = load(ptrIn, 7);
  vmadn $v10, $v00, $v00                             ## L:632  |    114 | posClip *= normScaleW:ufract;
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:638  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcph $v05.e3, $v09.e3                             ## L:634  |  **117 | invW.w = invert_half(posClip).w;
  andi $t1, $t4, 1028                                ## L:628  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  vrcpl $v06.e3, $v10.e3                             ## L:634  |    118 | invW.w = invert_half(posClip).w;
  andi $t5, $t4, 16448                               ## L:629  |      ^ | clipB = temp1 & 0b0100'0000'0100'0000;
  vrcph $v05.e3, $v09.e7                             ## L:635  |    119 | invW.W = invert_half(posClip).W;
  lsv $v09, 14, 0, $at                               ## L:639  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  vrcpl $v06.e7, $v10.e7                             ## L:635  |    120 | invW.W = invert_half(posClip).W;
  vmov $v10.e3, $v00.e0                              ## L:640  |    121 | posClip:sfract.w = 0;
  vmov $v10.e7, $v00.e0                              ## L:641  |    122 | posClip:sfract.W = 0;
  lsv $v09, 6, 0, $at                                ## L:638  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcph $v05.e7, $v00.e7                             ## L:635  |    123 | invW.W = invert_half(posClip).W;
  lb $t4, 15($s6)                                    ## L:677  |      ^ | temp1:s8 = load(ptrIn, 15);
  vmudl $v29, $v10, $v06.h3                          ## L:644  |  **126 | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v09, $v06.h3                          ## L:644  |    127 | posClip *= invW.wwwwWWWW;
  vmadn $v10, $v10, $v05.h3                          ## L:644  |    128 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:644  |    129 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:647  | ***133 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:650  |    134 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:650  |    135 | vec32 posScreen = posClip * screenSize;
  vmadn $v04, $v10, $v19.v                           ## L:650  |    136 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:650  |    137 | vec32 posScreen = posClip * screenSize;
  addu $t4, $t4, $s2                                 ## L:683  |      ^ | temp1 += texOffset; temp1 <<= 3;
  sll $t4, $t4, 3                                    ## L:683  |    138 | temp1 += texOffset; temp1 <<= 3;
  vmadh $v02, $v18, $v07.v                           ## L:651  |      ^ | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v04, $v07, $v14.v                           ## L:656  |    139 | vec16 posEnd = vecOne +* localPartSize:sint;
  vsubc $v08, $v02, $v14.v                           ## L:657  |  **142 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:661  |    143 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:662  | ***147 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:662  |    148 | invW.x = invert_half(localPartSize).x;
  vrcp $v06.e1, $v14.e1                              ## L:663  |    149 | invW.y = invert_half(localPartSize).y;
  vrcph $v05.e1, $v14.e1                             ## L:663  |    150 | invW.y = invert_half(localPartSize).y;
  vrcp $v06.e4, $v14.e4                              ## L:664  |    151 | invW.X = invert_half(localPartSize).X;
  vrcph $v05.e4, $v14.e4                             ## L:664  |    152 | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:665  |    153 | invW.Y = invert_half(localPartSize).Y;
  ssv $v02, 4, 12, $s3                               ## L:751  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  vrcph $v05.e5, $v14.e5                             ## L:665  |    154 | invW.Y = invert_half(localPartSize).Y;
  addu $a2, $a2, $s2                                 ## L:682  |      ^ | temp0 += texOffset; temp0 <<= 3;
  vmudh $v01, $v08, $v05.v                           ## L:670  | ***158 | vec16 uvStart = posStart * invW:sint;
  vor $v03, $v00, $v05                               ## L:666  |    159 | vec16 uvDelta = invW:sint;
  sll $a2, $a2, 3                                    ## L:682  |      ^ | temp0 += texOffset; temp0 <<= 3;
  vxor $v05, $v00, $v00.e0                           ## L:696  |    160 | vec16 texOffsetTotal = 0;
  vmudm $v01, $v01, $v31.e6                          ## L:671  |   *162 | uvStart >>= 7;
  mtc2 $t4, $v05.e4                                  ## L:698  |      ^ | texOffsetTotal.X = temp1:s16;
  mtc2 $a2, $v05.e0                                  ## L:697  | ***166 | texOffsetTotal.x = temp0:s16;
  vlt $v01, $v01, $v00.e0                            ## L:672  |      ^ | uvStart = uvStart < 0;
  vsubc $v06, $v00, $v01.v                           ## L:694  | ***170 | vec16 uvStartNeg = VZERO - uvStart;
  slv $v03, 0, 28, $s3                               ## L:760  |      ^ | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vand $v05, $v05, $v12.e3                           ## L:699  |    171 | texOffsetTotal &= texMirrorMask.w;
  vge $v29, $v11, $v05.h0                            ## L:702  | ***175 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vmrg $v01, $v06, $v01                              ## L:702  |    176 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vlt $v04, $v04, $v15                               ## L:705  |    177 | posEnd = min(posEnd, screenMax);
  addiu $t2, $zero, 36                               ## L:739  |      ^ | cmdRdpRect = 0x24;
  addiu $at, $zero, 3                                ## L:773  |    178 | if(clipA == 0b0000'0011) {
  vge $v08, $v08, $v16                               ## L:706  |      ^ | posStart = max(posStart, screenMin);
  mfc2 $sp, $v04.e1                                  ## L:1188 |  **181 | outA = pos.y;
  vaddc $v01, $v01, $v05.v                           ## L:708  |      ^ | uvStart += texOffsetTotal;
  mfc2 $s1, $v08.e1                                  ## L:1188 |    182 | outA = pos.y;
  andi $sp, $sp, 4095                                ## L:1189 |   *184 | outA &= 0b1111'1111'1111;
  mfc2 $k0, $v08.e0                                  ## L:1190 |    185 | u32 tmp = pos.x;
  vsubc $v06, $v12, $v01.v                           ## L:713  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  mfc2 $fp, $v08.e5                                  ## L:1194 |    186 | outB = pos.Y;
  andi $s1, $s1, 4095                                ## L:1189 |    187 | outA &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1191 |    188 | tmp <<= 12;
  or $s1, $s1, $k0                                   ## L:1192 |    189 | outA |= tmp;
  vaddc $v06, $v06, $v12.e2                          ## L:714  |      ^ | uvStartNeg += texMirrorMask.z;
  vge $v29, $v11, $v05.h0                            ## L:716  |    190 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  mfc2 $k0, $v08.e4                                  ## L:1196 |      ^ | tmp = pos.X;
  ldv $v05, 0, 16, $s6                               ## L:734  |    191 | vec16 color = load(ptrIn, 16).xyzw;
  andi $fp, $fp, 4095                                ## L:1195 |    192 | outB &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1197 |    193 | tmp <<= 12;
  vmrg $v01, $v01, $v06                              ## L:716  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  vlt $v06, $v08, $v04                               ## L:723  |    194 | vec16 extend = posStart < posEnd;
  or $fp, $fp, $k0                                   ## L:1198 |      ^ | outB |= tmp;
  sw $s1, 4 + 16($s3)                                ## L:756  |    195 | store(posA, dmaDmemEnd, 0x04, 16);
  mfc2 $k0, $v04.e0                                  ## L:1190 |    196 | u32 tmp = pos.x;
  mfc2 $k1, $v04.e5                                  ## L:1194 |    197 | outB = pos.Y;
  ldv $v08, 8, 32, $s6                               ## L:765  |    198 | posStart.XYZW = load(ptrIn, 24).XYZW;
  ldv $v08, 0, 24, $s6                               ## L:764  |    199 | posStart.xyzw = load(ptrIn, 24).xyzw;
  sll $k0, $k0, 12                                   ## L:1191 |    200 | tmp <<= 12;
  or $sp, $sp, $k0                                   ## L:1192 |    201 | outA |= tmp;
  mfc2 $k0, $v04.e4                                  ## L:1196 |    202 | tmp = pos.X;
  sw $sp, 0 + 16($s3)                                ## L:757  |    203 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  andi $k1, $k1, 4095                                ## L:1195 |    204 | outB &= 0b1111'1111'1111;
  vmulf $v14, $v13, $v08.h3                          ## L:766  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  sll $k0, $k0, 12                                   ## L:1197 |    205 | tmp <<= 12;
  sb $t2, 0 + 16($s3)                                ## L:758  |    206 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  cfc2 $t4, $vcc                                     ## L:724  |    207 | temp1 = get_vcc();
  slv $v05, 0, 4, $s3                                ## L:747  |    208 | store(color.xy, dmaDmemEnd, 4);
  or $k1, $k1, $k0                                   ## L:1198 |    209 | outB |= tmp;
  slv $v01, 0, 24, $s3                               ## L:759  |    210 | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  andi $a2, $t4, 3                                   ## L:726  |    211 | temp0 = temp1 & 0b0000'0011;
  or $t1, $t1, $a2                                   ## L:727  |    212 | clipA |= temp0;
  andi $a2, $t4, 48                                  ## L:729  |    213 | temp0 = temp1 & 0b0011'0000;
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_0019       ## L:773  |    214 | if(clipA == 0b0000'0011) {
  or $t5, $t5, $a2                                   ## L:730  |    215 | clipB |= temp0;
  addiu $s3, $s3, 32                                 ## L:774  |    216 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_0019:
  addiu $at, $zero, 48                               ## L:784  |    217 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001A       ## L:784  |    218 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 24                                 ## L:778  |    219 | ptrIn += 24;
  slv $v01, 8, 24, $s3                               ## L:792  |    220 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  slv $v05, 4, 4, $s3                                ## L:785  |    221 | store(color.zw, dmaDmemEnd, 4);
  sw $k1, 0 + 16($s3)                                ## L:789  |    222 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sw $fp, 4 + 16($s3)                                ## L:788  |    223 | store(posB, dmaDmemEnd, 0x04, 16);
  ssv $v02, 12, 12, $s3                              ## L:786  |    224 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  sb $t2, 0 + 16($s3)                                ## L:790  |    225 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  addiu $s3, $s3, 32                                 ## L:795  |    226 | dmaDmemEnd += 32;
  slv $v03, 8, -4, $s3                               ## L:793  |    227 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  LABEL_TPXCmd_DrawTextured_001A:
  vmudn $v10, $v28, $v08.h0                          ## L:94   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:94   |    228 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:95   |    229 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:95   |    230 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:96   |    231 | out = mat2 +* vec.zzzzZZZZ;
  sltu $at, $s3, $s5                                 ## L:802  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v23, $v08.h2                          ## L:96   |    232 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:97   |    233 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_001B     ## L:802  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:97   |    234 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:803  |    235 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:803  |    236 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:804  |   *238 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_001B:
  vch $v29, $v09, $v09.h3                            ## L:807  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:807  |    239 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_0017       ## L:807  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:807  |    240 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0018:
  beq $zero, $zero, LABEL_TPXCmd_DrawTextured_0016   ## L:807  |    241 | temp1 = clip(posClip, posClip.wwwwWWWW);
  nop                                                ## L:807  |   *243 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0015:
  lpv $v08, 0, 0, $s6                                ## L:258  |      1 | vec16 posStart = load_vec_s8(ptrIn, 0x00);
  vxor $v07, $v00, $v30.e7                           ## L:263  |      ^ | const vec16 vecOne = 1;
  vmulf $v14, $v13, $v08.h3                          ## L:259  |   ***5 | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  vmudm $v08, $v08, $v31.e7                          ## L:260  |      6 | posStart >>= 8;
  vmudn $v10, $v28, $v08.h0                          ## L:94   |  ***10 | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:94   |     11 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:95   |     12 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:95   |     13 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:96   |     14 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:96   |     15 | out = mat2 +* vec.zzzzZZZZ;
  vmadn $v10, $v22, $v30.e7                          ## L:97   |     16 | out = mat3 +* 1;
  vmadh $v09, $v21, $v30.e7                          ## L:97   |     17 | out = mat3 +* 1;
  vch $v29, $v09, $v09.h3                            ## L:268  |  ***21 | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:268  |     22 | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:268  |     23 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_001C:
  vmudl $v10, $v10, $v17.v                           ## L:279  |      ^ | posClip *= normScaleW:ufract;
  vmadm $v09, $v09, $v17.v                           ## L:279  |     24 | posClip *= normScaleW:ufract;
  lb $a2, 11($s6)                                    ## L:326  |      ^ | temp0:s8 = load(ptrIn, 0x0B);
  ori $at, $zero, %lo(BASE_SIZE)                     ## L:285  |     25 | posClip:sint.w = load(BASE_SIZE).x;
  vmadn $v10, $v00, $v00                             ## L:279  |      ^ | posClip *= normScaleW:ufract;
  addiu $t2, $zero, 36                               ## L:386  |   **28 | cmdRdpRect = 0x24;
  vrcph $v05.e3, $v09.e3                             ## L:281  |      ^ | invW.w = invert_half(posClip).w;
  vrcpl $v06.e3, $v10.e3                             ## L:281  |     29 | invW.w = invert_half(posClip).w;
  vrcph $v05.e3, $v09.e7                             ## L:282  |     30 | invW.W = invert_half(posClip).W;
  lsv $v09, 14, 0, $at                               ## L:286  |      ^ | posClip:sint.W = load(BASE_SIZE).x;
  andi $t5, $t4, 16448                               ## L:276  |     31 | clipB = temp1 & 0b0100'0000'0100'0000;
  vrcpl $v06.e7, $v10.e7                             ## L:282  |      ^ | invW.W = invert_half(posClip).W;
  vmov $v10.e7, $v00.e0                              ## L:288  |     32 | posClip:sfract.W = 0;
  vmov $v10.e3, $v00.e0                              ## L:287  |     33 | posClip:sfract.w = 0;
  lsv $v09, 6, 0, $at                                ## L:285  |      ^ | posClip:sint.w = load(BASE_SIZE).x;
  vrcph $v05.e7, $v00.e7                             ## L:282  |     34 | invW.W = invert_half(posClip).W;
  vmudl $v29, $v10, $v06.h3                          ## L:291  |   **37 | posClip *= invW.wwwwWWWW;
  vmadm $v29, $v09, $v06.h3                          ## L:291  |     38 | posClip *= invW.wwwwWWWW;
  vmadn $v10, $v10, $v05.h3                          ## L:291  |     39 | posClip *= invW.wwwwWWWW;
  vmadh $v09, $v09, $v05.h3                          ## L:291  |     40 | posClip *= invW.wwwwWWWW;
  vmulf $v14, $v14, $v09.h3                          ## L:294  |  ***44 | localPartSize:sfract *= posClip:sint.wwwwWWWW;
  vmudl $v29, $v10, $v20.v                           ## L:297  |     45 | vec32 posScreen = posClip * screenSize;
  vmadm $v29, $v09, $v20.v                           ## L:297  |     46 | vec32 posScreen = posClip * screenSize;
  andi $t1, $t4, 1028                                ## L:275  |      ^ | clipA = temp1 & 0b0000'0100'0000'0100;
  vmadn $v04, $v10, $v19.v                           ## L:297  |     47 | vec32 posScreen = posClip * screenSize;
  vmadh $v03, $v09, $v19.v                           ## L:297  |     48 | vec32 posScreen = posClip * screenSize;
  lb $t4, 15($s6)                                    ## L:327  |      ^ | temp1:s8 = load(ptrIn, 0x0F);
  vmadh $v02, $v18, $v07.v                           ## L:298  |     49 | vec16 posCenter = screenOffset:sint +* vecOne;
  vmadh $v04, $v07, $v14.v                           ## L:303  |    *51 | vec16 posEnd = vecOne +* localPartSize:sint;
  addu $t4, $t4, $s2                                 ## L:330  |      ^ | temp1 += texOffset; temp1 <<= 3;
  vsubc $v08, $v02, $v14.v                           ## L:304  |    *53 | posStart = posCenter - localPartSize:sint;
  vmudn $v14, $v14, $v30.e6                          ## L:308  |     54 | localPartSize *= 2;
  vrcp $v06.e0, $v14.e0                              ## L:309  |  ***58 | invW.x = invert_half(localPartSize).x;
  vrcph $v05.e0, $v14.e0                             ## L:309  |     59 | invW.x = invert_half(localPartSize).x;
  vrcp $v06.e1, $v14.e1                              ## L:310  |     60 | invW.y = invert_half(localPartSize).y;
  vrcph $v05.e1, $v14.e1                             ## L:310  |     61 | invW.y = invert_half(localPartSize).y;
  vrcp $v06.e4, $v14.e4                              ## L:311  |     62 | invW.X = invert_half(localPartSize).X;
  vrcph $v05.e4, $v14.e4                             ## L:311  |     63 | invW.X = invert_half(localPartSize).X;
  vrcp $v06.e5, $v14.e5                              ## L:312  |     64 | invW.Y = invert_half(localPartSize).Y;
  vrcph $v05.e5, $v14.e5                             ## L:312  |     65 | invW.Y = invert_half(localPartSize).Y;
  addu $a2, $a2, $s2                                 ## L:329  |      ^ | temp0 += texOffset; temp0 <<= 3;
  sll $a2, $a2, 3                                    ## L:329  |  ***69 | temp0 += texOffset; temp0 <<= 3;
  vmudh $v01, $v08, $v05.v                           ## L:317  |      ^ | vec16 uvStart = posStart * invW:sint;
  vor $v03, $v00, $v05                               ## L:313  |     70 | vec16 uvDelta = invW:sint;
  sll $t4, $t4, 3                                    ## L:330  |      ^ | temp1 += texOffset; temp1 <<= 3;
  vxor $v05, $v00, $v00.e0                           ## L:343  |     71 | vec16 texOffsetTotal = 0;
  vmudm $v01, $v01, $v31.e6                          ## L:318  |    *73 | uvStart >>= 7;
  mtc2 $t4, $v05.e4                                  ## L:345  |      ^ | texOffsetTotal.X = temp1:s16;
  vlt $v01, $v01, $v00.e0                            ## L:319  |  ***77 | uvStart = uvStart < 0;
  mtc2 $a2, $v05.e0                                  ## L:344  |      ^ | texOffsetTotal.x = temp0:s16;
  vsubc $v06, $v00, $v01.v                           ## L:341  |  ***81 | vec16 uvStartNeg = VZERO - uvStart;
  vand $v05, $v05, $v12.e3                           ## L:346  |     82 | texOffsetTotal &= texMirrorMask.w;
  slv $v03, 0, 28, $s3                               ## L:407  |      ^ | store(uvDelta.xy, dmaDmemEnd, 0x0C, 16);
  vge $v29, $v11, $v05.h0                            ## L:349  |  ***86 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vmrg $v01, $v06, $v01                              ## L:349  |     87 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStartNeg : uvStart;
  vlt $v04, $v04, $v15                               ## L:352  |     88 | posEnd = min(posEnd, screenMax);
  addiu $at, $zero, 3                                ## L:420  |      ^ | if(clipA == 0b0000'0011) {
  vge $v08, $v08, $v16                               ## L:353  |     89 | posStart = max(posStart, screenMin);
  vaddc $v01, $v01, $v05.v                           ## L:355  |    *91 | uvStart += texOffsetTotal;
  ssv $v02, 4, 12, $s3                               ## L:398  |      ^ | store(posCenter.z, dmaDmemEnd, 0x04, 8);
  mfc2 $sp, $v04.e1                                  ## L:1188 |     92 | outA = pos.y;
  mfc2 $fp, $v08.e5                                  ## L:1194 |     93 | outB = pos.Y;
  andi $sp, $sp, 4095                                ## L:1189 |    *95 | outA &= 0b1111'1111'1111;
  vsubc $v06, $v12, $v01.v                           ## L:360  |      ^ | uvStartNeg = texMirrorMask - uvStart;
  mfc2 $k0, $v08.e0                                  ## L:1190 |     96 | u32 tmp = pos.x;
  mfc2 $s1, $v08.e1                                  ## L:1188 |     97 | outA = pos.y;
  andi $fp, $fp, 4095                                ## L:1195 |     98 | outB &= 0b1111'1111'1111;
  sll $k0, $k0, 12                                   ## L:1191 |     99 | tmp <<= 12;
  vaddc $v06, $v06, $v12.e2                          ## L:361  |      ^ | uvStartNeg += texMirrorMask.z;
  vge $v29, $v11, $v05.h0                            ## L:363  |    100 | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  andi $s1, $s1, 4095                                ## L:1189 |      ^ | outA &= 0b1111'1111'1111;
  or $s1, $s1, $k0                                   ## L:1192 |    101 | outA |= tmp;
  mfc2 $k0, $v08.e4                                  ## L:1196 |    102 | tmp = pos.X;
  mfc2 $k1, $v04.e5                                  ## L:1194 |    103 | outB = pos.Y;
  ldv $v05, 0, 8, $s6                                ## L:383  |    104 | vec16 color = load(ptrIn, 8).xyzw;
  sll $k0, $k0, 12                                   ## L:1197 |    105 | tmp <<= 12;
  andi $k1, $k1, 4095                                ## L:1195 |    106 | outB &= 0b1111'1111'1111;
  vmrg $v01, $v01, $v06                              ## L:363  |      ^ | uvStart = texMirrorCompare >= texOffsetTotal.xxxxXXXX ? uvStart : uvStartNeg;
  or $fp, $fp, $k0                                   ## L:1198 |    107 | outB |= tmp;
  slv $v05, 0, 4, $s3                                ## L:394  |    108 | store(color.xy, dmaDmemEnd, 4);
  vlt $v06, $v08, $v04                               ## L:370  |      ^ | vec16 extend = posStart < posEnd;
  mfc2 $k0, $v04.e0                                  ## L:1190 |    109 | u32 tmp = pos.x;
  cfc2 $t4, $vcc                                     ## L:371  |    110 | temp1 = get_vcc();
  lpv $v08, 0, 16, $s6                               ## L:415  |    111 | posStart = load_vec_s8(ptrIn, 16);
  sll $k0, $k0, 12                                   ## L:1191 |    112 | tmp <<= 12;
  andi $a2, $t4, 3                                   ## L:373  |    113 | temp0 = temp1 & 0b0000'0011;
  or $sp, $sp, $k0                                   ## L:1192 |    114 | outA |= tmp;
  sw $sp, 0 + 16($s3)                                ## L:404  |    115 | @Barrier("pos-cmd") store(posEndA, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v01, 0, 24, $s3                               ## L:406  |    116 | store(uvStart.xy, dmaDmemEnd, 0x08, 16);
  mfc2 $k0, $v04.e4                                  ## L:1196 |    117 | tmp = pos.X;
  sb $t2, 0 + 16($s3)                                ## L:405  |    118 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  or $t1, $t1, $a2                                   ## L:374  |    119 | clipA |= temp0;
  sw $s1, 4 + 16($s3)                                ## L:403  |    120 | store(posA, dmaDmemEnd, 0x04, 16);
  vmulf $v14, $v13, $v08.h3                          ## L:416  |      ^ | localPartSize = globalPartSize:sfract * posStart:sfract.wwwwWWWW;
  andi $a2, $t4, 48                                  ## L:376  |    121 | temp0 = temp1 & 0b0011'0000;
  or $t5, $t5, $a2                                   ## L:377  |    122 | clipB |= temp0;
  sll $k0, $k0, 12                                   ## L:1197 |    123 | tmp <<= 12;
  vmudm $v08, $v08, $v31.e7                          ## L:417  |      ^ | posStart >>= 8;
  bne $t1, $at, LABEL_TPXCmd_DrawTextured_001E       ## L:420  |    124 | if(clipA == 0b0000'0011) {
  or $k1, $k1, $k0                                   ## L:1198 |   *126 | outB |= tmp;
  addiu $s3, $s3, 32                                 ## L:421  |    127 | dmaDmemEnd += 32;
  LABEL_TPXCmd_DrawTextured_001E:
  addiu $at, $zero, 48                               ## L:431  |    128 | if(clipB == 0b0011'0000) {
  bne $t5, $at, LABEL_TPXCmd_DrawTextured_001F       ## L:431  |    129 | if(clipB == 0b0011'0000) {
  addiu $s6, $s6, 16                                 ## L:427  |   *131 | ptrIn += 16;
  sw $k1, 0 + 16($s3)                                ## L:436  |    132 | @Barrier("pos-cmd") store(posEndB, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  sw $fp, 4 + 16($s3)                                ## L:435  |    133 | store(posB, dmaDmemEnd, 0x04, 16);
  slv $v03, 8, 28, $s3                               ## L:440  |    134 | store(uvDelta.XY, dmaDmemEnd, 0x0C, 16);
  sb $t2, 0 + 16($s3)                                ## L:437  |    135 | @Barrier("pos-cmd") store(cmdRdpRect, dmaDmemEnd, 0x00, 16); ## Barrier: 0x1
  slv $v01, 8, 24, $s3                               ## L:439  |    136 | store(uvStart.XY, dmaDmemEnd, 0x08, 16);
  ssv $v02, 12, 12, $s3                              ## L:433  |    137 | store(posCenter.Z, dmaDmemEnd, 0x04, 8);
  addiu $s3, $s3, 32                                 ## L:442  |    138 | dmaDmemEnd += 32;
  slv $v05, 4, -28, $s3                              ## L:432  |    139 | store(color.zw, dmaDmemEnd, 4);
  LABEL_TPXCmd_DrawTextured_001F:
  vmudn $v10, $v28, $v08.h0                          ## L:94   |      ^ | out = mat0  * vec.xxxxXXXX;
  vmadh $v09, $v27, $v08.h0                          ## L:94   |    140 | out = mat0  * vec.xxxxXXXX;
  vmadn $v10, $v26, $v08.h1                          ## L:95   |    141 | out = mat1 +* vec.yyyyYYYY;
  vmadh $v09, $v25, $v08.h1                          ## L:95   |    142 | out = mat1 +* vec.yyyyYYYY;
  vmadn $v10, $v24, $v08.h2                          ## L:96   |    143 | out = mat2 +* vec.zzzzZZZZ;
  vmadh $v09, $v23, $v08.h2                          ## L:96   |    144 | out = mat2 +* vec.zzzzZZZZ;
  sltu $at, $s3, $s5                                 ## L:449  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadn $v10, $v22, $v30.e7                          ## L:97   |    145 | out = mat3 +* 1;
  bne $at, $zero, LABEL_TPXCmd_DrawTextured_0020     ## L:449  |      ^ | if(dmaDmemEnd >= dmaDmemFlush) {
  vmadh $v09, $v21, $v30.e7                          ## L:97   |   *147 | out = mat3 +* 1;
  jal RDPQ_Send                                      ## L:450  |    148 | RDPQ_Send(dmaDmem, dmaDmemEnd); ## Args: $s4, $s3
  nop                                                ## L:450  |   *150 | RDPQ_Send(dmaDmem, dmaDmemEnd);
  or $s3, $zero, $s4                                 ## L:451  |    151 | dmaDmemEnd = dmaDmem;
  LABEL_TPXCmd_DrawTextured_0020:
  vch $v29, $v09, $v09.h3                            ## L:454  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  vcl $v29, $v10, $v10.h3                            ## L:454  |    152 | temp1 = clip(posClip, posClip.wwwwWWWW);
  bne $s6, $s7, LABEL_TPXCmd_DrawTextured_001C       ## L:454  |      ^ | temp1 = clip(posClip, posClip.wwwwWWWW);
  cfc2 $t4, $vcc                                     ## L:454  |   *154 | temp1 = clip(posClip, posClip.wwwwWWWW);
  LABEL_TPXCmd_DrawTextured_0016:
  j RDPQ_Send                                        ## L:1177 | *****************************************************************************************244 | goto RDPQ_Send;
  ori $ra, $zero, %lo(RSPQ_Loop)                     ## L:1176 |   *246 | RA = RSPQ_Loop;

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