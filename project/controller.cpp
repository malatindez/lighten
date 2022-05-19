#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace engine::math;

Controller::Controller(BitmapWindow &window,
                       std::shared_ptr<Scene> scene,
                       CameraController cam) : scene_(scene), window_(window), camera_controller_(cam) {}
void Controller::OnEvent(engine::Event &event)
{
    if (event.in_category(EventCategoryApplication))
    {
        if (event.type() == EventType::AppUpdate)
        {
            window_.PeekOSMessages();
        }
        else if (event.type() == EventType::AppTick)
        {
            auto const &ape = dynamic_cast<AppTickEvent &>(event);
            Tick(ape.delta_time());
        }
        else if (event.type() == EventType::AppRender)
        {
            scene_->Draw(camera_controller_.camera(), window_);
        }
        else if (event.type() == EventType::WindowClose)
        {
            Application::Exit();
        }
    }
    else if (event.in_category(EventCategoryInput))
    {
        if (event.in_category(EventCategoryMouse))
        {
            if (event.type() == EventType::MouseButtonReleased)
            {
                ShowCursor(false);
            }
            else if (event.type() == EventType::MouseButtonPressed)
            {
                ShowCursor(true);
            }
        }
        input_.OnEvent(event);
    }
}
const vec3 kUp{0, 1, 0};
const vec3 kDown{0, -1, 0};
const vec3 kLeft{-1, 0, 0};
const vec3 kRight{1, 0, 0};
const vec3 kForward{0, 0, 1};
const vec3 kBackwards{0, 0, -1};

void Controller::Tick(float delta_time)
{
    math::vec3 offset{0,0,0};
    if(input_.key_state('W')) 
    { 
        offset += kForward; 
}
    if(input_.key_state('S')) { offset += kBackwards; }
    if(input_.key_state('A')) { offset += kLeft; }
    if(input_.key_state('D')) { offset += kRight; }
    if(input_.key_state(VK_CONTROL)) { offset += kDown; }
    if(input_.key_state(VK_SPACE)) { offset += kUp; }
    camera_controller_.AddRelativeOffset(offset * delta_time);
    math::vec3 angles{0,0,0};
    if (input_.key_state('Q')) { 
        angles.x += delta_time * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f;
    }
    if (input_.key_state('E')) { angles.x -= delta_time * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f; }

    camera_controller_.AddRelativeAngles(angles);
    camera_controller_.UpdateMatrices();
}