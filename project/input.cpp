#include "input.hpp"
using namespace engine;
using namespace core::events;
void Input::OnEvent(Event &event)
{
    if (event.in_category(EventCategoryMouse))
    {
        if (event.type() == EventType::MouseButtonPressed)
        {
            auto const &mbpe = dynamic_cast<MouseButtonPressedEvent&>(event);
            if(mbpe.mouse_button() == 0) 
            {
                lbuttonstate_ = true;
            } 
            else if(mbpe.mouse_button() == 1) 
            {
                rbuttonstate_ = true;
            }
        }
        else if (event.type() == EventType::MouseButtonReleased)
        {
            auto const& mbre = dynamic_cast<MouseButtonReleasedEvent&>(event);
            if(mbre.mouse_button() == 0) 
            {
                lbuttonstate_ = false;
            } 
            else if(mbre.mouse_button() == 1) 
            {
                rbuttonstate_ = false;
            }
        }
        else if (event.type() == EventType::MouseMoved)
        {
            auto const& mme = dynamic_cast<MouseMovedEvent&>(event);
            mouse_position_ = mme.coordinates();
        }
        event.handled = true;
    }
    else if (event.in_category(EventCategoryKeyboard))
    {
        if (event.type() == EventType::KeyPressed)
        {
            auto const& kpe = dynamic_cast<KeyPressedEvent &>(event);
            key_states_[kpe.key_code()] = true;
        }
        else if (event.type() == EventType::KeyReleased)
        {
            auto const& kre = dynamic_cast<KeyReleasedEvent &>(event);
            key_states_[kre.key_code()] = false;
        }
        event.handled = true;
    }
}