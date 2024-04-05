#pragma once
#include "include/library-pch.hpp"
#include "input-management-system.hpp"
#include "../core/reflection-macro.hpp"
namespace lighten::systems
{
    LIGHTEN_SYSTEM(name = "Viewport Management System", category = "Core", CHILD_OF = InputManagementSystem)
    class ViewportManagementSystem final : public core::Layer
    {
        
    };
}