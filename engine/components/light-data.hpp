#pragma once
#include "math.hpp"
namespace engine::components
{
    struct LightData
    {
        using Color = math::vec3;
        Color color;
        math::Ray const ray;
        math::vec3 const point;
        math::vec3 const normal;
        math::vec3 const view_dir; // normalize(point - ray.origin())
    };
} // namespace engine