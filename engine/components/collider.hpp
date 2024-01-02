#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"
namespace lighten::components
{
    /**
     * @brief Collider component
     *
     * @warning This class is unused for now
     * @todo Implement colliders
     *
     */
    LIGHTEN_COMPONENT(name = "Collider", category = "Physics")
    struct BoxCollider
    {
        /// half extents of the box collider
        LIGHTEN_PROPERTY(serialize, save_game, name = "Half Extents")
        glm::vec3 half_extents;

    private:
        // aligning to 4 bytes
        float padding;
    };

    /**
     * @brief Collider component
     *
     * @warning This class is unused for now
     * @todo Implement colliders
     *
     */
    LIGHTEN_COMPONENT(name = "Collider", category = "Physics")
    struct SphereCollider
    {
        /// radius of the sphere collider
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius")
        float radius;
    };
} // namespace lighten::components