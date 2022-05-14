#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace engine::math;

Controller::Controller(BitmapWindow &window, std::shared_ptr<Scene> scene) : scene_(scene), window_(window) {}
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
            scene_->Draw(window_);
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
            if (event.type() == EventType::MouseButtonPressed)
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

void Controller::Tick(float delta_time) const
{
    if (!input_.rbutton_down())
    {
        vec3 vec{0};
        if (input_.key_state('W'))
        {
            vec += kUp;
        }
        if (input_.key_state('A'))
        {
            vec += kLeft;
        }
        if (input_.key_state('S'))
        {
            vec += kDown;
        }
        if (input_.key_state('D'))
        {
            vec += kRight;
        }
        scene_->sphere.SetCenter(scene_->sphere.center() + vec * delta_time);
        
        // update the scene if position of the sphere has changed
        if (squared_length(vec) != 0)
        {
            scene_->UpdateScene();
        }
    }
    else
    {
        auto middle = window_.position() + window_.size() / 2;

        POINT point;
        GetCursorPos(&point);

        vec2 diff = vec2{ static_cast<float>(point.x) - static_cast<float>(middle.x),
                         static_cast<float>(middle.y) - static_cast<float>(point.y) } /
            length(window_.size());

        scene_->sphere.SetCenter(scene_->sphere.center() + vec3{diff.x, diff.y, 0});

        SetCursorPos(middle.x, middle.y);
        scene_->UpdateScene();
    }
}