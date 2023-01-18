#pragma once
#include "render/shader-program.hpp"

namespace engine::core
{
    struct ShaderMacro
    {
        std::string name;
        std::string definition;
    };

    struct ShaderCompileInput
    {
        enum CompileFlags
        {
            FlagNone = 0,
            FlagDebug = 1 << 0,
            FlagDisableOptimization = 1 << 1,
        };
        direct3d::ShaderType type = direct3d::ShaderType::None;
        std::filesystem::path source_file;
        std::string entrypoint = "";
        std::vector<ShaderMacro> macros = {};
#if defined(_DEBUG)
        uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
        uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    };

    struct ShaderCompileOutput
    {
        render::ShaderBlob blob;
        std::vector<std::filesystem::path> dependent_files;
    };

    namespace ShaderCompiler
    {
        struct CompilerException : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };
        void CompileShader(ShaderCompileInput const &input, ShaderCompileOutput &output);
        void GetBlobFromCompiledShader(std::filesystem::path const &filename, render::ShaderBlob &blob);
    }
}

namespace std
{
    template <>
    struct hash<engine::core::ShaderCompileInput>
    {
        size_t operator()(engine::core::ShaderCompileInput const &input) const
        {
            size_t hash = 0;
            engine::utils::hash_combine(hash, input.type);
            engine::utils::hash_combine(hash, input.source_file);
            engine::utils::hash_combine(hash, input.entrypoint);
            engine::utils::hash_combine(hash, input.flags);
            for (auto const &macro : input.macros)
            {
                engine::utils::hash_combine(hash, macro.name);
                engine::utils::hash_combine(hash, macro.definition);
            }
            return hash;
        }
    };
}