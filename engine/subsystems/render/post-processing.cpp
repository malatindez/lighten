#include "post-processing.hpp"
namespace engine::render
{
    PostProcessing::~PostProcessing()
    {
        for (auto &layer : all_)
        {
            layer->OnDetach();
        }
    }

    void PostProcessing::OnEvent(core::events::Event &e)
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
    void PostProcessing::OnUpdate()
    {
        for (auto &layer : update_)
        {
            layer->OnUpdate();
        }
    }
    void PostProcessing::OnRender()
    {
        for (auto &layer : render_)
        {
            layer->OnRender();
        }
    }
    void PostProcessing::OnGuiRender()
    {
        for (auto &layer : gui_render_)
        {
            layer->OnGuiRender();
        }
    }
    void PostProcessing::OnTick(float delta_time)
    {
        for (auto &layer : tick_)
        {
            layer->OnTick(delta_time);
        }
    }
    void PostProcessing::OnFrameBegin()
    {
        for (auto &layer : frame_begin_)
        {
            layer->OnFrameBegin();
        }
    }
    void PostProcessing::OnFrameEnd(direct3d::RenderTargetBase &target)
    {
        for (auto &layer : frame_end_)
        {
            layer->OnFrameEnd(target);
        }
    }
    direct3d::RenderTargetBase &PostProcessing::OnProcess(direct3d::RenderTargetBase &source)
    {
        direct3d::RenderTargetBase *current = &source;
        for (auto &layer : process_)
        {
            current = &layer->OnProcess(*current);
        }
        return *current;
    }
}