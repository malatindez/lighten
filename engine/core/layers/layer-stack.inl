#pragma once
#include "layer-stack.hpp"
namespace engine::core
{
    namespace _layer_stack_detail
    {
        template <typename T>
        constexpr bool StaticCheck()
        {
            return &T::OnUpdate != &Layer::OnUpdate ||
                &T::OnRender != &Layer::OnRender ||
                &T::OnGuiRender != &Layer::OnGuiRender ||
                &T::OnEvent != &Layer::OnEvent ||
                &T::OnTick != &Layer::OnTick;
        }
    }
    template <typename T>
    void LayerStack::PushLayer(std::shared_ptr<T> layer)
    {
        static_assert(_layer_stack_detail::StaticCheck<T>());
        if (HasLayer(layer)) [[unlikely]]
        {
            spdlog::warn("Warning: trying to push overlay that already exists. Skipping");
            return;
        }
        all_.push_back(static_pointer_cast<Layer>(layer));
        if constexpr (&T::OnUpdate != &Layer::OnUpdate)
        {
            update_.PushLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnRender != &Layer::OnRender)
        {
            render_.PushLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnGuiRender != &Layer::OnGuiRender)
        {
            gui_render_.PushLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnEvent != &Layer::OnEvent)
        {
            event_.PushLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnTick != &Layer::OnTick)
        {
            tick_.PushLayer(static_pointer_cast<Layer>(layer));
        }
        layer->OnAttach();
    }
    template <typename T>
    void LayerStack::PushOverlay(std::shared_ptr<T> layer)
    {
        static_assert(_layer_stack_detail::StaticCheck<T>());
        if (HasLayer(layer)) [[unlikely]]
        {
            spdlog::warn("Warning: trying to push overlay that already exists. Skipping");
            return;
        }
        all_.push_back(static_pointer_cast<Layer>(layer));
        if constexpr (&T::OnUpdate != &Layer::OnUpdate)
        {
            update_.PushOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnRender != &Layer::OnRender)
        {
            render_.PushOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnGuiRender != &Layer::OnGuiRender)
        {
            gui_render_.PushOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnEvent != &Layer::OnEvent)
        {
            event_.PushOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnTick != &Layer::OnTick)
        {
            tick_.PushOverlay(static_pointer_cast<Layer>(layer));
        }
        layer->OnAttach();
    }
    template <typename T>
    void LayerStack::PopLayer(std::shared_ptr<T> layer)
    {
        static_assert(_layer_stack_detail::StaticCheck<T>());
        if (auto it = std::ranges::find(all_, layer);
            it != all_.end()) [[unlikely]]
        {
            all_.erase(it);
        }
        else
        {
            spdlog::warn("Warning: trying to pop layer that doesnt exist. Skipping");
            return;
        }
        if constexpr (&T::OnUpdate != &Layer::OnUpdate)
        {
            update_.PopLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnRender != &Layer::OnRender)
        {
            render_.PopLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnGuiRender != &Layer::OnGuiRender)
        {
            gui_render_.PopLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnEvent != &Layer::OnEvent)
        {
            event_.PopLayer(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnTick != &Layer::OnTick)
        {
            tick_.PopLayer(static_pointer_cast<Layer>(layer));
        }
            layer->OnDetach();
    }
    template <typename T>
    void LayerStack::PopOverlay(std::shared_ptr<T> layer)
    {
        static_assert(_layer_stack_detail::StaticCheck<T>());
        if (auto it = std::ranges::find(all_, layer);
            it != all_.end()) [[unlikely]]
        {
            all_.erase(it);
        }
        else
        {
            spdlog::warn("Warning: trying to pop overlay that doesnt exist. Skipping");
            return;
        }
        if constexpr (&T::OnUpdate != &Layer::OnUpdate)
        {
            update_.PopOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnRender != &Layer::OnRender)
        {
            render_.PopOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnGuiRender != &Layer::OnGuiRender)
        {
            gui_render_.PopOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnEvent != &Layer::OnEvent)
        {
            event_.PopOverlay(static_pointer_cast<Layer>(layer));
        }
        if constexpr (&T::OnTick != &Layer::OnTick)
        {
            tick_.PopOverlay(static_pointer_cast<Layer>(layer));
        }
            layer->OnDetach();
    }
    template <typename T>
    bool LayerStack::HasLayer(std::shared_ptr<T> layer)
    {
        static_assert(_layer_stack_detail::StaticCheck<T>());
        return std::ranges::find(all_, layer) != all_.end();
    }
}