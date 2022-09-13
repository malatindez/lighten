#include "model-system.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_;
    namespace
    {
        inline bool CheckMeshIntersection(
            Mesh const &mesh,
            Ray const &ray,
            Intersection &nearest)
        {
            bool rv = false;
            for (size_t i = 0; i < mesh.indices.size();)
            {
                auto const &p0 = mesh.vertices[mesh.indices[i++]].coords;
                auto const &p1 = mesh.vertices[mesh.indices[i++]].coords;
                auto const &p2 = mesh.vertices[mesh.indices[i++]].coords;
                auto const U = p1 - p0;
                auto const V = p2 - p0;
                auto const normal = cross(U, V);
                bool a = Triangle::Intersect(p0, p1, p2, normal, nearest, ray);
                if (a)
                {
                    spdlog::info("intersected");
                    rv = true;
                }
            }
            return rv;
        }
        bool CheckForIntersection(Model const &model,
                                  components::TransformComponent transform,
                                  Ray const &ray,
                                  core::MeshIntersection &nearest)
        {
            bool rv = false;
            for (auto const &mesh : model.meshes)
            {
                mat4 mat = mesh.mesh_to_model * transform.model;
                mat4 inv_mat = inverse(mat);
                Ray mesh_local = ray;
                mesh_local.origin() = (core::math::vec4{ mesh_local.origin(), 1 } *inv_mat).xyz;
                mesh_local.SetDirection((core::math::vec4{ mesh_local.direction(), 0 } *inv_mat).xyz);
                float temp = nearest.t;
                bool t = mesh.triangle_octree.intersect(mesh_local, nearest);
                if (t)
                {
                    nearest.point = (core::math::vec4{ nearest.point, 1 } *mat).xyz;
                    nearest.normal = (core::math::vec4{ nearest.normal, 0 } *mat).xyz;
                    rv = t / core::math::length(mesh_local.direction());
                }
            }
            return rv;
        }
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              core::math::Ray const &ray,
                                                              core::MeshIntersection &nearest)
    {
        auto group = registry.group<components::OpaqueComponent, components::TransformComponent>();
        std::optional<entt::entity> rv = std::nullopt;
        group.each([&rv, &ray, &nearest] (auto const entity, auto const &model_instance, auto const &transform)
                   {
                       auto const &model = GetModel(model_instance.model_id);
                       if (CheckForIntersection(model, transform, ray, nearest))
                       {
                           rv = entity;
                       }
                   });
        return rv;
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              Ray const &ray,
                                                              Intersection &nearest)
    {
        core::MeshIntersection mesh_intersection;
        mesh_intersection.normal = nearest.normal;
        mesh_intersection.t = nearest.t;
        mesh_intersection.point = nearest.point;
        auto rv = FindIntersection(registry, ray, mesh_intersection);
        nearest.normal = mesh_intersection.normal;
        nearest.t = mesh_intersection.t;
        nearest.point = mesh_intersection.point;
        return rv;
    }
    uint64_t ModelSystem::AddModel(Model &&model_)
    {
        instance_->models_.emplace(current_index, std::move(model_));
        return current_index++;
    }

} // namespace engine::render