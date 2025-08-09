# 🕹️ Last64 (Abstract Style)
A minimalist reimagining of *Vampire Survivors* for the Nintendo 64 using `libdragon` and `tiny3d`. All visuals are abstract — inspired by *REZ* and *Geometry Wars* — relying on bloom and simple shapes for aesthetic effect.

## 🎨 Visual Style
- No textures or sprites
- Use triangles, circles, and quads for all entities
- Color-coded shapes with bloom blending
- Rely on additive glow and density for "intensity"
- Stylish, clean, abstract — looks good with low effort

## 🧩 Core Concepts
| System            | Description |
|-------------------|-------------|
| **Player**        | Triangle or diamond shape, glows with color |
| **Enemies**       | Triangles or circles with color per type |
| **Projectiles**   | Glowing lines or thin quads |
| **XP Orbs**       | Floating dots that glow and pulse |
| **GUI**           | Line-based HUD (health, XP, level) |
| **Multiplayer**   | (Optional) Shared-screen co-op |

## 🛠️ Development Milestones

### ✅Phase 0: Files and Compile
- [x] Create the needed files, incl. a makefile in order to compile
- [x] Render a single box

### ✅ Phase 1: Core Setup
- [ ] Init tiny3d with bloom pipeline
- [ ] Render and move player (triangle)

### ✅ Phase 2: Combat System
- [ ] Auto-fire projectiles
- [ ] Enemy spawning and AI
- [ ] Collision and damage system

### ✅ Phase 3: Progression
- [ ] XP orb drops and collection
- [ ] Level up system
- [ ] Weapon types and upgrades

### ✅ Phase 4: Polish & Features
- [ ] GUI (health, XP bars)
- [ ] Visual effects and bloom
- [ ] (Optional) Multiplayer support

## 📂 Project Structure (Suggested)
n64-project/
├── assets/
├── include/
├── src/
│ ├── main.c
│ ├── player.c / player.h
│ ├── enemy.c / enemy.h
│ ├── projectile.c / projectile.h
│ └── gui.c / gui.h
├── Makefile
└── DESIGN.md

## 📌 Notes
- Use object pools for projectiles and enemies
- Optimize with spatial partitioning if needed