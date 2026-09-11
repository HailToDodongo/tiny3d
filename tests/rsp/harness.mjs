// Unit-test harness for the tiny3d RSP ucode.
//
// Runs the fully linked ucode ELF (build/rsp/rsp_tiny3d.elf) inside the rsp-wasm
// emulator (ares RSP core) and exposes helpers to call single functions/commands
// with hand-made DMEM/RDRAM contents and inspect the results.
//
// Requires the ucode to be built (`make` in the repo root) and `npm install` in this
// directory (rsp-wasm >= 1.1.0).

import {readFileSync, writeFileSync, existsSync, mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import assert from 'node:assert/strict';
import {parseElf} from './elf.mjs';

const REPO = fileURLToPath(new URL('../../', import.meta.url));
// rsp-wasm npm package (>= 1.1.0 has the DMA/CTC2/jump fixes); RSP_WASM overrides the module path
const RSP_WASM = process.env.RSP_WASM ?? 'rsp-wasm';

export const ELF_T3D = REPO + 'build/rsp/rsp_tiny3d.elf';

// ---------------------------------------------------------------------------
// Emulator wrapper
// ---------------------------------------------------------------------------
export class Ucode {
  /**
   * @param {string} elfPath
   */
  static async load(elfPath = ELF_T3D) {
    const {createRSP} = await import(RSP_WASM);
    const rsp = await createRSP();
    const u = new Ucode(rsp, parseElf(elfPath));
    u.reset();
    return u;
  }

  constructor(rsp, elf) {
    this.rsp = rsp;
    this.elf = elf;
    /** symbol name -> 13-bit RSP address (DMEM 0x000-0xFFF, IMEM 0x1000-0x1FFF) */
    this.sym = {};
    for (const [k, v] of Object.entries(elf.symbols)) this.sym[k] = v & 0x1FFF;
    // return stub: a 'break' at the end of IMEM, used as $ra for function calls
    this.RET_STUB = 0x1FF8;
  }

  /** Reload text/data from the ELF and reset all registers. */
  reset() {
    const {rsp} = this;
    rsp.reset();
    for (let i = 0; i < 0x1000; i += 4) { rsp.IMEM.setUint32(i, 0, true); rsp.DMEM.setUint32(i, 0, true); }
    this.#loadBE(rsp.IMEM, this.elf.bytes('.text'));
    this.#loadBE(rsp.DMEM, this.elf.bytes('.data'));
    rsp.IMEM.setUint32(this.RET_STUB & 0xFFF, 0x0000000D, true); // break
    for (let r = 1; r < 32; r++) rsp.setGPR(r, 0);
    for (let r = 0; r < 32; r++) rsp.setVPR(r, [0, 0, 0, 0, 0, 0, 0, 0]);
    // constants rspq sets up before every command (vshift / vshift8)
    rsp.setVPR('$v30', [128, 64, 32, 16, 8, 4, 2, 1]);
    rsp.setVPR('$v31', [0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100]);
    // CPU-side init (t3d_init): vertex FX function
    if (this.sym.VERTEX_FX_FUNC !== undefined && this.sym.VertexFX_None !== undefined) {
      this.w16(this.sym.VERTEX_FX_FUNC, this.sym.VertexFX_None & 0xFFF);
    }
  }

  #loadBE(view, bytes) {
    const dv = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
    let i = 0;
    for (; i + 3 < bytes.byteLength; i += 4) view.setUint32(i, dv.getUint32(i, false), true);
    for (; i < bytes.byteLength; i++) this.#w8(view, i, bytes[i]);
  }
  #w8(view, a, v) { view.setUint8((a & ~3) | (3 - (a & 3)), v & 0xFF); }
  #r8(view, a) { return view.getUint8((a & ~3) | (3 - (a & 3))); }

  // --- DMEM (big-endian view, addresses as in the ucode) ---
  r8(a) { return this.#r8(this.rsp.DMEM, a & 0xFFF); }
  r16(a) { return (this.r8(a) << 8) | this.r8(a + 1); }
  s16(a) { return (this.r16(a) << 16) >> 16; }
  r32(a) { return ((this.r16(a) << 16) | this.r16(a + 2)) >>> 0; }
  s32(a) { return this.r32(a) | 0; }
  w8(a, v) { this.#w8(this.rsp.DMEM, a & 0xFFF, v); }
  w16(a, v) { this.w8(a, v >> 8); this.w8(a + 1, v); }
  w32(a, v) { this.w16(a, v >>> 16); this.w16(a + 2, v & 0xFFFF); }
  bytes(a, n) { const r = []; for (let i = 0; i < n; i++) r.push(this.r8(a + i)); return r; }

  // --- RDRAM (physical addresses, 4 MiB, wraps) ---
  rdr8(a) { return this.#r8(this.rsp.RDRAM, a); }
  rdr16(a) { return (this.rdr8(a) << 8) | this.rdr8(a + 1); }
  rdr32(a) { return ((this.rdr16(a) << 16) | this.rdr16(a + 2)) >>> 0; }
  rdw8(a, v) { this.#w8(this.rsp.RDRAM, a, v); }
  rdw16(a, v) { this.rdw8(a, v >> 8); this.rdw8(a + 1, v); }
  rdw32(a, v) { this.rdw16(a, v >>> 16); this.rdw16(a + 2, v & 0xFFFF); }

  // --- registers ---
  gpr(name) { return this.rsp.getGPR(name); }
  setGpr(name, v) { this.rsp.setGPR(name, v >>> 0); }
  vpr(name) { return this.rsp.getVPR(name); }
  setVpr(name, lanes) { this.rsp.setVPR(name, lanes); }
  /** current PC as a 12-bit IMEM offset (the emulator drops bit 12 after jumps) */
  get pc() { return this.rsp.getPC() & 0xFFF; }
  set pc(v) { this.rsp.setPC(v); }

  /**
   * Run until the PC hits one of the given addresses (or the RSP halts unexpectedly).
   * A 'break' is temporarily placed at every stop address so the run ends there even when
   * the emulator dual-issues across it; the original instructions are restored afterwards.
   * @returns {{pc:number, steps:number, cycles:number, halted:boolean}} pc = the stop that was hit
   */
  runUntil(stops, maxSteps = 200000) {
    const {rsp} = this;
    const addrs = stops.map((s) => (typeof s === 'string' ? this.sym[s] : s) & 0xFFF);
    const saved = addrs.map((a) => rsp.IMEM.getUint32(a, true));
    addrs.forEach((a) => rsp.IMEM.setUint32(a, 0x0000000D, true)); // break
    rsp.fn.rsp_set_halted(0);
    const c0 = rsp.getCycles();
    let steps = 0;
    const set = new Set(addrs);
    while (!set.has(this.pc) && !rsp.isHalted()) {
      if (steps++ >= maxSteps) {
        addrs.forEach((a, i) => rsp.IMEM.setUint32(a, saved[i], true));
        throw new Error(`RSP did not reach ${addrs.map((x) => x.toString(16))} within ${maxSteps} steps (pc=0x${this.pc.toString(16)})`);
      }
      rsp.step();
    }
    addrs.forEach((a, i) => rsp.IMEM.setUint32(a, saved[i], true));
    const pc = this.pc;
    const hit = addrs.find((a) => a === pc) ?? addrs.find((a) => pc - a > 0 && pc - a <= 8);
    return {pc: hit === undefined ? pc : hit | 0x1000, rawPc: pc, steps, cycles: rsp.getCycles() - c0, halted: hit === undefined && rsp.isHalted()};
  }

  /**
   * Call a plain function ('jr $ra' return): sets up $ra to the break stub.
   * @param {string} fn symbol name
   * @param {Object<string, number>} regs e.g. {$a0: 0x428, $v0: 2}
   * @param {(string|number)[]} extraStops additional addresses that end the run (e.g. a taken branch target)
   */
  call(fn, regs = {}, extraStops = []) {
    for (const [r, v] of Object.entries(regs)) this.setGpr(r, v);
    this.setGpr('$ra', this.RET_STUB);
    this.pc = this.sym[fn];
    const res = this.runUntil([this.RET_STUB, ...extraStops]);
    res.returned = res.pc === (this.RET_STUB | 0x1000);
    return res;
  }

  /**
   * Run an rspq command handler: args go to $a0-$a3 like rspq does (the command id byte
   * in the MSB of the first word is irrelevant for the handlers), ends at RSPQ_Loop.
   */
  command(fn, args = [], regs = {}, extraStops = []) {
    const names = ['$a0', '$a1', '$a2', '$a3'];
    args.forEach((v, i) => this.setGpr(names[i], v));
    for (const [r, v] of Object.entries(regs)) this.setGpr(r, v);
    this.setGpr('$ra', this.sym.RSPQ_Loop);
    this.pc = this.sym[fn];
    return this.runUntil(['RSPQ_Loop', this.RET_STUB, ...extraStops]);
  }
}

// ---------------------------------------------------------------------------
// tiny3d data formats
// ---------------------------------------------------------------------------
export const TRI_SIZE = 36;
export const VTX = {XY: 0, Z: 4, CLIP: 6, REJECT: 7, RGBA: 8, ST: 0xC, CLIPPOS_I: 0x10, W_I: 0x16, CLIPPOS_F: 0x18, W_F: 0x1E, INVW_I: 0x20, INVW_F: 0x22};

const clampS16 = (v) => Math.max(-32768, Math.min(32767, Math.round(v)));
export const fx = {
  /** float -> [int16, frac16] (s16.16) */
  s16_16(v) { const i = Math.floor(v); return [i & 0xFFFF, Math.round((v - i) * 0x10000) & 0xFFFF]; },
  s16_16_to_float(i, f) { return ((i << 16) >> 16) + f / 0x10000; },
};

/**
 * Write a transformed (screen-space) vertex as the triangle code expects it.
 * @param {Ucode} u
 * @param {number} addr DMEM address
 * @param {{x:number,y:number,z?:number,rgba?:number,s?:number,t?:number,w?:number,clip?:number,reject?:number,clipPos?:number[]}} v
 *   x/y in pixels (s13.2), z 0..0x7FFF, s/t in texels (s10.5), w clip-space W (float, stores W and 0.5/W),
 *   clip: clip-code byte, reject: rejection byte (inverted: 0xFF = not rejected)
 */
export function writeScreenVertex(u, addr, v) {
  u.w16(addr + VTX.XY, clampS16(v.x * 4));
  u.w16(addr + VTX.XY + 2, clampS16(v.y * 4));
  u.w16(addr + VTX.Z, v.z ?? 0x4000);
  u.w8(addr + VTX.CLIP, v.clip ?? 0);
  u.w8(addr + VTX.REJECT, v.reject ?? 0xFF);
  u.w32(addr + VTX.RGBA, (v.rgba ?? 0xFFFFFFFF) >>> 0);
  u.w16(addr + VTX.ST, clampS16((v.s ?? 0) * 32));
  u.w16(addr + VTX.ST + 2, clampS16((v.t ?? 0) * 32));
  const w = v.w ?? 1;
  const [wi, wf] = fx.s16_16(w);
  const [ii, iff] = fx.s16_16(0.5 / w); // the vertex loop stores the half reciprocal (0.5/W)
  const cp = v.clipPos ?? [0, 0, 0];
  for (let i = 0; i < 3; i++) { const [ci, cf] = fx.s16_16(cp[i]); u.w16(addr + VTX.CLIPPOS_I + i * 2, ci); u.w16(addr + VTX.CLIPPOS_F + i * 2, cf); }
  u.w16(addr + VTX.W_I, wi); u.w16(addr + VTX.W_F, wf);
  u.w16(addr + VTX.INVW_I, ii); u.w16(addr + VTX.INVW_F, iff);
}

/** Read back a transformed vertex from DMEM (as produced by the vertex loop). */
export function readScreenVertex(u, addr) {
  const f = (o) => fx.s16_16_to_float(u.r16(addr + o), u.r16(addr + o + 8));
  return {
    x: u.s16(addr + VTX.XY) / 4, y: u.s16(addr + VTX.XY + 2) / 4,
    z: u.s16(addr + VTX.Z),
    clip: u.r8(addr + VTX.CLIP), reject: u.r8(addr + VTX.REJECT),
    rgba: u.r32(addr + VTX.RGBA),
    s: u.s16(addr + VTX.ST) / 32, t: u.s16(addr + VTX.ST + 2) / 32,
    clipPos: [f(VTX.CLIPPOS_I), f(VTX.CLIPPOS_I + 2), f(VTX.CLIPPOS_I + 4)],
    w: f(VTX.W_I),
    invW: fx.s16_16_to_float(u.r16(addr + VTX.INVW_I), u.r16(addr + VTX.INVW_F)),
  };
}

/**
 * Write a pair of input vertices (T3DVertPacked, 32 bytes) into RDRAM.
 * @param {{pos:number[], norm?:number[], rgba?:number, st?:number[]}} a
 */
export function writePackedVertexPair(u, rdramAddr, a, b) {
  const packNorm = (n = [0, 0, 1]) => {
    const l = Math.hypot(...n) || 1;
    const q = (v, bits) => Math.round((v / l) * ((1 << (bits - 1)) - 1)) & ((1 << bits) - 1);
    return (q(n[0], 5) << 11) | (q(n[1], 6) << 5) | q(n[2], 5);
  };
  const wr = (base, v) => {
    for (let i = 0; i < 3; i++) u.rdw16(base + i * 2, clampS16(v.pos[i]));
    u.rdw16(base + 6, packNorm(v.norm));
  };
  wr(rdramAddr + 0x00, a); wr(rdramAddr + 0x08, b);
  u.rdw32(rdramAddr + 0x10, (a.rgba ?? 0xFFFFFFFF) >>> 0);
  u.rdw32(rdramAddr + 0x14, (b.rgba ?? 0xFFFFFFFF) >>> 0);
  const st = (base, v) => { const s = v.st ?? [0, 0]; u.rdw16(base, clampS16(s[0] * 32)); u.rdw16(base + 2, clampS16(s[1] * 32)); };
  st(rdramAddr + 0x18, a); st(rdramAddr + 0x1C, b);
}

/**
 * Write a 4x4 matrix (row-major float, m[row][col], applied as clip = M * pos with column
 * vectors like tiny3d: row i holds the coefficients multiplied with pos[i]) into a DMEM
 * vec16[4] slot in the ucode layout: per row int x,y,z,w then frac x,y,z,w.
 */
export function writeMatrixFP(u, addr, m) {
  for (let r = 0; r < 4; r++) for (let c = 0; c < 4; c++) {
    const [i, f] = fx.s16_16(m[r][c]);
    u.w16(addr + r * 16 + c * 2, i);
    u.w16(addr + r * 16 + 8 + c * 2, f);
  }
}

/**
 * Same computations as t3d_viewport_attach(), then runs T3DCmd_SetScreenSize and writes
 * NORM_SCALE_W like the CPU does. Returns the derived values for reference checks.
 */
export function setViewport(u, {width = 320, height = 240, offsetX = 0, offsetY = 0, guardBand = 2, normScaleW = 1.0} = {}) {
  const normWScale = Math.round(0xFFFF * Math.min(normScaleW, 1.0));
  const normWScaleFloat = normWScale / 0xFFFF;
  const guardMax = Math.floor(32767 / (4 * Math.max(width, height)));
  guardBand = Math.max(1, Math.min(guardBand & 0xF, Math.max(1, guardMax)));
  const invGuard = guardBand === 1 ? 0xFFFF : Math.floor(0x10000 / guardBand);
  u.w32(u.sym.NORM_SCALE_W, ((invGuard << 16) | invGuard) >>> 0);

  const screenFactorX = width * normWScaleFloat * 4.0 * guardBand;
  const screenFactorY = height * normWScaleFloat * -4.0 * guardBand;
  const screenScaleX = Math.round(screenFactorX * 0x10000);
  const screenScaleY = Math.round(screenFactorY * 0x10000);
  const screenOffsetX = offsetX * 2 + width;
  const screenOffsetY = offsetY * 2 + height;
  const screenOffset = ((screenOffsetX << 17) | (screenOffsetY << 1)) >>> 0;
  const screenScale = ((screenScaleX & 0xFFFF0000) | ((screenScaleY >>> 16) & 0xFFFF)) >>> 0;
  const screenScaleFrac = (((screenScaleX & 0xFFFF) << 16) | (screenScaleY & 0xFFFF)) >>> 0;
  const depthScaleFx = Math.round(0xFFFF * normWScaleFloat * 0.5 * 0x10000);
  const depthAndWScale = ((depthScaleFx & 0xFFFF0000) | normWScale) >>> 0;
  const invScreenSize = (Math.floor((1.0 / width) * 0x2A00) << 8) >>> 0;
  const guardBandScale = (guardBand | invScreenSize) >>> 0;

  u.command('T3DCmd_SetScreenSize', [guardBandScale, screenOffset, screenScale, depthAndWScale],
            {$t4: screenScaleFrac, $t5: (depthScaleFx & 0xFFFF) << 16});
  return {guardBand, normWScaleFloat, screenFactorX, screenFactorY, screenOffsetX, screenOffsetY, depthScaleFx};
}

/** Decode the RDP triangle command the triangle code wrote to DMEM. */
export function readRdpTriangle(u, addr) {
  const s32_16 = (v) => (v | 0) / 65536;
  const y = (v) => ((v << 18) >> 18) / 4;
  const w = (i) => u.r32(addr + i * 4);
  const cmd = w(0);
  const hasShade = !!(cmd & 0x04000000), hasTex = !!(cmd & 0x02000000), hasZ = !!(cmd & 0x01000000);
  let off = 0x20;
  const block = (n) => { const r = []; for (let i = 0; i < n; i++) r.push(u.s16(addr + off + i * 2)); off += n * 2; return r; };
  const res = {
    cmd: cmd >>> 24, leftMajor: !!(cmd & 0x800000), level: (cmd >>> 19) & 7, tile: (cmd >>> 16) & 7,
    yl: y(cmd), ym: y(w(1) >>> 16), yh: y(w(1)),
    xl: s32_16(w(2)), dxldy: s32_16(w(3)), xh: s32_16(w(4)), dxhdy: s32_16(w(5)), xm: s32_16(w(6)), dxmdy: s32_16(w(7)),
  };
  const attr = () => {
    const vI = block(4), dxI = block(4), vF = block(4), dxF = block(4), deI = block(4), dyI = block(4), deF = block(4), dyF = block(4);
    const c = (I, F) => I.map((v, i) => fx.s16_16_to_float(v & 0xFFFF, F[i] & 0xFFFF));
    return {v: c(vI, vF), dx: c(dxI, dxF), de: c(deI, deF), dy: c(dyI, dyF)};
  };
  if (hasShade) res.shade = attr();
  if (hasTex) res.tex = attr();
  if (hasZ) { const z = block(8); res.z = {v: fx.s16_16_to_float(z[0] & 0xFFFF, z[1] & 0xFFFF), dx: fx.s16_16_to_float(z[2] & 0xFFFF, z[3] & 0xFFFF), de: fx.s16_16_to_float(z[4] & 0xFFFF, z[5] & 0xFFFF), dy: fx.s16_16_to_float(z[6] & 0xFFFF, z[7] & 0xFFFF)}; }
  res.size = off;
  return res;
}

// ---------------------------------------------------------------------------
// Golden files (tests/rsp/golden/<name>.json)
// ---------------------------------------------------------------------------
const GOLDEN_DIR = fileURLToPath(new URL('./golden/', import.meta.url));
export const hex = (bytes) => bytes.map((b) => b.toString(16).padStart(2, '0')).join('');

/**
 * Compare `actual` (array of {name, ...}) with the recorded golden; records it when the
 * file does not exist yet or UPDATE_GOLDEN=1 is set.
 */
export function checkGolden(name, actual) {
  const file = GOLDEN_DIR + name + '.json';
  if (process.env.UPDATE_GOLDEN || !existsSync(file)) {
    mkdirSync(GOLDEN_DIR, {recursive: true});
    writeFileSync(file, JSON.stringify(actual, null, 1) + '\n');
    if (!process.env.UPDATE_GOLDEN) console.log(`recorded new golden ${file}`);
    return;
  }
  const expected = JSON.parse(readFileSync(file, 'utf8'));
  assert.equal(actual.length, expected.length, `golden '${name}': case count changed, re-record with UPDATE_GOLDEN=1`);
  for (let i = 0; i < expected.length; i++) {
    assert.deepEqual(actual[i], expected[i], `golden '${name}' case '${expected[i].name}' differs`);
  }
}
