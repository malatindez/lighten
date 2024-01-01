#pragma once
#include "detail/opaque-render-system.hpp"
#include "detail/dissolution-render-system.hpp"
#include "detail/emissive-render-system.hpp"
#include "components/components.hpp"
#include "core/math.hpp"
#include "render/model.hpp"

namespace engine::core
{
    class Engine;
    class ModelLoader;
}
namespace engine::render
{
    /**
     * @brief ModelSystem class, responsible for loading and rendering models.
     * @details This class is responsible for loading and rendering models. It also provides a way to find intersections with models.
     * @todo Remove opaque, emissive, and dissolve render systems and move their functionality into ModelSystem. This will allow more flexibility in rendering. To do this, we need to remove Opaque, Emissive and Dissolve components and create general purpose component for model rendering. It should contain data so that we can easily find the instance and material data for a given entity: instance data, material data, material instance data, and shading type.
     */
    class ModelSystem final
    {
    public:
        ~ModelSystem() = default;

        static std::optional<entt::entity> FindIntersection(entt::registry &registry,
                                                            core::math::Ray const &ray,
                                                            core::MeshIntersection &nearest);
        static std::optional<entt::entity> FindIntersection(entt::registry &registry,
                                                            core::math::Ray const &ray,
                                                            core::math::Intersection &nearest);

    public:
        [[nodiscard]] static ModelSystem &instance() noexcept
        {
            mal_toolkit::Assert(instance_ != nullptr);
            return *instance_;
        }
        [[nodiscard]] static Model &GetModel(ModelId model_id) { return instance().models_.find(model_id)->second; }

        [[nodiscard]] static ModelId AddModel(Model &&model);
        static void UnloadModel(ModelId model_id);

        static ModelId GetUnitSphereFlat();
        static ModelId GetUnitSphereLowPoly();
        static ModelId GetUnitCube();

    private:
        friend class ::engine::core::Engine;
        friend class ::engine::core::ModelLoader;

        static void Init()
        {
            mal_toolkit::Assert(instance_ == nullptr);
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
        static std::shared_ptr<ModelSystem> instance_;

    private:
        std::unordered_map<ModelId, Model> models_;
        ModelId current_index = 0;
    };
} // namespace engine::render