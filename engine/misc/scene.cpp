#include "misc/scene.hpp"
#include <sstream>
#include <tuple>
#include <vector>
using namespace engine::math;

namespace engine
{
  void Scene::UpdateScene() noexcept { update_scene = true; }
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

      GetIntersectedMaterial(intersection, ray, mat);

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
      for (auto const &[entity, dir_light] : directional_lights)
      {
        dir_light.get().UpdateColor(ld, mat, [this](math::Intersection &i, math::Ray &r)
                                    { return FindIntersection(i, r); });
      }
      for (auto const &[entity, point_light, transform] : point_lights)
      {
        point_light.get().UpdateColor(transform, ld, mat, [this](math::Intersection &i, math::Ray &r, Transform &t)
                                      { return GetIntersectedTransform(i, r, t); });
      }
      for (auto const &[entity, spot_light, transform] : spot_lights)
      {
        spot_light.get().UpdateColor(transform, ld, mat, [this](math::Intersection &i, math::Ray &r, Transform &t)
                                     { return GetIntersectedTransform(i, r , t); });
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
    for (int i = 0; i < bitmap_size.x * bitmap_size.y; i++)
    {
      func(0, i);
    }
#else
    executor.execute(func, bitmap_size.x * bitmap_size.y, 10);
#endif
  }

  void Scene::OnRegistryUpdate()
  {
    using namespace components;
    registry.group<Plane>(entt::get<Transform, Material>).each([this](auto const e, auto const &plane, auto const &transform, auto const &material)
                                                               { planes.emplace_back(e, plane, transform, material); });
    registry.group<Sphere>(entt::get<Transform, Material>).each([this](auto const e, auto const &transform, auto const &material)
                                                                { spheres.emplace_back(e, transform, material); });
    registry.group<Cube>(entt::get<Transform, Material>).each([this](auto const e, auto const &transform, auto const &material)
                                                              { cubes.emplace_back(e, transform, material); });
    registry.group<Mesh>(entt::get<Transform, Material>).each([this](auto const e, auto const &mesh, auto const &transform, auto const &material)
                                                              { meshes.emplace_back(e, mesh, transform, material); });

    registry.view<DirectionalLight>().each([this](const auto e, auto const &dirlight)
                                           { directional_lights.emplace_back(e, dirlight); });
    registry.group<PointLight>(entt::get<Transform>).each([this](const auto e, auto const &point_light, auto const &transform)
                                                          { point_lights.emplace_back(e, point_light, transform); });
    registry.group<SpotLight>(entt::get<Transform>).each([this](const auto e, auto const &spot_light, auto const &transform)
                                                         { spot_lights.emplace_back(e, spot_light, transform); });
  }
  bool Scene::FindIntersection(math::Intersection &intersection, math::Ray &ray)
  {
    for (auto const &[entity, transform, material] : spheres)
    {
      components::Sphere::CheckIntersection(transform.get(), intersection, ray);
    }
    for (auto const &[entity, plane, transform, material] : planes)
    {
      plane.get().CheckIntersection(transform.get(), intersection, ray);
    }
    for (auto const &[entity, transform, material] : cubes)
    {
      components::Cube::CheckIntersection(transform.get(), intersection, ray);
    }
    for (auto const &[entity, mesh, transform, material] : meshes)
    {
      mesh.get().CheckIntersection(transform.get(), intersection, ray);
    }
    return intersection.exists();
  }
  std::optional<entt::entity> Scene::GetIntersectedEntity(math::Intersection &intersection, math::Ray &ray)
  {
    std::optional<entt::entity> rv{};
    for (auto const &[entity, transform, material] : spheres)
    {
      if (components::Sphere::CheckIntersection(transform.get(), intersection, ray))
      {
        rv = entity;
      }
    }
    for (auto const &[entity, plane, transform, material] : planes)
    {
      if (plane.get().CheckIntersection(transform.get(), intersection, ray))
      {
        rv = entity;
      }
    }
    for (auto const &[entity, transform, material] : cubes)
    {
      if (components::Cube::CheckIntersection(transform.get(), intersection, ray))
      {
        rv = entity;
      }
    }
    for (auto const &[entity, mesh, transform, material] : meshes)
    {
      if (mesh.get().CheckIntersection(transform.get(), intersection, ray))
      {
        rv = entity;
      }
    }
    return rv;
  }

  bool Scene::GetIntersectedMaterial(math::Intersection &intersection, math::Ray &ray, components::Material &mat)
  {
    for (auto const &[entity, transform, material] : spheres)
    {
      if (components::Sphere::CheckIntersection(transform.get(), intersection, ray))
      {
        mat = material;
      }
    }
    for (auto const &[entity, plane, transform, material] : planes)
    {
      if (plane.get().CheckIntersection(transform.get(), intersection, ray))
      {
        mat = material;
      }
    }
    for (auto const &[entity, transform, material] : cubes)
    {
      if (components::Cube::CheckIntersection(transform.get(), intersection, ray))
      {
        mat = material;
      }
    }
    for (auto const &[entity, mesh, transform, material] : meshes)
    {
      if (mesh.get().CheckIntersection(transform.get(), intersection, ray))
      {
        mat = material;
      }
    }
    return intersection.exists();
  }
  
  bool Scene::GetIntersectedTransform(math::Intersection &intersection, math::Ray &ray, components::Transform &t)
  {
    for (auto const &[entity, transform, material] : spheres)
    {
      if (components::Sphere::CheckIntersection(transform.get(), intersection, ray))
      {
        t = transform;
      }
    }
    for (auto const &[entity, plane, transform, material] : planes)
    {
      if (plane.get().CheckIntersection(transform.get(), intersection, ray))
      {
        t = transform;
      }
    }
    for (auto const &[entity, transform, material] : cubes)
    {
      if (components::Cube::CheckIntersection(transform.get(), intersection, ray))
      {
        t = transform;
      }
    }
    for (auto const &[entity, mesh, transform, material] : meshes)
    {
      if (mesh.get().CheckIntersection(transform.get(), intersection, ray))
      {
        t = transform;
      }
    }
    return intersection.exists();
  }
  
} // namespace engine