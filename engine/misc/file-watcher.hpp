#pragma once
#include "core/layers/layer.hpp"
namespace lighten::misc
{
    class FileWatcher : public core::Layer, public core::Layer::HandleUpdate
    {
    public:
        explicit FileWatcher(core::EventCallbackFn const &event_callback)
            : core::Layer("File Watcher"), event_callback_{event_callback} {}
        void AddPathToWatch(std::filesystem::path const &path, bool recursive = true);
        void OnUpdate() override;

    private:
        std::map<std::filesystem::path, std::filesystem::file_time_type> file_map_;
        core::EventCallbackFn event_callback_;
    };
}