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
    ivec2 bitmap_size = window.bitmap_size();
    uint32_t *bitmap = window.bitmap().data();

    auto planes = registry.group<components::Plane>(entt::get<components::Transform, components::Material>);
    auto spheres = registry.group<components::Sphere>(entt::get<components::Transform, components::Material>);
    auto cubes = registry.group<components::Cube>(entt::get<components::Transform, components::Material>);
    auto meshes = registry.group<components::Mesh>(entt::get<components::Transform, components::Material>);

    auto directional_lights = registry.view<components::DirectionalLight>();
    auto point_lights = registry.group<components::PointLight>(entt::get<components::Transform>);
    auto spot_lights = registry.group<components::SpotLight>(entt::get<components::Transform>);
    auto find_intersection = [&spheres, &planes, &cubes, &meshes](math::Intersection &intersection, math::Ray &ray)
    {
      spheres.each([&intersection, &ray](auto const, auto const &transform, auto const &material)
                   { components::Sphere::CheckIntersection(transform, intersection, ray); });
      planes.each([&intersection, &ray](auto const, auto const &plane, auto const &transform, auto const &material)
                  { plane.CheckIntersection(transform, intersection, ray); });
      cubes.each([&intersection, &ray](auto const, auto const &transform, auto const &material)
                 { components::Cube::CheckIntersection(transform, intersection, ray); });
      meshes.each([&intersection, &ray](auto const, auto const &mesh, auto const &transform, auto const &material)
                  { mesh.CheckIntersection(transform, intersection, ray); });
      return intersection.exists();
    };

    auto find_intersection_material = [&spheres, &planes, &cubes, &meshes](math::Intersection &intersection, math::Ray &ray, components::Material &mat)
    {
      spheres.each([&intersection, &ray, &mat](auto const, auto const &transform, auto const &material)
                   { if(components::Sphere::CheckIntersection(transform, intersection, ray)) { mat = material; } });
      planes.each([&intersection, &ray, &mat](auto const, auto const &plane, auto const &transform, auto const &material)
                  { if(plane.CheckIntersection(transform, intersection, ray)) { mat = material; } });
      cubes.each([&intersection, &ray, &mat](auto const, auto const &transform, auto const &material)
                 { if(components::Cube::CheckIntersection(transform, intersection, ray)) { mat = material; } });
      meshes.each([&intersection, &ray, &mat](auto const, auto const &mesh, auto const &transform, auto const &material)
                  { if(mesh.CheckIntersection(transform, intersection, ray)) { mat = material; } });
      return intersection.exists();
    };

    for (int j = 0; j < bitmap_size.y; j++)
    {
      for (int i = 0; i < bitmap_size.x; i++)
      {
        float u = ((static_cast<float>(i) + 0.5f) / static_cast<float>(bitmap_size.x)) * 2 - 1;
        float v = ((static_cast<float>(j) + 0.5f) / static_cast<float>(bitmap_size.y)) * 2 - 1;
        math::vec4 near_ = vec4(u, v, 1, 1) * cam.inv_view_projection;
        math::Ray ray(cam.position(), normalize(near_.as_vec<3>() / near_.w - cam.position()));
        math::Intersection intersection;

        components::Material mat;
        intersection.reset();

        find_intersection_material(intersection, ray, mat);

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
            .view_dir = normalize(ray.origin() - intersection.point)};
        directional_lights.each([&ld, &mat, &find_intersection](const auto, auto &dirlight)
                                { dirlight.UpdateColor(ld, mat, find_intersection); });
        point_lights.each([&ld, &mat, &find_intersection](const auto, auto const &point_light, auto const &transform)
                          { point_light.UpdateColor(transform, ld,  mat,find_intersection); });
        spot_lights.each([&ld, &mat, &find_intersection](const auto, auto const &spot_light, auto const &transform)
                         { spot_light.UpdateColor(transform,  ld,  mat,find_intersection); });
        ivec3 color{256 * (mat.emission + ld.color)};
        color = math::ivec3{
            color.r > 255 ? 255 : color.r,
            color.g > 255 ? 255 : color.g,
            color.b > 255 ? 255 : color.b};
        color.r <<= 16;
        color.g <<= 8;

        bitmap[size_t(j) * bitmap_size.x + i] = color.r | color.g | color.b;
      }
    }
  }

} // namespace engine