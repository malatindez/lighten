#include "core/math.hpp"
#include "pch.h"
#include "mal-toolkit/mal-toolkit.hpp"
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
using namespace engine::core::math;

static int const kNumIterations = 100;
template<typename T>
T random_float_arr(float from = -100, float to = 100)
{
    T return_value;
    auto &arr = reinterpret_cast<std::array<float, size_t(sizeof(T) / sizeof(float))> &>(return_value);
    for (auto &v : arr)
    {
        v = mal_toolkit::Random<float>(from, to);
    }
    return return_value;
}
template<AnyMat M, int x, int y, typename T, glm::qualifier Q>
M create_from_other(glm::mat<x, y, T, Q> const &source)
{
    static_assert(sizeof(M) == sizeof(glm::mat<x, y, T, Q>), "Size of matrices must be equal");
    M return_value;
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            return_value[i][j] = source[i][j];
        }
    }
    return return_value;
}
template<AnyVec V, int x, typename T, glm::qualifier Q>
V create_from_other(glm::vec<x, T, Q> const &source)
{
    static_assert(sizeof(V) == sizeof(glm::vec<x, T, Q>), "Size of vectors must be equal");
    V return_value;
    for (int i = 0; i < x; ++i)
    {
        return_value[i] = source[i];
    }
    return return_value;
}
template<AnyMat M, int x, int y, typename T, glm::qualifier Q>
bool operator==(M const &lhs, glm::mat<x, y, T, Q> const &rhs)
{
    static_assert(sizeof(M) == sizeof(glm::mat<x, y, T, Q>));
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            if (!almost_equal(lhs[i][j], rhs[i][j], 10000000) || std::isnan(lhs[i][j]) != std::isnan(rhs[i][j]))
            {
                return false;
            }
        }
    }
    return true;
}
template<AnyVec V, int x, typename T, glm::qualifier Q>
bool operator==(V const &lhs, glm::vec<x, T, Q> const &rhs)
{
    static_assert(sizeof(V) == sizeof(glm::vec<x, T, Q>));
    for (int i = 0; i < x; ++i)
    {
        if (!almost_equal(lhs[i], rhs[i], 1000000) && std::isnan(lhs[i]) != std::isnan(rhs[i]))
        {
            return false;
        }
    }
    return true;
}
template<AnyMat M, size_t x, size_t y, typename T, glm::qualifier Q>
bool operator==(glm::mat<x, y, T, Q> const &lhs, M const &rhs)
{
    return rhs == lhs;
}
template<AnyVec V, size_t x, typename T, glm::qualifier Q>
bool operator==(glm::vec<x, T, Q> const &lhs, V const &rhs)
{
    return rhs == lhs;
}

