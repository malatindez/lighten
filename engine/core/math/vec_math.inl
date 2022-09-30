#pragma once
#include "vec_math.hpp"
namespace engine::core::math
{
    template <AnyVec U>
    constexpr std::istream &operator>>(std::istream &is, U &vec)
    {
        for (size_t i = 0; i < U::size; i++)
        {
            is >> vec[i];
        }
        return is;
    }
    template <AnyVec U>
    constexpr std::ostream &operator<<(std::ostream &os, U const &vec)
    {
        for (size_t i = 0; i < U::size; i++)
        {
            os << vec[i] << " ";
        }
        return os;
    }
    template <AnyVec U>
    constexpr std::ostream &operator<<(std::ostream &os, U &vec)
    {
        for (size_t i = 0; i < U::size; i++)
        {
            os << vec[i] << " ";
        }
        return os;
    }

    template <size_t n, Primitive U, AnyVec Vector>
    [[nodiscard]] constexpr vec<n, U> as_vec(Vector const &v) noexcept requires(n >= 2 && n <= Vector::size)
    {
        vec<n, U> rv;
        for (size_t i = 0; i < n; i++)
        {
            rv.data[i] = static_cast<U>(v.data[i]);
        }
        return rv;
    }

    template <size_t n, AnyVec Vector>
    [[nodiscard]] constexpr vec<n, std::remove_const_t<typename Vector::type>> as_vec(Vector const &v) noexcept requires(n >= 2 && n <= Vector::size)
    {
        return as_vec<n, std::remove_const_t<typename Vector::type>>(v);
    }

    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> as_vec(Vector const &v) noexcept
    {
        return as_vec<Vector::size, std::remove_const_t<typename Vector::type>>(v);
    }
    template <size_t n, AnyVec Vector>
    [[nodiscard]] constexpr vec<n, typename Vector::type> &as_rvec(Vector &v) noexcept requires(n >= 2 && n <= Vector::size)
    {
        return reinterpret_cast<vec<n, typename Vector::type> &>(v);
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, typename Vector::type> &as_rvec(Vector &v) noexcept
    {
        return as_rvec<Vector::size>(v);
    }

    template <size_t n, AnyVec Vector>
    [[nodiscard]] constexpr vec<n, typename Vector::type> const &as_crvec(Vector const &v) noexcept requires(n >= 2 && n <= Vector::size)
    {
        return reinterpret_cast<vec<n, typename Vector::type> const &>(v);
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, typename Vector::type> const &as_crvec(Vector const &v) noexcept
    {
        return as_rvec<Vector::size>(v);
    }

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, T>(vector) += value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, T>(vector) -= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, T>(vector) *= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, T>(vector) /= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(U const value, vec<size, T> const &vector) noexcept
    {
        return vec<size, T>(vector) *= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(U const value, vec<size, T> const &vector) noexcept
    {
        return vec<size, T>(value) /= vector;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(U const value, vec<size, T> const &vector) noexcept
    {
        return vec<size, T>(vector) += value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(U const value, vec<size, T> const &vector) noexcept
    {
        return vec<size, T>(-vector) += value;
    }

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) += right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) -= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) *= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) /= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, U const value) noexcept
    {
        return vec<size, T>(left) %= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) %= right;
    }

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(U const value, _detail::rvec<size, T> const &vector) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) *= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(U const value, _detail::rvec<size, T> const &vector) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) += value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(U const value, _detail::rvec<size, T> const &vector) noexcept
    {
        return vec<size, std::remove_const_t<T>>(-vector) += value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(_detail::rvec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) += value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(_detail::rvec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) -= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(_detail::rvec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) *= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(_detail::rvec<size, T> const &vector, U const value) noexcept
    {
        return vec<size, std::remove_const_t<T>>(vector) /= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(_detail::rvec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) += right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(_detail::rvec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) -= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(_detail::rvec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) *= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(_detail::rvec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) /= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(_detail::rvec<size, T> const &left, U const value) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) %= value;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(_detail::rvec<size, T> const &left, vec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) %= right;
    }

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(_detail::rvec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) += right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(_detail::rvec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) -= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(_detail::rvec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) *= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(_detail::rvec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) /= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(_detail::rvec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, std::remove_const_t<T>>(left) %= right;
    }

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) += right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) -= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) *= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) /= right;
    }
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, _detail::rvec<size, U> const &right) noexcept
    {
        return vec<size, T>(left) %= right;
    }

    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr bool operator==(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        bool rv = true;
        for (size_t i = 0; i < T::size; i++)
        {
            rv = rv && (left.data[i] == right.data[i]);
        }
        return rv;
    }

    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr bool operator!=(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        return !(left == right);
    }
    template <AnyVec T>
    [[nodiscard]] constexpr std::remove_const_t<typename T::type> squared_length(T const &vector) noexcept
    {
        std::remove_const_t<typename T::type> return_value = 0;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value += vector.data[i] * vector.data[i];
        }
        return return_value;
    }
    template <AnyVec T>
    [[nodiscard]] constexpr auto length(T const &vector) noexcept
    {
        return sqrt(squared_length(vector));
    }

    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> unit_vector(T const &vector) noexcept
    {
        return vector / length(vector);
    }
    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> normalize(T const &vector) noexcept
    {
        return vector / length(vector);
    }

    template <AnyVec T, AnyVec U>
    constexpr std::remove_const_t<typename T::type> dot(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        std::remove_const_t<typename T::type> return_value = 0;
        auto const temp = left * right;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value += temp[i];
        }
        return return_value;
    }

    template <AnyVec T, AnyVec U>
    constexpr typename std::remove_const_t<typename T::type> angle(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        return acos(clamp(dot(left, right), typename T::type(-1), typename T::type(1)));
    }

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> cos(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = cos(vector[i]);
        }
        return return_value;
    }

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sin(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = sin(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> tan(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = tan(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> acos(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = acos(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> asin(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = asin(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> atan(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = atan(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T, AnyVec U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> atan(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = atan2(left[i], right[i]);
        }
        return return_value;
    }
    template <AnyVec T, AnyVec U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> atan2(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        return atan(left, right);
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sinh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = sinh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> cosh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = cosh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> tanh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = tanh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> asinh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = asinh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> acosh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = acosh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> atanh(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = atanh(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> exp(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = exp(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> exp2(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = exp2(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> expm1(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = expm1(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> log(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = log(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> log2(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = log2(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> log10(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = log10(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> log1p(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = log1p(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> logb(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = logb(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sqrt(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = sqrt(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> inversesqrt(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = inversesqrt(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> abs(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = abs(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sign(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = static_cast<typename T::type>(sign(vector[i]));
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> floor(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = floor(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> ceil(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = ceil(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> trunc(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = trunc(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> round(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = round(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> roundEven(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = roundEven(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> fract(T const &vector) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = fract(vector[i]);
        }
        return return_value;
    }
    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> mod(T const &vector, typename T::type const &scalar) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = mod(vector[i], scalar);
        }
        return return_value;
    }
    template <AnyVec T, AnyVec U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> mod(T const &left, U const &right) noexcept requires(T::size == U::size)
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = mod(left[i], right[i]);
        }
        return return_value;
    }

    template <AnyVec T, Primitive U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> pow(T const &vector, U const value) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = pow(vector[i], value);
        }
        return return_value;
    }
    template <AnyVec T, AnyVec U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> pow(T const &vector1, U const &vector2) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size; i++)
        {
            return_value[i] = pow(vector1[i], vector2[i]);
        }
        return return_value;
    }

    template <AnyVec T, AnyVec U>
    constexpr vec<3, std::remove_const_t<typename T::type>> cross(T const &left, U const &right) noexcept requires(T::size == U::size && T::size == 3)
    {
        return vec<3, std::remove_const_t<typename T::type>>{left.y *right.z - left.z * right.y,
            left.x *right.z - left.z * right.x,
            left.x *right.y - left.y * right.x};
    }

    template <Primitive T, Primitive U>
    constexpr void rclamp(T &left, U const min, U const max) noexcept
    {
        if (left < min)
            left = static_cast<T>(min);
        else if (left > max)
            left = static_cast<T>(max);
    }

    template <AnyVec T, Primitive U>
    constexpr void rclamp(T &left, U const min, U const max) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            if (left[i] < static_cast<typename T::type>(min))
            {
                left[i] = min;
            }
            else if (left[i] > static_cast<typename T::type>(max))
            {
                left[i] = max;
            }
        }
    }
    template <AnyVec T, AnyVec U>
    constexpr void rclamp(T &left, U const min, U const max) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            if (left[i] < static_cast<typename T::type>(min[i]))
            {
                left[i] = min[i];
            }
            else if (left[i] > static_cast<typename T::type>(max[i]))
            {
                left[i] = max[i];
            }
        }
    }
    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> clamp(T const &left, U const min, U const max) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rvec;

        for (size_t i = 0; i < T::size; i++)
        {
            if (left[i] < static_cast<std::remove_const_t<typename T::type>>(min))
            {
                rvec[i] = static_cast<std::remove_const_t<typename T::type>>(min);
            }
            else if (left[i] > static_cast<std::remove_const_t<typename T::type>>(max))
            {
                rvec[i] = static_cast<std::remove_const_t<typename T::type>>(max);
            }
            else
            {
                rvec[i] = left[i];
            }
        }
        return rvec;
    }
    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> clamp(T const &left, U const &min, U const &max) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rvec;
        for (size_t i = 0; i < T::size; i++)
        {
            if (left[i] < static_cast<std::remove_const_t<typename T::type>>(min[i]))
            {
                rvec[i] = static_cast<std::remove_const_t<typename T::type>>(min[i]);
            }
            else if (left[i] > static_cast<std::remove_const_t<typename T::type>>(max[i]))
            {
                rvec[i] = static_cast<std::remove_const_t<typename T::type>>(max[i]);
            }
            else
            {
                rvec[i] = left[i];
            }
        }
        return rvec;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr T clamp(T left, U const min, U const max) noexcept
    {
        if (left < min)
            return min;
        if (left > max)
            return max;
        return left;
    }
    template <typename T, Primitive U>
    constexpr auto lerp(T const from, T const to, U const param) noexcept
    {
        return from * (1 - param) + to * param;
    }

    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> min(T const &left, U const max) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rv;
        for (size_t i = 0; i < T::size; i++)
        {
            rv[i] = std::min(left[i], static_cast<typename T::type>(max));
        }
        return rv;
    }

    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> max(T const &left, U const min) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rv;
        for (size_t i = 0; i < T::size; i++)
        {
            rv[i] = std::max(left[i], static_cast<typename T::type>(min));
        }
        return rv;
    }

    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> min(T const &left, T const &right) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rv;
        for (size_t i = 0; i < T::size; i++)
        {
            rv[i] = std::min(left[i], static_cast<typename T::type>(right[i]));
        }
        return rv;
    }

    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> max(T const &left, T const &right) noexcept
    {
        vec<T::size, std::remove_const_t<typename T::type>> rv;
        for (size_t i = 0; i < T::size; i++)
        {
            rv[i] = std::max(left[i], static_cast<typename T::type>(right[i]));
        }
        return rv;
    }

    template <AnyVec T>
    constexpr void rmin(T &left, T const &max) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            left[i] = std::min(left[i], static_cast<typename T::type>(max[i]));
        }
    }

    template <AnyVec T>
    constexpr void rmax(T &left, T const &min) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            left[i] = std::max(left[i], static_cast<typename T::type>(min[i]));
        }
    }

    template <AnyVec T, Primitive U>
    constexpr void rmin(T &left, U const max) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            left[i] = std::min(left[i], static_cast<typename T::type>(max));
        }
    }

    template <AnyVec T, Primitive U>
    constexpr void rmax(T &left, U const min) noexcept
    {
        for (size_t i = 0; i < T::size; i++)
        {
            left[i] = std::max(left[i], static_cast<typename T::type>(min));
        }
    }

    template <Primitive T, Primitive U>
    constexpr void rmin(T &left, U const max) noexcept
    {
        left = left < max ? left : max;
    }

    template <Primitive T, Primitive U>
    constexpr void rmax(T &left, U const min) noexcept
    {
        left = left > min ? left : min;
    }
    template <AnyVec T>
    [[nodiscard]] constexpr std::remove_const_t<typename T::type> distance(T const &left, T const &right) noexcept
    {
        return length(left - right);
    }
    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> lerp(T const &from, T const &to, U const param) noexcept
    {
        return from * (1 - param) + to * param;
    }
    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> mix(T const &from, T const &to, U const param) noexcept
    {
        return from * (1 - param) + to * param;
    }
    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> mix(T const &from, T const &to, U const &param) noexcept
    {
        return from * (1 - param) + to * param;
    }
} // namespace engine::core::math