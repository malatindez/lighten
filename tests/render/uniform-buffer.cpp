#define ENGINE_TEST
#include "render/uniform-buffer.hpp"
#include "direct3d11/direct3d11.hpp"
#include "core/engine.hpp"
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
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    engine::direct3d::api::public_init();
    int rv = RUN_ALL_TESTS();
    engine::direct3d::api::public_deinit();
    return rv;
}