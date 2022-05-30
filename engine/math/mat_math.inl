#pragma once
#include "mat_math.hpp"
namespace engine::math
{
  template <size_t a, size_t b, Primitive T>
  constexpr std::istream &operator>>(std::istream &is, mat<a, b, T> matrix)
  {
    for (int i = 0; i < a; i++)
    {
      for (int j = 0; j < b; j++)
      {
        is >> matrix[i][j];
      }
    }
    return is;
  }
  template <size_t a, size_t b, Primitive T>
  constexpr std::ostream &operator<<(std::ostream &os, mat<a, b, T> matrix)
  {
    for (int i = 0; i < a; i++)
    {
      for (int j = 0; j < b; j++)
      {
        os << matrix[i][j] << " ";
      }
      os << std::endl;
    }
    return os;
  }

  template <size_t a, size_t b, size_t c, Primitive T, Primitive U>
  constexpr mat<a, c, T> operator*(mat<a, b, T> const &left,
                                   mat<b, c, U> const &right)
  {
    mat<a, c, T> return_value;
    for (size_t i = 0; i < a; i++)
    {
      for (size_t j = 0; j < c; j++)
      {
        return_value[i][j] = 0;
        for (size_t k = 0; k < b; k++)
        {
          return_value[i][j] += left[i][k] * right[k][j];
        }
      }
    }
    return return_value;
  }
  template <size_t b, size_t c, Primitive T, Primitive U>
  constexpr vec<b, T> operator*(vec<b, T> const &left,
                                mat<b, c, U> const &right)
  {
    vec<b, T> return_value;
    for (size_t j = 0; j < c; j++)
    {
      return_value[j] = 0;
      for (size_t k = 0; k < b; k++)
      {
        return_value[j] += left[k] * right[k][j];
      }
    }
    return return_value;
  }
  template <size_t a, size_t b, Primitive T, Primitive U>
  constexpr mat<a, b, T> operator*(mat<a, b, T> const &left, U const right)
  {
    return mat<a, b, T>(left) *= right;
  }
  template <size_t a, size_t b, Primitive T, Primitive U>
  constexpr mat<a, b, T> operator*(U const left, mat<a, b, T> const &right)
  {
    return mat<a, b, T>(right) *= left;
  }

  template <size_t a, size_t b, Primitive T>
  constexpr mat<b, a, T> transpose(mat<a, b, T> const &matrix)
  {
    mat<b, a, T> return_value;
    for (int i = 0; i < a; i++)
    {
      for (int j = 0; j < b; j++)
      {
        return_value[j][i] = matrix[i][j];
      }
    }
    return return_value;
  }
  template <Primitive T>
  constexpr T det(mat<2, 2, T> const &m)
  {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
  }
  template <Primitive T>
  constexpr T det(mat<3, 3, T> const &m)
  {
    return +m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
           m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
           m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
  }
  template <Primitive T>
  constexpr T det(mat<4, 4, T> const &m)
  {
    T const sub_det_00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    T const sub_det_01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    T const sub_det_02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    T const sub_det_03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    T const sub_det_04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    T const sub_det_05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

    vec<4, T> sub_det(
        +(m[1][1] * sub_det_00 - m[1][2] * sub_det_01 + m[1][3] * sub_det_02),
        -(m[1][0] * sub_det_00 - m[1][2] * sub_det_03 + m[1][3] * sub_det_04),
        +(m[1][0] * sub_det_01 - m[1][1] * sub_det_03 + m[1][3] * sub_det_05),
        -(m[1][0] * sub_det_02 - m[1][1] * sub_det_04 + m[1][2] * sub_det_05));

    return m[0][0] * sub_det[0] + m[0][1] * sub_det[1] + m[0][2] * sub_det[2] +
           m[0][3] * sub_det[3];
  }

  template <Primitive T>
  constexpr mat<2, 2, T> adj(mat<2, 2, T> const &m)
  {
    return mat<2, 2, T>(+m[1][1], -m[0][1], -m[1][0], +m[0][0]);
  }

