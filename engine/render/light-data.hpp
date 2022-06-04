#pragma once
#include "math.hpp"
#include "render/color.hpp"
#include "math/ray.hpp"
namespace engine::render
{
    struct LightData
    {
        Color color;
        core::math::Ray const ray;
        core::math::vec3 const point;
        core::math::vec3 const normal;
        core::math::vec3 const view_dir; // normalize(point - ray.origin()))
    };
} // namespace engine::components