#pragma once
#include "core/math.hpp"
#include "core/math/ray.hpp"
#include "render/color.hpp"
namespace engine::render
{
    struct LightData
    {
        core::math::Ray const ray;
        core::math::vec3 const point;
        core::math::vec3 const normal;
        core::math::vec3 const view_dir; // point - ray.origin()
    };
} // namespace engine::components