template<AnyMat M, size_t x, size_t y, typename T, glm::qualifier Q>
void BasicArithmeticTestSquaredMatricesMultiplication(M const &mat_1,
                                                      M const &mat_2,
                                                      M const &mat_3,
                                                      glm::mat<x, y, T, Q> const &glm_mat_1,
                                                      glm::mat<x, y, T, Q> const &glm_mat_2,
                                                      glm::mat<x, y, T, Q> const &glm_mat_3)
{
    ASSERT_TRUE(mat_1 + (mat_2 * mat_3) == glm_mat_1 + (glm_mat_2 * glm_mat_3));
    ASSERT_TRUE(mat_1 - (mat_2 * mat_3) == glm_mat_1 - (glm_mat_2 * glm_mat_3));
    ASSERT_TRUE((mat_1 + mat_2) * mat_3 == (glm_mat_1 + glm_mat_2) * glm_mat_3);
    ASSERT_TRUE((mat_1 - mat_2) * mat_3 == (glm_mat_1 - glm_mat_2) * glm_mat_3);
    ASSERT_TRUE(mat_1 * (mat_2 + mat_3) == glm_mat_1 * (glm_mat_2 + glm_mat_3));
    ASSERT_TRUE(mat_1 * (mat_2 - mat_3) == glm_mat_1 * (glm_mat_2 - glm_mat_3));
    ASSERT_TRUE(mat_1 * mat_2 * mat_3 == glm_mat_1 * glm_mat_2 * glm_mat_3);
    ASSERT_TRUE(mat_2 * mat_1 * mat_3 == glm_mat_2 * glm_mat_1 * glm_mat_3);
    ASSERT_TRUE(mat_2 * mat_3 * mat_1 == glm_mat_2 * glm_mat_3 * glm_mat_1);
    ASSERT_TRUE(mat_3 * mat_1 * mat_2 == glm_mat_3 * glm_mat_1 * glm_mat_2);
    ASSERT_TRUE(mat_3 * mat_2 * mat_1 == glm_mat_3 * glm_mat_2 * glm_mat_1);
    ASSERT_TRUE(mat_1 * mat_2 == glm_mat_1 * glm_mat_2);
    ASSERT_TRUE(mat_2 * mat_3 == glm_mat_2 * glm_mat_3);
    ASSERT_TRUE(mat_1 * (mat_2 * mat_3) == glm_mat_1 * (glm_mat_2 * glm_mat_3));
    ASSERT_TRUE((mat_1 * mat_2) * mat_3 == (glm_mat_1 * glm_mat_2) * glm_mat_3);
    ASSERT_TRUE(mat_3 * (mat_2 * mat_1) == glm_mat_3 * (glm_mat_2 * glm_mat_1));
    ASSERT_TRUE((mat_3 * mat_2) * mat_1 == (glm_mat_3 * glm_mat_2) * glm_mat_1);
    ASSERT_TRUE(mat_1 * (mat_2 + mat_3) == glm_mat_1 * (glm_mat_2 + glm_mat_3));
    ASSERT_TRUE((mat_1 + mat_2) * mat_3 == (glm_mat_1 + glm_mat_2) * glm_mat_3);
    ASSERT_TRUE(mat_3 * (mat_2 + mat_1) == glm_mat_3 * (glm_mat_2 + glm_mat_1));
    ASSERT_TRUE((mat_3 + mat_2) * mat_1 == (glm_mat_3 + glm_mat_2) * glm_mat_1);
    ASSERT_TRUE(mat_1 * (mat_2 - mat_3) == glm_mat_1 * (glm_mat_2 - glm_mat_3));
    ASSERT_TRUE((mat_1 - mat_2) * mat_3 == (glm_mat_1 - glm_mat_2) * glm_mat_3);
    ASSERT_TRUE(mat_3 * (mat_2 - mat_1) == glm_mat_3 * (glm_mat_2 - glm_mat_1));
    ASSERT_TRUE((mat_3 - mat_2) * mat_1 == (glm_mat_3 - glm_mat_2) * glm_mat_1);
}
template<typename A, typename B, typename C, typename D, typename E, typename F>
void BasicArithmeticTestNonSquaredMatricesMultiplication(A const &mat_1,
                                                         B const &mat_2,
                                                         C const &mat_3,
                                                         D const &glm_mat_1,
                                                         E const &glm_mat_2,
                                                         F const &glm_mat_3)
{
    ASSERT_TRUE(mat_2 * mat_1 == glm_mat_2 * glm_mat_1);
    ASSERT_TRUE(mat_3 * mat_2 == glm_mat_3 * glm_mat_2);
    ASSERT_TRUE((mat_3 * mat_2) * mat_1 == (glm_mat_3 * glm_mat_2) * glm_mat_1);
}
template<typename A, typename B, typename C, typename D, typename E, typename F>
void BasicArithmeticTestMatricesVectorsMultiplication(A const &mat_1,
                                                      B const &mat_2,
                                                      C const &mat_3,
                                                      D const &glm_mat_1,
                                                      E const &glm_mat_2,
                                                      F const &glm_mat_3)
{
    ASSERT_TRUE(mat_1 * mat_2 == glm_mat_1 * glm_mat_2);
    ASSERT_TRUE(mat_1 * mat_2 * mat_3 == glm_mat_1 * glm_mat_2 * glm_mat_3);
    ASSERT_TRUE((mat_1 * mat_2) * mat_3 == (glm_mat_1 * glm_mat_2) * glm_mat_3);
}
template<AnyMat M, size_t x, size_t y, typename T, glm::qualifier Q>
void BasicArithmeticTest(M const &mat_1,
                         M const &mat_2,
                         M const &mat_3,
                         glm::mat<x, y, T, Q> const &glm_mat_1,
                         glm::mat<x, y, T, Q> const &glm_mat_2,
                         glm::mat<x, y, T, Q> const &glm_mat_3)
{
    ASSERT_TRUE(mat_1 + mat_2 == glm_mat_1 + glm_mat_2);
    ASSERT_TRUE(mat_1 - mat_2 == glm_mat_1 - glm_mat_2);
    ASSERT_TRUE(mat_1 + mat_3 == glm_mat_1 + glm_mat_3);
    ASSERT_TRUE(mat_1 - mat_3 == glm_mat_1 - glm_mat_3);
    ASSERT_TRUE(mat_2 + mat_3 == glm_mat_2 + glm_mat_3);
    ASSERT_TRUE(mat_2 - mat_3 == glm_mat_2 - glm_mat_3);
    ASSERT_TRUE(mat_1 + mat_2 + mat_3 == glm_mat_1 + glm_mat_2 + glm_mat_3);
    ASSERT_TRUE(mat_1 - mat_2 - mat_3 == glm_mat_1 - glm_mat_2 - glm_mat_3);
    ASSERT_TRUE(mat_1 + mat_2 - mat_3 == glm_mat_1 + glm_mat_2 - glm_mat_3);
    ASSERT_TRUE(mat_1 - mat_2 + mat_3 == glm_mat_1 - glm_mat_2 + glm_mat_3);
}

