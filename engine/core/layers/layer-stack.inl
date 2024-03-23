#pragma once
#include "layer-stack.hpp"
namespace lighten::core
{
    namespace _layer_stack_detail
    {
        template <typename T>
        void UnderlyingStack<T>::PushLayer(T *layer)
        {
            layers_.emplace(layers_.begin() + layer_insert_index_, layer);
            layer_insert_index_++;
        }
        template <typename T>
        void UnderlyingStack<T>::PushOverlay(T *overlay)
        {
            layers_.emplace_back(overlay);
        }
        template <typename T>
        void UnderlyingStack<T>::PopLayer(T *layer)
        {
            auto it = std::find(layers_.begin(), layers_.begin() + layer_insert_index_, layer);
            if (it != layers_.begin() + layer_insert_index_) [[likely]]
            {
                layers_.erase(it);
                layer_insert_index_--;
            }
        }
        template <typename T>
        void UnderlyingStack<T>::PopOverlay(T *overlay)
        {
            auto it = std::find(layers_.begin() + layer_insert_index_, layers_.end(), overlay);
            if (it != layers_.end()) [[likely]]
            {
                layers_.erase(it);
            }
        }
        template <typename T>
        bool UnderlyingStack<T>::HasLayer(T *layer)
        {
            return layers_.end() != std::find(layers_.begin(), layers_.end(), layer);
        }

        template <typename T>
        constexpr bool StaticCheck()
        {
            return std::is_base_of_v<Layer::HandleEvent, T> ||
                   std::is_base_of_v<Layer::HandlePreUpdate, T> ||
                   std::is_base_of_v<Layer::HandleUpdate, T> ||
                   std::is_base_of_v<Layer::HandlePostUpdate, T> ||
                   std::is_base_of_v<Layer::HandleRender, T> ||
                   std::is_base_of_v<Layer::HandleGuiRender, T> ||
                   std::is_base_of_v<Layer::HandleTick, T>;
        }
    } // namespace _layer_stack_detail
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
        if constexpr (std::is_base_of_v<Layer::HandlePreUpdate, T>)
        {
            pre_update_.PushLayer(static_cast<Layer::HandlePreUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PushLayer(static_cast<Layer::HandleUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandlePostUpdate, T>)
        {
            post_update_.PushLayer(static_cast<Layer::HandlePostUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleRender, T>)
        {
            render_.PushLayer(static_cast<Layer::HandleRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleGuiRender, T>)
        {
            gui_render_.PushLayer(static_cast<Layer::HandleGuiRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleEvent, T>)
        {
            event_.PushLayer(static_cast<Layer::HandleEvent *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleTick, T>)
        {
            tick_.PushLayer(static_cast<Layer::HandleTick *>(layer.get()));
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
        
        if constexpr (std::is_base_of_v<Layer::HandlePreUpdate, T>)
        {
            pre_update_.PushOverlay(static_cast<Layer::HandlePreUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PushOverlay(static_cast<Layer::HandleUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandlePostUpdate, T>)
        {
            post_update_.PushOverlay(static_cast<Layer::HandlePostUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleRender, T>)
        {
            render_.PushOverlay(static_cast<Layer::HandleRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleGuiRender, T>)
        {
            gui_render_.PushOverlay(static_cast<Layer::HandleGuiRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleEvent, T>)
        {
            event_.PushOverlay(static_cast<Layer::HandleEvent *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleTick, T>)
        {
            tick_.PushOverlay(static_cast<Layer::HandleTick *>(layer.get()));
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
        if constexpr (std::is_base_of_v<Layer::HandlePreUpdate, T>)
        {
            pre_update_.PopLayer(static_cast<Layer::HandlePreUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PopLayer(static_cast<Layer::HandleUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandlePostUpdate, T>)
        {
            post_update_.PopLayer(static_cast<Layer::HandlePostUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleRender, T>)
        {
            render_.PopLayer(static_cast<Layer::HandleRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleGuiRender, T>)
        {
            gui_render_.PopLayer(static_cast<Layer::HandleGuiRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleEvent, T>)
        {
            event_.PopLayer(static_cast<Layer::HandleEvent *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleTick, T>)
        {
            tick_.PopLayer(static_cast<Layer::HandleTick *>(layer.get()));
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
        if constexpr (std::is_base_of_v<Layer::HandlePreUpdate, T>)
        {
            pre_update_.PopOverlay(static_cast<Layer::HandlePreUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PopOverlay(static_cast<Layer::HandleUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandlePostUpdate, T>)
        {
            post_update_.PopOverlay(static_cast<Layer::HandlePostUpdate *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleRender, T>)
        {
            render_.PopOverlay(static_cast<Layer::HandleRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleGuiRender, T>)
        {
            gui_render_.PopOverlay(static_cast<Layer::HandleGuiRender *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleEvent, T>)
        {
            event_.PopOverlay(static_cast<Layer::HandleEvent *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<Layer::HandleTick, T>)
        {
            tick_.PopOverlay(static_cast<Layer::HandleTick *>(layer.get()));
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