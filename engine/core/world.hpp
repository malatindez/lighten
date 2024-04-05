#pragma once
#include "../components/components.hpp"
namespace lighten::core
{
    /**
     * @brief The world class which holds the registry.
     * All of the scenes are stored in the world.
     */
    class World
    {
    public:
        World()
        {
            world_scene_ = CreateScene("World");
            registry_.on_destroy<::lighten::components::GameObject>().connect<&World::OnDestroy>(*this);
        }
        ~World() = default;
        
        // TODO:
        // Make an observer which gathers all deleted entities and remove them in PostUpdate for better performance.
        void OnDestroy(entt::registry &, entt::entity entity)
        {
            auto &game_object = registry_.get<::lighten::components::GameObject>(entity);
            if (game_object.parent != entt::null)
            {
                auto &parent_game_object = registry_.get<::lighten::components::GameObject>(game_object.parent);
                parent_game_object.children.erase(std::remove(parent_game_object.children.begin(), parent_game_object.children.end(), entity), parent_game_object.children.end());
            }
            for (auto child : game_object.children)
            {
                auto &child_game_object = registry_.get<::lighten::components::GameObject>(child);
                child_game_object.parent = game_object.parent;
            }
        }
        
        
        [[nodiscard]] entt::entity CreateGameObject(std::string name, entt::entity parent, components::GameObject &parent_obj)
        {
            auto entity = registry_.create();

            parent_obj.children.push_back(entity);

            registry_.emplace<components::Transform>(entity, components::Transform::Default());

            components::GameObject game_object;
            game_object.name = name;
            game_object.parent = parent;

            registry_.emplace<components::GameObject>(entity, std::move(game_object));

            return entity;
        }
        
        [[nodiscard]] entt::entity CreateGameObject(std::string name, entt::entity parent = entt::null)
        {
            if(parent == entt::null)
            {
                parent = world_scene_;
            }
            auto& parent_game_object = registry_.get<components::GameObject>(parent);
            return CreateGameObject(name, parent, parent_game_object);
        }

        entt::entity CreateScene(std::string name = "Scene")
        {
            auto entity = CreateGameObject(name);
            registry_.emplace<components::Scene>(entity);
            return entity;
        }
        
        [[nodiscard]] entt::registry const &registry()      const   noexcept { return registry_; }
        [[nodiscard]] entt::entity          world_scene()   const   noexcept { return world_scene_; }

        [[nodiscard]] entt::registry       &registry()              noexcept { return registry_; }
        
    private:
        entt::entity world_scene_;
        entt::registry registry_;
    };
} // namespace lighten::core