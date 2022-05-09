#pragma once
#include "vec.hpp"
namespace engine::math
{
    template <size_t rows, size_t columns, Primitive T>
    struct mat;
}
#include "matnxn.hpp"
namespace engine::math
{
    using mat4 = mat<4, 4, float>;
    using dmat4 = mat<4, 4, double>;
    using imat4 = mat<4, 4, int>;
    using mat4x4 = mat<4, 4, float>;
    using dmat4x4 = mat<4, 4, double>;
    using imat4x4 = mat<4, 4, int>;

    using mat3 = mat<3, 3, float>;
    using dmat3 = mat<3, 3, double>;
    using imat3 = mat<3, 3, int>;
    using mat3x3 = mat<3, 3, float>;
    using dmat3x3 = mat<3, 3, double>;
    using imat3x3 = mat<3, 3, int>;

    using mat2 = mat<2, 2, float>;
    using dmat2 = mat<2, 2, double>;
    using imat2 = mat<2, 2, int>;
    using mat2x2 = mat<2, 2, float>;
    using dmat2x2 = mat<2, 2, double>;
    using imat2x2 = mat<2, 2, int>;

    using mat2x3 = mat<2, 3, float>;
    using dmat2x3 = mat<2, 3, double>;
    using imat2x3 = mat<2, 3, int>;

    using mat3x2 = mat<3, 2, float>;
    using dmat3x2 = mat<3, 2, double>;
    using imat3x2 = mat<3, 2, int>;

    using mat2x4 = mat<2, 4, float>;
    using dmat2x4 = mat<2, 4, double>;
    using imat2x4 = mat<2, 4, int>;

    using mat4x2 = mat<4, 2, float>;
    using dmat4x2 = mat<4, 2, double>;
    using imat4x2 = mat<4, 2, int>;

    using mat3x4 = mat<3, 4, float>;
    using dmat3x4 = mat<3, 4, double>;
    using imat3x4 = mat<3, 4, int>;

    using mat4x3 = mat<4, 3, float>;
    using dmat4x3 = mat<4, 3, double>;
    using imat4x3 = mat<4, 3, int>;
}
#include "mat_math.hpp"