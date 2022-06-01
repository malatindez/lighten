#include "misc/scene.hpp"
#include <sstream>
#include <tuple>
#include <vector>
using namespace engine::math;

namespace engine
{
  void Scene::UpdateScene() noexcept { update_scene = true; }
  template <class T>
  using cref = std::reference_wrapper<const T>;
  void Scene::Draw(components::Camera const &cam, BitmapWindow &window, ParallelExecutor &executor)
  {
    using namespace components;
    if (!update_scene)
    {
      // return;
    }
    update_scene = false;
    ivec2 bitmap_size = window.bitmap_size();
    uint32_t *bitmap = window.bitmap().data();

    std::vector<std::tuple<cref<Plane>, cref<Transform>, cref<Material>>> planes;
    std::vector<std::tuple<cref<Transform>, cref<Material>>> spheres;
    std::vector<std::tuple<cref<Transform>, cref<Material>>> cubes;
    std::vector<std::tuple<cref<Mesh>, cref<Transform>, cref<Material>>> meshes;
    registry.group<Plane>(entt::get<Transform, Material>).each([&planes](auto const, auto const &plane, auto const &transform, auto const &material)
                                                               { planes.emplace_back(plane, transform, material); });
    registry.group<Sphere>(entt::get<Transform, Material>).each([&spheres](auto const, auto const &transform, auto const &material)
                                                                { spheres.emplace_back(transform, material); });
    registry.group<Cube>(entt::get<Transform, Material>).each([&cubes](auto const, auto const &transform, auto const &material)
                                                              { cubes.emplace_back(transform, material); });
    registry.group<Mesh>(entt::get<Transform, Material>).each([&meshes](auto const, auto const &mesh, auto const &transform, auto const &material)
                                                              { meshes.emplace_back(mesh, transform, material); });

    std::vector<cref<DirectionalLight>> directional_lights;
    std::vector<std::tuple<cref<PointLight>, cref<Transform>>> point_lights;
    std::vector<std::tuple<cref<SpotLight>, cref<Transform>>> spot_lights;
    registry.view<DirectionalLight>().each([&directional_lights](const auto, auto const &dirlight)
                                           { directional_lights.emplace_back(dirlight); });
    registry.group<PointLight>(entt::get<Transform>).each([&point_lights](const auto, auto const &point_light, auto const &transform)
                                                          { point_lights.emplace_back(point_light, transform); });
    registry.group<SpotLight>(entt::get<Transform>).each([&spot_lights](const auto, auto const &spot_light, auto const &transform)
                                                         { spot_lights.emplace_back(spot_light, transform); });
    auto find_intersection = [&spheres, &planes, &cubes, &meshes](math::Intersection &intersection, math::Ray &ray)
    {
      for (auto const &[transform, material] : spheres)
      {
        Sphere::CheckIntersection(transform.get(), intersection, ray);
      }
      for (auto const &[plane, transform, material] : planes)
      {
        plane.get().CheckIntersection(transform.get(), intersection, ray);
      }
      for (auto const &[transform, material] : cubes)
      {
        Cube::CheckIntersection(transform.get(), intersection, ray);
      }
      for (auto const &[mesh, transform, material] : meshes)
      {
        mesh.get().CheckIntersection(transform.get(), intersection, ray);
      }
      return intersection.exists();
    };

    auto find_intersection_material = [&spheres, &planes, &cubes, &meshes](math::Intersection &intersection, math::Ray &ray, Material &mat)
    {
      for (auto const &[transform, material] : spheres)
      {
        if (Sphere::CheckIntersection(transform.get(), intersection, ray))
        {
          mat = material;
        }
      }
      for (auto const &[plane, transform, material] : planes)
      {
        if (plane.get().CheckIntersection(transform.get(), intersection, ray))
        {
          mat = material;
        }
      }
      for (auto const &[transform, material] : cubes)
      {
        if (Cube::CheckIntersection(transform.get(), intersection, ray))
        {
          mat = material;
        }
      }
      for (auto const &[mesh, transform, material] : meshes)
      {
        if (mesh.get().CheckIntersection(transform.get(), intersection, ray))
        {
          mat = material;
        }
      }
      return intersection.exists();
    };

    auto func = [&](uint32_t, uint32_t task_num)
    {
      int j = task_num / bitmap_size.x;
      int i = task_num % bitmap_size.x;
      float u = ((static_cast<float>(i) + 0.5f) / static_cast<float>(bitmap_size.x)) * 2 - 1;
      float v = ((static_cast<float>(j) + 0.5f) / static_cast<float>(bitmap_size.y)) * 2 - 1;
      math::vec4 near_ = vec4(u, v, 1, 1) * cam.inv_view_projection;
      math::Ray ray(cam.position(), normalize(near_.as_vec<3>() / near_.w - cam.position()));
      math::Intersection intersection;

      Material mat;
      intersection.reset();

      find_intersection_material(intersection, ray, mat);

      if (!intersection.exists())
      {
        bitmap[task_num] = 0;
        return;
      }

      LightData ld{
          .color = math::vec3{0, 0, 0},
          .ray = ray,
          .point = intersection.point,
          .normal = intersection.normal,
          .view_dir = normalize(ray.origin() - intersection.point)};
      for (auto const &dir_light : directional_lights)
      {
        dir_light.get().UpdateColor(ld, mat, find_intersection);
      }
      for (auto const &[point_light, transform] : point_lights)
      {
        point_light.get().UpdateColor(transform, ld, mat, find_intersection);
      }
      for (auto const &[spot_light, transform] : spot_lights)
      {
        spot_light.get().UpdateColor(transform, ld, mat, find_intersection);
      }
      ivec3 color{256 * (mat.emission + ld.color)};
      color = math::ivec3{
          color.r > 255 ? 255 : color.r,
          color.g > 255 ? 255 : color.g,
          color.b > 255 ? 255 : color.b};
      color.r <<= 16;
      color.g <<= 8;

      bitmap[task_num] = color.r | color.g | color.b;
    };
#ifdef _DEBUG // parallel mode is worse in debug
    for(int i = 0; i < bitmap_size.x * bitmap_size.y; i++)
    {
      func(0, i);
    }
#else
    executor.execute(func, bitmap_size.x * bitmap_size.y, 100);
#endif
  }

} // namespace engine