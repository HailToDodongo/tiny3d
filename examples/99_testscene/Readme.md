## Tiny3D - Testscene

This demo is mostly intended to test the performance of the engine and features.<br>
It's also used to compare it against OpenGL & F3DEX3.
Keep in mind to enable performance-timers in libdragon in order to get numbers.

OpenGL version: https://github.com/HailToDodongo/gl_test_scene

### Enable Performance-timers

To see any numbers, you have to compile libdragon with them enabled, and re-compile tiny3d afterwards.<br/>
There are a few manual changes necessary to make this work:

#### Libdragon

In `include/rspq_constants.h` set the define `RSPQ_PROFILE` to `1`.

It's likely that the OpenGL ucode no longer fits, delete a few lines there.
For example the lines here: https://github.com/DragonMinded/libdragon/blob/208f4eadf73b323efab4b98f7df09c6f2a950db8/src/GL/rsp_gl_pipeline.S#L556-L574

Then rebuild libdragon.

#### Tiny3D

The ucode here will no longer fit into DMEM, you can reduce the amount of point-lights from 8 to 4.<br/>
For that, change the `LIGHT_DIR_COLOR: .ds.b 128` into `LIGHT_DIR_COLOR: .ds.b 64`,<br/>
in both `src/t3d/rsp/rsp_tiny3d_clipping.S` and `src/t3d/rsp/rsp_tiny3d.S`.

After that make a clean build via `./build.sh clean` in the project root.

Note that even if you don't use profiling in your code, running with a libdragon version that has it enabled will reduce RSP performance.