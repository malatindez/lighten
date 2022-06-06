#include "misc/scene.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include <functional>
#include <sstream>
#include <tuple>
#include <vector>

namespace engine
{
    using namespace core;
    using namespace math;
    using namespace components;

    void Scene::UpdateScene() noexcept { update_scene = true; }
    void Scene::Draw(components::Camera const &cam, BitmapWindow &window,
                     [[maybe_unused]] ParallelExecutor &executor)
    {

        auto spheres = registry.group<components::SphereRenderer>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshRenderer>(entt::get<components::Transform>);

        auto directional_lights = registry.view<components::DirectionalLight>();
        auto point_lights = registry.group<components::PointLight>(entt::get<components::Transform>);
        auto spot_lights = registry.group<components::SpotLight>(entt::get<components::Transform>);

        update_scene = false;
        ivec2 bitmap_size = window.bitmap_size();
        uint32_t *bitmap = window.bitmap().data();
        vec4 bl4 = vec4(-1, -1, 1, 1) * cam.inv_view_projection;
        vec4 br4 = vec4(1, -1, 1, 1) * cam.inv_view_projection;
        vec4 tl4 = vec4(-1, 1, 1, 1) * cam.inv_view_projection;
        vec4 right4 = br4 - bl4;
        vec4 up4 = tl4 - bl4;
        auto func = [&](uint32_t, uint32_t task_num)
        {
            int j = task_num / bitmap_size.x;
            int i = task_num % bitmap_size.x;
            float u = ((static_cast<float>(i) + 0.5f) / static_cast<float>(bitmap_size.x));
            float v = ((static_cast<float>(j) + 0.5f) / static_cast<float>(bitmap_size.y));

            vec4 t = (bl4 + up4 * v + right4 * u);
            Ray ray(cam.position(), t.as_rvec<3>() / t.w - cam.position());
            Intersection nearest;
            nearest.reset();

            render::Material mat;
            if (floor.CheckIntersection(nearest, ray))
            {
                mat = floor.material;
            }
            spheres.each([&nearest, &ray, &mat](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                         { if(sphere.CheckIntersection(transform, nearest, ray)) { mat = sphere.material; } });
            meshes.each([&nearest, &ray, &mat](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                        { if(mesh.CheckIntersection(transform, nearest, ray)) { mat = mesh.material(); } });

            if (!nearest.exists())
            {
                bitmap[task_num] = 0;
                return;
            }

            render::LightData ld{.color = vec3{0, 0, 0},
                                 .ray = ray,
                                 .point = nearest.point,
                                 .normal = nearest.normal,
                                 .view_dir = normalize(ray.origin() - nearest.point)};

            Illuminate(spheres, meshes, directional_lights, point_lights, spot_lights, ld, mat);
            ivec3 color{256 * (mat.emission + ld.color)};
            color = ivec3{color.r > 255 ? 255 : color.r,
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
        executor.execute(func, bitmap_size.x * bitmap_size.y, 100);
#endif
    }

    std::optional<entt::entity> Scene::GetIntersectedEntity(Intersection &intersection, Ray &ray)
    {
        auto spheres = registry.group<components::SphereRenderer>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshRenderer>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&rv, &intersection, &ray](auto const e, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(sphere.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        meshes.each([&rv, &intersection, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(mesh.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        return rv;
    }

    std::optional<entt::entity> Scene::GetIntersectedEntityIf(
        Intersection &intersection, Ray &ray,
        std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func)
    {
        auto spheres = registry.group<components::SphereRenderer>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshRenderer>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&func, &rv, &intersection, &ray](auto const e, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(func(e, transform, sphere.material) && sphere.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        meshes.each([&func, &rv, &intersection, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(func(e, transform, mesh.material()) && mesh.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        return rv;
    }

    inline bool Scene::FindIntersection(SphereGroup &spheres, MeshGroup &meshes, Intersection &intersection, Ray &ray) const noexcept
    {
        floor.CheckIntersection(intersection, ray);
        spheres.each([&intersection, &ray](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                     { sphere.CheckIntersection(transform, intersection, ray); });
        meshes.each([&intersection, &ray](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                    { mesh.CheckIntersection(transform, intersection, ray); });
        return intersection.exists();
    }

    inline bool Scene::FindIntersectionIf(SphereGroup &spheres, MeshGroup &meshes, Intersection &intersection, Ray &ray, IntersectionCallbackFn const &func) const noexcept
    {
        if (func(entt::entity{}, floor.transform, floor.material))
        {
            floor.CheckIntersection(intersection, ray);
        }
        spheres.each([&intersection, &ray, &func](auto const entity, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(func(entity, transform, sphere.material)) { sphere.CheckIntersection(transform, intersection, ray); } });
        meshes.each([&intersection, &ray, &func](auto const entity, auto const &mesh, auto const &transform) __lambda_force_inline
                    {  if(func(entity, transform, mesh.material())) { mesh.CheckIntersection(transform, intersection, ray); } });
        return intersection.exists();
    }

    void Scene::Illuminate(SphereGroup &spheres,
                           MeshGroup &meshes,
                           DirectionalLightView &directional_lights,
                           PointLightGroup &point_lights,
                           SpotLightGroup &spot_lights,
                           render::LightData &ld,
                           render::Material &mat) const
    {

        auto find_intersection_if_casts_shadow = [this, &spheres, &meshes](Intersection &intersection, Ray &ray) __lambda_force_inline
        {
            return FindIntersectionIf(spheres, meshes, intersection, ray,
                                      [](entt::entity, Transform const &, render::Material const &m) __lambda_force_inline
                                      { return m.casts_shadow; });
        };
        for (auto entity : directional_lights)
        {
            auto &dirlight = directional_lights.get<DirectionalLight>(entity);

            Ray ray(ld.point + ld.normal * 0.001f, -dirlight.direction);

            Intersection nearest;
            nearest.reset();
            find_intersection_if_casts_shadow(nearest, ray);

            if (!nearest.exists())
            {
              dirlight.Illuminate(ld, mat);
            }
        }
        for (auto entity : point_lights)
        {
            auto &transform = point_lights.get<Transform>(entity);
            auto &point_light = point_lights.get<PointLight>(entity);

            if (!point_light.Illuminable(transform, ld))
            {
                continue;
            }

            vec3 L = transform.position - ld.point;
            float d = length(L);
            vec3 dir = normalize(L - ld.normal * 0.001f);
            Ray ray(ld.point + ld.normal * 0.001f, dir);

            Intersection nearest;
            nearest.reset();
            find_intersection_if_casts_shadow(nearest, ray);

            if (!nearest.exists() || nearest.t >= d)
            {
                point_light.Illuminate(transform, ld, mat);
            }
        }

        for (auto entity : spot_lights)
        {
            auto &transform = spot_lights.get<Transform>(entity);
            auto &spot_light = spot_lights.get<SpotLight>(entity);

            if (!spot_light.Illuminable(transform, ld))
            {
                continue;
            }

            vec3 L = transform.position - ld.point;
            float d = length(L);
            vec3 dir = normalize(L - ld.normal * 0.001f);
            Ray ray(ld.point + ld.normal * 0.001f, dir);

            Intersection nearest;
            nearest.reset();
            find_intersection_if_casts_shadow(nearest, ray);

            if (!nearest.exists() || nearest.t - 0.01f >= d)
            {
               spot_light.Illuminate(transform, ld, mat);
            }
        }
    }

} // namespace engine