  template <size_t m_size, Primitive T>
  constexpr mat<m_size, m_size, T> adjugate(mat<m_size, m_size, T> const &m)
  {
    mat<m_size, m_size, T> return_value;
    mat<m_size - 1, m_size - 1, T> temp;
    int sign = 1;
    for (int i = 0; i < m_size; i++)
    {
      for (int j = 0; j < m_size; j++)
      {
        // create 3x3 matrix using m without column i and row j
        for (int k = 0, s = 0; k < m_size; k++)
        {
          if (k == j)
            continue;
          for (int l = 0, t = 0; l < m_size; l++)
          {
            if (l == i)
              continue;
            temp[s][t] = m[k][l];
            t++;
          }
          s++;
        }
        return_value[i][j] = sign * det(temp);
        sign *= -1;
      }
      sign *= -1;
    }
    return return_value;
  }

  template <size_t a, Primitive T>
  constexpr mat<a, a, T> inverse(mat<a, a, T> const &m)
  {
    return adjugate(m) * (static_cast<T>(1) / det(m));
  }

  template <Primitive T>
  constexpr mat<4, 4, T> translate(mat<4, 4, T> const &matrix, vec<3, T> const &vec)
  {
    mat<4, 4, T> return_value;
    return_value[0] = matrix[0];
    return_value[1] = matrix[1];
    return_value[2] = matrix[2];
    return_value[3] =
        matrix[0] * vec[0] + matrix[1] * vec[1] + matrix[2] * vec[2] + matrix[3];
    return return_value;
  }

