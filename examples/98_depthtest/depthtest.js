// Depth precision measurement driver for 98_depthtest.
//
// Usage: ares-test examples/98_depthtest/depthtest.js <rom.z64> [preset=0] [scenes=sweep,plane,sep] [dump] [label]
//   preset: 0 = 10/12800, 1 = 10/1000, 2 = 10/150, 3 = 1/100
//   dump:   any non-empty value -> the raw measurements (sweep samples, plane depth
//           buffers, colour classification maps, SEP patterns) are printed as one JSON
//           document between "DTDUMP-BEGIN" / "DTDUMP-END" lines (see dump.sh), so the
//           results can be visualised (tools/depth_report.html) without re-running.
//   label:  free-form name stored in the dump (defaults to the ROM file name)
//
// Drives the ROM through its scenes via controller input, reads the decoded
// 18-bit z-buffer (ares.depthBuffer) / colour buffer at the exact moment each
// frame completes (the ROM prints its "DT ..." line right after rspq_wait,
// onLog fires on that line), and condenses everything into a few numbers:
//
//   SWEEP  bits            log2(distinct stored depth values over [near, far])
//          inversions      steps where Z decreases although w increases (max size)
//          vertex err      max / p99 of |dw|/w, from the residual against a
//                          least-squares fit Z = A + B/w (mapping-agnostic)
//   PLANE  slope err       single quad vs analytic depth   (|dw|/w max / rms)
//          vertex err      dense grid vs analytic depth
//          coplanar        max |Z_single - Z_dense| in integer Z units
//          decal fail %    pixels where a coplanar DECAL pass lost
//          opaque fight %  pixels where a coplanar opaque pass lost
//   SEP    separable g/d   per distance: smallest gap that (reliably) resolves
//
// Analytic plane depth uses pixel centres at (px+0.5, py+0.5); the RDP's
// exact sample convention may differ by a fraction of a pixel, which mostly
// affects the grazing far rows - treat the plane's absolute numbers as
// slightly pessimistic and the coplanar / % numbers as exact.

const [rom, presetArg, scenesArg, dumpArg, labelArg] = ares.args;
if (!rom) throw new Error("usage: depthtest.js <rom.z64> [preset] [scenes] [dump] [label]");
const PRESET = parseInt(presetArg || "0", 10);
const SCENES = (scenesArg || "sweep,plane,sep").split(",");
const DUMP = !!dumpArg;
const dump = { format: 1, rom: rom.split("/").pop(), label: labelArg || rom.split("/").pop(), preset: PRESET,
               screen: { w: 320, h: 240 }, fovDeg: 60 };

// base64 of a Uint32Array (little endian), used for the 320x240 depth buffers
function b64u32(arr) {
  const bytes = new Uint8Array(arr.buffer, arr.byteOffset, arr.byteLength);
  const T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  let out = "", i = 0;
  for (; i + 2 < bytes.length; i += 3) {
    const n = (bytes[i] << 16) | (bytes[i + 1] << 8) | bytes[i + 2];
    out += T[n >> 18] + T[(n >> 12) & 63] + T[(n >> 6) & 63] + T[n & 63];
  }
  if (i < bytes.length) {
    const n = (bytes[i] << 16) | ((i + 1 < bytes.length ? bytes[i + 1] : 0) << 8);
    out += T[n >> 18] + T[(n >> 12) & 63] + (i + 1 < bytes.length ? T[(n >> 6) & 63] : "=") + "=";
  }
  return out;
}

const W = 320, H = 240, FOV_DEG = 60, ASPECT = W / H;
const TAN_HALF = Math.tan(FOV_DEG / 2 * Math.PI / 180);
const SUB_COUNT = [2, 4, 2], STEP_COUNT = [2000, 4, 8];
const SEP_GRID = 4;

ares.setRenderer("angrylion");
ares.loadRom(rom);
ares.resume();
if (!ares.waitLog("DT ready", 60)) throw new Error("ROM did not boot:\n" + ares.log());

const p1 = ares.controller(1);

// ---- ROM state tracking + input -------------------------------------------
const cur = { scene: 0, sub: 0, step: 0, preset: 0 };
const marker = (s) => `DT scene=${s.scene} sub=${s.sub} step=${s.step} `;

