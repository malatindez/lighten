// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);

#include "math/ray.hpp"
#include "math/vec.hpp"
using namespace math;
vec3 color(ray const& r) {
  vec3 unit_direction = r.direction().unit_vector();
  float t = 0.5 * (unit_direction.y() + 1);
  return (1.0 - t) * vec3{1} + t * vec3{0.5, 0.7, 1.0};
}

void draw(HDC hdc, ivec2 window_size) {
  vec3 lower_left_corner{-2, -1, -1};
  vec3 horizontal{4, 0, 0};
  vec3 vertical{0, 2, 0};
  vec3 origin{0};
  for (int j = window_size.y() - 1; j >= 0; j--) {
    for (int i = 0; i < window_size.x(); i++) {
      float u = float(i) / window_size.x();
      float v = float(j) / window_size.y();
      ray r(origin, lower_left_corner + u * horizontal + v * vertical);
      vec3 col = color(r);
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]) << 8;
      int ib = int(255.99 * col[2]) << 16;

      SetPixel(hdc, i, j, ir | ig | ib);
    }
  }
}
const ivec2 kWindowPosition{0};
const ivec2 kWindowResolution{1280, 720};

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
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
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
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
                         NULL,       // we have no parent window, NULL
                         NULL,       // we aren't using menus, NULL
                         hInstance,  // application handle
                         NULL);      // used with multiple windows, NULL

  // display the window on the screen
  ShowWindow(hWnd, nShowCmd);

  draw(GetDC(hWnd), kWindowResolution);  // draw the image

  // enter the main loop:

  // this struct holds Windows event messages
  MSG msg;
  // wait for the next message in the queue, store the result in 'msg'
  while (GetMessage(&msg, NULL, 0, 0)) {
    // translate keystroke messages into the right format
    TranslateMessage(&msg);

    // send the message to the WindowProc function
    DispatchMessage(&msg);
  }

  // return this part of the WM_QUIT message to Windows
  return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  // sort through and find what code to run for the message given
  switch (message) {
    // this message is read when the window is closed
    case WM_DESTROY: {
      // close the application entirely
      PostQuitMessage(0);
      return 0;
    } break;
  }

  // Handle any messages the switch statement didn't
  return DefWindowProc(hWnd, message, wParam, lParam);
}