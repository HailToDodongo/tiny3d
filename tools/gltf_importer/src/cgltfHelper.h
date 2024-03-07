/**
 * @copyright 2023 - Max Bebök
 * @license MIT
 */
#pragma once

#include "lib/cgltf.h"

namespace Gltf
{
  inline const char* getComponentTypeString(cgltf_component_type type)
  {
    switch(type) {
      case cgltf_component_type_r_8: return "S8";
      case cgltf_component_type_r_8u: return "U8";
      case cgltf_component_type_r_16: return "S16";
      case cgltf_component_type_r_16u: return "U16";
      case cgltf_component_type_r_32u: return "U32";
      case cgltf_component_type_r_32f: return "F32";
      default: return "<?>";
    }
  }

  inline int getDataSize(cgltf_component_type type)
  {
    switch(type) {
      case cgltf_component_type_r_8: return 1;
      case cgltf_component_type_r_8u: return 1;
      case cgltf_component_type_r_16: return 2;
      case cgltf_component_type_r_16u: return 2;
      case cgltf_component_type_r_32u: return 4;
      case cgltf_component_type_r_32f: return 4;
      default: return 0;
    }
  }

  inline float readAsFloat(const uint8_t* data, cgltf_component_type type) {
    switch(type) {
      case cgltf_component_type_r_8: return (float)(*(int8_t*)data) / 127.0f;
      case cgltf_component_type_r_8u: return (float)(*(uint8_t*)data) / 255.0f;
      case cgltf_component_type_r_16: return (float)(*(int16_t*)data) / 32767.0f;
      case cgltf_component_type_r_16u: return (float)(*(uint16_t*)data) / 65535.0f;
      case cgltf_component_type_r_32u: return (float)(*(uint32_t*)data);
      case cgltf_component_type_r_32f: return (float)(*(float*)data);
      default: return 0.0f;
    }
  }

  inline uint32_t readAsU32(uint8_t* data, cgltf_component_type type) {
    switch(type) {
      case cgltf_component_type_r_8: return (uint32_t)(*(int8_t*)data);
      case cgltf_component_type_r_8u: return (uint32_t)(*(uint8_t*)data);
      case cgltf_component_type_r_16: return (uint32_t)(*(int16_t*)data);
      case cgltf_component_type_r_16u: return (uint32_t)(*(uint16_t*)data);
      case cgltf_component_type_r_32u: return (uint32_t)(*(uint32_t*)data);
      case cgltf_component_type_r_32f: return (uint32_t)(*(float*)data);
      default: return 0;
    }
  }
}