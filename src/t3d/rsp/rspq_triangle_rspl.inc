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
.set noat

T3DCmd_TriDraw:
  ## hot-path cycles: 2
  lbu $v0, %lo(FACE_CULLING + 0)                     ## L:69   |      ^ | u8<$v0> cullFlag = load(FACE_CULLING);
  srl $a2, $a1, 16                                   ## L:68   |      2 | u32<$a2> vert2 = vert1 >> 16;
RDPQ_Triangle_Send_Async:
  ## hot-path cycles: 142
  mtc2 $a0, $v01.e3                                  ## L:93   |      ^ | vall1.w = vtx1;
  mtc2 $a1, $v02.e3                                  ## L:94   |      2 | vall2.w = vtx2;
  llv $v01, 0, 0, $a0                                ## L:95   |      3 | vall1.xy = load(vtx1, 0x00).xy;
  lhu $t3, 6($a0)                                    ## L:100  |      4 | u16<$t3> clip1 = load(vtx1, 0x06);
  lhu $s7, %lo(RDPQ_TRI_BUFF_OFFSET + 0)             ## L:133  |      5 | u16<$s7> buffOffset = load(RDPQ_TRI_BUFF_OFFSET);
  lhu $t4, 6($a1)                                    ## L:101  |      6 | u16<$t4> clip2 = load(vtx2, 0x06);
  llv $v02, 0, 0, $a1                                ## L:96   |      7 | vall2.xy = load(vtx2, 0x00).xy;
  vor $v06, $v00, $v01.e1                            ## L:106  |      ^ | vec16<$v06> vy1 = vall1.y;
  addiu $at, $zero, 255                              ## L:138  |      8 | if(rejCodes != 0xFF)goto JrRa;
  lhu $t5, 6($a2)                                    ## L:102  |      9 | u16<$t5> clip3 = load(vtx3, 0x06);
  mtc2 $a2, $v03.e3                                  ## L:97   |     10 | vall3.w = vtx3;
  or $t3, $t3, $t4                                   ## L:109  |     11 | clip1 |= clip2;
  vge $v09, $v06, $v02.e1                            ## L:123  |      ^ | vytmp1 = max(vy1, vall2.y);
  llv $v03, 0, 0, $a2                                ## L:98   |     12 | vall3.xy = load(vtx3, 0x00).xy;
  vlt $v06, $v06, $v02.e1                            ## L:124  |      ^ | vy1 = min(vy1, vall2.y);
  or $t3, $t3, $t5                                   ## L:110  |     13 | clip1 |= clip3;
  vmrg $v04, $v02, $v01                              ## L:125  |      ^ | valltmp1 = select(vall2, vall1);
  vmrg $v01, $v01, $v02                              ## L:126  |     14 | vall1 = select(vall1, vall2);
  andi $t6, $t3, 255                                 ## L:137  |      ^ | u32<$t6> rejCodes = clip1 & 0xFF;
  vaddc $v28, $v00, $v00.v                           ## L:131  |     15 | vec16<$v28> vhmlupp = VZERO + VZERO;
  bne $t6, $at, JrRa                                 ## L:138  |      ^ | if(rejCodes != 0xFF)goto JrRa;
  cfc2 $t0, $vcc                                     ## L:127  |     16 | did_swap_0 = get_vcc();
  vlt $v06, $v06, $v03.e1                            ## L:141  |     17 | vy1 = min(vy1, vall3.y);
  andi $t4, $t3, 7936                                ## L:161  |      ^ | u32<$t4> clipCodes = clip1 & 0x1F00;
  vmrg $v05, $v03, $v01                              ## L:142  |     18 | valltmp2 = select(vall3, vall1);
  vmrg $v01, $v01, $v03                              ## L:143  |     19 | vall1 = select(vall1, vall3);
  addiu $s3, $s7, %lo(CLIP_BUFFER_TMP)               ## L:162  |      ^ | u16<$s3> outPtr = buffOffset + CLIP_BUFFER_TMP;
  cfc2 $t1, $vcc                                     ## L:144  |     20 | did_swap_1 = get_vcc();
  ssv $v06, 0, 6, $s3                                ## L:163  |     21 | @Barrier("tri-out") store(vy1.x, outPtr, 6); ## Barrier: 0x1
  lw $a3, %lo(TRI_COMMAND + 0)                       ## L:146  |     22 | u32<$a3> triCmd = load(TRI_COMMAND);
  vlt $v07, $v09, $v05.e1                            ## L:154  |      ^ | vy2 = min(vytmp1, valltmp2.y);
  cfc2 $t5, $vcc                                     ## L:157  |     23 | u32<$t5> did_swap_2 = get_vcc();
  vmrg $v03, $v05, $v04                              ## L:155  |      ^ | vall3 = select(valltmp2, valltmp1);
  vmrg $v02, $v04, $v05                              ## L:156  |     24 | vall2 = select(valltmp1, valltmp2);
  bne $t4, $zero, RDPQ_Triangle_Clip                 ## L:167  |      ^ | if(clipCodes != 0)goto RDPQ_Triangle_Clip;
  srl $t2, $t3, 13                                   ## L:152  |     25 | u32<$t2> tileIdx = clip1 >> 13;
  xor $t0, $t0, $t1                                  ## L:170  |     26 | did_swap_0 ^= did_swap_1;
  vmov $v23.e0, $v01.e0                              ## L:188  |      ^ | vx12F.x = vall1.x;
  mfc2 $a2, $v03.e3                                  ## L:184  |     27 | vtx3 = vall3.w;
  mfc2 $a0, $v01.e3                                  ## L:182  |     28 | vtx1 = vall1.w;
  vsubc $v05, $v02, $v01.v                           ## L:178  |      ^ | vec16<$v05> vm = vall2 - vall1;
  vsubc $v24, $v03, $v01.v                           ## L:176  |     29 | vec16<$v24> vhml = vall3 - vall1;
  xor $t0, $t0, $t5                                  ## L:171  |      ^ | did_swap_0 ^= did_swap_2;
  andi $t0, $t0, 1                                   ## L:194  |     30 | did_swap_0 &= 1;
  vmudn $v26, $v06, $v31.e1                          ## L:203  |      ^ | vec16<$v26> vfyF = vy1 << 14;
  mfc2 $a1, $v02.e3                                  ## L:183  |     31 | vtx2 = vall2.w;
  xor $t1, $v0, $t0                                  ## L:195  |     32 | u32<$t1> cullFlip = cull ^ did_swap_0;
  vsubc $v29, $v00, $v05.e1                          ## L:200  |      ^ | VTEMP = VZERO - vm.y;
  vsubc $v04, $v03, $v02.v                           ## L:179  |     33 | vec16<$v04> vl = vall3 - vall2;
  lsv $v08, 0, 30, $a0                               ## L:242  |      ^ | vwF.x = load(vtx1, 0x1E).x;
  vsubc $v26, $v00, $v26.v                           ## L:237  |     34 | vfyF = VZERO - vfyF;
  ssv $v03, 2, 2, $s3                                ## L:192  |      ^ | @Barrier("tri-out") store(vall3.y, outPtr, 2); ## Barrier: 0x1
  vmudh $v19, $v05, $v24.e1                          ## L:214  |     35 | vnzF = vm:sint * vhml:sint.y;
  xori $s7, $s7, 176                                 ## L:205  |      ^ | buffOffset ^= 176;
  vmadh $v19, $v24, $v29.v                           ## L:215  |     36 | vnzF = vhml:sint +* VTEMP:sint;
  ssv $v07, 0, 4, $s3                                ## L:207  |      ^ | @Barrier("tri-out") store(vy2.x, outPtr, 4); ## Barrier: 0x1
  vsar $v18, COP2_ACC_HI                             ## L:216  |     37 | vnzI = get_acc_high();
  slv $v05, 0, 8, $s3                                ## L:225  |      ^ | @Barrier("tri-oA", before) @Barrier("tri-out") store(vm.xy, outPtr, 8); ## Barrier: 0x1
  addiu $t4, $s3, 32                                 ## L:231  |     38 | u32<$t4> color_dmem = outPtr + 0x20;
  vsar $v19, COP2_ACC_MD                             ## L:217  |      ^ | vnzF = get_acc_mid();
  vmov $v23.e2, $v02.e0                              ## L:189  |     39 | vx12F.z = vall2.x;
  slv $v04, 0, 12, $s3                               ## L:226  |      ^ | @Barrier("tri-oA", before) @Barrier("tri-out") store(vl.xy, outPtr, 12); ## Barrier: 0x1
  ldv $v24, 4, 8, $s3                                ## L:227  |     40 | @Barrier("tri-oA", after) @Barrier("tri-oA2", before) vhml.z = load(outPtr, 8).xyzw;
  vsub $v25, $v26, $v26.v                            ## L:238  |      ^ | vfyI:sint = vfyF - vfyF;
  vrcph $v20.e7, $v18.e0                             ## L:259  |     41 | vslope.W = invert_half(vnz).x;
  mfc2 $t0, $v18.e0                                  ## L:247  |      ^ | s32<$t0> nzSign = vnzI.x;
  vrcpl $v21.e7, $v19.e0                             ## L:259  |     42 | vslope.W = invert_half(vnz).x;
  lsv $v08, 4, 30, $a1                               ## L:243  |      ^ | vwF.z = load(vtx2, 0x1E).x;
  lsv $v08, 8, 30, $a2                               ## L:244  |     43 | vwF.X = load(vtx3, 0x1E).x;
  vrcph $v20.e7, $v00.e0                             ## L:259  |      ^ | vslope.W = invert_half(vnz).x;
  vrcp $v21.e0, $v24.e1                              ## L:263  |     44 | vslope.x = invert_half(vhml).y;
  TAG_CullFlipA: slt $t0, $t0, $zero                                ## L:264  |      ^ | @Tag("CullFlipA") asm_op("slt", nzSign, nzSign, ZERO);
  beq $t0, $t1, JrRa                                 ## L:265  |     45 | if(nzSign == cullFlip)goto JrRa;
  vrcph $v20.e0, $v24.e1                             ## L:263  |     46 | vslope.x = invert_half(vhml).y;
  vrcp $v21.e2, $v05.e1                              ## L:280  |     47 | vslope.z = invert_half(vm).y;
  TAG_CullFlipB: xori $t0, $t0, 1                                   ## L:293  |      ^ | @Tag("CullFlipB") nzSign ^= 1;
  lsv $v07, 8, 22, $a2                               ## L:286  |     48 | vwI.X = load(vtx3, 0x16).x;
  vmov $v28.e7, $v18.e0                              ## L:277  |      ^ | vhmlupp.W = vnzI.x;
  vmov $v24.e7, $v19.e0                              ## L:276  |     49 | vhml.W = vnzF.x;
  lsv $v07, 0, 22, $a0                               ## L:284  |      ^ | vwI.x = load(vtx1, 0x16).x;
  lsv $v07, 4, 22, $a1                               ## L:285  |     50 | vwI.z = load(vtx2, 0x16).x;
  vrcph $v20.e2, $v05.e1                             ## L:280  |      ^ | vslope.z = invert_half(vm).y;
  vrcp $v21.e4, $v04.e1                              ## L:282  |     51 | vslope.X = invert_half(vl).y;
  luv $v09, 0, 8, $a0                                ## L:297  |      ^ | vec16<$v09> vattr1 = load_vec_u8(vtx1, 0x08);
  vrcph $v20.e4, $v04.e1                             ## L:282  |     52 | vslope.X = invert_half(vl).y;
  lsv $v15, 10, 34, $a1                              ## L:326  |      ^ | vinvwF.Y = load(vtx2, 0x22).x;
  vsubc $v29, $v08, $v08.e2                          ## L:306  |     53 | VTEMP = vwF - vwF.z;
  llv $v04, 8, 12, $a0                               ## L:401  |      ^ | vtex1.XY = load(vtx1, 0x0C).xy;
  vlt $v07, $v07, $v07.e2                            ## L:307  |     54 | vwI = min(vwI, vwI.z);
  sll $t0, $t0, 7                                    ## L:294  |      ^ | nzSign <<= 7;
  lsv $v15, 12, 34, $a2                              ## L:314  |     55 | vinvwF.Z = load(vtx3, 0x22).x;
  vmrg $v08, $v08, $v08.e2                           ## L:309  |      ^ | vwF = select(vwF, vwF.z);
  lsv $v15, 8, 34, $a0                               ## L:325  |     56 | vinvwF.X = load(vtx1, 0x22).x;
  vmudl $v03, $v21, $v24.q1                          ## L:323  |      ^ | asm_op("vmudl", vtmpF, vslopeF, vhml.yywwYYWW);
  or $a3, $a3, $t0                                   ## L:308  |     57 | triCmd |= nzSign;
  vmadm $v03, $v20, $v24.q1                          ## L:324  |      ^ | asm_op("vmadm", vtmpF, vslopeI, vhml.yywwYYWW);
  andi $t5, $a3, 1024                                ## L:329  |     58 | u32<$t5> tex_dmem = triCmd & 0x400;
  vmadn $v03, $v21, $v28.v                           ## L:332  |      ^ | vtmpF = vslopeF:ufract +* vhmlupp:sint;
  vmudl $v09, $v09, $v31.e6                          ## L:424  |     59 | vattr1 >>>= 7;
  mfc2 $t0, $v24.e3                                  ## L:335  |      ^ | u32<$t0> negMY = vhml.w;
  lsv $v14, 8, 32, $a0                               ## L:353  |     60 | vinvwI.X = load(vtx1, 0x20).x;
  vmudn $v21, $v21, $v30.e5                          ## L:342  |      ^ | vslopeF = vslopeF:ufract * VSHIFT:sint.Y;
  vmadh $v20, $v20, $v30.e5                          ## L:343  |     61 | vslopeI = vslopeI:sint +* VSHIFT:sint.Y;
  lsv $v14, 12, 32, $a2                              ## L:352  |      ^ | vinvwI.Z = load(vtx3, 0x20).x;
  vsubc $v03, $v00, $v03.v                           ## L:339  |     62 | vtmpF = VZERO - vtmpF;
  lsv $v14, 10, 32, $a1                              ## L:351  |      ^ | vinvwI.Y = load(vtx2, 0x20).x;
  subu $t0, $zero, $t0                               ## L:417  |     63 | negMY = ZERO - negMY;
  vsubc $v29, $v08, $v08.e4                          ## L:347  |      ^ | VTEMP = vwF - vwF.X;
  vlt $v07, $v07, $v07.e4                            ## L:348  |     64 | vwI = min(vwI, vwI.X);
  andi $t6, $a3, 512                                 ## L:363  |      ^ | u32<$t6> z_dmem = triCmd & 0x200;
  vmrg $v08, $v08, $v08.e4                           ## L:349  |     65 | vwF = select(vwF, vwF.X);
  lsv $v09, 14, 4, $a0                               ## L:429  |      ^ | vattr1.W = load(vtx1, 0x04).x;
  mfc2 $t1, $v24.e0                                  ## L:336  |     66 | u32<$t1> negHX = vhml.x;
  vmudl $v29, $v21, $v03                             ## L:358  |      ^ | asm_op("vmudl", VTEMP, vslopeF, vtmpF);
  vmadm $v18, $v20, $v03.v                           ## L:359  |     67 | vnzI = vslopeI:sint +* vtmpF:ufract;
  srl $t5, $t5, 4                                    ## L:330  |      ^ | tex_dmem >>= 4;
  llv $v03, 8, 12, $a1                               ## L:402  |     68 | vtex2.XY = load(vtx2, 0x0C).xy;
  vsar $v19, COP2_ACC_LO                             ## L:360  |      ^ | vnzF = get_acc_low();
  vmudl $v29, $v15, $v08.e0                          ## L:388  |     69 | asm_op("vmudl", VTEMP, vinvwF, vwF.x);
  luv $v10, 0, 8, $a1                                ## L:298  |      ^ | vec16<$v10> vattr2 = load_vec_u8(vtx2, 0x08);
  vmadm $v29, $v14, $v08.e0                          ## L:389  |     70 | asm_op("vmadm", VTEMP, vinvwI, vwF.x);
  llv $v06, 8, 12, $a2                               ## L:403  |      ^ | vtex3.XY = load(vtx3, 0x0C).xy;
  subu $t1, $zero, $t1                               ## L:418  |     71 | negHX = ZERO - negHX;
  vmadn $v15, $v15, $v07.e0                          ## L:390  |      ^ | vinvwF = vinvwF:ufract +* vwI:sint.x;
  luv $v11, 0, 8, $a2                                ## L:296  |     72 | vec16<$v11> vattr3 = load_vec_u8(vtx3, 0x08);
  vmudn $v29, $v19, $v24.v                           ## L:411  |      ^ | VTEMP = vnzF:ufract * vhml:sint;
  vmadh $v29, $v18, $v24.v                           ## L:412  |     73 | VTEMP = vnzI:sint +* vhml:sint;
  mtc2 $t0, $v24.e3                                  ## L:419  |      ^ | vhml.w = negMY;
  vsar $v20, COP2_ACC_HI                             ## L:414  |     74 | vslopeI = get_acc_high();
  or $a3, $a3, $t2                                   ## L:370  |      ^ | triCmd |= tileIdx;
  lbu $t0, %lo(RDPQ_SYNCFULL_ONGOING + 0)            ## L:471  |     75 | u8<$t0> syncFull = load(RDPQ_SYNCFULL_ONGOING);
  vsar $v21, COP2_ACC_MD                             ## L:413  |      ^ | vslopeF = get_acc_mid();
  vsubc $v15, $v15, $v30.e7                          ## L:392  |     76 | vinvwF:ufract -= 1;
  mtc2 $t1, $v24.e0                                  ## L:420  |      ^ | vhml.x = negHX;
  vmudl $v11, $v11, $v31.e6                          ## L:426  |     77 | vattr3 >>>= 7;
  andi $t3, $a3, 256                                 ## L:450  |      ^ | u32<$t3> prim_size = triCmd & 0x100;
  vmudl $v10, $v10, $v31.e6                          ## L:425  |     78 | vattr2 >>>= 7;
  lsv $v11, 14, 4, $a2                               ## L:431  |      ^ | vattr3.W = load(vtx3, 0x04).x;
  vmudm $v22, $v23, $v31.e1                          ## L:468  |     79 | vec16<$v22> vx12I = vx12F >> 2;
  lsv $v10, 14, 4, $a1                               ## L:430  |      ^ | vattr2.W = load(vtx2, 0x04).x;
  vmudm $v15, $v15, $v31.e0                          ## L:393  |     80 | vinvwF >>= 1;
  addiu $at, $zero, 207                              ## L:438  |      ^ | set_vcc(0xCF);
  vmudn $v23, $v23, $v31.e1                          ## L:469  |     81 | vx12F <<= 14;
  addu $t5, $t5, $t4                                 ## L:365  |      ^ | tex_dmem += color_dmem;
  vge $v09, $v09, $v00.e0                            ## L:433  |     82 | vattr1 = vattr1 >= 0;
  sh $a3, 0($s3)                                     ## L:372  |      ^ | @Barrier("tri-out") store(triCmd:u16, outPtr, 0); ## Barrier: 0x1
  srl $t6, $t6, 3                                    ## L:364  |     83 | z_dmem >>= 3;
  vge $v10, $v10, $v00.e0                            ## L:434  |      ^ | vattr2 = vattr2 >= 0;
  sdv $v15, 8, 16, $s3                               ## L:455  |     84 | @Barrier("tri-oB", before) @Barrier("tri-out") store(vinvwF.XYZW, outPtr, 16); ## Barrier: 0x1
  vge $v11, $v11, $v00.e0                            ## L:435  |      ^ | vattr3 = vattr3 >= 0;
  mfc0 $t1, COP0_DMA_BUSY                            ## L:452  |     85 | u32<$t1> dmaBusy = get_dma_busy();
  vmudl $v16, $v21, $v26.e4                          ## L:448  |      ^ | vec32<$v16> vedges = vslope * vfy.X;
  vmadm $v16, $v20, $v26.e4                          ## L:448  |     86 | vec32<$v16> vedges = vslope * vfy.X;
  ctc2 $at, $vcc                                     ## L:438  |      ^ | set_vcc(0xCF);
  vmadn $v17, $v21, $v25.e4                          ## L:448  |     87 | vec32<$v16> vedges = vslope * vfy.X;
  lsv $v09, 12, 16, $s3                              ## L:456  |      ^ | @Barrier("tri-oB", after) @Barrier("tri-oB2", before) vattr1.Z = load(outPtr, 16).x;
  lsv $v11, 12, 20, $s3                              ## L:458  |     88 | @Barrier("tri-oB", after) @Barrier("tri-oB2", before) vattr3.Z = load(outPtr, 20).x;
  vmadh $v16, $v20, $v25.e4                          ## L:448  |      ^ | vec32<$v16> vedges = vslope * vfy.X;
  vmulf $v04, $v04, $v15.h0                          ## L:461  |     89 | vtex1:sfract *= vinvwF.xxxxXXXX;
  lsv $v10, 12, 18, $s3                              ## L:457  |      ^ | @Barrier("tri-oB", after) @Barrier("tri-oB2", before) vattr2.Z = load(outPtr, 18).x;
  vmulf $v03, $v03, $v15.h1                          ## L:462  |     90 | vtex2:sfract *= vinvwF.yyyyYYYY;
  bne $t1, $zero, LABEL_RDPQ_Triangle_Send_Async_0006## L:473  |      ^ | @Unlikely if(dmaBusy != 0)
  vmulf $v06, $v06, $v15.h2                          ## L:463  |     91 | vtex3:sfract *= vinvwF.zzzzZZZZ;
  LABEL_RDPQ_Triangle_Send_Async_0005:
  ssv $v20, 8, 12, $s3                               ## L:502  |     92 | @Barrier("tri-oA2", after) @Barrier("tri-out") store(vslope:sint.X, outPtr, 12); ## Barrier: 0x1
  ssv $v22, 4, 8, $s3                                ## L:503  |     93 | @Barrier("tri-oA2", after) @Barrier("tri-out") store(vx12I.z, outPtr, 8); ## Barrier: 0x1
  vmrg $v09, $v09, $v04                              ## L:494  |      ^ | vattr1 = select(vattr1, vtex1);
  srl $t3, $t3, 4                                    ## L:490  |     94 | prim_size >>= 4;
  vmrg $v10, $v10, $v03                              ## L:495  |      ^ | vattr2 = select(vattr2, vtex2);
  lw $a0, %lo(RDPQ_CURRENT + 0)                      ## L:507  |     95 | u32<$a0> rdramCur = load(RDPQ_CURRENT);
  vmrg $v11, $v11, $v06                              ## L:496  |      ^ | vattr3 = select(vattr3, vtex3);
  ssv $v21, 8, 14, $s3                               ## L:504  |     96 | @Barrier("tri-oA2", after) @Barrier("tri-out") store(vslope:ufract.X, outPtr, 14); ## Barrier: 0x1
  vaddc $v17, $v17, $v23.e0                          ## L:512  |      ^ | vedges += vx12.x;
  vadd $v16, $v16, $v22.e0                           ## L:512  |     97 | vedges += vx12.x;
  bne $t0, $zero, LABEL_RDPQ_Triangle_Send_Async_0008## L:520  |      ^ | if(syncFull == 0) {
  addu $t6, $t6, $t5                                 ## L:489  |     98 | z_dmem += tex_dmem;
  mtc0 $a0, COP0_DP_END                              ## L:521  |     99 | set_rdp_end(rdramCur);
  LABEL_RDPQ_Triangle_Send_Async_0008:
  vsubc $v29, $v00, $v00                             ## L:533  |      ^ | clear_vcc();
  vsub $v12, $v10, $v09.v                            ## L:543  |    100 | vec16<$v12> vma:sint = vattr2 - vattr1;
  ssv $v23, 4, 10, $s3                               ## L:529  |      ^ | @Barrier("tri-oA2", after) @Barrier("tri-out") store(vx12F.z, outPtr, 10); ## Barrier: 0x1
  addu $t3, $t3, $t6                                 ## L:597  |    101 | prim_size += z_dmem;
  vsub $v13, $v11, $v09.v                            ## L:544  |      ^ | vec16<$v13> vha:sint = vattr3 - vattr1;
  vmudn $v19, $v19, $v30.e5                          ## L:551  |    102 | vnz = vnz * 4;
  ssv $v20, 4, 28, $s3                               ## L:565  |      ^ | @Barrier("tri-out") store(vslope:sint.z, outPtr, 28); ## Barrier: 0x1
  ssv $v21, 4, 30, $s3                               ## L:566  |    103 | @Barrier("tri-out") store(vslope:ufract.z, outPtr, 30); ## Barrier: 0x1
  vmadh $v18, $v18, $v30.e5                          ## L:551  |      ^ | vnz = vnz * 4;
  ssv $v16, 4, 24, $s3                               ## L:568  |    104 | @Barrier("tri-out") store(vedges:sint.z, outPtr, 24); ## Barrier: 0x1
  vmudh $v29, $v12, $v24.e1                          ## L:558  |      ^ | VTEMP = vma:sint * vhml:sint.y;
  vmadh $v29, $v13, $v24.e3                          ## L:559  |    105 | VTEMP = vha:sint +* vhml:sint.w;
  ssv $v17, 4, 26, $s3                               ## L:569  |      ^ | @Barrier("tri-out") store(vedges:ufract.z, outPtr, 26); ## Barrier: 0x1
  ssv $v16, 0, 16, $s3                               ## L:570  |    106 | @Barrier("tri-oB2", after) @Barrier("tri-out") store(vedges:sint.x, outPtr, 16); ## Barrier: 0x1
  vsar $v03, COP2_ACC_HI                             ## L:561  |      ^ | vdxI = get_acc_high();
  ssv $v17, 0, 18, $s3                               ## L:571  |    107 | @Barrier("tri-oB2", after) @Barrier("tri-out") store(vedges:ufract.x, outPtr, 18); ## Barrier: 0x1
  vsar $v04, COP2_ACC_MD                             ## L:560  |      ^ | vdxF = get_acc_mid();
  lw $a2, %lo(RDPQ_SENTINEL + 0)                     ## L:553  |    108 | u32<$a2> rdramSentinel = load(RDPQ_SENTINEL);
  vmudh $v29, $v13, $v24.e2                          ## L:577  |      ^ | VTEMP = vha:sint * vhml:sint.z;
  ssv $v20, 0, 20, $s3                               ## L:585  |    109 | @Barrier("tri-oB2", after) @Barrier("tri-out") store(vslope:sint.x, outPtr, 20); ## Barrier: 0x1
  vmadh $v29, $v12, $v24.e0                          ## L:578  |      ^ | VTEMP = vma:sint +* vhml:sint.x;
  subu $t3, $t3, $s3                                 ## L:598  |    110 | prim_size -= outPtr;
  vsar $v08, COP2_ACC_MD                             ## L:579  |      ^ | vdyF = get_acc_mid();
  ssv $v21, 0, 22, $s3                               ## L:586  |    111 | @Barrier("tri-oB2", after) @Barrier("tri-out") store(vslope:ufract.x, outPtr, 22); ## Barrier: 0x1
  vsar $v07, COP2_ACC_HI                             ## L:580  |      ^ | vdyI = get_acc_high();
  vmudl $v29, $v04, $v19.e7                          ## L:591  |    112 | vdx *= vnz.W;
  mtc0 $s3, COP0_DMA_SPADDR                          ## L:588  |      ^ | @Barrier("tri-out") set_dma_addr_rsp(outPtr); ## Barrier: 0x1
  vmadm $v29, $v03, $v19.e7                          ## L:591  |    113 | vdx *= vnz.W;
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:600  |      ^ | @Barrier("tri-out") set_dma_addr_rdram(rdramCur); ## Barrier: 0x1
  vmadn $v04, $v04, $v18.e7                          ## L:591  |    114 | vdx *= vnz.W;
  addu $a0, $a0, $t3                                 ## L:606  |      ^ | rdramCur += prim_size;
  addu $s3, $s3, $t3                                 ## L:599  |    115 | outPtr += prim_size;
  vmadh $v03, $v03, $v18.e7                          ## L:591  |      ^ | vdx *= vnz.W;
  sh $s7, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)           ## L:602  |    116 | store(buffOffset, RDPQ_TRI_BUFF_OFFSET);
  vmudl $v29, $v08, $v19.e7                          ## L:593  |      ^ | vdy *= vnz.W;
  vmadm $v29, $v07, $v19.e7                          ## L:593  |    117 | vdy *= vnz.W;
  sltu $at, $a2, $a0                                 ## L:628  |      ^ | @Unlikely if(rdramSentinel < rdramCur)
  vmadn $v08, $v08, $v18.e7                          ## L:593  |    118 | vdy *= vnz.W;
  sdv $v04, 0, 24, $t4                               ## L:609  |      ^ | @Barrier("tri-out") store(vdx:ufract.xyzw, color_dmem, 0x18); ## Barrier: 0x1
  vmadh $v07, $v07, $v18.e7                          ## L:593  |    119 | vdy *= vnz.W;
  sdv $v04, 8, 24, $t5                               ## L:610  |      ^ | @Barrier("tri-out") store(vdx:ufract.XYZW, tex_dmem, 0x18); ## Barrier: 0x1
  vmadl $v05, $v04, $v21.e0                          ## L:615  |    120 | vec32<$v05> vde = vdx +* vslope.x;
  sdv $v03, 0, 8, $t4                                ## L:611  |      ^ | @Barrier("tri-out") store(vdx:sint.xyzw, color_dmem, 0x08); ## Barrier: 0x1
  sdv $v03, 8, 8, $t5                                ## L:612  |    121 | @Barrier("tri-out") store(vdx:sint.XYZW, tex_dmem, 0x08); ## Barrier: 0x1
  vmadm $v05, $v03, $v21.e0                          ## L:615  |      ^ | vec32<$v05> vde = vdx +* vslope.x;
  vmadn $v06, $v04, $v20.e0                          ## L:615  |    122 | vec32<$v05> vde = vdx +* vslope.x;
  sdv $v08, 0, 56, $t4                               ## L:619  |      ^ | @Barrier("tri-out") store(vdy:ufract.xyzw, color_dmem, 0x38); ## Barrier: 0x1
  vmadh $v05, $v03, $v20.e0                          ## L:615  |    123 | vec32<$v05> vde = vdx +* vslope.x;
  bne $at, $zero, LABEL_RDPQ_Triangle_Send_Async_0009## L:628  |      ^ | @Unlikely if(rdramSentinel < rdramCur)
  sdv $v08, 8, 56, $t5                               ## L:622  |    124 | @Barrier("tri-out") store(vdy:ufract.XYZW, tex_dmem, 0x38); ## Barrier: 0x1
  LABEL_RDPQ_Triangle_Send_Async_000A:
  sdv $v07, 0, 40, $t4                               ## L:663  |    125 | @Barrier("tri-out") store(vdy:sint.xyzw, color_dmem, 0x28); ## Barrier: 0x1
  vmudh $v29, $v09, $v30.e7                          ## L:659  |      ^ | VTEMP = vattr1:sint * 1;
  sdv $v07, 8, 40, $t5                               ## L:664  |    126 | @Barrier("tri-out") store(vdy:sint.XYZW, tex_dmem, 0x28); ## Barrier: 0x1
  vmadl $v01, $v06, $v26.e4                          ## L:660  |      ^ | vec32<$v01> vfinal = vde +* vfy.X;
  vmadm $v01, $v05, $v26.e4                          ## L:660  |    127 | vec32<$v01> vfinal = vde +* vfy.X;
  sw $a0, %lo(RDPQ_CURRENT)($zero)                   ## L:672  |      ^ | store(rdramCur, RDPQ_CURRENT);
  vmadn $v02, $v06, $v25.e4                          ## L:660  |    128 | vec32<$v01> vfinal = vde +* vfy.X;
  addiu $t3, $t3, 65535                              ## L:708  |      ^ | prim_size -= 1;
  vmadh $v01, $v05, $v25.e4                          ## L:660  |    129 | vec32<$v01> vfinal = vde +* vfy.X;
  sdv $v06, 0, 48, $t4                               ## L:666  |      ^ | @Barrier("tri-out") store(vde:ufract.xyzw, color_dmem, 0x30); ## Barrier: 0x1
  sdv $v06, 8, 48, $t5                               ## L:667  |    130 | @Barrier("tri-out") store(vde:ufract.XYZW, tex_dmem, 0x30); ## Barrier: 0x1
  vmov $v10.e3, $v04.e7                              ## L:696  |      ^ | vzout.w = vdxF.W;
  sdv $v05, 0, 32, $t4                               ## L:668  |    131 | @Barrier("tri-out") store(vde:sint.xyzw, color_dmem, 0x20); ## Barrier: 0x1
  vmov $v06.e6, $v05.e7                              ## L:693  |      ^ | vdeF.Z = vdeI.W;
  sdv $v05, 8, 32, $t5                               ## L:669  |    132 | @Barrier("tri-out") store(vde:sint.XYZW, tex_dmem, 0x20); ## Barrier: 0x1
  vmov $v10.e1, $v02.e7                              ## L:694  |      ^ | vzout.y = vfinalF.W;
  vmov $v10.e2, $v03.e7                              ## L:697  |    133 | vzout.z = vdxI.W;
  sdv $v02, 0, 16, $t4                               ## L:700  |      ^ | @Barrier("tri-out") store(vfinalF.xyzw, color_dmem, 0x10); ## Barrier: 0x1
  sdv $v02, 8, 16, $t5                               ## L:701  |    134 | @Barrier("tri-out") store(vfinalF.XYZW, tex_dmem, 0x10); ## Barrier: 0x1
  vmov $v08.e6, $v07.e7                              ## L:692  |      ^ | vdyF.Z = vdyI.W;
  sdv $v01, 0, 0, $t4                                ## L:702  |    135 | @Barrier("tri-out") store(vfinalI.xyzw, color_dmem, 0x00); ## Barrier: 0x1
  vmov $v10.e0, $v01.e7                              ## L:695  |      ^ | vzout.x = vfinalI.W;
  sdv $v01, 8, 0, $t5                                ## L:703  |    136 | @Barrier("tri-out") store(vfinalI.XYZW, tex_dmem, 0x00); ## Barrier: 0x1
  slv $v06, 12, 8, $t6                               ## L:706  |    137 | @Barrier("tri-out") store(vdeF.ZW, z_dmem, 0x08); ## Barrier: 0x1
  slv $v08, 12, 12, $t6                              ## L:707  |    138 | @Barrier("tri-out") store(vdyF.ZW, z_dmem, 0x0C); ## Barrier: 0x1
  sdv $v10, 0, 0, $t6                                ## L:709  |    139 | @Barrier("tri-out") store(vzout.xyzw, z_dmem, 0x00); ## Barrier: 0x1
  jr $ra                                             ## L:721  |    140 | }
  mtc0 $t3, COP0_DMA_WRITE                           ## L:720  |   *142 | @Barrier("tri-out", after) set_dma_write(prim_size);
  LABEL_RDPQ_Triangle_Send_Async_0006:
  mfc0 $t1, COP0_DMA_BUSY                            ## L:481  |      1 | dmaBusy = get_dma_busy();
  bne $t1, $zero, LABEL_RDPQ_Triangle_Send_Async_0006## L:481  |    **4 | dmaBusy = get_dma_busy();
  nop                                                ## L:481  |     *6 | dmaBusy = get_dma_busy();
  LABEL_RDPQ_Triangle_Send_Async_0007:
  j LABEL_RDPQ_Triangle_Send_Async_0005              ## L:481  |      7 | dmaBusy = get_dma_busy();
  nop                                                ## L:481  |     *9 | dmaBusy = get_dma_busy();
  LABEL_RDPQ_Triangle_Send_Async_0009:
  lw $a0, %lo(RDPQ_DYNAMIC_BUFFERS + 4)              ## L:631  |     10 | rdramCur = load(RDPQ_DYNAMIC_BUFFERS, 4);
  sb $t3, 0($t4)                                     ## L:642  |     11 | @Barrier("cb-stash") store(prim_size:u8, color_dmem, 0x00); ## Barrier: 0x2
  lw $t1, %lo(RDPQ_DYNAMIC_BUFFERS + 0)              ## L:632  |     12 | u32<$t1> otherBuff = load(RDPQ_DYNAMIC_BUFFERS, 0);
  sw $a0, %lo(0 + RDPQ_DYNAMIC_BUFFERS)($zero)       ## L:633  |     13 | store(rdramCur, RDPQ_DYNAMIC_BUFFERS, 0);
  lui $at, 0x01                                      ## L:644  |     14 | rdramSentinel = rdramCur + 65536;
  sw $t1, %lo(4 + RDPQ_DYNAMIC_BUFFERS)($zero)       ## L:634  |     15 | store(otherBuff, RDPQ_DYNAMIC_BUFFERS, 4);
  or $a3, $zero, $ra                                 ## L:630  |     16 | u32<$a3> savedRA = RA;
  or $a1, $zero, $a0                                 ## L:645  |     17 | u32<$a1> rdramCurCopy = rdramCur;
  jal RSPQCmd_RdpSetBuffer                           ## L:646  |     18 | RSPQCmd_RdpSetBuffer(rdramCur, rdramCurCopy, rdramSentinel); ## Args: $a0, $a1, $a2
  addu $a2, $a0, $at                                 ## L:644  |    *20 | rdramSentinel = rdramCur + 65536;
  or $ra, $zero, $a3                                 ## L:654  |     21 | RA = savedRA;
  lbu $t3, 0($t4)                                    ## L:652  |     22 | @Barrier("cb-stash") prim_size:u8 = load(color_dmem, 0x00); ## Barrier: 0x2
  mtc0 $a0, COP0_DMA_RAMADDR                         ## L:651  |     23 | @Barrier("tri-out") set_dma_addr_rdram(rdramCur); ## Barrier: 0x1
  j LABEL_RDPQ_Triangle_Send_Async_000A              ## L:654  |     24 | RA = savedRA;
  addu $a0, $a0, $t3                                 ## L:653  |    *26 | rdramCur += prim_size;
