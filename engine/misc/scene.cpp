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
    void Scene::UpdateScene() noexcept { update_scene = true; }
    void Scene::Draw(components::Camera const &cam, BitmapWindow &window,
                     ParallelExecutor &executor)
    {
        using namespace components;

        auto spheres = registry.group<components::Sphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshComponent>(entt::get<components::Transform>);

        auto directional_lights = registry.view<components::DirectionalLight>();
        auto point_lights = registry.group<components::PointLight>(entt::get<components::Transform>);
        auto spot_lights = registry.group<components::SpotLight>(entt::get<components::Transform>);
        auto find_intersection = [this, &spheres, &meshes](Intersection &intersection, Ray &ray)
        {
            floor.CheckIntersection(intersection, ray);
            spheres.each([&intersection, &ray](auto const, auto const &, auto const &transform) __lambda_force_inline
                         { components::Sphere::CheckIntersection(transform, intersection, ray); });
            meshes.each([&intersection, &ray](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                        { mesh.CheckIntersection(transform, intersection, ray); });
            return intersection.exists();
        };

        auto find_intersection_if = [this, &spheres, &meshes](
                                        Intersection &intersection,
                                        Ray &ray,
                                        std::function<bool(entt::entity, Transform const &, render::Material const &)> const &func)
        {
            if (func(entt::entity{}, floor.transform, floor.material)) { floor.CheckIntersection(intersection, ray); }
            spheres.each([&intersection, &ray, &func](auto const entity, auto const &sphere, auto const &transform) __lambda_force_inline
                         {
                             if(func(entity, transform, sphere.material)) { components::Sphere::CheckIntersection(transform, intersection, ray); } });
            meshes.each([&intersection, &ray, &func](auto const entity, auto const &mesh, auto const &transform) __lambda_force_inline
                        { 
                             if(func(entity, transform, mesh.material())) { mesh.CheckIntersection(transform, intersection, ray); } });
            return intersection.exists();
        };

        auto find_intersection_material = [this, &spheres, &meshes](Intersection &intersection, Ray &ray, render::Material &mat)
        {
            if (floor.CheckIntersection(intersection, ray))
            {
                mat = floor.material;
            }
            spheres.each([&intersection, &ray, &mat](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                         { if(components::Sphere::CheckIntersection(transform, intersection, ray)) { mat = sphere.material; } });
            meshes.each([&intersection, &ray, &mat](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                        { if(mesh.CheckIntersection(transform, intersection, ray)) { mat = mesh.material(); } });
            return intersection.exists();
        };

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
            float u = ((float(i) + 0.5f) / float(bitmap_size.x));
            float v = ((float(j) + 0.5f) / float(bitmap_size.y));

            vec4 t = (bl4 + up4 * v + right4 * u);
            Ray ray(cam.position(), t.as_vec<3>() / t.w - cam.position());
            Intersection intersection;
            intersection.reset();

            render::Material mat;

            find_intersection_material(intersection, ray, mat);

            if (!intersection.exists())
            {
                bitmap[task_num] = 0;
                return;
            }

            render::LightData ld{.color = vec3{0, 0, 0},
                                 .ray = ray,
                                 .point = intersection.point,
                                 .normal = intersection.normal,
                                 .view_dir = normalize(ray.origin() - intersection.point)};
            directional_lights.each([&ld, &mat, &find_intersection_if](const auto, auto &dirlight)
                                    {
                                Intersection nearest;
                                nearest.reset();
                                Ray ray(ld.point + dirlight.direction * 0.01f, +dirlight.direction);
                                find_intersection_if(
                                    nearest, ray, 
                                    [](entt::entity, Transform const&, render::Material const& mat) __lambda_force_inline
                                    {
                                        return mat.casts_shadow;
                                    }
                                );
                                if(!nearest.exists())
                                {
                                  dirlight.Illuminate(ld, mat);
                                } });
            point_lights.each([&ld, &mat, &find_intersection_if](const auto, auto const &point_light, auto const &transform)
                              { 
                                if(!point_light.Illuminable(transform, ld)) { return; }
                                vec3 L = transform.position - ld.point;
                                float d = length(L);
                                L /= d;
                                Intersection nearest;
                                nearest.reset();
                                Ray ray(ld.point + L * 0.01f, +L);
                                find_intersection_if(
                                    nearest, ray, 
                                    [](entt::entity, Transform const&, render::Material const& mat) __lambda_force_inline
                                    {
                                        return mat.casts_shadow;
                                    }
                                );
                                if (!nearest.exists() || nearest.t >= d)
                                {
                                    point_light.Illuminate(transform, ld, mat);
                                }});
            spot_lights.each([&ld, &mat, &find_intersection_if](const auto, auto const &spot_light, auto const &transform)
                             { 
                                if(!spot_light.Illuminable(transform, ld)) { return; }
                                vec3 L = transform.position - ld.point;
                                float d = length(L);
                                L /= d;
                                Intersection nearest;
                                nearest.reset();
                                Ray ray(transform.position + L * 0.01f, +L);
                                find_intersection_if(
                                    nearest, ray, 
                                    [](entt::entity, Transform const&, render::Material const& mat) __lambda_force_inline
                                    {
                                        return mat.casts_shadow;
                                    }
                                );
                                if (!nearest.exists() || nearest.t >= d)
                                {
                                    spot_light.Illuminate(transform, ld, mat); 
                                } });
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
        executor.execute(func, bitmap_size.x * bitmap_size.y, 25);
#endif
    }

    std::optional<entt::entity> Scene::GetIntersectedEntity(Intersection &intersection, Ray &ray)
    {
        auto spheres = registry.group<components::Sphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshComponent>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&rv, &intersection, &ray](auto const e, auto const &, auto const &transform) __lambda_force_inline
                     { if(components::Sphere::CheckIntersection(transform, intersection, ray)) {rv = e;} });
        meshes.each([&rv, &intersection, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(mesh.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        return rv;
    }

    std::optional<entt::entity> Scene::GetIntersectedEntityIf(
        Intersection &intersection, Ray &ray,
        std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func)
    {
        auto spheres = registry.group<components::Sphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::MeshComponent>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&func, &rv, &intersection, &ray](auto const e, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(func(e, transform, sphere.material) && components::Sphere::CheckIntersection(transform, intersection, ray)) {rv = e;} });
        meshes.each([&func, &rv, &intersection, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(func(e, transform, mesh.material()) && mesh.CheckIntersection(transform, intersection, ray)) {rv = e;} });
        return rv;
    }

} // namespace engine