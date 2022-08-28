#pragma once
#include "include/library-pch.hpp"
#include "core/math.hpp"
namespace engine::render
{
    struct PerFrame
    {
        core::math::mat4 view_projection;
    };
}