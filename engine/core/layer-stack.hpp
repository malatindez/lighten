#pragma once
#include "layer.hpp"
#include "include/spdlog.hpp"
#include <memory>
#include <vector>

namespace engine::core::_detail
{
    using LayerPtrVec = std::vector<std::shared_ptr<Layer>>;
    class UnderlyingStack
    {
    public:
        void PushLayer(std::shared_ptr<Layer> layer);
        void PopLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);
        void PopOverlay(std::shared_ptr<Layer> overlay);
        bool HasLayer(std::shared_ptr<Layer> layer);

        LayerPtrVec::iterator begin() { return layers_.begin(); }
        LayerPtrVec::iterator end() { return layers_.end(); }
        LayerPtrVec::reverse_iterator rbegin() { return layers_.rbegin(); }
        LayerPtrVec::reverse_iterator rend() { return layers_.rend(); }

        LayerPtrVec::const_iterator begin() const { return layers_.begin(); }
        LayerPtrVec::const_iterator end() const { return layers_.end(); }
        LayerPtrVec::const_reverse_iterator rbegin() const { return layers_.rbegin(); }
        LayerPtrVec::const_reverse_iterator rend() const { return layers_.rend(); }

    private:
        LayerPtrVec layers_;
        uint32_t layer_insert_index_ = 0;
    };
} // namespace engine::core::_detail
namespace engine::core
{
    class LayerStack : Layer
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
        bool HasLayer(std::shared_ptr<T>);

        void OnUpdate() override;
        void OnRender() override;
        void OnGuiRender() override;
        void OnEvent(events::Event &e) override;
        void OnTick(float delta_time) override;
    private:
        std::vector<std::shared_ptr<Layer>> all_;
        _detail::UnderlyingStack update_;
        _detail::UnderlyingStack render_;
        _detail::UnderlyingStack gui_render_;
        _detail::UnderlyingStack event_;
        _detail::UnderlyingStack tick_;
    };
} // namespace engine::core
#include "layer-stack.inl"