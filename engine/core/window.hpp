#pragma once
#include "core/events.hpp"
#include "core/math.hpp"

#include <string>

namespace engine::core
{
    class Window
    {
    public:
        struct Props
        {
            std::wstring title;
            math::ivec2 size;
            math::ivec2 position;
            Props(std::wstring const &title = L"Engine", math::ivec2 size = { 1600, 900 }, math::ivec2 position = { 100, 100 }) : title { title }, size { size }, position { position } {}
        };
        Window(Props const &props) : title_ { props.title }, size_ { props.size }, position_ { props.position } {}
        virtual ~Window() = default;

        [[nodiscard]] math::ivec2 const &size() const noexcept { return size_; }
        [[nodiscard]] math::ivec2 const &position() const noexcept { return position_; }
        [[nodiscard]] std::wstring const &title() const noexcept { return title_; }
        [[nodiscard]] bool const &alive() const noexcept { return alive_; }

        [[nodiscard]] virtual void *native() = 0;

        void SetEventCallback(core::EventCallbackFn const &event_fn) noexcept { event_callback_ = event_fn; }
    protected:
        core::EventCallbackFn event_callback_;
        std::wstring title_;
        math::ivec2 size_;
        math::ivec2 position_;
        bool alive_ = true;
    };
} // namespace engine::core