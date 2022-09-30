#pragma once
#include "include/library-pch.hpp"
#include <type_traits>

namespace engine::core::math
{
    template <typename T>
    concept Primitive = std::floating_point<T> || std::integral<T>;
    template <size_t L, Primitive T>
    struct vec;
    namespace _detail
    {
        template <size_t L, Primitive T>
        struct rvec;
    } // namespace _detail
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
            explicit constexpr primitive_reference_wrapper(T &ref) : ptr_{ std::addressof(ref) } {}
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
        struct is_reference_vec<_detail::rvec<size, T>> : public std::true_type
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
    template <Primitive T>
    [[nodiscard]] inline constexpr T clamp(T x, T min, T max) noexcept { return x < min ? min : x > max ? max : x; }

    template <Primitive T>
    [[nodiscard]] inline float cos(T x) noexcept { return std::cosf(x); }
    [[nodiscard]] inline double cos(double x) { return std::cos(x); }
    [[nodiscard]] inline long double cos(long double x) { return std::cosl(x); }
    template <Primitive T>
    [[nodiscard]] inline float sin(T x) noexcept { return std::sinf(x); }
    [[nodiscard]] inline double sin(double x) { return std::sin(x); }
    [[nodiscard]] inline long double sin(long double x) { return std::sinl(x); }
    template <Primitive T>
    [[nodiscard]] inline float tan(T x) noexcept { return std::tanf(x); }
    [[nodiscard]] inline double tan(double x) { return std::tan(x); }
    [[nodiscard]] inline long double tan(long double x) { return std::tanl(x); }
    template <Primitive T>
    [[nodiscard]] inline float acos(T x) noexcept { return std::acosf(x); }
    [[nodiscard]] inline double acos(double x) { return std::acos(x); }
    [[nodiscard]] inline long double acos(long double x) { return std::acosl(x); }
    template <Primitive T>
    [[nodiscard]] inline float asin(T x) noexcept { return std::asinf(x); }
    [[nodiscard]] inline double asin(double x) { return std::asin(x); }
    [[nodiscard]] inline long double asin(long double x) { return std::asinl(x); }
    template <Primitive T>
    [[nodiscard]] inline float atan(T x) noexcept { return std::atanf(x); }
    [[nodiscard]] inline double atan(double x) { return std::atan(x); }
    [[nodiscard]] inline long double atan(long double x) { return std::atanl(x); }
    template <Primitive T>
    [[nodiscard]] inline float atan2(T x, T y) noexcept { return std::atan2f(x, y); }
    [[nodiscard]] inline double atan2(double x, double y) { return std::atan2(x, y); }
    [[nodiscard]] inline long double atan2(long double x, long double y) { return std::atan2l(x, y); }
    template <Primitive T>
    [[nodiscard]] inline float cosh(T x) noexcept { return std::coshf(x); }
    [[nodiscard]] inline double cosh(double x) { return std::cosh(x); }
    [[nodiscard]] inline long double cosh(long double x) { return std::coshl(x); }
    template <Primitive T>
    [[nodiscard]] inline float sinh(T x) noexcept { return std::sinhf(x); }
    [[nodiscard]] inline double sinh(double x) { return std::sinh(x); }
    [[nodiscard]] inline long double sinh(long double x) { return std::sinhl(x); }
    template <Primitive T>
    [[nodiscard]] inline float tanh(T x) noexcept { return std::tanhf(x); }
    [[nodiscard]] inline double tanh(double x) { return std::tanh(x); }
    [[nodiscard]] inline long double tanh(long double x) { return std::tanhl(x); }
    template <Primitive T>
    [[nodiscard]] inline float acosh(T x) noexcept { return std::acoshf(x); }
    [[nodiscard]] inline double acosh(double x) { return std::acosh(x); }
    [[nodiscard]] inline long double acosh(long double x) { return std::acoshl(x); }
    template <Primitive T>
    [[nodiscard]] inline float asinh(T x) noexcept { return std::asinhf(x); }
    [[nodiscard]] inline double asinh(double x) { return std::asinh(x); }
    [[nodiscard]] inline long double asinh(long double x) { return std::asinhl(x); }
    template <Primitive T>
    [[nodiscard]] inline float atanh(T x) noexcept { return std::atanhf(x); }
    [[nodiscard]] inline double atanh(double x) { return std::atanh(x); }
    [[nodiscard]] inline long double atanh(long double x) { return std::atanhl(x); }
    template <Primitive T>
    [[nodiscard]] inline float exp(T x) noexcept { return std::expf(x); }
    [[nodiscard]] inline double exp(double x) { return std::exp(x); }
    [[nodiscard]] inline long double exp(long double x) { return std::expl(x); }
    template <Primitive T>
    [[nodiscard]] inline float exp2(T x) noexcept { return std::exp2f(x); }
    [[nodiscard]] inline double exp2(double x) { return std::exp2(x); }
    [[nodiscard]] inline long double exp2(long double x) { return std::exp2l(x); }
    template <Primitive T>
    [[nodiscard]] inline float expm1(T x) noexcept { return std::expm1f(x); }
    [[nodiscard]] inline double expm1(double x) { return std::expm1(x); }
    [[nodiscard]] inline long double expm1(long double x) { return std::expm1l(x); }
    template <Primitive T>
    [[nodiscard]] inline float log(T x) noexcept { return std::logf(x); }
    [[nodiscard]] inline double log(double x) { return std::log(x); }
    [[nodiscard]] inline long double log(long double x) { return std::logl(x); }
    template <Primitive T>
    [[nodiscard]] inline float log2(T x) noexcept { return std::log2f(x); }
    [[nodiscard]] inline double log2(double x) { return std::log2(x); }
    [[nodiscard]] inline long double log2(long double x) { return std::log2l(x); }
    template <Primitive T>
    [[nodiscard]] inline float log10(T x) noexcept { return std::log10f(x); }
    [[nodiscard]] inline double log10(double x) { return std::log10(x); }
    [[nodiscard]] inline long double log10(long double x) { return std::log10l(x); }
    template <Primitive T>
    [[nodiscard]] inline float log1p(T x) noexcept { return std::log1pf(x); }
    [[nodiscard]] inline double log1p(double x) { return std::log1p(x); }
    [[nodiscard]] inline long double log1p(long double x) { return std::log1pl(x); }
    template <Primitive T>
    [[nodiscard]] inline float logb(T x) noexcept { return std::logbf(x); }
    [[nodiscard]] inline double logb(double x) { return std::logb(x); }
    [[nodiscard]] inline long double logb(long double x) { return std::logbl(x); }
    template <Primitive T>
    [[nodiscard]] inline float sqrt(T x) noexcept { return std::sqrtf(x); }
    [[nodiscard]] inline double sqrt(double x) { return std::sqrt(x); }
    [[nodiscard]] inline long double sqrt(long double x) { return std::sqrtl(x); }
    template <Primitive T>
    [[nodiscard]] inline float inversesqrt(T x) noexcept { return 1.0f / std::sqrtf(x); }
    [[nodiscard]] inline double inversesqrt(double x) { return 1.0 / std::sqrt(x); }
    [[nodiscard]] inline long double inversesqrt(long double x) { return 1.0l / std::sqrtl(x); }
    template <Primitive T>
    [[nodiscard]] inline T abs(T x) noexcept { return std::abs(x); }
    template <Primitive T>
    [[nodiscard]] inline int sign(T x) noexcept { return x < 0 ? -1 : 1; }
    template <Primitive T>
    [[nodiscard]] inline T floor(T x) noexcept { return std::floor(x); }
    template <Primitive T>
    [[nodiscard]] inline T ceil(T x) noexcept { return std::ceil(x); }
    template <Primitive T>
    [[nodiscard]] inline T trunc(T x) noexcept { return std::trunc(x); }
    template <Primitive T>
    [[nodiscard]] inline T round(T x) noexcept { return std::round(x); }
    template <Primitive T>
    [[nodiscard]] inline T roundEven(T x) noexcept { return std::nearbyint(x); }
    template <Primitive T>
    [[nodiscard]] inline T fract(T x) noexcept { return x - std::trunc(x); }
    template <Primitive T, Primitive U>
    [[nodiscard]] inline float mod(T x, U y) noexcept { return std::fmodf(x, y); }
    [[nodiscard]] inline double mod(double x, double y) { return std::fmod(x, y); }
    [[nodiscard]] inline long double mod(long double x, long double y) { return std::fmodl(x, y); }

    template <Primitive T, Primitive U>
    [[nodiscard]] inline float pow(T x, U y) noexcept { return std::powf(x, y); }
    template <Primitive U>
    [[nodiscard]] inline double pow(double x, U y) { return std::pow(x, y); }
    template <Primitive U>
    [[nodiscard]] inline long double pow(long double x, U y) { return std::powl(x, y); }
} // namespace engine::core::math