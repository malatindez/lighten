#include "include/library-pch.hpp"
#include "pch.h"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>


using namespace engine::core::math;

template <size_t size, typename T>
void vec_test_basic_arithmetic_same_types()
{
    std::array<T, size> arr;
    for (int i = 0; i < arr.size(); ++i)
    {
        arr[i] = utils::Random<T>(1);
    }
    std::array<T, size> invalid_array = arr;
    vec<size, T> vec1{ invalid_array };
    vec<size, T> vec2{ std::move(invalid_array) };

    for (int i = 0; i < arr.size(); ++i)
    {
        ASSERT_TRUE(vec1[i] == vec2[i]);
        ASSERT_TRUE(vec1[i] == arr[i]);
    }
    ASSERT_TRUE(vec1 == vec2);
    ASSERT_TRUE((vec1 == vec<size, T>{arr}));
    for (int i = 0; i < arr.size(); ++i)
    {
        vec1[i] = utils::Random<T>(1);
    }

    vec<size, T> add{ vec1 + vec2 };
    vec<size, T> sub{ vec1 - vec2 };
    vec<size, T> mul{ vec1 * vec2 };
    vec<size, T> div{ vec1 / vec2 };
    vec<size, T> mod;
    if constexpr (std::is_integral_v<T>)
    {
        mod = vec<size, T>{ vec1 % vec2 };
    }

    for (int i = 0; i < arr.size(); ++i)
    {
        ASSERT_TRUE(static_cast<T>(vec1[i] + vec2[i]) == add[i]) << vec1 << std::endl
            << vec2 << std::endl
            << add << " " << i << " " << static_cast<T>(vec1[i] + vec2[i]) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] - vec2[i]) == sub[i]) << vec1 << std::endl
            << vec2 << std::endl
            << sub << " " << i << " " << static_cast<T>(vec1[i] - vec2[i]) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] * vec2[i]) == mul[i]) << vec1 << std::endl
            << vec2 << std::endl
            << mul << " " << i << " " << static_cast<T>(vec1[i] * vec2[i]) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] / vec2[i]) == div[i]) << vec1 << std::endl
            << vec2 << std::endl
            << div << " " << i << " " << static_cast<T>(vec1[i] / vec2[i]) << std::endl;
        if constexpr (std::is_integral_v<T>)
        {
            ASSERT_TRUE((vec1[i] % vec2[i]) == mod[i]) << vec1 << std::endl
                << vec2 << std::endl
                << mod << " " << i << " " << static_cast<T>(vec1[i] % vec2[i]) << std::endl;
        }
    }

    T random_number = utils::Random<T>(1);

    add = vec1 + random_number;
    sub = vec1 - random_number;
    mul = vec1 * random_number;
    div = vec1 / random_number;
    if constexpr (std::is_integral_v<T>)
    {
        mod = vec1 % random_number;
    }

    for (int i = 0; i < arr.size(); ++i)
    {
        ASSERT_TRUE(static_cast<T>(vec1[i] + random_number) == add[i]) << vec1 << std::endl
            << vec2 << std::endl
            << add << " " << i << " " << static_cast<T>(vec1[i] + random_number) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] - random_number) == sub[i]) << vec1 << std::endl
            << vec2 << std::endl
            << sub << " " << i << " " << static_cast<T>(vec1[i] - random_number) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] * random_number) == mul[i]) << vec1 << std::endl
            << vec2 << std::endl
            << mul << " " << i << " " << static_cast<T>(vec1[i] * random_number) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] / random_number) == div[i]) << vec1 << std::endl
            << vec2 << std::endl
            << div << " " << i << " " << static_cast<T>(vec1[i] / random_number) << std::endl;
        if constexpr (std::is_integral_v<T>)
        {
            ASSERT_TRUE((vec1[i] % random_number) == mod[i]) << vec1 << std::endl
                << vec2 << std::endl
                << mod << " " << i << " " << static_cast<T>(vec1[i] % random_number) << std::endl;
        }
    }
}
template <size_t size>
void vec_test_basic_arithmetic_same_types_all()
{
    vec_test_basic_arithmetic_same_types<size, uint16_t>();
    vec_test_basic_arithmetic_same_types<size, uint32_t>();
    vec_test_basic_arithmetic_same_types<size, uint64_t>();
    vec_test_basic_arithmetic_same_types<size, int16_t>();
    vec_test_basic_arithmetic_same_types<size, int32_t>();
    vec_test_basic_arithmetic_same_types<size, int64_t>();
    vec_test_basic_arithmetic_same_types<size, float>();
    vec_test_basic_arithmetic_same_types<size, double>();
}

TEST(TEST_VECTORS_ARITHMETIC, DefaultTestSameTypes)
{
    vec_test_basic_arithmetic_same_types_all<2>();
    vec_test_basic_arithmetic_same_types_all<3>();
    vec_test_basic_arithmetic_same_types_all<4>();
}

