
#include "render/uniform-buffer.hpp"
#include "core/math.hpp"
#include "direct3d11/direct3d11.hpp"
#include "include/pch.hpp"
#include "pch.h"
#include "utils/utils.hpp"
#include <filesystem>
#include <fstream>
#include <misc/ini.hpp>
#include <queue>
#include <set>
#include "utils.hpp"
using namespace engine::core::math;
using namespace engine::render;

class UNIFORM_BUFFER_TEST : public ::testing::Test
{
public:
    virtual ~UNIFORM_BUFFER_TEST() = default;

    static void SetUpTestSuite()
    {
        engine::direct3d::Init();
    }

    static void TearDownTestSuite()
    {
        engine::direct3d::Deinit();
    }
};

TEST(UNIFORM_BUFFER_TEST, BasicTest)
{
    int x = 5, y = 3;
    bool z = false;
    vec2 vector { 16, 2 };
    struct test_data
    {
        int x, y;
        bool z;
    } data {
        .x = 5,
        .y = 1,
        .z = false };
    UniformBuffer<int, int> a;
    UniformBuffer<int, int> b { x, y };
    UniformBuffer<int, int> c { false };
    UniformBuffer<int, int> d { x, y, false };
    UniformBuffer<int, int, bool> e;
    UniformBuffer<int, int, bool> f { x, y, false };
    UniformBuffer<int, int, bool> g { false };
    UniformBuffer<int, int, bool> h { x, y, true, false };
    a.Update(vector.data.data(), a.kSize);
    a.Update(-1, 3);
    b.Update(vector.data.data(), a.kSize);
    b.Update(x, y);
    c.Update(vector.data.data(), a.kSize);
    c.Update(x, y);
    d.Update(vector.data.data(), a.kSize);
    d.Update(x, y);

    e.Update(1, 2, false);
    e.Update(&data, e.kSize);
    f.Update(x, y, z);
    f.Update(&data, e.kSize);
    g.Update(-5, 2, true);
    g.Update(x, y, z);
    h.Update(100, 5, false);
    h.Update(x, y, z);
}

TEST(UNIFORM_BUFFER_TEST, MathTest)
{
    vec2 first { 1, 2 };
    vec3 second { first, 2 };
    vec4 third { second, 5 };
    mat2 fourth { first, 1, 2 };
    mat3 fifth { 1, third, third };
    mat4 sixth { third, third, second, second, 2, 3 };
    UniformBuffer<vec2> a { first };
    UniformBuffer<vec3> b { second };
    UniformBuffer<vec4> c { third };
    UniformBuffer<mat2> d { fourth };
    UniformBuffer<mat3> e { fifth };
    UniformBuffer<mat4> f { sixth };
    a.Update(&first, a.kSize);
    b.Update(&second, b.kSize);
    c.Update(&third, c.kSize);
    d.Update(&fourth, d.kSize);
    e.Update(&fifth, e.kSize);
    f.Update(&sixth, f.kSize);
}
struct TypeHelper
{
private:
    using helper = std::tuple<
        vec2, vec3, vec4, mat2,
        mat3, mat4, mat2x3, mat3x2,
        mat3x4, mat4x3,
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        float, double, long double>;
public:
    static constexpr size_t amount = std::tuple_size_v<helper>;
    template<size_t id>
    struct TypeAt
    {
        using type = std::tuple_element<id, helper>::type;
    };
};
template<size_t seed>
using RandomType = TypeHelper::TypeAt<utils::RandomConstexpr(seed, 0, TypeHelper::amount)>::type;

template<typename... Args, size_t... is>
void RandomTest(std::index_sequence<is...> const &)
{
    UniformBuffer<Args...> buffer;
    std::tuple<Args...> tuple;
    void *data = malloc(buffer.kSize);
    buffer.Update((std::get<is>(tuple))...);
    buffer.Update(data, buffer.kSize);
    free(data);
}
template<typename... Args>
void RandomTestPrep()
{
    RandomTest<Args...>(std::make_index_sequence<engine::utils::parameter_pack_info<Args...>::amount>{});
}
template<size_t seed>
void RandomTests()
{
    RandomTestPrep<RandomType<seed + 1>>();
    RandomTestPrep<RandomType<seed + 2>, RandomType<seed + 3>>();
    if constexpr (utils::RandomConstexpr(seed, 0, 1) == 0)
    {
        RandomTestPrep<RandomType<seed + 4>, RandomType<seed + 5>, RandomType<seed + 6>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 2) == 0)
    {
        RandomTestPrep<RandomType<seed + 7>, RandomType<seed + 8>, RandomType<seed + 9>, RandomType<seed + 10>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 3) == 0)
    {
        RandomTestPrep<RandomType<seed + 11>, RandomType<seed + 12>, RandomType<seed + 13>, RandomType<seed + 14>,
            RandomType<seed + 15>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 4) == 0)
    {
        RandomTestPrep<RandomType<seed + 16>, RandomType<seed + 17>, RandomType<seed + 18>, RandomType<seed + 19>,
            RandomType<seed + 20>, RandomType<seed + 21>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 5) == 0)
    {
        RandomTestPrep<RandomType<seed + 16>, RandomType<seed + 17>, RandomType<seed + 18>, RandomType<seed + 19>,
            RandomType<seed + 20>, RandomType<seed + 21>, RandomType<seed + 22>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 7) == 0)
    {
        RandomTestPrep<RandomType<seed + 23>, RandomType<seed + 24>, RandomType<seed + 25>, RandomType<seed + 26>,
            RandomType<seed + 27>, RandomType<seed + 28>, RandomType<seed + 29>, RandomType<seed + 30>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 8) == 0)
    {
        RandomTestPrep<RandomType<seed + 31>, RandomType<seed + 32>, RandomType<seed + 33>, RandomType<seed + 34>,
            RandomType<seed + 35>, RandomType<seed + 36>, RandomType<seed + 37>, RandomType<seed + 38>,
            RandomType<seed + 39>>();
    }
    if constexpr (utils::RandomConstexpr(seed, 0, 10) == 0)
    {
        RandomTestPrep<RandomType<seed + 40>, RandomType<seed + 41>, RandomType<seed + 42>, RandomType<seed + 43>,
            RandomType<seed + 44>, RandomType<seed + 45>, RandomType<seed + 46>, RandomType<seed + 47>,
            RandomType<seed + 48>, RandomType<seed + 49>>();
    }
}
TEST(UNIFORM_BUFFER_TEST, RandomTest)
{
    RandomTests<utils::RandomConstexpr(2, 5, 100000)>();
}
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    engine::direct3d::Init();
    int rv = RUN_ALL_TESTS();
    engine::direct3d::Deinit();
    return rv;
}