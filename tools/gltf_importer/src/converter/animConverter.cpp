/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include <algorithm>
#include <cassert>
#include "converter.h"
#include "../math/quantizer.h"
#include "mse.h"

namespace {
  constexpr float MIN_VALUE_DELTA = 0.00001f;
  constexpr float MIN_QUAT_DELTA = 0.000000001f;

  constexpr uint16_t time_to_ticks(float t) {
    return (uint16_t)roundf(t * 60.0f);
  }

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
      //if(isIdentity)printf("  - Channel %s %d has identity value\n", channel.targetName.c_str(), channel.targetType);
      return isIdentity;
    }
    return false;
  }

  /**
   * Optimizes the keyframes of a channel.
   * This will attempt to remove keyframes while staying within a certain error threshold.
   */
  void optimizeChannel(AnimChannelMapping &channel, float time) {
    if(channel.keyframes.size() < 2)return;
    auto channelOrg = channel;
    float mse = calcMSE(channel.keyframes, channelOrg.keyframes, 0, time, channel.isRotation());
    assert(mse < 0.00001f); //  initial MSE must be zero
    //printf("  - Channel %s %d: %d keyframes, MSE: %f\n", channel.targetName.c_str(), channel.targetType, channel.keyframes.size(), mse);

    // now remove keyframes and keep the error below a certain threshold
    float threshold      = 0.000001f;
    float thresholdLocal = 0.0000001f;

    for(int i=1;; ++i) {
      if(i >= channel.keyframes.size()-1)break;
      float timeStart = channel.keyframes[i-1].time;
      auto kf = channel.keyframes[i];
      float timeEnd = channel.keyframes[i+1].time;

      channel.keyframes.erase(channel.keyframes.begin() + i);
      float newMSELocal = calcMSE(channel.keyframes, channelOrg.keyframes, timeStart, timeEnd, channel.isRotation());
      float newMSE = calcMSE(channel.keyframes, channelOrg.keyframes, 0, time, channel.isRotation());
      if(newMSE > threshold || newMSELocal > thresholdLocal) {
        channel.keyframes.insert(channel.keyframes.begin() + i, kf);
      } else {
        --i;
      }
    }
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
  // remove all empty channels
  anim.channelMap.erase(
    std::remove_if(anim.channelMap.begin(), anim.channelMap.end(), isEmptyChannel),
    anim.channelMap.end()
  );

  // resample keyframes
  for(auto &ch : anim.channelMap) {
    optimizeChannel(ch, anim.duration);
  }

  // Map the channel target by name to the node index
  for(auto &ch : anim.channelMap) {
    auto it = nodeMap.find(ch.targetName);
    if(it == nodeMap.end()) {
      std::string error = "Animation channel mapper: Node '" + ch.targetName + "' not found";
      throw std::runtime_error(error);
    }
    ch.targetIdx = it->second->index;
    //printf("  - ChannelMapping %s %d.%d\n", ch.targetName.c_str(), ch.targetType, ch.targetIdx);
  }

  // combine channel keyframes into the global timeline
  for(uint32_t c=0; c<anim.channelMap.size(); ++c) {
    auto &keyframes = anim.channelMap[c].keyframes;

    // get the "time needed" which is the time of the previous keyframe
    // This means this keyframe is required to be loaded when the previous keyframe is active.
    // (It will be preloaded as the next KF, so it's ready to be interpolated)
    for(uint32_t k=0; k<keyframes.size(); ++k) {
      keyframes[k].timeNeeded = 0;
      if(k > 0)keyframes[k].timeNeeded = keyframes[(k-1) % keyframes.size()].time;
    }

    for(uint32_t k=0; k<keyframes.size(); ++k) {
      auto &kf = keyframes[k];

      // Now calculate the relative time until the next keyframe is needed
      float nextNeededTime = (k+1) < keyframes.size() ? keyframes[k+1].timeNeeded : anim.duration;
      kf.timeNextInChannel = nextNeededTime - kf.timeNeeded;

      kf.timeTicks = time_to_ticks(kf.time);
      kf.timeNeededTicks = time_to_ticks(kf.timeNeeded);
      kf.timeNextInChannelTicks = time_to_ticks(kf.timeNextInChannel);
      kf.chanelIdx = c;
      //printf("KF[%d]: %.4f, needed: %.4f, next: %.4f\n", k+1, kf.time, kf.timeNeeded, kf.timeNextInChannel);
      anim.keyframes.push_back(kf);
    }
  }

  // sort keyframes by time, if the time is the same, sort by channel index
  std::sort(anim.keyframes.begin(), anim.keyframes.end(), [](const Keyframe &a, const Keyframe &b) {
    if(a.timeNeededTicks == b.timeNeededTicks) {
      if(a.timeTicks == b.timeTicks) {
        return a.chanelIdx < b.chanelIdx;
      }
      return a.timeTicks < b.timeTicks;
    }
    return a.timeNeededTicks < b.timeNeededTicks;
  });

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