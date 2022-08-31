#pragma once
#include "render/model.hpp"
namespace engine::core
{
    class Engine;
    class ModelLoader final
    {
    public:
        // Returns id of the model stored in ModelSystem
        static std::optional<uint32_t> Load(std::filesystem::path const &path);

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
        std::unordered_map<size_t, uint32_t> models_;

        bool attached_ = false;
    };
}