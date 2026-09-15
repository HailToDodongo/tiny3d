// Golden-byte tests: exact output of the triangle code and the vertex loop for a fixed
// set of inputs. Recorded once, every later change must reproduce them byte for byte
// (or consciously re-record with UPDATE_GOLDEN=1 and explain the diff in the commit).
import {test, before} from 'node:test';
import assert from 'node:assert/strict';
import {Ucode, TRI_SIZE, writeScreenVertex, writePackedVertexPair, writeMatrixFP, setViewport, checkGolden, hex} from './harness.mjs';

let u;
before(async () => { u = await Ucode.load(); });

// ---------------------------------------------------------------------------
// RDPQ_Triangle_Send_Async
// ---------------------------------------------------------------------------
const CMD = {shadeTexZ: 0xCF00, shade: 0xCC00, shadeZ: 0xCD00, texZ: 0xCB00, tex: 0xCA00, flat: 0xC800};
const v = (x, y, w, o = {}) => ({x, y, w, z: o.z ?? 0x2000, rgba: o.rgba ?? 0x80C040FF, s: o.s ?? 0, t: o.t ?? 0, clip: o.clip ?? 0, reject: o.reject ?? 0xFF});

const TRI_CASES = [
  {name: 'basic', verts: [v(10, 10, 2, {z: 0x1000, rgba: 0xFF0000FF}), v(100, 20, 4, {z: 0x3000, rgba: 0x00FF00FF, s: 32}), v(50, 90, 8, {z: 0x5000, rgba: 0x0000FFFF, t: 32})]},
  {name: 'fractional_positions', verts: [v(10.25, 10.75, 2.5), v(100.5, 20.25, 4.25, {s: 17.5, t: 3.25}), v(50.75, 90.5, 8.75, {s: 1.25, t: 40})]},
  {name: 'reversed_winding', verts: [v(10, 10, 2), v(50, 90, 8, {t: 32}), v(100, 20, 4, {s: 32})]},
  {name: 'unsorted_input', verts: [v(50, 90, 8, {rgba: 0x0000FFFF}), v(10, 10, 2, {rgba: 0xFF0000FF}), v(100, 20, 4, {rgba: 0x00FF00FF})]},
  {name: 'flat_top', verts: [v(10, 10, 3), v(120, 10, 3), v(60, 80, 5)]},
  {name: 'flat_bottom', verts: [v(60, 5, 5), v(10, 80, 3), v(120, 80, 3)]},
  {name: 'all_same_y', verts: [v(10, 40, 3), v(60, 40, 3), v(120, 40, 3)]},
  {name: 'zero_area_vertical', verts: [v(40, 10, 3), v(40, 50, 3), v(40, 90, 3)]},
  {name: 'sliver', verts: [v(10, 10, 3), v(11, 200, 3), v(10.25, 100, 3)]},
  {name: 'guard_band_extent', verts: [v(-600, -450, 1.5), v(950, -300, 6), v(300, 700, 2.25, {s: 500, t: -300})]},
  {name: 'negative_coords', verts: [v(-30, -20, 2), v(-5, -60, 2.5), v(-80, -70, 3)]},
  {name: 'w_close_to_one', verts: [v(20, 20, 1.0), v(200, 30, 1.0001), v(90, 200, 1.5)]},
  {name: 'w_large', verts: [v(20, 20, 3000), v(200, 30, 4500.5), v(90, 200, 12000)]},
  {name: 'w_not_power_of_two', verts: [v(20, 20, 3.7), v(200, 30, 5.3), v(90, 200, 9.9)]},
  {name: 'colour_extremes', verts: [v(20, 20, 2, {rgba: 0x00000000}), v(200, 30, 2, {rgba: 0xFFFFFFFF}), v(90, 200, 2, {rgba: 0xFF00FF00})]},
  {name: 'st_saturation', verts: [v(20, 20, 2, {s: -1000, t: 1000}), v(200, 30, 2, {s: 1023, t: -1023}), v(90, 200, 2, {s: 0.5, t: 0.03125})]},
  {name: 'depth_extremes', verts: [v(20, 20, 2, {z: 0}), v(200, 30, 2, {z: 0x7FFF}), v(90, 200, 2, {z: 0x4000})]},
  {name: 'tile_3', verts: [v(10, 10, 2, {clip: 3 << 5}), v(100, 20, 4, {clip: 3 << 5}), v(50, 90, 8, {clip: 3 << 5})]},
  {name: 'cmd_shade_only', cmd: CMD.shade, verts: [v(10, 10, 2, {rgba: 0xFF0000FF}), v(100, 20, 4, {rgba: 0x00FF00FF}), v(50, 90, 8, {rgba: 0x0000FFFF})]},
  {name: 'cmd_shade_z', cmd: CMD.shadeZ, verts: [v(10, 10, 2, {z: 0x100}), v(100, 20, 4, {z: 0x3000}), v(50, 90, 8, {z: 0x7000})]},
  {name: 'cmd_tex_z', cmd: CMD.texZ, verts: [v(10, 10, 2), v(100, 20, 4, {s: 32}), v(50, 90, 8, {t: 32})]},
  {name: 'cmd_tex_only', cmd: CMD.tex, verts: [v(10, 10, 2), v(100, 20, 4, {s: 32}), v(50, 90, 8, {t: 32})]},
  {name: 'cmd_flat', cmd: CMD.flat, verts: [v(10, 10, 2), v(100, 20, 4), v(50, 90, 8)]},
  {name: 'buffer_offset_176', buffOffset: 176, verts: [v(10, 10, 2), v(100, 20, 4), v(50, 90, 8)]},
  {name: 'cull_front_ccw', cull: 0, verts: [v(10, 10, 2), v(100, 20, 4), v(50, 90, 8)]},
  {name: 'cull_back_ccw', cull: 1, verts: [v(10, 10, 2), v(100, 20, 4), v(50, 90, 8)]},
  {name: 'cull_front_cw', cull: 0, verts: [v(10, 10, 2), v(50, 90, 8), v(100, 20, 4)]},
  {name: 'cull_back_cw', cull: 1, verts: [v(10, 10, 2), v(50, 90, 8), v(100, 20, 4)]},
  {name: 'rejected', verts: [v(10, 10, 2, {reject: 0xFB}), v(100, 20, 4, {reject: 0xFB}), v(50, 90, 8, {reject: 0xFB})]},
  {name: 'clip_handoff', verts: [v(10, 10, 2, {clip: 0x10}), v(100, 20, 4), v(50, 90, 8)]},
];

