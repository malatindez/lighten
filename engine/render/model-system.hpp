#pragma once
#include "detail/opaque-render-system.hpp"
#include "components/components.hpp"
#include "core/math/ray.hpp"
#include "model.hpp"
namespace engine::components
{
    struct ModelComponent
    {
        uint32_t model_id;
    };
}
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
            engine::core::math::Ray const &ray,
            engine::core::math::Intersection &nearest);

        void Render() { opaque_render_system_.Render(); }

        void OnInstancesUpdated(entt::registry &registry)
        {
            opaque_render_system_.OnInstancesUpdated(registry);
        }

    public:
        [[nodiscard]] static std::shared_ptr<ModelSystem> instance() noexcept { return instance_; }
        [[nodiscard]] static Model &GetModel(uint32_t model_id) { return instance_->models_[model_id]; }

    private:
        friend class ::engine::core::ModelLoader;

        static uint32_t AddModel(Model &&model);
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
        _detail::OpaqueRenderSystem opaque_render_system_;

    private:
        static std::shared_ptr<ModelSystem> instance_;

    private:
        std::vector<Model> models_;
    };
} // namespace engine::render