RDPQ_Triangle_Send_End:
  ## hot-path cycles: 11
  LABEL_RDPQ_Triangle_Send_End_000B:
  mfc0 $t2, COP0_DMA_BUSY                            ## L:760  |      1 | dmaBusy = get_dma_busy();
  lw $t1, %lo(RDPQ_CURRENT + 0)                      ## L:764  |      2 | rdramCur = load(RDPQ_CURRENT);
  lbu $t0, %lo(RDPQ_SYNCFULL_ONGOING + 0)            ## L:763  |      3 | syncFull = load(RDPQ_SYNCFULL_ONGOING);
  bne $t2, $zero, LABEL_RDPQ_Triangle_Send_End_000B  ## L:764  |      4 | rdramCur = load(RDPQ_CURRENT);
  sh $zero, %lo(RDPQ_TRI_BUFF_OFFSET)($zero)         ## L:762  |     *6 | store(ZERO:u16, RDPQ_TRI_BUFF_OFFSET);
  LABEL_RDPQ_Triangle_Send_End_000C:
  bne $t0, $zero, JrRa                               ## L:771  |      7 | if(syncFull != 0)goto JrRa;
  sw $t1, %lo(RDPQ_CURRENT)($zero)                   ## L:770  |      8 | store(rdramCur, RDPQ_CURRENT);
  jr $ra                                             ## L:774  |      9 | }
  mtc0 $t1, COP0_DP_END                              ## L:773  |    *11 | set_rdp_end(rdramCur);

.set at
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