TEST(TEST_MATRICES, TestBasicArithmetic)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::mat2x2 glm_mat2x2_1 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_2 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_3 = random_float_arr<glm::mat2x2>(-100, 100);
        mat2x2 mat2x2_1 = create_from_other<mat2x2>(glm_mat2x2_1);
        mat2x2 mat2x2_2 = create_from_other<mat2x2>(glm_mat2x2_2);
        mat2x2 mat2x2_3 = create_from_other<mat2x2>(glm_mat2x2_3);
        glm::mat2x3 glm_mat2x3_1 = random_float_arr<glm::mat2x3>(-100, 100);
        glm::mat2x3 glm_mat2x3_2 = random_float_arr<glm::mat2x3>(-100, 100);
        glm::mat2x3 glm_mat2x3_3 = random_float_arr<glm::mat2x3>(-100, 100);
        mat2x3 mat2x3_1 = create_from_other<mat2x3>(glm_mat2x3_1);
        mat2x3 mat2x3_2 = create_from_other<mat2x3>(glm_mat2x3_2);
        mat2x3 mat2x3_3 = create_from_other<mat2x3>(glm_mat2x3_3);
        glm::mat2x4 glm_mat2x4_1 = random_float_arr<glm::mat2x4>(-100, 100);
        glm::mat2x4 glm_mat2x4_2 = random_float_arr<glm::mat2x4>(-100, 100);
        glm::mat2x4 glm_mat2x4_3 = random_float_arr<glm::mat2x4>(-100, 100);
        mat2x4 mat2x4_1 = create_from_other<mat2x4>(glm_mat2x4_1);
        mat2x4 mat2x4_2 = create_from_other<mat2x4>(glm_mat2x4_2);
        mat2x4 mat2x4_3 = create_from_other<mat2x4>(glm_mat2x4_3);
        glm::mat3x2 glm_mat3x2_1 = random_float_arr<glm::mat3x2>(-100, 100);
        glm::mat3x2 glm_mat3x2_2 = random_float_arr<glm::mat3x2>(-100, 100);
        glm::mat3x2 glm_mat3x2_3 = random_float_arr<glm::mat3x2>(-100, 100);
        mat3x2 mat3x2_1 = create_from_other<mat3x2>(glm_mat3x2_1);
        mat3x2 mat3x2_2 = create_from_other<mat3x2>(glm_mat3x2_2);
        mat3x2 mat3x2_3 = create_from_other<mat3x2>(glm_mat3x2_3);
        glm::mat3x3 glm_mat3x3_1 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_2 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_3 = random_float_arr<glm::mat3x3>(-100, 100);
        mat3x3 mat3x3_1 = create_from_other<mat3x3>(glm_mat3x3_1);
        mat3x3 mat3x3_2 = create_from_other<mat3x3>(glm_mat3x3_2);
        mat3x3 mat3x3_3 = create_from_other<mat3x3>(glm_mat3x3_3);
        glm::mat3x4 glm_mat3x4_1 = random_float_arr<glm::mat3x4>(-100, 100);
        glm::mat3x4 glm_mat3x4_2 = random_float_arr<glm::mat3x4>(-100, 100);
        glm::mat3x4 glm_mat3x4_3 = random_float_arr<glm::mat3x4>(-100, 100);
        mat3x4 mat3x4_1 = create_from_other<mat3x4>(glm_mat3x4_1);
        mat3x4 mat3x4_2 = create_from_other<mat3x4>(glm_mat3x4_2);
        mat3x4 mat3x4_3 = create_from_other<mat3x4>(glm_mat3x4_3);
        glm::mat4x2 glm_mat4x2_1 = random_float_arr<glm::mat4x2>(-100, 100);
        glm::mat4x2 glm_mat4x2_2 = random_float_arr<glm::mat4x2>(-100, 100);
        glm::mat4x2 glm_mat4x2_3 = random_float_arr<glm::mat4x2>(-100, 100);
        mat4x2 mat4x2_1 = create_from_other<mat4x2>(glm_mat4x2_1);
        mat4x2 mat4x2_2 = create_from_other<mat4x2>(glm_mat4x2_2);
        mat4x2 mat4x2_3 = create_from_other<mat4x2>(glm_mat4x2_3);
        glm::mat4x3 glm_mat4x3_1 = random_float_arr<glm::mat4x3>(-100, 100);
        glm::mat4x3 glm_mat4x3_2 = random_float_arr<glm::mat4x3>(-100, 100);
        glm::mat4x3 glm_mat4x3_3 = random_float_arr<glm::mat4x3>(-100, 100);
        mat4x3 mat4x3_1 = create_from_other<mat4x3>(glm_mat4x3_1);
        mat4x3 mat4x3_2 = create_from_other<mat4x3>(glm_mat4x3_2);
        mat4x3 mat4x3_3 = create_from_other<mat4x3>(glm_mat4x3_3);
        glm::mat4x4 glm_mat4x4_1 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_2 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_3 = random_float_arr<glm::mat4x4>(-100, 100);
        mat4x4 mat4x4_1 = create_from_other<mat4x4>(glm_mat4x4_1);
        mat4x4 mat4x4_2 = create_from_other<mat4x4>(glm_mat4x4_2);
        mat4x4 mat4x4_3 = create_from_other<mat4x4>(glm_mat4x4_3);
        glm::vec2 glm_vec2_1 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_2 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_3 = random_float_arr<glm::vec2>(-100, 100);
        vec2 vec2_1 = create_from_other<vec2>(glm_vec2_1);
        vec2 vec2_2 = create_from_other<vec2>(glm_vec2_2);
        vec2 vec2_3 = create_from_other<vec2>(glm_vec2_3);
        glm::vec3 glm_vec3_1 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_2 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_3 = random_float_arr<glm::vec3>(-100, 100);
        vec3 vec3_1 = create_from_other<vec3>(glm_vec3_1);
        vec3 vec3_2 = create_from_other<vec3>(glm_vec3_2);
        vec3 vec3_3 = create_from_other<vec3>(glm_vec3_3);
        glm::vec4 glm_vec4_1 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_2 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_3 = random_float_arr<glm::vec4>(-100, 100);
        vec4 vec4_1 = create_from_other<vec4>(glm_vec4_1);
        vec4 vec4_2 = create_from_other<vec4>(glm_vec4_2);
        vec4 vec4_3 = create_from_other<vec4>(glm_vec4_3);
        ASSERT_TRUE(mat2x2_1 == glm_mat2x2_1);
        ASSERT_TRUE(mat2x2_2 == glm_mat2x2_2);
        ASSERT_TRUE(mat2x2_3 == glm_mat2x2_3);
        ASSERT_TRUE(mat2x3_1 == glm_mat2x3_1);
        ASSERT_TRUE(mat2x3_2 == glm_mat2x3_2);
        ASSERT_TRUE(mat2x3_3 == glm_mat2x3_3);
        ASSERT_TRUE(mat2x4_1 == glm_mat2x4_1);
        ASSERT_TRUE(mat2x4_2 == glm_mat2x4_2);
        ASSERT_TRUE(mat2x4_3 == glm_mat2x4_3);
        ASSERT_TRUE(mat3x2_1 == glm_mat3x2_1);
        ASSERT_TRUE(mat3x2_2 == glm_mat3x2_2);
        ASSERT_TRUE(mat3x2_3 == glm_mat3x2_3);
        ASSERT_TRUE(mat3x3_1 == glm_mat3x3_1);
        ASSERT_TRUE(mat3x3_2 == glm_mat3x3_2);
        ASSERT_TRUE(mat3x3_3 == glm_mat3x3_3);
        ASSERT_TRUE(mat3x4_1 == glm_mat3x4_1);
        ASSERT_TRUE(mat3x4_2 == glm_mat3x4_2);
        ASSERT_TRUE(mat3x4_3 == glm_mat3x4_3);
        ASSERT_TRUE(mat4x2_1 == glm_mat4x2_1);
        ASSERT_TRUE(mat4x2_2 == glm_mat4x2_2);
        ASSERT_TRUE(mat4x2_3 == glm_mat4x2_3);
        ASSERT_TRUE(mat4x3_1 == glm_mat4x3_1);
        ASSERT_TRUE(mat4x3_2 == glm_mat4x3_2);
        ASSERT_TRUE(mat4x3_3 == glm_mat4x3_3);
        ASSERT_TRUE(mat4x4_1 == glm_mat4x4_1);
        ASSERT_TRUE(mat4x4_2 == glm_mat4x4_2);
        ASSERT_TRUE(mat4x4_3 == glm_mat4x4_3);
        ASSERT_TRUE(vec2_1 == glm_vec2_1);
        ASSERT_TRUE(vec2_2 == glm_vec2_2);
        ASSERT_TRUE(vec2_3 == glm_vec2_3);
        ASSERT_TRUE(vec3_1 == glm_vec3_1);
        ASSERT_TRUE(vec3_2 == glm_vec3_2);
        ASSERT_TRUE(vec3_3 == glm_vec3_3);
        ASSERT_TRUE(vec4_1 == glm_vec4_1);
        ASSERT_TRUE(vec4_2 == glm_vec4_2);
        ASSERT_TRUE(vec4_3 == glm_vec4_3);
        BasicArithmeticTestSquaredMatricesMultiplication(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTestSquaredMatricesMultiplication(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTestSquaredMatricesMultiplication(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat2x3_1, mat3x2_1, mat2x2_1, glm_mat2x3_1, glm_mat3x2_1, glm_mat2x2_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat2x4_1, mat4x2_1, mat2x2_1, glm_mat2x4_1, glm_mat4x2_1, glm_mat2x2_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat3x2_1, mat2x3_1, mat3x3_1, glm_mat3x2_1, glm_mat2x3_1, glm_mat3x3_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat3x4_1, mat4x3_1, mat3x3_1, glm_mat3x4_1, glm_mat4x3_1, glm_mat3x3_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat4x2_1, mat2x4_1, mat4x4_1, glm_mat4x2_1, glm_mat2x4_1, glm_mat4x4_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat4x3_1, mat3x4_1, mat4x4_1, glm_mat4x3_1, glm_mat3x4_1, glm_mat4x4_1);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTestNonSquaredMatricesMultiplication(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x2_1, vec3_1, vec2_1, glm_mat3x2_1, glm_vec3_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, vec3_1, vec3_1, glm_mat3x3_1, glm_vec3_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x4_1, vec3_1, vec4_1, glm_mat3x4_1, glm_vec3_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x2_1, vec4_1, vec2_1, glm_mat4x2_1, glm_vec4_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x3_1, vec4_1, vec3_1, glm_mat4x3_1, glm_vec4_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, vec4_1, vec4_1, glm_mat4x4_1, glm_vec4_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat2x3_1, vec2_1, vec3_1, glm_mat2x3_1, glm_vec2_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat2x4_1, vec2_1, vec4_1, glm_mat2x4_1, glm_vec2_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x2_1, vec3_1, vec2_1, glm_mat3x2_1, glm_vec3_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, vec3_1, vec3_1, glm_mat3x3_1, glm_vec3_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x4_1, vec3_1, vec4_1, glm_mat3x4_1, glm_vec3_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x2_1, vec4_1, vec2_1, glm_mat4x2_1, glm_vec4_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x3_1, vec4_1, vec3_1, glm_mat4x3_1, glm_vec4_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, vec4_1, vec4_1, glm_mat4x4_1, glm_vec4_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat2x2_1, vec2_1, vec2_1, glm_mat2x2_1, glm_vec2_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, vec3_1, vec3_1, glm_mat3x3_1, glm_vec3_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, vec4_1, vec4_1, glm_mat4x4_1, glm_vec4_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x2_1, mat2x3_1, vec2_1, glm_mat3x2_1, glm_mat2x3_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, mat3x3_1, vec3_1, glm_mat3x3_1, glm_mat3x3_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x4_1, mat4x3_1, vec4_1, glm_mat3x4_1, glm_mat4x3_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x2_1, mat2x4_1, vec2_1, glm_mat4x2_1, glm_mat2x4_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x3_1, mat3x4_1, vec3_1, glm_mat4x3_1, glm_mat3x4_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, mat4x4_1, vec4_1, glm_mat4x4_1, glm_mat4x4_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat2x2_1, mat2x2_1, vec2_1, glm_mat2x2_1, glm_mat2x2_1, glm_vec2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, mat3x3_1, vec3_1, glm_mat3x3_1, glm_mat3x3_1, glm_vec3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, mat4x4_1, vec4_1, glm_mat4x4_1, glm_mat4x4_1, glm_vec4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec2_1, mat3x2_1, mat2x3_1, glm_vec2_1, glm_mat3x2_1, glm_mat2x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec3_1, mat3x3_1, mat3x3_1, glm_vec3_1, glm_mat3x3_1, glm_mat3x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec4_1, mat3x4_1, mat4x3_1, glm_vec4_1, glm_mat3x4_1, glm_mat4x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec2_1, mat4x2_1, mat2x4_1, glm_vec2_1, glm_mat4x2_1, glm_mat2x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec3_1, mat4x3_1, mat3x4_1, glm_vec3_1, glm_mat4x3_1, glm_mat3x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec4_1, mat4x4_1, mat4x4_1, glm_vec4_1, glm_mat4x4_1, glm_mat4x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec2_1, mat2x2_1, mat2x2_1, glm_vec2_1, glm_mat2x2_1, glm_mat2x2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec3_1, mat3x3_1, mat3x3_1, glm_vec3_1, glm_mat3x3_1, glm_mat3x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(vec4_1, mat4x4_1, mat4x4_1, glm_vec4_1, glm_mat4x4_1, glm_mat4x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x2_1, mat2x3_1, mat2x2_1, glm_mat3x2_1, glm_mat2x3_1, glm_mat2x2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, mat3x3_1, mat3x3_1, glm_mat3x3_1, glm_mat3x3_1, glm_mat3x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x4_1, mat4x3_1, mat4x4_1, glm_mat3x4_1, glm_mat4x3_1, glm_mat4x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x2_1, mat2x4_1, mat2x2_1, glm_mat4x2_1, glm_mat2x4_1, glm_mat2x2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x3_1, mat3x4_1, mat3x3_1, glm_mat4x3_1, glm_mat3x4_1, glm_mat3x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, mat4x4_1, mat4x4_1, glm_mat4x4_1, glm_mat4x4_1, glm_mat4x4_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat2x2_1, mat2x2_1, mat2x2_1, glm_mat2x2_1, glm_mat2x2_1, glm_mat2x2_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat3x3_1, mat3x3_1, mat3x3_1, glm_mat3x3_1, glm_mat3x3_1, glm_mat3x3_1);
        BasicArithmeticTestMatricesVectorsMultiplication(mat4x4_1, mat4x4_1, mat4x4_1, glm_mat4x4_1, glm_mat4x4_1, glm_mat4x4_1);
        BasicArithmeticTest(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTest(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTest(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
        BasicArithmeticTest(mat2x3_1, mat2x3_2, mat2x3_3, glm_mat2x3_1, glm_mat2x3_2, glm_mat2x3_3);
        BasicArithmeticTest(mat2x4_1, mat2x4_2, mat2x4_3, glm_mat2x4_1, glm_mat2x4_2, glm_mat2x4_3);
        BasicArithmeticTest(mat3x2_1, mat3x2_2, mat3x2_3, glm_mat3x2_1, glm_mat3x2_2, glm_mat3x2_3);
        BasicArithmeticTest(mat3x4_1, mat3x4_2, mat3x4_3, glm_mat3x4_1, glm_mat3x4_2, glm_mat3x4_3);
        BasicArithmeticTest(mat4x2_1, mat4x2_2, mat4x2_3, glm_mat4x2_1, glm_mat4x2_2, glm_mat4x2_3);
        BasicArithmeticTest(mat4x3_1, mat4x3_2, mat4x3_3, glm_mat4x3_1, glm_mat4x3_2, glm_mat4x3_3);
        BasicArithmeticTest(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTest(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTest(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
        BasicArithmeticTest(mat2x2_1, mat2x2_2, mat2x2_3, glm_mat2x2_1, glm_mat2x2_2, glm_mat2x2_3);
        BasicArithmeticTest(mat3x3_1, mat3x3_2, mat3x3_3, glm_mat3x3_1, glm_mat3x3_2, glm_mat3x3_3);
        BasicArithmeticTest(mat4x4_1, mat4x4_2, mat4x4_3, glm_mat4x4_1, glm_mat4x4_2, glm_mat4x4_3);
    }
}

TEST(TEST_MATRICES, TestBasicFunctions)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::mat2x2 glm_mat2x2_1 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_2 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_3 = random_float_arr<glm::mat2x2>(-100, 100);
        mat2x2 mat2x2_1 = create_from_other<mat2x2>(glm_mat2x2_1);
        mat2x2 mat2x2_2 = create_from_other<mat2x2>(glm_mat2x2_2);
        mat2x2 mat2x2_3 = create_from_other<mat2x2>(glm_mat2x2_3);
        glm::mat2x3 glm_mat2x3_1 = random_float_arr<glm::mat2x3>(-100, 100);
        glm::mat2x3 glm_mat2x3_2 = random_float_arr<glm::mat2x3>(-100, 100);
        glm::mat2x3 glm_mat2x3_3 = random_float_arr<glm::mat2x3>(-100, 100);
        mat2x3 mat2x3_1 = create_from_other<mat2x3>(glm_mat2x3_1);
        mat2x3 mat2x3_2 = create_from_other<mat2x3>(glm_mat2x3_2);
        mat2x3 mat2x3_3 = create_from_other<mat2x3>(glm_mat2x3_3);
        glm::mat2x4 glm_mat2x4_1 = random_float_arr<glm::mat2x4>(-100, 100);
        glm::mat2x4 glm_mat2x4_2 = random_float_arr<glm::mat2x4>(-100, 100);
        glm::mat2x4 glm_mat2x4_3 = random_float_arr<glm::mat2x4>(-100, 100);
        mat2x4 mat2x4_1 = create_from_other<mat2x4>(glm_mat2x4_1);
        mat2x4 mat2x4_2 = create_from_other<mat2x4>(glm_mat2x4_2);
        mat2x4 mat2x4_3 = create_from_other<mat2x4>(glm_mat2x4_3);
        glm::mat3x2 glm_mat3x2_1 = random_float_arr<glm::mat3x2>(-100, 100);
        glm::mat3x2 glm_mat3x2_2 = random_float_arr<glm::mat3x2>(-100, 100);
        glm::mat3x2 glm_mat3x2_3 = random_float_arr<glm::mat3x2>(-100, 100);
        mat3x2 mat3x2_1 = create_from_other<mat3x2>(glm_mat3x2_1);
        mat3x2 mat3x2_2 = create_from_other<mat3x2>(glm_mat3x2_2);
        mat3x2 mat3x2_3 = create_from_other<mat3x2>(glm_mat3x2_3);
        glm::mat3x3 glm_mat3x3_1 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_2 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_3 = random_float_arr<glm::mat3x3>(-100, 100);
        mat3x3 mat3x3_1 = create_from_other<mat3x3>(glm_mat3x3_1);
        mat3x3 mat3x3_2 = create_from_other<mat3x3>(glm_mat3x3_2);
        mat3x3 mat3x3_3 = create_from_other<mat3x3>(glm_mat3x3_3);
        glm::mat3x4 glm_mat3x4_1 = random_float_arr<glm::mat3x4>(-100, 100);
        glm::mat3x4 glm_mat3x4_2 = random_float_arr<glm::mat3x4>(-100, 100);
        glm::mat3x4 glm_mat3x4_3 = random_float_arr<glm::mat3x4>(-100, 100);
        mat3x4 mat3x4_1 = create_from_other<mat3x4>(glm_mat3x4_1);
        mat3x4 mat3x4_2 = create_from_other<mat3x4>(glm_mat3x4_2);
        mat3x4 mat3x4_3 = create_from_other<mat3x4>(glm_mat3x4_3);
        glm::mat4x2 glm_mat4x2_1 = random_float_arr<glm::mat4x2>(-100, 100);
        glm::mat4x2 glm_mat4x2_2 = random_float_arr<glm::mat4x2>(-100, 100);
        glm::mat4x2 glm_mat4x2_3 = random_float_arr<glm::mat4x2>(-100, 100);
        mat4x2 mat4x2_1 = create_from_other<mat4x2>(glm_mat4x2_1);
        mat4x2 mat4x2_2 = create_from_other<mat4x2>(glm_mat4x2_2);
        mat4x2 mat4x2_3 = create_from_other<mat4x2>(glm_mat4x2_3);
        glm::mat4x3 glm_mat4x3_1 = random_float_arr<glm::mat4x3>(-100, 100);
        glm::mat4x3 glm_mat4x3_2 = random_float_arr<glm::mat4x3>(-100, 100);
        glm::mat4x3 glm_mat4x3_3 = random_float_arr<glm::mat4x3>(-100, 100);
        mat4x3 mat4x3_1 = create_from_other<mat4x3>(glm_mat4x3_1);
        mat4x3 mat4x3_2 = create_from_other<mat4x3>(glm_mat4x3_2);
        mat4x3 mat4x3_3 = create_from_other<mat4x3>(glm_mat4x3_3);
        glm::mat4x4 glm_mat4x4_1 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_2 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_3 = random_float_arr<glm::mat4x4>(-100, 100);
        mat4x4 mat4x4_1 = create_from_other<mat4x4>(glm_mat4x4_1);
        mat4x4 mat4x4_2 = create_from_other<mat4x4>(glm_mat4x4_2);
        mat4x4 mat4x4_3 = create_from_other<mat4x4>(glm_mat4x4_3);
        glm::vec2 glm_vec2_1 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_2 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_3 = random_float_arr<glm::vec2>(-100, 100);
        vec2 vec2_1 = create_from_other<vec2>(glm_vec2_1);
        vec2 vec2_2 = create_from_other<vec2>(glm_vec2_2);
        vec2 vec2_3 = create_from_other<vec2>(glm_vec2_3);
        glm::vec3 glm_vec3_1 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_2 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_3 = random_float_arr<glm::vec3>(-100, 100);
        vec3 vec3_1 = create_from_other<vec3>(glm_vec3_1);
        vec3 vec3_2 = create_from_other<vec3>(glm_vec3_2);
        vec3 vec3_3 = create_from_other<vec3>(glm_vec3_3);
        glm::vec4 glm_vec4_1 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_2 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_3 = random_float_arr<glm::vec4>(-100, 100);
        vec4 vec4_1 = create_from_other<vec4>(glm_vec4_1);
        vec4 vec4_2 = create_from_other<vec4>(glm_vec4_2);
        vec4 vec4_3 = create_from_other<vec4>(glm_vec4_3);
        // test transpose
        ASSERT_TRUE(transpose(mat2x2_1) == glm::transpose(glm_mat2x2_1));
        ASSERT_TRUE(transpose(mat2x2_2) == glm::transpose(glm_mat2x2_2));
        ASSERT_TRUE(transpose(mat2x2_3) == glm::transpose(glm_mat2x2_3));
        ASSERT_TRUE(transpose(mat2x3_1) == glm::transpose(glm_mat2x3_1));
        ASSERT_TRUE(transpose(mat2x3_2) == glm::transpose(glm_mat2x3_2));
        ASSERT_TRUE(transpose(mat2x3_3) == glm::transpose(glm_mat2x3_3));
        ASSERT_TRUE(transpose(mat2x4_1) == glm::transpose(glm_mat2x4_1));
        ASSERT_TRUE(transpose(mat2x4_2) == glm::transpose(glm_mat2x4_2));
        ASSERT_TRUE(transpose(mat2x4_3) == glm::transpose(glm_mat2x4_3));
        ASSERT_TRUE(transpose(mat3x2_1) == glm::transpose(glm_mat3x2_1));
        ASSERT_TRUE(transpose(mat3x2_2) == glm::transpose(glm_mat3x2_2));
        ASSERT_TRUE(transpose(mat3x2_3) == glm::transpose(glm_mat3x2_3));
        ASSERT_TRUE(transpose(mat3x3_1) == glm::transpose(glm_mat3x3_1));
        ASSERT_TRUE(transpose(mat3x3_2) == glm::transpose(glm_mat3x3_2));
        ASSERT_TRUE(transpose(mat3x3_3) == glm::transpose(glm_mat3x3_3));
        ASSERT_TRUE(transpose(mat3x4_1) == glm::transpose(glm_mat3x4_1));
        ASSERT_TRUE(transpose(mat3x4_2) == glm::transpose(glm_mat3x4_2));
        ASSERT_TRUE(transpose(mat3x4_3) == glm::transpose(glm_mat3x4_3));
        ASSERT_TRUE(transpose(mat4x2_1) == glm::transpose(glm_mat4x2_1));
        ASSERT_TRUE(transpose(mat4x2_2) == glm::transpose(glm_mat4x2_2));
        ASSERT_TRUE(transpose(mat4x2_3) == glm::transpose(glm_mat4x2_3));
        ASSERT_TRUE(transpose(mat4x3_1) == glm::transpose(glm_mat4x3_1));
        ASSERT_TRUE(transpose(mat4x3_2) == glm::transpose(glm_mat4x3_2));
        ASSERT_TRUE(transpose(mat4x3_3) == glm::transpose(glm_mat4x3_3));
        ASSERT_TRUE(transpose(mat4x4_1) == glm::transpose(glm_mat4x4_1));
        ASSERT_TRUE(transpose(mat4x4_2) == glm::transpose(glm_mat4x4_2));
        ASSERT_TRUE(transpose(mat4x4_3) == glm::transpose(glm_mat4x4_3));
        ASSERT_TRUE(determinant(mat2x2_1) == glm::determinant(glm_mat2x2_1));
        ASSERT_TRUE(determinant(mat2x2_2) == glm::determinant(glm_mat2x2_2));
        ASSERT_TRUE(determinant(mat2x2_3) == glm::determinant(glm_mat2x2_3));
        ASSERT_TRUE(determinant(mat3x3_1) == glm::determinant(glm_mat3x3_1));
        ASSERT_TRUE(determinant(mat3x3_2) == glm::determinant(glm_mat3x3_2));
        ASSERT_TRUE(determinant(mat3x3_3) == glm::determinant(glm_mat3x3_3));
        ASSERT_TRUE(determinant(mat4x4_1) == glm::determinant(glm_mat4x4_1));
        ASSERT_TRUE(determinant(mat4x4_2) == glm::determinant(glm_mat4x4_2));
        ASSERT_TRUE(determinant(mat4x4_3) == glm::determinant(glm_mat4x4_3));
        ASSERT_TRUE(adjugate(mat2x2_1) == glm::adjugate(glm_mat2x2_1));
        ASSERT_TRUE(adjugate(mat2x2_2) == glm::adjugate(glm_mat2x2_2));
        ASSERT_TRUE(adjugate(mat2x2_3) == glm::adjugate(glm_mat2x2_3));
        ASSERT_TRUE(adjugate(mat3x3_1) == glm::adjugate(glm_mat3x3_1));
        ASSERT_TRUE(adjugate(mat3x3_2) == glm::adjugate(glm_mat3x3_2));
        ASSERT_TRUE(adjugate(mat3x3_3) == glm::adjugate(glm_mat3x3_3));
        ASSERT_TRUE(adjugate(mat4x4_1) == glm::adjugate(glm_mat4x4_1));
        ASSERT_TRUE(adjugate(mat4x4_2) == glm::adjugate(glm_mat4x4_2));
        ASSERT_TRUE(adjugate(mat4x4_3) == glm::adjugate(glm_mat4x4_3));
        ASSERT_TRUE(inverse(mat2x2_1) == glm::inverse(glm_mat2x2_1));
        ASSERT_TRUE(inverse(mat2x2_2) == glm::inverse(glm_mat2x2_2));
        ASSERT_TRUE(inverse(mat2x2_3) == glm::inverse(glm_mat2x2_3));
        ASSERT_TRUE(inverse(mat3x3_1) == glm::inverse(glm_mat3x3_1));
        ASSERT_TRUE(inverse(mat3x3_2) == glm::inverse(glm_mat3x3_2));
        ASSERT_TRUE(inverse(mat3x3_3) == glm::inverse(glm_mat3x3_3));
        ASSERT_TRUE(inverse(mat4x4_1) == glm::inverse(glm_mat4x4_1));
        ASSERT_TRUE(inverse(mat4x4_2) == glm::inverse(glm_mat4x4_2));
        ASSERT_TRUE(inverse(mat4x4_3) == glm::inverse(glm_mat4x4_3));
    }
}

TEST(TEST_MATRICES, TestMatrixTransformations)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::mat2x2 glm_mat2x2_1 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_2 = random_float_arr<glm::mat2x2>(-100, 100);
        glm::mat2x2 glm_mat2x2_3 = random_float_arr<glm::mat2x2>(-100, 100);
        mat2x2 mat2x2_1 = create_from_other<mat2x2>(glm_mat2x2_1);
        mat2x2 mat2x2_2 = create_from_other<mat2x2>(glm_mat2x2_2);
        mat2x2 mat2x2_3 = create_from_other<mat2x2>(glm_mat2x2_3);
        glm::mat3x3 glm_mat3x3_1 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_2 = random_float_arr<glm::mat3x3>(-100, 100);
        glm::mat3x3 glm_mat3x3_3 = random_float_arr<glm::mat3x3>(-100, 100);
        mat3x3 mat3x3_1 = create_from_other<mat3x3>(glm_mat3x3_1);
        mat3x3 mat3x3_2 = create_from_other<mat3x3>(glm_mat3x3_2);
        mat3x3 mat3x3_3 = create_from_other<mat3x3>(glm_mat3x3_3);
        glm::mat4x4 glm_mat4x4_1 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_2 = random_float_arr<glm::mat4x4>(-100, 100);
        glm::mat4x4 glm_mat4x4_3 = random_float_arr<glm::mat4x4>(-100, 100);
        mat4x4 mat4x4_1 = create_from_other<mat4x4>(glm_mat4x4_1);
        mat4x4 mat4x4_2 = create_from_other<mat4x4>(glm_mat4x4_2);
        mat4x4 mat4x4_3 = create_from_other<mat4x4>(glm_mat4x4_3);
        glm::vec2 glm_vec2_1 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_2 = random_float_arr<glm::vec2>(-100, 100);
        glm::vec2 glm_vec2_3 = random_float_arr<glm::vec2>(-100, 100);
        vec2 vec2_1 = create_from_other<vec2>(glm_vec2_1);
        vec2 vec2_2 = create_from_other<vec2>(glm_vec2_2);
        vec2 vec2_3 = create_from_other<vec2>(glm_vec2_3);
        glm::vec3 glm_vec3_1 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_2 = random_float_arr<glm::vec3>(-100, 100);
        glm::vec3 glm_vec3_3 = random_float_arr<glm::vec3>(-100, 100);
        vec3 vec3_1 = create_from_other<vec3>(glm_vec3_1);
        vec3 vec3_2 = create_from_other<vec3>(glm_vec3_2);
        vec3 vec3_3 = create_from_other<vec3>(glm_vec3_3);
        glm::vec4 glm_vec4_1 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_2 = random_float_arr<glm::vec4>(-100, 100);
        glm::vec4 glm_vec4_3 = random_float_arr<glm::vec4>(-100, 100);
        vec4 vec4_1 = create_from_other<vec4>(glm_vec4_1);
        vec4 vec4_2 = create_from_other<vec4>(glm_vec4_2);
        vec4 vec4_3 = create_from_other<vec4>(glm_vec4_3);
        float f1 = mal_toolkit::Random<float>(-100, 100);
        float f2 = mal_toolkit::Random<float>(-100, 100);
        float f3 = mal_toolkit::Random<float>(-100, 100);
        float f4 = mal_toolkit::Random<float>(-100, 100);
        float f5 = mal_toolkit::Random<float>(-100, 100);
        float f6 = mal_toolkit::Random<float>(-100, 100);

        // test translate
        ASSERT_TRUE(translate(mat4x4_1, vec3_1) == glm::translate(glm_mat4x4_1, glm_vec3_1));
        ASSERT_TRUE(translate(mat4x4_2, vec3_2) == glm::translate(glm_mat4x4_2, glm_vec3_2));
        ASSERT_TRUE(translate(mat4x4_3, vec3_3) == glm::translate(glm_mat4x4_3, glm_vec3_3));
        // test scale
        ASSERT_TRUE(scale(mat4x4_1, vec3_1) == glm::scale(glm_mat4x4_1, glm_vec3_1));
        ASSERT_TRUE(scale(mat4x4_2, vec3_2) == glm::scale(glm_mat4x4_2, glm_vec3_2));
        ASSERT_TRUE(scale(mat4x4_3, vec3_3) == glm::scale(glm_mat4x4_3, glm_vec3_3));
        // test rotate
        ASSERT_TRUE(rotate(mat4x4_1, f1, vec3_1) == glm::rotate(glm_mat4x4_1, f1, glm_vec3_1));
        ASSERT_TRUE(rotate(mat4x4_2, f2, vec3_2) == glm::rotate(glm_mat4x4_2, f2, glm_vec3_2));
        ASSERT_TRUE(rotate(mat4x4_3, f3, vec3_3) == glm::rotate(glm_mat4x4_3, f3, glm_vec3_3));
        // test perspective
        ASSERT_TRUE(perspective(f1, f2, f3, f4) == glm::perspectiveLH_ZO(f1, f2, f3, f4));
        // test ortho
        ASSERT_TRUE(ortho(f1, f2, f3, f4, f5, f6) == glm::orthoLH_ZO(f1, f2, f3, f4, f5, f6));
        // test look_at
        ASSERT_TRUE(vec3_1 == glm_vec3_1);
        ASSERT_TRUE(vec3_2 == glm_vec3_2);
        ASSERT_TRUE(vec3_3 == glm_vec3_3);
        auto a = look_at(vec3_1, vec3_2, vec3_3);
        auto b = glm::lookAtLH(glm_vec3_1, glm_vec3_2, glm_vec3_3);
        ASSERT_TRUE(look_at(vec3_1, vec3_2, vec3_3) == glm::lookAtLH(glm_vec3_1, glm_vec3_2, glm_vec3_3));
    }
}