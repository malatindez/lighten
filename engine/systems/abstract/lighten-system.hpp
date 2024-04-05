#pragma once
#include "include/library-pch.hpp"
#include "core/layers/layer-stack.hpp"

namespace lighten::systems
{
    struct System : public core::Layer
    {
        System(std::string_view const name) : core::Layer(name) {}
        virtual ~System() = default;
        
        // Called after every system was constructed
        // allocated_systems is a vector of all systems that were constructed
        // It's order is the same as the order in reflection::SystemTuple
        virtual void Init(std::vector<System*> &allocated_systems) = 0;
    };
    struct SystemStack : public core::LayerStack
    {
        System(std::string_view const name) : core::LayerStack(name) {}
        virtual ~System() = default;
        
        // Called after every system was constructed
        // allocated_systems is a vector of all systems that were constructed
        // It's order is the same as the order in reflection::SystemTuple
        virtual void Init() = 0;
    };
}