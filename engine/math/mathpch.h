#pragma once
#include <algorithm>
#include <array>
#include <assert.h>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <istream>
#include <limits>
#include <numbers>
#include <ostream>
#include <type_traits>

namespace engine::core::math
{

    template <typename T>
    concept Primitive = std::floating_point<T> || std::integral<T>;
    namespace _detail
    {
        // reference wrapper without rebinding
        // reference can only be instantiated in constructor
        template <Primitive T>
        class primitive_reference_wrapper
        {
        public:
            using type = T;
            explicit constexpr primitive_reference_wrapper(T &ref) : ptr_{std::addressof(ref)} {}
            constexpr void set_ptr(T &ref) noexcept
            {
                ptr_ = std::addressof(ref);
            }

            constexpr operator T &() const noexcept
            {
                return *ptr_;
            }
            template <Primitive U>
            constexpr primitive_reference_wrapper<T> &operator=(U const &v) requires(!std::is_const_v<T>)
            {
                *ptr_ = static_cast<T>(v);
                return *this;
            }
            template <Primitive U>
            constexpr primitive_reference_wrapper<T> &operator=(U &&v) requires(!std::is_const_v<T>)
            {
                *ptr_ = static_cast<T>(v);
                return *this;
            }

            template <Primitive U>
            constexpr primitive_reference_wrapper<T> &operator=(primitive_reference_wrapper<U> const &v) requires(!std::is_const_v<T>)
            {
                *ptr_ = static_cast<T>(*v.ptr_);
                return *this;
            }
            template <Primitive U>
            constexpr primitive_reference_wrapper<T> &operator=(primitive_reference_wrapper<U> &&v) requires(!std::is_const_v<T>)
            {
                *ptr_ = static_cast<T>(*v.ptr_);
                return *this;
            }

        private: 
        T *ptr_{};
        public:
            template <class... Types>
            constexpr auto operator()(Types &&...args) const
                noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
                    -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
            {
                return std::invoke(*ptr_, static_cast<Types &&>(args)...);
            }
        };
    } // namespace _detail

    template <class T>
    [[nodiscard]] bool almost_equal(T x, T y, int ulp = 2) requires(!std::numeric_limits<T>::is_integer)
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::fabs(x - y) <=
                   std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
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
} // namespace engine::core::math