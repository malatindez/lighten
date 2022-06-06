#pragma once
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/sphere.hpp"
#include "transform.hpp"
namespace engine::components
{
    struct SphereRenderer
    {
    public:
        bool CheckIntersection(Transform const &transform, core::math::Intersection &i, core::math::Ray const &ray) const
        {
            core::math::Ray local = ray;
            local.direction() = (core::math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
            local.origin() = (core::math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();
            bool rv = sphere.CheckIntersection(i, local);
            if (rv)
            {
                i.normal = normalize((core::math::vec4{i.normal, 0} * transform.model).as_vec<3>());
                i.point = (core::math::vec4{i.point, 1} * transform.model).as_vec<3>();
            }
            return rv;
        }
        core::math::Sphere sphere;
        render::Material material;
    };
} // namespace engine::components