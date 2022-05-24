#pragma once
#include "math.hpp"
#include "misc/color.hpp"
namespace engine::components
{
    struct LightData
    {
        Color color;
        math::Ray const ray;
        math::vec3 const point;
        math::vec3 const normal;
        math::vec3 const view_dir; // normalize(point - ray.origin())
    };
} // namespace engine