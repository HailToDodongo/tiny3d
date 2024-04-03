/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"

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
}

Anim parseAnimation(const cgltf_animation &anim, uint32_t sampleRate)
{
  AnimPage page{.sampleRate = sampleRate,};
  Anim res{.name = std::string(anim.name),};

  printf("Animation: %s\n", anim.name);
  printf(" - Channels: %d\n", anim.channels_count);
  printf(" - Samplers: %d\n", anim.samplers_count);

  float timeStart = 0.0f;
  float timeEnd = 0.0f;
  for(int c=0; c < anim.channels_count; ++c)
  {
    auto &channel = anim.channels[c];
    const auto &samplerIn = *channel.sampler->input;
    const auto &samplerOut = *channel.sampler->output;
    const char* targetName = channel.target_node->name;

    // create channels, rotation is always combined, the rest (translation, scale) are separate for each axis
    page.channels.emplace_back(targetName, getTarget(channel.target_path), 0);
    if(channel.target_path != cgltf_animation_path_type::cgltf_animation_path_type_rotation) {
      page.channels.emplace_back(targetName, getTarget(channel.target_path), 1);
      page.channels.emplace_back(targetName, getTarget(channel.target_path), 2);
    }

    uint8_t *dataInput = ((uint8_t*)samplerIn.buffer_view->buffer->data) + samplerIn.offset + samplerIn.buffer_view->offset;
    uint8_t *dataOutput = ((uint8_t*)samplerOut.buffer_view->buffer->data) + samplerOut.offset + samplerOut.buffer_view->offset;
    uint8_t *dataOutputNext = dataOutput + samplerOut.stride;
    uint8_t *dataOutputEnd = dataOutput + samplerOut.count * samplerOut.stride;

    timeStart = Gltf::readAsFloat(dataInput, samplerIn.component_type);
    timeEnd = Gltf::readAsFloat(dataInput + (samplerIn.count-1) * samplerIn.stride, samplerIn.component_type);
    float sampleStep = 1.0f / sampleRate;
    float time = timeStart;
    float nextTime = timeStart;

    printf("  - Channel %d\n", c);
    printf("    - Target: %s -> %s\n", targetName, Gltf::getAnimTargetString(channel.target_path));
    printf("    - Input[%s]: %d @ %d\n", Gltf::getTypeString(channel.sampler->input->type), samplerIn.count, samplerIn.stride);
    printf("    - Output[%s]: %d @ %d\n", Gltf::getTypeString(samplerOut.type), samplerOut.count, samplerOut.stride, samplerOut.type);
    printf("    - Time Range: %.4fs -> %.4fs\n", timeStart, timeEnd);

    for(float t=timeStart; t <= timeEnd; t += sampleStep)
    {
      while(dataOutput < dataOutputEnd) {
        time = Gltf::readAsFloat(dataInput, samplerIn.component_type);
        nextTime = Gltf::readAsFloat(dataInput + samplerIn.stride, samplerIn.component_type);
        if(nextTime > t)break;
        dataInput += samplerIn.stride;
        dataOutput += samplerOut.stride;
        dataOutputNext += samplerOut.stride;
      }
      float sampleInterpol = (t - time) / (nextTime - time);

      if(channel.target_path == cgltf_animation_path_type_rotation) {
        Quat value = Gltf::readAsVec4(dataOutput, samplerOut.type, samplerOut.component_type);
        Quat valueNext = Gltf::readAsVec4(dataOutputNext, samplerOut.type, samplerOut.component_type);
        value = value.slerp(valueNext, sampleInterpol);

        //printf("    - %.4fs/%.4f [f:%.2f]: b:%.4f i=%d: %.4f %.4f %.4f %.4f\n", t, timeEnd, frame, sampleInterpol, samplerIn.count, value[0], value[1], value[2], value[3]);
        page.channels.back().valQuat.push_back(value);
      } else {
        Vec3 value = Gltf::readAsVec3(dataOutput, samplerOut.type, samplerOut.component_type);
        Vec3 valueNext = Gltf::readAsVec3(dataOutputNext, samplerOut.type, samplerOut.component_type);
        value = value.mix(valueNext, sampleInterpol);

        //printf("    - %.4fs/%.4f [f:%.2f]: b:%.4f i=%d: %.4f %.4f %.4f\n", t, timeEnd, frame, sampleInterpol, samplerIn.count, value[0], value[1], value[2]);
        page.channels[page.channels.size()-3].valScalar.push_back(value[0]);
        page.channels[page.channels.size()-2].valScalar.push_back(value[1]);
        page.channels[page.channels.size()-1].valScalar.push_back(value[2]);
      }
    }
  }

  page.timeStart = timeStart;
  page.duration = timeEnd - timeStart;
  res.pages.push_back(page);
  return res;
}