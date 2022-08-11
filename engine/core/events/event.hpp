#pragma once
#include <string>

#define EVENT_CLASS_TYPE(event_type)                            \
    static constexpr std::string_view event_name = #event_type; \
    [[nodiscard]] std::string_view name() const override { return event_name; }

namespace engine::core::events
{
    enum class EventType : uint32_t
    {
        None = 0,
        WindowClose,
        WindowResize,

        KeyPressed,
        KeyReleased,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled,

        FilesChanged
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryUtils = 1 << 1,
        EventCategoryInput = 1 << 2,
        EventCategoryKeyboard = 1 << 3,
        EventCategoryMouse = 1 << 4,
        EventCategoryMouseButton = 1 << 5,
    };

    class Event
    {
    public:
        Event(EventType type, uint8_t flags) : kType { type }, kCategoryFlags { flags } {}
        virtual ~Event() = default;

        [[nodiscard]] EventType type() const noexcept { return kType; }
        [[nodiscard]] uint8_t category_flags() const noexcept { return kCategoryFlags; };
        [[nodiscard]] inline bool in_category(uint8_t category) const noexcept
        {
            return (category & category_flags());
        }
        [[nodiscard]] virtual std::string_view name() const = 0;
        [[nodiscard]] virtual std::string to_string() const noexcept
        {
            return std::string(name());
        }

        bool handled = false;
        const EventType kType;
        const uint8_t kCategoryFlags;
    };
} // namespace engine::core