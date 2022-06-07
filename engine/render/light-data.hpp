#pragma once
#include "math.hpp"
#include "math/ray.hpp"
#include "render/color.hpp"
namespace engine::render
{
    struct LightData
    {
        Color color;
        core::math::Ray const ray;
        core::math::vec3 const point;
        core::math::vec3 const normal;
        core::math::vec3 const view_dir; // point - ray.origin()
    };
} // namespace engine::components