  template <Primitive T>
  constexpr mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, T angle,
                                vec<3, T> const &vector)
  {
    T const c = math::cos(angle);
    T const s = math::sin(angle);
    vec<3, T> axis = normalize(vector);
    vec<3, T> temp = (T(1) - c) * axis;
    mat<4, 4, T> rotate;

    rotate[0][0] = c + temp[0] * axis[0];
    rotate[0][1] = temp[0] * axis[1] + s * axis[2];
    rotate[0][2] = temp[0] * axis[2] - s * axis[1];

    rotate[1][0] = temp[1] * axis[0] - s * axis[2];
    rotate[1][1] = c + temp[1] * axis[1];
    rotate[1][2] = temp[1] * axis[2] + s * axis[0];

    rotate[2][0] = temp[2] * axis[0] + s * axis[1];
    rotate[2][1] = temp[2] * axis[1] - s * axis[0];
    rotate[2][2] = c + temp[2] * axis[2];

    mat<4, 4, T> result(1);
    result[0] = matrix[0] * rotate[0][0] + matrix[1] * rotate[0][1] +
                matrix[2] * rotate[0][2];
    result[1] = matrix[0] * rotate[1][0] + matrix[1] * rotate[1][1] +
                matrix[2] * rotate[1][2];
    result[2] = matrix[0] * rotate[2][0] + matrix[1] * rotate[2][1] +
                matrix[2] * rotate[2][2];
    result[3] = matrix[3];
    return result;
  }

  template <Primitive T>
  constexpr mat<4, 4, T> scale(mat<4, 4, T> const &matrix, vec<3, T> const &scale)
  {
    mat<4, 4, T> return_value;
    return_value[0] = matrix[0] * scale[0];
    return_value[1] = matrix[1] * scale[1];
    return_value[2] = matrix[2] * scale[2];
    return_value[3] = matrix[3];
    return return_value;
  }

  template <Primitive T>
  constexpr mat<4, 4, T> lookAt(vec<3, T> const &eye, vec<3, T> const &center,
                                vec<3, T> const &world_up)
  {
    vec<3, T> const forward = normalize(center - eye);
    vec<3, T> const right = normalize(cross(forward, world_up));
    vec<3, T> const up = cross(forward, right);
    mat<4, 4, T> return_value(1);
    return_value[0][0] = right.x;
    return_value[1][0] = right.y;
    return_value[2][0] = right.z;
    return_value[0][1] = up.x;
    return_value[1][1] = up.y;
    return_value[2][1] = up.z;
    return_value[0][2] = -forward.x;
    return_value[1][2] = -forward.y;
    return_value[2][2] = -forward.z;
    return_value[3][0] = -dot(right, eye);
    return_value[3][1] = -dot(up, eye);
    return_value[3][2] = -dot(forward, eye);
    return return_value;
  }
  template <Primitive T>
  constexpr mat<4, 4, T> perspective(T fov_y, T aspect_ratio, T z_near, T z_far) requires(!std::numeric_limits<T>::is_integer)
  {
    assert(std::abs(aspect_ratio - std::numeric_limits<T>::epsilon()) >
           static_cast<T>(0));

    T const tan_half_fov_y = tan(fov_y / static_cast<T>(2));

    mat<4, 4, T> return_value(0);
    return_value[0][0] = static_cast<T>(1) / (aspect_ratio * tan_half_fov_y);
    return_value[1][1] = static_cast<T>(1) / (tan_half_fov_y);
    return_value[2][2] = z_far / (z_far - z_near);
    return_value[2][3] = static_cast<T>(1);
    return_value[3][2] = -(z_far * z_near) / (z_far - z_near);
    return return_value;
  }

  template <Primitive T>
  constexpr void invert_orthonormal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
  {
    dst[0][0] = src[2][2];
    dst[1][1] = src[1][1];
    dst[2][2] = src[0][0];
    dst[0][1] = src[1][0];
    dst[1][0] = src[0][1];
    dst[0][2] = src[2][0];
    dst[2][0] = src[0][2];
    dst[1][2] = src[2][1];
    dst[2][1] = src[1][2];
    dst[3][0] = -src[3].x * dst.data[0][0] - src[3].x * dst.data[0][1] - src[3].x * dst.data[0][2];
    dst[3][1] = -src[3].y * dst.data[1][0] - src[3].y * dst.data[1][1] - src[3].y * dst.data[1][2];
    dst[3][2] = -src[3].z * dst.data[2][0] - src[3].z * dst.data[2][1] - src[3].z * dst.data[2][2];
    dst[0][3] = 0;
    dst[1][3] = 0;
    dst[2][3] = 0;
    dst[3][3] = 1;
  }
  template <Primitive T>
  constexpr void invert_orthogonal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
  {
    dst[0][0] = src[2][2];
    dst[1][1] = src[1][1];
    dst[2][2] = src[0][0];
    dst[0][1] = src[1][0];
    dst[1][0] = src[0][1];
    dst[0][2] = src[2][0];
    dst[2][0] = src[0][2];
    dst[1][2] = src[2][1];
    dst[2][1] = src[1][2];

    vec3 lengths{
        std::sqrt(dst[0][0] * dst[0][0] + dst[0][1] * dst[0][1] + dst[0][2] * dst[0][2]),
        std::sqrt(dst[1][0] * dst[1][0] + dst[1][1] * dst[1][1] + dst[1][2] * dst[1][2]),
        std::sqrt(dst[2][0] * dst[2][0] + dst[2][1] * dst[2][1] + dst[2][2] * dst[2][2]),
    };

    dst[0][0] = 1.0f / (dst[0][0] * lengths[0]);
    dst[0][1] = 1.0f / (dst[0][1] * lengths[0]);
    dst[0][2] = 1.0f / (dst[0][2] * lengths[0]);
    dst[1][0] = 1.0f / (dst[1][0] * lengths[1]);
    dst[1][1] = 1.0f / (dst[1][1] * lengths[1]);
    dst[1][2] = 1.0f / (dst[1][2] * lengths[1]);
    dst[2][0] = 1.0f / (dst[2][0] * lengths[2]);
    dst[2][1] = 1.0f / (dst[2][1] * lengths[2]);
    dst[2][2] = 1.0f / (dst[2][2] * lengths[2]);

    dst[3][0] =
        -src[3].x * dst.data[0][0] / lengths[0] - src[3].x * dst.data[0][1] / lengths[0] - src[3].x * dst.data[0][2] / lengths[0];
    dst[3][1] =
        -src[3].y * dst.data[1][0] / lengths[1] - src[3].y * dst.data[1][1] / lengths[1] - src[3].y * dst.data[1][2] / lengths[1];
    dst[3][2] =
        -src[3].z * dst.data[2][0] / lengths[2] - src[3].z * dst.data[2][1] / lengths[2] - src[3].z * dst.data[2][2] / lengths[2];
    dst[0][3] = 0;
    dst[1][3] = 0;
    dst[2][3] = 0;
    dst[3][3] = 1;
  }
  template <Primitive T>
  constexpr mat<4, 4, T> invert_orthonormal(mat<4, 4, T> const &src)
  {
    mat<4, 4, T> return_value;
    return_value.reset();
    invert_orthonormal(src, return_value);
    return return_value;
  }
  template <Primitive T>
  constexpr mat<4, 4, T> invert_orthogonal(mat<4, 4, T> const &src)
  {
    mat<4, 4, T> return_value;
    return_value.reset();
    invert_orthogonal(src, return_value);
    return return_value;
  }
} // namespace engine::math