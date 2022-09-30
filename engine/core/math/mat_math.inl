#pragma once
#include "mat_math.hpp"
#pragma warning(push)
#pragma warning(disable : 4701)
namespace engine::core::math
{
    template <AnyMat U>
    constexpr std::istream &operator>>(std::istream &is, U &matrix)
    {
        for (size_t i = 0; i < U::size.x; i++)
        {
            for (size_t j = 0; j < U::size.y; j++)
            {
                is >> matrix[i][j];
            }
        }
        return is;
    }
    template <AnyMat U>
    constexpr std::ostream &operator<<(std::ostream &os, U const &matrix)
    {
        for (size_t i = 0; i < U::size.x; i++)
        {
            for (size_t j = 0; j < U::size.y; j++)
            {
                os << matrix[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }
    template <AnyMat U, AnyMat V>
    [[nodiscard]] constexpr auto operator*(U const &lhs, V const &rhs) requires (U::size.x == V::size.y)
    {
        using T = decltype(lhs[0][0] * rhs[0][0]);
        using R = mat<V::size.x, U::size.y, T>;
        R result;
        for (size_t i = 0; i < V::size.x; i++)
        {
            for (size_t j = 0; j < U::size.y; j++)
            {
                T sum = 0;
                for (size_t k = 0; k < V::size.y; k++)
                {
                    sum += lhs[k][j] * rhs[i][k];
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    template <AnyVec V, AnyMat M>
    [[nodiscard]] constexpr auto operator*(V const &left, M const &right) requires (V::size == M::size.y)
    {
        using T = decltype(left[0] * right[0][0]);
        using R = vec<M::size.x, T>;
        R result;
        for (size_t i = 0; i < M::size.x; i++)
        {
            T sum = 0;
            for (size_t j = 0; j < M::size.y; j++)
            {
                sum += left[j] * right[i][j];
            }
            result[i] = sum;
        }
        return result;
    }
    template <AnyMat M, AnyVec V>
    [[nodiscard]] constexpr auto operator*(M const &left, V const &right) requires (M::size.x == V::size)
    {
        using T = decltype(left[0][0] * right[0]);
        using R = vec<M::size.y, T>;
        R result;
        for (size_t i = 0; i < M::size.y; i++)
        {
            T sum = 0;
            for (size_t j = 0; j < M::size.x; j++)
            {
                sum += left[j][i] * right[j];
            }
            result[i] = sum;
        }
        return result;
    }

    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator*(T const &left, U const right)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(left) *= right;
    }
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator*(U const left, T const &right)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(right) *= left;
    }

    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator-(T const &left, U const right)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(left) -= right;
    }
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator+(U const left, T const &right)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(left) += right;
    }

    template <AnyMat T, AnyMat U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator-(T const left, U const &right) requires(T::size.x == U::size.x && T::size.y == U::size.y)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(left) -= right;
    }
    template <AnyMat T, AnyMat U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator+(T const &left, U const &right) requires(T::size.x == U::size.x && T::size.y == U::size.y)
    {
        return mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>>(left) += right;
    }
    template <AnyMat T>
    constexpr mat<T::size.y, T::size.x, std::remove_const_t<typename T::type>> transpose(T const &matrix)
    {
        mat<T::size.y, T::size.x, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size.x; i++)
        {
            for (size_t j = 0; j < T::size.y; j++)
            {
                return_value[j][i] = matrix[i][j];
            }
        }
        return return_value;
    }
    template <AnyMat T>
    constexpr rmat<T::size.y, T::size.x, typename T::type> rtranspose(T &matrix)
    {
        rmat<T::size.y, T::size.x, typename T::type> return_value;
        for (size_t i = 0; i < T::size.x; i++)
        {
            for (size_t j = 0; j < T::size.y; j++)
            {
                return_value.arr[j * T::size.x + i].set_ptr(matrix[i][j]);
            }
        }
        return return_value;
    }
    template <AnyMat T>
    constexpr rmat<T::size.y, T::size.x, const typename T::type> rctranspose(T const &matrix)
    {
        rmat<T::size.y, T::size.x, const typename T::type> return_value;
        for (size_t i = 0; i < T::size.x; i++)
        {
            for (size_t j = 0; j < T::size.y; j++)
            {
                return_value.arr[j * T::size.x + i].set_ptr(matrix[i][j]);
            }
        }
        return return_value;
    }

    template <AnyMat T>
    constexpr typename T::type det_2(T const &m) requires(T::size.x == T::size.y && T::size.x == 2)
    {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    }
    template <AnyMat T>
    constexpr typename T::type det_3(T const &m) requires(T::size.x == T::size.y && T::size.x == 3)
    {
        return +m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    }
    template <AnyMat T>
    constexpr typename T::type det_4(T const &m) requires(T::size.x == T::size.y && T::size.x == 4)
    {
        typename T::type const sub_det_00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        typename T::type const sub_det_01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        typename T::type const sub_det_02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        typename T::type const sub_det_03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        typename T::type const sub_det_04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        typename T::type const sub_det_05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        vec<4, typename T::type> sub_det(
            +(m[1][1] * sub_det_00 - m[1][2] * sub_det_01 + m[1][3] * sub_det_02),
            -(m[1][0] * sub_det_00 - m[1][2] * sub_det_03 + m[1][3] * sub_det_04),
            +(m[1][0] * sub_det_01 - m[1][1] * sub_det_03 + m[1][3] * sub_det_05),
            -(m[1][0] * sub_det_02 - m[1][1] * sub_det_04 + m[1][2] * sub_det_05));

        return m[0][0] * sub_det[0] + m[0][1] * sub_det[1] + m[0][2] * sub_det[2] +
            m[0][3] * sub_det[3];
    }

    template <AnyMat T>
    constexpr typename T::type det(T const &m) requires(T::size.x == T::size.y)
    {
        if constexpr (T::size.x == 1)
            return m[0][0];
        if constexpr (T::size.x == 2)
            return det_2(m);
        if constexpr (T::size.x == 3)
            return det_3(m);
        if constexpr (T::size.x == 4)
            return det_4(m);
    }
    template <AnyMat T>
    constexpr typename T::type determinant(T const &matrix) requires(T::size.x == T::size.y)
    {
        return det(matrix);
    }

    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> adj(T const &m) requires(T::size.x == T::size.y)
    {
        mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> return_value;
        for (size_t i = 0; i < T::size.x; i++)
        {
            for (size_t j = 0; j < T::size.y; j++)
            {
                mat<T::size.x - 1, T::size.y - 1, std::remove_const_t<typename T::type>> sub_matrix;
                for (size_t k = 0; k < T::size.x; k++)
                {
                    for (size_t l = 0; l < T::size.y; l++)
                    {
                        if (k != i && l != j)
                        {
                            sub_matrix[k < i ? k : k - 1][l < j ? l : l - 1] = m[k][l];
                        }
                    }
                }
                return_value[j][i] = det(sub_matrix) * ((i + j) % 2 == 0 ? 1 : -1);
            }
        }
        return return_value;
    }
    // calculate an adjugate for a matrix
    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> adjugate(T const &m) requires(T::size.x == T::size.y)
    {
        return adj(m);
    }

    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> inverse(T const &m) requires(T::size.x == T::size.y)
    {
        return adjugate(m) * (static_cast<typename T::type>(1) / det(m));
    }

    template <AnyMat T, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> translate(T const &matrix, V const &vec) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3)
    {
        mat<4, 4, typename T::type> return_value;
        return_value[0] = matrix[0];
        return_value[1] = matrix[1];
        return_value[2] = matrix[2];
        return_value[3] = matrix[0] * vec[0] + matrix[1] * vec[1] + matrix[2] * vec[2] + matrix[3];
        return return_value;
    }
    template <AnyMat T, Primitive U, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> rotate(T const &matrix, U angle, V const &vector) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3)
    {
        typename T::type const c = std::cos(angle);
        typename T::type const s = std::sin(angle);
        vec<3, typename T::type> axis = normalize(vector);
        vec<3, typename T::type> temp = (static_cast<typename T::type>(1) - c) * axis;
        mat<4, 4, typename T::type> rotate;

        rotate[0][0] = c + temp[0] * axis[0];
        rotate[0][1] = temp[0] * axis[1] + s * axis[2];
        rotate[0][2] = temp[0] * axis[2] - s * axis[1];

        rotate[1][0] = temp[1] * axis[0] - s * axis[2];
        rotate[1][1] = c + temp[1] * axis[1];
        rotate[1][2] = temp[1] * axis[2] + s * axis[0];

        rotate[2][0] = temp[2] * axis[0] + s * axis[1];
        rotate[2][1] = temp[2] * axis[1] - s * axis[0];
        rotate[2][2] = c + temp[2] * axis[2];

        mat<4, 4, typename T::type> result(1);
        result[0] = matrix[0] * rotate[0][0] + matrix[1] * rotate[0][1] +
            matrix[2] * rotate[0][2];
        result[1] = matrix[0] * rotate[1][0] + matrix[1] * rotate[1][1] +
            matrix[2] * rotate[1][2];
        result[2] = matrix[0] * rotate[2][0] + matrix[1] * rotate[2][1] +
            matrix[2] * rotate[2][2];
        result[3] = matrix[3];
        return result;
    }
    template <AnyMat T, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> scale(T const &matrix, V const &scale) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3)
    {
        mat<4, 4, typename T::type> return_value;
        return_value[0] = matrix[0] * scale[0];
        return_value[1] = matrix[1] * scale[1];
        return_value[2] = matrix[2] * scale[2];
        return_value[3] = matrix[3];
        return return_value;
    }

    template <AnyVec Position>
    constexpr mat<4, 4, std::remove_const_t<typename Position::type>> look_at(Position const &eye, Position const &center, Position const &world_up) requires(Position::size == 3)
    {
        vec<3, typename Position::type> const forward = normalize(center - eye);
        vec<3, typename Position::type> const right = normalize(cross(world_up, forward));
        vec<3, typename Position::type> const up = cross(forward, right);
        mat<4, 4, typename Position::type> return_value(1);
        return_value[0][0] = right.x;
        return_value[1][0] = right.y;
        return_value[2][0] = right.z;
        return_value[0][1] = up.x;
        return_value[1][1] = up.y;
        return_value[2][1] = up.z;
        return_value[0][2] = forward.x;
        return_value[1][2] = forward.y;
        return_value[2][2] = forward.z;
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
        return_value[1][1] = static_cast<T>(1) / tan_half_fov_y;
        return_value[2][2] = z_far / (z_far - z_near);
        return_value[2][3] = static_cast<T>(1);
        return_value[3][2] = -(z_far * z_near) / (z_far - z_near);
        return return_value;
    }

    template <Primitive T>
    constexpr mat<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        mat<4, 4, T> Result(1);
        Result[0][0] = static_cast<T>(2) / (right - left);
        Result[1][1] = static_cast<T>(2) / (top - bottom);
        Result[2][2] = static_cast<T>(1) / (zFar - zNear);
        Result[3][0] = -(right + left) / (right - left);
        Result[3][1] = -(top + bottom) / (top - bottom);
        Result[3][2] = -zNear / (zFar - zNear);
        return Result;
    }

    template <Primitive T>
    constexpr void invert_orthonormal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
    {
        assert(&src != &dst);
        dst[0][0] = src[0][0];
        dst[1][1] = src[1][1];
        dst[2][2] = src[2][2];
        dst[0][1] = src[1][0];
        dst[1][0] = src[0][1];
        dst[0][2] = src[2][0];
        dst[2][0] = src[0][2];
        dst[1][2] = src[2][1];
        dst[2][1] = src[1][2];
        as_rvec<3>(dst[3]) =
            -src[3].x * as_rvec<3>(dst.data[0]) - src[3].y * as_rvec<3>(dst.data[1]) - src[3].z * as_rvec<3>(dst.data[2]);
        dst[0][3] = 0;
        dst[1][3] = 0;
        dst[2][3] = 0;
        dst[3][3] = 1;
    }
    template <Primitive T>
    constexpr void invert_orthogonal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
    {
        assert(&src != &dst);
        dst[0][0] = src[0][0];
        dst[1][1] = src[1][1];
        dst[2][2] = src[2][2];
        dst[0][1] = src[1][0];
        dst[1][0] = src[0][1];
        dst[0][2] = src[2][0];
        dst[2][0] = src[0][2];
        dst[1][2] = src[2][1];
        dst[2][1] = src[1][2];

        vec3 lengths{
            core::math::sqrt(dst[0][0] * dst[0][0] + dst[0][1] * dst[0][1] +
                                     dst[0][2] * dst[0][2]),
            core::math::sqrt(dst[1][0] * dst[1][0] + dst[1][1] * dst[1][1] +
                                     dst[1][2] * dst[1][2]),
            core::math::sqrt(dst[2][0] * dst[2][0] + dst[2][1] * dst[2][1] +
                                     dst[2][2] * dst[2][2]),
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

        dst[3][0] = -src[3].x * dst.data[0][0] / lengths[0] -
            src[3].x * dst.data[0][1] / lengths[0] -
            src[3].x * dst.data[0][2] / lengths[0];
        dst[3][1] = -src[3].y * dst.data[1][0] / lengths[1] -
            src[3].y * dst.data[1][1] / lengths[1] -
            src[3].y * dst.data[1][2] / lengths[1];
        dst[3][2] = -src[3].z * dst.data[2][0] / lengths[2] -
            src[3].z * dst.data[2][1] / lengths[2] -
            src[3].z * dst.data[2][2] / lengths[2];
        dst[0][3] = 0;
        dst[1][3] = 0;
        dst[2][3] = 0;
        dst[3][3] = 1;
    }
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthonormal(mat<4, 4, T> const &src)
    {
        mat<4, 4, T> return_value;
        invert_orthonormal(src, return_value);
        return return_value;
    }
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthogonal(mat<4, 4, T> const &src)
    {
        mat<4, 4, T> return_value;
        invert_orthogonal(src, return_value);
        return return_value;
    }
} // namespace engine::core::math
#pragma warning(pop)