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

    class Application final : private LayerStackThreadsafe
    {
    public:
        ~Application();
        using LayerStackThreadsafe::PopLayer;
        using LayerStackThreadsafe::PopOverlay;
        using LayerStackThreadsafe::PushLayer;
        using LayerStackThreadsafe::PushOverlay;

        [[nodiscard]] static inline Application &Get() noexcept { return *application_; }
        [[nodiscard]] static inline EventCallbackFn const &event_function() { return application_->event_function_; }
        [[nodiscard]] static inline spdlog::logger &logger() { return *application_->logger_; }
        [[nodiscard]] static inline ini::Ini &config() { return *application_->config_; }
        [[nodiscard]] static inline float TimeFromStart() { return from_start_.elapsed(); }

        static void Exit();

        static void OnEvent(events::Event &e) { application_->LayerStackThreadsafe::OnEvent(e); }

    private:
        static void Init();
        static void Deinit();

        void Run();

        Application();
        // delete move & copy semantics
        Application(Application &&Application) = delete;
        Application(Application const &Application) = delete;
        Application &operator=(Application &&Application) = delete;
        Application &operator=(Application const &Application) = delete;

        bool running_ = true;

        utils::SteadyTimer render_;
        utils::SteadyTimer tick_;

        EventCallbackFn event_function_;

        std::unique_ptr<ini::Ini> config_;
        std::shared_ptr<spdlog::logger> logger_;

        static std::unique_ptr<Application> application_;
        static utils::SteadyTimer from_start_;
        friend INT WINAPI::wWinMain(HINSTANCE, HINSTANCE, PWSTR, int cmd_show);
    };
} // namespace engine::core