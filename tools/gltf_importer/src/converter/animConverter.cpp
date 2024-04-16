/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "converter.h"
#include "../math/quantizer.h"
#include "mse.h"

namespace {
  constexpr float MIN_VALUE_DELTA = 0.00001f;
  constexpr float MIN_QUAT_DELTA = 0.000000001f;

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
        bool isIdentity = false;
        switch(channel.targetType) {
          case AnimChannelTarget::TRANSLATION  : isIdentity = fabsf(channel.valScalar[0]) < MIN_VALUE_DELTA; break;
          case AnimChannelTarget::ROTATION     : isIdentity = channel.valQuat[0].isIdentity(); break;
          case AnimChannelTarget::SCALE_UNIFORM:
          case AnimChannelTarget::SCALE        : isIdentity = channel.valScalar[0] == 1.0f;    break;
        }
        if(isIdentity)printf("  - Channel %s %d.%d has identity value\n", channel.targetName.c_str(), channel.targetType, channel.targetIndex);
        if(isIdentity)continue;
      }
      newChannels.push_back(channel);
    }
    return newChannels;
  }

  void quantizeRotations(AnimChannel &ch)
  {
    uint32_t quatQuant;
    for(const Quat &q : ch.valQuat) {
      quatQuant = Quantizer::quatTo32Bit(q);
      if(quatQuant == 0) {
        throw std::runtime_error(std::string{"Quantized rotation is zero: "} + ch.targetName + " Quat: " + q.toString());
      }

      ch.valQuantized.push_back(quatQuant & 0xFFFF);
      ch.valQuantized.push_back(quatQuant >> 16);
    }
  }

  void resampleScalars(AnimChannel &channel, uint32_t frames)
  {
    if(channel.valScalar.empty())return;
    // First check if all values are the same,
    // if so only store one value and set the sample-rate to zero
    float lastValue = channel.valScalar.back();
    int endIdx;
    for(endIdx=channel.valScalar.size()-1; endIdx >= 0; --endIdx) {
      if(fabs(channel.valScalar[endIdx] - lastValue) > MIN_VALUE_DELTA)break;
    }
    endIdx += 1;

    if(endIdx == 0) {
      channel.valScalar = {lastValue};
      channel.sampleRate = 0;
    } else {
      channel.valScalar = std::vector<float>(channel.valScalar.begin(), channel.valScalar.begin() + endIdx+1);
    }
  }

  void resampleQuats(AnimChannel &channel, uint32_t frames)
  {
    if(channel.valQuat.empty())return;
    // First check if all values are the same,
    // if so only store one value and set the sample-rate to zero
    Vec4 lastValue = channel.valQuat.back().toVec4();
    int endIdx;
    for(endIdx=channel.valQuat.size()-1; endIdx >= 0; --endIdx) {
      Vec4 curr = channel.valQuat[endIdx].toVec4();
      float dist2 = (curr - lastValue).length2();
      if(dist2 > MIN_QUAT_DELTA)break;
    }
    endIdx += 1;

    if(endIdx == 0) {
      //printf("  - All values are the same in %s\n", channel.targetName.c_str());
      channel.valQuat = {lastValue};
      channel.sampleRate = 0;
    } else {
      //printf("  - Partial values (%d/%d) are the same in %s\n", endIdx, channel.valQuat.size(), channel.targetName.c_str());
      channel.valQuat = std::vector<Quat>(channel.valQuat.begin(), channel.valQuat.begin() + endIdx + 1);
    }
  }

  std::vector<AnimPage> splitPages(AnimPage &page) {
    if(page.channels.size() == 0) {
      return {page};
    }

    constexpr uint32_t TARGET_PAGE_SIZE = 1024*4;

    // @TODO: implement proper splitting by max-size
    auto pageOrg = page;
    std::vector<AnimPage> pages;

    // split page in half
    int totalFrames = pageOrg.channels[0].isRotation() ? pageOrg.channels[0].valQuat.size() : pageOrg.channels[0].valScalar.size();
    uint32_t predictedAnimSize = 0;
    for(auto &ch : page.channels) {
      uint32_t frames = ch.isRotation() ? ch.valQuat.size() : ch.valScalar.size();
      predictedAnimSize += frames * (ch.isRotation() ? 4 : 2);
    }

    float time = 0.0f;
    uint32_t sliceFrames = (uint32_t)((float)TARGET_PAGE_SIZE / ((float)predictedAnimSize / (float)totalFrames));
    if(sliceFrames < 4)sliceFrames = 4;

    printf("Predicted anim size: %d (%d/frame) | slice: %d\n", predictedAnimSize, predictedAnimSize / totalFrames, sliceFrames);

    uint32_t frameOffset = 0;
    int framesLeft = totalFrames;

    while(framesLeft > 0)
    {
      AnimPage newPage = page;
      newPage.timeStart = time;

      if(frameOffset + sliceFrames > totalFrames) {
        sliceFrames = totalFrames - frameOffset;
      }
      uint32_t frameOffsetEnd = frameOffset + sliceFrames+1;
      uint32_t wrapFrames = 0;
      if(frameOffsetEnd > totalFrames) {
        wrapFrames = 1;
        frameOffsetEnd = totalFrames;
      }

      newPage.duration = (float)(sliceFrames) / (float)page.sampleRate;

      printf("Duration: %.2f (end: %.4f), frames: %d/%d, left: %d\n", newPage.duration, newPage.timeStart +newPage.duration, frameOffset, totalFrames, framesLeft);

      uint32_t c=0;
      for(auto &ch : newPage.channels) {
        const auto orgChannel = pageOrg.channels[c];
        if(ch.isRotation()) {
          ch.valQuat = std::vector<Quat>(orgChannel.valQuat.begin() + frameOffset, orgChannel.valQuat.begin() + frameOffsetEnd);
          if(wrapFrames > 0)ch.valQuat.push_back(*orgChannel.valQuat.begin());

          resampleQuats(ch, sliceFrames);
        } else {
          ch.valScalar = std::vector<float>(orgChannel.valScalar.begin() + frameOffset, orgChannel.valScalar.begin() + frameOffsetEnd);
          if(wrapFrames > 0)ch.valScalar.push_back(*orgChannel.valScalar.begin());

          resampleScalars(ch, sliceFrames);
        }
        ++c;
      }

      pages.push_back(newPage);
      time += newPage.duration;
      framesLeft -= sliceFrames;
      frameOffset += sliceFrames;
    }

    for(int c=0; c<pageOrg.channels.size(); ++c) {
      float mse = calcMSE(pageOrg, pages, c);
      printf("Resampling, channel %d MSE: %.4f%% (%d original frames)\n", c, mse * 100.0f, page.channels[c].valScalar.size());
    }

    return pages;
  }
}

void convertAnimation(Anim &anim, const std::unordered_map<std::string, const Bone*> &nodeMap)
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
      ch.targetName, it->second->index, ch.targetType, ch.targetIndex, 0.0f, 0.0f
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
  anim.maxPageSize = 0;
  for(auto &page : anim.pages)
  {
    page.byteSize = 0;
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