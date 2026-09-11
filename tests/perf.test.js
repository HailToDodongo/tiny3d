// RSP performance test for a single example (invoked by tests/perf.sh).
//
// Usage: ares-test tests/perf.test.js <rom.z64> <config>
//   config   contents of tests/perf/<name>.json:
//            { "framesWarmup": n, "framesProfile": n, "data": {"tiny3d/<cmd>": usPerFrame, ...} }
//
// Boots the ROM, profiles the RSP over a fixed number of game frames and prints the
// per-command time, a grouped summary, and the comparison against the baseline.
// Exits non-zero if any command got slower than the baseline (beyond TOLERANCE_US).

const [rom, configArg] = ares.args;
if (!rom || !configArg) throw new Error("usage: perf.test.js <rom.z64> <config-json>");

const cfg = JSON.parse(configArg);
const framesWarmup = cfg.framesWarmup;
const frames = cfg.framesProfile;
const lastPerf = cfg.data ?? {};
if (!(framesWarmup >= 0) || !(frames > 0)) {
  throw new Error("config needs framesWarmup >= 0 and framesProfile > 0");
}

const TOLERANCE_US = 0.0025; // ignore differences below this (us/frame)
const RSP_MHZ = 62.5;

ares.setRenderer("angrylion");
ares.loadRom(rom);
ares.resume();
ares.waitFrames(framesWarmup);

ares.waitRspCommand("Screen Size");
ares.rspProfileStart();
ares.waitRspCommand("Screen Size", frames);
const t = ares.rspProfile();

if (ares.log().includes("RSP CRASH") || ares.log().includes("CPU exception")) {
  throw new Error("ROM crashed:\n" + ares.log());
}

const usPerFrame = (c) => c / frames / RSP_MHZ; // 62.5 MHz -> us per game frame

console.log("");
console.log("RSP profile over " + frames + " game frames  (lostRows: " + t.lostRows + ")");
console.log("total: " + Math.round(t.totalCycles) + " cycles  = " +
            usPerFrame(t.totalCycles).toFixed(2) + " us/frame  (commands " +
            usPerFrame(t.commandCycles).toFixed(2) + ", rspq overhead " +
            usPerFrame(t.overheadCycles).toFixed(2) + ")");
console.log("");
console.log("command".padEnd(30) + "count".padStart(8) + "avg cyc".padStart(10) +
            "cyc/frame".padStart(12) + "us/frame".padStart(10) + "share".padStart(8));

const perfMap = {};
for (const r of t.rows) {
  if (r.overlay !== "tiny3d") continue;
  const name = r.overhead ? "rspq: " + r.overheadType : r.overlay + "/" + r.name;
  const usPF = usPerFrame(r.cycles);
  perfMap[name] = usPF;
  console.log(name.padEnd(30) +
              String(r.count).padStart(8) +
              r.avg.toFixed(1).padStart(10) +
              String(Math.round(r.cycles / frames)).padStart(12) +
              usPF.toFixed(2).padStart(10) +
              ((r.cycles / t.totalCycles) * 100).toFixed(1).padStart(7) + "%");
}

// grouped summary for the vertex/triangle refactor work
const GROUPS = [
  { label: "triangles (Draw+Strip+Seq)", names: ["Tri Draw", "Tri Strip", "Tri Seq"] },
  { label: "vertices (Vert Load)",       names: ["Vert Load"] },
];
console.log("");
console.log("group".padEnd(30) + "count".padStart(8) + "avg cyc".padStart(10) +
            "cyc/frame".padStart(12) + "us/frame".padStart(10) + "share".padStart(8));
for (const g of GROUPS) {
  let count = 0, cycles = 0;
  for (const r of t.rows) {
    if (!r.overhead && r.overlay === "tiny3d" && g.names.includes(r.name)) {
      count += r.count; cycles += r.cycles;
    }
  }
  console.log(g.label.padEnd(30) +
              String(count).padStart(8) +
              (count ? (cycles / count) : 0).toFixed(1).padStart(10) +
              String(Math.round(cycles / frames)).padStart(12) +
              usPerFrame(cycles).toFixed(1).padStart(10) +
              ((cycles / t.totalCycles) * 100).toFixed(1).padStart(7) + "%");
}

console.log("=========================================");

// compare against the baseline and report what got better/worse
let foundWorse = false, strBetter = "", strWorse = "", strNew = "";
for (const [name, usPF] of Object.entries(perfMap)) {
  const last = lastPerf[name];
  if (last === undefined) {
    strNew += name + " " + usPF.toFixed(4) + " us/frame\n";
    continue;
  }
  if (Math.abs(usPF - last) < TOLERANCE_US) continue;
  const perc = ((usPF / last - 1) * 100).toFixed(1);
  const line = name + " " + usPF.toFixed(4) + " us/frame (was " + last.toFixed(4) + ", " +
               (usPF > last ? "+" : "") + perc + "%)\n";
  if (usPF > last) { strWorse += line; foundWorse = true; } else strBetter += line;
}
for (const name of Object.keys(lastPerf)) {
  if (perfMap[name] === undefined) strNew += name + " GONE (was " + lastPerf[name].toFixed(4) + " us/frame)\n";
}

if (strNew) console.log("New/removed:\n" + strNew);
if (strBetter) console.log("Better:\n" + strBetter);
if (strWorse) console.log("Worse:\n" + strWorse);
if (!foundWorse) console.log("Performance OK");
console.log("=========================================");
console.log("perfMap: " + JSON.stringify(perfMap)); // paste into tests/perf/<name>.json to update

if (foundWorse) ares.exit(1);
