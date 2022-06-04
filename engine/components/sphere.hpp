#pragma once
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
    struct Sphere
    {
    public:
        static constexpr float Hit(math::Ray const &r, math::vec3 center,
                                   float radius) noexcept
        {
            // TODO
            // figure out how can we adjust this formula to the sphere scaled by
            // different factors
            const math::vec3 oc = r.origin() - center;
            const float a = math::dot(r.direction(), r.direction());
            const float b = math::dot(oc, r.direction());
            const float c = math::dot(oc, oc) - radius;
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

        static bool CheckIntersection(Transform const &transform,
                                      math::Intersection &i, math::Ray const &ray)
        {
            float t = Hit(ray, transform.position, transform.scale.x);

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
    };
} // namespace engine::components