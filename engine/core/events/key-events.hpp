#pragma once
#include "event.hpp"

namespace engine
{
    class KeyPressedEvent final : public Event
    {
    public:
        explicit KeyPressedEvent(uint32_t const code, uint16_t const count) : key_code_(code), count_(count) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(key_code_);
        }
        [[nodiscard]] uint32_t key_code() const noexcept { return key_code_; };
        [[nodiscard]] uint16_t count() const noexcept { return count_; };
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
        EVENT_CLASS_TYPE(KeyPressed)
    private:
        uint32_t const key_code_;
        uint16_t const count_;
    };

    class KeyReleasedEvent final : public Event
    {
    public:
        explicit KeyReleasedEvent(uint32_t const code) : key_code_(code) {}
        [[nodiscard]] std::string to_string() const noexcept override
        {
            return std::string(name()) + std::to_string(key_code_);
        }
        [[nodiscard]] uint32_t key_code() const noexcept { return key_code_; };
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
        EVENT_CLASS_TYPE(KeyReleased)
    private:
        uint32_t const key_code_;
    };
} // namespace engine