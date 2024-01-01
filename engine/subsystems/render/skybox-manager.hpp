#pragma once
#include "render/render.hpp"
#include "components/components.hpp"
#include "../core/texture-manager.hpp"
namespace engine
{
    namespace core
    {
        // Forward declaration for friend class
        class Engine;
    }
    namespace components
    {
        struct SkyboxComponent
        {
            TextureId texture_id = std::numeric_limits<TextureId>::max();
        };
    }
    class SkyboxManager
    {
    public:
        static entt::entity LoadSkybox(entt::registry &registry, std::filesystem::path const &path);
        static entt::entity LoadSkybox(entt::registry &registry, std::array<std::filesystem::path, 6> const &paths);
        static void RenderSkybox(components::SkyboxComponent const &skybox, render::PerFrame const &per_frame);

    private:
        friend class core::Engine;
        static void Init();
        static void Deinit();

    private:
        static auto constexpr skybox_vs_shader_path = "assets/shaders/skybox/skybox-vs.hlsl";
        static auto constexpr skybox_ps_shader_path = "assets/shaders/skybox/skybox-ps.hlsl";

        static render::GraphicsShaderProgram skybox_shader_;
        static std::unique_ptr<direct3d::DynamicUniformBuffer<glm::mat4x3>> skybox_buffer_;
        static direct3d::DepthStencilState skybox_depth_state;
    };
}