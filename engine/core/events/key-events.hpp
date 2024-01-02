#pragma once
#include "event.hpp"

namespace lighten::core::events
{
    class KeyPressedEvent final : public Event
    {
    public:
        explicit KeyPressedEvent(uint32_t const code)
            : Event(EventType::KeyPressed, EventCategoryKeyboard | EventCategoryInput), key_code_(code) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(key_code_);
        }
        [[nodiscard]] inline uint32_t key_code() const noexcept { return key_code_; };
        EVENT_CLASS_TYPE(KeyPressed)
    private:
        uint32_t const key_code_;
    };

    class KeyReleasedEvent final : public Event
    {
    public:
        explicit KeyReleasedEvent(uint32_t const code)
            : Event(EventType::KeyReleased, EventCategoryKeyboard | EventCategoryInput), key_code_(code) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(key_code_);
        }
        [[nodiscard]] inline uint32_t key_code() const noexcept { return key_code_; };
        EVENT_CLASS_TYPE(KeyReleased)
    private:
        uint32_t const key_code_;
    };
} // namespace lighten::core::events