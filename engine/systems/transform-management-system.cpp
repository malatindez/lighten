#include "transform-management-system.hpp"
#include "core/engine.hpp"
namespace lighten::systems
{
    using TransformView = entt::basic_view<entt::entity, entt::type_list<components::Transform, components::WorldTransform, components::GameObject>, entt::type_list<>>;

    TransformManagementSystem::TransformManagementSystem(std::shared_ptr<core::World> world) noexcept : 
                                core::Layer("Transform Management System"),
                                observer{world->registry(), 
                                         entt::collector.update<components::Transform>().group<components::WorldTransform, components::GameObject>()
                                        },
                                world_{ world }
    {
        world->registry().on_construct<components::Transform>().connect<&entt::registry::emplace_or_replace<components::WorldTransform>>();
        world->registry().on_destroy<components::Transform>().connect<&entt::registry::remove<components::WorldTransform>>();
    }
    void UpdateTree(std::unordered_set<entt::entity> &updated,
                                                      TransformView &view,
                                                      entt::entity entity,
                                                      components::WorldTransform *parent_world)
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

        updated.emplace(entity);
        for (auto child : game_object.children)
        {
            UpdateTree(updated, view, child, &world);
        }
    }

    void TransformManagementSystem::OnPreUpdate() noexcept
    {
        if (observer.empty())
        {
            return;
        }
        std::unordered_set<entt::entity> updated;
        updated.reserve(observer.size()); // minimum observer size

        auto view = world_->registry().view<components::Transform, components::WorldTransform, components::GameObject>();
        observer.each([&view, &updated](auto entity)
                      {
                if (auto it = updated.find(entity); it == updated.end())
                {
                    auto& game_object = view.get<components::GameObject>(entity);
                    components::WorldTransform *parent_world = nullptr;
                    // If you get an exception here, it means that the object was unlawfully created from other part of the code. 
                    // Any game object should have Transform. WorldTransform is generated automatically.
                    // In nearly all cases you should use CreateGameObject from core::World.
                    parent_world = &view.get<components::WorldTransform>(game_object.parent);

                    UpdateTree(updated, view, entity, parent_world);
                } });
        observer.clear();
        // TODO: 
        // remove scheduling, add observers in render systems
        //core::Engine::scene()->renderer->emissive_render_system().ScheduleInstanceUpdate();
        //core::Engine::scene()->renderer->light_render_system().ScheduleInstanceUpdate();
        //core::Engine::scene()->renderer->opaque_render_system().ScheduleInstanceUpdate();
        //core::Engine::scene()->renderer->dissolution_render_system().ScheduleInstanceUpdate();
        //core::Engine::scene()->renderer->decal_render_system().ScheduleInstanceUpdate();
    }
  
} // namespace lighten::systems