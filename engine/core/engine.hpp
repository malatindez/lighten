#pragma once
#include "include/lighten-pch.hpp"
#include "layers.hpp"
#include "misc/ini.hpp"
#include "core/scene.hpp"
#include "subsystems/core/core.hpp"
#include "subsystems/render/render.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
#include "world.hpp"
namespace lighten::core
{
    // TODO: move to config file
    const glm::ivec2 kWindowPosition{0};
    const glm::ivec2 kWindowResolution{640, 360};

    const float kUpdateLimit = 1e9f;
    const float kFpsLimit = 150.0f;
    const float kTickrate = 150.0f;

    const glm::vec3 kSphereCoords{0, 0, -1};
    const float kSphereRadius{0.5f};

    /**
     * @brief Represents the core of the lighten
     *
     * This class is a singleton and is responsible for the main loop of the lighten.
     * It also holds the LayerStack and the EventFunction.
     *
     */
    class Engine final : private LayerStackThreadsafe
    {
    public:
        ~Engine();
        using LayerStackThreadsafe::PopLayer;
        using LayerStackThreadsafe::PopOverlay;
        using LayerStackThreadsafe::PushLayer;
        using LayerStackThreadsafe::PushOverlay;
        /**
         * @brief Get the Engine reference
         *
         * @note This function is not thread safe
         *
         * @return Engine&
         */
        [[nodiscard]] static inline Engine &Get() noexcept { return *application_; }

        /**
         * @brief Get the Event Function reference
         *
         * Returns the event function that is on a highest place in the tree.
         *
         * @return EventCallbackFn const&
         */
        [[nodiscard]] static inline EventCallbackFn const &event_function() { return application_->event_function_; }
        /**
         * @brief Get the logger reference
         *
         * Returns a logger that is used by the lighten.
         * This logger is used by the lighten itself and all the subsystems.
         * All the logs are written to the file located at the path specified in the config file.
         *
         * @return spdlog::logger&
         */
        [[nodiscard]] static inline spdlog::logger &logger() { return *application_->logger_; }

        /**
         * @brief Get the World pointer
         *
         * Returns the pointer to the world.
         *
         * @return std::shared_ptr<World>
         */

        [[nodiscard]] static inline std::shared_ptr<World> world() { return application_->world_; }

        /**
         * @brief Get the Time From Start of the lighten
         *
         * Returns the time elapsed since the lighten was started.
         *
         * @return float
         */

        [[nodiscard]] static inline float TimeFromStart() { return from_start_.elapsed(); }

        /**
         * @brief Stops the lighten entirely
         */
        static void Exit();

        /**
         * @brief Set the tickrate value
         *
         * @note This function is not thread safe
         *
         * @param tickrate
         */
        static void SetMaximumTickrate(float tickrate)
        {
            application_->tickrate_ = tickrate;
            application_->tick_duration_ = 1.0f / tickrate;
        }

        /**
         * @brief Set the maximum framerate value
         *
         * @note This function is not thread safe
         *
         * @param framerate
         */
        static void SetMaximumFramerate(float framerate)
        {
            application_->framerate_ = framerate;
            application_->frame_duration_ = 1.0f / framerate;
        }

        /**
         * @brief Set the maximum update rate value
         *
         * @note This function is not thread safe
         *
         * @param updaterate
         */
        static void SetMaximumUpdateRate(float update_rate)
        {
            application_->update_rate_ = update_rate;
            application_->update_duration_ = 1.0f / update_rate;
        }

        /**
         * @brief Get the tickrate value
         *
         * @note This function is not thread safe
         *
         * @return float
         */
        [[nodiscard]] static inline float maximum_tickrate() { return application_->tickrate_; }

        /**
         * @brief Get the maximum framerate value
         *
         * @note This function is not thread safe
         *
         * @return float
         */
        [[nodiscard]] static inline float maximum_framerate() { return application_->framerate_; }

        /**
         * @brief Get the maximum update rate value
         *
         * @note This function is not thread safe
         *
         * @return float
         */
        [[nodiscard]] static inline float maximum_update_rate() { return application_->update_rate_; }

#ifdef _DEBUG
        mal_toolkit::Measurer<std::chrono::high_resolution_clock> update_measurer{"Engine::Update()", false};
        mal_toolkit::Measurer<std::chrono::high_resolution_clock> render_measurer{"Engine::Render()", false};
        mal_toolkit::Measurer<std::chrono::high_resolution_clock> tick_measurer{"Engine::Tick()", false};
#endif

        [[nodiscard]] static uint64_t random_seed()
        {
            try
            {
                std::random_device rd;
                return rd();
            }
            catch ([[maybe_unused]] const std::exception &e)
            {
                return std::chrono::system_clock::now().time_since_epoch().count();
            }
        }

        [[nodiscard]] static inline std::mt19937 &random_engine() { return *application_->random_engine_; }

    private:
        /**
         * @brief Initializes the lighten
         *
         * Should be called in the main function before the lighten starts.
         * Creates an instance_ of an lighten and initializes all the subsystems.
         *
         */
        static void Init();

        /**
         * @brief Deinitializes the lighten
         *
         * Should be called in the main function after the lighten returns control.
         *
         */
        static void Deinit();

        /**
         * @brief Starts the lighten
         *
         * You should initialize and push all the subsystems after Init() but before calling this function.
         * Should be called in the main function after the lighten is initialized.
         * Starts the lighten and runs it until the lighten is stopped.
         *
         */
        void Run();

        Engine();
        // delete move & copy semantics
        Engine(Engine &&) = delete;
        Engine(Engine const &) = delete;
        Engine &operator=(Engine &&) = delete;
        Engine &operator=(Engine const &) = delete;
        std::unique_ptr<std::mt19937> random_engine_;

        std::shared_ptr<World> world_ = nullptr;

        bool running_ = true;

        mal_toolkit::SteadyTimer update_;
        mal_toolkit::SteadyTimer render_;
        mal_toolkit::SteadyTimer tick_;

        EventCallbackFn event_function_;

        std::shared_ptr<spdlog::logger> logger_;

        static std::unique_ptr<Engine> application_;
        static mal_toolkit::SteadyTimer from_start_;

        float update_rate_ = kUpdateLimit;
        float tickrate_ = kTickrate;
        float framerate_ = kFpsLimit;
        float update_duration_ = 1.0f / kUpdateLimit;
        float frame_duration_ = 1.0f / kFpsLimit;
        float tick_duration_ = 1.0f / kTickrate;
        friend INT WINAPI::wWinMain(HINSTANCE, HINSTANCE, PWSTR, int cmd_show);
    };
} // namespace lighten::core