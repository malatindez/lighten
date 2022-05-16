#pragma once
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
namespace engine::components
{
    struct Cube
    {
        [[nodiscard]] static bool CheckIntersection(Transform &transform, math::Intersection& i, math::Ray const& ray) {
            return false;
        }
    };
}