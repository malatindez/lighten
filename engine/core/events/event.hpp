#pragma once
#include <string>

#define EVENT_CLASS_TYPE(event_type)                                        \
    static EventType static_type() { return EventType::event_type; }        \
    [[nodiscard]] EventType type() const noexcept override { return static_type(); } \
    static constexpr std::string_view event_name = #event_type;             \
    [[nodiscard]] std::string_view name() const override { return event_name; }

#define EVENT_CLASS_CATEGORY(category) \
    [[nodiscard]] uint8_t category_flags() const noexcept override { return category; }
namespace engine
{
    enum class EventType
    {
        None = 0,
        WindowClose,
        WindowResize,

        AppTick,
        AppUpdate,
        AppRender,

        KeyPressed,
        KeyReleased,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput = 1 << 1,
        EventCategoryKeyboard = 1 << 2,
        EventCategoryMouse = 1 << 3,
        EventCategoryMouseButton = 1 << 4
    };

    class Event
    {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType type() const noexcept = 0;
        [[nodiscard]] virtual uint8_t category_flags() const noexcept = 0;
        [[nodiscard]] inline bool in_category(uint8_t category) const noexcept { return (category & category_flags()); }
        [[nodiscard]] virtual std::string_view name() const = 0;
        [[nodiscard]] virtual std::string to_string() const noexcept { return std::string(name()); }

        bool handled = false;
    };
} // namespace engine