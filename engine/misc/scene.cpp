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

  void Scene::Draw(Camera &cam, BitmapWindow &window)
  {
    if (!update_scene)
    {
      // return;
    }
    update_scene = false;

    vec3 lower_left_corner{-2, -1, -1};
    vec3 horizontal{4, 0, 0};
    vec3 vertical{0, 2, 0};
    vec3 origin{0};
    ivec2 window_size = window.size();
    uint32_t *bitmap = window.bitmap().data();
    for (int j = window_size.y - 1; j >= 0; j--)
    {
      for (int i = 0; i < window_size.x; i++)
      {

        float u = static_cast<float>(i) / static_cast<float>(window_size.x);
        float v = static_cast<float>(j) / static_cast<float>(window_size.y);

        Ray r(origin, normalize(lower_left_corner + u * horizontal + v * vertical));

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

        auto ir = static_cast<int>(255.99 * ld.color.r) << 16;
        auto ig = static_cast<int>(255.99 * ld.color.g) << 8;
        auto ib = static_cast<int>(255.99 * ld.color.b);

        bitmap[size_t(j) * window_size.x + i] = ir | ig | ib;
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