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
            Camera cam{};
            scene_->Draw(cam, window_);
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

}