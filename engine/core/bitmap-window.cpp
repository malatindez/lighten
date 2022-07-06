#include "core/bitmap-window.hpp"
namespace engine::core
{
  BitmapWindow::BitmapWindow(WNDCLASSEXW const &window_class,
                             DWORD extended_style, std::wstring const &class_name,
                             std::wstring const &window_name, DWORD style,
                             core::math::ivec2 position, core::math::ivec2 size,
                             HWND parent_window, HMENU menu, HINSTANCE instance,
                             LPVOID lp_param)
      : Window(window_class, extended_style, class_name, window_name, style,
               position, size, parent_window, menu, instance, lp_param),
        bitmap_size_(size / resolution_scale_)
  {
    bitmap_.resize(size_t(bitmap_size_.x) * bitmap_size_.y);

    hdc_ = GetDC(handle());

    bitmap_info_.bmiHeader.biSize = sizeof(bitmap_info_.bmiHeader);
    bitmap_info_.bmiHeader.biPlanes = 1;
    bitmap_info_.bmiHeader.biBitCount = 32;
    bitmap_info_.bmiHeader.biCompression = BI_RGB;
    bitmap_info_.bmiHeader.biWidth = bitmap_size_.x;
    bitmap_info_.bmiHeader.biHeight = bitmap_size_.y;
  }
  void BitmapWindow::OnScaleChanged()
  {
    core::math::ivec2 prev = bitmap_size_;
    std::vector<uint32_t> copy = bitmap_;
    OnSizeChanged();
    for (int i = 0; i < bitmap_size_.y; ++i)
    {
      for (int j = 0; j < bitmap_size_.x; ++j)
      {
        bitmap_[i * bitmap_size_.x + j] = copy[static_cast<uint32_t>(float(i) / bitmap_size_.y * prev.y) * prev.x +
                                               static_cast<uint32_t>(float(j) / bitmap_size_.x * prev.x)];
      }
    }
  }
  void BitmapWindow::OnSizeChanged()
  {
    const core::math::ivec2 &size = this->window_size();
    bitmap_size_ = size / resolution_scale_;
    bitmap_.resize(size_t(bitmap_size_.x) * bitmap_size_.y);
    bitmap_info_.bmiHeader.biWidth = bitmap_size_.x;
    bitmap_info_.bmiHeader.biHeight = bitmap_size_.y;
  }

  void BitmapWindow::SetResolutionScale(int resolution_scale)
  {
    resolution_scale_ = resolution_scale;
    OnSizeChanged();
  }
  bool BitmapWindow::PeekOSMessages()
  {
    bool rv = Window::PeekOSMessages();
    StretchDIBits(hdc_, 0, 0, window_size().x, window_size().y, 0, 0,
                  bitmap_size_.x, bitmap_size_.y, bitmap_.data(), &bitmap_info_,
                  DIB_RGB_COLORS, SRCCOPY);
    return rv;
  }
} // namespace engine::core