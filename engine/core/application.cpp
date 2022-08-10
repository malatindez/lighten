#include "application.hpp"
#include "direct3d11/direct3d11.hpp"
#include "include/win-debug.hpp"

#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static std::string const kDefaultConfig =
R"(
[Logger]
is_absolute = no
folder = logs
console_enabled = true
log_level = trace
)";

namespace engine::core
{
    using namespace events;
    std::unique_ptr<Application> Application::application_;
    utils::SteadyTimer Application::from_start_;

    void Application::Init()
    {
        if (application_)
        {
            return;
        }
        application_ = std::unique_ptr<Application>(new Application {});
        debug::RedirectOutputDebugString([&] (std::string_view view)
                                         { application_->logger_->info(view); });
        if (config()["Logger"]["console_enabled"].as_boolean())
        {
            AllocConsole();
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
            auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
            stdout_sink->set_level(spdlog::level::trace);
            logger().sinks().push_back(stdout_sink);
            logger().set_level(spdlog::level::trace);
        }
        spdlog::set_default_logger(application_->logger_);

        auto func = [] (LPEXCEPTION_POINTERS) -> LONG
        {
            for (auto const &sink : logger().sinks())
            {
                sink->flush();
            }
            return EXCEPTION_EXECUTE_HANDLER;
        };

        direct3d::Init();
    }

    void Application::Deinit()
    {
        application_->logger_->flush();
        application_ = nullptr;
        direct3d::Deinit();
    }
    void Application::Exit()
    {
        application_->running_ = false;
        application_->logger_->flush();
    }
    void Application::Run()
    {
        render_.reset();
        tick_.reset();
        while (running_)
        {
            OnUpdate();

            if (tick_.elapsed() > kTickDuration)
            {
                float dt = tick_.elapsed();
                tick_.reset();
                OnTick(dt);
            }

            if (render_.elapsed() > kFrameDuration)
            {
                render_.reset();
                OnRender();
            }
            std::this_thread::yield();
        }
    }
    Application::Application()
    {
        event_function_ = std::bind_front(&Application::OnEvent);
        std::ifstream config("config.ini");
        std::stringstream data;
        if (config.is_open())
        {
            data << config.rdbuf();
        }
        else
        {
            data << kDefaultConfig;
        }
        config.close();
        config_ = std::make_unique<ini::Ini>(data.str());
        std::filesystem::path target_folder;
        if (!(*config_)["Logger"]["is_absolute"].as_boolean())
        {
            target_folder = std::filesystem::current_path() / (*config_)["Logger"]["folder"].str();
        }
        if (!std::filesystem::is_directory(target_folder) && !std::filesystem::exists(target_folder))
        {
            std::filesystem::create_directories(target_folder);
        }
        for (auto const &file : std::filesystem::directory_iterator(target_folder))
        {
            if (file.path().filename() == "latest.log")
            {
                std::ifstream log(file.path());
                char buffer[25];
                log.read(buffer, 24);
                buffer[24] = '\0';

                std::string date(buffer + 1);
                std::ranges::replace(date, ' ', '_');
                std::ranges::replace(date, ':', '-');
                log.close();
                std::filesystem::rename(file.path(), file.path().parent_path() / (date + ".log"));
                break;
            }
        }
        auto t = (target_folder / "latest.log");
        auto y = t.string();
        logger_ = spdlog::basic_logger_mt<spdlog::async_factory>("Engine", y);
        logger_->set_level(spdlog::level::debug);
        logger_->sinks()[0]->set_level(spdlog::level::debug);
    }

    Application::~Application()
    {
        if (config_)
        {
            std::ofstream config("config.ini");
            config << config_->Serialize();
            config.close();
        }
    }

} // namespace engine::core