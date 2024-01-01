#pragma once
#include "core/math.hpp"
#include "event.hpp"

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
        explicit WindowResizeEvent(glm::ivec2 &&size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{std::move(size)} {}
        explicit WindowResizeEvent(glm::ivec2 const &size) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{size} {}
        WindowResizeEvent(int x, int y) : Event(EventType::WindowResize, EventCategoryApplication), window_size_{x, y} {}
        [[nodiscard]] inline glm::ivec2 const &size() const noexcept { return window_size_; }

        EVENT_CLASS_TYPE(WindowResize)
    private:
        glm::ivec2 window_size_;
    };
} // namespace engine::core::events