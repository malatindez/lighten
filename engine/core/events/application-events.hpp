#pragma once
#include "core/math.hpp"
#include "event.hpp"
namespace engine::core::events
{
    class AppUpdateEvent final : public Event
    {
    public:
        AppUpdateEvent() : Event(EventType::AppUpdate, EventCategoryApplication) {}
        EVENT_CLASS_TYPE(AppUpdate)
    };
    class AppTickEvent final : public Event
    {
    public:
        explicit AppTickEvent(float delta_time) : Event(EventType::AppTick, EventCategoryApplication), delta_time_(delta_time) {}
        [[nodiscard]] inline float delta_time() const noexcept { return delta_time_; }
        EVENT_CLASS_TYPE(AppTick)
    private:
        float delta_time_;
    };
    class AppRenderEvent final : public Event
    {
    public:
        AppRenderEvent() : Event(EventType::AppRender, EventCategoryApplication) {}
        EVENT_CLASS_TYPE(AppRender)
    };

} // namespace engine::core::events