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
        using namespace std::chrono_literals;
        using namespace std::this_thread;
        while (running_)
        {
            using namespace std::chrono;
            const time_point<steady_clock> now = steady_clock::now();

            delta_time_ = duration_cast<duration<float>>(now - last_time_point_).count();
            
            // limit fps
            while (delta_time_ < kFrameDuration)
            {
                sleep_for(100us);
                delta_time_ = duration_cast<duration<float>>(steady_clock::now() - last_time_point_).count();
            }
            
            Update();
            
            last_time_point_ = now;
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

} // namespace engine