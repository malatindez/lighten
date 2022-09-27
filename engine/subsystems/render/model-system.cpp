#include "model-system.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_;
    namespace
    {
        bool CheckForIntersection(Model const &model,
                                  components::TransformComponent transform,
                                  Ray const &ray,
                                  core::MeshIntersection &nearest)
        {
            bool rv = false;
            for (auto const &mesh : model.meshes)
            {
                mat4 mat = transform.model * mesh.mesh_to_model;
                mat4 inv_mat = mesh.inv_mesh_to_model * transform.inv_model;
                Ray mesh_local = ray;
                mesh_local.origin() = (inv_mat * core::math::vec4{ mesh_local.origin(), 1 }).xyz;
                mesh_local.SetDirection((inv_mat * core::math::vec4{ mesh_local.direction(), 0 }).xyz);
                bool t = mesh.triangle_octree.intersect(mesh_local, nearest);
                if (t)
                {
                    nearest.point = (mat * core::math::vec4{ nearest.point, 1 }).xyz;
                    nearest.normal = (mat * core::math::vec4{ nearest.normal, 0 }).xyz;
                    rv = t;
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
        mesh_intersection.near = std::numeric_limits<float>::max();
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