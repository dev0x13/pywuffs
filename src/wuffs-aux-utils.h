#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace utils {

template <typename QuirkType>
std::vector<wuffs_aux::QuirkKeyValuePair> ConvertQuirks(
    const std::map<QuirkType, uint64_t>& quirks_map) {
  std::vector<wuffs_aux::QuirkKeyValuePair> quirks_vector;
  quirks_vector.reserve(quirks_map.size());
  for (const auto& quirk : quirks_map) {
    quirks_vector.emplace_back(static_cast<uint32_t>(quirk.first),
                               quirk.second);
  }
  return quirks_vector;
}

}  // namespace utils