#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

using Axes = std::pair<std::vector<std::uint32_t>, std::vector<std::uint32_t>>;

template<typename T>
std::vector<T> attrs_dot (std::vector<T>& lhs, std::vector<T>& rhs, Axes axes);

// ----------------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------------

template<typename T>
std::vector<T> attrs_dot (std::vector<T>& lhs, std::vector<T>& rhs, Axes axes)
{
  // TODO: add verification of input data correctness
  auto& [lhs_idxs, rhs_idxs] = axes;
  std::vector<T> new_attrs;
  new_attrs.reserve(lhs.size() + rhs.size());
  for (size_t i = 0; i < lhs.size(); ++i)
  {
    auto it = find(lhs_idxs.begin(), lhs_idxs.end(), i);
    if (it == lhs_idxs.end())
      new_attrs.emplace_back(std::move(lhs[i]));
  }
  for (size_t i = 0; i < rhs.size(); ++i)
  {
    auto it = find(rhs_idxs.begin(), rhs_idxs.end(), i);
    if (it == rhs_idxs.end())
      new_attrs.emplace_back(std::move(rhs[i]));
  }
  return new_attrs;
}