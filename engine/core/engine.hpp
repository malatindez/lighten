#pragma once
#include "events.hpp"
#include "include/pch.hpp"
#include "layers/layer-stack-threadsafe.hpp"
#include "misc/ini.hpp"
#include "utils/utils.hpp"

namespace engine::core
{

    const core::math::ivec2 kWindowPosition { 0 };
    const core::math::ivec2 kWindowResolution { 640, 360 };

    const float kFpsLimit = 60.0f;
    const float kFrameDuration = 1.0f / kFpsLimit;

    const float kTickrate = 120.0f;
    const float kTickDuration = 1.0f / kTickrate;

    const core::math::vec3 kSphereCoords { 0, 0, -1 };
    const float kSphereRadius { 0.5f };

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
        [[nodiscard]] static inline ini::Ini &config() { return *application_->config_; }
        [[nodiscard]] static inline float TimeFromStart() { return from_start_.elapsed(); }

        static void Exit();

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

        bool running_ = true;

        utils::SteadyTimer render_;
        utils::SteadyTimer tick_;

        EventCallbackFn event_function_;

        std::unique_ptr<ini::Ini> config_;
        std::shared_ptr<spdlog::logger> logger_;

        static std::unique_ptr<Engine> application_;
        static utils::SteadyTimer from_start_;
        friend INT WINAPI::wWinMain(HINSTANCE, HINSTANCE, PWSTR, int cmd_show);
    };
} // namespace engine::core