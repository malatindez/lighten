#include "misc/scene.hpp"
#include <sstream>
using namespace engine::math;

namespace engine
{
  void Scene::UpdateScene() noexcept { update_scene = true; }

  void Scene::Draw(components::Camera const &cam, BitmapWindow &window)
  {
    if (!update_scene)
    {
      // return;
    }
    update_scene = false;
    vec3 origin{cam.view[3][0],cam.view[3][1],cam.view[3][2]};
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
    BL -= vec4(origin, 0);
    BR -= vec4(origin, 0);
    TL -= vec4(origin, 0);
    math::vec4 BR_BL = BR - BL;
    
    math::vec4 TL_BL = TL - BL; 
    std::stringstream s;
    s << BR_BL << std::endl << TL_BL << std::endl;
    OutputDebugStringA(s.str().c_str());

    auto planes = registry.group<components::Plane>(entt::get<components::Transform, components::Material>);
    auto spheres = registry.group<components::Sphere>(entt::get<components::Transform, components::Material>);
    auto cubes = registry.group<components::Cube>(entt::get<components::Transform, components::Material>);

    auto directional_lights = registry.view<components::DirectionalLight>();
    auto point_lights = registry.group<components::PointLight>(entt::get<components::Transform>);
    auto spot_lights = registry.group<components::SpotLight>(entt::get<components::Transform>);
    for (int j = 0; j < bitmap_size.y; j++)
    {
      for (int i = 0; i < bitmap_size.x; i++)
      {
        float u = ((static_cast<float>(i)) / static_cast<float>(bitmap_size.x)) * 2 - 1;
        float v = ((static_cast<float>(j)) / static_cast<float>(bitmap_size.y)) * 2 - 1;
        math::vec4 near_ = vec4(u, v, -1, 1) * cam.inv_view_projection;
        math::vec4 far_ = vec4(u, v, 1, 1) * cam.inv_view_projection;
        math::vec3 origin(near_.as_vec<3>() / near_.w);
        math::Ray ray(origin, normalize(far_.as_vec<3>() / far_.w - origin));
        math::Intersection intersection;
    
        components::Material mat;
        intersection.reset();

        spheres.each([&intersection, &ray, &mat](const auto, auto &transform, auto &material)
        { if(components::Sphere::CheckIntersection(transform, intersection, ray)) { mat = material; } });
        planes.each([&intersection, &ray, &mat](const auto, auto const &plane, auto const &transform, auto &material)
        { if(plane.CheckIntersection(transform, intersection, ray)) { mat = material; } });
        cubes.each([&intersection, &ray, &mat](const auto, auto &transform, auto &material)
        { if(components::Cube::CheckIntersection(transform, intersection, ray)) { mat = material; } });

        if (!intersection.exists())
        {
          bitmap[size_t(j) * bitmap_size.x + i] = 0;
          continue;
        }
        
        components::LightData ld{
            .color = math::vec3{0,0,0},
            .ray = ray,
            .point = intersection.point,
            .normal = intersection.normal,
            .view_dir = normalize(intersection.point - ray.origin())};
        directional_lights.each([&ld](const auto, auto &dirlight)
                                { dirlight.UpdateColor(ld); });
        point_lights.each([&ld](const auto, auto const &point_light, auto const &transform)
                          { point_light.UpdateColor(transform, ld); });
        spot_lights.each([&ld](const auto, auto const &spot_light, auto const &transform)
                         { spot_light.UpdateColor(transform, ld); });
        ivec3 color{255.99f * (mat.color * ld.color)};
        color %= 256;
        color.r <<= 16;
        color.g <<= 8;

        bitmap[size_t(j) * bitmap_size.x + i] = color.r | color.g | color.b;
      }
    }
  }

} // namespace engine