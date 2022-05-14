#pragma once
#include "matnxn.hpp"

namespace engine::math
{

  // sets all values to zero
  template <size_t a, size_t b, Primitive T>
  constexpr void mat<a, b, T>::reset() noexcept
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i].reset();
    }
  }

  template <size_t a, size_t b, Primitive T>
  [[nodiscard]] constexpr vec<b, T> &mat<a, b, T>::operator[](size_t i)
  {
    assert(i < size.x);
    return data[i];
  }
  template <size_t a, size_t b, Primitive T>
  [[nodiscard]] constexpr vec<b, T> const &
  mat<a, b, T>::operator[](size_t i) const
  {
    assert(i < size.x);
    return data[i];
  }

  template <size_t a, size_t b, Primitive T>
  [[nodiscard]] constexpr mat<a, b, T> const &
  mat<a, b, T>::operator+() const noexcept
  {
    return *this;
  }

  template <size_t a, size_t b, Primitive T>
  [[nodiscard]] constexpr mat<a, b, T> mat<a, b, T>::operator-() const noexcept
  {
    mat<a, b, T> return_value;
    for (int i = 0; i < size.x; i++)
    {
      return_value.data[i] = -data[i];
    }
    return return_value;
  }

  template <size_t a, size_t b, Primitive T>
  template <Primitive U>
  constexpr mat<a, b, T> &mat<a, b, T>::operator+=(mat<a, b, U> const &other)
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i] += other.data[i];
    }
    return *this;
  }
  template <size_t a, size_t b, Primitive T>
  template <Primitive U>
  constexpr mat<a, b, T> &mat<a, b, T>::operator-=(mat<a, b, U> const &other)
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i] -= other.data[i];
    }
    return *this;
  }

  template <size_t a, size_t b, Primitive T>
  template <size_t c, Primitive U>
  constexpr mat<a, c, T> &mat<a, b, T>::operator*=(mat<b, c, U> const &other)
  {
    return (*this = *this * other);
  }

  template <size_t a, size_t b, Primitive T>
  template <Primitive U>
  constexpr mat<a, b, T> &mat<a, b, T>::operator+=(U const value)
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i] += value;
    }
    return *this;
  }
  template <size_t a, size_t b, Primitive T>
  template <Primitive U>
  constexpr mat<a, b, T> &mat<a, b, T>::operator-=(U const value)
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i] -= value;
    }
    return *this;
  }
  template <size_t a, size_t b, Primitive T>
  template <Primitive U>
  constexpr mat<a, b, T> &mat<a, b, T>::operator*=(U const value)
  {
    for (int i = 0; i < size.x; i++)
    {
      data[i] *= value;
    }
    return *this;
  }

  template <size_t a, size_t b, Primitive T>
  template <Primitive _> // primitives
  constexpr size_t mat<a, b, T>::get_parameter_pack_size()
  {
    return 1;
  }
  template <size_t a, size_t b, Primitive T>
  template <class V> // vectors
  constexpr size_t mat<a, b, T>::get_parameter_pack_size()
  {
    return V::size;
  }
  template <size_t a, size_t b, Primitive T>
  template <typename A, typename B, typename... C>
  constexpr size_t mat<a, b, T>::get_parameter_pack_size()
  {
    return get_parameter_pack_size<A>() + get_parameter_pack_size<B, C...>();
  }

  template <size_t a, size_t b, Primitive T>
  template <Primitive U> // primitives
  constexpr void mat<a, b, T>::unpack_data(int offset, U u)
  {
    arr[offset] = static_cast<T>(u);
  }
  template <size_t a, size_t b, Primitive T>
  template <class V> // vectors
  constexpr void mat<a, b, T>::unpack_data(int offset, V vec)
  {
    for (int i = 0; i < V::size; i++)
    {
      arr[offset + i] = static_cast<T>(vec[i]);
    }
  }
  template <size_t a, size_t b, Primitive T>
  template <typename A, typename B, typename... C>
  constexpr void mat<a, b, T>::unpack_data(int offset, A first, B second,
                                           C... rest)
  {
    unpack_data(offset, first);
    offset += get_parameter_pack_size<A>();
    unpack_data(offset, second, rest...);
  }
} // namespace engine::math