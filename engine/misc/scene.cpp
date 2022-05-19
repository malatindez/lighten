#include "misc/scene.hpp"
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
    ivec2 bitmap_size = window.bitmap_size();
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

    auto planes = registry.group<components::Plane>(entt::get<components::Transform>);
    auto spheres = registry.group<components::Sphere>(entt::get<components::Transform>);
    auto cubes = registry.group<components::Cube>(entt::get<components::Transform>);

    auto directional_lights = registry.view<components::DirectionalLight>();
    auto point_lights = registry.group<components::PointLight>(entt::get < components::Transform>);
    auto spot_lights = registry.group<components::SpotLight>(entt::get < components::Transform>);

    for (int j = 0; j < bitmap_size.y; j++)
    {
      for (int i = 0; i < bitmap_size.x; i++)
      {
        float u = ((static_cast<float>(i)) / static_cast<float>(bitmap_size.x));
        float v = ((static_cast<float>(j)) / static_cast<float>(bitmap_size.y));
        math::vec4 a = BL + BR_BL * u + TL_BL * v;
        Ray ray(origin, normalize(a.as_vec<3>()));
        math::Intersection intersection;
        intersection.reset();

        spheres.each([&intersection, &ray](const auto, auto& transform) { components::Sphere::CheckIntersection(transform, intersection, ray); });
        planes.each([&intersection, &ray](const auto, auto const& plane, auto const& transform) { plane.CheckIntersection(transform, intersection, ray); });
        cubes.each([&intersection, &ray](const auto, auto& transform) { components::Cube::CheckIntersection(transform, intersection, ray);  });

        if (!intersection.exists())
        {
          bitmap[size_t(j) * bitmap_size.x + i] = 0;
          continue;
        }

        components::LightData ld{
            .color = math::vec3{0, 0, 0},
            .ray = ray,
            .point = intersection.point,
            .normal = intersection.normal,
            .view_dir = normalize(intersection.point - ray.origin())};
        directional_lights.each([&ld](const auto, auto& dirlight) {dirlight.UpdateColor(ld); });
        point_lights.each([&ld](const auto, auto const& point_light, auto const& transform) { point_light.UpdateColor(transform, ld); });
        spot_lights.each([&ld](const auto, auto const& spot_light, auto const& transform) { spot_light.UpdateColor(transform, ld); });
        ivec3 color{ 255.99f * ld.color };
        color %= 256;
        color.r <<= 16;
        color.g <<= 8;

        bitmap[size_t(j) * bitmap_size.x + i] = color.r | color.g | color.b;
      }
    }
  }

} // namespace engine