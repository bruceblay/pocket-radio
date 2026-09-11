#pragma once
#include <cstddef>
#include <cstdint>

class StationSelection {
  size_t total;
  uint32_t selected;
public:
  explicit StationSelection(size_t count, uint32_t defaults = 0) : total(count), selected((uint32_t(1) << count) - 1) { if (defaults) set(defaults); }
  uint32_t mask() const { return selected; }
  bool set(uint32_t value) {
    const uint32_t allowed = (uint32_t(1) << total) - 1;
    if (!value || (value & ~allowed)) return false;
    selected = value; return true;
  }
  bool includes(size_t index) const { return index < total && (selected & (uint32_t(1) << index)); }
  size_t first() const { for (size_t i = 0; i < total; ++i) if (includes(i)) return i; return 0; }
  size_t next(size_t index) const {
    for (size_t offset = 1; offset <= total; ++offset) {
      size_t candidate = (index + offset) % total;
      if (includes(candidate)) return candidate;
    }
    return first();
  }
  size_t count() const { size_t n = 0; for (size_t i = 0; i < total; ++i) n += includes(i); return n; }
};
