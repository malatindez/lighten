#include "controller.hpp"
#include "core/application.hpp"
using namespace engine;
using namespace engine::math;
using namespace engine::components;

namespace
{
    const vec3 kUp{0, 1, 0};
    const vec3 kDown{0, -1, 0};
    const vec3 kLeft{-1, 0, 0};
    const vec3 kRight{1, 0, 0};
    const vec3 kForward{0, 0, 1};
    const vec3 kBackwards{0, 0, -1};
    render::Material &UpdateMaterial(render::Material &mat,
                             vec3 albedo = vec3{0.3f},
                             vec3 emission = vec3{0.0f},
                             float specular = 1,
                             float glossiness = 8,
                             bool casts_shadow = true)
    {
        mat.albedo = albedo;
        mat.emission = emission;
        mat.specular = specular;
        mat.glossiness = glossiness;
        mat.casts_shadow = casts_shadow;
        return mat;
    }

    Transform &UpdateTransform(entt::registry &registry, entt::entity e,
                               vec3 position = vec3{0},
                               vec3 scale = vec3{1},
                               quat rotation = quat{1, 0, 0, 0})
    {
        Transform &t = registry.get_or_emplace<Transform>(e);
        t.position = position;
        t.scale = scale;
        t.rotation = rotation;
        t.UpdateMatrices();
        return t;
    }
    MeshComponent &AddCubeComponent(entt::registry &registry, entt::entity e)
    {
        return registry.emplace<MeshComponent>(e, engine::render::kCubeMesh);
    }
    Sphere &AddSphereComponent(entt::registry &registry, entt::entity e)
    {
        return registry.emplace<Sphere>(e);
    }
    PointLight &AddPointLight(entt::registry &registry, entt::entity e, vec3 color = vec3{1, 1, 1}, float R = 5)
    {
        PointLight &point_light = registry.get_or_emplace<PointLight>(e);
        point_light.color = color;
        point_light.R = R;
        return point_light;
    }

    SpotLight &AddSpotLight(entt::registry &registry, entt::entity e,
                            vec3 color = vec3{1, 1, 1},
                            float cut_off = radians(45.0F),
                            float R = 1.0f,
                            vec3 direction = vec3{0, -1, 0})
    {
        SpotLight &spot = registry.get_or_emplace<SpotLight>(e);
        spot.direction = direction;
        spot.color = color;
        spot.cut_off = cut_off;
        spot.R = R;
        return spot;
    }

