#pragma once

#include "vec.hpp"

namespace engine::math {

template <Primitive T> class type_vec2 final {
public:
  explicit constexpr type_vec2(T value = 0) { x = y = value; }
  constexpr type_vec2(T x, T y) {
    this->x = x;
    this->y = y;
  }
  [[nodiscard]] constexpr float length() const noexcept {
    return std::sqrt(squared_length());
  }
  [[nodiscard]] constexpr float squared_length() const noexcept {
    return x * x + y * y;
  }

  [[nodiscard]] constexpr type_vec2<T> unit_vector() const noexcept {
    return *this / length();
  }
  constexpr void make_unit_vector() { operator/=(length()); }

  [[nodiscard]] constexpr type_vec2<T> const &operator+() const noexcept {
    return *this;
  }
  [[nodiscard]] constexpr type_vec2<T> operator-() const noexcept {
    return type_vec2<T>{-x, -y};
  }

  template <Primitive U>
  constexpr type_vec2<T> &operator+=(U const value) noexcept {
    x += value;
    y += value;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator-=(U const value) noexcept {
    x -= value;
    y -= value;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator*=(U const value) noexcept {
    x *= value;
    y *= value;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator/=(U const value) noexcept {
    x /= value;
    y /= value;
    return *this;
  }

  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T> operator+(U const value) const noexcept {
    type_vec2<T> return_value{*this};
    return_value += value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T> operator-(U const value) const noexcept {
    type_vec2<T> return_value{*this};
    return_value -= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T> operator*(U const value) const noexcept {
    type_vec2<T> return_value{*this};
    return_value *= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T> operator/(U const value) const noexcept {
    type_vec2<T> return_value{*this};
    return_value /= value;
    return return_value;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator+=(type_vec2<U> const &other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator-=(type_vec2<U> const &other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator*=(type_vec2<U> const &other) noexcept {
    x *= other.x;
    y *= other.y;
    return *this;
  }
  template <Primitive U>
  constexpr type_vec2<T> &operator/=(type_vec2<U> const &other) noexcept {
    x /= other.x;
    y /= other.y;
    return *this;
  }

  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T>
  operator+(type_vec2<U> const &other) const noexcept {
    type_vec2<T> return_value{*this};
    return_value += other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T>
  operator-(type_vec2<U> const &other) const noexcept {
    type_vec2<T> return_value{*this};
    return_value -= other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T>
  operator*(type_vec2<U> const &other) const noexcept {
    type_vec2<T> return_value{*this};
    return_value *= other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] constexpr type_vec2<T>
  operator/(type_vec2<U> const &other) const noexcept {
    type_vec2<T> return_value{*this};
    return_value /= other;
    return return_value;
  }

  T x;
  T y;
};
template <Primitive T>
constexpr std::istream &operator>>(std::istream &is, type_vec2<T> &vec) {
  is >> vec.x >> vec.y;
  return is;
}
template <Primitive T>
constexpr std::ostream &operator>>(std::ostream &os, type_vec2<T> &vec) {
  os << vec.x << " " << vec.y;
  return os;
}

template <Primitive T, Primitive U>
constexpr T dot(type_vec2<T> const &left, type_vec2<U> const &right) {
  return left.x * right.x + left.y * right.y;
}
template <Primitive T, Primitive U>
constexpr T angle(type_vec2<T> const &left, type_vec2<U> const &right) {
  return std::acos(dot(left, right) / left.length() / right.length());
}

template <Primitive T, Primitive U>
[[nodiscard]] constexpr type_vec2<T> operator*(U const value,
                                               type_vec2<T> const &vec) {
  type_vec2<T> return_value{vec};
  return_value *= value;
  return return_value;
}
template <Primitive T, Primitive U>
[[nodiscard]] constexpr type_vec2<T> operator/(U const value,
                                               type_vec2<T> const &vec) {
  type_vec2<T> return_value{vec};
  return_value /= value;
  return return_value;
}

using vec2 = type_vec2<float>;
using dvec2 = type_vec2<double>;
using ivec2 = type_vec2<int>;
using bvec2 = type_vec2<bool>;
}; // namespace engine::math