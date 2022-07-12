#include "application.hpp"

#include "core/events.hpp"

#include <memory>
#include <numeric>
#include <thread>

static std::string const kDefaultConfig =
    R"(
[Logger]
is_absolute = no
folder = logs
)";

namespace engine::core
{
    using namespace events;
    std::unique_ptr<Application> Application::application_;
    SteadyTimer Application::from_start_;

    void Application::OnEvent(Event &e)
    {
        if (e.handled)
        {
            return;
        }

        for (auto const &layer : application_->layers_)
        {
            layer->OnEvent(e);
            if (e.handled)
            {
                return;
            }
        }
    }

    void Application::Init()
    {
        if (application_)
        {
            return;
        }
        application_ = std::unique_ptr<Application>(new Application{});
    }

    void Application::Exit() { application_->running_ = false; }

    void Application::Run()
    {
        render_.reset();
        tick_.reset();
        while (running_)
        {
            AppUpdateEvent update;
            Application::OnEvent(update);
            assert(!update.handled);

            if (tick_.elapsed() > kTickDuration)
            {
                AppTickEvent tick_event(tick_.elapsed());
                tick_.reset();
                Application::OnEvent(tick_event);
                assert(!tick_event.handled);
            }

            if (render_.elapsed() > kFrameDuration)
            {
                AppRenderEvent render_event;
                render_.reset();
                Application::OnEvent(render_event);
                assert(!render_event.handled);
                static std::vector<float> last_100_frames(100);
                static int frame_num = 0;
                last_100_frames[frame_num % 100] = render_.elapsed();
                frame_num++;
                logger_->info((std::to_string(100 / std::accumulate(last_100_frames.begin(), last_100_frames.end(), 0.0f)) + "\n"));
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
        logger_ = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", y);
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