    DirectionalLight &AddDirLight(entt::registry &registry, entt::entity e,
                                  vec3 color = vec3{1, 1, 1},
                                  vec3 direction = vec3{0, -1, 0})
    {
        DirectionalLight &dir = registry.get_or_emplace<DirectionalLight>(e);
        dir.direction = direction;
        dir.color = color;
        return dir;
    }
}
void Controller::InitScene()
{
    UpdateMaterial(scene_->floor.material, vec3{0.3f}, vec3{0}, 0.25f, 4, false);
    scene_->floor.transform.reset();
    scene_->floor.update_plane(math::vec3{0, 0, 1}, math::vec3{1, 0, 0});
    scene_->floor.transform.position = math::vec3{0, -2, 0};
    scene_->floor.transform.UpdateMatrices();
    entt::registry &registry = scene_->registry;

    entt::entity directional_light = registry.create();

    DirectionalLight &directional_light_v = registry.emplace<DirectionalLight>(directional_light);
    directional_light_v.direction = math::normalize(math::vec3{0, -1, -1});
    directional_light_v.color = math::vec3{0.3f, 0.0f, 0.4f};
    /*
         tick_layer->funcs.push_back(
            [&time_from_start_, &directional_light_v](float delta_time)
            {
                directional_light_v.direction = math::normalize(math::vec3{
                    math::sin(time_from_start_),
                    math::cos(time_from_start_),
                    0 });
            });   */
    entt::entity sphere1 = registry.create();
    UpdateTransform(registry, sphere1, vec3{0}, vec3{0.5f});
    UpdateMaterial(AddSphereComponent(registry, sphere1).material, vec3{1.0f}, vec3{0}, 1, 2);

    entt::entity sphere2 = registry.create();
    Transform &sphere2_transform = UpdateTransform(registry, sphere2, vec3{1}, vec3{1.5f});
    AddPointLight(registry, sphere2, vec3{0.2f, 0.0f, 0.2f}, 3);
    UpdateMaterial(AddSphereComponent(registry, sphere2).material, vec3{0}, vec3{0.2f, 0.0f, 0.2f}, 1, 2, true);
    update_callbacks_.emplace_back( [&sphere2_transform, this](float dt) {sphere2_transform.scale = vec3{1.0f} + cos(time_from_start_ - 1.2f) / 2;});
    
    entt::entity cube1 = registry.create();
    UpdateTransform(registry, cube1, vec3{3}, vec3{1.0f});
    UpdateMaterial(AddCubeComponent(registry, cube1).material(), vec3{1.0f}, vec3{0}, 1, 2, true);

    entt::entity cube2 = registry.create();
    UpdateTransform(registry, cube2, vec3{5}, vec3{1.5f});
    AddPointLight(registry, cube2, vec3{0.5f, 0.0f, 0.5f}, 3);
    UpdateMaterial(AddCubeComponent(registry, cube2).material(), vec3{0}, vec3{0.5f, 0.0f, 0.5f}, 1, 2, false);

    entt::entity main_light = registry.create();
    Transform &main_light_transform = UpdateTransform(registry, main_light, vec3{3, 2.5f, -3}, vec3{0.5f});
    AddPointLight(registry, main_light, vec3{1.0f, 1.0f, 0.5f}, 10);
    UpdateMaterial(AddSphereComponent(registry, main_light).material, vec3{0}, vec3{1.0f, 1.0f, 0.5f}, 1, 4, false);

    entt::entity main_light_orbit = registry.create();
    Transform &main_light_orbit_transform = UpdateTransform(registry, main_light_orbit, vec3{0}, vec3{0});
    UpdateMaterial(AddSphereComponent(registry, main_light_orbit).material, vec3{0}, vec3{0.0f, 0.0f, 1.0f}, 1, 2, true);
    update_callbacks_.emplace_back(
        [&main_light_orbit_transform, &main_light_transform, this](float dt)
        {
            main_light_orbit_transform.position = main_light_transform.position + 2 * vec3{2 * cos(time_from_start_  / 2 - 0.5f), cos(time_from_start_ + 1.7f) * sin(time_from_start_ + 0.9f) * dt, 2 * sin(time_from_start_ / 2  + 0.5f)};
            main_light_orbit_transform.scale = math::vec3{0.25f + cos(time_from_start_ / 2) / 4};
            main_light_orbit_transform.UpdateMatrices();
        });
        
    entt::entity main_light_orbit_orbit = registry.create();
    Transform &main_light_orbit_orbit_transform = UpdateTransform(registry, main_light_orbit_orbit, vec3{0}, vec3{0});
    AddPointLight(registry, main_light_orbit_orbit, vec3{1.0f, 0.0f, 0.0f}, 2.5f);
    UpdateMaterial(AddSphereComponent(registry, main_light_orbit_orbit).material, vec3{0}, vec3{1.0f, 0.0f, 0.0f}, 1, 3);
    update_callbacks_.emplace_back(
        [&main_light_orbit_transform, &main_light_orbit_orbit_transform, this](float dt)
        {
            main_light_orbit_orbit_transform.position = main_light_orbit_transform.position + 4 * vec3{cos(time_from_start_), cos(time_from_start_ / 2) * sin(time_from_start_ / 2), sin(time_from_start_)};
            main_light_orbit_orbit_transform.scale =  math::vec3{0.125f + cos(time_from_start_ / 2) / 8};
            main_light_orbit_orbit_transform.UpdateMatrices();
        });
}

Controller::Controller(BitmapWindow &window,
                       std::shared_ptr<Scene> scene,
                       CameraController const &cam) : camera_controller_(cam), scene_(scene), window_(window) {}
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
            auto const &ate = dynamic_cast<AppTickEvent &>(event);
            Tick(ate.delta_time());
            time_from_start_ += ate.delta_time();
            for (auto const &func : update_callbacks_)
            {
                func(ate.delta_time());
            }
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
            auto const &mbpe = static_cast<MouseButtonPressedEvent &>(event);
            if (mbpe.mouse_button() == 1)
            {
                Ray ray = PixelRaycast(math::vec2{mbpe.coordinates()});
                math::Intersection intersection;
                intersection.reset();
                std::optional<entt::entity> entity = scene_->GetIntersectedEntity(intersection, ray);
                if (entity.has_value())
                {
                    selected_object = &scene_->registry.get<components::Transform>(entity.value());
                    selected_object_distance = math::length(selected_object->position - camera_controller_.position());
                    rb_saved_mouse_position_ = mbpe.coordinates();
                }
                else
                {
                    selected_object = nullptr;
                    selected_object_distance = 0;
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
        roll -= delta_time * roll_speed_;
    }
    if (input_.key_state('E'))
    {
        roll += delta_time * roll_speed_;
    }
    if (input_.lbutton_down())
    {
        vec2 t{lb_saved_mouse_position_ - input_.mouse_position()};
        t = t / window_.window_size();
        t *= sensivity_;
        yaw = delta_time * t.x;
        pitch = delta_time * t.y;
    }
    if (input_.rbutton_down() && selected_object)
    {
        Ray const b = PixelRaycast(math::vec2{rb_saved_mouse_position_});
        rb_saved_mouse_position_ = input_.mouse_position();

        selected_object->position = b.PointAtParameter(selected_object_distance);
        selected_object->UpdateMatrices();
    }
    if (!(roll == 0 && pitch == 0 && yaw == 0))
    {
        camera_controller_.AddRelativeAngles(roll, pitch, yaw);
    }
    if (length(offset) != 0)
    {
        camera_controller_.AddRelativeOffset(move_speed_ * offset * delta_time);
    }
    camera_controller_.UpdateMatrices();
}