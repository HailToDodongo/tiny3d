/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "converter.h"
#include "../math/quantizer.h"

namespace {
  template<typename T>
  bool hasConstValue(const std::vector<T> &values) {
    T lastValue = values[0];
    for(int i=1; i < values.size(); ++i) {
      if(values[i] != lastValue)return false;
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
      ch.valQuantized.push_back(quatQuant >> 16);
      ch.valQuantized.push_back(quatQuant & 0xFFFF);
    }
  }

  std::vector<AnimPage> splitPages(AnimPage &page) {
    // @TODO:
    return {page};
  }
}

void convertAnimation(Anim &anim)
{
  printf("Convert: %s\n", anim.name.c_str());

  // Apply optimizations to the animation
  anim.pages.back().channels = filterEmptyChannels(anim.pages.back().channels);
  // @TODO: check and convert SCALE to SCALE_UNIFORM if all components are the same

  // Split animations into pages / resample if possible
  anim.pages = splitPages(anim.pages.back());

  // Now quantize/compress the values
  for(auto &page : anim.pages)
  {
    for(auto &ch : page.channels)
    {
      if(ch.isRotation()) {
        quantizeRotations(ch);
      } else {
        ch.valQuantized = Quantizer::floatsToU16(ch.valScalar, ch.quantOffset, ch.quantScale);
      }
    }
  }
}