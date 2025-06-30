/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "main.h"
#include <functional>

namespace DebugMenu
{
  enum class EntryType : uint32_t {
    INT=0, FLOAT, BOOL
  };

  struct Entry
  {
    const char* name;
    EntryType type{};
    void* value{nullptr};

    float min{0.0f};
    float max{1.0f};
    float incr{1.0f};
  };

  void reset();
  void addEntry(const Entry& entry, bool *changedFlag = nullptr);

  void draw();
}