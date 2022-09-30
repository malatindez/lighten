#include "layer-stack.hpp"
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