// hold a button until the ROM reports the expected state (its frames can be
// longer than one VI, so a fixed-length press could be missed)
function pressUntil(btn, expect) {
  ares.clearLog();
  p1.hold(btn);
  const ok = ares.waitLog(marker(expect), 20);
  p1.release(btn);
  ares.waitVI(); ares.waitVI();
  if (!ok) throw new Error(`button ${btn}: ROM never reached ${JSON.stringify(expect)}\n` + ares.log().slice(-800));
  Object.assign(cur, expect);
}

function setPreset(p) {
  while (cur.preset !== p) pressUntil("R", { ...cur, preset: (cur.preset + 1) % 4 });
}
function setScene(scene, sub, step) {
  while (cur.scene !== scene) {
    pressUntil("C-Right", { scene: (cur.scene + 1) % 3, sub: 0, step: 0, preset: cur.preset });
  }
  while (cur.sub !== sub) pressUntil("A", { ...cur, sub: (cur.sub + 1) % SUB_COUNT[scene] });
  if (cur.step !== step) {
    if (cur.step !== 0) pressUntil("Z", { ...cur, step: 0 });
    while (step - cur.step >= 100) pressUntil("Up", { ...cur, step: Math.min(cur.step + 100, STEP_COUNT[scene] - 1) });
    while (cur.step < step) pressUntil("Right", { ...cur, step: cur.step + 1 });
  }
}

// ---- capture ----------------------------------------------------------------
function parseLine(line) {
  const o = {};
  for (const tok of line.slice(3).split(" ")) {
    const eq = tok.indexOf("=");
    if (eq > 0) o[tok.slice(0, eq)] = tok.slice(eq + 1);
  }
  return o;
}
let onFrame = null;
ares.onLog((line) => {
  if (!onFrame || !line.startsWith("DT scene=")) return;
  onFrame(parseLine(line));
});

// wait until the current state has been rendered and captured once
function captureCurrent(fn) {
  let done = false;
  onFrame = (st) => {
    if (done) return;
    if (+st.scene !== cur.scene || +st.sub !== cur.sub || +st.step !== cur.step) return;
    fn(st); done = true;
  };
  ares.clearLog();
  if (!ares.waitLog(marker(cur), 20)) throw new Error("no frame for " + marker(cur));
  onFrame = null;
  if (!done) throw new Error("capture missed for " + marker(cur));
}

const depthFull = () => new Uint32Array(ares.depthBuffer(W, H).data);
function depthRegion(x, y, w, h) { return new Uint32Array(ares.depthBuffer(W, H, { x, y, width: w, height: h }).data); }
function shotRGB() {
  const s = ares.screenshot();
  return { w: s.width, h: s.height, px: new Uint8Array(s.data) };
}
function classify(shot, x, y) { // x,y in 320x240 space -> "red" | "green" | "other"
  const sx = Math.floor((x + 0.5) * shot.w / W), sy = Math.floor((y + 0.5) * shot.h / H);
  const i = (sy * shot.w + sx) * 4, r = shot.px[i], g = shot.px[i + 1], b = shot.px[i + 2];
  if (r > 128 && g < 100 && b < 100) return "red";
  if (g > 128 && r < 100 && b < 100) return "green";
  return "other";
}

const fmtPct = (v) => (v * 100).toFixed(4) + "%";
const fmt = (v, d = 2) => (typeof v === "number" ? v.toFixed(d) : String(v));

