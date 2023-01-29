#pragma once
#include "components/components.hpp"
#include "core/math.hpp"
#include "render/common.hpp"
#include "subsystems/render/detail/dissolution-render-system.hpp"
#include "subsystems/render/detail/emissive-render-system.hpp"
#include "subsystems/render/detail/grass-render-system.hpp"
#include "subsystems/render/detail/decal-render-system.hpp"
#include "subsystems/render/detail/light-render-system.hpp"
#include "subsystems/render/detail/opaque-render-system.hpp"
#include "subsystems/render/detail/emissive-particle-render-system.hpp"
#include "subsystems/render/detail/particle-render-system.hpp"
#include "subsystems/render/model-system.hpp"
namespace engine::core
{
    class Scene;
}
namespace engine::render
{
    // TODO(malatindez):
    // Refactor the renderer code to use a render graph.

    // The renderer itself should just contain the render graph.
    // Render graph itself should initialize the render passes required data and call the render passes.
    // The render graph should be able to link the subsystems to the entt::registry of the scene.
    // To do so, renderer should get the scene as a parameter to the constructor
    // and the render graph should get the scene as a parameter to the constructor.

    // The render graph should be able to be serialized and deserialized.
    // The render graph should be able to be modified at runtime.
    // The render graph should be able to be used to render to multiple targets.
    // The render graph should be able to be used to render to multiple targets at the same time.
    // The render graph should be able to be used to render to multiple targets at the same time with different render passes.
    // The renderer should be able to be used with different render graphs.
    // Render passes should be able to be added/removed/modified to/from/at the render graph at runtime.
    // Render pass node should contain:
    // - Render pass
    // - Render pass inputs
    // - Render pass outputs
    // - Render pass dependencies
    // - Next render pass/es
    // - Previous render pass/es and input data from them
    // - Render pass node type
    // - Render pass node name
    // - Render pass node description
    // - Render pass node parameters
    // - Render pass node parameters description
    // - Render pass node parameters type
    // - Render pass node parameters default value
    // - Render pass node parameters min value
    // - Render pass node parameters max value
    // - Render pass node parameters step value
    // - Render pass node parameters enum values
    // - Render pass node parameters enum values description
    // - Render pass node parameters enum values default value
    // - Render pass node parameters enum values min value
    // - Render pass node parameters enum values max value

    // For now, I'll use this bad implementation.
    // I'll refactor it later.

