#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"
namespace lighten::components
{
    /// @brief Force field type
    /// @warning This class is unused for now
    /// @todo Implement force fields

    // Point force field is a sphere that applies a force to all the particles inside it
    // Basically works like this: applied_force += force * (1 - (distance / radius) ^ falloff)
    /**
     * @brief Point force field is a sphere that applies a force to all the particles inside of it
     * Works like this: \f$\mathrm{applied_force} \pluseq \mathrm{force} \cdot {{(1 - (\frac{\mathrm{distance}}{\mathrm{radius}})}^\mathrm{falloff}}\f$
     */
    LIGHTEN_COMPONENT(name = "Force Field", category = "Physics")
    struct PointForceField
    {
        /**
         * @brief Force field strength
         *
         * Negative -> repel, positive -> attract
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Force")
        float force = 0;
        /// @brief Radius of the force field
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius")
        float radius;
        /// @brief Force falloff
        ///
        /// This variable is used to calculate how fast the force decreases with distance
        LIGHTEN_PROPERTY(serialize, save_game, name = "Falloff")
        float falloff;
    };

    // Directional force field is a plane that applies a force to all the particles in front of it
    // Basically works like this: applied_force += direction * force * (1 - (dot(normal, particle_position) / radius) ^ falloff)
    // where direction = transform.forward

    /**
     * @brief Directional force field is a plane that applies a force to all the particles in front of it
     * Works like this: \f$\mathrm{applied_force} \pluseq \mathrm{direction} \cdot \mathrm{force} \cdot {{(1 - (\frac{\mathrm{dot}(\mathrm{normal}, \mathrm{particle_position})}{\mathrm{radius}})}^\mathrm{falloff}}\f$
     * `applied_force += direction * force * (1 - (dot(normal, particle_position) / radius) ^ falloff)`
     */
    LIGHTEN_COMPONENT(name = "Force Field", category = "Physics")
    struct DirectionalForceField
    {
        /**
         * @brief Force field strength
         *
         * Negative -> repel, positive -> attract
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Force")
        float force = 0;
        /// @brief Radius of the force field
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius")
        float radius;
        /// @brief Force falloff
        ///
        /// This variable is used to calculate how fast the force decreases with distance
        LIGHTEN_PROPERTY(serialize, save_game, name = "Falloff")
        float falloff;
    };

    // OmniDirectional force field is a sphere field that applies force inwards of this sphere from all dire
    // Can be used to simulate the air/water/whatever resistance
    // You can calculate approximate force for the field like this:
    // 0.5 * fluid_density * average_particle_velocity ^ 2 * average_particle_area * fluid_drag_coefficient
    // It is pretty costy to simulate the whole fluid dynamics, so this is a good approximation
    // Basically works like this: applied_force += -normalized(velocity.direction) * force

    /**
     * @brief OmniDirectional force field is a sphere field that applies force inwards of this sphere from all directions
     * Can be used to simulate the air/water/whatever resistance
     * You can calculate approximate force for the field like this:
     * \f$0.5 \cdot \mathrm{fluid_density} \cdot \mathrm{average_particle_velocity}^2 \cdot \mathrm{average_particle_area} \cdot \mathrm{fluid_drag_coefficient}\f$
     * It is pretty costy to simulate the whole fluid dynamics, so this is a good approximation
     * Works like this: \f$\mathrm{applied_force} \pluseq -\mathrm{normalized}(\mathrm{velocity.direction}) \cdot \mathrm{force}\f$
     */
    LIGHTEN_COMPONENT(name = "Force Field", category = "Physics")
    struct OmniDirectionalForceField
    {
        /**
         * @brief Force field strength
         *
         * Negative -> repel, positive -> attract
         */
        LIGHTEN_PROPERTY(serialize, save_game, name = "Force")
        float force = 0;
        /// @brief Radius of the force field
        LIGHTEN_PROPERTY(serialize, save_game, name = "Radius")
        float radius;
        /// @brief Force falloff
        ///
        /// This variable is used to calculate how fast the force decreases with distance
        LIGHTEN_PROPERTY(serialize, save_game, name = "Falloff")
        float falloff;
    };

} // namespace lighten::components