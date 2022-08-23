#pragma once
#include "layer.hpp"
#include "render/model.hpp"
#include "utils/utils.hpp"
namespace engine::core
{
    class Engine;
    class ModelLoader final
    {
    public:
        std::shared_ptr<render::Model> Load(std::filesystem::path const &path);
        [[nodiscard]] static std::shared_ptr<ModelLoader> instance() noexcept { return instance_; }

    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<ModelLoader>(new ModelLoader());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        ModelLoader() {}

        // delete move & copy semantics
        ModelLoader(ModelLoader &&) = delete;
        ModelLoader(ModelLoader const &) = delete;
        ModelLoader &operator=(ModelLoader &&) = delete;
        ModelLoader &operator=(ModelLoader const &) = delete;

    private:
        static std::shared_ptr<ModelLoader> instance_;

    private:
        std::unordered_map<size_t, std::shared_ptr<render::Model>> models_;

        bool attached_ = false;
    };
}