// Sort coverage for RDPQ_Triangle_Send_Async: every input permutation, every tie position,
// with and without culling. Two layers:
//  - invariants that must hold for any correct sort (permutation independence, cull parity)
//  - goldens of the exact bytes per permutation (tie-breaking depends on the input order and
//    the rewrite must reproduce the current choice)
import {test, before} from 'node:test';
import assert from 'node:assert/strict';
import {Ucode, TRI_SIZE, writeScreenVertex, checkGolden, hex} from './harness.mjs';

let u;
before(async () => { u = await Ucode.load(); });

const PERMS = [[0, 1, 2], [0, 2, 1], [1, 0, 2], [1, 2, 0], [2, 0, 1], [2, 1, 0]];
const parity = (p) => (p[0] === 0 && p[1] === 1) || (p[0] === 1 && p[1] === 2) || (p[0] === 2 && p[1] === 0) ? 0 : 1;

function draw(verts, cull = 2) {
  u.reset();
  const VB = u.sym.VERT_BUFFER, OUT = u.sym.CLIP_BUFFER_TMP;
  u.w32(u.sym.TRI_COMMAND, 0xCF00);
  u.w32(u.sym.RDPQ_CURRENT, 0x00100000);
  u.w32(u.sym.RDPQ_SENTINEL, 0x00200000);
  for (let i = 0; i < 176; i++) u.w8(OUT + i, 0xEE);
  verts.forEach((v, i) => writeScreenVertex(u, VB + i * TRI_SIZE, v));
  const res = u.call('RDPQ_Triangle_Send_Async', {$a0: VB, $a1: VB + TRI_SIZE, $a2: VB + 2 * TRI_SIZE, $v0: cull}, ['RDPQ_Triangle_Clip']);
  return {drawn: u.r32(u.sym.RDPQ_CURRENT) !== 0x00100000, bytes: u.bytes(OUT, 176), returned: res.returned};
}

const v = (x, y, w, rgba, z) => ({x, y, w, rgba, z, s: x, t: y});
const SETS = {
  distinct:    [v(10, 10, 2, 0xFF0000FF, 0x1000), v(100, 20.5, 4, 0x00FF00FF, 0x3000), v(50, 90.25, 8, 0x0000FFFF, 0x5000)],
  tie_top:     [v(10, 10, 2, 0xFF0000FF, 0x1000), v(100, 10, 4, 0x00FF00FF, 0x3000), v(50, 90, 8, 0x0000FFFF, 0x5000)],
  tie_bottom:  [v(10, 10, 2, 0xFF0000FF, 0x1000), v(100, 90, 4, 0x00FF00FF, 0x3000), v(50, 90, 8, 0x0000FFFF, 0x5000)],
  tie_outer:   [v(10, 10, 2, 0xFF0000FF, 0x1000), v(100, 50, 4, 0x00FF00FF, 0x3000), v(50, 10, 8, 0x0000FFFF, 0x5000)],
  all_equal:   [v(10, 40, 2, 0xFF0000FF, 0x1000), v(100, 40, 4, 0x00FF00FF, 0x3000), v(50, 40, 8, 0x0000FFFF, 0x5000)],
  tie_frac:    [v(10, 10.25, 2, 0xFF0000FF, 0x1000), v(100, 10.25, 4, 0x00FF00FF, 0x3000), v(50, 90, 8, 0x0000FFFF, 0x5000)],
  negative:    [v(-30, -20, 2, 0xFF0000FF, 0x1000), v(-5, -60, 2.5, 0x00FF00FF, 0x3000), v(-80, -70, 3, 0x0000FFFF, 0x5000)],
  same_x:      [v(40, 10, 3, 0xFF0000FF, 0x1000), v(40, 50, 3, 0x00FF00FF, 0x3000), v(40, 90, 3, 0x0000FFFF, 0x5000)],
  same_point:  [v(40, 40, 3, 0xFF0000FF, 0x1000), v(40, 40, 3, 0x00FF00FF, 0x3000), v(40, 40, 3, 0x0000FFFF, 0x5000)],
};
const perm = (verts, p) => p.map((i) => verts[i]);

test('distinct Y: every input permutation gives the same primitive', () => {
  const ref = draw(SETS.distinct).bytes;
  for (const p of PERMS) assert.deepEqual(draw(perm(SETS.distinct, p)).bytes, ref, `perm ${p}`);
});

test('distinct Y: cull decision follows the permutation parity', () => {
  const base = [draw(SETS.distinct, 0).drawn, draw(SETS.distinct, 1).drawn];
  assert.notEqual(base[0], base[1]);
  for (const p of PERMS) {
    const odd = parity(p);
    assert.equal(draw(perm(SETS.distinct, p), 0).drawn, odd ? !base[0] : base[0], `perm ${p} cull front`);
    assert.equal(draw(perm(SETS.distinct, p), 1).drawn, odd ? !base[1] : base[1], `perm ${p} cull back`);
    assert.equal(draw(perm(SETS.distinct, p), 2).drawn, true, `perm ${p} no cull`);
  }
});

test('ties: the sorted Y values are always ordered and every permutation terminates', () => {
  for (const [name, verts] of Object.entries(SETS)) {
    for (const p of PERMS) {
      const r = draw(perm(verts, p));
      assert.ok(r.returned, `${name} perm ${p} did not return`);
      const y = (o) => ((u.r16(u.sym.CLIP_BUFFER_TMP + o) << 18) >> 18) / 4;
      const [yl, ym, yh] = [y(2), y(4), y(6)];
      assert.ok(yh <= ym && ym <= yl, `${name} perm ${p}: YH ${yh} YM ${ym} YL ${yl}`);
    }
  }
});

test('sort golden bytes: every set x permutation x cull mode', () => {
  const results = [];
  for (const [name, verts] of Object.entries(SETS)) {
    for (const p of PERMS) {
      for (const cull of [2, 0, 1]) {
        const r = draw(perm(verts, p), cull);
        results.push({name: `${name}_${p.join('')}_cull${cull}`, drawn: r.drawn, bytes: hex(r.bytes)});
      }
    }
  }
  checkGolden('sort', results);
});
