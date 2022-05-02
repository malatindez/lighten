#include "engine.hpp"
#include <iostream>
namespace engine
{
    std::unique_ptr<Engine> Engine::engine_;

    using namespace engine::math;

    INT Engine::Start(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                      int cmd_show)
    {
        if (engine_)
        {
            throw std::runtime_error{
                "Start function cannot be called more than one time."};
        }
        WNDCLASSEXW wc;

        // clear out the window class for use
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        // fill in the struct with the needed information
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

        // create the window and use the result as the handle
        auto window = std::make_unique<BitmapWindow>(
            wc, NULL,
            L"WindowClass1",                  // name of the window class
            L"Our First Windowed Program",    // title of the window
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
            kWindowPosition,                  // position of the window
            kWindowResolution,                // window size
            nullptr,                          // we have no parent window, nullptr
            nullptr,                          // we aren't using menus, nullptr
            instance,                         // application handle
            nullptr);                         // used with multiple windows, nullptr

        engine_ = std::unique_ptr<Engine>(new Engine{
            instance, prev_instance, cmd_line, cmd_show, std::move(window)});
        return engine_->Join();
    }

    LRESULT Engine::OnDestroy(Window &, HWND, UINT, WPARAM, LPARAM)
    {
        PostQuitMessage(0);
        return 0;
    }

