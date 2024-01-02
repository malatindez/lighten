#pragma once
#include "render/model.hpp"
namespace lighten::render
{
    class ModelSystem;
}
namespace lighten::core
{
    class Engine;
    class ModelLoader final
    {
    public:
        using FilepathHash = size_t;
        struct ModelInfo
        {
            ModelInfo(std::filesystem::path const &path, ModelId model_id, std::string_view model_name)
                : path{path},
                  model_id{model_id},
                  model_name{model_name}
            {
            }
            const std::filesystem::path path;
            const ModelId model_id;
            const std::string model_name;
        };
        // Returns id of the model stored in ModelSystem
        static std::optional<ModelId> Load(std::filesystem::path const &path);

        [[nodiscard]] static std::unordered_map<FilepathHash, ModelInfo> const &loaded_models() { return instance_->loaded_models_; }
        [[nodiscard]] static std::optional<FilepathHash> get_hash_from_model_id(ModelId model_id)
        {
            auto it = instance_->models_inverse_.find(model_id);
            if (it != instance_->models_inverse_.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

    private:
        friend class ::lighten::core::Engine;

        static void Init()
        {
            mal_toolkit::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<ModelLoader>(new ModelLoader());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        friend class ::lighten::render::ModelSystem;
        // Helper function that is called from mmodel system if the model was deleted.
        static void UnloadModel(ModelId model_id)
        {
            auto filepath_hash = instance_->models_inverse_.find(model_id)->second;
            instance_->models_inverse_.erase(model_id);
            instance_->models_.erase(filepath_hash);
            instance_->loaded_models_.erase(filepath_hash);
        }
        ModelLoader() {}

        // delete move & copy semantics
        ModelLoader(ModelLoader &&) = delete;
        ModelLoader(ModelLoader const &) = delete;
        ModelLoader &operator=(ModelLoader &&) = delete;
        ModelLoader &operator=(ModelLoader const &) = delete;

    private:
        static std::shared_ptr<ModelLoader> instance_;

    private:
        std::unordered_map<FilepathHash, ModelId> models_;
        std::unordered_map<ModelId, FilepathHash> models_inverse_;
        std::unordered_map<FilepathHash, ModelInfo> loaded_models_;

        bool attached_ = false;
    };
}