// Depth regression capture for 98_depthtest (see tests/depth.sh).
//
// Usage: ares-test tests/depth.test.js <rom.z64> <preset>
//
// Runs only the SWEEP scene of the depth test ROM for one near/far preset:
// a view-aligned quad is moved through 2000 log-spaced distances and the
// stored z-buffer code (18-bit decoded, ares.depthBuffer) is read for every
// step. The result is printed as a text block between "DEPTH-BEGIN" and
// "DEPTH-END", one "step w z" line per sample, which tests/depth_compare.js
// turns into metrics and compares against tests/depth/*.txt.
//
// A screen-aligned quad only exercises the vertex side of the ucode (W
// normalisation, reciprocal, Z multiply, fraction handling), which is exactly
// the part an optimisation can silently degrade, so this is the complete
// per-vertex depth behaviour for the preset in a few KB.

const [rom, presetArg] = ares.args;
if (!rom) throw new Error("usage: depth.test.js <rom.z64> <preset>");
const PRESET = parseInt(presetArg || "0", 10);

const W = 320, H = 240;
const PRESET_COUNT = 4, STEPS = 2000;
const RX = 128, RY = 104, RW = 64, RH = 32; // centre region of the quad

ares.setRenderer("angrylion");
ares.loadRom(rom);
ares.resume();
if (!ares.waitLog("DT ready", 60)) throw new Error("ROM did not boot:\n" + ares.log());

const p1 = ares.controller(1);
const cur = { scene: 0, sub: 0, step: 0, preset: 0 };
const marker = (s) => `DT scene=${s.scene} sub=${s.sub} step=${s.step} `;

function pressUntil(btn, expect) {
  ares.clearLog();
  p1.hold(btn);
  const ok = ares.waitLog(marker(expect), 20);
  p1.release(btn);
  ares.waitVI(); ares.waitVI();
  if (!ok) throw new Error(`button ${btn}: ROM never reached ${JSON.stringify(expect)}\n` + ares.log().slice(-800));
  Object.assign(cur, expect);
}
while (cur.preset !== PRESET) pressUntil("R", { ...cur, preset: (cur.preset + 1) % PRESET_COUNT });

function parseLine(line) {
  const o = {};
  for (const tok of line.slice(3).split(" ")) {
    const eq = tok.indexOf("=");
    if (eq > 0) o[tok.slice(0, eq)] = tok.slice(eq + 1);
  }
  return o;
}

const samples = [];
let info = null;
ares.onLog((line) => {
  if (!line.startsWith("DT scene=")) return;
  const st = parseLine(line);
  if (+st.scene !== 0 || +st.sub !== 0) return;
  const step = +st.step;
  if (samples.length && samples[samples.length - 1].step === step) return;
  info = info || { near: +st.near, far: +st.far };
  const z = new Uint32Array(ares.depthBuffer(W, H, { x: RX, y: RY, width: RW, height: RH }).data);
  const counts = new Map();
  for (const v of z) counts.set(v, (counts.get(v) || 0) + 1);
  let mode = 0, modeN = 0;
  for (const [v, n] of counts) if (n > modeN) { mode = v; modeN = n; }
  samples.push({ step, w: st.w, z: mode, spread: z.length - modeN });
});

ares.clearLog();
if (!ares.waitLog(marker(cur), 20)) throw new Error("sweep: no first frame");
p1.hold("Start"); // auto-advance one step per frame
if (!ares.waitLog("step=1 ", 20)) throw new Error("sweep: auto-advance did not start");
p1.release("Start");
if (!ares.waitLog(`step=${STEPS - 1} `, 900)) throw new Error("sweep: did not reach the last step");
ares.waitVI(); ares.waitVI();

if (ares.log().includes("RSP CRASH") || ares.log().includes("CPU exception")) throw new Error("ROM crashed:\n" + ares.log());
samples.sort((a, b) => a.step - b.step);
if (samples.length !== STEPS) throw new Error(`sweep: captured ${samples.length} of ${STEPS} steps`);
const nonUniform = samples.filter((s) => s.spread > 0).length;
if (nonUniform) console.log(`WARNING: ${nonUniform} samples had non-uniform depth across the flat quad`);

console.log("DEPTH-BEGIN");
console.log(`# rom=${rom.split("/").pop()} preset=${PRESET} near=${info.near} far=${info.far} steps=${STEPS}`);
console.log("# step w z18");
for (const s of samples) console.log(`${s.step} ${s.w} ${s.z}`);
console.log("DEPTH-END");
