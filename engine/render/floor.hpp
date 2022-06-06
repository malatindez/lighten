#pragma once
#include "math/plane.hpp"
#include "render/material.hpp"
namespace engine::render
{
    struct Floor
    {
        bool CheckIntersection(core::math::Intersection &i, core::math::Ray const &ray) const
        {
            core::math::Ray local = ray;
            local.direction() = (core::math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
            local.origin() = (core::math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();
            bool rv = plane.CheckIntersection(i, local);
            if (rv)
            {
                i.normal = normalize((core::math::vec4{i.normal, 0} * transform.model).as_vec<3>());
                i.point = (core::math::vec4{i.point, 1} * transform.model).as_vec<3>();
            }
            return rv;
        }
        render::Material material;
        components::Transform transform;
        core::math::Plane plane;
    };
} // namespace engine::render