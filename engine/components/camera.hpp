#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"
namespace lighten::components
{
    /**
     * @brief Camera component
     */
    LIGHTEN_COMPONENT(name = "Camera", category = "Core")
    struct CameraComponent
    {
        /// @brief View matrix
        LIGHTEN_PROPERTY(name = "View Matrix", category = "debug_matrices")
        glm::mat4 view;

        /// @brief Projection matrix
        LIGHTEN_PROPERTY(name = "Projection Matrix", category = "debug_matrices")
        glm::mat4 projection{1.0};

        /// @brief View projection matrix
        LIGHTEN_PROPERTY(name = "View Projection Matrix", category = "debug_matrices")
        glm::mat4 view_projection{1.0};

        /// @brief Inverse of the view matrix
        LIGHTEN_PROPERTY(name = "Inverse View Matrix", category = "debug_matrices")
        glm::mat4 inv_view{1.0};

        /// @brief Inverse of the projection matrix
        LIGHTEN_PROPERTY(name = "Inverse Projection Matrix", category = "debug_matrices")
        glm::mat4 inv_projection{1.0};

        /// @brief Inverse of the view projection matrix
        LIGHTEN_PROPERTY(name = "Inverse View Projection Matrix", category = "debug_matrices")
        glm::mat4 inv_view_projection{1.0};

        LIGHTEN_PROPERTY_GETTER(serialize, name = "Field of View", category = "Camera Settings")
        [[nodiscard]] float get_fovy() const noexcept { return glm::radians(fovy_); }

        LIGHTEN_PROPERTY_SETTER(name = "Field of View", category = "Camera Settings")
        void set_fovy(float fovy) noexcept { fovy_ = glm::radians(fovy); }

        /// @brief Field of view in radians
        float fovy_ = glm::radians(45.0f);

        /// @brief near plane Z coordinate
        LIGHTEN_PROPERTY(serialize, name = "Near Plane", category = "Camera Settings")
        float z_near_ = 2000.0f;

        /// @brief far plane Z coordinate
        LIGHTEN_PROPERTY(serialize, name = "Far Plane", category = "Camera Settings")
        float z_far_ = 0.02f;

        /**
         * @brief Returns the right vector of the camera
         *
         * @return right vector of the camera
         */

        [[nodiscard]] constexpr glm::vec3 right() const noexcept
        {
            return glm::vec3{inv_view[0][0], inv_view[0][1], inv_view[0][2]};
        }
        /// @brief Returns the up vector of the camera
        /// @return up vector of the camera
        [[nodiscard]] constexpr glm::vec3 up() const noexcept
        {
            return glm::vec3{inv_view[1][0], inv_view[1][1], inv_view[1][2]};
        }
        /// @brief Returns the forward vector of the camera
        /// @return forward vector of the camera
        [[nodiscard]] constexpr glm::vec3 forward() const noexcept
        {
            return glm::vec3{inv_view[2][0], inv_view[2][1], inv_view[2][2]};
        }
        /// @brief Returns the position of the camera
        /// @return position of the camera
        [[nodiscard]] constexpr glm::vec3 position() const noexcept
        {
            return glm::vec3{inv_view[3][0], inv_view[3][1], inv_view[3][2]};
        }
    };
} // namespace lighten::components
