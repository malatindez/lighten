#include "application.hpp"

#include "core/events.hpp"

#include <memory>
#include <thread>
#include <numeric>

namespace engine::core
{
  using namespace events;
  std::unique_ptr<Application> Application::application_;

  void Application::OnEvent(Event &e)
  {
    if (e.handled)
    {
      return;
    }

    for (auto const &layer : application_->layers_)
    {
      layer->OnEvent(e);
      if (e.handled)
      {
        return;
      }
    }
  }

  void Application::Init()
  {
    if (application_)
    {
      return;
    }
    application_ = std::unique_ptr<Application>(new Application{});
  }

  void Application::Exit() { application_->running_ = false; }
  
  void Application::Run()
  {
    render.reset();
    tick.reset();
    while (running_)
    {
      AppUpdateEvent update;
      Application::OnEvent(update);
      assert(!update.handled);

      if (tick.elapsed() > kTickDuration)
      {
        AppTickEvent tick_event(tick.elapsed());
        tick.reset();
        Application::OnEvent(tick_event);
        assert(!tick_event.handled);
      }

      if (render.elapsed() > kFrameDuration)
      {
        AppRenderEvent render_event;
        render.reset();
        Application::OnEvent(render_event);
        assert(!render_event.handled);
        static std::vector<float> last_100_frames(100);
        static int frame_num = 0;
        last_100_frames[frame_num % 100] = render.elapsed();
        frame_num++;
        OutputDebugStringA((std::to_string(100 / std::accumulate(last_100_frames.begin(), last_100_frames.end(), 0.0f)) + "\n").c_str());
      }
      std::this_thread::yield();
    }
  }
  Application::Application()
  {
    event_function_ = std::bind_front(&Application::OnEvent);
  }

} // namespace engine::core