#pragma once
#include "vec4.hpp"
namespace engine::math
{
  template <Primitive T>
  constexpr void vec<4, T>::reset() noexcept
  {
    x = y = z = w = 0;
  }

  template <Primitive T>
  [[nodiscard]] constexpr vec<4, T> const &vec<4, T>::operator+() const noexcept
  {
    return *this;
  }
  template <Primitive T>
  [[nodiscard]] constexpr vec<4, T> vec<4, T>::operator-() const noexcept
  {
    return vec<4, T>(-x, -y, -z, -w);
  }

  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator+=(U const value) noexcept
  {
    x += value;
    y += value;
    z += value;
    w += value;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator-=(U const value) noexcept
  {
    x -= value;
    y -= value;
    z -= value;
    w -= value;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator*=(U const value) noexcept
  {
    x *= value;
    y *= value;
    z *= value;
    w *= value;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator/=(U const value) noexcept
  {
    x /= value;
    y /= value;
    z /= value;
    w /= value;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator%=(U const value) noexcept
  {
    x %= value;
    y %= value;
    z %= value;
    w %= value;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator+=(vec<4, U> const &other) noexcept
  {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator-=(vec<4, U> const &other) noexcept
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator*=(vec<4, U> const &other) noexcept
  {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator/=(vec<4, U> const &other) noexcept
  {
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;
    return *this;
  }
  template <Primitive T>
  template <Primitive U>
  constexpr vec<4, T> &vec<4, T>::operator%=(vec<4, U> const &other) noexcept
  {
    x %= other.x;
    y %= other.y;
    z %= other.z;
    w %= other.w;
    return *this;
  }
  template <Primitive T>
  [[nodiscard]] constexpr T &vec<4, T>::operator[](size_t i)
  {
    assert(i < size);
    return data[i];
  }
  template <Primitive T>
  [[nodiscard]] constexpr T const &vec<4, T>::operator[](size_t i) const
  {
    assert(i < size);
    return data[i];
  }

  template <Primitive T>
  template <Primitive _> // primitives
  constexpr size_t vec<4, T>::get_parameter_pack_size()
  {
    return 1;
  }
  template <Primitive T>
  template <class V> // vectors
  constexpr size_t vec<4, T>::get_parameter_pack_size()
  {
    return V::size;
  }
  template <Primitive T>
  template <typename A, typename B, typename... C>
  constexpr size_t vec<4, T>::get_parameter_pack_size()
  {
    return get_parameter_pack_size<A>() + get_parameter_pack_size<B, C...>();
  }

  template <Primitive T>
  template <Primitive U> // primitives
  constexpr void vec<4, T>::unpack_data(int offset, U u)
  {
    data[offset] = static_cast<T>(u);
  }
  template <Primitive T>
  template <class V> // vectors
  constexpr void vec<4, T>::unpack_data(int offset, V vec)
  {
    for (int i = 0; i < V::size; i++)
    {
      data[offset + i] = static_cast<T>(vec[i]);
    }
  }
  template <Primitive T>
  template <typename A, typename B, typename... C>
  constexpr void vec<4, T>::unpack_data(int offset, A a, B b, C... c)
  {
    unpack_data(offset, a);
    offset += get_parameter_pack_size<A>();
    unpack_data(offset, b, c...);
  }
}; // namespace engine::math
