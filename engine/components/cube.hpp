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

        static bool CheckIntersection(Transform const &transform, math::Intersection &i, math::Ray const &ray)
        {
            math::Ray local = ray;
            local.direction() = (math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
            local.origin() = (math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();
            bool rv = false;
            for (auto const &triangle : triangles_)
            {
                rv |= triangle.CheckIntersection(transform, i, local);
            }
            if (rv)
            {
                i.normal = normalize((math::vec4{i.normal, 0} * transform.model).as_vec<3>());
                i.point = (math::vec4{i.point, 1} * transform.model).as_vec<3>();
            }
            return rv;
        }

    private:
        static std::array<Triangle, 12> triangles_;
    };
}