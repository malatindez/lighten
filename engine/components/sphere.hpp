#pragma once
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
    struct Sphere
    {
    public:
        static constexpr float Hit(core::math::Ray const &r, core::math::vec3 center,
                                   float radius) noexcept
        {
            const core::math::vec3 oc = r.origin() - center;
            const float a = core::math::dot(r.direction(), r.direction());
            const float b = core::math::dot(oc, r.direction());
            const float c = core::math::dot(oc, oc) - radius;
            const float discriminant = b * b - a * c;
            if (discriminant < 0)
            {
                return -1.0f;
            }
            const float d = std::sqrt(discriminant);
            float rv0 = (-b - d) / a;
            float rv1 = (-b + d) / a;
            if (rv0 > rv1)
            {
                std::swap(rv0, rv1);
            }
            if (rv0 < 0)
            {
                return rv1;
            }
            return rv0;
        }

        bool CheckIntersection(Transform const &transform, core::math::Intersection &i, core::math::Ray const &ray) const
        {
            core::math::Ray local = ray;
            local.direction() = (core::math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
            local.origin() = (core::math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();
            float t = Hit(local, position, radius);

            if (t > i.t || t < 0)
            {
                return false;
            }
            i.t = t;
            i.point = ray.PointAtParameter(t);
            i.normal = normalize(i.point - transform.position);
            return true;
        }
        
        render::Material material;
        core::math::vec3 position{0};
        float radius{1};
    };
} // namespace engine::components