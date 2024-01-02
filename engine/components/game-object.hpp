#pragma once

#include "../core/reflection-macro.hpp"
#include "core/math.hpp"
#include "include/entt.hpp"

namespace lighten::components
{
    /**
     * @brief Component that contains the name of the object and its tag
     * @note This component is used to identify the object in the scene and to find it's descendants
     */
    LIGHTEN_COMPONENT(name = "Game Object", category = "Core")
    struct GameObject
    {
        LIGHTEN_PROPERTY(serialize, name = "Object Name")
        std::string name = "None";

        LIGHTEN_PROPERTY(serialize, name = "Object Tag")
        std::string tag;

        LIGHTEN_PROPERTY(serialize, name = "Parent")
        entt::entity parent = entt::null;

        LIGHTEN_PROPERTY(serialize, name = "Children")
        std::vector<entt::entity> children;
    };
} // namespace lighten::components