#pragma once
#include <vector>
namespace calculus
{
  unsigned greatest_common_divisor(unsigned a, unsigned b);
  unsigned least_common_multiple(unsigned a, unsigned b);
  unsigned least_common_multiple(const std::vector<unsigned>& values);
}