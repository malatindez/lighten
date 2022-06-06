#pragma once
#include "matnxn.hpp"
#include "rmatnxn.hpp"
#include "vec.hpp"
namespace engine::core::math
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

    using rmat4 = rmat<4, 4, float>;
    using drmat4 = rmat<4, 4, double>;
    using irmat4 = rmat<4, 4, int>;
    using rmat4x4 = rmat<4, 4, float>;
    using drmat4x4 = rmat<4, 4, double>;
    using irmat4x4 = rmat<4, 4, int>;

    using rmat3 = rmat<3, 3, float>;
    using drmat3 = rmat<3, 3, double>;
    using irmat3 = rmat<3, 3, int>;
    using rmat3x3 = rmat<3, 3, float>;
    using drmat3x3 = rmat<3, 3, double>;
    using irmat3x3 = rmat<3, 3, int>;

    using rmat2 = rmat<2, 2, float>;
    using drmat2 = rmat<2, 2, double>;
    using irmat2 = rmat<2, 2, int>;
    using rmat2x2 = rmat<2, 2, float>;
    using drmat2x2 = rmat<2, 2, double>;
    using irmat2x2 = rmat<2, 2, int>;

    using rmat2x3 = rmat<2, 3, float>;
    using drmat2x3 = rmat<2, 3, double>;
    using irmat2x3 = rmat<2, 3, int>;

    using rmat3x2 = rmat<3, 2, float>;
    using drmat3x2 = rmat<3, 2, double>;
    using irmat3x2 = rmat<3, 2, int>;

    using rmat2x4 = rmat<2, 4, float>;
    using drmat2x4 = rmat<2, 4, double>;
    using irmat2x4 = rmat<2, 4, int>;

    using rmat4x2 = rmat<4, 2, float>;
    using drmat4x2 = rmat<4, 2, double>;
    using irmat4x2 = rmat<4, 2, int>;

    using rmat3x4 = rmat<3, 4, float>;
    using drmat3x4 = rmat<3, 4, double>;
    using irmat3x4 = rmat<3, 4, int>;

    using rmat4x3 = rmat<4, 3, float>;
    using drmat4x3 = rmat<4, 3, double>;
    using irmat4x3 = rmat<4, 3, int>;
} // namespace engine::core::math
#include "mat_math.hpp"