#include "shader-manager.hpp"
#include <coroutine>
namespace engine::core
{
    std::shared_ptr<ShaderManager> ShaderManager::instance_ = nullptr;

    void ShaderManager::OnEvent(events::Event &e)
    {
        std::unordered_set<std::shared_ptr<render::Shader>> compiled_shaders_;
        if (e.type() == events::EventType::FilesChanged) [[unlikely]]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto &fce = static_cast<events::FilesChangedEvent &>(e);
            for (auto const &path : fce.files_changed())
            {
                if (dependent_shaders_map_.find(std::filesystem::hash_value(path)) == dependent_shaders_map_.end()) [[unlikely]]
                {
                    continue;
                }
                std::mutex compile_mutex;
                auto const recompile_task = [&](std::shared_ptr<render::Shader> const &shader) __lambda_force_inline -> void
                {
                    {
                        std::lock_guard<std::mutex> lock(compile_mutex);
                    }
                    ShaderCompileOutput out;

                    utils::HighResolutionTimer timer;
                    try
                    {
                        ShaderCompiler::CompileShader(shader_inputs_[shader], out);
                    }
                    catch (ShaderCompiler::CompilerException e)
                    {
                        std::lock_guard<std::mutex> lock(compile_mutex);
                        spdlog::info("Failed to compiler shader @ " + shader_inputs_[shader].source_file.string());
                        spdlog::warn(e.what());
                        return;
                    }
                    shader->Recreate(out.blob);

                    std::string tmp = std::to_string(timer.elapsed() * 1000);
                    if (size_t dot = tmp.find_last_of('.');
                        tmp.size() - dot > 5)
                    {
                        tmp = tmp.substr(0, dot + 5);
                    }
                    {
                        std::lock_guard<std::mutex> lock(compile_mutex);
                        spdlog::info("Recompiled shader in " + tmp + " ms @ " + shader_inputs_[shader].source_file.string());
                        for (auto &subpath : out.dependent_files)
                        {
                            dependent_shaders_map_[std::filesystem::hash_value(subpath)].emplace(shader);
                            watcher_.AddPathToWatch(subpath);
                        }
                    }
                };
                utils::HighResolutionTimer timer;
                {
                    std::vector<std::jthread> threads;

                    spdlog::info("Detected change of the source code of the file @ " + path.string());
                    for (auto const &shader : dependent_shaders_map_.at(std::filesystem::hash_value(path)))
                    {
                        if (compiled_shaders_.find(shader) != compiled_shaders_.end()) [[unlikely]]
                        {
                            continue;
                        }
                            if (dependent_shaders_map_.find(std::filesystem::hash_value(path)) == dependent_shaders_map_.end()) [[unlikely]]
                            {
                                dependent_shaders_map_[std::filesystem::hash_value(path)] = std::unordered_set<std::shared_ptr<render::Shader>>{};
                            }
                                try
                            {
                                std::ifstream o(path);
                                o.close();
                            }
                            catch (std::exception const &e)
                            {
                                spdlog::error("Failed to open file @ " + path.string() + " with error: " + e.what());
                                continue;
                            }
                            // I'm not sure why, but the threads will throw sometimes(pretty rarely)
                            // I will disable it for now, but I will try to fix this later
                            //                            threads.emplace_back(recompile_task, shader);

                            // TODO(malatindez):
                            // fix multithreading

                            recompile_task(shader);
                    }
                    // jthreads will automatically be joined as the vector goes out of scope
                }
                std::string tmp = std::to_string(timer.elapsed() * 1000);
                if (size_t dot = tmp.find_last_of('.');
                    tmp.size() - dot > 5)
                {
                    tmp = tmp.substr(0, dot + 5);
                }
                spdlog::info("Total time shader recompilation took: " + tmp + " ms");
            }
        }
    }
}