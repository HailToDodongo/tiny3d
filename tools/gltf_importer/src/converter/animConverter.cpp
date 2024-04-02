/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "converter.h"

namespace {
  const char TYPE_CHAR[4] = {'T', 'S', 's', 'R'};

  template<typename T>
  bool hasConstValue(const std::vector<T> &values) {
    T lastValue = values[0];
    for(int i=1; i < values.size(); ++i) {
      if(values[i] != lastValue)return false;
    }
    return true;
  }

  // Filters any channel that has a constant value which is also the identity value of the specific target
  std::vector<AnimChannel> filterEmptyChannels(const std::vector<AnimChannel> &channels)
  {
    std::vector<AnimChannel> newChannels;
    for(auto &channel : channels)
    {
      if(channel.isRotation() ? hasConstValue(channel.valQuat) : hasConstValue(channel.valScalar)) {
        //printf("  - Channel %s %d.%d has constant value\n", channel.targetName.c_str(), channel.targetType, channel.targetIndex);
        bool isIdentity = false;
        switch(channel.targetType) {
          case AnimChannelTarget::TRANSLATION  : isIdentity = channel.startValScalar == 0.0f;    break;
          case AnimChannelTarget::ROTATION     : isIdentity = channel.startValQuat.isIdentity(); break;
          case AnimChannelTarget::SCALE_UNIFORM:
          case AnimChannelTarget::SCALE        : isIdentity = channel.startValScalar == 1.0f;    break;
        }
        if(isIdentity)continue;
      }
      newChannels.push_back(channel);
    }
    return newChannels;
  }
}

void convertAnimation(Anim &anim)
{
  printf("Convert: %s\n", anim.name.c_str());
  anim.channels = filterEmptyChannels(anim.channels);

  printf("\n\n");
  for(const auto &ch : anim.channels) {
    printf("==== Channel %s %c[%d] ====\n", ch.targetName.c_str(), TYPE_CHAR[ch.targetType], ch.targetIndex);
  }
}