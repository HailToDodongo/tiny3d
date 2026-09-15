#!/usr/bin/env node
// Depth regression metrics + comparison (see tests/depth.sh).
//
// Usage: node tests/depth_compare.js <golden.txt> <actual.txt> [--update]
//
// Both files are the "step w z18" blocks written by tests/depth.test.js.
// The metrics below are computed for both and printed side by side, always.
// Any metric that got worse fails the test, no tolerance. A different sample
// vector with equal-or-better metrics is reported as CHANGED (still a pass);
// --update overwrites the golden with the actual file.
//
// Metrics (all derived from the 2000 sweep samples + near/far):
//   codes      distinct stored z-buffer codes (more = finer)
//   err rms    rms  |Z - ideal| in Z units (ideal = exact mapping, pushed
//   err max    max  |Z - ideal|             through the RDP z-buffer format)
//   bias       mean (Z - ideal), Z units (sign of the systematic error)
//   inv        samples whose Z is smaller than a nearer sample's Z
//   inv reach  worst distance (world units) such a wrong-order pair spans
//   amb p99    99th percentile / max of the ambiguity span: at a sample, how
//   amb max    far back + forward another sample sits whose Z is not sorted
//              strictly against it (equal or inverted), in world units

const fs = require("fs");
const [goldenPath, actualPath, ...flags] = process.argv.slice(2);
if (!goldenPath || !actualPath) { console.error("usage: depth_compare.js <golden.txt> <actual.txt> [--update]"); process.exit(2); }
const update = flags.includes("--update");

function parse(path) {
  const meta = {}, samples = [];
  for (const line of fs.readFileSync(path, "utf8").split("\n")) {
    if (!line.trim()) continue;
    if (line[0] === "#") {
      for (const tok of line.slice(1).trim().split(/\s+/)) { const eq = tok.indexOf("="); if (eq > 0) meta[tok.slice(0, eq)] = tok.slice(eq + 1); }
      continue;
    }
    const [step, w, z] = line.trim().split(/\s+/);
    samples.push({ step: +step, w: +w, z: +z });
  }
  samples.sort((a, b) => a.w - b.w);
  return { meta, samples, near: +meta.near, far: +meta.far };
}

// ---- reference: exact perspective mapping -> RDP z-buffer format roundtrip (angrylion) ----
const idealZ = (w, near, far) => { const zndc = ((far + near) * w - 2 * far * near) / ((far - near) * w); return (zndc + 1) * 0.5 * 32767; };
const Z_DEC = [[6, 0x00000], [5, 0x20000], [4, 0x30000], [3, 0x38000], [2, 0x3C000], [1, 0x3E000], [0, 0x3F000], [0, 0x3F800]];
function rdpRoundtrip(zUnits) {
  const z16 = Math.floor(zUnits * 65536);
  let sz = (z16 >> 10) & 0x3fffff;
  sz = sz >> 3;
  sz = Math.max(0, Math.min(0x3FFFF, sz));
  const key = (sz >> 11) & 0x7F; let m, e;
  if (key <= 0x3F) { m = (sz >> 4) & 0x1FFC; e = 0; } else if (key <= 0x5F) { m = (sz >> 3) & 0x1FFC; e = 1; }
  else if (key <= 0x6F) { m = (sz >> 2) & 0x1FFC; e = 2; } else if (key <= 0x77) { m = (sz >> 1) & 0x1FFC; e = 3; }
  else if (key <= 0x7B) { m = sz & 0x1FFC; e = 4; } else if (key <= 0x7D) { m = (sz << 1) & 0x1FFC; e = 5; }
  else if (key === 0x7E) { m = (sz << 2) & 0x1FFC; e = 6; } else { m = (sz << 2) & 0x1FFC; e = 7; }
  return (((m >> 2) << Z_DEC[e][0]) + Z_DEC[e][1]) & 0x3ffff;
}

