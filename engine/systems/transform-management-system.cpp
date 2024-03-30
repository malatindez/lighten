#include "transform-management-system.hpp"
#include "core/engine.hpp"
namespace lighten::systems
{
    TransformManagementSystem::TransformManagementSystem(entt::registry &registry) noexcept : registry{registry},
                                                                                              observer{registry, entt::collector.update<components::Transform>().group<components::WorldTransform, components::GameObject>()}
    {
        registry.on_construct<components::Transform>().connect<&entt::registry::emplace_or_replace<components::WorldTransform>>();
        registry.on_destroy<components::Transform>().connect<&entt::registry::remove<components::WorldTransform>>();
    }
    void UpdateTree(std::unordered_map<entt::entity, uint32_t> &updated,
                                                      TransformManagementSystem::TransformView &view,
                                                      entt::entity entity,
                                                      components::WorldTransform *parent_world,
                                                      uint32_t function_depth = 0)
    {
        auto &transform = view.get<components::Transform>(entity);
        auto &world = view.get<components::WorldTransform>(entity);
        auto &game_object = view.get<components::GameObject>(entity);
        world.UpdateModelMatrices(transform);
        if (parent_world)
        {
            world.UpdateWorldMatrices(*parent_world);
        }
        else
        {
            world.UpdateWorldMatrices();
        }

        updated.emplace(entity, function_depth);
        for (auto child : game_object.children)
        {
            UpdateTree(updated, view, child, &world, function_depth + 1u);
        }
    }

    void TransformManagementSystem::OnPreUpdate() noexcept
    {
        if (observer.empty())
        {
            return;
        }
        std::unordered_map<entt::entity, uint32_t> updated;
        updated.reserve(observer.size()); // minimum observer size

        auto view = registry.view<components::Transform, components::WorldTransform, components::GameObject>();
        observer.each([&view, &updated](auto entity)
                      {
                if (auto it = updated.find(entity); it == updated.end())
                {
                    auto& game_object = view.get<components::GameObject>(entity);
                    components::WorldTransform *parent_world = nullptr;
                    if (game_object.parent != entt::null && view.contains(game_object.parent))
                    {
                        parent_world = &view.get<components::WorldTransform>(game_object.parent);
                    }
                    UpdateTree(updated, view, entity, parent_world);
                } });
        observer.clear();

        core::Engine::scene()->renderer->emissive_render_system().ScheduleInstanceUpdate();
        core::Engine::scene()->renderer->light_render_system().ScheduleInstanceUpdate();
        core::Engine::scene()->renderer->opaque_render_system().ScheduleInstanceUpdate();
        core::Engine::scene()->renderer->dissolution_render_system().ScheduleInstanceUpdate();
        core::Engine::scene()->renderer->decal_render_system().ScheduleInstanceUpdate();
    }
  
} // namespace lighten::systems