#pragma once
#include "layer-stack.hpp"
namespace engine::core
{
    class LayerStackThreadsafe : private LayerStack
    {
    public:
        using LayerStack::LayerStack;
        virtual ~LayerStackThreadsafe() { std::unique_lock guard { mutex }; }
        template <typename T>
        void PushLayer(std::shared_ptr<T> layer);
        template <typename T>
        void PushOverlay(std::shared_ptr<T> layer);
        template <typename T>
        void PopLayer(std::shared_ptr<T> layer);
        template <typename T>
        void PopOverlay(std::shared_ptr<T> layer);
        template <typename T>
        bool HasLayer(std::shared_ptr<T> layer);

        void OnUpdate() override;
        void OnRender() override;
        void OnGuiRender() override;
        void OnEvent(events::Event &e) override;
        void OnTick(float delta_time) override;
    private:
        std::shared_mutex mutex;
    };
}
#include "layer-stack-threadsafe.inl"