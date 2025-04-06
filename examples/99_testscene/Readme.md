## Tiny3D - Testscene

This demo is mostly intended to test the performance of the engine and features.<br>
It's also used to compare it against OpenGL & F3DEX3.
Keep in mind to enable performance-timers in libdragon in order to get numbers.

OpenGL version: https://github.com/HailToDodongo/gl_test_scene

### Enable Performance-timers

To see any numbers, you have to compile libdragon with them enabled, and re-compile tiny3d afterwards.<br/>
There are a few manual changes necessary to make this work:

#### Libdragon

In `include/rspq_constants.h` set the define `RSPQ_PROFILE` to `1`, and `RSPQ_DEBUG` to `0`.

Then rebuild libdragon.

#### Tiny3D

The ucode will no longer fit into DMEM, so a few features are automatically disabled.<br>
This includes: reduced lights from 7 to 2, any vertex-effect (e.g. uvgen) turns into UB.

Note that even if you don't use profiling in your code, running with a libdragon version that has it enabled will reduce RSP performance.
