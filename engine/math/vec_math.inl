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

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator*(U const value, vec<size, T> const &vector)
  {
    return vec<size, T>(vector) *= value;
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
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(U const value, rvec<size, T> const &vector)
  {
    return vec<size, std::remove_const_t<T>>(vector) *= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, std::remove_const_t<T>>(vector) += value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, std::remove_const_t<T>>(vector) -= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, std::remove_const_t<T>>(vector) *= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, std::remove_const_t<T>>(vector) /= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, U const value) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) %= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) %= right;
  }

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, std::remove_const_t<T>>(left) %= right;
  }

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) %= right;
  }

  template <AnyVec T, AnyVec U>
  [[nodiscard]] constexpr bool operator==(T const &left, U const &right) requires(T::size == U::size)
  {
    bool rv = true;
    for (size_t i = 0; i < T::size; i++)
    {
      rv = rv && (left.data[i] == right.data[i]);
    }
    return rv;
  }

  template <AnyVec T, AnyVec U>
  [[nodiscard]] constexpr bool operator!=(T const &left, U const &right) requires(T::size == U::size)
  {
    return !(left == right);
  }
  template <AnyVec T>
  [[nodiscard]] constexpr typename T::type squared_length(T const &vector) noexcept
  {
    typename T::type return_value = 0;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value += vector.data[i] * vector.data[i];
    }
    return return_value;
  }
  template <AnyVec T>
  [[nodiscard]] constexpr auto length(T const &vector) noexcept
  {
    return math::sqrt(squared_length(vector));
  }

  template <AnyVec T>
  [[nodiscard]] constexpr vec<T::size, typename T::type> unit_vector(T const &vector) noexcept
  {
    return vector / length(vector);
  }
  template <AnyVec T>
  [[nodiscard]] constexpr vec<T::size, typename T::type> normalize(T const &vector) noexcept
  {
    return vector / length(vector);
  }

  template <AnyVec T, AnyVec U>
  constexpr typename T::type dot(T const &left, U const &right) requires(T::size == U::size)
  {
    typename T::type return_value = 0;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value += left[i] * static_cast<typename T::type>(right[i]);
    }
    return return_value;
  }
  template <AnyVec T, AnyVec U>
  constexpr typename T::type angle(T const &left, U const &right) requires(T::size == U::size)
  {
    return std::acos(dot(left, right) / length(left) / length(right));
  }

  template <AnyVec T>
  constexpr vec<T::size, typename T::type> cos(T const &vector)
  {
    vec<T::size, typename T::type> return_value;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value[i] = core::math::cos(vector[i]);
    }
    return return_value;
  }

  template <AnyVec T>
  constexpr vec<T::size, typename T::type> sin(T const &vector)
  {
    vec<T::size, typename T::type> return_value;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value[i] = core::math::sin(vector[i]);
    }
    return return_value;
  }

  template <AnyVec T, Primitive U>
  constexpr vec<T::size, typename T::type> pow(T const &vector, U const value)
  {
    vec<T::size, typename T::type> return_value;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value[i] = core::math::pow(vector[i], value);
    }
    return return_value;
  }

  template <AnyVec T, Primitive U>
  constexpr vec<T::size, typename T::type> sqrt(T const &vector)
  {
    vec<T::size, typename T::type> return_value;
    for (size_t i = 0; i < T::size; i++)
    {
      return_value[i] = core::math::sqrt(vector[i]);
    }
    return return_value;
  }

  template <AnyVec T, AnyVec U>
  constexpr vec<3, T> cross(T const &left, U const &right) requires(T::size == U::size && T::size == 3)
  {
    return vec<3, T>{left.y * right.z - left.z * right.y,
                     left.x * right.z - left.z * right.x,
                     left.x * right.y - left.y * right.x};
  }

  template <Primitive T, Primitive U>
  constexpr void rclamp(T &left, U const min, U const max)
  {
    if (left < min)
      left = min;
    if (left > max)
      left = max;
  }

  template <AnyVec T, Primitive U>
  constexpr void rclamp(T &left, U const min, U const max)
  {
    for (size_t i = 0; i < T::size; i++)
    {
      if (left[i] < static_cast<typename T::type>(min))
      {
        left[i] = min;
      }
      if (left[i] > static_cast<typename T::type>(max))
      {
        left[i] = max;
      }
    }
  }
  template <AnyVec T, Primitive U>
  [[nodiscard]] constexpr vec<T::size, typename T::type> clamp(T const &left, U const min, U const max)
  {
    vec<T::size, typename T::type> rvec{left};

    for (size_t i = 0; i < T::size; i++)
    {
      if (left[i] < static_cast<typename T::type>(min))
      {
        rvec[i] = min;
      }
      if (left[i] > static_cast<typename T::type>(max))
      {
        rvec[i] = max;
      }
      else
      {
        rvec[i] = left[i];
      }
    }
    return rvec;
  }
  template <Primitive T, Primitive U>
  [[nodiscard]] constexpr T clamp(T left, U const min, U const max)
  {
    if (left < min)
      return min;
    if (left > max)
      return max;
    return left;
  }
} // namespace engine::core::math