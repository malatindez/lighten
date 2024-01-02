#pragma once
#include "post-processing.hpp"
namespace lighten::render
{
    namespace _post_processing_detail
    {
        template <typename T>
        constexpr bool StaticCheck()
        {
            return std::is_base_of_v<core::Layer::HandleEvent, T> ||
                   std::is_base_of_v<core::Layer::HandleUpdate, T> ||
                   std::is_base_of_v<core::Layer::HandleRender, T> ||
                   std::is_base_of_v<core::Layer::HandleGuiRender, T> ||
                   std::is_base_of_v<core::Layer::HandleTick, T> ||
                   std::is_base_of_v<PostProcessingLayer::HandleFrameBegin, T> ||
                   std::is_base_of_v<PostProcessingLayer::HandleFrameEnd, T> ||
                   std::is_base_of_v<PostProcessingLayer::HandleProcess, T>;
        }

        template <typename T>
        void UnderlyingStack<T>::PushLayer(T *layer, int64_t weight)
        {
            auto lower = std::lower_bound(weights_.begin(), weights_.end(), weight);
            layers_.emplace(layers_.begin() + (lower - weights_.begin()), layer);
            weights_.emplace(lower, weight);
        }
        template <typename T>
        void UnderlyingStack<T>::PopLayer(T *layer)
        {
            auto it = std::ranges::find(layers_, layer);
            if (it != layers_.end())
            {
                weights_.erase(weights_.begin() + (it - layers_.begin()));
                layers_.erase(it);
            }
        }
        template <typename T>
        bool UnderlyingStack<T>::HasLayer(T *layer)
        {
            auto it = std::find(layers_.begin(), layers_.end(), layer);
            return it != layers_.end();
        }
    }
    template <typename T>
    void PostProcessing::AddLayer(std::shared_ptr<T> layer)
    {
        static_assert(_post_processing_detail::StaticCheck<T>());
        if (HasLayer(layer)) [[unlikely]]
        {
            spdlog::warn("Warning: trying to push layer that already exists. Skipping");
            return;
        }
        all_.push_back(static_pointer_cast<PostProcessingLayer>(layer));
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PushLayer(static_cast<Layer::HandleUpdate *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<Layer::HandleRender, T>)
        {
            render_.PushLayer(static_cast<Layer::HandleRender *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<Layer::HandleGuiRender, T>)
        {
            gui_render_.PushLayer(static_cast<Layer::HandleGuiRender *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<Layer::HandleEvent, T>)
        {
            event_.PushLayer(static_cast<Layer::HandleEvent *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<Layer::HandleTick, T>)
        {
            tick_.PushLayer(static_cast<Layer::HandleTick *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleFrameBegin, T>)
        {
            frame_begin_.PushLayer(static_cast<PostProcessingLayer::HandleFrameBegin *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleFrameEnd, T>)
        {
            frame_end_.PushLayer(static_cast<PostProcessingLayer::HandleFrameEnd *>(layer.get()), layer->kWeight);
        }
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleProcess, T>)
        {
            process_.PushLayer(static_cast<PostProcessingLayer::HandleProcess *>(layer.get()), layer->kWeight);
        }
        layer->OnAttach();
    }
    template <typename T>
    void PostProcessing::RemoveLayer(std::shared_ptr<T> layer)
    {
        static_assert(_post_processing_detail::StaticCheck<T>());
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
        if constexpr (std::is_base_of_v<Layer::HandleUpdate, T>)
        {
            update_.PopLayer(static_cast<Layer::HandleUpdate *>(layer.get()));
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
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleFrameBegin, T>)
        {
            frame_begin_.PopLayer(static_cast<PostProcessingLayer::HandleFrameBegin *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleFrameEnd, T>)
        {
            frame_end_.PopLayer(static_cast<PostProcessingLayer::HandleFrameEnd *>(layer.get()));
        }
        if constexpr (std::is_base_of_v<PostProcessingLayer::HandleProcess, T>)
        {
            process_.PopLayer(static_cast<PostProcessingLayer::HandleProcess *>(layer.get()));
        }
        layer->OnDetach();
    }
    template <typename T>
    bool PostProcessing::HasLayer(std::shared_ptr<T> layer)
    {
        static_assert(_post_processing_detail::StaticCheck<T>());
        return std::ranges::find(all_, layer) != all_.end();
    }
} // namespace lighten::render