#include "file-watcher.hpp"
namespace lighten::misc
{
    void FileWatcher::AddPathToWatch(std::filesystem::path const &path, bool recursive)
    {
        if (std::filesystem::is_regular_file(path))
        {
            file_map_[path] = std::filesystem::last_write_time(path);
            return;
        }
        if (recursive) [[likely]]
        {
            for (auto &subpath : std::filesystem::recursive_directory_iterator(path))
            {
                if (std::filesystem::is_regular_file(subpath))
                {
                    file_map_[subpath.path()] = std::filesystem::last_write_time(subpath.path());
                }
            }
        }
        else if (std::filesystem::is_directory(path))
        {
            for (auto &subpath : std::filesystem::directory_iterator(path))
            {
                file_map_[subpath.path()] = std::filesystem::last_write_time(subpath.path());
            }
        }
    }
    void FileWatcher::OnUpdate()
    {
        std::vector<std::filesystem::path> paths;
        static mal_toolkit::SteadyTimer timer;
        if (timer.elapsed() < 0.25f) [[likely]]
        {
            return;
        }
        timer.reset_to_now();
        for (auto const &[file, ft] : file_map_)
        {
            if (!std::filesystem::exists(file)) [[unlikely]]
            {
				continue;
			}
            auto current_file_last_write_time = std::filesystem::last_write_time(file);
            if (file_map_[file] != current_file_last_write_time) [[unlikely]]
            {
                file_map_[file] = current_file_last_write_time;
                paths.emplace_back(file);
            }
        }
        if (!paths.empty()) [[unlikely]]
        {
            core::events::FilesChangedEvent fce{std::move(paths)};
            event_callback_(fce);
        }
    }
} // namespace lighten::misc