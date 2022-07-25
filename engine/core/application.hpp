#pragma once
#include "core/timer.hpp"
#include "core/utils.hpp"
#include "events.hpp"
#include "layer.hpp"
#include "misc/ini.hpp"
#include "pch.hpp"
#include <chrono>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace engine::core
{

    const core::math::ivec2 kWindowPosition{0};
    const core::math::ivec2 kWindowResolution{640, 360};

    const float kFpsLimit = 60.0f;
    const float kFrameDuration = 1.0f / kFpsLimit;

    const float kTickrate = 120.0f;
    const float kTickDuration = 1.0f / kTickrate;

    const core::math::vec3 kSphereCoords{0, 0, -1};
    const float kSphereRadius{0.5f};

    class Application final
    {
    public:
        ~Application();


        [[nodiscard]] static inline Application &Get() noexcept
        {
            return *application_;
        }
        [[nodiscard]] static inline EventCallbackFn const &event_function()
        {
            return application_->event_function_;
        }
        [[nodiscard]] static inline spdlog::logger &logger()
        {
            return *application_->logger_;
        }
        [[nodiscard]] static inline ini::Ini &config()
        {
            return *application_->config_;
        }
        static void Exit();

        static void OnEvent(events::Event &e);

        template <class T>
        inline void AddLayer(std::shared_ptr<T> t)
        {
            layers_.push_back(std::static_pointer_cast<Layer>(t));
        }

        [[nodiscard]] static inline float TimeFromStart()
        {
            return from_start_.elapsed();
        }

    private:
        static void Init();

        void Run();

        Application();
        // delete move & copy semantics
        Application(Application &&Application) = delete;
        Application(Application const &Application) = delete;
        Application &operator=(Application &&Application) = delete;
        Application &operator=(Application const &Application) = delete;

        bool running_ = true;

        SteadyTimer render_;
        SteadyTimer tick_;

        std::vector<std::shared_ptr<Layer>> layers_;

        EventCallbackFn event_function_;

        std::unique_ptr<ini::Ini> config_;
        std::shared_ptr<spdlog::logger> logger_;

        static std::unique_ptr<Application> application_;
        static SteadyTimer from_start_;
        friend INT WINAPI ::wWinMain(HINSTANCE, HINSTANCE, PWSTR, int cmd_show);
    };
} // namespace engine::core