#include "pch.h"
#include "include/engine-pch.hpp"
#include "mal-toolkit/mal-toolkit.hpp"

using namespace engine::core::events;

TEST(FILE_WATCHER_TEST, BasicTest)
{
    std::optional<FilesChangedEvent> last_event = std::nullopt;
    auto callback_fn = [&] (Event &e)
    {
        mal_toolkit::AlwaysAssert(e.type() == EventType::FilesChanged);

        auto &fce = static_cast<FilesChangedEvent &>(e);
        last_event.emplace(fce);
        fce.handled = true;
    };
    {
        mal_toolkit::FileWatcher file_watcher{ callback_fn };
        std::filesystem::path path = std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test/first.file";
        std::string random_string = mal_toolkit::RandomBinaryString(4096);
        mal_toolkit::CreateFile(path, random_string.c_str(), random_string.size());
        file_watcher.AddPathToWatch(path);
        random_string = mal_toolkit::RandomBinaryString(4096);
        mal_toolkit::CreateFile(path, random_string.c_str(), random_string.size());
        file_watcher.OnUpdate();
        ASSERT_FALSE(last_event.value().files_changed().empty());
        ASSERT_TRUE(last_event.value().files_changed()[0] == path);
    }
}
TEST(FILE_WATCHER_TEST, RandomTest)
{
    static constexpr size_t kTestSize = mal_toolkit::RandomConstexpr(0, 2, 8);
    std::optional<FilesChangedEvent> last_event = std::nullopt;
    auto callback_fn = [&] (Event &e)
    {
        mal_toolkit::AlwaysAssert(e.type() == EventType::FilesChanged);

        auto &fce = static_cast<FilesChangedEvent &>(e);
        last_event.emplace(fce);
        fce.handled = true;
    };
    auto paths = mal_toolkit::CreateRandomFilesRecursive(std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test" / "random-test", 3, 2, 6);
    std::set<std::filesystem::path> files_changed;

    mal_toolkit::FileWatcher file_watcher{ callback_fn };
    file_watcher.AddPathToWatch(std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test" / "random-test");
    for (int i = 0; i < kTestSize; i++)
    {
        size_t amount_to_change = mal_toolkit::Random<size_t>(0, paths.size());
        for (int j = 0; j < amount_to_change; j++)
        {
            auto &file = paths.at(mal_toolkit::Random<size_t>(0, paths.size() - 1));
            files_changed.emplace(file);
            std::string s = mal_toolkit::RandomBinaryString(1024);
            mal_toolkit::CreateFile(file, s.c_str(), s.size());
        }
        file_watcher.OnUpdate();
        for (auto &file : last_event->files_changed())
        {
            files_changed.erase(file);
        }
        ASSERT_TRUE(files_changed.empty());
    }
}