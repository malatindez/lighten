#pragma once
#include "core/events.hpp"
#include "core/layers/layer.hpp"
#include "include/pch.hpp"
namespace engine::utils
{
    class FileWatcher : public core::Layer
    {
    public:
        explicit FileWatcher(core::EventCallbackFn const &event_callback)
            : event_callback_ { event_callback } {}
        void AddPathToWatch(std::filesystem::path const &path, bool recursive = true);
        void OnUpdate() override;
    private:
        std::map<std::filesystem::path, std::filesystem::file_time_type> file_map_;
        core::EventCallbackFn event_callback_;
    };
}