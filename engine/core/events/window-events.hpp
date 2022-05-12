#pragma once
#include "event.hpp"
#include "math.hpp"
#include "pch.hpp"

namespace engine
{
    class WindowCloseEvent final : public Event
    {
    public:
        explicit WindowCloseEvent(HWND handle) : handle_(handle) {}
        [[nodiscard]] inline HWND handle() const noexcept { return handle_; }
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowClose)
    private:
        HWND handle_;
    };
    class WindowResizeEvent final : public Event
    {
    public:
        explicit WindowResizeEvent(math::ivec2 &&size) : size_{std::move(size)} {}
        explicit WindowResizeEvent(math::ivec2 const &size) : size_{size} {}
        WindowResizeEvent(int x, int y) : size_{x, y} {}
        [[nodiscard]] inline math::ivec2 const &size() const noexcept { return size_; }

        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(WindowResize)
    private:
        math::ivec2 size_;
    };
}