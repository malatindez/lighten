#pragma once
#include "components/triangle.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
#include <array>
#include <memory>
namespace engine::components
{
    class Cube
    {
    public:
        Cube() = default;

        static bool CheckIntersection(Transform const &transform,
                                      math::Intersection &i, math::Ray const &ray);

    private:
        static std::array<Triangle, 12> triangles_;
    };
} // namespace engine::components