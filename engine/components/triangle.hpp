#pragma once
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
    class Triangle
    {
    public:
        Triangle(math::vec3 &a, math::vec3 &b, math::vec3 &c, math::vec3 &normal) : points_{a, b, c}, normal_(normal) { }
        bool CheckIntersection(Transform const &, math::Intersection &i, math::Ray const &ray) const noexcept
        {
            assert(!std::_Is_nan(normal_.x));
            float ndotdir = math::dot(normal_, ray.direction());
            if (fabs(ndotdir) < 1e-6f)
            {
                return false;
            }
            float t = math::dot(points_[0].get() - ray.origin(), normal_) / ndotdir;
            if (t > i.t || t < 0)
            {
                return false;
            }
            math::vec3 P = ray.PointAtParameter(t);
            math::vec3 c;
            math::vec3 edge0 = points_[1].get() - points_[0].get();
            math::vec3 vp0 = P - points_[0].get();
            c = math::cross(edge0, vp0);
            if (math::dot(normal_, c) < 0.0f)
            {
                return false;
            }
            math::vec3 edge1 = points_[2].get() - points_[1].get();
            math::vec3 vp1 = P - points_[1].get();
            c = math::cross(edge1, vp1);
            if (math::dot(normal_, c) < 0.0f)
            {
                return false;
            }
            math::vec3 edge2 = points_[0].get() - points_[2].get();
            math::vec3 vp2 = P - points_[2].get();
            c = math::cross(edge2, vp2);
            if (math::dot(normal_, c) < 0.0f)
            {
                return false;
            }
            i.point = P;
            i.normal = normal_;
            i.t = t;
            return true;
        }
    private:
        std::array<std::reference_wrapper<math::vec3>, 3> points_;
        math::vec3 &normal_;
        // TODO
        // create vector class which uses reference wrappers to wrap vec<n, T>
        // use it here so we can use less memory and apply this class to a mesh
    };
}