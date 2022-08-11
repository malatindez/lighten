#pragma once
#include "direct3d11/direct3d11.hpp"
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
        render::ShaderType type = render::ShaderType::None;
        std::filesystem::path source_file;
        std::string entrypoint = "";
        std::vector<ShaderMacro> macros;
#if defined(_DEBUG)
        uint64_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
        uint64_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
    };

    struct ShaderCompileOutput
    {
        render::ShaderBlob blob;
        std::unordered_set<std::filesystem::path> dependent_files;
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