/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include <algorithm>
#include "converter.h"
#include "../math/quantizer.h"
#include "mse.h"

namespace {
  constexpr float MIN_VALUE_DELTA = 0.00001f;
  constexpr float MIN_QUAT_DELTA = 0.000000001f;


  bool hasConstValue(const std::vector<Keyframe> &keyframes, bool isRotation) {
    if(!isRotation) {
      float lastValue = keyframes[0].valScalar;
      for(int i=1; i < keyframes.size(); ++i) {
        if(fabs(keyframes[i].valScalar - lastValue) > MIN_VALUE_DELTA)return false;
      }
    } else {
      Quat lastValue = keyframes[0].valQuat;
      for(int i=1; i < keyframes.size(); ++i) {
        if(keyframes[i].valQuat != lastValue)return false;
      }
    }
    return true;
  }

  // Filters any channel that has a constant value, and is also the identity value of the specific target
  bool isEmptyChannel(AnimChannelMapping &channel)
  {
    if(hasConstValue(channel.keyframes, channel.isRotation())) {
      bool isIdentity = false;
      const auto &kf = channel.keyframes[0];
      switch(channel.targetType) {
        case AnimChannelTarget::TRANSLATION  : isIdentity = fabsf(kf.valScalar) < MIN_VALUE_DELTA; break;
        case AnimChannelTarget::ROTATION     : isIdentity = kf.valQuat.isIdentity(); break;
        case AnimChannelTarget::SCALE_UNIFORM:
        case AnimChannelTarget::SCALE        : isIdentity = kf.valScalar == 1.0f; break;
      }
      if(isIdentity)printf("  - Channel %s %d has identity value\n", channel.targetName.c_str(), channel.targetType);
      return isIdentity;
    }
    return false;
  }

  void quantizeRotation(Keyframe &kf)
  {
    uint32_t quatQuant = Quantizer::quatTo32Bit(kf.valQuat);
    if(quatQuant == 0) {
      throw std::runtime_error(std::string{"Quantized rotation is zero: "} + std::to_string(kf.chanelIdx) + " Quat: " + kf.valQuat.toString());
    }
    kf.valQuantSize = 2;
    kf.valQuant[1] = quatQuant & 0xFFFF;
    kf.valQuant[0] = quatQuant >> 16;
  }
}

void convertAnimation(Anim &anim, const std::unordered_map<std::string, const Bone*> &nodeMap)
{
  printf("Convert: %s\n", anim.name.c_str());

  // remove all empty channels
  anim.channelMap.erase(
    std::remove_if(anim.channelMap.begin(), anim.channelMap.end(), isEmptyChannel),
    anim.channelMap.end()
  );

  // resample keyframes
  // @TODO:

  for(auto &ch : anim.channelMap) {
    auto it = nodeMap.find(ch.targetName);
    if(it == nodeMap.end()) {
      std::string error = "Animation channel mapper: Node '" + ch.targetName + "' not found";
      throw std::runtime_error(error);
    }

    ch.targetIdx = it->second->index;
    printf("  - ChannelMapping %s %d.%d\n", ch.targetName.c_str(), ch.targetType, ch.targetIdx);
  }

  // combine channel keyframes into the global timeline
  for(uint32_t c=0; c<anim.channelMap.size(); ++c) {
    for(auto &kf : anim.channelMap[c].keyframes) {
      kf.chanelIdx = c;
      anim.keyframes.push_back(kf);
    }
  }

  // sort keyframes by time
  std::sort(anim.keyframes.begin(), anim.keyframes.end(), [](const Keyframe &a, const Keyframe &b) {
    return a.time < b.time;
  });

  // Now change the timestamps to point to the previous keyframe of the same channel
  // this is done to actually load the next keyframe at any given time
  std::unordered_map<uint32_t, float> kfChannelTime;
  for(auto &kf : anim.keyframes)
  {
    auto it = kfChannelTime.find(kf.chanelIdx);
    if(it != kfChannelTime.end()) {
      auto &ch = anim.channelMap[kf.chanelIdx];
      float prevTime = it->second;
      if(ch.timeOffset == 0.0f)ch.timeOffset = kf.time - prevTime;
      kfChannelTime[kf.chanelIdx] = kf.time;
      kf.time = prevTime;
    } else {
      kfChannelTime[kf.chanelIdx] = kf.time;
    }
  }

  // Now quantize/compress the values
  for(auto &kf : anim.keyframes)
  {
    auto &ch = anim.channelMap[kf.chanelIdx];
    if(ch.targetType == AnimChannelTarget::ROTATION) {
      quantizeRotation(kf);
    } else {
      kf.valQuantSize = 1;
      kf.valQuant[0] = Quantizer::floatToU16(kf.valScalar, ch.valueMin, ch.valueMax - ch.valueMin);
    }
  }

  // re-count channels
  anim.channelCountQuat = 0;
  anim.channelCountScalar = 0;
  for(auto &ch : anim.channelMap) {
    if(ch.targetType == AnimChannelTarget::ROTATION) {
      anim.channelCountQuat++;
    } else {
      anim.channelCountScalar++;
    }
  }
}