// ---- SWEEP -----------------------------------------------------------------
let fit = null; // {A, B}: Z = A + B/w (18-bit units)
function runSweep(info) {
  setScene(0, 0, 0);
  const samples = []; // {step, w, z, spread}
  const RX = 128, RY = 104, RW = 64, RH = 32;
  onFrame = (st) => {
    if (+st.scene !== 0 || +st.sub !== 0) return;
    const step = +st.step;
    if (samples.length && samples[samples.length - 1].step === step) return;
    const z = depthRegion(RX, RY, RW, RH);
    const counts = new Map();
    for (const v of z) counts.set(v, (counts.get(v) || 0) + 1);
    let mode = 0, modeN = 0;
    for (const [v, n] of counts) if (n > modeN) { mode = v; modeN = n; }
    samples.push({ step, w: +st.w, z: mode, spread: z.length - modeN });
  };
  ares.clearLog();
  if (!ares.waitLog(marker(cur), 20)) throw new Error("sweep: no first frame");
  // auto-advance: one step per frame
  p1.hold("Start");
  if (!ares.waitLog("step=1 ", 20)) throw new Error("sweep: auto-advance did not start");
  p1.release("Start");
  if (!ares.waitLog(`step=${STEP_COUNT[0] - 1} `, 600)) throw new Error("sweep: did not reach the last step");
  ares.waitVI(); ares.waitVI();
  onFrame = null;
  cur.step = STEP_COUNT[0] - 1;

  samples.sort((a, b) => a.w - b.w);
  const n = samples.length;
  // least squares Z = A + B * (1/w)
  let sx = 0, sy = 0, sxx = 0, sxy = 0;
  for (const s of samples) { const x = 1 / s.w; sx += x; sy += s.z; sxx += x * x; sxy += x * s.z; }
  const B = (n * sxy - sx * sy) / (n * sxx - sx * sx);
  const A = (sy - B * sx) / n;
  fit = { A, B };

  const distinct = new Set(samples.map((s) => s.z)).size;
  let inversions = 0, maxInv = 0;
  for (let i = 1; i < n; ++i) {
    const d = samples[i - 1].z - samples[i].z;
    if (d > 0) { inversions++; maxInv = Math.max(maxInv, d); }
  }
  const relErr = samples.map((s) => Math.abs(s.z - (A + B / s.w)) * s.w / Math.abs(B));
  const zErr = samples.map((s) => Math.abs(s.z - (A + B / s.w)) / 8);
  const sortedRel = [...relErr].sort((a, b) => a - b);
  const p99 = sortedRel[Math.floor(0.99 * (n - 1))];
  const maxRel = sortedRel[n - 1];
  const maxZ = Math.max(...zErr);
  const rmsZ = Math.sqrt(zErr.reduce((a, v) => a + v * v, 0) / n);
  const nonUniform = samples.filter((s) => s.spread > 0).length;
  const zMin = samples[0].z / 8, zMax = samples[n - 1].z / 8;

  console.log(`\n== SWEEP  ${info}  (${n} samples, w ${fmt(samples[0].w)} .. ${fmt(samples[n - 1].w)})`);
  console.log(`  stored codes   : ${distinct}  (${Math.log2(distinct).toFixed(2)} bits)   Z range ${zMin.toFixed(1)} .. ${zMax.toFixed(1)} (integer units, 0..32767)`);
  console.log(`  inversions     : ${inversions}  (max ${(maxInv / 8).toFixed(2)} Z units)`);
  console.log(`  vertex error   : max ${maxZ.toFixed(2)} / rms ${rmsZ.toFixed(2)} Z units;  |dw|/w max ${fmtPct(maxRel)}  p99 ${fmtPct(p99)}`);
  console.log(`  fit            : Z = ${A.toFixed(1)} + ${B.toFixed(1)}/w   (18-bit units)`);
  if (nonUniform) console.log(`  WARNING: ${nonUniform} samples had non-uniform depth across the flat quad (max spread px)`);
  const res = { distinct, bits: Math.log2(distinct), inversions, maxInv: maxInv / 8, maxZ, rmsZ, maxRel, p99 };
  if (DUMP) dump.sweep = { region: { x: RX, y: RY, w: RW, h: RH }, samples, fit: { A, B }, result: res };
  return res;
}

