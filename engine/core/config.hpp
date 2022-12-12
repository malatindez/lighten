#pragma once
#include "misc/ini.hpp"

namespace engine::core
{
    namespace _config_detail
    {
        static auto const kDefaultConfig =
            R"([Logger]
is_absolute = no
folder = logs
console_enabled = true
log_level = trace
[Direct3D]
vsync = true
[Input]
mouse_sensitivity = 0.1
[Render]
max_fps = 60
[Window]
width = 1280
height = 720
title = Engine
[Debug]
show_direct3d_debug_output = false
)";
    } // namespace _config_detail
    /**
     * @brief Config class which is used to store all the configuration data for the engine
     * 
     * @note This class is a singleton, so you can't create an instance of it, you can only access it through the instance() method
     * 
     * @note This class is final, so you can't inherit from it
     * 
     * @note This class is not copyable nor movable
     * 
     * @note This class is not thread safe
     * 
     * If you want to add a new config value, you have to add it to the config.ini 
     * file or use the methods declared in the ini::Ini class.
     */
    class Config final : public ini::Ini
    {
    public:
        [[nodiscard]] static inline Config &instance() noexcept { utils::Assert(instance_ != nullptr, "Config is not initialized!"); return *instance_; }
    private:
        Config(Config &&) = delete;
        Config(Config const &) = delete;
        Config &operator=(Config &&) = delete;
        Config &operator=(Config const &) = delete;
        using ini::Ini::Ini;
    private:
        friend class Engine;
        /**
         * @brief Load the config from the config.ini file
         * 
         * If the config.ini file doesn't exist, it will be created with the default values.
         * 
         */
        static void Load()
        {
            std::ifstream config("config.ini");
            std::stringstream data;
            if (config.is_open())
            {
                data << config.rdbuf();
            }
            else
            {
                data << _config_detail::kDefaultConfig;
            }
            config.close();
            instance_ = std::make_unique<Config>(data.view());
        }
        /**
         * @brief Save the config to the config.ini file
         * 
         * @note This method is called automatically when the engine is deinitialized
         * 
         */
        static void Save()
        {
            std::ofstream config("config.ini");
            config << instance().Serialize();
            config.close();
        }
        /**
         * @brief Initialize the config
         * 
         * @note This method is called automatically when the engine is initialized
         * 
         */
        static void Init() { Load(); }
        /**
         * @brief Deinitialize the config
         * 
         * @note This method is called automatically when the engine is deinitialized
         * 
         */
        static void Deinit() { Save(); instance_.reset(); }
        static std::unique_ptr<Config> instance_;
    };
    [[nodiscard]] inline Config &config() noexcept { return Config::instance(); }
}