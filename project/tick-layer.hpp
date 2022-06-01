#pragma once
#include "components/material.hpp"
#include "components/mesh.hpp"
#include "components/plane.hpp"
#include "controller.hpp"
#include "core/application.hpp"
#include "misc/camera-controller.hpp"
class OnTickLayer : public engine::Layer
{
public:
    OnTickLayer() = default;

    void OnEvent(engine::Event &event) override
    {
        if (event.type() == engine::EventType::AppTick)
        {
            engine::AppTickEvent const &ate = dynamic_cast<engine::AppTickEvent &>(event);
            for (auto const &func : funcs)
            {
                func(ate.delta_time());
            }
        }
    }
    std::vector<std::function<void(float)>> funcs;
};
