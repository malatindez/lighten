#include "engine.hpp"

#include "render.hpp"

namespace engine {
std::unique_ptr<Engine> Engine::engine_;

using namespace engine::math;
const math::ivec2 kWindowPosition{0};
const math::ivec2 kWindowResolution{1280, 720};

INT Engine::Start(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                  int cmd_show) {
  if (engine_) {
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

LRESULT Engine::OnDestroy(Window &, HWND, UINT, WPARAM, LPARAM) {
  PostQuitMessage(0);
  return 0;
}

LRESULT Engine::OnPaint(Window &, HWND handle, UINT message, WPARAM w_param,
                        LPARAM l_param) {
  return DefWindowProcW(handle, message, w_param, l_param);
}

LRESULT Engine::OnExitSizeMove(Window &window, HWND handle, UINT message,
                               WPARAM w_param, LPARAM l_param) {
  auto &bmwnd = dynamic_cast<BitmapWindow &>(window);
  draw(bmwnd.bitmap(), window.size());
  return DefWindowProcW(handle, message, w_param, l_param);
}

LRESULT Engine::OnKeyDown(Window &window, HWND handle, UINT message,
                          WPARAM w_param, LPARAM l_param) {
  switch (w_param) {
  case 'W':
    break;
  case 'A':
    break;
  case 'S':
    break;
  case 'D':
    break;
  }

  return DefWindowProcW(handle, message, w_param, l_param);
}
Engine::Engine(HINSTANCE instance, HINSTANCE prev_instance, PWSTR, int cmd_show,
               std::unique_ptr<BitmapWindow> window)
    : window_(std::move(window)), instance_(instance),
      prev_instance_(prev_instance) {
  // display the window on the screen
  ShowWindow(window_->handle(), cmd_show);
  draw(window_->bitmap(), window_->size());
  // register callbacks
  window_->SetCallback(WM_DESTROY, std::bind_front(&Engine::OnDestroy, this));
  window_->SetCallback(WM_PAINT, std::bind_front(&Engine::OnPaint, this));
  window_->SetCallback(WM_EXITSIZEMOVE,
                       std::bind_front(&Engine::OnExitSizeMove, this));
  window_->SetCallback(WM_KEYDOWN, std::bind_front(&Engine::OnKeyDown, this));
}

} // namespace engine