#pragma once

#include <cstdint>
#include <vector>
#include <cmath>

#include "tensors_utils.hpp"

struct FakeTensor
{
  std::vector<std::uint32_t> shape;
  std::uint64_t past_complexity = 0;
};

FakeTensor tensordot (FakeTensor lhs, FakeTensor rhs, Axes axes);

// ----------------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------------

FakeTensor tensordot (FakeTensor lhs, FakeTensor rhs, Axes axes)
{
  auto accumulator = [](std::uint32_t first, std::uint32_t second){
    return first * second;
  };
  std::uint64_t s1 = std::accumulate(lhs.shape.begin(), lhs.shape.end(), 1, accumulator);
  std::uint64_t s2 = std::accumulate(rhs.shape.begin(), rhs.shape.end(), 1, accumulator);
  std::vector<std::uint32_t> new_shape = attrs_dot(lhs.shape, rhs.shape, std::move(axes));
  std::uint64_t s12 = std::accumulate(new_shape.begin(), new_shape.end(), 1, accumulator);
  std::uint64_t new_complexity = lhs.past_complexity
                          + rhs.past_complexity 
                          + static_cast<std::uint64_t> (std::sqrt(s1 * s2 * s12));
  return {std::move(new_shape), new_complexity};
}
