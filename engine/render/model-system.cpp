#include "model-system.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_;
    namespace
    {
        inline bool CheckForIntersection(AABB const &aabb,
            Ray const &ray,
            Intersection &nearest)
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            auto const &bounds = reinterpret_cast<std::array<vec3, 2> const &>(aabb);

            tmin = (bounds[ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tmax = (bounds[1ll - ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tymin = (bounds[ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;
            tymax = (bounds[1ll - ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (bounds[ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;
            tzmax = (bounds[1ll - ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            float t = tmin;

            if (t < 0 || t < nearest.t)
            {
                t = tmax;
                if (t < 0 || t < nearest.t)
                    return false;
            }
            nearest.t = t;
            return true;
        }
        bool CheckForIntersection(Model const &model,
            components::TransformComponent transform,
            Ray const &ray,
            Intersection &nearest)
        {
            Ray local = ray;
            local.SetDirection(core::math::as_vec<3>(core::math::vec4{ local.direction(), 0 } *transform.inv_model));
            local.origin() = core::math::as_vec<3>(core::math::vec4{ local.origin(), 1 } *transform.inv_model);
            bool rv = false;
            for (auto const &mesh : model.meshes)
            {
                rv |= CheckForIntersection(mesh.mesh_range.bounding_box, ray, nearest);
            }
            return rv;
        }
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
        Ray const &ray,
        Intersection &nearest)
    {
        auto group = registry.group<>(entt::get<components::ModelComponent, components::TransformComponent>);
        std::optional<entt::entity> rv = std::nullopt;
        group.each([&rv, &ray, &nearest](auto const entity, auto const &mesh_component, auto const &transform)
            {
                if (CheckForIntersection(instance_->models_[mesh_component.model_id], transform, ray, nearest))
                {
                    rv = entity;
                }
            });
        return rv;
    }
    uint32_t ModelSystem::AddModel(Model &&model)
    {
        instance_->models_.emplace_back(std::move(model));
        return (uint32_t)instance_->models_.size() - 1;
    }

} // namespace engine::render