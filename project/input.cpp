#include "input.hpp"
using namespace engine;
using namespace core::events;
void Input::OnEvent(Event &event)
{
    if (event.in_category(EventCategoryApplication))
    {
        if (event.type() == EventType::AppTick)
        {
            auto const &ate = static_cast<AppTickEvent &>(event);
            OnTick(ate.delta_time());
        }
        else if (event.type() == EventType::AppUpdate)
        {
            OnUpdate();
        }
    }
    else if (event.in_category(EventCategoryMouse))
    {
        if (event.type() == EventType::MouseButtonPressed)
        {
            auto const &mbpe = static_cast<MouseButtonPressedEvent &>(event);
            if (mbpe.mouse_button() == 0)
            {
                lbuttonstate_ = true;
            }
            else if (mbpe.mouse_button() == 1)
            {
                rbuttonstate_ = true;
            }
        }
        else if (event.type() == EventType::MouseButtonReleased)
        {
            auto const &mbre = static_cast<MouseButtonReleasedEvent &>(event);
            if (mbre.mouse_button() == 0)
            {
                lbuttonstate_ = false;
            }
            else if (mbre.mouse_button() == 1)
            {
                rbuttonstate_ = false;
            }
        }
        else if (event.type() == EventType::MouseMoved)
        {
            auto const &mme = static_cast<MouseMovedEvent &>(event);
            mouse_position_ = mme.coordinates();
        }
        else if (event.type() == EventType::MouseScrolled)
        {
            auto const &mse = static_cast<MouseScrollEvent &>(event);
            mouse_scrolled_ = mse.delta() != 0;
            scroll_delta_ = mse.delta();
        }
        event.handled = true;
    }
    else if (event.in_category(EventCategoryKeyboard))
    {
        if (event.type() == EventType::KeyPressed)
        {
            auto const &kpe = static_cast<KeyPressedEvent &>(event);
            key_states_[kpe.key_code()] = true;
        }
        else if (event.type() == EventType::KeyReleased)
        {
            auto const &kre = static_cast<KeyReleasedEvent &>(event);
            key_states_[kre.key_code()] = false;
        }
        event.handled = true;
    }
}
void Input::OnTick(float dt)
{
    for(auto &[key, value] : on_tick_callbacks_)
    {
        bool t = true;
        for(auto k : key)
        {
            t &= key_states_.at(k);
        }
        auto &[count, func] = value;
        if(t) {
            func(dt, key, ++count);
            continue;
        }
        count = 0;
    }
}
void Input::OnUpdate()
{
    for(auto &[key, value] : on_update_callbacks_)
    {
        bool t = true;
        for(auto k : key)
        {
            t &= key_states_.at(k);
        }
        auto &[count, func] = value;
        if(t) {
            func(key, ++count);
            continue;
        }
        count = 0;
    }
}