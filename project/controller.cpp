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
        input_.OnEvent(event);
        if (!event.in_category(EventCategoryMouse))
        {
            return;
        }
        if (event.type() == EventType::MouseButtonPressed)
        {
            auto const& mbpe = dynamic_cast<MouseButtonPressedEvent&>(event);
            if (mbpe.mouse_button() != 0)
            {
                return;
            }
            saved_mouse_position_ = mbpe.coordinates();
        }
        else if (event.type() == EventType::MouseButtonReleased)
        {
            auto const& mbre = dynamic_cast<MouseButtonReleasedEvent&>(event);
            if (mbre.mouse_button() != 0)
            {
                return;
            }
        }
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
    if(input_.key_state('W')) { offset += kForward; }
    if(input_.key_state('S')) { offset += kBackwards; }
    if(input_.key_state('A')) { offset += kLeft; }
    if(input_.key_state('D')) { offset += kRight; }
    if(input_.key_state(VK_CONTROL)) { offset += kDown; }
    if(input_.key_state(VK_SPACE)) { offset += kUp; }
    camera_controller_.AddRelativeOffset(offset * delta_time);
    math::vec3 angles{0,0,0};
    if (input_.key_state('Q')) { angles.z -= 10.0f * delta_time * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f; }
    if (input_.key_state('E')) { angles.z += 10.0f * delta_time * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f; }
    // 0.001 degree per pixel
    if (input_.lbutton_down()) {
        angles.x = float(saved_mouse_position_.x - input_.mouse_position().x) / 1000.0f * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f;
        angles.y = float(input_.mouse_position().y - saved_mouse_position_.y) / 1000.0f * 2.0f * static_cast<float>(std::numbers::pi) / 180.0f;
    }
    camera_controller_.AddRelativeAngles(angles);
    camera_controller_.UpdateMatrices();
    std::stringstream s;
    s << camera_controller_.camera().inv_view << std::endl;
    s << camera_controller_.camera().view << std::endl;
    s << camera_controller_.camera().inv_view_projection << std::endl;
    s << camera_controller_.camera().view_projection << std::endl;
    OutputDebugStringA(s.str().c_str());
}