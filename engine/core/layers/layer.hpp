#pragma once
#include "core/events.hpp"

namespace engine::core
{
    class Layer
    {
    public:
        explicit Layer(std::string_view name = "Layer") : debug_name_(name) {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate() {}
        virtual void OnRender() {}
        virtual void OnGuiRender() {}
        virtual void OnEvent([[maybe_unused]] events::Event &event) {}
        virtual void OnTick([[maybe_unused]] float delta_time) {}

        [[nodiscard]] std::string_view name() const { return debug_name_; }

    private:
        std::string debug_name_;
    };
} // namespace engine::core