#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace engine::math;


Controller::Controller(BitmapWindow &window,
                       std::shared_ptr<Scene> scene,
                       CameraController cam) : scene_(scene), window_(window), camera_controller_(cam),
                                               executor{std::max(1u, std::max(ParallelExecutor::kMaxThreads - 4u, ParallelExecutor::kHalfThreads))} {}
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
            scene_->Draw(camera_controller_.camera(), window_, executor);
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
            auto const &mbpe = dynamic_cast<MouseButtonPressedEvent &>(event);
            if (mbpe.mouse_button() == 1)
            {
                Ray ray = PixelRaycast(math::vec2{mbpe.coordinates()});
                math::Intersection intersection;
                intersection.reset();
                std::optional<entt::entity> entity = scene_->GetIntersectedEntity(intersection, ray);
                if (entity.has_value())
                {
                    selected_object = &scene_->registry.get<components::Transform>(entity.value());
                    rb_saved_mouse_position_ = mbpe.coordinates();
                }
                else
                {
                    selected_object = nullptr;
                }
            }
            else if (mbpe.mouse_button() == 0)
            {
                lb_saved_mouse_position_ = mbpe.coordinates();
            }
        }
        else if (event.type() == EventType::MouseButtonReleased)
        {
            auto const &mbre = dynamic_cast<MouseButtonReleasedEvent &>(event);
        }
    }
}
const vec3 kUp{0, 1, 0};
const vec3 kDown{0, -1, 0};
const vec3 kLeft{-1, 0, 0};
const vec3 kRight{1, 0, 0};
const vec3 kForward{0, 0, 1};
const vec3 kBackwards{0, 0, -1};

const float kRollSpeed = engine::math::radians(180.0f);
const float kMoveSpeed = 2.0f;

#include <numbers>
void Controller::Tick(float delta_time)
{
    math::vec3 offset{0, 0, 0};
    float roll = 0;
    float pitch = 0;
    float yaw = 0;

    if (input_.key_state('W'))
    {
        offset += kForward;
    }
    if (input_.key_state('S'))
    {
        offset += kBackwards;
    }
    if (input_.key_state('A'))
    {
        offset += kLeft;
    }
    if (input_.key_state('D'))
    {
        offset += kRight;
    }
    if (input_.key_state(VK_CONTROL))
    {
        offset += kDown;
    }
    if (input_.key_state(VK_SPACE))
    {
        offset += kUp;
    }

    // 180 degrees per second
    if (input_.key_state('Q'))
    {
        roll -= delta_time * kRollSpeed;
    }
    if (input_.key_state('E'))
    {
        roll += delta_time * kRollSpeed;
    }
    if (input_.lbutton_down())
    {
        yaw = delta_time * float(lb_saved_mouse_position_.x - input_.mouse_position().x) / window_.window_size().x * engine::math::radians(90.0f);
        pitch = delta_time * float(lb_saved_mouse_position_.y - input_.mouse_position().y) / window_.window_size().y * engine::math::radians(90.0f);
    }
    if (input_.rbutton_down() && selected_object)
    {
        // find a new location for the object by casting ray
        Ray const a = PixelRaycast(math::vec2{input_.mouse_position()});
        Ray const b = PixelRaycast(math::vec2{rb_saved_mouse_position_});
        rb_saved_mouse_position_ = input_.mouse_position();

        float const distance = math::length(selected_object->position - camera_controller_.position());
        
        selected_object->position = b.PointAtParameter(distance);
        selected_object->UpdateMatrices();
    }
    if (!(roll == 0 && pitch == 0 && yaw == 0))
    {
        camera_controller_.AddRelativeAngles(roll, pitch, yaw);
    }
    if (length(offset) != 0)
    {
        camera_controller_.AddRelativeOffset(kMoveSpeed * offset * delta_time);
    }
    camera_controller_.UpdateMatrices();
}