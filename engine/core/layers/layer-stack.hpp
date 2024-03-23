#pragma once
#include "include/lighten-pch.hpp"
#include "layer.hpp"
namespace lighten::core::_layer_stack_detail
{
    using LayerPtrVec = std::vector<std::shared_ptr<Layer>>;
    template <typename T>
    class UnderlyingStack
    {
    public:
        using Vector = std::vector<T *>;
        void PushLayer(T *layer);
        void PopLayer(T *layer);
        void PushOverlay(T *overlay);
        void PopOverlay(T *overlay);
        bool HasLayer(T *layer);

        typename Vector::iterator begin() { return layers_.begin(); }
        typename Vector::iterator end() { return layers_.end(); }
        typename Vector::reverse_iterator rbegin() { return layers_.rbegin(); }
        typename Vector::reverse_iterator rend() { return layers_.rend(); }

        typename Vector::const_iterator begin() const { return layers_.begin(); }
        typename Vector::const_iterator end() const { return layers_.end(); }
        typename Vector::const_reverse_iterator rbegin() const { return layers_.rbegin(); }
        typename Vector::const_reverse_iterator rend() const { return layers_.rend(); }

    private:
        Vector layers_;
        uint32_t layer_insert_index_ = 0;
    };
} // namespace lighten::core::_detail
namespace lighten::core
{
    class LayerStack
        : public Layer,
          public Layer::HandleEvent,
          public Layer::HandlePreUpdate,
          public Layer::HandleUpdate,
          public Layer::HandlePostUpdate,
          public Layer::HandleRender,
          public Layer::HandleGuiRender,
          public Layer::HandleTick
    {
    public:
        using Layer::Layer;
        virtual ~LayerStack();

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

        void OnPreUpdate() override;
        void OnUpdate() override;
        void OnPostUpdate() override;
        void OnRender() override;
        void OnGuiRender() override;
        void OnEvent(events::Event &e) override;
        void OnTick(float delta_time) override;

    protected:
        std::vector<std::shared_ptr<Layer>> all_;
        _layer_stack_detail::UnderlyingStack<Layer::HandlePreUpdate> pre_update_;
        _layer_stack_detail::UnderlyingStack<Layer::HandleUpdate> update_;
        _layer_stack_detail::UnderlyingStack<Layer::HandlePostUpdate> post_update_;
        _layer_stack_detail::UnderlyingStack<Layer::HandleRender> render_;
        _layer_stack_detail::UnderlyingStack<Layer::HandleGuiRender> gui_render_;
        _layer_stack_detail::UnderlyingStack<Layer::HandleEvent> event_;
        _layer_stack_detail::UnderlyingStack<Layer::HandleTick> tick_;
    };
} // namespace lighten::core
#include "layer-stack.inl"