# Tiny3D - 3D Model (T3DM)

All data is stored in binary in a chunk-based file.<br>
It starts with a header containing the number of chunks and their offsets followed by the data.<br>

### Streaming-Data
A `.t3dm` file can be accompanied by one or more streaming-data files (`.t3ds`).<br>
This file can be compressed and contains data to be streamed in during runtime (e.g. animations).<br> 

## Header

| Offset | Type            | Description                    |
|--------|-----------------|--------------------------------|
| 0x00   | `char[4]`       | Magic (`T3DM`)                 |
| 0x04   | `u32`           | Chunk count                    |
| 0x08   | `u16`           | Total vertex count             |
| 0x0A   | `u16`           | Total index count              |
| 0x0C   | `u32`           | First Vertex-chunk index       |
| 0x10   | `u32`           | First Indices-chunk index      |
| 0x14   | `u32`           | First Material-chunk index     |
| 0x18   | `u32`           | String table offset (in bytes) |
| 0x1C   | `ChunkOffset[]` | Chunk offsets/types            |

### ChunkOffset

| Offset | Type   | Description                   |
|--------|--------|-------------------------------|
| 0x00   | `char` | Type (e.g. `M`)               |
| 0x01   | `u24`  | Offset relative to file start |

## Chunks
After the header the chunks are stored in the order of their offsets.<br>
Chunks are sorted by type and may be aligned.<br>
The first chunk type must be `O` (Object).

### Vertices (`V`)
Vertex buffer, this is a shared buffer across all model/parts.<br>
This chunk must only appear once.

| Offset | Type       | Description                 |
|--------|------------|-----------------------------|
| 0x00   | `Vertex[]` | Vertices, no size is stored |

#### Vertex
Interleaved data for two vertices.

| Offset | Type       | Description                   |
|--------|------------|-------------------------------|
| 0x00   | `int16[3]` | Position A (16.0 fixed point) |
| 0x06   | `u16`      | Normal A (5,5,5 packed)       |
| 0x08   | `int16[3]` | Position B (16.0 fixed point) |
| 0x0E   | `u16`      | Normal B (5,5,5 packed)       |
| 0x10   | `u32`      | RGBA A (RGBA8 color)          |
| 0x14   | `u32`      | RGBA B (RGBA8 color)          |
| 0x18   | `int16[2]` | UV A (pixel coords)           |
| 0x1C   | `int16[2]` | UV B (pixel coords)           |

### Indices (`I`)
Index buffer, this is a shared buffer across all model/parts.

| Offset | Type   | Description                      |
|--------|--------|----------------------------------|
| 0x00   | `u8[]` | Local Indices, no size is stored |

### Material (`M`)
Material data referenced by Objects.<br>
Directly mapped to the struct `T3DMaterial`.
Objects can have two materials assigned (with two textures),
only the first materials CC and draw flags are used.

| Offset | Type                 | Description                   |
|--------|----------------------|-------------------------------|
| 0x00   | `u64`                | Color-Combiner                |
| 0x08   | `u32`                | T3D Draw flags                |
| 0x0C   | `u8 (AlphaMode)`     | Alpha mode                    |
| 0x0D   | `u8 (FogMode)`       | Fog mode                      |
| 0x0E   | `u8[2]`              | _reserved_                    |
| 0x10   | `u32`                | Texture reference (offscreen) |
| 0x14   | `u32`                | Texture path offset           |
| 0x18   | `u32`                | Texture hash / ID             |
| 0x1C   | `u32`                | Runtime texture pointer (`0`) |
| 0x20   | `u16`                | Texture width                 |
| 0x22   | `u16`                | Texture height                |
| 0x24   | `T3DMaterialAxis[2]` | Setting per UV axis           |

#### `T3DMaterialAxis`

| Offset | Type  | Description |
|--------|-------|-------------|
| 0x00   | `f32` | Low         |
| 0x04   | `f32` | High        |
| 0x08   | `s8`  | Mask        |
| 0x09   | `s8`  | Shift       |
| 0x0A   | `u8`  | Mirror      |
| 0x0B   | `u8`  | Clamp       |

#### `AlphaMode`
```
0 - Default (no change applied)
1 - Opaque
2 - Cutout
3 - Transparent
```