    class Renderer
    {
    private:
        struct SkyboxRenderWrapper : public RenderPass
        {
            SkyboxRenderWrapper() : RenderPass(0x2FFFF) {}
            render::PerFrame const *per_frame_ptr = nullptr;
            void OnRender(core::Scene *scene);
        };
    public:
        Renderer()
        {
            skybox_render_pass_ = std::make_shared<SkyboxRenderWrapper>();
            AddRenderPass(skybox_render_pass_);
            opaque_render_system_ = std::make_shared<_opaque_detail::OpaqueRenderSystem>();
            AddRenderPass(opaque_render_system_);
            dissolution_render_system_ = std::make_shared<_dissolution_detail::DissolutionRenderSystem>();
            AddRenderPass(dissolution_render_system_);
            grass_render_system_ = std::make_shared<_grass_detail::GrassRenderSystem>();
            AddRenderPass(grass_render_system_);
            emissive_render_system_ = std::make_shared<_emissive_detail::EmissiveRenderSystem>();
            AddRenderPass(emissive_render_system_);
            light_render_system_ = std::make_shared<_light_detail::LightRenderSystem>();
            AddRenderPass(light_render_system_);
            particle_render_system_ = std::make_shared<_particle_detail::ParticleRenderSystem>();
            AddRenderPass(particle_render_system_);
            decal_render_system_ = std::make_shared<_decal_detail::DecalRenderSystem>();
            AddRenderPass(decal_render_system_);
            emissive_particle_render_system_ = std::make_shared<_emissive_particle_detail::EmissiveParticleRenderSystem>();
            AddRenderPass(emissive_particle_render_system_);
        }
        Renderer(Renderer const &) = delete;
        Renderer &operator=(Renderer const &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(Renderer &&) = delete;
        virtual ~Renderer() = default;

        [[nodiscard]] auto &opaque_render_system() const noexcept { return *opaque_render_system_; }
        [[nodiscard]] auto &dissolution_render_system() const noexcept { return *dissolution_render_system_; }
        [[nodiscard]] auto &grass_render_system() const noexcept { return *grass_render_system_; }
        [[nodiscard]] auto &emissive_render_system() const noexcept { return *emissive_render_system_; }
        [[nodiscard]] auto &decal_render_system() const noexcept { return *decal_render_system_; }
        [[nodiscard]] auto &light_render_system() const noexcept { return *light_render_system_; }
        [[nodiscard]] auto &emissive_particle_render_system() const noexcept { return *emissive_particle_render_system_; }
        [[nodiscard]] auto &particle_render_system() const noexcept { return *particle_render_system_; }

        [[nodiscard]] auto &opaque_render_system() noexcept { return *opaque_render_system_; }
        [[nodiscard]] auto &dissolution_render_system() noexcept { return *dissolution_render_system_; }
        [[nodiscard]] auto &grass_render_system() noexcept { return *grass_render_system_; }
        [[nodiscard]] auto &emissive_render_system() noexcept { return *emissive_render_system_; }
        [[nodiscard]] auto &decal_render_system() noexcept { return *decal_render_system_; }
        [[nodiscard]] auto &light_render_system() noexcept { return *light_render_system_; }
        [[nodiscard]] auto &emissive_particle_render_system() noexcept { return *emissive_particle_render_system_; }
        [[nodiscard]] auto &particle_render_system() noexcept { return *particle_render_system_; }

        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            opaque_render_system_->AddInstance(model_id, registry, entity);
        }

        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials)
        {
            opaque_render_system_->AddInstance(model_id, registry, entity, materials);
        }

        inline void AddEmissiveInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            emissive_render_system_->AddInstance(model_id, registry, entity);
        }
        template<typename T>
        inline void AddRenderPass(std::shared_ptr<T> const &ptr)
        {
            auto render_pass = static_pointer_cast<RenderPass>(ptr);
            utils::SortedInsert<std::shared_ptr<RenderPass>>(render_passes_, std::move(render_pass),
                                                             [](auto const &lhs, auto const &rhs) __lambda_force_inline -> bool
                                                             { return lhs->kPriority < rhs->kPriority; });
        }
        template<typename T>
        inline void RemoveRenderPass(std::shared_ptr<T> const &ptr)
        {
            auto render_pass = static_pointer_cast<RenderPass>(ptr);
            utils::SortedErase<std::shared_ptr<RenderPass>>(render_passes_, std::move(render_pass),
                                                            [](auto const &lhs, auto const &rhs) __lambda_force_inline -> bool
                                                            { return lhs->kPriority < rhs->kPriority; });
        }
        void FrameBegin(core::Scene *scene);
        void DeferredRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv);
        void ForwardRender(core::Scene *scene, render::PerFrame const &per_frame, GBuffer const &buffer, ID3D11DepthStencilView *dsv);
        void Tick(core::Scene *scene, float delta_time);
        void Update(core::Scene *scene);
        void ScheduleInstanceUpdate();

    private:
        std::vector<std::shared_ptr<RenderPass>> render_passes_;
        std::shared_ptr<SkyboxRenderWrapper> skybox_render_pass_;
        std::shared_ptr<_opaque_detail::OpaqueRenderSystem> opaque_render_system_;
        std::shared_ptr<_dissolution_detail::DissolutionRenderSystem> dissolution_render_system_;
        std::shared_ptr<_grass_detail::GrassRenderSystem> grass_render_system_;
        std::shared_ptr<_emissive_detail::EmissiveRenderSystem> emissive_render_system_;
        std::shared_ptr<_decal_detail::DecalRenderSystem> decal_render_system_;
        std::shared_ptr<_light_detail::LightRenderSystem> light_render_system_;
        std::shared_ptr<_emissive_particle_detail::EmissiveParticleRenderSystem> emissive_particle_render_system_;
        std::shared_ptr<_particle_detail::ParticleRenderSystem> particle_render_system_;
    };
}