// ---- PLANE -----------------------------------------------------------------
function runPlane(info) {
  const res = {};
  // sub 0 single, sub 1 dense: depth
  const depths = {};
  let params = null;
  for (const sub of [0, 1]) {
    setScene(1, sub, 0);
    captureCurrent((st) => { depths[sub] = depthFull(); params = st; });
  }
  const h = +params.h;
  const emptyZ = depths[0][0]; // pixel (0,0) is always background
  const isFilled = (buf, x, y) => x >= 0 && y >= 0 && x < W && y < H && buf[y * W + x] < emptyZ;
  // footprint: filled in both, eroded by 2 px
  const inFoot = (x, y) => {
    for (let dy = -2; dy <= 2; ++dy) for (let dx = -2; dx <= 2; ++dx)
      if (!isFilled(depths[0], x + dx, y + dy) || !isFilled(depths[1], x + dx, y + dy)) return false;
    return true;
  };
  let nPix = 0, copMax = 0, copSq = 0;
  const err = { 0: { max: 0, sq: 0 }, 1: { max: 0, sq: 0 } };
  for (let y = 0; y < H; ++y) for (let x = 0; x < W; ++x) {
    if (!inFoot(x, y)) continue;
    nPix++;
    const zs = depths[0][y * W + x], zd = depths[1][y * W + x];
    const cop = Math.abs(zs - zd) / 8;
    copMax = Math.max(copMax, cop); copSq += cop * cop;
    if (fit) {
      const ndcY = 1 - (y + 0.5) / H * 2;
      if (ndcY >= 0) continue;
      const w = h / (-ndcY * TAN_HALF);
      const zi = fit.A + fit.B / w;
      for (const sub of [0, 1]) {
        const rel = Math.abs(depths[sub][y * W + x] - zi) * w / Math.abs(fit.B);
        err[sub].max = Math.max(err[sub].max, rel); err[sub].sq += rel * rel;
      }
    }
  }
  // sub 2 decal, sub 3 opaque: colour
  const fails = {};
  for (const sub of [2, 3]) {
    setScene(1, sub, 0);
    let shot = null;
    captureCurrent(() => { shot = shotRGB(); });
    let red = 0, green = 0;
    const map = new Array(H);
    for (let y = 0; y < H; ++y) {
      let row = "";
      for (let x = 0; x < W; ++x) {
        const c = classify(shot, x, y);
        row += c === "red" ? "r" : c === "green" ? "g" : ".";
        if (!inFoot(x, y)) continue;
        if (c === "red") red++; else if (c === "green") green++;
      }
      map[y] = row;
    }
    fails[sub] = red / Math.max(1, red + green);
    if (DUMP) (dump.planeMaps = dump.planeMaps || {})[sub] = map.join("\n");
  }
  console.log(`\n== PLANE  ${info}  (h=${h}, ${nPix} px evaluated)`);
  if (fit) {
    console.log(`  slope error  (single quad) : |dw|/w max ${fmtPct(err[0].max)}  rms ${fmtPct(Math.sqrt(err[0].sq / nPix))}`);
    console.log(`  vertex error (dense grid)  : |dw|/w max ${fmtPct(err[1].max)}  rms ${fmtPct(Math.sqrt(err[1].sq / nPix))}`);
  } else {
    console.log(`  (run the sweep first to get absolute errors against the fitted mapping)`);
  }
  console.log(`  coplanar |Zsingle - Zdense| : max ${copMax.toFixed(2)}  rms ${Math.sqrt(copSq / nPix).toFixed(2)} Z units`);
  console.log(`  decal fail                  : ${fmtPct(fails[2])} of the plane`);
  console.log(`  opaque coplanar fight       : ${fmtPct(fails[3])} of the plane lost by the dense mesh`);
  const out = { slopeMax: err[0].max, vertexMax: err[1].max, copMax, decalFail: fails[2], fight: fails[3],
                slopeRms: Math.sqrt(err[0].sq / Math.max(1, nPix)), vertexRms: Math.sqrt(err[1].sq / Math.max(1, nPix)), nPix };
  if (DUMP) {
    dump.plane = { params, h, emptyZ, depthSingle: b64u32(depths[0]), depthDense: b64u32(depths[1]),
                   decalMap: (dump.planeMaps || {})[2], opaqueMap: (dump.planeMaps || {})[3], result: out };
    delete dump.planeMaps;
  }
  return out;
}

