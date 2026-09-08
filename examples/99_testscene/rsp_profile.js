const frames = 30;
const arg = ares.args[0];

ares.setRenderer("angrylion");

ares.loadRom("t3d_99_testscene.z64");
ares.resume();

ares.waitFrames(40);
if(arg === "clip") {
  const p1 = ares.controller(1);
  p1.hold('R');
  ares.waitFrames(1);
  p1.release('R');
}

ares.waitRspCommand("Screen Size");
ares.rspProfileStart();
ares.waitRspCommand("Screen Size", frames);

const t = ares.rspProfile();
const usPerFrame = c => c / frames / 62.5; // 62.5 MHz -> µs per game frame

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
for (const r of t.rows) 
{
  if(r.overlay !== 'tiny3d') continue;
  const name = r.overhead ? "rspq: " + r.overheadType
                          : (r.overlay ? r.overlay + "/" : "") + r.name;

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

//const shot = ares.screenshot();
//shot.save("rsp_profile.png");

console.log("=========================================");
//console.log(perfMap);
// og rdpq tri:
//const lastPerf = {"tiny3d/Tri Strip":3608.464,"tiny3d/Vert Load":2445.296,"tiny3d/Tri Seq":979.744,"tiny3d/Tri Draw":47.024,"tiny3d/Matrix Stack":17.12,"tiny3d/Proj Set":0.816,"tiny3d/Set Word":0.56,"tiny3d/Light Set":0.4341333333333333,"tiny3d/Screen Size":0.336,"tiny3d/Fog State":0.32,"tiny3d/Draw Flags":0.24};
// curr:
let lastPerf = {"tiny3d/Tri Strip":3583.76,"tiny3d/Vert Load":2445.296,"tiny3d/Tri Seq":972.656,"tiny3d/Tri Draw":45.792,"tiny3d/Matrix Stack":17.12,"tiny3d/Proj Set":0.816,"tiny3d/Set Word":0.56,"tiny3d/Light Set":0.43306666666666666,"tiny3d/Screen Size":0.336,"tiny3d/Fog State":0.32,"tiny3d/Draw Flags":0.24};
if(arg === "clip") {
  lastPerf = {"tiny3d/Vert Load":2445.296,"tiny3d/Tri Strip":2309.92,"tiny3d/Tri Seq":574.736,"tiny3d/Tri Draw":18.288,"tiny3d/Matrix Stack":17.12,"tiny3d/Proj Set":0.816,"tiny3d/Set Word":0.56,"tiny3d/Light Set":0.4341333333333333,"tiny3d/Screen Size":0.336,"tiny3d/Fog State":0.32,"tiny3d/Draw Flags":0.24};
}


// now compare and check if something got slower:
let foundWorse = false;
let strBetter = "";
let strWorse = "";
for (const [name, usPF] of Object.entries(perfMap)) {
  const last = lastPerf[name];
  const absDiff = Math.abs(usPF - last);
  if(absDiff < 0.0025) continue; // ignore small differences

  const perc = ((usPF / last - 1) * 100).toFixed(1);

  if (last === undefined) {
    console.log("new: " + name + " " + usPF.toFixed(4) + " us/frame");
  } else if (usPF > last) {
    strWorse += name + " " + usPF.toFixed(4) + " us/frame (was " + last.toFixed(4) + " +" + perc + "%)\n";
    foundWorse = true;
  } else if (usPF < last) {
    strBetter += name + " " + usPF.toFixed(4) + " us/frame (was " + last.toFixed(4) + " -" + Math.abs(perc) + "%)\n";
  }
}

if(!foundWorse)console.log("Performance OK");

console.log("=========================================");
if(strBetter) console.log("Better:\n" + strBetter);
if(strWorse) console.log("Worse:\n" + strWorse);