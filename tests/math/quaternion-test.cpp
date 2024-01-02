#include "include/library-pch.hpp"
#include "pch.h"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace lighten::core::math;

static int const kNumIterations = 10000;

template <typename T>
T random_float_arr(float from = -100, float to = 100)
{
    T return_value;
    auto &arr = reinterpret_cast<std::array<float, size_t(sizeof(T) / sizeof(float))> &>(return_value);
    for (auto &v : arr)
    {
        v = utils::Random<float>(from, to);
    }
    return return_value;
}
template <AnyMat M, int x, int y, typename T, glm::qualifier Q>
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
template <AnyVec V, int x, typename T, glm::qualifier Q>
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
quat create_from_other(glm::quat const &source)
{
    static_assert(sizeof(quat) == sizeof(glm::quat), "Size of vectors must be equal");
    quat return_value(source.w, source.x, source.y, source.z);
    return return_value;
}
template <AnyMat M, int x, int y, typename T, glm::qualifier Q>
bool operator==(M const &lhs, glm::mat<x, y, T, Q> const &rhs)
{
    static_assert(sizeof(M) == sizeof(glm::mat<x, y, T, Q>));
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            if (!almost_equal(lhs[i][j], rhs[i][j], 1000000) || std::isnan(lhs[i][j]) != std::isnan(rhs[i][j]))
            {
                return false;
            }
        }
    }
    return true;
}
template <AnyVec V, int x, typename T, glm::qualifier Q>
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
bool operator==(quat const &lhs, glm::quat const &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!almost_equal(lhs[i], rhs[i], 1000000) && std::isnan(lhs[i]) != std::isnan(rhs[i]))
        {
            return false;
        }
    }
    return true;
}
template <AnyMat M, size_t x, size_t y, typename T, glm::qualifier Q>
bool operator==(glm::mat<x, y, T, Q> const &lhs, M const &rhs)
{
    return rhs == lhs;
}
template <AnyVec V, size_t x, typename T, glm::qualifier Q>
bool operator==(glm::vec<x, T, Q> const &lhs, V const &rhs)
{
    return rhs == lhs;
}
bool operator==(glm::quat const &lhs, quat const &rhs)
{
    return rhs == lhs;
}
TEST(TEST_QUATERNIONS, TestBasicArithmetic)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::quat glm_quat1 = random_float_arr<glm::quat>();
        glm::quat glm_quat2 = random_float_arr<glm::quat>();
        glm::quat glm_quat3 = random_float_arr<glm::quat>();
        quat quat1 = create_from_other(glm_quat1);
        quat quat2 = create_from_other(glm_quat2);
        quat quat3 = create_from_other(glm_quat3);
        ASSERT_TRUE(quat1 + quat2 == glm_quat1 + glm_quat2);
        ASSERT_TRUE(quat1 - quat2 == glm_quat1 - glm_quat2);
        ASSERT_TRUE(quat1 * quat2 == glm_quat1 * glm_quat2);
        ASSERT_TRUE(quat1 * quat2 * quat3 == glm_quat1 * glm_quat2 * glm_quat3);
        ASSERT_TRUE(quat1 * quat2 + quat3 == glm_quat1 * glm_quat2 + glm_quat3);
        ASSERT_TRUE(quat1 * quat2 - quat3 == glm_quat1 * glm_quat2 - glm_quat3);
        float scalar = utils::Random<float>(-100, 100);
        ASSERT_TRUE(quat1 * scalar == glm_quat1 * scalar);
        ASSERT_TRUE(quat1 / scalar == glm_quat1 / scalar);
        ASSERT_TRUE(scalar * quat1 == scalar * glm_quat1);
    }
}
TEST(TEST_QUATERNIONS, TestFunctions)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::quat glm_quat1 = random_float_arr<glm::quat>();
        glm::quat glm_quat2 = random_float_arr<glm::quat>();
        glm::quat glm_quat3 = random_float_arr<glm::quat>();
        glm::vec3 glm_vec1 = random_float_arr<glm::vec3>();
        glm::vec3 glm_vec2 = random_float_arr<glm::vec3>();
        glm::vec3 glm_vec3 = random_float_arr<glm::vec3>();
        quat quat1 = create_from_other(glm_quat1);
        quat quat2 = create_from_other(glm_quat2);
        quat quat3 = create_from_other(glm_quat3);
        vec3 vec_1 = create_from_other<vec3>(glm_vec1);
        vec3 vec_2 = create_from_other<vec3>(glm_vec2);
        vec3 vec_3 = create_from_other<vec3>(glm_vec3);
        ASSERT_TRUE(almost_equal(glm::dot(glm_quat1, glm_quat2), dot(quat1, quat2), 1000000));
        ASSERT_TRUE(almost_equal(glm::dot(glm_quat2, glm_quat3), dot(quat2, quat3), 1000000));
        ASSERT_TRUE(almost_equal(glm::dot(glm_quat1, glm_quat3), dot(quat1, quat3), 1000000));
        ASSERT_TRUE(glm::conjugate(glm_quat1) == conjugate(quat1));
        ASSERT_TRUE(glm::conjugate(glm_quat2) == conjugate(quat2));
        ASSERT_TRUE(glm::conjugate(glm_quat3) == conjugate(quat3));
        ASSERT_TRUE(glm::inverse(glm_quat1) == inverse(quat1));
        ASSERT_TRUE(glm::inverse(glm_quat2) == inverse(quat2));
        ASSERT_TRUE(glm::inverse(glm_quat3) == inverse(quat3));
        ASSERT_TRUE(glm::cross(glm_quat1, glm_quat2) == cross(quat1, quat2));
        ASSERT_TRUE(glm::cross(glm_quat2, glm_quat3) == cross(quat2, quat3));
        ASSERT_TRUE(glm::cross(glm_quat1, glm_quat3) == cross(quat1, quat3));
        ASSERT_TRUE(glm::cross(glm_vec1, glm_quat1) == cross(vec_1, quat1));
        ASSERT_TRUE(glm::cross(glm_vec2, glm_quat2) == cross(vec_2, quat2));
        ASSERT_TRUE(glm::cross(glm_vec3, glm_quat3) == cross(vec_3, quat3));
        ASSERT_TRUE(glm::cross(glm_quat1, glm_vec1) == cross(quat1, vec_1));
        ASSERT_TRUE(glm::cross(glm_quat2, glm_vec2) == cross(quat2, vec_2));
        ASSERT_TRUE(glm::cross(glm_quat3, glm_vec3) == cross(quat3, vec_3));
        ASSERT_TRUE(glm::eulerAngles(glm_quat1) == QuaternionToEuler(quat1));
        ASSERT_TRUE(glm::eulerAngles(glm_quat2) == QuaternionToEuler(quat2));
        ASSERT_TRUE(glm::eulerAngles(glm_quat3) == QuaternionToEuler(quat3));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z)) == QuaternionFromEuler(vec_1.x, vec_1.y, vec_1.z));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z)) == QuaternionFromEuler(vec_1));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z)) == QuaternionFromEuler(vec_2.x, vec_2.y, vec_2.z));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z)) == QuaternionFromEuler(vec_2));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z)) == QuaternionFromEuler(vec_3.x, vec_3.y, vec_3.z));
        ASSERT_TRUE(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z)) == QuaternionFromEuler(vec_3));
        ASSERT_TRUE(glm::angle(glm_quat1) == angle(quat1) || std::isnan(glm::angle(glm_quat1)) == std::isnan(angle(quat1)));
        ASSERT_TRUE(glm::angle(glm_quat2) == angle(quat2) || std::isnan(glm::angle(glm_quat2)) == std::isnan(angle(quat2)));
        ASSERT_TRUE(glm::angle(glm_quat3) == angle(quat3) || std::isnan(glm::angle(glm_quat3)) == std::isnan(angle(quat3)));
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z))) == QuaternionFromEuler(vec_1.x, vec_1.y, vec_1.z).as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z))) == QuaternionFromEuler(vec_1).as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z))) == QuaternionFromEuler(vec_2.x, vec_2.y, vec_2.z).as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z))) == QuaternionFromEuler(vec_2).as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z))) == QuaternionFromEuler(vec_3.x, vec_3.y, vec_3.z).as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z))) == QuaternionFromEuler(vec_3).as_mat4());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z))) == QuaternionFromEuler(vec_1.x, vec_1.y, vec_1.z).as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec1.x, glm_vec1.y, glm_vec1.z))) == QuaternionFromEuler(vec_1).as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z))) == QuaternionFromEuler(vec_2.x, vec_2.y, vec_2.z).as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec2.x, glm_vec2.y, glm_vec2.z))) == QuaternionFromEuler(vec_2).as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z))) == QuaternionFromEuler(vec_3.x, vec_3.y, vec_3.z).as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm::quat(glm::vec3(glm_vec3.x, glm_vec3.y, glm_vec3.z))) == QuaternionFromEuler(vec_3).as_mat3());
        ASSERT_TRUE(glm::conjugate(glm_quat1) == conjugate(glm_quat1));
        ASSERT_TRUE(glm::conjugate(glm_quat2) == conjugate(glm_quat2));
        ASSERT_TRUE(glm::conjugate(glm_quat3) == conjugate(glm_quat3));
        ASSERT_TRUE(glm::inverse(glm_quat1) == inverse(glm_quat1));
        ASSERT_TRUE(glm::inverse(glm_quat2) == inverse(glm_quat2));
        ASSERT_TRUE(glm::inverse(glm_quat3) == inverse(glm_quat3));
        ASSERT_TRUE(almost_equal(glm::length(glm_quat1), length(quat1), 100000));
        ASSERT_TRUE(almost_equal(glm::length(glm_quat2), length(quat2), 100000));
        ASSERT_TRUE(almost_equal(glm::length(glm_quat3), length(quat3), 100000));
        ASSERT_TRUE(glm::normalize(glm_quat1) == normalize(quat1));
        ASSERT_TRUE(glm::normalize(glm_quat2) == normalize(quat2));
        ASSERT_TRUE(glm::normalize(glm_quat3) == normalize(quat3));
        ASSERT_TRUE(glm::mat4_cast(glm_quat1) == quat1.as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm_quat2) == quat2.as_mat4());
        ASSERT_TRUE(glm::mat4_cast(glm_quat3) == quat3.as_mat4());
        ASSERT_TRUE(glm::mat3_cast(glm_quat1) == quat1.as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm_quat2) == quat2.as_mat3());
        ASSERT_TRUE(glm::mat3_cast(glm_quat3) == quat3.as_mat3());
    }
}