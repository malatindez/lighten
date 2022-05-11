#pragma once
#include "interfaces/updatable.hpp"
#include "math/sphere.hpp"
#include "core/bitmap-window.hpp"
#include "render/scene.hpp"
#include "pch.hpp"

#include <chrono>

namespace engine
{

    const math::ivec2 kWindowPosition{0};
    const math::ivec2 kWindowResolution{1280, 720};

    const float kFpsLimit = 60.0f;
    const float kFrameDuration = 1.0f / kFpsLimit;

    const math::vec3 kSphereCoords{0, 0, -1};
    const float kSphereRadius{0.5f};

    class Application final
    {
    public:
        [[nodiscard]] static inline Application &engine() noexcept
        {
            if (engine_)
            {
                return *engine_;
            }

            engine_ = std::unique_ptr<Application>(new Engine{});

            return *engine_;
        }

        INT Start();

        void Exit();

        template <typename T>
        void AddUpdatable(std::shared_ptr<T> const &updatable)
        {
            update_list_.push_back(std::static_pointer_cast<interfaces::Updatable>(updatable));
        }
        template <typename T>
        void AddWindow(std::shared_ptr<T> const &window)
        {
            windows_.push_back(std::static_pointer_cast<Window>(window));
        }

        [[nodiscard]] constexpr float delta_time() const noexcept
        {
            return delta_time_;
        }

    private:
        Application() = default;
        // delete move & copy semantics
        Application(Application &&Engine) = delete;
        Application(Application const &Engine) = delete;
        Application &operator=(Application &&Engine) = delete;
        Application &operator=(Application const &Engine) = delete;

        bool FrameTimeElapsed();

        void MainLoop();

        void Update();

        void PeekOSMessages();

        bool running_ = true;

        // time from previous update
        std::chrono::time_point<std::chrono::steady_clock> last_time_point_;
        float delta_time_ = 0;

        std::vector<std::shared_ptr<Window>> windows_;

        std::vector<std::shared_ptr<interfaces::Updatable>> update_list_;

        static std::unique_ptr<Application> engine_;
    };
    [[nodiscard]] inline Application &GetEngine() { return Application::engine(); }
} // namespace engine