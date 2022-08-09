#pragma once
#include "core/math.hpp"
namespace engine::render
{
    class Material final
    {
    public:
        core::math::vec3 albedo { 1.0f };
        core::math::vec3 emission { 0.0f };
        core::math::vec3 F0 { 0.04f };
        float roughness = 1.0f;
        float metalness = 0.0f;
        bool casts_shadow = true;
    };
} // namespace engine::render