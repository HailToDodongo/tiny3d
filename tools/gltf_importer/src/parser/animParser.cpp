/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"
#include <cassert>

namespace
{
  AnimChannelTarget getTarget(cgltf_animation_path_type type) {
    switch(type) {
      case cgltf_animation_path_type_translation: return AnimChannelTarget::TRANSLATION;
      case cgltf_animation_path_type_rotation   : return AnimChannelTarget::ROTATION;
      case cgltf_animation_path_type_scale      : return AnimChannelTarget::SCALE;
      default:
        printf("Unknown animation target: %d\n", type);
        throw std::runtime_error("Unknown animation target");
      break;
    }
  }

  constexpr float MIN_VALUE_DELTA = 0.0001f;

  Vec3 cleanupVector(Vec3 v) {
    return Vec3{
      fabsf(v[0]) < MIN_VALUE_DELTA ? 0.0f : v[0],
      fabsf(v[1]) < MIN_VALUE_DELTA ? 0.0f : v[1],
      fabsf(v[2]) < MIN_VALUE_DELTA ? 0.0f : v[2]
    };
  }

  void insertScalarKeyframe(Anim &anim, float time, uint32_t chIdx, Vec3 value, bool isTranslate) {
    value = cleanupVector(value);
    if(isTranslate)value *= config.globalScale;

    for(int i=0; i<3; ++i) {
      anim.channelMap[chIdx + i].keyframes.push_back({.time = time, .valScalar = value[i]});
      anim.channelMap[chIdx + i].valueMin = std::min(anim.channelMap[chIdx + i].valueMin, value[i]);
      anim.channelMap[chIdx + i].valueMax = std::max(anim.channelMap[chIdx + i].valueMax, value[i]);
    }
  }
}

