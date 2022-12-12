#pragma once
#include "include/engine-pch.hpp"
#include "layers.hpp"
#include "misc/ini.hpp"
#include "core/scene.hpp"
#include "subsystems/core/core.hpp"
#include "subsystems/render/render.hpp"

namespace engine::core
{
    // TODO: move to config file
    const core::math::ivec2 kWindowPosition{ 0 };
    const core::math::ivec2 kWindowResolution{ 640, 360 };

    const float kFpsLimit = 60.0f;
    const float kFrameDuration = 1.0f / kFpsLimit;

    const float kTickrate = 120.0f;
    const float kTickDuration = 1.0f / kTickrate;

    const core::math::vec3 kSphereCoords{ 0, 0, -1 };
    const float kSphereRadius{ 0.5f };

    /**
     * @brief Represents the core of the engine
     * 
     * This class is a singleton and is responsible for the main loop of the engine.
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
         * Returns a logger that is used by the engine.
         * This logger is used by the engine itself and all the subsystems.
         * All the logs are written to the file located at the path specified in the config file.
         * 
         * @return spdlog::logger& 
         */
        [[nodiscard]] static inline spdlog::logger &logger() { return *application_->logger_; }

        /**
         * @brief Get the Scene pointer
         * 
         * Returns the pointer to the current selected scene.
         * 
         * @return std::shared_ptr<Scene> 
         */

        [[nodiscard]] static inline std::shared_ptr<Scene> scene() { return application_->scene_; }

        /**
         * @brief Get the Time From Start of the engine
         * 
         * Returns the time elapsed since the engine was started.
         * 
         * @return float 
         */

        [[nodiscard]] static inline float TimeFromStart() { return from_start_.elapsed(); }

        /**
         * @brief Stops the engine entirely
         */
        static void Exit();
        /**
         * @brief Set the Scene object
         * 
         * @param scene Scene that will be set as the current scene.
         */
        static void SetScene(std::shared_ptr<Scene> scene) { application_->scene_ = scene; }

    private:
        /**
         * @brief Initializes the engine
         * 
         * Should be called in the main function before the engine starts.
         * Creates an instance_ of an engine and initializes all the subsystems.
         * 
         */
        static void Init();

        /**
         * @brief Deinitializes the engine
         * 
         * Should be called in the main function after the engine returns control.
         * 
         */
        static void Deinit();

        /**
         * @brief Starts the engine
         * 
         * You should initialize and push all the subsystems after Init() but before calling this function.
         * Should be called in the main function after the engine is initialized.
         * Starts the engine and runs it until the engine is stopped.
         * 
         */
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