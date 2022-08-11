#pragma once
#include "core/engine.hpp"

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
            engine::core::Engine::Exit();
        }
    }

private:
    std::vector<UpdateCallback> update_callbacks_;
    long double time_from_start_ = 0;
};