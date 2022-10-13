#pragma once
#include "detail/opaque-render-system.hpp"
#include "detail/emissive-render-system.hpp"
#include "components/components.hpp"
#include "core/math.hpp"
#include "render/model.hpp"
namespace engine::core
{
    class Engine;
    class ModelLoader;
}
namespace engine::render
{
    class ModelSystem final
    {
    public:
        ~ModelSystem() = default;

        static std::optional<entt::entity> FindIntersection(entt::registry &registry,
                                                            core::math::Ray const &ray,
                                                            core::MeshIntersection &nearest);
        static std::optional<entt::entity> FindIntersection(entt::registry &registry,
                                                            core::math::Ray const &ray,
                                                            core::math::Intersection &nearest);

        void Render(entt::registry &registry)
        {
            opaque_render_system_.Render(registry);
            emissive_render_system_.Render();
        }

        void OnInstancesUpdated(entt::registry &registry)
        {
            opaque_render_system_.OnInstancesUpdated(registry);
            emissive_render_system_.OnInstancesUpdated(registry);
        }
    public:
        [[nodiscard]] static ModelSystem &instance() noexcept { utils::Assert(instance_ != nullptr); return *instance_; }
        [[nodiscard]] static Model &GetModel(uint64_t model_id) { return instance().models_.find(model_id)->second; }

    public:
        uint64_t AddModel(Model &&model);
        [[nodiscard]] auto &opaque_render_system() const noexcept { return opaque_render_system_; }
        [[nodiscard]] auto &emissive_render_system() const noexcept { return emissive_render_system_; }
        [[nodiscard]] auto &opaque_render_system() noexcept { return opaque_render_system_; }
        [[nodiscard]] auto &emissive_render_system() noexcept { return emissive_render_system_; }

        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            opaque_render_system_.AddInstance(model_id, registry, entity);
        }
        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials)
        {
            opaque_render_system_.AddInstance(model_id, registry, entity, materials);
        }
        inline void AddEmissiveInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            emissive_render_system_.AddInstance(model_id, registry, entity);
        }
        inline void AddEmissiveInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<EmissiveMaterial> const &materials)
        {
            emissive_render_system_.AddInstance(model_id, registry, entity, materials);
        }

        static uint64_t GetUnitSphereFlat();
        static uint64_t GetUnitCube();

    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<ModelSystem>(new ModelSystem());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        ModelSystem() = default;

        // delete move & copy semantics
        ModelSystem(ModelSystem &&) = delete;
        ModelSystem(ModelSystem const &) = delete;
        ModelSystem &operator=(ModelSystem &&) = delete;
        ModelSystem &operator=(ModelSystem const &) = delete;

    private:
        _opaque_detail::OpaqueRenderSystem opaque_render_system_;
        _emissive_detail::EmissiveRenderSystem emissive_render_system_;

    private:
        static std::shared_ptr<ModelSystem> instance_;

    private:
        std::unordered_map<uint64_t, Model> models_;
        uint64_t current_index = 0;
    };
} // namespace engine::render