#pragma once
#include "core/events.hpp"

namespace lighten::core
{
    class Layer
    {
    public:
        explicit Layer(std::string_view name = "Layer") : debug_name_(name) {}
        virtual ~Layer() = default;
        Layer(Layer const &) = delete;
        Layer &operator=(Layer const &) = delete;
        Layer(Layer &&) = delete;
        Layer &operator=(Layer &&) = delete;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        [[nodiscard]] std::string_view name() const { return debug_name_; }

    private:
        std::string debug_name_;

    public:
        struct HandleEvent
        {
            virtual void OnEvent([[maybe_unused]] events::Event &event) = 0;
        };
        struct HandleUpdate
        {
            virtual void OnUpdate() = 0;
        };
        struct HandleRender
        {
            virtual void OnRender() = 0;
        };
        struct HandleGuiRender
        {
            virtual void OnGuiRender() = 0;
        };
        struct HandleTick
        {
            virtual void OnTick([[maybe_unused]] float delta_time) = 0;
        };
    };
} // namespace lighten::core