# HDR & Bloom

This demo shows a basic HDR and screen-space bloom effect.

## How it works

### HDR
The HDR works by artificially limiting color depth to gain range.<br>
Usually games render into RGBA16 which already has only 5 bits of color depth per channel.<br>
By rendering into RGBA32 instead, we get 8 bits.<br>
The scene is then rendered darker than usual, so that in the end full white would only be 1/8th of the brightness.<br>
This means the final color in the buffer only uses 5bits, exactly the same as RGBA16.<br>
Everything beyond that is the HDR part as it exceeds what RGBA16 could display.<br>
Note that the final buffer displayed by VI here is still RGBA16<br>

A custom ucode is then responsible for transferring the color data over into the final RGBA16 buffer,
which also gives it the chance to scale the colors up and down as needed.<br>
This allows for controlling the exposure of the scene.

### Bloom

Bloom works by creating a downsampled version of the framebuffer (here 1/4th of the size).<br>
Which is then blurred in one or more passes.<br>

The final result is passed into the existing HDR ucode, which will add it ontop of the HDR color.<br>

Downscaling can be done via RSP or RDP, blurring is RSP only and can also apply a threshold to ignore darker colors.<br>
The amount of bloom added in the last step can also be controlled.

## Integration

### Build
Most of this example is concerned with the demo scenes, to integrate it into existing projects only a few files are needed.<br>
Specifically:
- `src/postProcess.h`
- `src/postProcess.cpp`
- `src/rsp/*`

In addition, the makefile needs to compile the custom ucode, which is handled by this line:

```makefile
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.cpp=$(BUILD_DIR)/%.o) $(BUILD_DIR)/src/rsp/rsp_fx.o
```

### Runtime

At runtime, the HDR/Bloom code injects itself by changing the framebuffer you render to.<br>
Besides considerations about the brightness of a scene, the rendering code can mostly stay the same.<br> 

The framebuffer *must* be RGBA16 at 240x320:
```cpp
constexpr int BUFF_COUNT = 3;
display_init(RESOLUTION_320x240, DEPTH_16_BPP, BUFF_COUNT, ..., ...);
```
After which you also create the post-processing object:
```cpp
PostProcess postProc[BUFF_COUNT]{};
```

At the beginning of each frame, attach it:
```cpp
rdpq_attach(fb, display_get_zbuf());

postProc[frameIdx].setConf(state.ppConf); // update config if needed
postProc[frameIdx].beginFrame(); // switch over to the RGBA32 buffer
```
Then render as usual, this can be 3D or 2D rendering.<br>
Once you are done, perform the post-processing into the actual framebuffer:
```cpp
postProc[frameIdx].endFrame();
postProc[frameIdxLast].applyEffects(*fb);

// attach the RDP back to the actual RGBA16 buffer, now you can draw
// things that should be unaffected by the post-processing
rdpq_sync_pipe();
rdpq_set_color_image(fb);
```

If you need to know the average brightness of the image (e.g. for auto-exposure), you can call:
```cpp
postProc[frameIdx].getBrightness()
```
Which gives you a float from 0.0 to 1.0.

Once thing to consider is syncing.<br>
Since the RSP needs to access data that the RDP is rendering, it may overstep it.<br>
This demo chooses to triple buffer the post-processing.<br>
Meaning it renders into buffer `x`, but cals `applyEffects` on buffer `x-2`.

Alternatively, you can also let the RSP wait for the RDP to avoid this, albeit at cost of performance.<br>