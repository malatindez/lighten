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


    [[nodiscard]] bool CheckIntersection(Transform &transform, math::Intersection& i, math::Ray const& ray)
    {
      if (math::length(ray.origin() - transform.position) < i.t)
      {
        return false;
      }
      float t = Hit(ray, transform.position, transform.scale.x);

      if (t < i.t)
      {
        return false;
      }
      i.t = t;
      i.point = ray.direction() * t;
      return true;
    }
    void update_plane(math::vec3 const &first, math::vec3 const &second, math::vec3 const &)
    {
      math::vec3 temp = math::cross(first, second);
      if (math::length(temp) == 0)
      {
        throw std::invalid_argument("Input vectors can't be collinear!");
      }
      v_ = first;
      w_ = second;
      cross_ = temp;
    }
    [[nodiscard]] math::vec3 const &v() const noexcept { return v_; }
    [[nodiscard]] math::vec3 const &w() const noexcept { return w_; }
    [[nodiscard]] math::vec3 const &cross() const noexcept { return cross_; }

  private:
    math::vec3 v_;
    math::vec3 w_;
    math::vec3 cross_;
  };
}