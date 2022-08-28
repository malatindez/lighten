#pragma once
#include "render/render.hpp"
#include "components/components.hpp"
#include "core/layers/texture-manager.hpp"
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
        static void RenderSkybox(components::SkyboxComponent const &skybox, core::math::vec3 const &camera_pos);
    private:
        friend class core::Engine;
        static void Init();
        static void Deinit();
    private:
        static auto constexpr skybox_vertex_shader_path = "assets/shaders/skybox/vs.hlsl";
        static auto constexpr skybox_pixel_shader_path = "assets/shaders/skybox/ps.hlsl";

        static std::unique_ptr<direct3d::ImmutableVertexBuffer<core::math::vec3>> cube_vb_;
        static std::unique_ptr<direct3d::ImmutableIndexBuffer<uint32_t>> cube_ib_;
        static std::unique_ptr<direct3d::DynamicUniformBuffer<core::math::mat4>> camera_model_buffer;
        static render::GraphicsShaderProgram skybox_shader_;
    };
}