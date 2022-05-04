#include "engine.hpp"
#include <chrono>
#include <thread>
namespace engine
{
    std::unique_ptr<Engine> Engine::engine_;

    using namespace engine::math;
    using namespace std::chrono;
    using namespace std::chrono_literals;
    using namespace std::this_thread;

    INT Engine::Start()
    {
        MainLoop();
        return 0;
    }

    void Engine::Exit()
    {
        running_ = false;
    }

    bool Engine::FrameTimeElapsed() 
    {
        const time_point<steady_clock> now = steady_clock::now();

        delta_time_ = duration_cast<duration<float>>(steady_clock::now() - last_time_point_).count();

        if (delta_time_ >= kFrameDuration) {
            last_time_point_ = now;
            return true;
        }
        return false;
    }

    void Engine::MainLoop()
    {
        while (running_)
        {

            PeekOSMessages();

            if (FrameTimeElapsed()) {
                Update();
            }

            std::this_thread::yield();
        }
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

    void Engine::PeekOSMessages()
    {
        for (auto it = windows_.begin(); it != windows_.end();)
        {
            if ((*it)->PeekOSMessages())
            {
                ++it;
            }
            else
            {
                it = windows_.erase(it);
            }
        }
    }

} // namespace engine