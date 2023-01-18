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
        static void Save()
        {
            std::ofstream config("config.ini");
            config << instance().Serialize();
            config.close();
        }
        static void Init() { Load(); }
        static void Deinit() { Save(); instance_.reset(); }
        static std::unique_ptr<Config> instance_;
    };
    [[nodiscard]] inline Config &config() noexcept { return Config::instance(); }
}