.section .text.applyTexture
.p2align 5
.globl applyTexture
.func applyTexture
.set at
.set noreorder

#define fbTexIn    $a0
#define fbTexInEnd $a1
#define fbOut64    $a2

#define fbTexInLoopEnd $t0
#define p0Addr     $t1
#define p1Addr     $t2
#define p2Addr     $t3
#define p3Addr     $t4
#define gridIndex  $t5
#define indices    $t6
#define baseAddr0  $t7
#define baseAddr1  $t8
#define baseAddr2  $t9
#define baseAddr3  $v0
#define baseAddr4  $a3

#define ADDR_MASK $v1

#define CACHE_OP_DIRTY ((0x3 << 2) | 0x1)
#define CACHE_OP_FLUSH ((0x5 << 2) | 0x1)

/**
 * Decodes a 4x4 block worth of texture data by a given address
 * the address itself has the 4 LSBs as the index into the 4x4 grid.
 * The data iself is 16bytes, split into two halves.
 * The first containing 4 RGBA16 colors, the other 16 2bit indices for which color to use.
 * Some space is wasted with this, but it is the fasted to process and actually benefit from reduced memory usage.
 */
.macro decodeBlockNew baseAddr, pAddr, idx
    and gridIndex, \pAddr, 0b1111
    and \baseAddr, \pAddr, ADDR_MASK
    lw indices, 8(\baseAddr)
    sll gridIndex, 1
    srl indices, gridIndex

    and indices, indices, 0b110
    addu indices, \baseAddr
    lhu \baseAddr, 0(indices)
.endm

applyTexture:
  li ADDR_MASK, ~0b1111
  li p0Addr, 0x80000000
  or p1Addr, $zero, p0Addr
  or p2Addr, $zero, p0Addr
  or p3Addr, $zero, p0Addr

  1:
    // example value at fbTexIn: 0x41'AB'C0'E0
    // first byte is the texture index, the two after the UV, the last coverage
    // first 3 bytes form a 24bit pixel index into the large texture buffer, coverage is ignored
    // to avoid shifting, do a LWR here to load the upper 3 bytes into the lower part of the register
    // and preserve the 0x80 in the MSBs of the register that get setup once before the loop
    lwr p0Addr, 2(fbTexIn)
    lwr p1Addr, 6(fbTexIn)

    // prevent loading the old FB values during writes.
    // flushing is not done here and the CPU does it when it feels like it.
    // (The last few bytes of the FB are flushed manually after this function in C)
    cache CACHE_OP_DIRTY, 0(fbOut64)

    lwr p2Addr, 10(fbTexIn)
    lwr p3Addr, 14(fbTexIn)

    // now transform multiple blocks to do all the output for one cache-line worth of input.
    // saves are between decoding steps to reduce stalling
    decodeBlockNew baseAddr0, p0Addr, 0
    lwr p0Addr, 18(fbTexIn)
    decodeBlockNew baseAddr1, p1Addr, 1
    sh baseAddr0, 0(fbOut64)
    decodeBlockNew baseAddr2, p2Addr, 2
    sh baseAddr1, 2(fbOut64)
    decodeBlockNew baseAddr3, p3Addr, 3

    lwr p1Addr, 22(fbTexIn)

    sh baseAddr2, 4(fbOut64)

    lwr p2Addr, 26(fbTexIn)
    lwr p3Addr, 30(fbTexIn)

    decodeBlockNew baseAddr0, p0Addr, 0
    sh baseAddr3, 6(fbOut64)
    decodeBlockNew baseAddr1, p1Addr, 1
    sh baseAddr0, 8(fbOut64)
    decodeBlockNew baseAddr2, p2Addr, 2
    sh baseAddr1, 10(fbOut64)
    decodeBlockNew baseAddr3, p3Addr, 3
    sh baseAddr2, 12(fbOut64)

    addiu fbTexIn, 32
    sh baseAddr3, 14(fbOut64)

    // Do this outside this function once with the last bit of data:
    // cache CACHE_OP_FLUSH, 0(fbOut64)

  bne fbTexIn, fbTexInEnd, 1b
  addiu fbOut64, 16
  # nop # addu fbTexInLoopEnd, fbTexIn, 64 // 4 iterations

  jr $ra
  nop
.endfunc
