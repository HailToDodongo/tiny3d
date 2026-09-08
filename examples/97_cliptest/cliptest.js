// RSP profile + screenshot check for 97_cliptest (run from this directory).
//
// Usage: ares-test examples/97_cliptest/cliptest.js [rom.z64] [--update] [--no-compare]
//   rom           defaults to t3d_97_cliptest.z64 next to this script
//   --update      (re-)record cliptest.png (the reference screenshot)
//   --no-compare  skip the comparison against the embedded baseline numbers
//
// Prints the per-command RSP time (all clipping work is inside "Tri Draw"),
// compares it against the baseline recorded below, and saves a screenshot to
// cliptest.actual.png which is compared against cliptest.png (sha256, exact).

const dir = ares.args.length && ares.args[0].includes("/") ? ares.args[0].replace(/\/[^/]*$/, "/") : "";
let rom = "t3d_97_cliptest.z64";
let update = false, compareBase = true;
for (const a of ares.args) {
  if (a === "--update") update = true;
  else if (a === "--no-compare") compareBase = false;
  else rom = a;
}
const refPng = dir + "cliptest.png";
const actualPng = dir + "cliptest.actual.png";
const frames = 20;

ares.setRenderer("angrylion");
ares.loadRom(rom);
ares.resume();
ares.waitFrames(20);

ares.waitRspCommand("Screen Size");
ares.rspProfileStart();
ares.waitRspCommand("Screen Size", frames);
const t = ares.rspProfile();

if (ares.log().includes("RSP CRASH") || ares.log().includes("CPU exception")) {
  throw new Error("ROM crashed:\n" + ares.log());
}

const usPerFrame = c => c / frames / 62.5; // 62.5 MHz -> us per game frame

console.log("");
console.log("RSP profile over " + frames + " frames (lostRows: " + t.lostRows + ")");
console.log("command".padEnd(24) + "count".padStart(7) + "avg cyc".padStart(10) + "us/frame".padStart(10));
const perfMap = {};
for (const r of t.rows) {
  if (r.overhead || r.overlay !== "tiny3d") continue;
  const name = r.overlay + "/" + r.name;
  perfMap[name] = usPerFrame(r.cycles);
  console.log(name.padEnd(24) + String(r.count / frames).padStart(7) +
              r.avg.toFixed(1).padStart(10) + perfMap[name].toFixed(2).padStart(10));
}

const lastPerf = {"tiny3d/Tri Draw":273.72,"tiny3d/Vert Load":28.112,"tiny3d/Matrix Stack":17.024,"tiny3d/Tri Sync":2.6392,"tiny3d/Proj Set":0.784,"tiny3d/Set Word":0.464,"tiny3d/Light Set":0.368,"tiny3d/Screen Size":0.304,"tiny3d/Draw Flags":0.208,"tiny3d/Fog State":0.128};

console.log("=========================================");
if (compareBase && lastPerf) {
  let foundWorse = false, strBetter = "", strWorse = "";
  for (const [name, usPF] of Object.entries(perfMap)) {
    const last = lastPerf[name];
    if (last === undefined) { console.log("new: " + name + " " + usPF.toFixed(4) + " us/frame"); continue; }
    if (Math.abs(usPF - last) < 0.0025) continue;
    const line = name + " " + usPF.toFixed(4) + " us/frame (was " + last.toFixed(4) + ", " +
                 ((usPF / last - 1) * 100).toFixed(1) + "%)\n";
    if (usPF > last) { strWorse += line; foundWorse = true; } else strBetter += line;
  }
  if (!foundWorse) console.log("Performance OK");
  if (strBetter) console.log("Better:\n" + strBetter);
  if (strWorse) console.log("Worse:\n" + strWorse);
  console.log("=========================================");
}
//console.log("perfMap: " + JSON.stringify(perfMap));

// ---- screenshot ----
const shot = ares.screenshot().crop(0, 1, 640, 238);
if (update) {
  shot.save(refPng);
  console.log("recorded " + refPng + " sha256 " + shot.sha256);
} else {
  shot.save(actualPng);
  let ref = null;
  try { ref = ares.loadImage(refPng); } catch (e) {}
  if (!ref) {
    console.log("no reference " + refPng + " (run with --update); actual sha256 " + shot.sha256);
  } else {
    const cmp = shot.compare(ref, 0);
    if (cmp.match) console.log("screenshot: SAME (sha256 " + shot.sha256 + ")");
    else {
      cmp.diff.save(dir + "cliptest.diff.png");
      console.log("screenshot: CHANGED " + JSON.stringify({ diffPixels: cmp.diffPixels, maxDelta: cmp.maxDelta, sha256: shot.sha256 }) +
                  " (see cliptest.actual.png / cliptest.diff.png)");
    }
  }
}
