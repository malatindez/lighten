#pragma once
#include "../core/reflection-macro.hpp"
#include "../include/library-pch.hpp"
namespace engine::components
{
    /**
     * @brief Component that contains the name of the object and its tag
     * @note This component is used to identify the object in the scene and to find it's descendants
     */
    LIGHTEN_COMPONENT(name="Local Transform", category="Core")
    struct Transform
    {
        /// @brief Object position in the local space
        LIGHTEN_PROPERTY(serialize, save_game, name="Position")
        core::math::vec3 position;
        /// @brief Object scale in the local space
        LIGHTEN_PROPERTY(serialize, save_game, name="Scale")
        core::math::vec3 scale;
        /// @brief Object rotation in the local space
        LIGHTEN_PROPERTY(serialize, save_game, name="Rotation")
        core::math::quat rotation;

        LIGHTEN_METHOD(name="Reset", button, mutator)
        void reset() noexcept
        {
            position = core::math::vec3{ 0 };
            scale = core::math::vec3{ 1 };
            rotation = core::math::quat{ 1, 0, 0, 0 };
            UpdateMatrices();
        }
        LIGHTEN_METHOD(name="Reset Position", button, mutator)
        void reset_position() noexcept 
        { 
            position = core::math::vec3{ 0 }; 
        }
        LIGHTEN_METHOD(name="Reset Scale", button, mutator)
        void reset_scale() noexcept 
        { 
            scale = core::math::vec3{ 1 }; 
        }
        LIGHTEN_METHOD(name="Reset Rotation", button, mutator)
        void reset_rotation() noexcept 
        {
            rotation = core::math::quat{ 1, 0, 0, 0 }; 
        }

        /// @brief Model matrix of the object, basically it's the product of the translation, rotation and scale matrices in LocalTransform.
        LIGHTEN_PROPERTY(serialize, name = "Model Matrix")
            core::math::mat4 model;

        /// @brief Inverse of the model matrix
        LIGHTEN_PROPERTY(serialize, name = "Inverse Model Matrix")
            core::math::mat4 inv_model;

        /**
         * @brief World matrix, it is equal to the model matrix of the parent object multiplied by the model matrix of this object
         *
         * @note it is used to calculate the world position of the object
         *
         * @note it is updated in the TransformSystem
         *
         */
        LIGHTEN_PROPERTY(serialize, name="World Matrix")
        core::math::mat4 world;
        
        /// @brief Inverse of the model matrix
        LIGHTEN_PROPERTY(serialize, name="Inverse World Matrix")
        core::math::mat4 inv_world;
        
        /// @warning will be deprecated. Should be removed due to implementation of WorldTransform.
        /// 
        [[deprecated]] void UpdateMatrices()
        {
            model = core::math::mat4{ 1 };
            model = core::math::translate(model, position);
            model = model * glm::mat4_cast(rotation);
            model = core::math::scale(model, scale);
            inv_model = core::math::inverse(model);
        }
    };
    /**
     * @brief Component that contains the transform matrices
     * 
     * @note If you want to update member variables you should call 
     *       `registry.patch<Transform>(entity, [](auto &t){})` to notify the systems 
     *       that use the transform component about the changes in the component
     * @note The systems that want to capture the changes should initialize 
     *       entt::observer{registry, entt::collector.update<Transform>()} and call 
     *       observer.each([](auto entity, auto &component){}) to process the changes
     * 
     */
    LIGHTEN_COMPONENT(name="World Transform", category="Core", editor_readonly)
    struct WorldTransform
    {
        /**
         * @brief World matrix, it is equal to the model matrix of the parent object multiplied by the model matrix of this object
         *
         * @note it is used to calculate the world position of the object
         *
         * @note it is updated in the TransformSystem
         *
         */
        LIGHTEN_PROPERTY(serialize, name="World Matrix")
        core::math::mat4 world;
        
         /// @brief Model matrix of the object, basically it's the product of the translation, rotation and scale matrices in LocalTransform.
        LIGHTEN_PROPERTY(serialize, name="Model Matrix")
        core::math::mat4 model;
        
        /// @brief Inverse of the model matrix
        LIGHTEN_PROPERTY(serialize, name="Inverse Model Matrix")
        core::math::mat4 inv_model;
        
        /// @brief Inverse of the model matrix
        LIGHTEN_PROPERTY(serialize, name="Inverse World Matrix")
        core::math::mat4 inv_world;
    };
    
} // namespace engine::components