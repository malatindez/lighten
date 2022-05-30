#pragma once
#include <algorithm>
#include <array>
#include <assert.h>
#include <cmath>
#include <concepts>
#include <istream>
#include <limits>
#include <numbers>
#include <ostream>
#include <type_traits>

namespace engine::math
{

    template <typename T>
    concept Primitive = std::floating_point<T> || std::integral<T>;

    template <class T>
    [[nodiscard]] bool almost_equal(T x, T y, int ulp = 2) requires(!std::numeric_limits<T>::is_integer)
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
               // unless the result is subnormal
               || std::fabs(x - y) < std::numeric_limits<T>::min();
    }
    template <class T>
    [[nodiscard]] constexpr T radians(T x) noexcept requires(!std::numeric_limits<T>::is_integer)
    {
        return (x / 180.0f) * std::numbers::pi;
    }
    template <Primitive T>
    [[nodiscard]] inline T sin(T x) noexcept { return std::sinf(x); }
    [[nodiscard]] inline double sin(double x) { return std::sin(x); }
    template <Primitive T>
    [[nodiscard]] inline T cos(T x) noexcept { return std::cosf(x); }
    [[nodiscard]] inline double cos(double x) { return std::cos(x); }
    template <Primitive T>
    [[nodiscard]] inline T pow(T x, T y) noexcept { return std::powf(x, y); }
    [[nodiscard]] inline double pow(double x, double y) { return std::pow(x, y); }
}