#pragma once
#include "include/engine-pch.hpp"
#include "layers.hpp"
#include "misc/ini.hpp"
#include "core/scene.hpp"
#include "subsystems/core/core.hpp"
#include "subsystems/render/render.hpp"

namespace engine::core
{

    const core::math::ivec2 kWindowPosition{ 0 };
    const core::math::ivec2 kWindowResolution{ 640, 360 };

    const float kFpsLimit = 60.0f;
    const float kFrameDuration = 1.0f / kFpsLimit;

    const float kTickrate = 120.0f;
    const float kTickDuration = 1.0f / kTickrate;

    const core::math::vec3 kSphereCoords{ 0, 0, -1 };
    const float kSphereRadius{ 0.5f };

    class Engine final : private LayerStackThreadsafe
    {
    public:
        ~Engine();
        using LayerStackThreadsafe::PopLayer;
        using LayerStackThreadsafe::PopOverlay;
        using LayerStackThreadsafe::PushLayer;
        using LayerStackThreadsafe::PushOverlay;

        [[nodiscard]] static inline Engine &Get() noexcept { return *application_; }
        [[nodiscard]] static inline EventCallbackFn const &event_function() { return application_->event_function_; }
        [[nodiscard]] static inline spdlog::logger &logger() { return *application_->logger_; }

        [[nodiscard]] static inline std::shared_ptr<Scene> scene() { return application_->scene_; }

        [[nodiscard]] static inline float TimeFromStart() { return from_start_.elapsed(); }

        static void Exit();

        static void SetScene(std::shared_ptr<Scene> scene) { application_->scene_ = scene; }

    private:
        static void Init();
        static void Deinit();

        void Run();

        Engine();
        // delete move & copy semantics
        Engine(Engine &&) = delete;
        Engine(Engine const &) = delete;
        Engine &operator=(Engine &&) = delete;
        Engine &operator=(Engine const &) = delete;

        std::shared_ptr<Scene> scene_ = nullptr;

        bool running_ = true;

        utils::SteadyTimer render_;
        utils::SteadyTimer tick_;

        EventCallbackFn event_function_;

        std::shared_ptr<spdlog::logger> logger_;

        static std::unique_ptr<Engine> application_;
        static utils::SteadyTimer from_start_;
        friend INT WINAPI::wWinMain(HINSTANCE, HINSTANCE, PWSTR, int cmd_show);
    };
} // namespace engine::core