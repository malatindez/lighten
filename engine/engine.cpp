#include "engine.hpp"
#include <chrono>
#include <thread>
namespace engine
{
    std::unique_ptr<Engine> Engine::engine_;

    using namespace engine::math;

    INT Engine::Start()
    {
        MainLoop();
        return 0;
    }

    void Engine::Exit()
    {
        running_ = false;
    }

    void Engine::MainLoop()
    {
        using namespace std::chrono;
        using namespace std::this_thread;
        while (running_)
        {
            UpdateDelta();
            Update();

            // limit fps
            if (delta_time_ < 1000000.0f / kFpsLimit)
            {
                sleep_for(microseconds(size_t(1000000.0f / kFpsLimit - delta_time_)));
            }
        }
    }

    void Engine::UpdateDelta()
    {
        using namespace std::chrono;
        const time_point<steady_clock> now = steady_clock::now();

        delta_time_ =
            float(duration_cast<microseconds>(now - last_time_point_).count()) /
            1e6f;

        last_time_point_ = now;
    }

    void Engine::Update()
    {
        for (auto it = update_list_.begin(); it != update_list_.end();)
        {
            if ((*it)->Update(delta_time_))
            {
                ++it;
            }
            else
            {
                it = update_list_.erase(it);
            }
        }
    }

} // namespace engine