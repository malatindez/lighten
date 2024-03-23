#include "layer-stack-threadsafe.hpp"
namespace lighten::core
{
    void LayerStackThreadsafe::OnPreUpdate()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnPreUpdate();
    }
    void LayerStackThreadsafe::OnUpdate()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnUpdate();
    }
    void LayerStackThreadsafe::OnPostUpdate()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnPostUpdate();
    }
    void LayerStackThreadsafe::OnRender()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnRender();
    }
    void LayerStackThreadsafe::OnGuiRender()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnGuiRender();
    }
    void LayerStackThreadsafe::OnEvent(events::Event &e)
    {
        std::shared_lock guard{mutex};
        LayerStack::OnEvent(e);
    }
    void LayerStackThreadsafe::OnTick(float delta_time)
    {
        std::shared_lock guard{mutex};
        LayerStack::OnTick(delta_time);
    }
}