#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"
namespace lighten::components
{
    /// @brief Point light component
    /// TODO: Add these features:
    /// * add casts_shadow boolean so that we can have lights that don't cast shadows
    /// * add shadow_bias
    /// * add shadow_normal_bias
    /// * add shadow_blur
    /// * add shadow_blur_radius
    /// * add shadow_blur_sigma
    LIGHTEN_COMPONENT(name = "Point Light", category = "Lights")
    struct PointLight
    {
        /// @brief Color of the light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Light color")
        glm::vec3 color;

        /// @brief Intensity of light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Intensity")
        float power;

        /// @brief If the light casts shadows
        LIGHTEN_PROPERTY(serialize, save_game, name = "Casts Shadows")
        bool casts_shadows = false;

        /// @brief Radius of the light
        /// @todo
        /// Use radius in LightRenderSystem and multiply it by the scale of the transform component
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius (TODO)")
        float radius;
    };

    /// @brief Spot light component
    LIGHTEN_COMPONENT(name = "Spot Light", category = "Lights")
    struct SpotLight
    {
        /// @brief Color of the light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Light color")
        glm::vec3 color;

        /// @brief Intensity of light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Intensity")
        float power;

        /// @brief If the light casts shadows
        LIGHTEN_PROPERTY(serialize, save_game, name = "Casts Shadows")
        bool casts_shadows = false;

        /// @brief Radius of the light
        /// @todo
        /// Use radius in LightRenderSystem and multiply it by the scale of the transform component
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius (TODO)")
        float radius;

        /// @brief inner cutoff angle in radians
        LIGHTEN_PROPERTY(serialize, save_game, name = "Inner cutoff")
        float inner_cutoff;
        /// @brief outer cutoff angle in radians
        LIGHTEN_PROPERTY(serialize, save_game, name = "Outer cutoff")
        float outer_cutoff;
    };

    /// @brief Directional light component
    LIGHTEN_COMPONENT(name = "Directional Light", category = "Lights")
    struct DirectionalLight
    {
        /// @brief Color of the light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Light color")
        glm::vec3 color;

        /// @brief Intensity of light
        LIGHTEN_PROPERTY(serialize, save_game, name = "Intensity")
        float power;

        /// @brief If the light casts shadows
        LIGHTEN_PROPERTY(serialize, save_game, name = "Casts Shadows")
        bool casts_shadows = false;

        /// @brief Solid angle of the light, it is used in the shaders to calculate the light intensity
        LIGHTEN_PROPERTY(serialize, save_game, name = "Solid angle")
        float solid_angle;
    };
} // namespace lighten::components