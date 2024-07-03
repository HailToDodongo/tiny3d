# Fast64 Support

When exporting models from blender to GLTF, any data from fast64 will be included.<br>
A lot of these (non-game-specific) values will be re-used by the tiny3d model format.<br>
Here is a full list of all settings currently supported:

## Material Settings

- Combiner
  - Color-Combiner
  - Draw Layer
- Sources
  - Primitive Color (color & flag to use it or not)
  - Environment Color (color & flag to use it or not)
  - Use Texture-Reference
  - Texture Reference
  - Texture Size 
  - Texture Image Path
  - Clamp S/T
  - Mirror S/T   
  - Mask S/T
  - Shift S/T
  - Low S/T
  - High S/T   
- Geo
  - Shade-Alpha = Fog
  - Cull Front
  - Cull Back
  - Texture UV Generate 
- Upper
  - Cycle Type (1 Cycle, 2 Cycle)
  - Texture filter
- Lower
  - Set Render Mode? 
  - Render Mode
    - Blend: None, Multiply (aka Alpha)
    - Z-Mode: Opaque, Interpenetrating, Decal
  - Blend Color (if not set, the alpha value 128 by default in case of alpha-clipping) 

### Libragon Mapping
Most Settings will behave as expected, there are some exceptions which will be mapped to the closest libdragon equivalent.<br>

"Texture-Reference" will mark the given texture as a placeholder, a real texture can be loaded at runtime in the C code.<br>
The address you can set has no meaning in t3d, it will be used as a arbitrary value you can later read from the material.<br>
For a full example making use of this, see `06_offscreen`.

The "Render Mode" will map to one of the preset blender and z-modes.
Other settings usually contained in the render mode can be set as usual with te RDPQ API on top.<br>
If no value was set in the "Lower" tab, the "Draw Layer" is used instead.
