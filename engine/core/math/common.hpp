#pragma once
#include <algorithm>
#include <array>
#include <assert.h>
#include <concepts>
#include <cstdint>
#include <istream>
#include <limits>
#include <math.h>
#include <numbers>
#include <ostream>
#include <type_traits>

namespace engine::core::math
{
    template <typename T>
    concept Primitive = std::floating_point<T> || std::integral<T>;
    template <size_t L, Primitive T>
    struct vec;
    template <size_t L, Primitive T>
    struct rvec;
    template <size_t rows, size_t columns, Primitive T>
    struct mat;
    template <size_t rows, size_t columns, Primitive T>
    struct rmat;

    namespace _detail
    {
        // reference wrapper without rebinding
        // reference can only be instantiated in constructor
        template <Primitive T>
        class primitive_reference_wrapper
        {
        public:
            using type = T;
            explicit constexpr primitive_reference_wrapper(T &ref) : ptr_ { std::addressof(ref) } {}
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
            T *ptr_ {};

        public:
            template <class... Types>
            constexpr auto operator()(Types &&...args) const
                noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
                -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
            {
                return std::invoke(*ptr_, static_cast<Types &&>(args)...);
            }
        };

        template <class T>
        struct is_mat : public std::false_type
        {};
        template <size_t a, size_t b, Primitive T>
        struct is_mat<mat<a, b, T>> : public std::true_type
        {};
        template <size_t a, size_t b, Primitive T>
        struct is_mat<rmat<a, b, T>> : public std::true_type
        {};

        template <class T>
        struct is_reference_vec : public std::false_type
        {};
        template <size_t size, Primitive T>
        struct is_reference_vec<rvec<size, T>> : public std::true_type
        {};
        template <class T>
        struct is_default_vec : public std::false_type
        {};
        template <size_t size, Primitive T>
        struct is_default_vec<vec<size, T>> : public std::true_type
        {};

        template <class T>
        constexpr bool is_mat_v = is_mat<T>::value;

        template <class T>
        constexpr bool is_reference_vec_v = is_reference_vec<T>::value;
        template <class T>
        constexpr bool is_default_vec_v = is_default_vec<T>::value;
        template <class T>
        constexpr bool is_vec_v = is_reference_vec_v<T> || is_default_vec_v<T>;

    } // namespace _detail

    template <class T>
    concept AnyMat = _detail::is_mat_v<T>;
    template <class T>
    concept AnyVec = _detail::is_vec_v<T>;
    template <class T>
    concept Vec = _detail::is_default_vec_v<T>;
    template <class T>
    concept RVec = _detail::is_reference_vec_v<T>;

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
        return (x / 180.0f) * static_cast<float>(std::numbers::pi);
    }
    namespace detail
    {
        template <Primitive T>
        [[nodiscard]] inline float tan(T x) noexcept { return std::tanf(x); }
        [[nodiscard]] inline double tan(double x) { return std::tan(x); }
        [[nodiscard]] inline long double tan(long double x) { return std::tanl(x); }
        template <Primitive T>
        [[nodiscard]] inline float sin(T x) noexcept { return std::sinf(x); }
        [[nodiscard]] inline double sin(double x) { return std::sin(x); }
        [[nodiscard]] inline long double sin(long double x) { return std::sinl(x); }
        template <Primitive T>
        [[nodiscard]] inline float cos(T x) noexcept { return std::cosf(x); }
        [[nodiscard]] inline double cos(double x) { return std::cos(x); }
        [[nodiscard]] inline long double cos(long double x) { return std::cosl(x); }
        template <Primitive T>
        [[nodiscard]] inline float atan(T x) noexcept { return std::atanf(x); }
        [[nodiscard]] inline double atan(double x) { return std::atan(x); }
        [[nodiscard]] inline long double atan(long double x) { return std::atanl(x); }
        template <Primitive T>
        [[nodiscard]] inline float asin(T x) noexcept { return std::asinf(x); }
        [[nodiscard]] inline double asin(double x) { return std::asin(x); }
        [[nodiscard]] inline long double asin(long double x) { return std::asinl(x); }
        template <Primitive T>
        [[nodiscard]] inline float acos(T x) noexcept { return std::acosf(x); }
        [[nodiscard]] inline double acos(double x) { return std::acos(x); }
        [[nodiscard]] inline long double acos(long double x) { return std::acosl(x); }
        template <Primitive T, Primitive U>
        [[nodiscard]] inline float pow(T x, U y) noexcept { return std::powf(x, y); }
        template <Primitive U>
        [[nodiscard]] inline double pow(double x, U y) { return std::pow(x, y); }
        template <Primitive U>
        [[nodiscard]] inline long double pow(long double x, U y) { return std::powl(x, y); }
        template <Primitive T>
        [[nodiscard]] inline float exp(T x) noexcept { return std::expf(x); }
        [[nodiscard]] inline double exp(double x) { return std::exp(x); }
        [[nodiscard]] inline long double exp(long double x) { return std::expl(x); }
        template <Primitive T>
        [[nodiscard]] inline float sqrt(T x) noexcept { return std::sqrtf(x); }
        [[nodiscard]] inline double sqrt(double x) { return std::sqrt(x); }
        [[nodiscard]] inline long double sqrt(long double x) { return std::sqrtl(x); }
        template <Primitive T>
        [[nodiscard]] inline float cbrt(T x) noexcept { return std::cbrtf(x); }
        [[nodiscard]] inline double cbrt(double x) { return std::cbrt(x); }
        [[nodiscard]] inline long double cbrt(long double x) { return std::cbrtl(x); }
    } // namespace detail
} // namespace engine::core::math