TEST(TEST_VECTORS_ARITHMETIC, RandomTestSameTypes)
{
    for (size_t i = 0; i < utils::RandomConstexpr(-1, 10, 25); i++)
    {
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(0, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(1, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(2, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(3, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(4, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(5, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(6, 2, 256)>();
        vec_test_basic_arithmetic_same_types_all<utils::RandomConstexpr(7, 2, 256)>();
    }
}
template <size_t size, typename T, typename U>
void vec_test_basic_arithmetic_different_types()
{
    vec<size, T> vec1{};
    vec<size, U> vec2{};

    for (int i = 0; i < size; ++i)
    {
        vec1[i] = utils::Random<T>(1);
    }
    for (int i = 0; i < size; ++i)
    {
        vec2[i] = utils::Random<U>(1);
    }
    vec<size, U> _test1{ vec1 };
    vec<size, T> _test2{ vec2 };

    for (int i = 0; i < size; ++i)
    {
        ASSERT_TRUE(_test1[i] == static_cast<U>(vec1[i]));
        ASSERT_TRUE(_test2[i] == static_cast<T>(vec2[i]));
    }

    vec<size, T> add{ vec1 + vec2 };
    vec<size, T> sub{ vec1 - vec2 };
    vec<size, T> mul{ vec1 * vec2 };
    vec<size, T> div{ vec1 / vec2 };
    vec<size, T> mod;
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)
    {
        mod = vec<size, T>{ vec1 % vec2 };
    }

    for (int i = 0; i < size; ++i)
    {
        ASSERT_TRUE(static_cast<T>(vec1[i] + vec2[i]) == add[i]) << vec1 << std::endl
            << vec2 << std::endl
            << add << " " << i << " " << static_cast<T>(vec1[i] + static_cast<T>(vec2[i])) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] - vec2[i]) == sub[i]) << vec1 << std::endl
            << vec2 << std::endl
            << sub << " " << i << " " << static_cast<T>(vec1[i] - static_cast<T>(vec2[i])) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] * vec2[i]) == mul[i]) << vec1 << std::endl
            << vec2 << std::endl
            << mul << " " << i << " " << static_cast<T>(vec1[i] * static_cast<T>(vec2[i])) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] / vec2[i]) == div[i]) << vec1 << std::endl
            << vec2 << std::endl
            << div << " " << i << " " << static_cast<T>(vec1[i] / static_cast<T>(vec2[i])) << std::endl;
        if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)
        {
            ASSERT_TRUE(static_cast<T>(vec1[i] % vec2[i]) == mod[i]) << vec1 << std::endl
                << vec2 << std::endl
                << mod << " " << i << " " << static_cast<T>(vec1[i] % static_cast<T>(vec2[i])) << std::endl;
        }
    }

    T random_number = utils::Random<T>(1);

    add = vec1 + random_number;
    sub = vec1 - random_number;
    mul = vec1 * random_number;
    div = vec1 / random_number;
    if constexpr (std::is_integral_v<T>)
    {
        mod = vec1 % random_number;
    }

    for (int i = 0; i < size; ++i)
    {
        ASSERT_TRUE(static_cast<T>(vec1[i] + random_number) == add[i]) << vec1 << std::endl
            << vec2 << std::endl
            << add << " " << i << " " << static_cast<T>(vec1[i] + static_cast<T>(random_number)) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] - random_number) == sub[i]) << vec1 << std::endl
            << vec2 << std::endl
            << sub << " " << i << " " << static_cast<T>(vec1[i] - static_cast<T>(random_number)) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] * random_number) == mul[i]) << vec1 << std::endl
            << vec2 << std::endl
            << mul << " " << i << " " << static_cast<T>(vec1[i] * static_cast<T>(random_number)) << std::endl;
        ASSERT_TRUE(static_cast<T>(vec1[i] / random_number) == div[i]) << vec1 << std::endl
            << vec2 << std::endl
            << div << " " << i << " " << static_cast<T>(vec1[i] / static_cast<T>(random_number)) << std::endl;
        if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)
        {
            ASSERT_TRUE((vec1[i] % random_number) == mod[i]) << vec1 << std::endl
                << vec2 << std::endl
                << mod << " " << i << " " << static_cast<T>(vec1[i] % static_cast<T>(random_number)) << std::endl;
        }
    }
}

template <size_t size, typename T>
void vec_test_basic_arithmetic_different_types_all_p2()
{
    vec_test_basic_arithmetic_different_types<size, T, uint16_t>();
    vec_test_basic_arithmetic_different_types<size, T, uint32_t>();
    vec_test_basic_arithmetic_different_types<size, T, uint64_t>();
    vec_test_basic_arithmetic_different_types<size, T, int16_t>();
    vec_test_basic_arithmetic_different_types<size, T, int32_t>();
    vec_test_basic_arithmetic_different_types<size, T, int64_t>();
    vec_test_basic_arithmetic_different_types<size, T, float>();
    vec_test_basic_arithmetic_different_types<size, T, double>();
}
template <size_t size>
void vec_test_basic_arithmetic_different_types_all()
{
    vec_test_basic_arithmetic_different_types_all_p2<size, uint16_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, uint32_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, uint64_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, int16_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, int32_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, int64_t>();
    vec_test_basic_arithmetic_different_types_all_p2<size, float>();
    vec_test_basic_arithmetic_different_types_all_p2<size, double>();
}
TEST(TEST_VECTORS_ARITHMETIC, DefaultTestDifferentTypes)
{
    vec_test_basic_arithmetic_different_types_all<2>();
    vec_test_basic_arithmetic_different_types_all<3>();
    vec_test_basic_arithmetic_different_types_all<4>();
}

TEST(TEST_VECTORS_ARITHMETIC, RandomTestDifferentTypes)
{
    for (size_t i = 0; i < utils::RandomConstexpr(-1, 10, 25); i++)
    {
        vec_test_basic_arithmetic_different_types_all<utils::RandomConstexpr(15, 4, 10)>();
        vec_test_basic_arithmetic_different_types_all<utils::RandomConstexpr(34, 4, 10)>();
    }
}

