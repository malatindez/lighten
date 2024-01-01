#include "layer-stack-threadsafe.hpp"
namespace engine::core
{
    void LayerStackThreadsafe::OnUpdate()
    {
        std::shared_lock guard{mutex};
        LayerStack::OnUpdate();
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