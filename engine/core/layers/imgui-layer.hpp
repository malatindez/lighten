#pragma once
#include "core/events.hpp"
#include "layer.hpp"

#include <imgui.h>
#include <imguizmo.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "core/window.hpp"

namespace engine::core
{
    class ImGuiLayer final : public Layer
    {
    public:
        ImGuiLayer(std::shared_ptr<Window> window) : Layer("ImGuiLayer"), window_ { window } {}
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