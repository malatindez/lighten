#include "misc/scene.hpp"
#include "components/cube.hpp"
#include "components/directional-light.hpp"
#include "components/plane.hpp"
#include "components/point-light.hpp"
#include "components/sphere.hpp"
#include "components/spot-light.hpp"

using namespace engine::math;

namespace engine
{
  void Scene::UpdateScene() noexcept { update_scene = true; }

  void Scene::Draw(components::Camera const& cam, BitmapWindow &window)
  {
    if (!update_scene)
    {
      // return;
    }
    update_scene = false;
    vec3 origin{0};
    ivec2 window_size = window.size();
    uint32_t *bitmap = window.bitmap().data();
    vec4 BL{-1, -1, 1, 1}; 
    vec4 BR{1, -1, 1, 1}; 
    vec4 TL{-1, 1, 1, 1}; 
    BL = BL * cam.inv_view_projection;
    BR = BR * cam.inv_view_projection;
    TL = TL * cam.inv_view_projection;
    BL /= BL.w;
    BR /= BR.w;
    TL /= TL.w;
    math::vec4 BR_BL = BR - BL;
    math::vec4 TL_BL = TL - BL;

    for (int j = window_size.y - 1; j >= 0; j--)
    {
      for (int i = 0; i < window_size.x; i++)
      {
        float u = ((static_cast<float>(i)) / static_cast<float>(window_size.x));
        float v = ((static_cast<float>(j)) / static_cast<float>(window_size.y));
        math::vec4 a = BL + BR_BL * u + TL_BL * v;
        Ray r(origin, normalize(a.as_vec<3>()));
        
        auto intersection = FindIntersection(r);
        if (!intersection.exists())
        {
          bitmap[size_t(j) * window_size.x + i] = 0;
          continue;
        }

        components::LightData ld{
            .color = math::vec3{0, 0, 0},
            .ray = r,
            .point = intersection.point,
            .normal = intersection.normal,
            .view_dir = normalize(intersection.point - r.origin())};
        registry.view<components::DirectionalLight>().each(
            [&ld](auto entity, auto &dirlight) { dirlight.UpdateColor(ld); });
        registry.view<components::Transform, components::SpotLight>().each(
            [&ld](auto entity, auto &transform, auto &spot_light) { spot_light.UpdateColor(transform, ld); });
        registry.view<components::Transform, components::PointLight>().each(
            [&ld](auto entity, auto &transform, auto &point_light) { point_light.UpdateColor(transform, ld); });
        ivec3 color{ 255.99f * ld.color };
        color %= 256;
        color.r <<= 16;
        color.g <<= 8;

        bitmap[size_t(j) * window_size.x + i] = color.r | color.g | color.b;
      }
    }
  }

  math::Intersection Scene::FindIntersection(math::Ray const &ray)
  {
    math::Intersection nearest;
    nearest.reset();
    for (auto &&[entity, transform] : registry.group<components::Transform, components::Sphere>().each())
    {
      if(components::Sphere::CheckIntersection(transform, nearest, ray))
      {
          return nearest;
      }
    }
    for (auto &&[entity, transform, plane] : registry.view<components::Transform, components::Plane>().each())
    {
      if(plane.CheckIntersection(transform, nearest, ray))
      {
          return nearest;
      }
    }
    for (auto &&[entity, transform] : registry.view<components::Transform, components::Cube>().each())
    {
      if(components::Cube::CheckIntersection(transform, nearest, ray))
      {
          return nearest;
      }
    }
    return nearest;
  }
} // namespace engine