test('triangle golden bytes', () => {
  const results = [];
  for (const c of TRI_CASES) {
    u.reset();
    const VB = u.sym.VERT_BUFFER, OUT = u.sym.CLIP_BUFFER_TMP + (c.buffOffset ?? 0);
    u.w32(u.sym.TRI_COMMAND, c.cmd ?? CMD.shadeTexZ);
    u.w16(u.sym.RDPQ_TRI_BUFF_OFFSET, c.buffOffset ?? 0);
    u.w32(u.sym.RDPQ_CURRENT, 0x00100000);
    u.w32(u.sym.RDPQ_SENTINEL, 0x00200000);
    u.w8(u.sym.RDPQ_SYNCFULL_ONGOING, 0);
    for (let i = 0; i < 176 * 2; i++) u.w8(u.sym.CLIP_BUFFER_TMP + i, 0xEE);
    c.verts.forEach((vv, i) => writeScreenVertex(u, VB + i * TRI_SIZE, vv));
    const res = u.call('RDPQ_Triangle_Send_Async',
      {$a0: VB, $a1: VB + TRI_SIZE, $a2: VB + 2 * TRI_SIZE, $v0: c.cull ?? 2}, ['RDPQ_Triangle_Clip']);
    results.push({
      name: c.name,
      end: res.pc === u.sym.RDPQ_Triangle_Clip ? 'clip' : res.returned ? 'return' : `pc=${res.pc.toString(16)}`,
      rdramAdvance: u.r32(u.sym.RDPQ_CURRENT) - 0x00100000,
      s3: u.gpr('$s3'), s7: u.gpr('$s7'),
      bytes: hex(u.bytes(OUT, 176)),
    });
  }
  checkGolden('triangle', results);
});

