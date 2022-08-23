#pragma once
#include "shader-manager.hpp"
namespace engine::core
{
    template <typename T>
    void ShaderManager::AddShaderForUpdate(std::shared_ptr<T> const &shader_ptr,
                                           ShaderCompileInput const &input,
                                           std::vector<std::filesystem::path> const &dependent_files)
    {
        utils::Assert(utils::for_each_true(
            dependent_files.cbegin(),
            dependent_files.cend(),
            [](auto const &it) __lambda_force_inline -> bool
            {
                return it->is_absolute();
            }),
            "Paths provided should be absolute");

        for (auto &path : dependent_files)
        {
            if (dependent_shaders_map_.find(std::filesystem::hash_value(path)) == dependent_shaders_map_.end())
            {
                dependent_shaders_map_[std::filesystem::hash_value(path)] = std::unordered_set<std::shared_ptr<render::Shader>> {};
            }
            dependent_shaders_map_[std::filesystem::hash_value(path)].emplace(std::static_pointer_cast<render::Shader>(shader_ptr));
            watcher_.AddPathToWatch(path);
        }
        shader_inputs_[std::static_pointer_cast<render::Shader>(shader_ptr)] = input;
    }

    template <typename T>
    std::shared_ptr<T> ShaderManager::CompileShader(ShaderCompileInput const &input)
    {
        utils::Assert(T::kType == input.type);
        ShaderCompileOutput out;
        ShaderCompiler::CompileShader(input, out);
        auto rv = std::make_shared<T>(out.blob);
        AddShaderForUpdate(rv, input, out.dependent_files);
        return rv;
    }
    template <typename T>
    std::shared_ptr<T> ShaderManager::CompileFromPath(std::filesystem::path const &path) { return CompileShader<T>(ShaderCompileInput { .type = T::kType, .source_file = path }); }
}