template<AnyVec V, int x, typename T, glm::qualifier Q>
bool operator==(V const &lhs, glm::vec<x, T, Q> const &rhs)
{
    static_assert(sizeof(V) == sizeof(glm::vec<x, T, Q>));
    for (int i = 0; i < x; ++i)
    {
        if (lhs[i] != rhs[i] && std::isnan(lhs[i]) != std::isnan(rhs[i]))
        {
            return false;
        }
    }
    return true;
}
template<AnyVec V, int x, typename T, glm::qualifier Q>
bool operator==(glm::vec<x, T, Q> const &lhs, V const &rhs)
{
    return rhs == lhs;
}
static int const kNumIterations = 100000;
TEST(TEST_VECTORS_GLM, TestBasicArithmetic)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::vec2 glm_vec2_1 = glm::vec2(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec2 glm_vec2_2 = glm::vec2(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec2 glm_vec2_3 = glm::vec2(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec3 glm_vec3_1 = glm::vec3(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec3 glm_vec3_2 = glm::vec3(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec3 glm_vec3_3 = glm::vec3(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec4 glm_vec4_1 = glm::vec4(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec4 glm_vec4_2 = glm::vec4(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec4 glm_vec4_3 = glm::vec4(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        vec2 vec2_1 = vec2(glm_vec2_1.x, glm_vec2_1.y);
        vec2 vec2_2 = vec2(glm_vec2_2.x, glm_vec2_2.y);
        vec2 vec2_3 = vec2(glm_vec2_3.x, glm_vec2_3.y);
        vec3 vec3_1 = vec3(glm_vec3_1.x, glm_vec3_1.y, glm_vec3_1.z);
        vec3 vec3_2 = vec3(glm_vec3_2.x, glm_vec3_2.y, glm_vec3_2.z);
        vec3 vec3_3 = vec3(glm_vec3_3.x, glm_vec3_3.y, glm_vec3_3.z);
        vec4 vec4_1 = vec4(glm_vec4_1.x, glm_vec4_1.y, glm_vec4_1.z, glm_vec4_1.w);
        vec4 vec4_2 = vec4(glm_vec4_2.x, glm_vec4_2.y, glm_vec4_2.z, glm_vec4_2.w);
        vec4 vec4_3 = vec4(glm_vec4_3.x, glm_vec4_3.y, glm_vec4_3.z, glm_vec4_3.w);
        ASSERT_TRUE(vec2_1 + vec2_2 == glm_vec2_1 + glm_vec2_2);
        ASSERT_TRUE(vec2_1 - vec2_2 == glm_vec2_1 - glm_vec2_2);
        ASSERT_TRUE(vec2_1 * vec2_2 == glm_vec2_1 * glm_vec2_2);
        ASSERT_TRUE(vec2_1 / vec2_2 == glm_vec2_1 / glm_vec2_2);
        ASSERT_TRUE(vec3_1 + vec3_2 == glm_vec3_1 + glm_vec3_2);
        ASSERT_TRUE(vec3_1 - vec3_2 == glm_vec3_1 - glm_vec3_2);
        ASSERT_TRUE(vec3_1 * vec3_2 == glm_vec3_1 * glm_vec3_2);
        ASSERT_TRUE(vec3_1 / vec3_2 == glm_vec3_1 / glm_vec3_2);
        ASSERT_TRUE(vec4_1 + vec4_2 == glm_vec4_1 + glm_vec4_2);
        ASSERT_TRUE(vec4_1 - vec4_2 == glm_vec4_1 - glm_vec4_2);
        ASSERT_TRUE(vec4_1 * vec4_2 == glm_vec4_1 * glm_vec4_2);
        ASSERT_TRUE(vec4_1 / vec4_2 == glm_vec4_1 / glm_vec4_2);
        ASSERT_TRUE(vec2_1 + vec2_2 + vec2_3 == glm_vec2_1 + glm_vec2_2 + glm_vec2_3);
        ASSERT_TRUE(vec2_1 - vec2_2 - vec2_3 == glm_vec2_1 - glm_vec2_2 - glm_vec2_3);
        ASSERT_TRUE(vec2_1 * vec2_2 * vec2_3 == glm_vec2_1 * glm_vec2_2 * glm_vec2_3);
        ASSERT_TRUE(vec2_1 / vec2_2 / vec2_3 == glm_vec2_1 / glm_vec2_2 / glm_vec2_3);
        ASSERT_TRUE(vec3_1 + vec3_2 + vec3_3 == glm_vec3_1 + glm_vec3_2 + glm_vec3_3);
        ASSERT_TRUE(vec3_1 - vec3_2 - vec3_3 == glm_vec3_1 - glm_vec3_2 - glm_vec3_3);
        ASSERT_TRUE(vec3_1 * vec3_2 * vec3_3 == glm_vec3_1 * glm_vec3_2 * glm_vec3_3);
        ASSERT_TRUE(vec3_1 / vec3_2 / vec3_3 == glm_vec3_1 / glm_vec3_2 / glm_vec3_3);
        ASSERT_TRUE(vec4_1 + vec4_2 + vec4_3 == glm_vec4_1 + glm_vec4_2 + glm_vec4_3);
        ASSERT_TRUE(vec4_1 - vec4_2 - vec4_3 == glm_vec4_1 - glm_vec4_2 - glm_vec4_3);
        ASSERT_TRUE(vec4_1 * vec4_2 * vec4_3 == glm_vec4_1 * glm_vec4_2 * glm_vec4_3);
        ASSERT_TRUE(vec4_1 / vec4_2 / vec4_3 == glm_vec4_1 / glm_vec4_2 / glm_vec4_3);
        ASSERT_TRUE(vec2_1 + vec2_2 * vec2_3 == glm_vec2_1 + glm_vec2_2 * glm_vec2_3);
        ASSERT_TRUE(vec2_1 - vec2_2 / vec2_3 == glm_vec2_1 - glm_vec2_2 / glm_vec2_3);
        ASSERT_TRUE(vec3_1 + vec3_2 * vec3_3 == glm_vec3_1 + glm_vec3_2 * glm_vec3_3);
        ASSERT_TRUE(vec3_1 - vec3_2 / vec3_3 == glm_vec3_1 - glm_vec3_2 / glm_vec3_3);
        ASSERT_TRUE(vec4_1 + vec4_2 * vec4_3 == glm_vec4_1 + glm_vec4_2 * glm_vec4_3);
        ASSERT_TRUE(vec4_1 - vec4_2 / vec4_3 == glm_vec4_1 - glm_vec4_2 / glm_vec4_3);
        ASSERT_TRUE(vec2_1 + vec2_2 * vec2_3 / vec2_1 == glm_vec2_1 + glm_vec2_2 * glm_vec2_3 / glm_vec2_1);
        ASSERT_TRUE(vec2_1 - vec2_2 / vec2_3 * vec2_1 == glm_vec2_1 - glm_vec2_2 / glm_vec2_3 * glm_vec2_1);
        ASSERT_TRUE(vec3_1 + vec3_2 * vec3_3 / vec3_1 == glm_vec3_1 + glm_vec3_2 * glm_vec3_3 / glm_vec3_1);
        ASSERT_TRUE(vec3_1 - vec3_2 / vec3_3 * vec3_1 == glm_vec3_1 - glm_vec3_2 / glm_vec3_3 * glm_vec3_1);
        ASSERT_TRUE(vec4_1 + vec4_2 * vec4_3 / vec4_1 == glm_vec4_1 + glm_vec4_2 * glm_vec4_3 / glm_vec4_1);
        ASSERT_TRUE(vec4_1 - vec4_2 / vec4_3 * vec4_1 == glm_vec4_1 - glm_vec4_2 / glm_vec4_3 * glm_vec4_1);

        ASSERT_TRUE(vec2_1 == glm_vec2_1);
        ASSERT_TRUE(vec2_2 == glm_vec2_2);
        ASSERT_TRUE(vec3_1 == glm_vec3_1);
        ASSERT_TRUE(vec3_2 == glm_vec3_2);
        ASSERT_TRUE(vec4_1 == glm_vec4_1);
        ASSERT_TRUE(vec4_2 == glm_vec4_2);
        vec2 vec2_add = vec2_1 + vec2_2;
        vec3 vec3_add = vec3_1 + vec3_2;
        vec4 vec4_add = vec4_1 + vec4_2;
        ASSERT_TRUE(vec2_add == glm_vec2_1 + glm_vec2_2);
        ASSERT_TRUE(vec3_add == glm_vec3_1 + glm_vec3_2);
        ASSERT_TRUE(vec4_add == glm_vec4_1 + glm_vec4_2);
        vec2 vec2_sub = vec2_1 - vec2_2;
        vec3 vec3_sub = vec3_1 - vec3_2;
        vec4 vec4_sub = vec4_1 - vec4_2;
        ASSERT_TRUE(vec2_sub == glm_vec2_1 - glm_vec2_2);
        ASSERT_TRUE(vec3_sub == glm_vec3_1 - glm_vec3_2);
        ASSERT_TRUE(vec4_sub == glm_vec4_1 - glm_vec4_2);
        vec2 vec2_mul = vec2_1 * vec2_2;
        vec3 vec3_mul = vec3_1 * vec3_2;
        vec4 vec4_mul = vec4_1 * vec4_2;
        ASSERT_TRUE(vec2_mul == glm_vec2_1 * glm_vec2_2);
        ASSERT_TRUE(vec3_mul == glm_vec3_1 * glm_vec3_2);
        ASSERT_TRUE(vec4_mul == glm_vec4_1 * glm_vec4_2);
        vec2 vec2_div = vec2_1 / vec2_2;
        vec3 vec3_div = vec3_1 / vec3_2;
        vec4 vec4_div = vec4_1 / vec4_2;
        ASSERT_TRUE(vec2_div == glm_vec2_1 / glm_vec2_2);
        ASSERT_TRUE(vec3_div == glm_vec3_1 / glm_vec3_2);
        ASSERT_TRUE(vec4_div == glm_vec4_1 / glm_vec4_2);
        vec2 vec2_add_scalar = vec2_1 + 5.0f;
        vec3 vec3_add_scalar = vec3_1 + 5.0f;
        vec4 vec4_add_scalar = vec4_1 + 5.0f;
        ASSERT_TRUE(vec2_add_scalar == glm_vec2_1 + 5.0f);
        ASSERT_TRUE(vec3_add_scalar == glm_vec3_1 + 5.0f);
        ASSERT_TRUE(vec4_add_scalar == glm_vec4_1 + 5.0f);
        vec2 vec2_sub_scalar = vec2_1 - 5.0f;
        vec3 vec3_sub_scalar = vec3_1 - 5.0f;
        vec4 vec4_sub_scalar = vec4_1 - 5.0f;
        ASSERT_TRUE(vec2_sub_scalar == glm_vec2_1 - 5.0f);
        ASSERT_TRUE(vec3_sub_scalar == glm_vec3_1 - 5.0f);
        ASSERT_TRUE(vec4_sub_scalar == glm_vec4_1 - 5.0f);
        vec2 vec2_mul_scalar = vec2_1 * 5.0f;
        vec3 vec3_mul_scalar = vec3_1 * 5.0f;
        vec4 vec4_mul_scalar = vec4_1 * 5.0f;
        ASSERT_TRUE(vec2_mul_scalar == glm_vec2_1 * 5.0f);
        ASSERT_TRUE(vec3_mul_scalar == glm_vec3_1 * 5.0f);
        ASSERT_TRUE(vec4_mul_scalar == glm_vec4_1 * 5.0f);
        vec2 vec2_div_scalar = vec2_1 / 5.0f;
        vec3 vec3_div_scalar = vec3_1 / 5.0f;
        vec4 vec4_div_scalar = vec4_1 / 5.0f;
        ASSERT_TRUE(vec2_div_scalar == glm_vec2_1 / 5.0f);
        ASSERT_TRUE(vec3_div_scalar == glm_vec3_1 / 5.0f);
        ASSERT_TRUE(vec4_div_scalar == glm_vec4_1 / 5.0f);
        vec2 vec2_add_scalar_2 = 5.0f + vec2_1;
        vec3 vec3_add_scalar_2 = 5.0f + vec3_1;
        vec4 vec4_add_scalar_2 = 5.0f + vec4_1;
        ASSERT_TRUE(vec2_add_scalar_2 == 5.0f + glm_vec2_1);
        ASSERT_TRUE(vec3_add_scalar_2 == 5.0f + glm_vec3_1);
        ASSERT_TRUE(vec4_add_scalar_2 == 5.0f + glm_vec4_1);
        vec2 vec2_sub_scalar_2 = 5.0f - vec2_1;
        vec3 vec3_sub_scalar_2 = 5.0f - vec3_1;
        vec4 vec4_sub_scalar_2 = 5.0f - vec4_1;
        ASSERT_TRUE(vec2_sub_scalar_2 == 5.0f - glm_vec2_1);
        ASSERT_TRUE(vec3_sub_scalar_2 == 5.0f - glm_vec3_1);
        ASSERT_TRUE(vec4_sub_scalar_2 == 5.0f - glm_vec4_1);
        vec2 vec2_mul_scalar_2 = 5.0f * vec2_1;
        vec3 vec3_mul_scalar_2 = 5.0f * vec3_1;
        vec4 vec4_mul_scalar_2 = 5.0f * vec4_1;
        ASSERT_TRUE(vec2_mul_scalar_2 == 5.0f * glm_vec2_1);
        ASSERT_TRUE(vec3_mul_scalar_2 == 5.0f * glm_vec3_1);
        ASSERT_TRUE(vec4_mul_scalar_2 == 5.0f * glm_vec4_1);
        vec2 vec2_div_scalar_2 = 5.0f / vec2_1;
        vec3 vec3_div_scalar_2 = 5.0f / vec3_1;
        vec4 vec4_div_scalar_2 = 5.0f / vec4_1;
        ASSERT_TRUE(vec2_div_scalar_2 == 5.0f / glm_vec2_1);
        ASSERT_TRUE(vec3_div_scalar_2 == 5.0f / glm_vec3_1);
        ASSERT_TRUE(vec4_div_scalar_2 == 5.0f / glm_vec4_1);
        vec2 vec2_add_assign = vec2_1;
        vec3 vec3_add_assign = vec3_1;
        vec4 vec4_add_assign = vec4_1;
        vec2_add_assign += vec2_2;
        vec3_add_assign += vec3_2;
        vec4_add_assign += vec4_2;
        ASSERT_TRUE(vec2_add_assign == glm_vec2_1 + glm_vec2_2);
        ASSERT_TRUE(vec3_add_assign == glm_vec3_1 + glm_vec3_2);
        ASSERT_TRUE(vec4_add_assign == glm_vec4_1 + glm_vec4_2);
        vec2 vec2_sub_assign = vec2_1;
        vec3 vec3_sub_assign = vec3_1;
        vec4 vec4_sub_assign = vec4_1;
        vec2_sub_assign -= vec2_2;
        vec3_sub_assign -= vec3_2;
        vec4_sub_assign -= vec4_2;
        ASSERT_TRUE(vec2_sub_assign == glm_vec2_1 - glm_vec2_2);
        ASSERT_TRUE(vec3_sub_assign == glm_vec3_1 - glm_vec3_2);
        ASSERT_TRUE(vec4_sub_assign == glm_vec4_1 - glm_vec4_2);
        vec2 vec2_mul_assign = vec2_1;
        vec3 vec3_mul_assign = vec3_1;
        vec4 vec4_mul_assign = vec4_1;
        vec2_mul_assign *= vec2_2;
        vec3_mul_assign *= vec3_2;
        vec4_mul_assign *= vec4_2;
        ASSERT_TRUE(vec2_mul_assign == glm_vec2_1 * glm_vec2_2);
        ASSERT_TRUE(vec3_mul_assign == glm_vec3_1 * glm_vec3_2);
        ASSERT_TRUE(vec4_mul_assign == glm_vec4_1 * glm_vec4_2);
        vec2 vec2_div_assign = vec2_1;
        vec3 vec3_div_assign = vec3_1;
        vec4 vec4_div_assign = vec4_1;
        vec2_div_assign /= vec2_2;
        vec3_div_assign /= vec3_2;
        vec4_div_assign /= vec4_2;
        ASSERT_TRUE(vec2_div_assign == glm_vec2_1 / glm_vec2_2);
        ASSERT_TRUE(vec3_div_assign == glm_vec3_1 / glm_vec3_2);
        ASSERT_TRUE(vec4_div_assign == glm_vec4_1 / glm_vec4_2);
        vec2 vec2_add_assign_scalar = vec2_1;
        vec3 vec3_add_assign_scalar = vec3_1;
        vec4 vec4_add_assign_scalar = vec4_1;
        vec2_add_assign_scalar += 5.0f;
        vec3_add_assign_scalar += 5.0f;
        vec4_add_assign_scalar += 5.0f;
        ASSERT_TRUE(vec2_add_assign_scalar == glm_vec2_1 + 5.0f);
        ASSERT_TRUE(vec3_add_assign_scalar == glm_vec3_1 + 5.0f);
        ASSERT_TRUE(vec4_add_assign_scalar == glm_vec4_1 + 5.0f);
        vec2 vec2_sub_assign_scalar = vec2_1;
        vec3 vec3_sub_assign_scalar = vec3_1;
        vec4 vec4_sub_assign_scalar = vec4_1;
        vec2_sub_assign_scalar -= 5.0f;
        vec3_sub_assign_scalar -= 5.0f;
        vec4_sub_assign_scalar -= 5.0f;
        ASSERT_TRUE(vec2_sub_assign_scalar == glm_vec2_1 - 5.0f);
        ASSERT_TRUE(vec3_sub_assign_scalar == glm_vec3_1 - 5.0f);
        ASSERT_TRUE(vec4_sub_assign_scalar == glm_vec4_1 - 5.0f);
        vec2 vec2_mul_assign_scalar = vec2_1;
        vec3 vec3_mul_assign_scalar = vec3_1;
        vec4 vec4_mul_assign_scalar = vec4_1;
        vec2_mul_assign_scalar *= 5.0f;
        vec3_mul_assign_scalar *= 5.0f;
        vec4_mul_assign_scalar *= 5.0f;
        ASSERT_TRUE(vec2_mul_assign_scalar == glm_vec2_1 * 5.0f);
        ASSERT_TRUE(vec3_mul_assign_scalar == glm_vec3_1 * 5.0f);
        ASSERT_TRUE(vec4_mul_assign_scalar == glm_vec4_1 * 5.0f);
        vec2 vec2_div_assign_scalar = vec2_1;
        vec3 vec3_div_assign_scalar = vec3_1;
        vec4 vec4_div_assign_scalar = vec4_1;
        vec2_div_assign_scalar /= 5.0f;
        vec3_div_assign_scalar /= 5.0f;
        vec4_div_assign_scalar /= 5.0f;
        ASSERT_TRUE(vec2_div_assign_scalar == glm_vec2_1 / 5.0f);
        ASSERT_TRUE(vec3_div_assign_scalar == glm_vec3_1 / 5.0f);
        ASSERT_TRUE(vec4_div_assign_scalar == glm_vec4_1 / 5.0f);
    }
}
TEST(TEST_VECTORS_GLM, TestTrigonometricFunctions)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::vec2 glm_vec2_1 = glm::vec2(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec2 glm_vec2_2 = glm::vec2(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec3 glm_vec3_1 = glm::vec3(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec3 glm_vec3_2 = glm::vec3(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec4 glm_vec4_1 = glm::vec4(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec4 glm_vec4_2 = glm::vec4(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        vec2 vec2_1 = vec2(glm_vec2_1.x, glm_vec2_1.y);
        vec2 vec2_2 = vec2(glm_vec2_2.x, glm_vec2_2.y);
        vec3 vec3_1 = vec3(glm_vec3_1.x, glm_vec3_1.y, glm_vec3_1.z);
        vec3 vec3_2 = vec3(glm_vec3_2.x, glm_vec3_2.y, glm_vec3_2.z);
        vec4 vec4_1 = vec4(glm_vec4_1.x, glm_vec4_1.y, glm_vec4_1.z, glm_vec4_1.w);
        vec4 vec4_2 = vec4(glm_vec4_2.x, glm_vec4_2.y, glm_vec4_2.z, glm_vec4_2.w);
        ASSERT_TRUE(sin(vec2_1) == glm::sin(glm_vec2_1));
        ASSERT_TRUE(sin(vec3_1) == glm::sin(glm_vec3_1));
        ASSERT_TRUE(sin(vec4_1) == glm::sin(glm_vec4_1));
        ASSERT_TRUE(cos(vec2_1) == glm::cos(glm_vec2_1));
        ASSERT_TRUE(cos(vec3_1) == glm::cos(glm_vec3_1));
        ASSERT_TRUE(cos(vec4_1) == glm::cos(glm_vec4_1));
        ASSERT_TRUE(tan(vec2_1) == glm::tan(glm_vec2_1));
        ASSERT_TRUE(tan(vec3_1) == glm::tan(glm_vec3_1));
        ASSERT_TRUE(tan(vec4_1) == glm::tan(glm_vec4_1));
        ASSERT_TRUE(asin(vec2_1) == glm::asin(glm_vec2_1));
        ASSERT_TRUE(asin(vec3_1) == glm::asin(glm_vec3_1));
        ASSERT_TRUE(asin(vec4_1) == glm::asin(glm_vec4_1));
        ASSERT_TRUE(acos(vec2_1) == glm::acos(glm_vec2_1));
        ASSERT_TRUE(acos(vec3_1) == glm::acos(glm_vec3_1));
        ASSERT_TRUE(acos(vec4_1) == glm::acos(glm_vec4_1));
        ASSERT_TRUE(atan(vec2_1) == glm::atan(glm_vec2_1));
        ASSERT_TRUE(atan(vec3_1) == glm::atan(glm_vec3_1));
        ASSERT_TRUE(atan(vec4_1) == glm::atan(glm_vec4_1));
        ASSERT_TRUE(atan(vec2_1, vec2_2) == glm::atan(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(atan(vec3_1, vec3_2) == glm::atan(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(atan(vec4_1, vec4_2) == glm::atan(glm_vec4_1, glm_vec4_2));
        // test hyperbolic functions
        ASSERT_TRUE(sinh(vec2_1) == glm::sinh(glm_vec2_1));
        ASSERT_TRUE(sinh(vec3_1) == glm::sinh(glm_vec3_1));
        ASSERT_TRUE(sinh(vec4_1) == glm::sinh(glm_vec4_1));
        ASSERT_TRUE(cosh(vec2_1) == glm::cosh(glm_vec2_1));
        ASSERT_TRUE(cosh(vec3_1) == glm::cosh(glm_vec3_1));
        ASSERT_TRUE(cosh(vec4_1) == glm::cosh(glm_vec4_1));
        ASSERT_TRUE(tanh(vec2_1) == glm::tanh(glm_vec2_1));
        ASSERT_TRUE(tanh(vec3_1) == glm::tanh(glm_vec3_1));
        ASSERT_TRUE(tanh(vec4_1) == glm::tanh(glm_vec4_1));
        ASSERT_TRUE(asinh(vec2_1) == glm::asinh(glm_vec2_1));
        ASSERT_TRUE(asinh(vec3_1) == glm::asinh(glm_vec3_1));
        ASSERT_TRUE(asinh(vec4_1) == glm::asinh(glm_vec4_1));
        ASSERT_TRUE(acosh(vec2_1) == glm::acosh(glm_vec2_1));
        ASSERT_TRUE(acosh(vec3_1) == glm::acosh(glm_vec3_1));
        ASSERT_TRUE(acosh(vec4_1) == glm::acosh(glm_vec4_1));
        ASSERT_TRUE(atanh(vec2_1) == glm::atanh(glm_vec2_1));
        ASSERT_TRUE(atanh(vec3_1) == glm::atanh(glm_vec3_1));
        ASSERT_TRUE(atanh(vec4_1) == glm::atanh(glm_vec4_1));
    }
}
TEST(TEST_VECTORS_GLM, TestExponentialFunctions)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::vec2 glm_vec2_1 = glm::vec2(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec2 glm_vec2_2 = glm::vec2(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec3 glm_vec3_1 = glm::vec3(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec3 glm_vec3_2 = glm::vec3(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec4 glm_vec4_1 = glm::vec4(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        glm::vec4 glm_vec4_2 = glm::vec4(utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100), utils::Random<float>(-100, 100));
        vec2 vec2_1 = vec2(glm_vec2_1.x, glm_vec2_1.y);
        vec2 vec2_2 = vec2(glm_vec2_2.x, glm_vec2_2.y);
        vec3 vec3_1 = vec3(glm_vec3_1.x, glm_vec3_1.y, glm_vec3_1.z);
        vec3 vec3_2 = vec3(glm_vec3_2.x, glm_vec3_2.y, glm_vec3_2.z);
        vec4 vec4_1 = vec4(glm_vec4_1.x, glm_vec4_1.y, glm_vec4_1.z, glm_vec4_1.w);
        vec4 vec4_2 = vec4(glm_vec4_2.x, glm_vec4_2.y, glm_vec4_2.z, glm_vec4_2.w);
        ASSERT_TRUE(pow(vec2_1, vec2_2) == glm::pow(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(pow(vec3_1, vec3_2) == glm::pow(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(pow(vec4_1, vec4_2) == glm::pow(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(exp(vec2_1) == glm::exp(glm_vec2_1));
        ASSERT_TRUE(exp(vec3_1) == glm::exp(glm_vec3_1));
        ASSERT_TRUE(exp(vec4_1) == glm::exp(glm_vec4_1));
        ASSERT_TRUE(log(vec2_1) == glm::log(glm_vec2_1));
        ASSERT_TRUE(log(vec3_1) == glm::log(glm_vec3_1));
        ASSERT_TRUE(log(vec4_1) == glm::log(glm_vec4_1));
        ASSERT_TRUE(exp2(vec2_1) == glm::exp2(glm_vec2_1));
        ASSERT_TRUE(exp2(vec3_1) == glm::exp2(glm_vec3_1));
        ASSERT_TRUE(exp2(vec4_1) == glm::exp2(glm_vec4_1));
        ASSERT_TRUE(log2(vec2_1) == glm::log2(glm_vec2_1));
        ASSERT_TRUE(log2(vec3_1) == glm::log2(glm_vec3_1));
        ASSERT_TRUE(log2(vec4_1) == glm::log2(glm_vec4_1));
        ASSERT_TRUE(sqrt(vec2_1) == glm::sqrt(glm_vec2_1));
        ASSERT_TRUE(sqrt(vec3_1) == glm::sqrt(glm_vec3_1));
        ASSERT_TRUE(sqrt(vec4_1) == glm::sqrt(glm_vec4_1));
        ASSERT_TRUE(inversesqrt(vec2_1) == glm::inversesqrt(glm_vec2_1));
        ASSERT_TRUE(inversesqrt(vec3_1) == glm::inversesqrt(glm_vec3_1));
        ASSERT_TRUE(inversesqrt(vec4_1) == glm::inversesqrt(glm_vec4_1));
    }
}
TEST(TEST_VECTORS_GLM, TestCommonFunctions)
{
    for (int i = 0; i < kNumIterations; i++)
    {
        glm::vec2 glm_vec2_1 = glm::vec2(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec2 glm_vec2_2 = glm::vec2(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec2 glm_vec2_3 = glm::vec2(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec3 glm_vec3_1 = glm::vec3(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec3 glm_vec3_2 = glm::vec3(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec3 glm_vec3_3 = glm::vec3(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec4 glm_vec4_1 = glm::vec4(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec4 glm_vec4_2 = glm::vec4(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        glm::vec4 glm_vec4_3 = glm::vec4(utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1), utils::Random<float>(-1, 1));
        vec2 vec2_1 = vec2(glm_vec2_1.x, glm_vec2_1.y);
        vec2 vec2_2 = vec2(glm_vec2_2.x, glm_vec2_2.y);
        vec2 vec2_3 = vec2(glm_vec2_3.x, glm_vec2_3.y);
        vec3 vec3_1 = vec3(glm_vec3_1.x, glm_vec3_1.y, glm_vec3_1.z);
        vec3 vec3_2 = vec3(glm_vec3_2.x, glm_vec3_2.y, glm_vec3_2.z);
        vec3 vec3_3 = vec3(glm_vec3_3.x, glm_vec3_3.y, glm_vec3_3.z);
        vec4 vec4_1 = vec4(glm_vec4_1.x, glm_vec4_1.y, glm_vec4_1.z, glm_vec4_1.w);
        vec4 vec4_2 = vec4(glm_vec4_2.x, glm_vec4_2.y, glm_vec4_2.z, glm_vec4_2.w);
        vec4 vec4_3 = vec4(glm_vec4_3.x, glm_vec4_3.y, glm_vec4_3.z, glm_vec4_3.w);
        ASSERT_TRUE(cross(vec3_1, vec3_2) == glm::cross(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(cross(vec3_2, vec3_3) == glm::cross(glm_vec3_2, glm_vec3_3));
        ASSERT_TRUE(cross(vec3_1, vec3_3) == glm::cross(glm_vec3_1, glm_vec3_3));
        ASSERT_TRUE(almost_equal(dot(vec2_1, vec2_2), glm::dot(glm_vec2_1, glm_vec2_2), 5));
        ASSERT_TRUE(almost_equal(dot(vec3_1, vec3_2), glm::dot(glm_vec3_1, glm_vec3_2), 10));
        ASSERT_TRUE(almost_equal(dot(vec4_1, vec4_2), glm::dot(glm_vec4_1, glm_vec4_2), 100000));
        ASSERT_TRUE(almost_equal(length(vec2_1), glm::length(glm_vec2_1)));
        ASSERT_TRUE(almost_equal(length(vec3_1), glm::length(glm_vec3_1)));
        ASSERT_TRUE(almost_equal(length(vec4_1), glm::length(glm_vec4_1)));
        ASSERT_TRUE(normalize(vec2_1) == glm::normalize(glm_vec2_1));
        ASSERT_TRUE(normalize(vec3_1) == glm::normalize(glm_vec3_1));
        ASSERT_TRUE(normalize(vec4_1) == glm::normalize(glm_vec4_1));
        ASSERT_TRUE(reflect(vec2_1, vec2_2) == glm::reflect(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(reflect(vec3_1, vec3_2) == glm::reflect(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(reflect(vec4_1, vec4_2) == glm::reflect(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(refract(vec2_1, vec2_2, 0.5f) == glm::refract(glm_vec2_1, glm_vec2_2, 0.5f));
        ASSERT_TRUE(refract(vec3_1, vec3_2, 0.5f) == glm::refract(glm_vec3_1, glm_vec3_2, 0.5f));
        ASSERT_TRUE(refract(vec4_1, vec4_2, 0.5f) == glm::refract(glm_vec4_1, glm_vec4_2, 0.5f));
        ASSERT_TRUE(unit_vector(vec2_1) == glm::normalize(glm_vec2_1));
        ASSERT_TRUE(unit_vector(vec3_1) == glm::normalize(glm_vec3_1));
        ASSERT_TRUE(unit_vector(vec4_1) == glm::normalize(glm_vec4_1));
        ASSERT_TRUE(almost_equal(angle(vec2_1, vec2_2), glm::angle(glm_vec2_1, glm_vec2_2), 100));
        ASSERT_TRUE(almost_equal(angle(vec3_1, vec3_2), glm::angle(glm_vec3_1, glm_vec3_2), 500));
        ASSERT_TRUE(almost_equal(angle(vec4_1, vec4_2), glm::angle(glm_vec4_1, glm_vec4_2), 100000));
        ASSERT_TRUE(almost_equal(distance(vec2_1, vec2_2), glm::distance(glm_vec2_1, glm_vec2_2)));
        ASSERT_TRUE(almost_equal(distance(vec3_1, vec3_2), glm::distance(glm_vec3_1, glm_vec3_2)));
        ASSERT_TRUE(almost_equal(distance(vec4_1, vec4_2), glm::distance(glm_vec4_1, glm_vec4_2)));
        ASSERT_TRUE(lerp(vec2_1, vec2_2, 0.5f) == glm::lerp(glm_vec2_1, glm_vec2_2, 0.5f));
        ASSERT_TRUE(lerp(vec3_1, vec3_2, 0.5f) == glm::lerp(glm_vec3_1, glm_vec3_2, 0.5f));
        ASSERT_TRUE(lerp(vec4_1, vec4_2, 0.5f) == glm::lerp(glm_vec4_1, glm_vec4_2, 0.5f));
        ASSERT_TRUE(max(vec2_1, vec2_2) == glm::max(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(max(vec3_1, vec3_2) == glm::max(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(max(vec4_1, vec4_2) == glm::max(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(min(vec2_1, vec2_2) == glm::min(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(min(vec3_1, vec3_2) == glm::min(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(min(vec4_1, vec4_2) == glm::min(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(mix(vec2_1, vec2_2, 0.5f) == glm::mix(glm_vec2_1, glm_vec2_2, 0.5f));
        ASSERT_TRUE(mix(vec3_1, vec3_2, 0.5f) == glm::mix(glm_vec3_1, glm_vec3_2, 0.5f));
        ASSERT_TRUE(mix(vec4_1, vec4_2, 0.5f) == glm::mix(glm_vec4_1, glm_vec4_2, 0.5f));
        ASSERT_TRUE(abs(vec2_1) == glm::abs(glm_vec2_1));
        ASSERT_TRUE(abs(vec3_1) == glm::abs(glm_vec3_1));
        ASSERT_TRUE(abs(vec4_1) == glm::abs(glm_vec4_1));
        ASSERT_TRUE(sign(vec2_1) == glm::sign(glm_vec2_1));
        ASSERT_TRUE(sign(vec3_1) == glm::sign(glm_vec3_1));
        ASSERT_TRUE(sign(vec4_1) == glm::sign(glm_vec4_1));
        ASSERT_TRUE(floor(vec2_1) == glm::floor(glm_vec2_1));
        ASSERT_TRUE(floor(vec3_1) == glm::floor(glm_vec3_1));
        ASSERT_TRUE(floor(vec4_1) == glm::floor(glm_vec4_1));
        ASSERT_TRUE(ceil(vec2_1) == glm::ceil(glm_vec2_1));
        ASSERT_TRUE(ceil(vec3_1) == glm::ceil(glm_vec3_1));
        ASSERT_TRUE(ceil(vec4_1) == glm::ceil(glm_vec4_1));
        ASSERT_TRUE(trunc(vec2_1) == glm::trunc(glm_vec2_1));
        ASSERT_TRUE(trunc(vec3_1) == glm::trunc(glm_vec3_1));
        ASSERT_TRUE(trunc(vec4_1) == glm::trunc(glm_vec4_1));
        ASSERT_TRUE(round(vec2_1) == glm::round(glm_vec2_1));
        ASSERT_TRUE(round(vec3_1) == glm::round(glm_vec3_1));
        ASSERT_TRUE(round(vec4_1) == glm::round(glm_vec4_1));
        ASSERT_TRUE(roundEven(vec2_1) == glm::roundEven(glm_vec2_1));
        ASSERT_TRUE(roundEven(vec3_1) == glm::roundEven(glm_vec3_1));
        ASSERT_TRUE(roundEven(vec4_1) == glm::roundEven(glm_vec4_1));
        ASSERT_TRUE(fract(vec2_1) == glm::fract(glm_vec2_1));
        ASSERT_TRUE(fract(vec3_1) == glm::fract(glm_vec3_1));
        ASSERT_TRUE(fract(vec4_1) == glm::fract(glm_vec4_1));
        ASSERT_TRUE(mod(vec2_1, vec2_2) == glm::mod(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(mod(vec3_1, vec3_2) == glm::mod(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(mod(vec4_1, vec4_2) == glm::mod(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(mod(vec2_1, 2.0f) == glm::mod(glm_vec2_1, 2.0f));
        ASSERT_TRUE(mod(vec3_1, 2.0f) == glm::mod(glm_vec3_1, 2.0f));
        ASSERT_TRUE(mod(vec4_1, 2.0f) == glm::mod(glm_vec4_1, 2.0f));
        ASSERT_TRUE(min(vec2_1, vec2_2) == glm::min(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(min(vec3_1, vec3_2) == glm::min(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(min(vec4_1, vec4_2) == glm::min(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(min(vec2_1, 2.0f) == glm::min(glm_vec2_1, 2.0f));
        ASSERT_TRUE(min(vec3_1, 2.0f) == glm::min(glm_vec3_1, 2.0f));
        ASSERT_TRUE(min(vec4_1, 2.0f) == glm::min(glm_vec4_1, 2.0f));
        ASSERT_TRUE(max(vec2_1, vec2_2) == glm::max(glm_vec2_1, glm_vec2_2));
        ASSERT_TRUE(max(vec3_1, vec3_2) == glm::max(glm_vec3_1, glm_vec3_2));
        ASSERT_TRUE(max(vec4_1, vec4_2) == glm::max(glm_vec4_1, glm_vec4_2));
        ASSERT_TRUE(max(vec2_1, 2.0f) == glm::max(glm_vec2_1, 2.0f));
        ASSERT_TRUE(max(vec3_1, 2.0f) == glm::max(glm_vec3_1, 2.0f));
        ASSERT_TRUE(max(vec4_1, 2.0f) == glm::max(glm_vec4_1, 2.0f));
        ASSERT_TRUE(clamp(vec2_1, vec2_2, vec2_3) == glm::clamp(glm_vec2_1, glm_vec2_2, glm_vec2_3));
        ASSERT_TRUE(clamp(vec3_1, vec3_2, vec3_3) == glm::clamp(glm_vec3_1, glm_vec3_2, glm_vec3_3));
        ASSERT_TRUE(clamp(vec4_1, vec4_2, vec4_3) == glm::clamp(glm_vec4_1, glm_vec4_2, glm_vec4_3));
        ASSERT_TRUE(clamp(vec2_1, 2.0f, 3.0f) == glm::clamp(glm_vec2_1, 2.0f, 3.0f));
        ASSERT_TRUE(clamp(vec3_1, 2.0f, 3.0f) == glm::clamp(glm_vec3_1, 2.0f, 3.0f));
        ASSERT_TRUE(clamp(vec4_1, 2.0f, 3.0f) == glm::clamp(glm_vec4_1, 2.0f, 3.0f));
        ASSERT_TRUE(mix(vec2_1, vec2_2, 0.5f) == glm::mix(glm_vec2_1, glm_vec2_2, 0.5f));
        ASSERT_TRUE(mix(vec3_1, vec3_2, 0.5f) == glm::mix(glm_vec3_1, glm_vec3_2, 0.5f));
        ASSERT_TRUE(mix(vec4_1, vec4_2, 0.5f) == glm::mix(glm_vec4_1, glm_vec4_2, 0.5f));
    }
}