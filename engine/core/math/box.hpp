#pragma once

#include "../math.hpp"

namespace engine::core::math
{
    struct Box
    {
        vec3 min;
        vec3 max;

        static constexpr float Inf = std::numeric_limits<float>::infinity();
        static constexpr Box empty() { return  { vec3{ Inf, Inf, Inf }, vec3{ -Inf, -Inf, -Inf } }; }
        static constexpr Box unit() { return  { vec3{ -1.f, -1.f, -1.f }, vec3{ 1.f, 1.f, 1.f } }; }

        vec3 size() const { return max - min; }
        vec3 center() const { return (min + max) / 2.f; }
        float radius() const { return length(size()) / 2.f; }

        void reset()
        {
            constexpr float maxf = std::numeric_limits<float>::max();
            min = vec3{ maxf , maxf , maxf };
            max = -min;
        }

        void expand(const Box &other)
        {
            expand(other.min);
            expand(other.max);
        }

        void expand(const vec3 &point)
        {
            min = math::min(min, point);
            max = math::max(max, point);
        }

        bool contains(const vec3 &P)
        {
            return
                min[0] <= P[0] && P[0] <= max[0] &&
                min[1] <= P[1] && P[1] <= max[1] &&
                min[2] <= P[2] && P[2] <= max[2];
        }
        inline bool Intersect(Ray const &ray, float &nearest_t) const
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            auto const &bounds = reinterpret_cast<std::array<vec3, 2> const &>(*this);

            tmin = (bounds[ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tmax = (bounds[1ll - ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tymin = (bounds[ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;
            tymax = (bounds[1ll - ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (bounds[ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;
            tzmax = (bounds[1ll - ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            float t = tmin;

            if (t < 0)
            {
                t = tmax;
                if (t < 0 || t > nearest_t)
                    return false;
            }
            nearest_t = t;
            return true;
        }

        inline bool Intersect(Ray const &ray, Intersection &nearest) const
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            auto const &bounds = reinterpret_cast<std::array<vec3, 2> const &>(*this);

            tmin = (bounds[ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tmax = (bounds[1ll - ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tymin = (bounds[ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;
            tymax = (bounds[1ll - ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (bounds[ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;
            tzmax = (bounds[1ll - ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            float t = tmin;

            if (t < 0)
            {
                t = tmax;
                if (t < 0 || t > nearest.t)
                    return false;
            }
            nearest.t = t;
            nearest.point = ray.PointAtParameter(t);
            return true;
        }
    };
}