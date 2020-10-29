#include "Calculus.h"
#include <optional>
#include <set>

namespace
{
  std::optional<unsigned> divides(unsigned number, unsigned divisor);
  std::set<unsigned> divisors(unsigned x);
  unsigned search_greatest_common_divisor(unsigned smallerValue, unsigned largerValue);
}

namespace calculus
{
  unsigned greatest_common_divisor(unsigned a, unsigned b)
  {
    if (a == b)
    {
      return a;
    }
    if (a < b)
    {
      if (divides(b, a).has_value())
      {
        return a;
      }
      return search_greatest_common_divisor(a, b);
    }
    else
    {
      if (divides(a, b).has_value())
      {
        return b;
      }
      return search_greatest_common_divisor(b, a);
    }
  }

  unsigned least_common_multiple(unsigned a, unsigned b)
  {
    if (a == 0 || b == 0)
    {
      return 0;
    }
    if (divides(b, a).has_value())
    {
      return b;
    }
    if (divides(a, b).has_value())
    {
      return a;
    }
    unsigned gcd = greatest_common_divisor(a, b);
    return a * b / gcd;//gcd * (a / gcd) * (b / gcd);
  }

  unsigned least_common_multiple(const std::vector<unsigned>& values)
  {
    if (values.size() < 3)
    {
      // break condition for recursion
      if (values.empty())
      {
        return 0;
      }
      if (1 == values.size())
      {
        return values[0];
      }
      return least_common_multiple(values[0], values[1]);
    }

    // recursive call
    std::vector<unsigned> next;
    unsigned oneValue = 0;
    for (auto i = 0; i < values.size() / 2; ++i)
    {
      next.push_back(least_common_multiple(values[2 * i], values[2 * i + 1]));
    }
    if (values.size() % 2 > 0)
    {
      next.push_back(values.back());
    }
    return least_common_multiple(next);
  }
}

namespace
{
  std::optional<unsigned> divides(unsigned number, unsigned divisor)
  {
    if (number == 0 || divisor == 0)
    {
      return 0;
    }
    unsigned result = number / divisor;
    if (result * divisor == number)
    {
      return result;
    }
    return std::optional<unsigned>{};
  }

  std::set<unsigned> divisors(unsigned x)
  {
    std::set<unsigned> divisors;
    for (unsigned factor = 1; factor <= sqrt(x); ++factor)
    {
      auto result = divides(x, factor);
      if (result.has_value())
      {
        divisors.insert(factor);
        divisors.insert(result.value());
      }
    }
    return divisors;
  }
  unsigned search_greatest_common_divisor(unsigned smallerValue, unsigned largerValue)
  {
    auto divisors_smaller = divisors(smallerValue);
    auto divisors_larger = divisors(largerValue);

    if (divisors_smaller.empty())
    {
      return 0;
    }

    for (auto divisor = divisors_smaller.rbegin(); divisor != divisors_smaller.rend(); ++divisor)
    {
      if (std::find(divisors_larger.begin(), divisors_larger.end(), *divisor) != divisors_larger.end())
      {
        return *divisor;
      }
    }
    return 1;
  }
}

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

TEST_CASE("test divides") {
  auto result = divides(25, 3);
  CHECK(!result.has_value());

  result = divides(24, 3);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 8);

  result = divides(3, 3);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 1);

  result = divides(16, 4);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 4);

  result = divides(0, 2);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(5, 0);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(0, 0);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(1953125, 16777216);
  CHECK(!result.has_value());
}

