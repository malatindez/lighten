#include "core/math.hpp"
#include "pch.h"
#include "utils.hpp"
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
    vec<size, T> vec1 { invalid_array };
    vec<size, T> vec2 { std::move(invalid_array) };

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

    vec<size, T> add { vec1 + vec2 };
    vec<size, T> sub { vec1 - vec2 };
    vec<size, T> mul { vec1 * vec2 };
    vec<size, T> div { vec1 / vec2 };
    vec<size, T> mod;
    if constexpr (std::is_integral_v<T>)
    {
        mod = vec<size, T> { vec1 % vec2 };
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
    vec<size, T> vec1 {};
    vec<size, U> vec2 {};

    for (int i = 0; i < size; ++i)
    {
        vec1[i] = utils::Random<T>(1);
    }
    for (int i = 0; i < size; ++i)
    {
        vec2[i] = utils::Random<U>(1);
    }
    vec<size, U> _test1 { vec1 };
    vec<size, T> _test2 { vec2 };

    for (int i = 0; i < size; ++i)
    {
        ASSERT_TRUE(_test1[i] == static_cast<U>(vec1[i]));
        ASSERT_TRUE(_test2[i] == static_cast<T>(vec2[i]));
    }

    vec<size, T> add { vec1 + vec2 };
    vec<size, T> sub { vec1 - vec2 };
    vec<size, T> mul { vec1 * vec2 };
    vec<size, T> div { vec1 / vec2 };
    vec<size, T> mod;
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)
    {
        mod = vec<size, T> { vec1 % vec2 };
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
        vec_test_basic_arithmetic_different_types_all<utils::RandomConstexpr(15, 2, 256)>();
        vec_test_basic_arithmetic_different_types_all<utils::RandomConstexpr(34, 2, 256)>();
    }
}

template <size_t size, typename T>
void vec_reference_test()
{
    std::array<T, size> arr;
    for (int i = 0; i < arr.size(); ++i)
    {
        arr[i] = utils::Random<T>(1);
    }
    vec<size, T> vec1 { arr };
    rvec<size, T> vec2 { vec1 };
    rvec<size, const T> vec3 { vec2 };
    rvec<size, const T> vec4 { vec1 };

    ASSERT_TRUE(vec1 == vec2) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_TRUE(vec2 == vec3) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_TRUE(vec4 == vec1) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_FALSE(vec1 != vec2);

    for (int i = 0; i < arr.size(); ++i)
    {
        ASSERT_TRUE(vec3[i] == vec4[i]);
        ASSERT_TRUE(vec2[i] == vec3[i]);
        ASSERT_TRUE(vec1[i] == vec2[i]);
        ASSERT_TRUE(vec1[i] == arr[i]);
    }

    for (int i = 0; i < arr.size(); ++i)
    {
        vec2[i] = utils::Random<T>(1);
    }

    ASSERT_TRUE(vec1 == vec2) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_TRUE(vec2 == vec3) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_TRUE(vec4 == vec1) << vec1 << std::endl
        << vec2 << std::endl;
    ASSERT_FALSE(vec1 != vec2);

    for (int i = 0; i < arr.size(); ++i)
    {
        ASSERT_TRUE(vec3[i] == vec4[i]);
        ASSERT_TRUE(vec2[i] == vec3[i]);
        ASSERT_TRUE(vec1[i] == vec2[i]);
        ASSERT_TRUE(vec1[i] != arr[i]);
    }

    rvec<2, const T> vec5 { vec4 };
    ASSERT_TRUE(vec5.x == vec1[0] && vec5.y == vec1[1]);
}

TEST(TEST_REFERENCE_VECTORS, DefaultTest)
{
    vec_reference_test<2, uint32_t>();
    vec_reference_test<3, uint32_t>();
    vec_reference_test<4, uint32_t>();
    vec_reference_test<2, float>();
    vec_reference_test<3, float>();
    vec_reference_test<4, float>();
    vec_reference_test<2, double>();
    vec_reference_test<3, double>();
    vec_reference_test<4, double>();
}

TEST(TEST_VECTORS, DefaultMathTest)
{
    for (int j = 0; j < 1000; j++)
    {
        vec4 vec1 { -4.01542568, -0.347009659, 6.97055626, -9.18363762 };
        rvec4 rvec1 { vec1 };
        crvec4 rcvec1 { rvec1 };
        vec4 vec2 { -9.63412666,-0.568608284,4.52160454,-0.382324219 };
        rvec4 rvec2 { vec2 };
        crvec4 rcvec2 { rvec2 };
        /* for (int i = 0; i < 4; ++i)
         {
             vec1[i] = utils::Random<float>(-1.0e1f, 1.0e1f);
             vec2[i] = utils::Random<float>(-1.0e1f, 1.0e1f);
         }*/
        ASSERT_TRUE(squared_length(vec1) == squared_length(rvec1) && squared_length(rvec1) == squared_length(rcvec1));
        ASSERT_TRUE(squared_length(vec2) == squared_length(rvec2) && squared_length(rvec2) == squared_length(rcvec2));
        ASSERT_TRUE(squared_length(vec2) == vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z + vec2.w * vec2.w);
        ASSERT_TRUE(length(vec1) == length(rvec1) && length(rvec1) == length(rcvec1));
        ASSERT_TRUE(length(vec2) == length(rvec2) && length(rvec2) == length(rcvec2));
        ASSERT_TRUE(length(vec1) == sqrt(squared_length(vec1)) && length(vec2) == sqrt(squared_length(vec2)));
        ASSERT_TRUE(normalize(vec2) == normalize(rvec2) && normalize(rvec2) == normalize(rcvec2) && almost_equal(length(normalize(rcvec2)), 1.0f));
        ASSERT_TRUE(normalize(vec2) == normalize(rvec2) && normalize(rvec2) == normalize(rcvec2) && almost_equal(length(normalize(rcvec2)), 1.0f));
        ASSERT_TRUE(dot(vec1, rcvec2) == dot(vec2, rcvec1) && dot(rvec2, rvec1) == dot(vec2, vec1) && dot(rcvec1, rcvec2) == dot(rvec1, rvec2) && dot(vec1, rcvec2) == dot(rvec2, rvec1));
        float a = angle(vec1, rcvec2);
        float b = angle(vec2, rcvec1);
        float g = angle(vec1, rcvec2);
        float h = angle(rvec2, rvec1);
        ASSERT_TRUE(angle(vec1, rcvec2) == angle(vec2, rcvec1) && angle(rvec2, rvec1) == angle(vec2, vec1) && angle(rcvec1, rcvec2) == angle(rvec1, rvec2) && angle(vec1, rcvec2) == angle(rvec2, rvec1));
    }
}