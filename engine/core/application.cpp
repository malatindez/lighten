#include "application.hpp"

#include "core/events.hpp"

#include <memory>
#include <thread>


namespace engine {
std::unique_ptr<Application> Application::application_;

void Application::OnEvent(Event &e) {
  for (auto it = application_->layers_.begin();
       it != application_->layers_.end(); ++it) {
    (*it)->OnEvent(e);
    if (e.handled) {
      return;
    }
  }
}

void Application::Init() {
  if (application_) {
    return;
  }
  application_ = std::unique_ptr<Application>(new Application{});
}

void Application::Exit() { application_->running_ = false; }

void Application::Run() {
  render.reset();
  tick.reset();
  while (running_) {
    AppUpdateEvent update;
    Application::OnEvent(update);
    assert(!update.handled);

    if (tick.elapsed() > kTickDuration) {
      AppTickEvent tick_event(tick.elapsed());
      tick.reset();
      Application::OnEvent(tick_event);
      assert(!tick_event.handled);
    }

    if (render.elapsed() > kFrameDuration) {
      AppRenderEvent render_event;
      render.reset();
      Application::OnEvent(render_event);
      assert(!render_event.handled);
    }
    std::this_thread::yield();
  }
}

} // namespace engine