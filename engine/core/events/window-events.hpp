#pragma once
#include "event.hpp"
#include "math.hpp"

namespace engine::core::events
{
    class WindowCloseEvent final : public Event
    {
    public:
        explicit WindowCloseEvent(void *handle) : Event(EventType::WindowClose, EventCategoryApplication), handle_(handle) {}
        [[nodiscard]] inline void *handle() const noexcept { return handle_; }
        EVENT_CLASS_TYPE(WindowClose)
    private:
        void *handle_;
    };
    class WindowResizeEvent final : public Event
    {
    public:
        explicit WindowResizeEvent(math::ivec2 &&size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{std::move(size)} {}
        explicit WindowResizeEvent(math::ivec2 const &size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{size} {}
        WindowResizeEvent(int x, int y) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{x, y} {}
        [[nodiscard]] inline math::ivec2 const &size() const noexcept { return window_size_; }

        EVENT_CLASS_TYPE(WindowResize)
    private:
        math::ivec2 window_size_;
    };
} // namespace engine::core::events