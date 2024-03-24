#include "input-layer.hpp"
#include <include/win.hpp>
#include <imgui.h>
// TODO EVENT SUBSCRIPTIONS
namespace lighten::core
{
    std::shared_ptr<InputLayer> InputLayer::instance_ = nullptr;
    using namespace events;
    void InputLayer::OnEvent(Event &event)
    {
        if (event.in_category(EventCategoryMouse))
        {
            if (event.type() == EventType::MouseButtonPressed)
            {
                auto const &mbpe = static_cast<MouseButtonPressedEvent &>(event);
                if (mbpe.mouse_button() == 0)
                {
                    key_states_[Key::KEY_LBUTTON] = true;
                }
                else if (mbpe.mouse_button() == 1)
                {
                    key_states_[Key::KEY_RBUTTON] = true;
                }
            }
            else if (event.type() == EventType::MouseButtonReleased)
            {
                auto const &mbre = static_cast<MouseButtonReleasedEvent &>(event);
                if (mbre.mouse_button() == 0)
                {
                    key_states_[Key::KEY_LBUTTON] = false;
                }
                else if (mbre.mouse_button() == 1)
                {
                    key_states_[Key::KEY_RBUTTON] = false;
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
    void InputLayer::AddUpdateKeyCallback(KeySeq const &sequence, OnUpdateKeyCallbackFn const &fn, bool on_hold)
    {
        for (auto &k : sequence)
        {
            if (!key_states_.contains(k))
            {
                key_states_[k] = 0;
            }
        }
        on_update_callbacks_[sequence] = std::tuple{on_hold, 0u, fn};
    }
    // if on_hold is true then the function will be called each tick/update that the KeySeq is being held
    void InputLayer::AddTickKeyCallback(KeySeq const &sequence, OnTickKeyCallbackFn const &fn, bool on_hold)
    {
        for (auto &k : sequence)
        {
            if (!key_states_.contains(k))
            {
                key_states_[k] = 0;
            }
        }
        on_tick_callbacks_[sequence] = std::tuple{on_hold, 0u, fn};
    }
    void InputLayer::OnTick(float dt)
    {
        for (auto &[key, value] : on_tick_callbacks_)
        {
            bool t = true;
            for (auto k : key)
            {
                t &= key_states_.at(k);
            }
            auto &[on_hold, count, func] = value;
            if (t)
            {
                if (on_hold || count == 0)
                {
                    func(dt, key, ++count);
                }
                continue;
            }
            if (count != 0)
            {
                func(dt, key, UINT32_MAX);
            }
            count = 0;
        }
    }
    void InputLayer::OnUpdate()
    {
        for (auto &[key, value] : on_update_callbacks_)
        {
            bool t = true;
            for (auto k : key)
            {
                t &= key_states_.at(k);
            }
            auto &[on_hold, count, func] = value;
            if (t)
            {
                if (on_hold || count == 0)
                {
                    func(key, ++count);
                }
                continue;
            }
            if (count != 0)
            {
                func(key, UINT32_MAX);
            }
            count = 0;
        }
    }
}