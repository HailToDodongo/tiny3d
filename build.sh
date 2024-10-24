#!/usr/bin/env bash

set -e

if [ "$1" = "clean" ] ; then
  echo "Cleaning up..."
  make clean
  make -C tools/gltf_importer clean
  make -C examples/00_quad clean
  make -C examples/01_model clean
  make -C examples/02_lighting clean
  make -C examples/03_objects clean
  make -C examples/04_dynamic clean
  make -C examples/05_splitscreen clean
  make -C examples/06_offscreen clean
  make -C examples/07_skeleton clean
  make -C examples/08_animation clean
  make -C examples/09_anim_viewer clean
  make -C examples/10_flipbook_tex clean
  make -C examples/11_segments clean
  make -C examples/12_uv_gen clean
  make -C examples/13_cel_shading clean
  make -C examples/14_outline clean
  make -C examples/15_pointlight clean
  make -C examples/16_light_clip clean
  make -C examples/17_culling clean
  make -C examples/99_testscene clean
fi

# Build Tiny3D
echo "Building Tiny3D..."
make -j4
make install || sudo make install

# Tools
echo "Building tools..."
make -C tools/gltf_importer -j4
make -C tools/gltf_importer install || sudo make -C tools/gltf_importer install

# Build Examples
echo "Building examples..."
make -C examples/00_quad -j4
make -C examples/01_model -j4
make -C examples/02_lighting -j4
make -C examples/03_objects -j4
make -C examples/04_dynamic -j4
make -C examples/05_splitscreen -j4
make -C examples/06_offscreen -j4
make -C examples/07_skeleton -j4
make -C examples/08_animation -j4
make -C examples/09_anim_viewer -j4
make -C examples/10_flipbook_tex -j4
make -C examples/11_segments -j4
make -C examples/12_uv_gen -j4
make -C examples/13_cel_shading -j4
make -C examples/14_outline -j4
make -C examples/15_pointlight -j4
make -C examples/16_light_clip -j4
make -C examples/17_culling -j4
make -C examples/99_testscene -j4

echo "Build done!"