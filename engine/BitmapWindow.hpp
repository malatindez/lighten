#pragma once
#include <exception>

#include "Window.hpp"
namespace engine {
class BitmapWindow final : public Window {
 public:
  BitmapWindow(WindowClass const& window_class, DWORD extended_style,
               std::wstring const& class_name, std::wstring const& window_name,
               DWORD style, math::ivec2 position, math::ivec2 size,
               HWND parent_window, HMENU menu, HINSTANCE instance,
               LPVOID lp_param)
      : Window(window_class, extended_style, class_name, window_name, style,
               position, size, parent_window, menu, instance, lp_param) {
    bitmap_.resize(size_t(size.x()) * size.y());

    Window::SetMainLoopCallback(std::bind_front(&BitmapWindow::MainLoop, this));

    hdc_ = GetDC(handle());

    bitmap_info_.bmiHeader.biSize = sizeof(bitmap_info_.bmiHeader);
    bitmap_info_.bmiHeader.biPlanes = 1;
    bitmap_info_.bmiHeader.biBitCount = 32;
    bitmap_info_.bmiHeader.biCompression = BI_RGB;
    bitmap_info_.bmiHeader.biWidth = size.x();
    bitmap_info_.bmiHeader.biHeight = size.y();
  }

  void SetMainLoopCallback(MainLoopCallback const& callback) override {
    main_loop_callback_ = callback;
  }
  [[nodiscard]] inline BITMAPINFO bitmap_info() const noexcept {
    return bitmap_info_;
  }

  [[nodiscard]] constexpr std::vector<uint32_t>& bitmap() noexcept {
    return bitmap_;
  }
  [[nodiscard]] constexpr std::vector<uint32_t> const& bitmap() const noexcept {
    return bitmap_;
  }

 private:
  virtual void OnSizeChanged() final {
    const math::ivec2 size = this->size();
    bitmap_info_.bmiHeader.biWidth = size.x();
    bitmap_info_.bmiHeader.biHeight = size.y();
    bitmap_.resize(size_t(size.x()) * size.y());
  }
  using u32 = uint32_t;
  void draw_pixel(int x, int y, u32 color) {
    u32* pixel = (u32*)bitmap_.data();
    pixel += y * size().x() + x;
    *pixel = color;
  }

  void clear_screen(u32 color) {
    u32* pixel = (u32*)bitmap_.data();
    for (int i = 0; i < size().x() * size().y(); ++i) {
      *pixel++ = color;
    }
  }
  void MainLoop(Window&) {
    if (main_loop_callback_ != nullptr) {
      main_loop_callback_(std::ref(*this));
    }
    // draw bitmap
    StretchDIBits(hdc_, 0, 0, size().x(), size().y(), 0, 0, size().x(),
                  size().y(), bitmap_.data(), &bitmap_info_, DIB_RGB_COLORS,
                  SRCCOPY);
  }
  HDC hdc_;
  std::vector<uint32_t> bitmap_;
  BITMAPINFO bitmap_info_;
  MainLoopCallback main_loop_callback_;
};
}  // namespace engine