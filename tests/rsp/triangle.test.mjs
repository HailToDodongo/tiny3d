// Unit tests for RDPQ_Triangle_Send_Async (rspq_triangle.rspl)
import {test, before, beforeEach} from 'node:test';
import assert from 'node:assert/strict';
import {Ucode, TRI_SIZE, writeScreenVertex, readRdpTriangle} from './harness.mjs';

const CULL_FRONT = 0, CULL_BACK = 1, CULL_NONE = 2;
const TRI_SHADE_TEX_Z = 0x0000CF00; // TRI_COMMAND value for shade+tex+z, tile 0

let u, VB, OUT;
before(async () => { u = await Ucode.load(); VB = u.sym.VERT_BUFFER; OUT = u.sym.CLIP_BUFFER_TMP; });

beforeEach(() => {
  u.reset();
  u.w32(u.sym.TRI_COMMAND, TRI_SHADE_TEX_Z);
  u.w16(u.sym.RDPQ_TRI_BUFF_OFFSET, 0);
  u.w32(u.sym.RDPQ_CURRENT, 0x00100000);
  u.w32(u.sym.RDPQ_SENTINEL, 0x00200000);
  u.w8(u.sym.RDPQ_SYNCFULL_ONGOING, 0);
  for (let i = 0; i < 176; i++) u.w8(OUT + i, 0xEE); // detect untouched output
});

/** writes 3 vertices and calls the triangle function */
function drawTriangle(verts, cull = CULL_NONE) {
  u.w32(u.sym.RDPQ_CURRENT, 0x00100000);
  verts.forEach((v, i) => writeScreenVertex(u, VB + i * TRI_SIZE, v));
  const res = u.call('RDPQ_Triangle_Send_Async',
    {$a0: VB, $a1: VB + TRI_SIZE, $a2: VB + 2 * TRI_SIZE, $v0: cull},
    ['RDPQ_Triangle_Clip']);
  res.drawn = u.r32(u.sym.RDPQ_CURRENT) !== 0x00100000; // pointer advanced -> primitive emitted
  res.clipped = res.pc === u.sym.RDPQ_Triangle_Clip;
  return res;
}

const V = [
  {x: 10, y: 10, z: 0x1000, rgba: 0xFF0000FF, s: 0, t: 0, w: 2},
  {x: 100, y: 20, z: 0x3000, rgba: 0x00FF00FF, s: 32, t: 0, w: 4},
  {x: 50, y: 90, z: 0x5000, rgba: 0x0000FFFF, s: 0, t: 32, w: 8},
];

test('edge coefficients match the float reference', () => {
  const res = drawTriangle(V);
  assert.ok(res.returned && res.drawn);
  const tri = readRdpTriangle(u, OUT);
  assert.equal(tri.cmd, 0xCF);
  assert.equal(tri.tile, 0);
  assert.deepEqual([tri.yh, tri.ym, tri.yl], [10, 20, 90]);
  // sorted by y: v1=(10,10) v2=(100,20) v3=(50,90)
  const H = [40, 80], M = [90, 10], L = [-50, 70];
  const near = (a, b, eps = 1 / 4096) => assert.ok(Math.abs(a - b) <= eps, `${a} != ${b} (+-${eps})`);
  // slopes come from a vrcp + one Newton-Raphson step: ~2^-11 relative precision
  const slope = (a, b) => near(a, b, Math.abs(b) / 2048 + 1 / 8192);
  slope(tri.dxhdy, H[0] / H[1]);
  slope(tri.dxmdy, M[0] / M[1]);
  slope(tri.dxldy, L[0] / L[1]);
  near(tri.xh, 10); near(tri.xm, 10); near(tri.xl, 100);
  assert.equal(tri.leftMajor, true);      // NZ = MX*HY - MY*HX = 6800 > 0
  assert.equal(tri.size, 176);
  assert.equal(u.gpr('$s3'), OUT + 176);  // end pointer
  assert.equal(u.gpr('$s7'), 176);        // next buffer offset
  assert.equal(u.r32(u.sym.RDPQ_CURRENT), 0x00100000 + 176);
});

