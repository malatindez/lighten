#pragma once
#include "event.hpp"
#include "math.hpp"
namespace engine
{
    class AppUpdateEvent final : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(AppUpdate)
    };
    class AppTickEvent final : public Event
    {
    public:
        explicit AppTickEvent(float delta_time) : delta_time_(delta_time) {}
        [[nodiscard]] float delta_time() const noexcept { return delta_time_; }
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(AppTick)
    private:
        float delta_time_;
    };
    class AppRenderEvent final : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_TYPE(AppRender)
    };

} // namespace engine