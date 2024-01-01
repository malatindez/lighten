#include "engine.hpp"
#include "include/win-debug.hpp"

namespace engine::core
{
    using namespace events;
    std::unique_ptr<Engine> Engine::application_;
    mal_toolkit::SteadyTimer Engine::from_start_;

    void Engine::Init()
    {
        if (application_)
        {
            return;
        }
        Config::Init();

        application_ = std::unique_ptr<Engine>(new Engine{});
        debug::RedirectOutputDebugString([&](std::string_view view) __lambda_force_inline
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

        direct3d::Api::Init();
        InputLayer::Init();
        ShaderManager::Init();
        ModelLoader::Init();
        SkyboxManager::Init();
        TextureManager::Init();
        render::ModelSystem::Init();
    }

    void Engine::Deinit()
    {
        application_->logger_->flush();
        application_ = nullptr;
        render::ModelSystem::Deinit();
        TextureManager::Deinit();
        SkyboxManager::Deinit();
        ModelLoader::Deinit();
        ShaderManager::Deinit();
        InputLayer::Deinit();
        direct3d::Api::Deinit();
        Config::Deinit();
    }
    void Engine::Exit()
    {
        application_->running_ = false;
        application_->logger_->flush();
    }
    void Engine::Run()
    {
        update_.reset();
        render_.reset();
        tick_.reset();
#ifndef _DEBUG
        try
        {
#endif
            while (running_)
            {
                if (update_.elapsed() > update_duration_)
                {
                    update_.reset();
#ifdef _DEBUG
                    update_measurer.begin();
#endif
                    OnUpdate();
#ifdef _DEBUG
                    update_measurer.end();
#endif
                }
                if (!running_)
                {
                    break;
                }
                if (tick_.elapsed() > tick_duration_)
                {
                    float dt = tick_.elapsed();
                    tick_.reset();
#ifdef _DEBUG
                    tick_measurer.begin();
#endif
                    OnTick(dt);
#ifdef _DEBUG
                    tick_measurer.end();
#endif
                }
                if (render_.elapsed() > frame_duration_)
                {
                    render_.reset();
#ifdef _DEBUG
                    render_measurer.begin();
#endif
                    OnRender();
#ifdef _DEBUG
                    render_measurer.end();
#endif
                }
                direct3d::LogDebugInfoQueue();

                std::this_thread::yield();
            }
#ifndef _DEBUG
        }
        catch (std::exception e)
        {
            spdlog::critical(e.what());
            spdlog::critical("Exception occurred within the engine layers. Shutting down.");
            Exit();
        }
        catch (...)
        {
            spdlog::critical("Unknown exception occurred within the engine layers. Shutting down.");
            Exit();
        }
#endif
    }
    Engine::Engine()
    {
        random_engine_ = std::make_unique<std::mt19937>(static_cast<uint32_t>(random_seed()));
        event_function_ = std::bind_front([this](Event &e) __lambda_force_inline
                                          { OnEvent(e); });
        std::filesystem::path target_folder;
        if (!config()["Logger"]["is_absolute"].as_boolean())
        {
            target_folder = std::filesystem::current_path() / config()["Logger"]["folder"].str();
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
                std::streampos fsize = log.tellg();
                log.seekg(0, std::ios::end);
                fsize = log.tellg() - fsize;
                log.seekg(0, std::ios::beg);
                if (fsize <= 25)
                {
                    break;
                }
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

    Engine::~Engine()
    {
    }
} // namespace engine::core