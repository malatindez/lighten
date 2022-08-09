#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;

Controller::Controller(std::shared_ptr<engine::core::Window> window) : window_(window)
{}

void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
}