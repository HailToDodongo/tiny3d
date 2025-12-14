/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#include <stdio.h>
#include <string>
#include <filesystem>

#include "structs.h"
#include "parser.h"
#include "args.h"

namespace T3DM
{
  thread_local Config config{};
}

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
  EnvArgs args{argc, argv};
  if(args.checkArg("--help")) {
    printf("Usage: %s <gltf-file> <t3dm-file> [--bvh] [--base-scale=64] [--ignore-materials] [--ignore-transforms] [--asset-path=assets] [--verbose]\n", argv[0]);
    printf("Params:\n");
    printf("  --bvh: Create a BVH for the model, this is used for culling and visibility checks\n");
    printf("  --base-scale=<scale>: Scale applied to blender units before conversion to integers, default is 64\n");
    printf("  --ignore-materials: Ignore F3D materials and write dummy data, useful for custom material systems\n");
    printf("  --ignore-transforms: Ignore all object transforms, can be used to force objects to be at (0,0,0)\n");
    printf("  --asset-path=<path>: Base asset path, default is 'assets/'\n");
    printf("  --verbose: Enable verbose output\n");
    return 1;
  }

  const std::string gltfPath = args.getFilenameArg(0);
  const std::string t3dmPath = args.getFilenameArg(1);

  auto &config = T3DM::config;
  config.globalScale = (float)args.getU32Arg("--base-scale", 64);
  config.ignoreMaterials = args.checkArg("--ignore-materials");
  config.ignoreTransforms = args.checkArg("--ignore-transforms");
  config.createBVH = args.checkArg("--bvh");
  config.verbose = args.checkArg("--verbose");

  config.assetPath = args.getStringArg("--asset-path");
  if(config.assetPath.empty()) {
    config.assetPath = "assets/";
  }
  if(config.assetPath.back() != '/') {
    config.assetPath.push_back('/');
  }

  config.assetPathFull = fs::absolute(config.assetPath).string();
  if(config.verbose) {
    printf("Asset path: %s (%s)\n", config.assetPath.c_str(), config.assetPathFull.c_str());
  }

  config.animSampleRate = 60;

  auto t3dm = T3DM::parseGLTF(gltfPath.c_str());
  writeT3DM(t3dm, t3dmPath);
}
