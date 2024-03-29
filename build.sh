set -e

if [ $1 = "clean" ] ; then
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
  make -C examples/99_testscene clean
fi

# Build Tiny3D
echo "Building Tiny3D..."
make -j4

# Tools
echo "Building tools..."
make -C tools/gltf_importer -j4

# Build Examples
echo "Building examples..."
make -C examples/00_quad -j4
make -C examples/01_model -j4
make -C examples/02_lighting -j4
make -C examples/03_objects -j4
make -C examples/04_dynamic -j4
make -C examples/05_splitscreen -j4
make -C examples/06_offscreen -j4
make -C examples/99_testscene -j4

echo "Build done!"