// ---------------------------------------------------------------------------
// T3DCmd_VertLoad
// ---------------------------------------------------------------------------
const S = 1 / 64;
const PERSPECTIVE = [[S, 0, 0, 0], [0, S, 0, 0], [0, 0, S, S], [0, 0, 0, 1]];           // w = 1 + z/64
const ROTATED = [[0.0123, 0.0071, -0.0032, 0.0011], [-0.0064, 0.0142, 0.0018, -0.0007],
                 [0.0027, -0.0045, 0.0151, 0.0161], [0.35, -0.2, 0.9, 1.0]];           // all lanes engaged
const p = (pos, o = {}) => ({pos, norm: o.norm ?? [0, 0, 1], rgba: o.rgba ?? 0xFFFFFFFF, st: o.st ?? [0, 0]});

const VERT_CASES = [
  {name: 'perspective_inside', mvp: PERSPECTIVE, pairs: [[p([16, -24, 64], {rgba: 0x102030FF, st: [10, 20]}), p([-40, 32, 128], {rgba: 0x80FF40C0, st: [-5.5, 33.25]})]]},
  {name: 'rotated_matrix', mvp: ROTATED, guardBand: 3, pairs: [
    [p([100, 200, -300], {norm: [1, 0, 0], rgba: 0xC08040FF, st: [64, 64]}), p([-250, 75, 500], {norm: [0, 0.7, 0.7], st: [0.03125, 1023]})],
    [p([3000, -2500, 1200], {norm: [-1, 0, 0]}), p([-3000, 3000, 900])]]},
  {name: 'guard_band_and_clip', mvp: PERSPECTIVE, guardBand: 2, pairs: [
    [p([0, 0, 0]), p([96, 0, 0])],          // inside | outside screen, inside guard band
    [p([192, 0, 0]), p([-192, -192, 0])],   // outside guard band | two planes
    [p([0, 64, 0]), p([0, -64, 0])],        // exactly on the top / bottom edge
    [p([0, 0, -64]), p([0, 0, -70])]]},     // w = 0 | w < 0 (behind the camera)
  {name: 'depth_range', mvp: PERSPECTIVE, pairs: [[p([0, 0, -32]), p([0, 0, 32])], [p([0, 0, 100]), p([0, 0, 2000])]]},
  {name: 'colours_and_uvs', mvp: PERSPECTIVE, pairs: [[p([8, 8, 0], {rgba: 0x00000000, st: [-1024, 1023.96875]}), p([-8, -8, 0], {rgba: 0xFFFFFFFF, st: [512.5, -0.03125]})]]},
];

test('vertex loop golden bytes', () => {
  const results = [];
  for (const c of VERT_CASES) {
    u.reset();
    setViewport(u, {width: 320, height: 240, guardBand: c.guardBand ?? 2});
    writeMatrixFP(u, u.sym.MATRIX_MVP, c.mvp);
    const RD = 0x00100000, VB = u.sym.VERT_BUFFER;
    c.pairs.forEach((pr, i) => writePackedVertexPair(u, RD + i * 32, pr[0], pr[1]));
    const inputSize = c.pairs.length * 32;
    const dest = (((u.sym.TEMP_STATE_MEM_END & ~0xF) - 32) - inputSize) & ~0xF;
    for (let i = 0; i < c.pairs.length * 2 * TRI_SIZE; i++) u.w8(VB + i, 0xEE);
    const res = u.command('T3DCmd_VertLoad', [inputSize, RD, (dest << 16) | VB]);
    results.push({
      name: c.name,
      end: res.pc === u.sym.RSPQ_Loop ? 'loop' : `pc=${res.pc.toString(16)}`,
      verts: Array.from({length: c.pairs.length * 2}, (_, i) => hex(u.bytes(VB + i * TRI_SIZE, TRI_SIZE))),
    });
  }
  checkGolden('vertload', results);
});
