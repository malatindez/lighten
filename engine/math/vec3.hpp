#pragma once

#include "vec.hpp"

namespace engine::math {

template <Primitive T> class type_vec3 final {
public:
  type_vec3(T value = 0) { data_[0] = data_[1] = data_[2] = value; }
  type_vec3(T x, T y, T z) {
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
  }

  [[nodiscard]] inline T x() const noexcept { return data_[0]; }
  [[nodiscard]] inline T y() const noexcept { return data_[1]; }
  [[nodiscard]] inline T z() const noexcept { return data_[2]; }
  [[nodiscard]] inline T r() const noexcept { return data_[0]; }
  [[nodiscard]] inline T g() const noexcept { return data_[1]; }
  [[nodiscard]] inline T b() const noexcept { return data_[2]; }
  [[nodiscard]] inline T operator[](int i) const { return data_[i]; }
  [[nodiscard]] inline T &operator[](int i) { return data_[i]; }

  [[nodiscard]] inline float length() const noexcept {
    return std::sqrt(squared_length());
  }
  [[nodiscard]] inline float squared_length() const noexcept {
    float sum = 0;
    for (int i = 0; i < 3; i++) {
      sum += data_[i] * data_[i];
    }
    return sum;
  }

  [[nodiscard]] inline type_vec3<T> unit_vector() const noexcept {
    return *this / length();
  }
  inline void make_unit_vector() { operator/=(length()); }

  [[nodiscard]] inline type_vec3<T> const &operator+() const noexcept {
    return *this;
  }
  [[nodiscard]] inline type_vec3<T> operator-() const noexcept {
    type_vec3<T> return_value{};
    for (int i = 0; i < 3; i++) {
      return_value[i] = -return_value[i];
    }
    return return_value;
  }

  template <Primitive U>
  inline type_vec3<T> &operator+=(U const value) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] += value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator-=(U const value) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] -= value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator*=(U const value) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] *= value;
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator/=(U const value) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] /= value;
    }
    return *this;
  }

  template <Primitive U>
  [[nodiscard]] inline type_vec3<T> operator+(U const value) const noexcept {
    type_vec3<T> return_value{*this};
    return_value += value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T> operator-(U const value) const noexcept {
    type_vec3<T> return_value{*this};
    return_value -= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T> operator*(U const value) const noexcept {
    type_vec3<T> return_value{*this};
    return_value *= value;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T> operator/(U const value) const noexcept {
    type_vec3<T> return_value{*this};
    return_value /= value;
    return return_value;
  }
  template <Primitive U>
  inline type_vec3<T> &operator+=(type_vec3<U> const &other) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] += other[i];
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator-=(type_vec3<U> const &other) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] -= other[i];
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator*=(type_vec3<U> const &other) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] *= other[i];
    }
    return *this;
  }
  template <Primitive U>
  inline type_vec3<T> &operator/=(type_vec3<U> const &other) noexcept {
    for (int i = 0; i < 3; i++) {
      data_[i] /= other[i];
    }
    return *this;
  }

  template <Primitive U>
  [[nodiscard]] inline type_vec3<T>
  operator+(type_vec3<U> const &other) const noexcept {
    type_vec3<T> return_value{*this};
    return_value += other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T>
  operator-(type_vec3<U> const &other) const noexcept {
    type_vec3<T> return_value{*this};
    return_value -= other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T>
  operator*(type_vec3<U> const &other) const noexcept {
    type_vec3<T> return_value{*this};
    return_value *= other;
    return return_value;
  }
  template <Primitive U>
  [[nodiscard]] inline type_vec3<T>
  operator/(type_vec3<U> const &other) const noexcept {
    type_vec3<T> return_value{*this};
    return_value /= other;
    return return_value;
  }

private:
  T data_[3];
};
template <Primitive T>
inline std::istream &operator>>(std::istream &is, type_vec3<T> &vec) {
  is >> vec.data_[0] >> vec.data_[1] >> vec.data_[2];
  return is;
}
template <Primitive T>
inline std::ostream &operator>>(std::ostream &os, type_vec3<T> &vec) {
  os << vec.data_[0] << " " << vec.data_[1] << " " << vec.data_[2];
  return os;
}

template <Primitive T, Primitive U>
inline T dot(type_vec3<T> const &left, type_vec3<U> const &right) {
  return left.x() * right.x() + left.y() * right.y() + left.z() * right.z();
}
template <Primitive T, Primitive U>
inline T angle(type_vec3<T> const &left, type_vec3<U> const &right) {
  return std::acos(dot(left, right) / left.length() / right.length());
}

template <Primitive T, Primitive U>
inline type_vec3<T> cross(type_vec3<T> const &left, type_vec3<U> const &right) {
  return type_vec3<T>{left.y() * right.z() - left.z() * right.y(),
                      left.x() * right.z() - left.z() * right.x(),
                      left.x() * right.y() - left.y() * right.x()};
}

template <Primitive T, Primitive U>
[[nodiscard]] inline type_vec3<T> operator*(U const value,
                                            type_vec3<T> const &vec) {
  type_vec3<T> return_value{vec};
  return_value *= value;
  return return_value;
}
template <Primitive T, Primitive U>
[[nodiscard]] inline type_vec3<T> operator/(U const value,
                                            type_vec3<T> const &vec) {
  type_vec3<T> return_value{vec};
  return_value /= value;
  return return_value;
}

using vec3 = type_vec3<float>;
using dvec3 = type_vec3<double>;
using ivec3 = type_vec3<int>;
using bvec3 = type_vec3<bool>;
}; // namespace engine::math