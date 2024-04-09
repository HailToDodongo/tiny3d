/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "converter.h"
#include "../math/quantizer.h"

namespace {
  constexpr float MIN_VALUE_DELTA = 0.0001f;

  bool hasConstValue(const std::vector<Quat> &values) {
    Quat lastValue = values[0];
    for(int i=1; i < values.size(); ++i) {
      if(values[i] != lastValue)return false;
    }
    return true;
  }

  bool hasConstValue(const std::vector<float> &values) {
    float lastValue = values[0];
    for(int i=1; i < values.size(); ++i) {
      if(fabs(values[i] - lastValue) > MIN_VALUE_DELTA)return false;
    }
    return true;
  }

  // Filters any channel that has a constant value, and is also the identity value of the specific target
  std::vector<AnimChannel> filterEmptyChannels(const std::vector<AnimChannel> &channels)
  {
    std::vector<AnimChannel> newChannels;
    for(auto &channel : channels)
    {
      if(channel.isRotation() ? hasConstValue(channel.valQuat) : hasConstValue(channel.valScalar)) {
        //printf("  - Channel %s %d.%d has constant value\n", channel.targetName.c_str(), channel.targetType, channel.targetIndex);
        bool isIdentity = false;
        switch(channel.targetType) {
          case AnimChannelTarget::TRANSLATION  : isIdentity = channel.valScalar[0] == 0.0f;    break;
          case AnimChannelTarget::ROTATION     : isIdentity = channel.valQuat[0].isIdentity(); break;
          case AnimChannelTarget::SCALE_UNIFORM:
          case AnimChannelTarget::SCALE        : isIdentity = channel.valScalar[0] == 1.0f;    break;
        }
        if(isIdentity)continue;
      }
      newChannels.push_back(channel);
    }
    return newChannels;
  }

  void quantizeRotations(AnimChannel &ch)
  {
    for(const Quat &q : ch.valQuat) {
      uint32_t quatQuant = Quantizer::quatTo32Bit(q);
      ch.valQuantized.push_back(quatQuant & 0xFFFF);
      ch.valQuantized.push_back(quatQuant >> 16);
    }
  }

  std::vector<AnimPage> splitPages(AnimPage &page) {
    // @TODO: implement proper splitting by max-size
    return {page};
  }
}

void convertAnimation(Anim &anim, const std::unordered_map<std::string, uint32_t> &nodeMap)
{
  printf("Convert: %s\n", anim.name.c_str());

  // Apply optimizations to the animation
  anim.pages.back().channels = filterEmptyChannels(anim.pages.back().channels);
  // @TODO: check and convert SCALE to SCALE_UNIFORM if all components are the same

  for(auto &ch : anim.pages.back().channels) {
    printf("  - ChannelMapping %s %d.%d\n", ch.targetName.c_str(), ch.targetType, ch.targetIndex);
    auto it = nodeMap.find(ch.targetName);
    if(it == nodeMap.end()) {
      std::string error = "Animation channel mapper: Node '" + ch.targetName + "' not found";
      throw std::runtime_error(error);
    }

    anim.channelMap.emplace_back(
      ch.targetName, it->second, ch.targetType, ch.targetIndex, 0.0f, 0.0f
    );

    if(!ch.isRotation()) {
      Quantizer::floatsGetOffsetScale(
        ch.valScalar, anim.channelMap.back().quantOffset, anim.channelMap.back().quantScale
      );
    }
  }

  // Split animations into pages / resample if possible
  anim.pages = splitPages(anim.pages.back());

  // Now quantize/compress the values
  anim.duration = 0.0f;
  anim.maxPageSize = 0;
  for(auto &page : anim.pages)
  {
    page.byteSize = 0;
    anim.duration += page.duration;
    uint32_t channelIdx = 0;
    for(auto &ch : page.channels)
    {
      if(ch.isRotation()) {
        quantizeRotations(ch);
      } else {
        ch.valQuantized = Quantizer::floatsToU16(ch.valScalar,
          anim.channelMap[channelIdx].quantOffset, anim.channelMap[channelIdx].quantScale
        );
      }
      page.byteSize += ch.valQuantized.size() * sizeof(uint16_t);
      ++channelIdx;
    }

    anim.maxPageSize = std::max(anim.maxPageSize, page.byteSize);
  }
}