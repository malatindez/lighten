#pragma once
#include "interfaces/updatable.hpp"
#include "math/sphere.hpp"
#include "misc/bitmap-window.hpp"
#include "misc/scene.hpp"
#include "pch.hpp"

#include <chrono>

namespace engine
{

    const math::ivec2 kWindowPosition{0};
    const math::ivec2 kWindowResolution{1280, 720};

    const float kFpsLimit = 60.0f;

    const math::vec3 kSphereCoords{0, 0, -1};
    const float kSphereRadius{0.5f};

    class Engine final
    {
    public:
        [[nodiscard]] static inline Engine &engine() noexcept
        {
            if (engine_)
            {
                return *engine_;
            }

            engine_ = std::unique_ptr<Engine>(new Engine{});

            return *engine_;
        }

        INT Start();

        void Exit();

        template <typename T>
        void AddUpdatable(std::shared_ptr<T> const &updatable)
        {
            update_list_.push_back(std::static_pointer_cast<interfaces::Updatable>(updatable));
        }

        [[nodiscard]] constexpr float delta_time() const noexcept
        {
            return delta_time_;
        }

    private:
        Engine() = default;
        // delete move & copy semantics
        Engine(Engine &&Engine) = delete;
        Engine(Engine const &Engine) = delete;
        Engine &operator=(Engine &&Engine) = delete;
        Engine &operator=(Engine const &Engine) = delete;

        void MainLoop();

        void UpdateDelta();

        void Update();

        bool running_ = true;

        // time from previous update
        std::chrono::time_point<std::chrono::steady_clock> last_time_point_;
        float delta_time_ = 0;

        std::vector<std::shared_ptr<interfaces::Updatable>> update_list_;

        static std::unique_ptr<Engine> engine_;
    };
    [[nodiscard]] inline Engine &GetEngine() { return Engine::engine(); }
} // namespace engine