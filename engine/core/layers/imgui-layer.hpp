#pragma once
#include "core/window.hpp"
#include "include/lighten-pch.hpp"
#include "include/imgui.hpp"

namespace lighten::core
{
    class ImGuiLayer final
        : public Layer,
          public Layer::HandleEvent
    {
    public:
        ImGuiLayer(std::shared_ptr<Window> window) : Layer("ImGuiLayer"), window_{window} {}
        ~ImGuiLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(events::Event &e) override;

        void Begin();
        void End();

        void BlockEvents(bool block = true) { block_events_ = block; }

        void ResetTheme();

    private:
        std::shared_ptr<Window> window_;
        bool block_events_ = true;
    };
}