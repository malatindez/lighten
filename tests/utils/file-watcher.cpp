#include "pch.h"
#include "include/engine-pch.hpp"
#include "utils.hpp"

using namespace engine::core::events;

TEST(FILE_WATCHER_TEST, BasicTest)
{
    std::optional<FilesChangedEvent> last_event = std::nullopt;
    auto callback_fn = [&] (Event &e)
    {
        engine::utils::AlwaysAssert(e.type() == EventType::FilesChanged);

        auto &fce = static_cast<FilesChangedEvent &>(e);
        last_event.emplace(fce);
        fce.handled = true;
    };
    {
        engine::utils::FileWatcher file_watcher{ callback_fn };
        std::filesystem::path path = std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test/first.file";
        std::string random_string = utils::RandomBinaryString(4096);
        utils::CreateFile(path, random_string.c_str(), random_string.size());
        file_watcher.AddPathToWatch(path);
        random_string = utils::RandomBinaryString(4096);
        utils::CreateFile(path, random_string.c_str(), random_string.size());
        file_watcher.OnUpdate();
        ASSERT_FALSE(last_event.value().files_changed().empty());
        ASSERT_TRUE(last_event.value().files_changed()[0] == path);
    }
}
TEST(FILE_WATCHER_TEST, RandomTest)
{
    static constexpr size_t kTestSize = utils::RandomConstexpr(0, 2, 8);
    std::optional<FilesChangedEvent> last_event = std::nullopt;
    auto callback_fn = [&] (Event &e)
    {
        engine::utils::AlwaysAssert(e.type() == EventType::FilesChanged);

        auto &fce = static_cast<FilesChangedEvent &>(e);
        last_event.emplace(fce);
        fce.handled = true;
    };
    auto paths = utils::CreateRandomFilesRecursive(std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test" / "random-test", 3, 2, 6);
    std::set<std::filesystem::path> files_changed;

    engine::utils::FileWatcher file_watcher{ callback_fn };
    file_watcher.AddPathToWatch(std::filesystem::temp_directory_path() / "engine/tests/file-watcher-test" / "random-test");
    for (int i = 0; i < kTestSize; i++)
    {
        size_t amount_to_change = utils::Random<size_t>(0, paths.size());
        for (int j = 0; j < amount_to_change; j++)
        {
            auto &file = paths.at(utils::Random<size_t>(0, paths.size() - 1));
            files_changed.emplace(file);
            std::string s = utils::RandomBinaryString(1024);
            utils::CreateFile(file, s.c_str(), s.size());
        }
        file_watcher.OnUpdate();
        for (auto &file : last_event->files_changed())
        {
            files_changed.erase(file);
        }
        ASSERT_TRUE(files_changed.empty());
    }
}