test('shade and depth start at the top vertex and interpolate along the major edge', () => {
  drawTriangle(V);
  const tri = readRdpTriangle(u, OUT);
  const near = (a, b, eps) => assert.ok(Math.abs(a - b) <= eps, `${a} != ${b} (+-${eps})`);
  // top vertex has an integer y, so FY = 0 and the start values are the vertex attributes
  assert.deepEqual(tri.shade.v.map(Math.round), [255, 0, 0, 255]);
  near(tri.z.v, 0x1000, 1);
  // vertex 3 lies on the H edge, 80 scanlines below: value = v + de * 80
  near(tri.shade.v[2] + tri.shade.de[2] * 80, 255, 1.5);
  near(tri.shade.v[0] + tri.shade.de[0] * 80, 0, 1.5);
  near(tri.z.v + tri.z.de * 80, 0x5000, 4);
  // vertex 2: 10 lines down the H edge then along x to x=100
  const xOnH = 10 + tri.dxhdy * 10;
  near(tri.shade.v[1] + tri.shade.de[1] * 10 + tri.shade.dx[1] * (100 - xOnH), 255, 1.5);
  near(tri.z.v + tri.z.de * 10 + tri.z.dx * (100 - xOnH), 0x3000, 4);
  // texture W: 0.5/W normalized to the closest vertex (W=2 -> 0.5 = 0x8000), minus 1, halved,
  // and placed in the integer lane of the coefficient (RDP only needs the relative scale)
  assert.equal(tri.tex.v[2], 0x3FFF);
});

test('tile index from the clip flags goes into the command word', () => {
  drawTriangle(V.map((v, i) => ({...v, clip: 5 << 5})));
  assert.equal(readRdpTriangle(u, OUT).tile, 5);
});

test('backface culling: exactly one cull mode drops the triangle, flipped winding swaps it', () => {
  const r = (verts, cull) => drawTriangle(verts, cull).drawn;
  assert.equal(r(V, CULL_NONE), true);
  const cw = [r(V, CULL_FRONT), r(V, CULL_BACK)];
  assert.deepEqual([...cw].sort(), [false, true]);
  const flipped = [V[0], V[2], V[1]];
  assert.equal(r(flipped, CULL_NONE), true);
  assert.equal(r(flipped, CULL_FRONT), !cw[0]);
  assert.equal(r(flipped, CULL_BACK), !cw[1]);
});

test('vertex order does not change the output', () => {
  drawTriangle(V);
  const a = u.bytes(OUT, 176);
  drawTriangle([V[1], V[2], V[0]]);
  assert.deepEqual(u.bytes(OUT, 176), a);
});

test('rejection: dropped only when all three vertices are outside the same plane', () => {
  // rejection bytes are stored inverted: 0xFF = inside all planes
  assert.equal(drawTriangle(V.map((v) => ({...v, reject: 0xFE}))).drawn, false);
  assert.equal(drawTriangle(V.map((v, i) => ({...v, reject: i === 2 ? 0xFF : 0xFE}))).drawn, true);
  assert.equal(drawTriangle(V.map((v, i) => ({...v, reject: 0xFF ^ (1 << i)}))).drawn, true);
});

test('clipping: any clip flag hands the triangle to the clipper', () => {
  for (let i = 0; i < 3; i++) {
    const res = drawTriangle(V.map((v, j) => ({...v, clip: j === i ? 0x02 : 0})));
    assert.equal(res.clipped, true, `vertex ${i}`);
    assert.equal(res.drawn, false);
  }
});

test('degenerate triangle (zero area) still terminates', () => {
  const res = drawTriangle([{...V[0]}, {...V[0], x: 50}, {...V[0], x: 90}]);
  assert.ok(res.returned || res.clipped);
});
