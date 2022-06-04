#pragma once
#define NOMINMAX
#include <Windows.h>
#include "event.hpp"
#include "math.hpp"
#include "pch.hpp"

namespace engine::core::events
{
    class WindowCloseEvent final : public Event
    {
    public:
        explicit WindowCloseEvent(HWND handle) : Event(EventType::WindowClose, EventCategoryApplication), handle_(handle) {}
        [[nodiscard]] inline HWND handle() const noexcept { return handle_; }
        EVENT_CLASS_TYPE(WindowClose)
    private:
        HWND handle_;
    };
    class WindowResizeEvent final : public Event
    {
    public:
        explicit WindowResizeEvent(core::math::ivec2 &&size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{std::move(size)} {}
        explicit WindowResizeEvent(core::math::ivec2 const &size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{size} {}
        WindowResizeEvent(int x, int y) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{x, y} {}
        [[nodiscard]] inline core::math::ivec2 const &size() const noexcept { return window_size_; }

        EVENT_CLASS_TYPE(WindowResize)
    private:
        core::math::ivec2 window_size_;
    };
} // namespace engine::core::events