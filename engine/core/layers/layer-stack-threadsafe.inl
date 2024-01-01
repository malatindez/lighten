#pragma once
#include "layer-stack-threadsafe.hpp"
namespace engine::core
{
    template <typename T>
    void LayerStackThreadsafe::PushLayer(std::shared_ptr<T> layer)
    {
        std::unique_lock guard{mutex};
        LayerStack::PushLayer(layer);
    }
    template <typename T>
    void LayerStackThreadsafe::PushOverlay(std::shared_ptr<T> layer)
    {
        std::unique_lock guard{mutex};
        LayerStack::PushOverlay(layer);
    }
    template <typename T>
    void LayerStackThreadsafe::PopLayer(std::shared_ptr<T> layer)
    {
        std::unique_lock guard{mutex};
        LayerStack::PopLayer(layer);
    }
    template <typename T>
    void LayerStackThreadsafe::PopOverlay(std::shared_ptr<T> layer)
    {
        std::unique_lock guard{mutex};
        LayerStack::PopOverlay(layer);
    }
    template <typename T>
    bool LayerStackThreadsafe::HasLayer(std::shared_ptr<T> layer)
    {
        std::shared_lock guard{mutex};
        LayerStack::HasLayer(layer);
    }
}