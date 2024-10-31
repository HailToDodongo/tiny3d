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

  inline const char* getTypeString(cgltf_type type)
  {
    switch(type) {
      case cgltf_type_scalar: return "Scalar";
      case cgltf_type_vec2: return "Vec2";
      case cgltf_type_vec3: return "Vec3";
      case cgltf_type_vec4: return "Vec4";
      case cgltf_type_mat2: return "Mat2";
      case cgltf_type_mat3: return "Mat3";
      case cgltf_type_mat4: return "Mat4";
      default: return "<?>";
    }
  }

  inline const char* getAnimTargetString(cgltf_animation_path_type type)
  {
    switch(type) {
      case cgltf_animation_path_type_translation: return "Translation";
      case cgltf_animation_path_type_rotation: return "Rotation";
      case cgltf_animation_path_type_scale: return "Scale";
      case cgltf_animation_path_type_weights: return "Weights";
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

  inline Vec3 readAsVec3(const uint8_t* data, cgltf_type type, cgltf_component_type compType) {
    Vec3 result{};
    switch(type) {
      case cgltf_type_scalar: {
        result[0] = readAsFloat(data, compType);
        result[1] = result[0];
        result[2] = result[0];
        break;
      }
      case cgltf_type_vec2: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = 0.0f;
        break;
      }
      case cgltf_type_vec4:
      case cgltf_type_vec3: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = readAsFloat(data + getDataSize(compType) * 2, compType);
        break;
      }
      default:
        printf("Unsupported type: %s (%d)\n", getTypeString(type), type);
        throw std::runtime_error("Unsupported type");
    }
    return result;
  }

  inline Vec4 readAsColor(uint8_t* &data, cgltf_type type, cgltf_component_type compType) {
    Vec4 result{};
    switch(type) {
      case cgltf_type_vec3: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = readAsFloat(data + getDataSize(compType) * 2, compType);
        result[3] = 1.0f;
        data += getDataSize(compType) * 3;
        break;
      }
      case cgltf_type_vec4: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = readAsFloat(data + getDataSize(compType) * 2, compType);
        result[3] = readAsFloat(data + getDataSize(compType) * 3, compType);
        data += getDataSize(compType) * 4;
        break;
      }
      default:
        printf("Unsupported type: %s (%d)\n", getTypeString(type), type);
        throw std::runtime_error("Unsupported type");
    }
    return result;
  }

  inline Vec4 readAsVec4(const uint8_t* data, cgltf_type type, cgltf_component_type compType) {
    Vec4 result{};
    switch(type) {
      case cgltf_type_scalar: {
        result[0] = readAsFloat(data, compType);
        result[1] = 0.0f;
        result[2] = 0.0f;
        result[3] = 0.0f;
        break;
      }
      case cgltf_type_vec2: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = 0.0f;
        result[3] = 0.0f;
        break;
      }
      case cgltf_type_vec3: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = readAsFloat(data + getDataSize(compType) * 2, compType);
        result[3] = 0.0f;
        break;
      }
      case cgltf_type_vec4: {
        result[0] = readAsFloat(data, compType);
        result[1] = readAsFloat(data + getDataSize(compType), compType);
        result[2] = readAsFloat(data + getDataSize(compType) * 2, compType);
        result[3] = readAsFloat(data + getDataSize(compType) * 3, compType);
        break;
      }
      default:
        printf("Unsupported type: %s (%d)\n", getTypeString(type), type);
        throw std::runtime_error("Unsupported type");
    }
    return result;
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

  inline const char *getInterpolationName(cgltf_interpolation_type type) {
    switch(type) {
      case cgltf_interpolation_type_linear: return "Linear";
      case cgltf_interpolation_type_step: return "Step";
      case cgltf_interpolation_type_cubic_spline: return "CubicSpline";
      default: return "<?>";
    }
  }
}