Anim parseAnimation(const cgltf_animation &anim, const std::unordered_map<std::string, const Bone*> &nodeMap, uint32_t sampleRate)
{
  Anim res{
    .name = std::string(anim.name),
    .keyframes = {},
  };

  float timeStart = 0.0f;
  float timeEnd = 0.0f;

  // get indices sorted by rotation/not-rotation
  std::vector<uint32_t> channelIndices;
  for(uint32_t c=0; c < anim.channels_count; ++c) {
    auto &channel = anim.channels[c];
    if(channel.target_path == cgltf_animation_path_type::cgltf_animation_path_type_rotation) {
      channelIndices.insert(channelIndices.begin(), c);
    } else {
      channelIndices.push_back(c);
    }
  }

  uint32_t chIdx = 0;
  res.duration = 0;

  for(uint32_t c : channelIndices)
  {
    auto &channel = anim.channels[c];
    const auto &samplerIn = *channel.sampler->input;
    const auto &samplerOut = *channel.sampler->output;
    const char* targetName = channel.target_node->name;

    auto it = nodeMap.find(targetName);
    if(it == nodeMap.end()) {
      printf("Channel target not found: %s, skipping channel...\n", targetName);
      continue;
    }

    bool isRot = channel.target_path == cgltf_animation_path_type_rotation;
    bool isTranslate = channel.target_path == cgltf_animation_path_type_translation;

    // create channels, rotation is always combined, the rest (translation, scale) are separate for each axis
    res.channelMap.push_back({
      .targetName = targetName, .targetType = getTarget(channel.target_path),
      .attributeIdx = 0,
    });

    if(!isRot) {
      res.channelMap.push_back({.targetName = targetName, .targetType = getTarget(channel.target_path), .attributeIdx = 1});
      res.channelMap.push_back({.targetName = targetName, .targetType = getTarget(channel.target_path), .attributeIdx = 2});
    }

    uint8_t *dataInput = ((uint8_t*)samplerIn.buffer_view->buffer->data) + samplerIn.offset + samplerIn.buffer_view->offset;
    uint8_t *dataOutputStart = ((uint8_t*)samplerOut.buffer_view->buffer->data) + samplerOut.offset + samplerOut.buffer_view->offset;
    uint8_t *dataOutput = dataOutputStart;
    uint8_t *dataOutputNext = dataOutput + samplerOut.stride;
    uint8_t *dataOutputEnd = dataOutput + samplerOut.count * samplerOut.stride;

    timeStart = Gltf::readAsFloat(dataInput, samplerIn.component_type);
    timeEnd = Gltf::readAsFloat(dataInput + (samplerIn.count-1) * samplerIn.stride, samplerIn.component_type);
    float sampleStep = 1.0f / sampleRate;
    float time = timeStart;
    float nextTime = timeStart;
/*
    printf("  - Channel %d\n", c);
    printf("    - Target: %s -> %s\n", targetName, Gltf::getAnimTargetString(channel.target_path));
    printf("    - Input[%s]: %d @ %d (%s)\n", Gltf::getTypeString(channel.sampler->input->type), samplerIn.count, samplerIn.stride, Gltf::getInterpolationName(channel.sampler->interpolation));
    printf("    - Output[%s]: %d @ %d\n", Gltf::getTypeString(samplerOut.type), samplerOut.count, samplerOut.stride, samplerOut.type);
    printf("    - Time Range: %.4fs -> %.4fs\n", timeStart, timeEnd);
*/
    uint32_t frame = 0;
    float t;
    for(t=timeStart; t<=(timeEnd+sampleStep); t += sampleStep)
    {
      while(dataOutput < dataOutputEnd) {
        time = Gltf::readAsFloat(dataInput, samplerIn.component_type);
        nextTime = Gltf::readAsFloat(dataInput + samplerIn.stride, samplerIn.component_type);
        if(nextTime > t)break;
        dataInput += samplerIn.stride;
        dataOutput += samplerOut.stride;
        dataOutputNext += samplerOut.stride;

        if(dataOutputNext >= dataOutputEnd)dataOutputNext = dataOutputEnd - samplerOut.stride;
      }

      if(dataOutput >= dataOutputEnd) {
        t -= sampleStep;
        break;
      }

      float sampleInterpol = (t - time) / (nextTime - time);
      //printf("Time: %.4f (%.4f - %.4f) -> %.4f\n", t, time, nextTime, sampleInterpol);
      assert(sampleInterpol >= 0.0f && sampleInterpol <= 1.0f);

      if(channel.target_path == cgltf_animation_path_type_rotation) {
        Quat valueCurr = Gltf::readAsVec4(dataOutput, samplerOut.type, samplerOut.component_type);
        //printf("    - %.4fs/%.4f [f:%d]: %.4f %.4f %.4f %.4f\n", t, timeEnd, frame, valueCurr[0], valueCurr[1], valueCurr[2], valueCurr[3]);
        Quat valueNext = Gltf::readAsVec4(dataOutputNext, samplerOut.type, samplerOut.component_type);
        Quat value = valueCurr.slerp(valueNext, sampleInterpol);

        if(value.isInvalid()) {
          throw std::runtime_error("Invalid Quaternion in anim-parser: " + value.toString());
        }

        res.channelMap.back().keyframes.push_back({.time = t, .valQuat = value});
      } else {
        Vec3 value = Gltf::readAsVec3(dataOutput, samplerOut.type, samplerOut.component_type);
        Vec3 valueNext = Gltf::readAsVec3(dataOutputNext, samplerOut.type, samplerOut.component_type);
        value = value.mix(valueNext, sampleInterpol);
        // printf("    - %.4fs/%.4f [f:%d]: b:%.4f i=%d: %.4f %.4f %.4f\n", t, timeEnd, frame, sampleInterpol, samplerIn.count, value[0], value[1], value[2]);
        insertScalarKeyframe(res, t, chIdx, value, isTranslate);
      }
      ++frame;
    }

    if((timeEnd - t) > 0.0001f) {
      // insert last keyframe in the data
      if(channel.target_path == cgltf_animation_path_type_rotation) {
        Quat value = Gltf::readAsVec4(dataOutputEnd - samplerOut.stride, samplerOut.type, samplerOut.component_type);
        res.channelMap.back().keyframes.push_back({.time = timeEnd, .chanelIdx = chIdx, .valQuat = value});
      } else {
        Vec3 value = Gltf::readAsVec3(dataOutputEnd - samplerOut.stride, samplerOut.type, samplerOut.component_type);
        insertScalarKeyframe(res, timeEnd, chIdx, value, isTranslate);
      }
    }

    chIdx += isRot ? 1 : 3;
    res.duration = std::max(timeEnd - timeStart, res.duration);
  }

  return res;
}