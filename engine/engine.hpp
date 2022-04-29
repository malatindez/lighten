#pragma once
#include "math/sphere.hpp"
#include "misc/bitmap-window.hpp"
#include "pch.hpp"

#include <chrono>

namespace engine {

const math::ivec2 kWindowPosition{0};
const math::ivec2 kWindowResolution{1280, 720};
const math::vec3 kSphereCoords{0, 0, -1};
const float kSphereRadius{0.5f};
class Engine final {
public:
  INT Join() const {
    window_->StartMainLoop();
    return 0;
  }

  static INT Start(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                   int cmd_show);
  // Start should be called before retrieving an engine otherwise we will
  // dereference nullptr and get undefined behaviour
  [[nodiscard]] static inline Engine &engine() noexcept { return *engine_; }

private:
  LRESULT OnDestroy(Window &, HWND, UINT, WPARAM, LPARAM);
  LRESULT OnPaint(Window &, HWND handle, UINT message, WPARAM w_param,
                  LPARAM l_param);
  LRESULT OnExitSizeMove(Window &window, HWND handle, UINT message,
                         WPARAM w_param, LPARAM l_param);
  LRESULT OnKeyDown(Window &window, HWND handle, UINT message, WPARAM w_param,
                    LPARAM l_param);
  LRESULT OnKeyRelease(Window &window, HWND handle, UINT message,
                       WPARAM w_param, LPARAM l_param);
  LRESULT OnRButtonDown(Window &window, HWND handle, UINT message,
                        WPARAM w_param, LPARAM l_param);
  LRESULT OnRButtonUp(Window &window, HWND handle, UINT message, WPARAM w_param,
                      LPARAM l_param);
  void MainLoop(Window &window);

  void Draw();

  Engine(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
         int cmd_show, std::unique_ptr<BitmapWindow> window);
  // delete move & copy semantics
  Engine(Engine &&Engine) = delete;
  Engine(Engine const &Engine) = delete;
  Engine &operator=(Engine &&Engine) = delete;
  Engine &operator=(Engine const &Engine) = delete;

  std::chrono::time_point<std::chrono::system_clock> last_time_point_;
  float delta_time_;
  std::unique_ptr<BitmapWindow> window_;
  HINSTANCE instance_;
  HINSTANCE prev_instance_;
  math::Sphere sphere_;
  math::ivec2 last_mouse_position_{0};
  math::vec3 sphere_moving_direction_{0};
  bool rbuttondown_{false}; // whenever RMB is pressed WASD movement is locked
  static std::unique_ptr<Engine> engine_;
};
[[nodiscard]] inline Engine &GetEngine() { return Engine::engine(); }
} // namespace engine