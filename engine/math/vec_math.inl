namespace engine::math
{
  template <size_t size, Primitive T>
  constexpr std::istream &operator>>(std::istream &is, vec<size, T> &vec)
  {
    for (int i = 0; i < size; i++)
    {
      is >> vec[i];
    }
    return is;
  }
  template <size_t size, Primitive T>
  constexpr std::ostream &operator<<(std::ostream &os, vec<size, T> &vec)
  {
    for (int i = 0; i < size; i++)
    {
      os << vec[i] << " ";
    }
    return os;
  }

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator*(U const value,
                                                 vec<size, T> const &vector)
  {
    return vec<size, T>(vector) *= value;
  }

  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector,
                                                 U const value) noexcept
  {
    return vec<size, T>(vector) += value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector,
                                                 U const value) noexcept
  {
    return vec<size, T>(vector) -= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector,
                                                 U const value) noexcept
  {
    return vec<size, T>(vector) *= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector,
                                                 U const value) noexcept
  {
    return vec<size, T>(vector) /= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T>
  operator+(vec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) += right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T>
  operator-(vec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) -= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T>
  operator*(vec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) *= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T>
  operator/(vec<size, T> const &left, vec<size, U> const &right) noexcept
  {
    return vec<size, T>(left) /= right;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const& left, U const value) noexcept
  {
      return vec<size, T>(left) %= value;
  }
  template <size_t size, Primitive T, Primitive U>
  [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const& left, vec<size, U> const& right) noexcept
  {
      return vec<size, T>(left) %= right;
  }

  template <size_t size, Primitive T>
  [[nodiscard]] constexpr float
  squared_length(vec<size, T> const &vector) noexcept
  {
    float return_value = 0;
    for (int i = 0; i < size; i++)
    {
      return_value += vector.data[i] * vector.data[i];
    }
    return return_value;
  }
  template <size_t size, Primitive T>
  [[nodiscard]] constexpr float length(vec<size, T> const &vector) noexcept
  {
    return std::sqrt(squared_length(vector));
  }

  template <size_t size, Primitive T>
  [[nodiscard]] constexpr vec<size, T>
  unit_vector(vec<size, T> const &vector) noexcept
  {
    return vector / length(vector);
  }
  template <size_t size, Primitive T>
  [[nodiscard]] constexpr vec<size, T>
  normalize(vec<size, T> const &vector) noexcept
  {
    return vector / length(vector);
  }

  template <size_t size, Primitive T, Primitive U>
  constexpr T dot(vec<size, T> const &left, vec<size, U> const &right)
  {
    T return_value = 0;
    for (size_t i = 0; i < size; i++)
    {
      return_value += left[i] * static_cast<T>(right[i]);
    }
    return return_value;
  }

  template <size_t size, Primitive T, Primitive U>
  constexpr T angle(vec<size, T> const &left, vec<size, U> const &right)
  {
    return std::acos(dot(left, right) / left.length() / right.length());
  }

  template <size_t size, Primitive T>
  constexpr vec<size, T> cos(vec<size, T> const &vector)
  {
    vec<size, T> return_value;
    for (int i = 0; i < return_value.size; i++)
    {
      return_value[i] = math::cos(vector[i]);
    }
    return return_value;
  }

  template <size_t size, Primitive T>
  constexpr vec<size, T> sin(vec<size, T> const &vector)
  {
    vec<size, T> return_value;
    for (int i = 0; i < return_value.size; i++)
    {
      return_value[i] = math::sin(vector[i]);
    }
    return return_value;
  }
} // namespace engine::math