TEST_CASE("test divisors") {
  std::set<unsigned> result;

  result = divisors(5);
  CHECK_EQ(result.size(), 2);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(5), result.end());

  result = divisors(16);
  CHECK_EQ(result.size(), 5);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(2), result.end());
  CHECK_NE(result.find(4), result.end());
  CHECK_NE(result.find(8), result.end());
  CHECK_NE(result.find(16), result.end());

  result = divisors(24);
  CHECK_EQ(result.size(), 8);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(2), result.end());
  CHECK_NE(result.find(3), result.end());
  CHECK_NE(result.find(4), result.end());
  CHECK_NE(result.find(6), result.end());
  CHECK_NE(result.find(8), result.end());
  CHECK_NE(result.find(12), result.end());
  CHECK_NE(result.find(24), result.end());

  result = divisors(36);
  CHECK_EQ(result.size(), 9);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(2), result.end());
  CHECK_NE(result.find(3), result.end());
  CHECK_NE(result.find(4), result.end());
  CHECK_NE(result.find(6), result.end());
  CHECK_NE(result.find(9), result.end());
  CHECK_NE(result.find(12), result.end());
  CHECK_NE(result.find(18), result.end());
  CHECK_NE(result.find(36), result.end());

  result = divisors(8192); // 2^13
  CHECK_EQ(result.size(), 14);
  unsigned factorInlist = 1;
  for (auto i = 0; i < 14; ++i)
  {
    CHECK_NE(result.find(factorInlist), result.end());
    factorInlist *= 2;
  }

  result = divisors(262144); // 2^18
  CHECK_EQ(result.size(), 19);
  factorInlist = 1;
  for (auto i = 0; i < 19; ++i)
  {
    CHECK_NE(result.find(factorInlist), result.end());
    factorInlist *= 2;
  }
}

TEST_CASE("test search_greatest_common_divisor") {
  CHECK_EQ(search_greatest_common_divisor(5, 3), 1);
  CHECK_EQ(search_greatest_common_divisor(59049, 262144), 1);
  CHECK_EQ(search_greatest_common_divisor(1024, 262144), 1024);
  CHECK_EQ(search_greatest_common_divisor(24, 36), 12);
  CHECK_EQ(search_greatest_common_divisor(0, 36), 0);
}

TEST_CASE("test greatest_common_divisor") {
  CHECK_EQ(calculus::greatest_common_divisor(5, 3), 1);
  CHECK_EQ(calculus::greatest_common_divisor(262144, 59049), 1);
  CHECK_EQ(calculus::greatest_common_divisor(262144, 1024), 1024);
  CHECK_EQ(calculus::greatest_common_divisor(1024, 262144), 1024);
  CHECK_EQ(calculus::greatest_common_divisor(36, 24), 12);
  CHECK_EQ(calculus::greatest_common_divisor(36, 0), 0);
}

TEST_CASE("test pair-based least_common_multiple") {
  CHECK_EQ(calculus::least_common_multiple(16, 8), 16);
  CHECK_EQ(calculus::least_common_multiple(8, 16), 16);
  CHECK_EQ(calculus::least_common_multiple(16, 18), 144);
  CHECK_EQ(calculus::least_common_multiple(1, 15), 15);
  CHECK_EQ(calculus::least_common_multiple(36, 24), 72);
  CHECK_EQ(calculus::least_common_multiple(243, 1024), 243 * 1024); // power of 3 and power of 2 don't share factors

  CHECK_EQ(calculus::least_common_multiple(0, 18), 0);
  CHECK_EQ(calculus::least_common_multiple(18, 0), 0);
  CHECK_EQ(calculus::least_common_multiple(0, 0), 0);
}

TEST_CASE("test vector-based least_common_multiple")
{
  CHECK_EQ(calculus::least_common_multiple({ 16, 8 }), 16);
  CHECK_EQ(calculus::least_common_multiple({ 8, 16 }), 16);
  CHECK_EQ(calculus::least_common_multiple({ 12, 16, 18 }), 144);
  CHECK_EQ(calculus::least_common_multiple({ 1, 15 }), 15);
  CHECK_EQ(calculus::least_common_multiple({ 36, 24, 9 }), 72);
  CHECK_EQ(calculus::least_common_multiple({ 243, 1024 }), 243 * 1024); // don't share factors
  CHECK_EQ(calculus::least_common_multiple({ 125, 243, 1024 }), 125 * 243 * 1024); // don't share factors

  CHECK_EQ(calculus::least_common_multiple({ 0, 18, 9 }), 0);
  CHECK_EQ(calculus::least_common_multiple({ 18, 0 }), 0);
  CHECK_EQ(calculus::least_common_multiple({ 0, 0 }), 0);
}

#endif