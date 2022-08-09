#pragma once
#include "core/application.hpp"
#include "core/debug_utils.hpp"
#include "globals.hpp"
#include <filesystem>
#include <optional>

#if defined(_DEBUG)
#define ENGINE_D3D_SHADER_THROWS true
#define ENGINE_D3D_SHADER_NOEXCEPT
#else
#define ENGINE_D3D_SHADER_THROWS false
#define ENGINE_D3D_SHADER_NOEXCEPT noexcept
#endif
namespace engine::direct3d
{
    class ShaderProgram
    {
    public:
#if defined(_DEBUG)
        static constexpr uint32_t kD3DShaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        static constexpr uint32_t kD3DShaderCompileFlags = 0;
#endif

        struct CompilationError : public std::invalid_argument
        {
        public:
            using std::invalid_argument::invalid_argument;
        };
        struct RuntimeError : public std::runtime_error
        {
        public:
            using std::runtime_error::runtime_error;
        };
        ShaderProgram(std::filesystem::path const &vertex_path,
                      std::filesystem::path const &pixel_path,
                      std::vector<D3D11_INPUT_ELEMENT_DESC> const &ied) ENGINE_D3D_SHADER_NOEXCEPT;

        [[nodiscard]] inline direct3d::VertexShader vertex_shader() const noexcept { return vertex_shader_; }
        [[nodiscard]] inline direct3d::PixelShader pixel_shader() const noexcept { return pixel_shader_; }
        void apply_shader();

        static direct3d::Buffer InitializeUniformBuffer(uint32_t uniform_buffer_size);

    private:
        std::pair<ID3D11VertexShader *, ID3D11InputLayout *> CompileVertexShader(std::filesystem::path const &vertex_path,
                                                                                 std::vector<D3D11_INPUT_ELEMENT_DESC> const &ied) const;
        ID3D11PixelShader *CompilePixelShader(std::filesystem::path const &pixel_path);

        direct3d::VertexShader vertex_shader_;
        direct3d::PixelShader pixel_shader_;
        direct3d::InputLayout input_layout_;
    };
} // namespace engine::direct3d