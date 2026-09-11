// Unit tests for T3DCmd_VertLoad (vertex transform loop in rsp_tiny3d.rspl)
import {test, before, beforeEach} from 'node:test';
import assert from 'node:assert/strict';
import {Ucode, TRI_SIZE, writePackedVertexPair, writeMatrixFP, setViewport, readScreenVertex} from './harness.mjs';

const RDRAM_VERTS = 0x00100000;
const W = 320, H = 240;

let u;
before(async () => { u = await Ucode.load(); });
beforeEach(() => { u.reset(); });

/**
 * Loads `pairs` (arrays of two packed vertices) through T3DCmd_VertLoad like t3d_vert_load()
 * does and returns the transformed vertices from VERT_BUFFER.
 */
function vertLoad(pairs) {
  const inputSize = pairs.length * 32;
  pairs.forEach((p, i) => writePackedVertexPair(u, RDRAM_VERTS + i * 32, p[0], p[1]));
  const tmpBufferEnd = (u.sym.TEMP_STATE_MEM_END & ~0xF) - 32;
  const offsetDest = (tmpBufferEnd - inputSize) & ~0xF;
  const offsetInput = u.sym.VERT_BUFFER;
  const res = u.command('T3DCmd_VertLoad', [inputSize, RDRAM_VERTS, (offsetDest << 16) | offsetInput]);
  assert.equal(res.pc, u.sym.RSPQ_Loop, 'command did not return to the rspq loop');
  const out = [];
  for (let i = 0; i < pairs.length * 2; i++) out.push(readScreenVertex(u, u.sym.VERT_BUFFER + i * TRI_SIZE));
  return out;
}

const near = (a, b, eps, msg = '') => assert.ok(Math.abs(a - b) <= eps, `${msg} ${a} != ${b} (+-${eps})`);

// clip = M * pos, pos.w = 1. Row r of the DMEM matrix holds the coefficients of pos[r].
const SCALE = 1 / 64;
const MVP_ORTHO = [[SCALE, 0, 0, 0], [0, SCALE, 0, 0], [0, 0, SCALE, 0], [0, 0, 0, 1]];

test('orthographic transform lands on the expected screen positions', () => {
  const vp = setViewport(u, {width: W, height: H, guardBand: 2});
  writeMatrixFP(u, u.sym.MATRIX_MVP, MVP_ORTHO);
  const verts = vertLoad([[
    {pos: [32, -48, 0], rgba: 0x102030FF, st: [10, 20]},   // ndc (0.5, -0.75, 0)
    {pos: [-64, 32, 32], rgba: 0xFFFFFFFF, st: [0, 0]},    // ndc (-1, 0.5, 0.5)
  ]]);
  const expect = (ndc) => ({
    x: vp.screenOffsetX / 2 + ndc[0] * (W / 2),
    y: vp.screenOffsetY / 2 - ndc[1] * (H / 2),
    z: 0x3FFF + ndc[2] * 0x3FFF,
  });
  const e0 = expect([0.5, -0.75, 0]), e1 = expect([-1, 0.5, 0.5]);
  near(verts[0].x, e0.x, 0.25, 'x0'); near(verts[0].y, e0.y, 0.25, 'y0'); near(verts[0].z, e0.z, 4, 'z0');
  near(verts[1].x, e1.x, 0.25, 'x1'); near(verts[1].y, e1.y, 0.25, 'y1'); near(verts[1].z, e1.z, 4, 'z1');
  // both are inside the view: no clip / reject flags
  // (vertex 1 sits exactly on the left edge, x = -w: the compare is inclusive, so it is
  //  flagged as rejected on that plane but not clipped; see the edge test below)
  assert.equal(verts[0].clip & 0x1F, 0); assert.equal(verts[0].reject, 0xFF);
  assert.equal(verts[1].clip & 0x1F, 0);
  // w = 1 -> stored W and the half reciprocal 0.5/W
  near(verts[0].w, 1, 1 / 4096); near(verts[0].invW, 0.5, 1 / 1024);
  // clip-space position is kept for the clipper, x/y pre-divided by the guard band (2)
  near(verts[0].clipPos[0], 0.5 / 2, 1 / 4096); near(verts[0].clipPos[1], -0.75 / 2, 1 / 4096);
  // UVs pass through, colour is unlit (ambient white, scaled by 0x7FFF/0x8000 -> each channel may lose 1)
  near(verts[0].s, 10, 1 / 32); near(verts[0].t, 20, 1 / 32);
  const ch = (rgba) => [24, 16, 8, 0].map((sh) => (rgba >>> sh) & 0xFF);
  ch(verts[0].rgba).forEach((c, i) => near(c, ch(0x102030FF)[i], 1, `channel ${i}`));
});

