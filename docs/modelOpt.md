# Model Optimizations

This document describes the process of building and optimizing meshes in the builtin model format.<br>
For the most part this includes handling vertex and triangle/index data at builtime.

## Why?
When converting a model from a generic format like GLTF, the input data can't be expected
to be optimized.<br>
In the case of the N64 we also have some additional constraints due to the hardware.<br>
The biggest one is the 4KB limit for memory we have on the RSP (DMEM).<br>
What's worse it that only parts of it are usable, since common data like the command processing also needs to fit in there.<br>

The general design of t3d's ucode (like others) is to have explicit commands to load vertices into a cache,
which will also perform T&L on them.<br> 
After that, triangles can be drawn referencing loaded vertices by the local index in the cache (0-69).<br>
While this means we may calculate vertices that are not used in the end (e.g. culled), it still seems to be the most efficient way.

Here is the current layout with the vertex buffer highlighted, each small block is one byte:
![image info](img/dmem_vert_buff.png)

Currently, t3d has a cache-size of 70, so we have to somehow split out triangle draws to only reference any of the loaded 70 vertices.
On top, we should optimize these splits to use as little commands and memory as possible.<br>
While both are interlinked, we take a look of the splitting part first now.

So we have to build up an array of structs that define what vertices to load, and then which triangles to draw after.<br>
This also means the loads/draws are fully known and prepared at builtime.<br>

(For more details on how the data is stored check out the file-format: [modelFormat.md](modelFormat.md))

## Splitting
A naive solution would be to just iterate over the original index-buffer,
check which vertices need to be loaded, and then draw triangles referencing them.<br>
If you go above 70 vertices, you overwrite older vertices.<br>

This approach does have a big issue though: Vertices are effectively random form the index buffers view,<br>
so we would need a lot of smaller, vertex loads over time.<br>
Worse yet, we may keep reloading the same vertices over again that where just overwritten by something else.<br>
What we ideally want is that each new triangle can re-use as much of the already loaded vertices.<br>
While at the same time having as little and therefore bigger loads as possible.

### Sorting
As a first step, we sort indices in such a way that more used vertices (aka more interconnected triangles) are first.<br>
For this t3d uses the library 'meshoptimizer', specifically the `meshopt_optimizeVertexCache` function.<br>

After that is done we normalize the vertex/index buffer into an array of triangles.<br>
Meaning an array of structs that directly contains 3 vertices each, no indices.<br>
This makes it easier to now generate individual parts of buffers to load.

### Partitioning
A 'Part' is one vertex load commands + all triangles that can be drawn with it.<br>
To fill it, we "emit" a triangle into it by writing out it's vertices (if not already there), and then take the index from that buffer as our 3 indices for the triangle.<br>
Note that the vertex buffer is local to the part, so it can contain only 70 vertices.<br>

We start this by emitting the first triangle, which will write 3 new vertices into the empty buffer.<br>
Each time a triangle is emitted, we check if there are any other triangles that could be emitted too without needing new vertices.<br>
As the buffer slowly fills, this becomes more and more likely.<br>
After that, check if there are any that require only 1, and then 2 new vertices.<br>
If none can be emitted, we move on to the next triangles in the list, which will once again emit 3 new vertices.<br>
This process is then repeated until we hit out limit of 70, after which a new part is started.<br>

If no triangles are left, we are done and now left with an array of parts.<br>

**@TODO: IMAGE**

This state would now finally allow us to draw the model at runtime, but we can do better.

### De-Fragmentation
You may notice that a triangle could require a vertex from an earlier part.<br>
This would require an additional vertex load, as the vertex was already emitted, but is not inside our part.<br>
Over time this leads to fragmentation of vertices from the triangles point of view.<br>

The reason we don't want this is that each RSP command has an inherent cost to it.<br>
On top, the vertex load command has a fixed cost too for e.g. setting up registers and the DMA.<br>

To avoid this, we can simply emit the vertex again in our current buffer.<br>
This also guarantees that every part will only ever need one vertex-load command no matter what.<br>
The tradeoff is that it leads to duplicated vertices in the final mode file.<br>

Taking the model from `examples/99_testscene`, which has 3642 vertices, it will add only 54 duped vertices.<br>
Due to the previous sorting / part-logic, it thankfully seems to keep those low.

It's important to note that this will only increase file-size, the RSP will still load & process the same amount as before.<br>
In fact, memory bandwidth will go slightly down due to needing fewer commands.

For Vertices we are now done!

## Indices
Now we need to tell the RSP how to draw triangles.<br>
For that, we can look at each part from the previous step in isolation.<br>
A part will already contain a list of indices local to the cache, so at runtime we could just iterate over it and emit `t3d_tri_draw` commands per triangle.

This would need one command per triangle, which as mentioned before, drives up runtime.<br>
One detail about RSP commands is that they need 1 byte to encode the overlay/command-id.<br>
Since `t3d_tri_draw` is 8 bytes in size, 1/8 of the memory-bandwidth would be wasted on that.<br>

### Index Buffer

One option is to draw multiple triangles with one command.<br>
Some ucodes have a command for that, in t3d i decided against it and instead go with a proper buffer.<br>
This means a single command will DMA a buffer with arbitrary size, and then process it in it's own compact loop to draw triangles.<br>

The major issue here is DMEM though, t3d is already sitting at 100% usage, so it can't afford any special buffer for it.<br>
However we can re-use parts of the vertex cache for it.<br>

As you draw more and more triangles of a part, more and more vertices will become unused.<br>

@TODO: image

Due to the previous steps, those tend to pool up at the end of the buffer.<br>
Since we have knowledge of all triangles, we can determine for every given triangle how much vertices are unused by checking if any following triangle will use it.<br>
If there is space available, we can use that as a target to DMA our index buffer to.<br>
The benefit of doing it that way is that we don't have to sacrifice our vertex cache size, and instead can dynamically take up free space.<br>


### Triangle-Strip

@TODO: text

## Materials

Objects in a model file are sorted by draw-layer and then materials.<br>
Besides that, nothing can be done that couldn't be done better at runtime.

But to summarize what happens at runtime:<br>
Materials don't store the commands to emit, but rather the desired state of a material.<br>
So this data is evaluated and turned into RDP commands, optionally being recorded into a DPL.<br>
Due to that, t3d offers a C struct that can be passed into the model draw function.<br>
This will keep track of the state to minimize commands even across materials or models.<br>
By default this is always done automatically for you.


## Edge-Cases
There are few special things to handle:

- Skinned meshes require multiple vertex loads, since between each of them they load a different matrix for the bone.<br>
The logic still works as before, as the part is simply further split into multiple ones per bone.<br>

- t3d's vertex struct contains 2 vertices at once interleaved. This is done to optimize loads on the RSP.<br>
This needs special handling since it can only have an even amount of vertices in the buffer.
For odd amounts it will either put garbage or a random vertex in the last once.

- The model format allows only for 4 index-buffers per part, which is enough in all my tests.
In the case it's not enough, the rest is emitted as regular `t3d_tri_draw` calls.