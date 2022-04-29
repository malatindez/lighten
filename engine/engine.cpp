#include "engine.hpp"
namespace engine {
std::unique_ptr<Engine> Engine::engine_;

using namespace engine::math;
const math::ivec2 kWindowPosition{0};
const math::ivec2 kWindowResolution{1280, 720};
bool hit_sphere(vec3 const &center, float radius, ray const &r) {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0f * dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  return discriminant > 0;
}

vec3 color(ray const &r) {
  if (hit_sphere(vec3{0, 0, -1}, 0.5f, r)) {
    return vec3{1, 0, 0};
  }
  vec3 unit_direction = r.direction().unit_vector();
  float t = 0.5f * (unit_direction.y() + 1);
  return (1.0f - t) * vec3{1} + t * vec3{0.5f, 0.7f, 1.0f};
}

void draw(std::vector<uint32_t> &bitmap, ivec2 window_size) {
  vec3 lower_left_corner{-2, -1, -1};
  vec3 horizontal{4, 0, 0};
  vec3 vertical{0, 2, 0};
  vec3 origin{0};
  for (int j = window_size.y() - 1; j >= 0; j--) {
    for (int i = 0; i < window_size.x(); i++) {
      float u = float(i) / float(window_size.x());
      float v = float(j) / float(window_size.y());
      ray r(origin, lower_left_corner + u * horizontal + v * vertical);
      vec3 col = color(r);
      auto ir = int(255.99 * col[0]) << 16;
      auto ig = int(255.99 * col[1]) << 8;
      auto ib = int(255.99 * col[2]);

      bitmap[size_t(j) * window_size.x() + i] = ir | ig | ib;
    }
  }
}

LRESULT OnDestroy(Window &, HWND, UINT, WPARAM, LPARAM) {
  PostQuitMessage(0);
  return 0;
}

LRESULT OnPaint(Window &, HWND handle, UINT message, WPARAM w_param,
                LPARAM l_param) {
  return DefWindowProcW(handle, message, w_param, l_param);
}

LRESULT OnExitSizeMove(Window &window, HWND handle, UINT message,
                       WPARAM w_param, LPARAM l_param) {
  auto &bmwnd = dynamic_cast<BitmapWindow &>(window);
  draw(bmwnd.bitmap(), window.size());
  return DefWindowProcW(handle, message, w_param, l_param);
}

INT Engine::Start(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                  int cmd_show) {
  if (engine_) {
    throw std::runtime_error{"Start function cannot be called more than one time."};
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
  auto window = std::make_unique<BitmapWindow>(wc,
                      NULL,
                      L"WindowClass1",  // name of the window class
                      L"Our First Windowed Program",     // title of the window
                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // window style
                      kWindowPosition,    // position of the window
                      kWindowResolution,  // window size
                      nullptr,            // we have no parent window, nullptr
                      nullptr,            // we aren't using menus, nullptr
                      instance,           // application handle
                      nullptr);           // used with multiple windows, nullptr

  engine_ = std::unique_ptr<Engine>(new Engine{
      instance, prev_instance, cmd_line, cmd_show, std::move(window)});
  return engine_->Join();
}

Engine::Engine(HINSTANCE instance, HINSTANCE prev_instance, PWSTR, int cmd_show,
    std::unique_ptr<BitmapWindow> window)
    : window_(std::move(window)),
      instance_(instance),
      prev_instance_(prev_instance) {
  // display the window on the screen
  ShowWindow(window_->handle(), cmd_show);
  draw(window_->bitmap(), window_->size());
  // register callbacks
  window_->SetCallback(WM_DESTROY, OnDestroy);
  window_->SetCallback(WM_PAINT, OnPaint);
  window_->SetCallback(WM_EXITSIZEMOVE, OnExitSizeMove);
}

}  // namespace engine