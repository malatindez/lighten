#pragma once
#include "math.hpp"
#include "math/ray.hpp"
namespace engine::math
{
    class Sphere
    {
    public:
        Sphere(vec3 center, float radius) : center_(center), radius_(radius) {}
        float Hit(Ray const &r) const noexcept
        {
            vec3 oc = r.origin() - center_;
            float a = dot(r.direction(), r.direction());
            float b = 2.0f * dot(oc, r.direction());
            float c = dot(oc, oc) - radius_ * radius_;
            float discriminant = b * b - 4 * a * c;
            if (discriminant < 0)
            {
                return -1.0f;
            }
            else
            {
                return (-b - sqrt(discriminant)) / (2.0f * a);
            }
        }
        [[nodiscard]] inline float radius() const noexcept { return radius_; }
        [[nodiscard]] inline vec3 center() const noexcept { return center_; }
        void SetRadius(float radius) noexcept { radius_ = radius; }
        void SetCenter(vec3 center) noexcept { center_ = center; }

    private:
        vec3 center_;
        float radius_;
    };
} // namespace engine::math