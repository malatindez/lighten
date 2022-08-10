#include "layer-stack.hpp"
#include "include/pch.hpp"
namespace engine::core::_detail
{

    void UnderlyingStack::PushLayer(std::shared_ptr<Layer> layer)
    {
        layers_.emplace(layers_.begin() + layer_insert_index_, layer);
        layer->OnAttach();
        layer_insert_index_++;
    }
    void UnderlyingStack::PushOverlay(std::shared_ptr<Layer> overlay)
    {
        layers_.emplace_back(overlay);
        overlay->OnAttach();
    }
    void UnderlyingStack::PopLayer(std::shared_ptr<Layer> layer)
    {
        auto it = std::find(layers_.begin(), layers_.begin() + layer_insert_index_, layer);
        if (it != layers_.begin() + layer_insert_index_) [[likely]]
        {
            layer->OnDetach();
            layers_.erase(it);
            layer_insert_index_--;
        }
    }
    void UnderlyingStack::PopOverlay(std::shared_ptr<Layer> overlay)
    {
        auto it = std::find(layers_.begin() + layer_insert_index_, layers_.end(), overlay);
        if (it != layers_.end()) [[likely]]
        {
            overlay->OnDetach();
            layers_.erase(it);
        }
    }
    bool UnderlyingStack::HasLayer(std::shared_ptr<Layer> layer)
    {
        return layers_.end() != std::find(layers_.begin(), layers_.end(), layer);
    }
} // namespace engine::core::_detail
namespace engine::core
{
    LayerStack::~LayerStack()
    {
        for (auto &layer : all_)
        {
            layer->OnDetach();
        }
    }
    void LayerStack::OnUpdate()
    {
        for (auto &layer : update_)
        {
            layer->OnUpdate();
        }
    }
    void LayerStack::OnRender()
    {
        for (auto &layer : render_)
        {
            layer->OnRender();
        }
    }
    void LayerStack::OnGuiRender()
    {
        for (auto &layer : gui_render_)
        {
            layer->OnGuiRender();
        }
    }
    void LayerStack::OnEvent(events::Event &e)
    {
        for (auto &layer : event_)
        {
            layer->OnEvent(e);
            if (e.handled) [[unlikely]]
            {
                return;
            }
        }
    }
    void LayerStack::OnTick(float delta_time)
    {
        for (auto &layer : tick_)
        {
            layer->OnTick(delta_time);
        }
    }

}