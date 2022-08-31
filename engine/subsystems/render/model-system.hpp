#pragma once
#include "detail/opaque-render-system.hpp"
#include "components/components.hpp"
#include "core/math/ray.hpp"
#include "render/model.hpp"
namespace engine::components
{
    struct ModelInstanceComponent
    {
        uint64_t kInstanceId;
    };
}
namespace engine::core
{
    class Engine;
    class ModelLoader;
}
namespace engine::render
{
    class ModelSystem;

    struct MeshInstance
    {
        // mesh id within the model vector
        uint32_t mesh_id = std::numeric_limits<uint32_t>::max();
        // material id within the model vector
        uint32_t material_id = std::numeric_limits<uint32_t>::max();
    };
    struct ModelInstance
    {
        // Id of this instance
        const uint64_t kInstanceId = std::numeric_limits<uint64_t>::max();
        uint32_t model_id = std::numeric_limits<uint32_t>::max();
        std::vector<MeshInstance> mesh_instances;
        ModelInstance(uint64_t kInstanceId) : kInstanceId(kInstanceId) {}
    };
    class ModelSystem final
    {
    public:
        ~ModelSystem() = default;

        static std::optional<entt::entity> FindIntersection(entt::registry &registry,
                                                            engine::core::math::Ray const &ray,
                                                            engine::core::math::Intersection &nearest);

        void Render() { opaque_render_system_.Render(); }

        void OnInstancesUpdated(entt::registry &registry)
        {
            opaque_render_system_.OnInstancesUpdated(registry);
        }
        static inline ModelInstance &CreateEmptyModelInstance()
        {
            return instance_->model_instances_.emplace_back(ModelInstance(instance_->model_instances_.size()));
        }
        static inline ModelInstance &GetModelInstance(uint64_t kInstanceId)
        {
            return instance_->model_instances_[kInstanceId];
        }
        static inline ModelInstance &GetLoadedModelInstance(uint32_t model_id)
        {
            return GetModelInstance(instance_->loaded_model_instance_ids_[model_id]);
        }
        static inline uint64_t GetLoadedModelInstanceId(uint32_t model_id)
        {
            return instance_->loaded_model_instance_ids_[model_id];
        }
    public:
        [[nodiscard]] static std::shared_ptr<ModelSystem> instance() noexcept { return instance_; }
        [[nodiscard]] static Model &GetModel(uint32_t model_id) { return instance_->models_[model_id]; }

    public:
        static uint32_t AddModel(Model &&model);
    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<ModelSystem>(new ModelSystem());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        ModelSystem() = default;

        // delete move & copy semantics
        ModelSystem(ModelSystem &&) = delete;
        ModelSystem(ModelSystem const &) = delete;
        ModelSystem &operator=(ModelSystem &&) = delete;
        ModelSystem &operator=(ModelSystem const &) = delete;

    private:
        _detail::OpaqueRenderSystem opaque_render_system_;

    private:
        static std::shared_ptr<ModelSystem> instance_;

    private:
        std::vector<Model> models_;
        std::unordered_map<uint32_t, uint64_t> loaded_model_instance_ids_;
        std::vector<ModelInstance> model_instances_;
    };
} // namespace engine::render