#pragma once
#include "include/library-pch.hpp"
#include "../core/reflection-macro.hpp"
namespace lighten::systems
{
    LIGHTEN_SYSTEM(name = "Input Management System", category = "Core")
    class InputManagementSystem final : public core::Layer
    {

    };
}