#pragma once
#include "shader-compiler.hpp"
#include "core/layers/layer.hpp"
#include "misc/file-watcher.hpp"
namespace lighten::core
{
    class Engine;
    class ShaderManager final : public Layer, public Layer::HandleEvent, public Layer::HandleUpdate
    {
    public:
        void OnUpdate() override { watcher_.OnUpdate(); }
        void OnEvent(events::Event &e) override;

        template <typename T>
        void AddShaderForUpdate(std::shared_ptr<T> const &shader_ptr,
                                ShaderCompileInput const &input,
                                std::vector<std::filesystem::path> const &dependent_files);

        template <typename T>
        std::shared_ptr<T> CompileShader(ShaderCompileInput const &input);
        template <typename T>
        std::shared_ptr<T> CompileFromPath(std::filesystem::path const &path);

        auto CompileVertexShader(std::filesystem::path const &input) { return CompileFromPath<render::VertexShader>(input); }
        auto CompileVertexShader(ShaderCompileInput const &input) { return CompileShader<render::VertexShader>(input); }

        auto CompilePixelShader(std::filesystem::path const &input) { return CompileFromPath<render::PixelShader>(input); }
        auto CompilePixelShader(ShaderCompileInput const &input) { return CompileShader<render::PixelShader>(input); }

        auto CompileHullShader(std::filesystem::path const &input) { return CompileFromPath<render::HullShader>(input); }
        auto CompileHullShader(ShaderCompileInput const &input) { return CompileShader<render::HullShader>(input); }

        auto CompileDomainShader(std::filesystem::path const &input) { return CompileFromPath<render::DomainShader>(input); }
        auto CompileDomainShader(ShaderCompileInput const &input) { return CompileShader<render::DomainShader>(input); }

        auto CompileGeometryShader(std::filesystem::path const &input) { return CompileFromPath<render::GeometryShader>(input); }
        auto CompileGeometryShader(ShaderCompileInput const &input) { return CompileShader<render::GeometryShader>(input); }

        auto CompileComputeShader(std::filesystem::path const &input) { return CompileFromPath<render::ComputeShader>(input); }
        auto CompileComputeShader(ShaderCompileInput const &input) { return CompileShader<render::ComputeShader>(input); }

        [[nodiscard]] static std::shared_ptr<ShaderManager> instance() noexcept { return instance_; }

        void OnAttach() override
        {
            mal_toolkit::Assert(!attached_);
            attached_ = true;
        }
        void OnDetach() override
        {
            mal_toolkit::Assert(attached_);
            attached_ = false;
        }

    private:
        friend class ::lighten::core::Engine;

        static void Init()
        {
            mal_toolkit::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<ShaderManager>(new ShaderManager());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        ShaderManager() :
            core::Layer("ShaderManager"),
            watcher_{[this](events::Event &e) __mal_toolkit_lambda_force_inline 
                                   { OnEvent(e); }}
        {
        }

        // delete move & copy semantics
        ShaderManager(ShaderManager &&) = delete;
        ShaderManager(ShaderManager const &) = delete;
        ShaderManager &operator=(ShaderManager &&) = delete;
        ShaderManager &operator=(ShaderManager const &) = delete;

    private:
        static std::shared_ptr<ShaderManager> instance_;

    private:
        bool attached_ = false;
        std::unordered_map<size_t, std::unordered_set<std::shared_ptr<render::Shader>>> dependent_shaders_map_{};
        std::unordered_map<std::shared_ptr<render::Shader>, ShaderCompileInput> shader_inputs_{};
        misc::FileWatcher watcher_;
    };
}
#include "shader-manager.inl"