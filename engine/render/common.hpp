#pragma once
#include "include/library-pch.hpp"
namespace engine::render
{
    struct PerFrame
    {
        engine::core::math::mat4 view_projection;
    };
}