    LRESULT Engine::OnPaint(Window &, HWND handle, UINT message, WPARAM w_param,
                            LPARAM l_param)
    {
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    LRESULT Engine::OnExitSizeMove(Window &window, HWND handle, UINT message,
                                   WPARAM w_param, LPARAM l_param)
    {
        Draw();
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    void MoveSphere(Sphere &sphere, vec3 const &direction, float delta_time)
    {
        sphere.SetCenter(sphere.center() + direction * delta_time);
    }

    const vec3 kUp{0, 1, 0};
    const vec3 kDown{0, -1, 0};
    const vec3 kLeft{-1, 0, 0};
    const vec3 kRight{1, 0, 0};

    LRESULT Engine::OnKeyDown(Window &, HWND handle, UINT message, WPARAM w_param,
                              LPARAM l_param)
    {
        // skip repeats
        if ((l_param & (1 << 30)) == 0)
        {
            if (w_param == 'W')
            {
                sphere_moving_direction_ += kUp;
            }
            else if (w_param == 'A')
            {
                sphere_moving_direction_ += kLeft;
            }
            else if (w_param == 'S')
            {
                sphere_moving_direction_ += kDown;
            }
            else if (w_param == 'D')
            {
                sphere_moving_direction_ += kRight;
            }
        }
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    LRESULT Engine::OnKeyRelease(Window &, HWND handle, UINT message,
                                 WPARAM w_param, LPARAM l_param)
    {
        if (w_param == 'W')
        {
            sphere_moving_direction_ -= kUp;
        }
        else if (w_param == 'A')
        {
            sphere_moving_direction_ -= kLeft;
        }
        else if (w_param == 'S')
        {
            sphere_moving_direction_ -= kDown;
        }
        else if (w_param == 'D')
        {
            sphere_moving_direction_ -= kRight;
        }

        return DefWindowProcW(handle, message, w_param, l_param);
    }
    LRESULT Engine::OnRButtonDown(Window &, HWND handle, UINT message,
                                  WPARAM w_param, LPARAM l_param)
    {
        rbuttondown_ = true;

        ShowCursor(false);

        auto new_pos = window_->position() + window_->size() / 2;
        SetCursorPos(new_pos.x, new_pos.y);

        return DefWindowProcW(handle, message, w_param, l_param);
    }
    LRESULT Engine::OnRButtonUp(Window &, HWND handle, UINT message, WPARAM w_param,
                                LPARAM l_param)
    {
        ShowCursor(true);

        rbuttondown_ = false;

        return DefWindowProcW(handle, message, w_param, l_param);
    }
    void Engine::MainLoop(Window &window)
    {
        using namespace std::chrono;
        const time_point<system_clock> now = system_clock::now();
        delta_time_ =
            float(duration_cast<microseconds>(now - last_time_point_).count()) / 1e6f;

        last_time_point_ = now;

        if (!rbuttondown_)
        {
            if (sphere_moving_direction_.length() != 0)
            {
                MoveSphere(sphere_, sphere_moving_direction_, delta_time_);
                Draw();
            }
        }
        else
        {
            auto middle = window_->position() + window_->size() / 2;

            POINT point;
            GetCursorPos(&point);

            vec2 diff = vec2{float(point.x) - float(middle.x),
                             float(middle.y) - float(point.y)} /
                        window_->size().length();

            MoveSphere(sphere_, {diff.x, diff.y, 0}, 1);

            SetCursorPos(middle.x, middle.y);

            Draw();
        }
    }

    vec3 color(Sphere sphere, Ray const &r)
    {
        if (sphere.Hit(r))
        {
            return vec3{1, 0, 0};
        }
        vec3 unit_direction = r.direction().unit_vector();

        float t = 0.5f * (unit_direction.y + 1);

        return (1.0f - t) * vec3{1} + t * vec3{0.5f, 0.7f, 1.0f};
    }

    void Engine::Draw()
    {
        vec3 lower_left_corner{-2, -1, -1};
        vec3 horizontal{4, 0, 0};
        vec3 vertical{0, 2, 0};
        vec3 origin{0};
        ivec2 window_size = window_->size();
        uint32_t *bitmap = window_->bitmap().data();
        for (int j = window_size.y - 1; j >= 0; j -= kResolutionScaling)
        {
            for (int i = 0; i < window_size.x; i += kResolutionScaling)
            {
                float u = float(i) / float(window_size.x);
                float v = float(j) / float(window_size.y);

                Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
                vec3 col = color(sphere_, r);

                auto ir = int(255.99 * col.r()) << 16;
                auto ig = int(255.99 * col.g()) << 8;
                auto ib = int(255.99 * col.b());

                for (int k = 0; i + k < window_size.x && k < kResolutionScaling; k++)
                {
                    bitmap[size_t(j) * window_size.x + i + k] = ir | ig | ib;
                }
            }
            for (int i = 1; i < kResolutionScaling; i++)
            {
                if (j - i >= 0)
                {
                    memcpy(bitmap + size_t(j - i) * window_size.x,
                           bitmap + size_t(j) * window_size.x,
                           window_size.x * sizeof(uint32_t));
                }
            }
        }
    }

    Engine::Engine(HINSTANCE instance, HINSTANCE prev_instance, PWSTR, int cmd_show,
                   std::unique_ptr<BitmapWindow> window)
        : window_(std::move(window)), instance_(instance),
          prev_instance_(prev_instance), sphere_{kSphereCoords, kSphereRadius}
    {
        // display the window on the screen
        ShowWindow(window_->handle(), cmd_show);

        Draw();
        // register callbacks
        window_->SetCallback(WM_DESTROY, std::bind_front(&Engine::OnDestroy, this));
        window_->SetCallback(WM_PAINT, std::bind_front(&Engine::OnPaint, this));
        window_->SetCallback(WM_EXITSIZEMOVE,
                             std::bind_front(&Engine::OnExitSizeMove, this));
        window_->SetCallback(WM_KEYDOWN, std::bind_front(&Engine::OnKeyDown, this));
        window_->SetCallback(WM_KEYUP, std::bind_front(&Engine::OnKeyRelease, this));
        window_->SetCallback(WM_RBUTTONDOWN,
                             std::bind_front(&Engine::OnRButtonDown, this));
        window_->SetCallback(WM_RBUTTONUP,
                             std::bind_front(&Engine::OnRButtonUp, this));
        window_->SetMainLoopCallback(std::bind_front(&Engine::MainLoop, this));
    }

} // namespace engine