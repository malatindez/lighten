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