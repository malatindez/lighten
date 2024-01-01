#pragma once
#include "core/math.hpp"
#include "event.hpp"
namespace engine::core::events
{
    class MouseMovedEvent final : public Event
    {
    public:
        explicit MouseMovedEvent(glm::ivec2 &&coordinates)
            : Event(EventType::MouseMoved, EventCategoryMouse | EventCategoryInput), coordinates_{std::move(coordinates)} {}
        explicit MouseMovedEvent(glm::ivec2 const &coordinates)
            : Event(EventType::MouseMoved, EventCategoryMouse | EventCategoryInput), coordinates_{coordinates} {}
        MouseMovedEvent(int x, int y)
            : Event(EventType::MouseMoved, EventCategoryMouse | EventCategoryInput), coordinates_{x, y} {}
        [[nodiscard]] inline glm::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        }
        EVENT_CLASS_TYPE(MouseMoved)
    private:
        glm::ivec2 coordinates_;
    };

    class MouseButtonPressedEvent final : public Event
    {
    public:
        explicit MouseButtonPressedEvent(uint16_t const code, glm::ivec2 &&coordinates)
            : Event(EventType::MouseButtonPressed, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput),
              mouse_button_(code), coordinates_(std::move(coordinates))
        {
        }
        explicit MouseButtonPressedEvent(uint16_t const code, glm::ivec2 const &coordinates)
            : Event(EventType::MouseButtonPressed, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput),
              mouse_button_(code), coordinates_(coordinates)
        {
        }
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(mouse_button_);
        }
        [[nodiscard]] inline uint16_t mouse_button() const noexcept
        {
            return mouse_button_;
        };
        [[nodiscard]] inline glm::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        };
        EVENT_CLASS_TYPE(MouseButtonPressed)
    private:
        uint16_t const mouse_button_;
        glm::ivec2 const coordinates_;
    };

    class MouseButtonReleasedEvent final : public Event
    {
    public:
        explicit MouseButtonReleasedEvent(uint16_t const code,
                                          glm::ivec2 &&coordinates)
            : Event(EventType::MouseButtonReleased, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput), mouse_button_(code), coordinates_(std::move(coordinates))
        {
        }
        explicit MouseButtonReleasedEvent(uint16_t const code,
                                          glm::ivec2 const &coordinates)
            : Event(EventType::MouseButtonReleased, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput), mouse_button_(code), coordinates_(coordinates)
        {
        }
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(mouse_button_);
        }
        [[nodiscard]] inline uint16_t mouse_button() const noexcept
        {
            return mouse_button_;
        };
        [[nodiscard]] inline glm::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        };
        EVENT_CLASS_TYPE(MouseButtonReleased)
    private:
        uint16_t const mouse_button_;
        glm::ivec2 const coordinates_;
    };

    class MouseScrollEvent final : public Event
    {
    public:
        explicit MouseScrollEvent(int16_t const scroll_delta,
                                  glm::ivec2 &&coordinates)
            : Event(EventType::MouseScrolled, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput), delta_(scroll_delta), coordinates_(std::move(coordinates))
        {
        }
        explicit MouseScrollEvent(int16_t const scroll_delta,
                                  glm::ivec2 const &coordinates)
            : Event(EventType::MouseScrolled, EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput), delta_(scroll_delta), coordinates_(coordinates)
        {
        }
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(delta_);
        }
        [[nodiscard]] inline int16_t delta() const noexcept
        {
            return delta_;
        };
        [[nodiscard]] inline glm::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        };
        EVENT_CLASS_TYPE(MouseScrollEvent)
    private:
        int16_t const delta_;
        glm::ivec2 const coordinates_;
    };
} // namespace engine::core::events