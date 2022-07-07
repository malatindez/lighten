#include "misc/scene.hpp"
#include "render/light-data.hpp"
#include "render/material.hpp"
#include <Windows.h>
#include <functional>
#include <sstream>
#include <string>
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
        if (!update_scene)
        {
            return;
        }
        auto spheres = registry.group<components::SceneSphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::SceneMesh>(entt::get<components::Transform>);

        auto directional_lights = registry.view<components::DirectionalLight>();
        auto point_lights = registry.group<components::PointLight>(entt::get<components::Transform>);
        auto spot_lights = registry.group<components::SpotLight>(entt::get<components::Transform>);

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

            Ray ray(cam.position(), t.as_rvec<3>() / t.w - cam.position());
            Intersection nearest;
            nearest.reset();
            render::Material mat;

            if (floor.CheckIntersection(nearest, ray))
            {
                mat = floor.material;
            }
            spheres.each([&nearest, &ray, &mat](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                         { if (sphere.CheckIntersection(transform, nearest, ray)) { mat = sphere.material; } });
            meshes.each([&nearest, &ray, &mat](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                        { if (mesh.CheckIntersection(transform, nearest, ray)) { mat = mesh.material(); } });

            vec3 const hdr = CalculatePointColor(spheres, meshes, directional_lights, point_lights, spot_lights, ray, nearest, mat);

            vec3 const tone_mapped = render::AcesHdr2Ldr(render::AdjustExposure(hdr, exposure));

            vec3 const gamma_corrected = pow(tone_mapped, vec3(1.0 / gamma));

            ivec3 ldr{256 * gamma_corrected};

            ldr = ivec3{ldr.r > 0xff ? 0xff : ldr.r,
                        ldr.g > 0xff ? 0xff : ldr.g,
                        ldr.b > 0xff ? 0xff : ldr.b};

            ldr.r <<= 16;
            ldr.g <<= 8;

            bitmap[task_num] = ldr.r | ldr.g | ldr.b;
        };
#ifdef _DEBUG // parallel mode is worse in debug
        for (int i = 0; i < bitmap_size.x * bitmap_size.y; i++)
        {
            func(0, i);
        }
#else
        executor.execute(func, bitmap_size.x * bitmap_size.y, 100);
#endif
        update_scene = false;
    }

    std::optional<entt::entity> Scene::GetIntersectedEntity(Intersection &nearest, Ray const &ray)
    {
        auto spheres = registry.group<components::SceneSphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::SceneMesh>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&rv, &nearest, &ray](auto const e, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(sphere.CheckIntersection(transform, nearest, ray)) {rv = e;} });
        meshes.each([&rv, &nearest, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(mesh.CheckIntersection(transform, nearest, ray)) {rv = e;} });
        return rv;
    }

    std::optional<entt::entity> Scene::GetIntersectedEntityIf(
        Intersection &nearest, Ray const &ray,
        std::function<bool(entt::entity, components::Transform const &, render::Material const &)> const &func)
    {
        auto spheres = registry.group<components::SceneSphere>(entt::get<components::Transform>);
        auto meshes = registry.group<components::SceneMesh>(entt::get<components::Transform>);
        std::optional<entt::entity> rv = std::nullopt;

        spheres.each([&func, &rv, &nearest, &ray](auto const e, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(func(e, transform, sphere.material) && sphere.CheckIntersection(transform, nearest, ray)) {rv = e;} });
        meshes.each([&func, &rv, &nearest, &ray](auto const e, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(func(e, transform, mesh.material()) && mesh.CheckIntersection(transform, nearest, ray)) {rv = e;} });
        return rv;
    }

    bool Scene::FindIntersectedMaterial(SphereGroup &spheres,
                                        MeshGroup &meshes,
                                        core::math::Intersection &nearest,
                                        core::math::Ray const &ray,
                                        render::Material &mat) const noexcept
    {
        if (floor.CheckIntersection(nearest, ray) && floor.material.casts_shadow)
        {
            mat = floor.material;
        }
        spheres.each([&nearest, &ray, &mat](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(sphere.material.casts_shadow && sphere.CheckIntersection(transform, nearest, ray)) { mat = sphere.material; } });
        meshes.each([&nearest, &ray, &mat](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                    { if(mesh.material().casts_shadow && mesh.CheckIntersection(transform, nearest, ray)) { mat = mesh.material(); } });
        return nearest.exists();
    }

    inline bool Scene::FindIntersection(SphereGroup &spheres, MeshGroup &meshes, Intersection &nearest, Ray const &ray) const noexcept
    {
        floor.CheckIntersection(nearest, ray);
        spheres.each([&nearest, &ray](auto const, auto const &sphere, auto const &transform) __lambda_force_inline
                     { sphere.CheckIntersection(transform, nearest, ray); });
        meshes.each([&nearest, &ray](auto const, auto const &mesh, auto const &transform) __lambda_force_inline
                    { mesh.CheckIntersection(transform, nearest, ray); });
        return nearest.exists();
    }

    inline bool Scene::FindIntersectionIf(SphereGroup &spheres, MeshGroup &meshes, Intersection &nearest, Ray const &ray, IntersectionCallbackFn const &func) const noexcept
    {
        if (func(entt::entity{}, floor.transform, floor.material))
        {
            floor.CheckIntersection(nearest, ray);
        }
        spheres.each([&nearest, &ray, &func](auto const entity, auto const &sphere, auto const &transform) __lambda_force_inline
                     { if(func(entity, transform, sphere.material)) { sphere.CheckIntersection(transform, nearest, ray); } });
        meshes.each([&nearest, &ray, &func](auto const entity, auto const &mesh, auto const &transform) __lambda_force_inline
                    {  if(func(entity, transform, mesh.material())) { mesh.CheckIntersection(transform, nearest, ray); } });
        return nearest.exists();
    }
    vec3 Scene::CalculatePointColor(SphereGroup &spheres,
                                    MeshGroup &meshes,
                                    DirectionalLightView &directional_lights,
                                    PointLightGroup &point_lights,
                                    SpotLightGroup &spot_lights,
                                    core::math::Ray const &ray, core::math::Intersection const &nearest,
                                    render::Material const &mat,
                                    int depth)
    {
        vec3 amb = ambient;
        if (!nearest.exists())
        {
            return amb;
        }
        if (global_illumination_on)
        {
            amb = CalculatePointGIAmbient(spheres, meshes, directional_lights, point_lights, spot_lights, nearest, depth);
        }

        core::math::vec3 light_energy{0};

        render::LightData ld{.ray = ray,
                             .point = nearest.point,
                             .normal = nearest.normal,
                             .view_dir = normalize(ray.origin() - nearest.point)};

        light_energy = Illuminate(spheres, meshes, directional_lights, point_lights, spot_lights, mat, ld);

        if (reflections_on && mat.roughness < reflection_roughness_threshold && depth < max_ray_depth)
        {
            float reflectivity = 1.0f - mat.roughness / reflection_roughness_threshold;

            Ray reflect_ray(nearest.point + nearest.normal * 0.001f, normalize(reflect_normal_safe(nearest.normal, ray.direction())));

            Intersection reflect_nearest;
            reflect_nearest.reset();

            render::Material reflect_mat;

            FindIntersectedMaterial(spheres, meshes, reflect_nearest, reflect_ray, reflect_mat);

            light_energy += reflectivity * CalculatePointColor(spheres, meshes, directional_lights, point_lights, spot_lights, reflect_ray, reflect_nearest, reflect_mat, depth + 1);
        }

        return amb * mat.albedo + light_energy + mat.emission;
    }

    core::math::vec3 Scene::Illuminate(SphereGroup &spheres,
                                       MeshGroup &meshes,
                                       DirectionalLightView &directional_lights,
                                       PointLightGroup &point_lights,
                                       SpotLightGroup &spot_lights,
                                       render::Material const &mat,
                                       render::LightData &ld)
    {
        core::math::vec3 light_energy{0};
        auto find_nearest_if_casts_shadow = [this, &spheres, &meshes](Intersection &nearest, Ray &ray) __lambda_force_inline
        {
            return FindIntersectionIf(spheres, meshes, nearest, ray,
                                      [](entt::entity, Transform const &, render::Material const &m) __lambda_force_inline
                                      { return m.casts_shadow; });
        };
        for (auto entity : directional_lights)
        {
            auto const &dirlight = directional_lights.get<DirectionalLight>(entity);

            Ray ray(ld.point + ld.normal * 0.001f, -dirlight.direction);

            Intersection nearest;
            nearest.reset();
            find_nearest_if_casts_shadow(nearest, ray);

            if (!nearest.exists())
            {
                light_energy += dirlight.Illuminate(ld, mat);
            }
        }
        for (auto entity : point_lights)
        {
            auto const &transform = point_lights.get<Transform>(entity);
            auto const &point_light = point_lights.get<PointLight>(entity);

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
            find_nearest_if_casts_shadow(nearest, ray);

            if (!nearest.exists() || nearest.t >= d)
            {
                light_energy += point_light.Illuminate(transform, ld, mat);
            }
        }
        for (auto entity : spot_lights)
        {
            auto const &transform = spot_lights.get<Transform>(entity);
            auto const &spot_light = spot_lights.get<SpotLight>(entity);

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
            find_nearest_if_casts_shadow(nearest, ray);

            if (!nearest.exists() || nearest.t >= d)
            {
                light_energy += spot_light.Illuminate(transform, ld, mat);
            }
        }
        return light_energy;
    }
    vec3 Scene::CalculatePointGIAmbient(SphereGroup &spheres,
                                        MeshGroup &meshes,
                                        DirectionalLightView &directional_lights,
                                        PointLightGroup &point_lights,
                                        SpotLightGroup &spot_lights,
                                        core::math::Intersection const &nearest,
                                        int depth)
    {
        if (depth >= max_ray_depth || !nearest.exists())
        {
            return ambient;
        }

        vec3 rv_color{0};

        // rotation matrix to align (0,1,0) to normal
        mat3 rotation_matrix{1};
        rotation_matrix[2] = nearest.normal;
        render::branchlessONB(nearest.normal, rotation_matrix[0], rotation_matrix[1]);

        for (int i = 0; i < hemisphere_ray_count; i++)
        {

            vec3 const ray_dir = normalize(GetHemispherePoint(i, hemisphere_ray_count) * rotation_matrix);
            vec3 const ray_origin = nearest.point + nearest.normal * 0.001f;

            Ray const hs_ray(ray_origin, ray_dir);

            Intersection hs_nearest;
            hs_nearest.reset();

            render::Material hs_mat;

            if (!FindIntersectedMaterial(spheres, meshes, hs_nearest, hs_ray, hs_mat))
            {
                rv_color += ambient;
                continue;
            }
            render::Color light_energy{0};
            render::LightData ld{.ray = hs_ray,
                                 .point = hs_nearest.point,
                                 .normal = hs_nearest.normal,
                                 .view_dir = normalize(hs_ray.origin() - hs_nearest.point)};
            std::stringstream s;
            light_energy = Illuminate(spheres, meshes, directional_lights, point_lights, spot_lights, hs_mat, ld);
            rv_color += ambient * hs_mat.albedo + light_energy + hs_mat.emission;
        }
        return std::numbers::pi * 2 * rv_color / hemisphere_ray_count;
    }
} // namespace engine