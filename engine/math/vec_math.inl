#pragma once
#include "vec_math.hpp"
namespace engine::core::math
{
  template <AnyVec U>
  constexpr std::istream &operator>>(std::istream &is, U &vec)
  {
    for (int i = 0; i < U::size; i++)
    {
      is >> vec[i];
    }
    return is;
  }
  template <AnyVec U>
  constexpr std::ostream &operator<<(std::ostream &os, U const &vec)
  {
    for (int i = 0; i < U::size; i++)
    {
      os << vec[i] << " ";
    }
    return os;
  }
  template <AnyVec U>
  constexpr std::ostream &operator<<(std::ostream &os, U &vec)
  {
    for (int i = 0; i < U::size; i++)
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
    return vec<size, T>(vector) += value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, T>(vector) -= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, T>(vector) *= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &vector, U const value) noexcept
  {
    return vec<size, T>(vector) /= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, U const value) noexcept
  {
    return vec<size, T>(left) %= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) %= right;
  }

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, rvec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) %= right;
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
      rv &= left.data[i] == right.data[i];
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
    float return_value = 0;
    for (int i = 0; i < T::size; i++)
    {
      return_value += vector.data[i] * vector.data[i];
    }
    return return_value;
  }
  template <AnyVec T>
  [[nodiscard]] constexpr typename T::type length(T const &vector) noexcept
  {
    return std::sqrt(squared_length(vector));
  }

  template <AnyVec T>
  [[nodiscard]] constexpr vec<T::size, typename T::type> unit_vector(T const &vector) noexcept
  {
    return vector / length(vector);
  }
  template <size_t size, Primitive T>
  [[nodiscard]] constexpr vec<size, T> normalize(vec<size, T> const &vector) noexcept
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
    for (int i = 0; i < return_value.size; i++)
    {
      return_value[i] = core::math::cos(vector[i]);
    }
    return return_value;
  }

  template <AnyVec T>
  constexpr vec<T::size, typename T::type> sin(T const &vector)
  {
    vec<T::size, typename T::type> return_value;
    for (int i = 0; i < return_value.size; i++)
    {
      return_value[i] = core::math::sin(vector[i]);
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
} // namespace engine::core::math