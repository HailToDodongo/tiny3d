# QWEN.md - Context for Tiny3D and Last64_Bloom

This file provides an overview of the Tiny3D project and the user's personal project, Last64_Bloom, for use as context in future interactions, particularly with AI assistants.

## Tiny3D Framework Overview

Tiny3D is a 3D graphics library and custom RSP (Reality Signal Processor) microcode for the Nintendo 64 (N64), built on top of the [libdragon](https://github.com/DragonMinded/libdragon/tree/preview) development framework.

The main goal is to provide a fast and efficient 3D API for N64 homebrew development, offering both low-level control over the RSP and a higher-level, easier-to-use C API. It includes features like a full 3D pipeline, lighting (ambient, directional, point), matrix operations, culling, skinned mesh animation, and a GLTF model importer with Fast64 support for Blender.

### Last64_Bloom (User Project)

Last64_Bloom is the personal project we want to built using the Tiny3D framework. It is a minimalist reimagining of *Vampire Survivors* for the N64, featuring an abstract visual style inspired by games like *REZ* and *Geometry Wars*. The project heavily utilizes advanced visual effects, particularly HDR and Bloom, to create its aesthetic, relying on simple geometric shapes (triangles, circles, quads) instead of traditional textures or sprites.

## Technology Stack

*   **Platform:** Nintendo 64 (N64)
*   **Framework:** Libdragon (preview branch), Tiny3D
*   **Language (Library/Project):** C++
*   **Microcode Language:** RSPL (RSP Language), transpiled to RSP assembly (for Tiny3D core and custom FX ucode).
*   **Build System:** Makefiles
*   **Model Format:** None (abstract shapes); Tiny3D's GLTF support is not used.

## Project Structure

*   `src/`: Contains the main C source code for the Tiny3D library (`t3d.c`, `t3dmath.c`, etc.) and the RSP microcode source (`rsp/rsp_tiny3d.rspl`).
*   `build/`: Output directory for compiled library files (e.g., `libt3d.a`).
*   `examples/`: Contains numerous example projects demonstrating various Tiny3D features. These are standalone N64 ROMs.
*   `tools/`: Contains tools related to Tiny3D, primarily the `gltf_importer` (written in C++) used to convert GLTF files into the Tiny3D `.t3d` model format.
*   `docs/`: Documentation files, including details on GLTF/Fast64 settings and model formats.
*   `last64_bloom/`: The user's personal project directory.
    *   `last64_bloom/src/`: Source code for Last64_Bloom, including custom post-processing (HDR/Bloom) logic and game scenes.
    *   `last64_bloom/assets/`: Likely intended for assets (though the project uses abstract shapes).
    *   `last64_bloom/build/`: Build output directory for the Last64_Bloom ROM.
*   `Makefile`, `build.sh`: Root build scripts for Tiny3D.
*   `t3d.mk`: A Makefile include used by projects wanting to integrate Tiny3D.

## Building and Running

### Prerequisites

*   A working [libdragon](https://github.com/DragonMinded/libdragon/tree/preview) setup.
*   Standard build tools (GCC, Make).

### Building Tiny3D

**We dont need to build the library as it is already build.**

The main way to build the project is using the `build.sh` script in the root directory. This script:

1.  Cleans previous builds.
2.  Builds the core Tiny3D static library (`libt3d.a`) using the root `Makefile`.
3.  Builds and installs the `gltf_importer` tool (`tools/gltf_importer`).
4.  Builds all the example projects in the `examples/` directory.

Commands:
```sh
# Standard build
./build.sh

# If using libdragon via Docker
libdragon exec ./build.sh
```

Individual components can also be built using Makefiles directly:
```sh
# Build only the library
make

# Build a specific example
make -C examples/00_quad
```

### Building Last64_Bloom

The user's project, Last64_Bloom, is built like a standard libdragon/Tiny3D project. It has its own `Makefile` within the `last64_bloom/` directory. To build it:

```sh
# Navigate to the project directory
cd last64_bloom

# Build the project
make

**IMPORTANT REMINDER: DO NOT BUILD THE PROJECT YOURSELF! The user builds it separately.**
**DO NOT OFFER TO BUILD OR RUN MAKE COMMANDS!**


# The output ROM will be in last64_bloom/build/
```

This project integrates Tiny3D by including `t3d.mk` in its Makefile, similar to the examples.

### Using Tiny3D in Your Project

There are two main ways to integrate Tiny3D into a libdragon project:

1.  **System-wide Install (Recommended for Tiny3D examples):**
    *   Build Tiny3D (`./build.sh`).
    *   Run `make install` (might require `sudo`) to install the library and headers.
    *   Add `include $(N64_INST)/include/t3d.mk` to your project's `Makefile`.

2.  **Local Copy (How Last64_Bloom likely integrates):**
    *   Copy the Tiny3D source tree into your project (e.g., as a submodule) or ensure the paths in your `t3d.mk` include point to the Tiny3D source.
    *   Add `include path/to/t3d.mk` to your project's `Makefile`.

The `t3d.mk` include file handles setting up the necessary compiler flags (include paths) and linker flags (library path to `libt3d.a`).

### Running Examples/Last64_Bloom

After building, the compiled N64 ROMs (`.z64` files) for the examples will be located in their respective `examples/*` subdirectories.

The compiled ROM for Last64_Bloom will be in the `last64_bloom/build/` directory. These can be run on an N64 emulator or flashcart.

## Key Features and Concepts

*   **RSP Microcode:** The core 3D rendering is performed by custom RSP microcode written in RSPL. The C API communicates with this ucode via `libdragon`'s RSPQ (RSP Command Queue) API.
*   **Vertex Format:** Uses a fixed, interleaved vertex format (`T3DVertPacked`) for performance. It packs two vertices together.
*   **Matrix Stack:** Provides a matrix stack for hierarchical transformations.
*   **DMA-based Data Transfer:** Matrices and vertex data are DMA'd to the RSP for rendering. They must remain in RDRAM while in use.
*   **No Abstract Materials:** Unlike OpenGL, Tiny3D does not have an abstract material system. Rendering states like combiner settings are typically controlled directly via the RDPQ API or loaded from the model format.
*   **GLTF Importer:** A key tool (`tools/gltf_importer`) converts GLTF files (preferably exported from Blender 4.0 using Fast64) into Tiny3D's optimized binary format. This tool handles vertex cache optimization, triangle stripping, and translating Fast64 material properties. *(Not used by Last64_Bloom)*
*   **Animation:** Supports skeletal animation with features like animation blending and streaming from ROM. *(Potentially used by Last64_Bloom for abstract effects?)*
*   **Culling:** Includes functionality for culling, potentially using BVH (Bounding Volume Hierarchy) trees.
*   **Custom Post-Processing (HDR/Bloom):** Last64_Bloom implements custom HDR and Bloom effects using additional RSP microcode (`rspFX`). This involves rendering to an HDR buffer (RGBA32), downscaling, blurring, and compositing the bloom back onto the final LDR (RGBA16) framebuffer.

## Development Conventions

*   **Language Standard:** C code generally uses `-std=gnu2x` (a recent GNU C standard). The user's project (Last64_Bloom) uses C++.
*   **Optimization:** Compilation often uses `-Os` for size optimization, suitable for N64 constraints.
*   **Warnings:** Strict compiler warnings are enabled (`-Wall`, `-Wextra`, `-Werror`).
*   **Makefiles:** Uses standard GNU Make with pattern rules for compilation. Dependencies are tracked (`.d` files).
*   **Includes:** Public headers are intended to be included via `<t3d/header.h>`.
*   **Examples:** Serve as the primary documentation and starting points for users. Last64_Bloom is developed as a parallel project, likely using examples as a base.

## Documentation

*   **README.md:** The main project documentation, covering features, usage, build instructions, and differences from OpenGL.
*   **Examples:** The `examples/` directory is crucial for understanding practical usage.
*   **Source Code Comments:** Inline comments and Doxygen-style comments exist in the source files (e.g., `src/t3d/t3d.h`).
*   **Docs Directory:** Files like `docs/fast64Settings.md` and `docs/modelFormat.md` provide specific details.
*   **Last64_Bloom Specific:**
    *   `last64_bloom/DESIGN.md`: Outlines the visual style, core concepts, and development milestones for the Last64_Bloom project.
    *   `last64_bloom/Readme.md`: Details the implementation of the custom HDR and Bloom post-processing effects used in the project.

*(This file was generated based on an analysis of the project structure and key files on Wednesday, 13 August 2025.)*