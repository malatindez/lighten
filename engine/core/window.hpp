#pragma once
#include "core/events.hpp"
#include "core/layers/layer.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
namespace engine::core
{
    class Window : public Layer, public Layer::HandleUpdate
    {
    public:

        struct Props
        {
            std::string title;
            math::ivec2 size;
            math::ivec2 position;
            Props(std::string_view title = "Engine", math::ivec2 size = math::ivec2{ 1200, 720 }, math::ivec2 position = math::ivec2{ 100, 100 }) : title{ title }, size{ size }, position{ position } {}
        };
        Window(Props const &props) : title_{ props.title }, size_{ props.size }, position_{ props.position } {}
        virtual ~Window() = default;

        [[nodiscard]] inline math::ivec2 const &size() const noexcept { return size_; }
        [[nodiscard]] inline math::ivec2 const &position() const noexcept { return position_; }
        [[nodiscard]] inline std::string const &title() const noexcept { return title_; }
        [[nodiscard]] inline bool const &alive() const noexcept { return alive_; }

        [[nodiscard]] virtual void *native() = 0;
        void SetEventCallback(core::EventCallbackFn const &event_fn) noexcept { event_callback_ = event_fn; }
    protected:
        core::EventCallbackFn event_callback_;
        std::string title_;
        math::ivec2 size_;
        math::ivec2 position_;
        bool alive_ = true;
    };
} // namespace engine::core