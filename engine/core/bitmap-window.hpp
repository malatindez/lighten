#pragma once
#include <exception>

#include "core/window.hpp"
namespace engine::core
{
  class BitmapWindow final : public Window
  {
  public:
    BitmapWindow(WNDCLASSEXW const &window_class, DWORD extended_style,
                 std::wstring const &class_name, std::wstring const &window_name,
                 DWORD style, math::ivec2 position, math::ivec2 size,
                 HWND parent_window, HMENU menu, HINSTANCE instance,
                 LPVOID lp_param);

    BitmapWindow(BitmapWindow &&BitmapWindow) = delete;
    BitmapWindow &operator=(BitmapWindow &&BitmapWindow) = delete;
    BitmapWindow(BitmapWindow const &BitmapWindow) = delete;
    BitmapWindow &operator=(BitmapWindow const &BitmapWindow) = delete;

    [[nodiscard]] inline BITMAPINFO bitmap_info() const noexcept { return bitmap_info_; }

    [[nodiscard]] constexpr std::vector<uint32_t> &bitmap() noexcept { return bitmap_; }
    [[nodiscard]] constexpr std::vector<uint32_t> const &bitmap() const noexcept { return bitmap_; }
    [[nodiscard]] constexpr math::ivec2 const &bitmap_size() const noexcept { return bitmap_size_; }
    [[nodiscard]] constexpr int resolution_scale() const { return resolution_scale_; }
    void SetResolutionScale(int resolution_scale);

    bool PeekOSMessages() override;

  private:
    void OnSizeChanged() override;
    HDC hdc_;
    std::vector<uint32_t> bitmap_;
    BITMAPINFO bitmap_info_;
#ifdef _DEBUG
    int resolution_scale_ = 8;
#else
    int resolution_scale_ = 3;
#endif
    math::ivec2 bitmap_size_;
  };
} // namespace engine::core