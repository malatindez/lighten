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
        static float Hit(math::Ray const& r, math::vec3 center, float radius) noexcept
        {
            // TODO
            // figure out how can we adjust this formula to the sphere scaled by different factors
            math::vec3 oc = r.origin() - center;
            float a = math::dot(r.direction(), r.direction());
            float b = math::dot(oc, r.direction());
            float c = math::dot(oc, oc) - radius;
            float discriminant = b * b - a * c;
            if (discriminant < 0)
            {
                return -1.0f;
            }
            float d = sqrt(discriminant);
            if(-b - d < 0.0f)
            {
                return (-b + d) / a;
            }
            return (-b - d) / a;
            
        }

        static bool CheckIntersection(Transform &transform, math::Intersection& i, math::Ray const& ray)
        {
            if (math::length(ray.origin() - transform.position) - transform.scale.x > i.t)
            {
                return false;
            }
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
    };
}