#### `FogMode`
```
0 - Default (no change applied)
1 - Fog Disabled
2 - Fog Active
```

### Object (`O`)
Model data consisting of multiple parts, can exist multiple times in a file.

| Offset | Type     | Description             |
|--------|----------|-------------------------|
| 0x00   | `u32`    | Name                    |
| 0x04   | `u32`    | Part count              |
| 0x08   | `u32`    | Material A, chunk index |
| 0x0C   | `u32`    | Material B, chunk index |
| 0x10   | `Part[]` | Parts                   |

#### Part
Model part data.

| Offset | Type  | Description                     |
|--------|-------|---------------------------------|
| 0x00   | `u32` | Vertex src. offset              |
| 0x04   | `u16` | Vertex count                    |
| 0x06   | `u16` | Vertex dest. offset             |
| 0x08   | `u32` | Index offset                    |
| 0x0C   | `u16` | Index count                     |
| 0x0E   | `u16` | Matrix index, `0xFFFF` for none |

## Skeleton (`S`)
Contains a tree of bones, used for skeletal animation.<br>

| Offset | Type        | Description   |
|--------|-------------|---------------|
| 0x00   | `u16`       | Bone count    |
| 0x04   | `u16`       | _reserved_    |
| 0x08   | `T3DBone[]` | List of bones |

#### T3DBone
Bone data, each bone references its parent by index.<br>
The list is sorted by index, so index references are guaranteed to be parsed before the bone itself.

| Offset | Type     | Description           |
|--------|----------|-----------------------|
| 0x00   | `u32`    | Name                  |
| 0x04   | `u16`    | Parent index          |
| 0x06   | `u16`    | Depth / Level         |
| 0x08   | `f32[3]` | Scale                 |
| 0x14   | `f32[4]` | Rotation (Quat, XYZW) |
| 0x24   | `f32[3]` | Translation           |

## Animation (`A`)
Contains a single animation with one or more channels.<br> 
Each animation then contains a list of keyframe changing the state of a channel.<br>

| Offset | Type               | Description                           |
|--------|--------------------|---------------------------------------|
| 0x00   | `char*`            | Name, offset into string table        |
| 0x04   | `f32`              | Duration (seconds)                    |
| 0x08   | `u32`              | Keyframe count                        |
| 0x0C   | `u16`              | Quaternion Channel count              |
| 0x0E   | `u16`              | Scalar Channel count                  |
| 0x10   | `char*`            | sdata path (offset into string table) |
| 0x14   | `ChannelMapping[]` | Maps channel to targets               |

#### `ChannelMapping`
Array of channels that define the connection to the data to be modified.<br>
They are sorted so that all rotation channels come first.

| Offset | Type          | Description                                  |
|--------|---------------|----------------------------------------------|
| 0x00   | `u16`         | Target index                                 |
| 0x02   | `u8`          | Target Type                                  |
| 0x03   | `u8`          | Attribute index (0-2 for x/y/z, 0 for quat.) |
| 0x04   | `f32`         | Quantization scale                           |
| 0x08   | `f32`         | Quantization offset                          |

#### `Target Type`
```
0 = Translation
1 = Scale (XYZ)
2 = Scale (uniform)
3 = Rotation (quaternion)
```
To drive arbitrary values, `Translation` should be used as a default.

#### Data
The actual data is stored in the streaming file.<br>
It is referenced by the data-offsets in the page.<br>
<br>
To know how large the next keyframe is, the MSB is used to encode size.<br>
`0` means scalar (2 data bytes), `1` means rotation (4 data bytes).<br>
The initial KF has always 4 bytes, to have a known start.<br>

##### `Keyframe`
| Offset | Type    | Description                                          |
|--------|---------|------------------------------------------------------|
| 0x00   | `u16`   | Time till next KF in ticks, MSB sets type of next KF |
| 0x01   | `u16`   | Channel Index                                        |
| 0x02   | `u16[]` | Data, on `u16` for scalars, two `u16` for rotation   |

## String Table

At the end of the `t3dm` file, after all chunk data, a string-table is stored.<br>
This contains arbitrary strings used by the model, e.g. texture paths.<br>
Values in there are referenced by a relative offset from the start of the string table.<br>
All strings are zero-terminated.