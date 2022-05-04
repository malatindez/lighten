#include "controller.hpp"
#include "engine.hpp"
using namespace engine;
using namespace engine::math;
Controller::Controller(BitmapWindow &window, std::shared_ptr<Scene> scene) : scene_(scene), window_(window)
{
    window_.SetCallback(WM_KEYDOWN, std::bind_front(&Controller::OnKeyDown, this));
    window_.SetCallback(WM_KEYUP, std::bind_front(&Controller::OnKeyRelease, this));
    window_.SetCallback(WM_RBUTTONDOWN, std::bind_front(&Controller::OnRButtonDown, this));
    window_.SetCallback(WM_RBUTTONUP, std::bind_front(&Controller::OnRButtonUp, this));
    window_.SetCallback(WM_LBUTTONDOWN, std::bind_front(&Controller::OnLButtonDown, this));
    window_.SetCallback(WM_LBUTTONUP, std::bind_front(&Controller::OnLButtonUp, this));
    window_.SetCallback(WM_EXITSIZEMOVE, std::bind_front(&Controller::OnExitSizeMove, this));
    window_.SetCallback(WM_DESTROY, std::bind_front(&Controller::OnDestroy, this));
}
LRESULT Controller::OnKeyDown(Window &, HWND handle, UINT message,
                              WPARAM w_param, LPARAM l_param)
{
    key_states_[w_param] = true;
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnKeyRelease(Window &, HWND handle, UINT message,
                                 WPARAM w_param, LPARAM l_param)
{
    key_states_[w_param] = false;
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnRButtonDown(Window &, HWND handle, UINT message,
                                  WPARAM w_param, LPARAM l_param)
{
    ShowCursor(false);
    rbuttonstate_ = true;
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnRButtonUp(Window &, HWND handle, UINT message,
                                WPARAM w_param, LPARAM l_param)
{
    ShowCursor(true);
    rbuttonstate_ = false;
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnLButtonDown(Window &, HWND handle, UINT message,
                                  WPARAM w_param, LPARAM l_param)
{
    lbuttonstate_ = true;
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnLButtonUp(Window &, HWND handle, UINT message,
                                WPARAM w_param, LPARAM l_param)
{
    lbuttonstate_ = false;
    return DefWindowProcW(handle, message, w_param, l_param);
}

LRESULT Controller::OnExitSizeMove(Window &, HWND handle, UINT message,
                                   WPARAM w_param, LPARAM l_param) const
{
    scene_->UpdateScene();
    return DefWindowProcW(handle, message, w_param, l_param);
}
LRESULT Controller::OnDestroy(Window &, HWND handle, UINT message,
                              WPARAM w_param, LPARAM l_param) const
{
    GetEngine().Exit();
    return DefWindowProcW(handle, message, w_param, l_param);
}

const vec3 kUp{0, 1, 0};
const vec3 kDown{0, -1, 0};
const vec3 kLeft{-1, 0, 0};
const vec3 kRight{1, 0, 0};

bool Controller::Update(float delta_time)
{
    if (!rbuttonstate_)
    {

        vec3 vec{0};
        if (key_states_['W'])
        {
            vec += kUp;
        }
        if (key_states_['A'])
        {
            vec += kLeft;
        }
        if (key_states_['S'])
        {
            vec += kDown;
        }
        if (key_states_['D'])
        {
            vec += kRight;
        }

        scene_->sphere.SetCenter(scene_->sphere.center() + vec * delta_time);

        // update the scene if position of the sphere has changed
        if (vec.squared_length() != 0)
        {
            scene_->UpdateScene();
        }
    }
    else
    {
        auto middle = window_.position() + window_.size() / 2;

        POINT point;
        GetCursorPos(&point);

        vec2 diff = vec2{float(point.x) - float(middle.x),
                         float(middle.y) - float(point.y)} /
                    window_.size().length();

        scene_->sphere.SetCenter(scene_->sphere.center() + vec3{diff.x, diff.y, 0});

        SetCursorPos(middle.x, middle.y);

        scene_->UpdateScene();
    }

    scene_->Draw(window_);
    return true;
}