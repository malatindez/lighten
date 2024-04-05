#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/layers/layer-stack.hpp"
namespace lighten::render
{
    class PostProcessingLayer : public core::Layer
    {
    public:
        explicit PostProcessingLayer(int64_t weight) : core::Layer("Post processing"), kWeight(weight) {}
        virtual ~PostProcessingLayer() = default;
        PostProcessingLayer(PostProcessingLayer const &) = delete;
        PostProcessingLayer &operator=(PostProcessingLayer const &) = delete;
        PostProcessingLayer(PostProcessingLayer &&) = delete;
        PostProcessingLayer &operator=(PostProcessingLayer &&) = delete;

        class HandleFrameBegin
        {
        public:
            virtual void OnFrameBegin() = 0;
        };
        class HandleFrameEnd
        {
        public:
            virtual void OnFrameEnd(direct3d::RenderTargetBase &) = 0;
        };
        // Each layer will receive the RenderTarget that is the result of execution of the previous layer
        class HandleProcess
        {
        public:
            virtual direct3d::RenderTargetBase &OnProcess(direct3d::RenderTargetBase &source) = 0;
        };
        // Each layer has a weight, which is used to determine the order in which they are executed.
        // So if you want to execute a layer before another, you can give it a higher weight.
        // The default weight is 0, so if you want to execute a layer after another, you can give it a lower weight.
        int64_t kWeight;
    };

    namespace _post_processing_detail
    {
        using LayerPtrVec = std::vector<std::shared_ptr<PostProcessingLayer>>;
        template <typename T>
        class UnderlyingStack
        {
        public:
            using Vector = std::vector<T *>;
            void PushLayer(T *layer, int64_t weight);
            void PopLayer(T *layer);
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
            // helper to sort layers by their weights
            std::vector<int64_t> weights_;
        };
    }

    class PostProcessing
        : public PostProcessingLayer,
          public core::Layer::HandleEvent,
          public core::Layer::HandleUpdate,
          public core::Layer::HandleRender,
          public core::Layer::HandleGuiRender,
          public core::Layer::HandleTick,
          public PostProcessingLayer::HandleFrameBegin,
          public PostProcessingLayer::HandleFrameEnd,
          public PostProcessingLayer::HandleProcess
    {
    public:
        PostProcessing() : PostProcessingLayer(0) {}
        virtual ~PostProcessing();
        template <typename T>
        void AddLayer(std::shared_ptr<T> layer);
        template <typename T>
        void RemoveLayer(std::shared_ptr<T> layer);
        template <typename T>
        bool HasLayer(std::shared_ptr<T> layer);
        void OnEvent(core::events::Event &e) override;
        void OnUpdate() override;
        void OnRender() override;
        void OnGuiRender() override;
        void OnTick(float delta_time) override;
        void OnFrameBegin() override;
        void OnFrameEnd(direct3d::RenderTargetBase &) override;
        direct3d::RenderTargetBase &OnProcess(direct3d::RenderTargetBase &source) override;

    protected:
        std::vector<std::shared_ptr<PostProcessingLayer>> all_;
        _post_processing_detail::UnderlyingStack<core::Layer::HandleUpdate> update_;
        _post_processing_detail::UnderlyingStack<core::Layer::HandleRender> render_;
        _post_processing_detail::UnderlyingStack<core::Layer::HandleGuiRender> gui_render_;
        _post_processing_detail::UnderlyingStack<core::Layer::HandleEvent> event_;
        _post_processing_detail::UnderlyingStack<core::Layer::HandleTick> tick_;
        _post_processing_detail::UnderlyingStack<PostProcessingLayer::HandleFrameBegin> frame_begin_;
        _post_processing_detail::UnderlyingStack<PostProcessingLayer::HandleFrameEnd> frame_end_;
        _post_processing_detail::UnderlyingStack<PostProcessingLayer::HandleProcess> process_;
    };
}
#include "post-processing.inl"