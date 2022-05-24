#pragma once
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
  struct Plane
  {
  public:
    float Hit(math::Ray const &r, math::vec3 center, float radius) noexcept
    {
        return -1;
    }


    bool CheckIntersection(Transform const& transform, math::Intersection& i, math::Ray const& ray) const
    {
      float denom = math::dot(normal_, ray.direction());
      if( abs(denom) <= 1e-6f)
      {
          return false;
      }
      math::vec3 a = transform.position - ray.origin();
      float t = math::dot(a, normal_) / denom;
      if (t > i.t || t <= 0)
      {
          return false;
      }
      i.t = t;
      i.normal = normal_;
      i.point = ray.PointAtParameter(t);
      return true;
    }
    void update_plane(math::vec3 const &first, math::vec3 const &second)
    {
      math::vec3 temp = math::cross(first, second);
      if (math::length(temp) == 0)
      {
        throw std::invalid_argument("Input vectors can't be collinear!");
      }
      v_ = first;
      w_ = second;
      normal_ = math::normalize(temp);
    }
    [[nodiscard]] math::vec3 const &v() const noexcept { return v_; }
    [[nodiscard]] math::vec3 const &w() const noexcept { return w_; }
    [[nodiscard]] math::vec3 const &normal() const noexcept { return normal_; }

  private:
    math::vec3 v_;
    math::vec3 w_;
    math::vec3 normal_;
  };
}