test('perspective: screen position is divided by W', () => {
  setViewport(u, {width: W, height: H, guardBand: 2});
  // w = 1 + z/64  -> z=64 gives w=2
  writeMatrixFP(u, u.sym.MATRIX_MVP, [[SCALE, 0, 0, 0], [0, SCALE, 0, 0], [0, 0, SCALE, SCALE], [0, 0, 0, 1]]);
  const [a, b] = vertLoad([[{pos: [64, 64, 64]}, {pos: [64, 64, 0]}]]);
  near(a.x, 160 + 0.5 * 160, 0.25, 'x/w'); near(a.y, 120 - 0.5 * 120, 0.25, 'y/w');
  near(b.x, 160 + 1.0 * 160, 0.25, 'x'); near(b.y, 120 - 1.0 * 120, 0.25, 'y');
  near(a.w, 2, 1 / 4096); near(a.invW, 0.25, 1 / 2048);
  assert.ok(a.z > b.z, 'farther vertex has a larger depth');
});

test('clip and reject codes: screen edge vs guard band', () => {
  setViewport(u, {width: W, height: H, guardBand: 2});
  writeMatrixFP(u, u.sym.MATRIX_MVP, MVP_ORTHO);
  const [inside, offscreen, farOut, nearSide] = vertLoad([
    [{pos: [0, 0, 0]}, {pos: [96, 0, 0]}],       // ndc x = 0 | 1.5 (outside screen, inside guard band 2)
    [{pos: [192, 0, 0]}, {pos: [0, 0, 0]}],     // ndc x = 3   (outside guard band)
  ]);
  assert.equal(inside.clip & 0x1F, 0); assert.equal(inside.reject, 0xFF);
  assert.equal(offscreen.clip & 0x1F, 0, 'inside the guard band must not clip');
  assert.notEqual(offscreen.reject, 0xFF, 'outside the screen must set a reject bit');
  assert.notEqual(farOut.clip & 0x1F, 0, 'outside the guard band must clip');
  assert.notEqual(farOut.reject, 0xFF);
  const bits = (v) => v.toString(2).split('1').length - 1;
  assert.equal(bits(0xFF ^ offscreen.reject), 1, 'one plane rejects');
  assert.equal(bits(farOut.clip & 0x1F), 1, 'one plane clips');
  assert.equal(farOut.reject, offscreen.reject, 'same plane rejects both');
  assert.equal(nearSide.reject, 0xFF);
});

test('a vertex exactly on a screen edge counts as outside for rejection, inside for clipping', () => {
  setViewport(u, {width: W, height: H, guardBand: 2});
  writeMatrixFP(u, u.sym.MATRIX_MVP, MVP_ORTHO);
  const [onEdge, justInside] = vertLoad([[{pos: [-64, 0, 0]}, {pos: [-63, 0, 0]}]]); // ndc x = -1 | -0.984
  assert.notEqual(onEdge.reject, 0xFF);
  assert.equal(onEdge.clip & 0x1F, 0);
  assert.equal(justInside.reject, 0xFF);
});

test('vertex count: every pair is transformed', () => {
  setViewport(u, {width: W, height: H, guardBand: 2});
  writeMatrixFP(u, u.sym.MATRIX_MVP, MVP_ORTHO);
  const pairs = [];
  for (let i = 0; i < 4; i++) pairs.push([{pos: [i * 8, 0, 0]}, {pos: [-i * 8, 0, 0]}]);
  const out = vertLoad(pairs);
  for (let i = 0; i < 4; i++) {
    near(out[i * 2].x, 160 + (i * 8 / 64) * 160, 0.25, `v${i * 2}`);
    near(out[i * 2 + 1].x, 160 - (i * 8 / 64) * 160, 0.25, `v${i * 2 + 1}`);
  }
});