function metrics(d) {
  const sm = d.samples, n = sm.length;
  const codes = new Set(sm.map((s) => s.z)).size;
  let sq = 0, mx = 0, sum = 0;
  for (const s of sm) {
    const e = (s.z - rdpRoundtrip(idealZ(s.w, d.near, d.far))) / 8;
    sq += e * e; mx = Math.max(mx, Math.abs(e)); sum += e;
  }
  let inv = 0, invReach = 0;
  const amb = [];
  for (let i = 0; i < n; i++) {
    let jf = i, jb = i, jInv = -1;
    for (let j = n - 1; j > i; j--) if (sm[j].z <= sm[i].z) { jf = j; break; }
    for (let j = n - 1; j > i; j--) if (sm[j].z < sm[i].z) { jInv = j; break; }
    for (let j = 0; j < i; j++) if (sm[j].z >= sm[i].z) { jb = j; break; }
    if (i > 0 && sm[i - 1].z > sm[i].z) inv++;
    if (jInv >= 0) invReach = Math.max(invReach, sm[jInv].w - sm[i].w);
    amb.push((sm[jf].w - sm[i].w) + (sm[i].w - sm[jb].w));
  }
  amb.sort((a, b) => a - b);
  return {
    codes, errRms: Math.sqrt(sq / n), errMax: mx, bias: sum / n,
    inv, invReach, ambP99: amb[Math.floor(0.99 * (n - 1))], ambMax: amb[n - 1],
  };
}

// name, label, unit, better = sign of (actual - golden) that is an improvement (+1 higher is better, -1 lower is better)
const TABLE = [
  ["codes",    "codes",     "",        +1, (v) => String(v)],
  ["errRms",   "err rms",   "Z units", -1, (v) => v.toFixed(3)],
  ["errMax",   "err max",   "Z units", -1, (v) => v.toFixed(3)],
  ["bias",     "bias",      "Z units",  0, (v) => v.toFixed(3)],
  ["inv",      "inversions", "",       -1, (v) => String(v)],
  ["invReach", "inv reach", "world",   -1, (v) => v.toFixed(3)],
  ["ambP99",   "amb p99",   "world",   -1, (v) => v.toFixed(3)],
  ["ambMax",   "amb max",   "world",   -1, (v) => v.toFixed(3)],
];

const actual = parse(actualPath);
if (actual.samples.length === 0) { console.log("no samples in " + actualPath); process.exit(1); }
const golden = fs.existsSync(goldenPath) ? parse(goldenPath) : null;

const mA = metrics(actual), mG = golden ? metrics(golden) : null;
const pad = (s, n) => String(s).padStart(n);
console.log(`  near=${actual.near} far=${actual.far}  samples=${actual.samples.length}` + (golden ? "" : "  (no golden yet)"));
console.log("  " + "metric".padEnd(12) + pad("golden", 12) + pad("actual", 12) + pad("delta", 12) + "  unit");
let worse = 0, better = 0;
for (const [key, label, unit, sign, f] of TABLE) {
  const a = mA[key], g = mG ? mG[key] : null;
  let tag = "";
  if (mG) {
    const d = a - g;
    if (sign !== 0 && d !== 0) { if (Math.sign(d) === sign) { better++; tag = "  better"; } else { worse++; tag = "  WORSE"; } }
    else if (sign === 0 && Math.abs(a) > Math.abs(g)) tag = "  (larger)";
  }
  console.log("  " + label.padEnd(12) + pad(mG ? f(g) : "-", 12) + pad(f(a), 12) + pad(mG ? (a - g === 0 ? "0" : f(a - g)) : "-", 12) + "  " + unit + tag);
}

let status;
if (!golden) {
  status = "CREATED";
} else {
  if (golden.near !== actual.near || golden.far !== actual.far || golden.samples.length !== actual.samples.length) {
    console.log(`  golden setup differs (near/far/steps), re-record with --update`);
    process.exit(1);
  }
  let diff = 0, first = -1, wDiff = 0;
  for (let i = 0; i < actual.samples.length; i++) {
    const a = actual.samples[i], g = golden.samples[i];
    if (a.z !== g.z) { diff++; if (first < 0) first = i; }
    if (a.w !== g.w) wDiff++;
  }
  if (wDiff) console.log(`  NOTE: sweep distances differ from the golden for ${wDiff} samples (scene setup changed?)`);
  if (diff) {
    const a = actual.samples[first], g = golden.samples[first];
    console.log(`  samples differ: ${diff} of ${actual.samples.length}, first at step ${a.step} (w=${a.w}: golden Z ${g.z / 8}, actual Z ${a.z / 8})`);
  }
  diff += wDiff;
  status = worse ? "FAIL" : diff ? "CHANGED" : "SAME";
}
console.log(`  result: ${status}` + (worse ? ` (${worse} metric(s) worse)` : better ? ` (${better} metric(s) better)` : "") +
            (status === "CHANGED" ? "  -> accept with --update" : ""));

if (update || !golden) {
  fs.copyFileSync(actualPath, goldenPath);
  console.log(`  ${golden ? "updated" : "created"} golden ${goldenPath}`);
}
process.exit(worse ? 1 : 0);
