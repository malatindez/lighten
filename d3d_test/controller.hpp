#pragma once
#include "core/layers/layer.hpp"
#include "core/parallel-executor.hpp"
#include "core/window.hpp"
#include "platform/windows/window.hpp"
#include <functional>
#include <memory>
#include <vector>
#include "core/application.hpp"

class Controller : public engine::core::Layer
{
public:
    using UpdateCallback = std::function<void(float)>;
    using engine::core::Layer::Layer;

    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }
    void OnTick(float delta_time) override;
    void OnEvent(engine::core::events::Event &e)
    {
        if (e.type() == engine::core::events::EventType::WindowClose) [[unlikely]]
        {
            engine::core::Application::Exit();
        }
    }

private:
    std::vector<UpdateCallback> update_callbacks_;
    long double time_from_start_ = 0;
};