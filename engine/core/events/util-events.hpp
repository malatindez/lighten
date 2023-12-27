#pragma once
#include "include/library-pch.hpp"
#include "event.hpp"
namespace engine::core::events
{
    // Event that is caused by misc::FileWatcher
    class FilesChangedEvent final : public Event
    {
    public:
        explicit FilesChangedEvent(std::vector<std::filesystem::path> &&files)
            : Event(EventType::FilesChanged, EventCategoryUtils), files_{ std::move(files) } {}

        FilesChangedEvent(FilesChangedEvent &&) = default;
        FilesChangedEvent(FilesChangedEvent const &) = default;
        FilesChangedEvent &operator=(FilesChangedEvent &&) = delete;
        FilesChangedEvent &operator=(FilesChangedEvent const &) = delete;

        [[nodiscard]] inline std::vector<std::filesystem::path> const &files_changed() { return files_; }
        EVENT_CLASS_TYPE(FilesChanged)
    private:
        std::vector<std::filesystem::path> files_;
    };
}