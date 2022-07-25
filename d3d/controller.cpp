#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;

Controller::Controller(std::shared_ptr<engine::core::Window> window) : window_(window)
{
}

void Controller::OnEvent(Event &event)
{
    if (event.in_category(EventCategoryApplication))
    {
        if (event.type() == EventType::AppUpdate)
        {
            window_->PeekOSMessages();
        }
        else if (event.type() == EventType::AppTick)
        {
            auto const &ate = static_cast<AppTickEvent &>(event);
            Tick(ate.delta_time());
            time_from_start_ += ate.delta_time();
            for (auto const &func : update_callbacks_)
            {
                func(ate.delta_time());
            }
        }
        else if (event.type() == EventType::AppRender)
        {
        }
        else if (event.type() == EventType::WindowClose)
        {
            Application::Exit();
        }
    }
}

void Controller::Tick(float delta_time)
{
}