#include <Windows.h>

#include "math/ray.hpp"
#include "math/vec.hpp"

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);

using namespace math;

const ivec2 kWindowPosition{0};
const ivec2 kWindowResolution{1280, 720};

bool hit_sphere(vec3 const& center, float radius, ray const& r) {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0f * dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  return discriminant > 0;
}

vec3 color(ray const& r) {
  if (hit_sphere(vec3{0, 0, -1}, 0.5f, r)) {
    return vec3{1, 0, 0};
  }
  vec3 unit_direction = r.direction().unit_vector();
  float t = 0.5f * (unit_direction.y() + 1);
  return (1.0f - t) * vec3{1} + t * vec3{0.5f, 0.7f, 1.0f};
}

void draw(HDC hdc, ivec2 window_size) {
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
      auto ir = int(255.99 * col[0]);
      auto ig = int(255.99 * col[1]) << 8;
      auto ib = int(255.99 * col[2]) << 16;

      SetPixel(hdc, i, j, ir | ig | ib);
    }
  }
}

// the entry point for any Windows program
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT nShowCmd) {
  // the handle for the window, filled by a function
  HWND hWnd;
  // this struct holds information for the window class
  WNDCLASSEXW wc;

  // clear out the window class for use
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  // fill in the struct with the needed information
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszClassName = L"WindowClass1";

  // register the window class
  RegisterClassExW(&wc);

  // create the window and use the result as the handle
  hWnd = CreateWindowExW(NULL,
                         L"WindowClass1",  // name of the window class
                         L"Our First Windowed Program",  // title of the window
                         WS_OVERLAPPEDWINDOW,            // window style
                         kWindowPosition.x(),    // x-position of the window
                         kWindowPosition.y(),    // y-position of the window
                         kWindowResolution.x(),  // width of the window
                         kWindowResolution.y(),  // height of the window
                         nullptr,    // we have no parent window, nullptr
                         nullptr,    // we aren't using menus, nullptr
                         hInstance,  // application handle
                         nullptr);   // used with multiple windows, nullptr

  // display the window on the screen
  ShowWindow(hWnd, nShowCmd);

  draw(GetDC(hWnd), kWindowResolution);  // draw the image

  // enter the main loop:

  // this struct holds Windows event messages
  MSG msg;
  // wait for the next message in the queue, store the result in 'msg'
  while (GetMessage(&msg, nullptr, 0, 0)) {
    // translate keystroke messages into the right format
    TranslateMessage(&msg);

    // send the message to the WindowProc function
    DispatchMessage(&msg);
  }

  // return this part of the WM_QUIT message to Windows
  return int(msg.wParam);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  if (message == WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  }

  // Handle any messages the switch statement didn't
  return DefWindowProc(hWnd, message, wParam, lParam);
}