// ---- SEP -------------------------------------------------------------------
function runSep(info) {
  const rows = [];
  for (const sub of [0, 1]) {
    for (let step = 0; step < STEP_COUNT[2]; ++step) {
      setScene(2, sub, step);
      let shot = null, params = null;
      captureCurrent((st) => { shot = shotRGB(); params = st; });
      const gaps = params.gapFrac.split(",").map(Number);
      const d = +params.d;
      const cellW = W / SEP_GRID, cellH = H / SEP_GRID;
      const pass = [];
      for (let cell = 0; cell < SEP_GRID * SEP_GRID; ++cell) {
        const cx = (cell % SEP_GRID + 0.5) * cellW, cy = (Math.floor(cell / SEP_GRID) + 0.5) * cellH;
        // front quad covers 40% of the cell; evaluate its central part
        const ex = cellW * 0.4 * 0.5 * 0.6, ey = cellH * 0.4 * 0.5 * 0.5;
        let red = 0, green = 0, tot = 0;
        for (let y = Math.round(cy - ey); y <= Math.round(cy + ey); ++y)
          for (let x = Math.round(cx - ex); x <= Math.round(cx + ex); ++x) {
            const c = classify(shot, x, y); tot++;
            if (c === "red") red++; else if (c === "green") green++;
          }
        pass.push(red === 0 && green > tot * 0.5);
      }
      // cells are ordered by decreasing gap: "reliable" = the last cell of the
      // initial run of passes, "smallest" = the smallest gap that passed at all
      let smallest = null;
      for (let c = 0; c < pass.length; ++c) if (pass[c]) smallest = gaps[c];
      const firstFail = pass.indexOf(false);
      const reliable = firstFail === -1 ? gaps[gaps.length - 1] : (firstFail === 0 ? null : gaps[firstFail - 1]);
      rows.push({ sub, step, d, gaps, pattern: pass.map((p) => (p ? "#" : ".")).join(""), reliable, smallest });
    }
  }
  console.log(`\n== SEP  ${info}   (cells: gap/d from ${fmt(Math.pow(2, -3), 4)} down to ${fmt(Math.pow(2, -(3 + 0.75 * 15)), 6)}, '#' = resolved)`);
  console.log(`  tilt  d          pattern           reliable g/d    smallest passing g/d`);
  for (const r of rows) {
    console.log(`  ${r.sub ? "30" : " 0"}  ${r.d.toFixed(2).padStart(9)}  ${r.pattern}  ${r.reliable === null ? "   none" : fmtPct(r.reliable).padStart(11)}    ${r.smallest === null ? "none" : fmtPct(r.smallest)}`);
  }
  if (DUMP) dump.sep = rows;
  return rows;
}

// ---- main ------------------------------------------------------------------
setPreset(PRESET);
// read the preset's near/far from the ROM's own line
let presetInfo = "";
captureCurrent((st) => { presetInfo = `near=${st.near} far=${st.far}`; });
console.log(`\n#### depth test: ${rom}  preset ${PRESET} (${presetInfo})`);

const results = {};
if (SCENES.includes("sweep")) results.sweep = runSweep(presetInfo);
if (SCENES.includes("plane")) results.plane = runPlane(presetInfo);
if (SCENES.includes("sep")) results.sep = runSep(presetInfo);

console.log("\n#### summary " + presetInfo);
if (results.sweep) console.log(`  bits ${results.sweep.bits.toFixed(2)} | inversions ${results.sweep.inversions} (max ${results.sweep.maxInv.toFixed(1)}) | vertex err max ${results.sweep.maxZ.toFixed(1)} Z, |dw|/w p99 ${fmtPct(results.sweep.p99)}`);
if (results.plane) console.log(`  coplanar max ${results.plane.copMax.toFixed(1)} Z | decal fail ${fmtPct(results.plane.decalFail)} | opaque fight ${fmtPct(results.plane.fight)}`);
if (results.sep) {
  const r0 = results.sep.filter((r) => r.sub === 0);
  console.log(`  separable g/d (flat): ` + r0.map((r) => `${r.d.toFixed(0)}:${r.reliable === null ? "none" : fmtPct(r.reliable)}`).join("  "));
}
console.log("#### done");
if (DUMP) {
  dump.near = +presetInfo.match(/near=(\S+)/)[1];
  dump.far = +presetInfo.match(/far=(\S+)/)[1];
  dump.summary = { sweep: results.sweep, plane: results.plane };
  console.log("DTDUMP-BEGIN");
  console.log(JSON.stringify(dump));
  console.log("DTDUMP-END");
}
