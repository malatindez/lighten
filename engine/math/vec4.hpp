#pragma once

#include "vec.hpp"

namespace engine::math {

template <Primitive T>
class type_vec4 final {
 public:
  type_vec4(T value = 0) { data_[0] = data_[1] = data_[2] = data_[3] = value; }
  type_vec4(T x, T y, T z, T w) {
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = w;
  }

  [[nodiscard]] inline T x() const noexcept { return data_[0]; }
  [[nodiscard]] inline T y() const noexcept { return data_[1]; }
  [[nodiscard]] inline T z() const noexcept { return data_[2]; }
  [[nodiscard]] inline T w() const noexcept { return data_[3]; }
  [[nodiscard]] inline T r() const noexcept { return data_[0]; }
  [[nodiscard]] inline T g() const noexcept { return data_[1]; }
  [[nodiscard]] inline T b() const noexcept { return data_[2]; }
  [[nodiscard]] inline T a() const noexcept { return data_[3]; }
  [[nodiscard]] inline T operator[](int i) const { return data_[i]; }
  [[nodiscard]] inline T& operator[](int i) { return data_[i]; }

  [[nodiscard]] inline float length() const noexcept {
    return std::sqrt(squared_length());
  }
  [[nodiscard]] inline float squared_length() const noexcept {
    float sum = 0;
    for (int i = 0; i < 4; i++) {
      sum += data_[i] * data_[i];
    }
    return sum;
  }

  [[nodiscard]] inline type_vec4<T> unit_vector() const noexcept {
    return *this / length();
  }
  inline void make_unit_vector() { operator/=(length()); }

  [[nodiscard]] inline type_vec4<T> const& operator+() const noexcept {
    return *this;
  }
  [[nodiscard]] inline type_vec4<T> operator-() const noexcept {
    type_vec4<T> return_value{};
    for (int i = 0; i < 4; i++) {
      return_value[i] = -return_value[i];
    }
    return return_value;
  }

  template <Primitive U>
  inline type_vec4<T>& operator+=(U const value) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] += value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec4<T>& operator-=(U const value) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] -= value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec4<T>& operator*=(U const value) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] *= value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec4<T>& operator/=(U const value) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] /= value;
    }
    return *this;
  }

  template <Primitive U>
  [[nodiscard]] inline type_vec4<T> operator+(U const value) const noexcept {
    type_vec4<T> return_value{*this};
    return_value += value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec4<T> operator-(U const value) const noexcept {
    type_vec4<T> return_value{*this};
    return_value -= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec4<T> operator*(U const value) const noexcept {
    type_vec4<T> return_value{*this};
    return_value *= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec4<T> operator/(U const value) const noexcept {
    type_vec4<T> return_value{*this};
    return_value /= value;
    return return_value;
  }
  template <typename U>
  inline type_vec4<T>& operator+=(type_vec4<U> const& other) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] += other[i];
    }
    return *this;
  }
  template <typename U>
  inline type_vec4<T>& operator-=(type_vec4<U> const& other) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] -= other[i];
    }
    return *this;
  }
  template <typename U>
  inline type_vec4<T>& operator*=(type_vec4<U> const& other) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] *= other[i];
    }
    return *this;
  }
  template <typename U>
  inline type_vec4<T>& operator/=(type_vec4<U> const& other) noexcept {
    for (int i = 0; i < 4; i++) {
      data_[i] /= other[i];
    }
    return *this;
  }

  template <typename U>
  [[nodiscard]] inline type_vec4<T>& operator+(
      type_vec4<U> const& other) const noexcept {
    type_vec4<T> return_value{*this};
    return_value += other;
    return return_value;
  }
  template <typename U>
  [[nodiscard]] inline type_vec4<T>& operator-(
      type_vec4<U> const& other) const noexcept {
    type_vec4<T> return_value{*this};
    return_value -= other;
    return return_value;
  }
  template <typename U>
  [[nodiscard]] inline type_vec4<T>& operator*(
      type_vec4<U> const& other) const noexcept {
    type_vec4<T> return_value{*this};
    return_value *= other;
    return return_value;
  }
  template <typename U>
  [[nodiscard]] inline type_vec4<T>& operator/(
      type_vec4<U> const& other) const noexcept {
    type_vec4<T> return_value{*this};
    return_value /= other;
    return return_value;
  }

 protected:
  T data_[4];
};
template <Primitive T>
inline std::istream& operator>>(std::istream& is, type_vec4<T>& vec) {
  is >> vec.data_[0] >> vec.data_[1] >> vec.data_[2] >> vec.data_[3];
  return is;
}
template <Primitive T>
inline std::ostream& operator>>(std::ostream& os, type_vec4<T>& vec) {
  os << vec.data_[0] << " " << vec.data_[1] << " " << vec.data_[2] << " "
     << vec.data_[3];
  return os;
}

template <Primitive T, Primitive U>
inline T dot(const type_vec4<T>& left, const type_vec4<U>& right) {
  return left.x() * left.x() + left.y() * left.y() +
      left.z() * left.z() + left.w() * right.w();
}

using vec4 = type_vec4<float>;
using dvec4 = type_vec4<double>;
using ivec4 = type_vec4<int>;
using bvec4 = type_vec4<bool>;
};  // namespace engine::math