#pragma once
#include "event.hpp"
#include "math.hpp"
namespace engine
{

    class MouseMovedEvent final : public Event
    {
    public:
        explicit MouseMovedEvent(math::ivec2 &&coordinates)
            : coordinates_{std::move(coordinates)} {}
        explicit MouseMovedEvent(math::ivec2 const &coordinates)
            : coordinates_{coordinates} {}
        MouseMovedEvent(int x, int y) : coordinates_{x, y} {}
        [[nodiscard]] inline math::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        }
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
        EVENT_CLASS_TYPE(MouseMoved)
    private:
        math::ivec2 coordinates_;
    };

    class MouseButtonPressedEvent final : public Event
    {
    public:
        explicit MouseButtonPressedEvent(uint16_t const code,
                                         math::ivec2 &&coordinates)
            : mouse_button_(code), coordinates_(std::move(coordinates)) {}
        explicit MouseButtonPressedEvent(uint16_t const code,
                                         math::ivec2 const &coordinates)
            : mouse_button_(code), coordinates_(coordinates) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(mouse_button_);
        }
        [[nodiscard]] inline uint16_t mouse_button() const noexcept
        {
            return mouse_button_;
        };
        [[nodiscard]] inline math::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        };
        EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryMouse |
                             EventCategoryInput)
        EVENT_CLASS_TYPE(MouseButtonPressed)
    private:
        uint16_t const mouse_button_;
        math::ivec2 const coordinates_;
    };

    class MouseButtonReleasedEvent final : public Event
    {
    public:
        explicit MouseButtonReleasedEvent(uint16_t const code,
                                          math::ivec2 &&coordinates)
            : mouse_button_(code), coordinates_(std::move(coordinates)) {}
        explicit MouseButtonReleasedEvent(uint16_t const code,
                                          math::ivec2 const &coordinates)
            : mouse_button_(code), coordinates_(coordinates) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(mouse_button_);
        }
        [[nodiscard]] inline uint16_t mouse_button() const noexcept
        {
            return mouse_button_;
        };
        [[nodiscard]] inline math::ivec2 const &coordinates() const noexcept
        {
            return coordinates_;
        };
        EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryMouse |
                             EventCategoryInput)
        EVENT_CLASS_TYPE(MouseButtonReleased)
    private:
        uint16_t const mouse_button_;
        math::ivec2 const coordinates_;
    };
} // namespace engine