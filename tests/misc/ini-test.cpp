
#include <filesystem>
#include <fstream>
#include <misc/ini.hpp>
#include <queue>
#include <set>

#include "pch.h"
#include "utils.hpp"
#include "utils/utils.hpp"
using namespace engine;
using namespace engine::utils;
using namespace ::utils;

namespace fs = std::filesystem;
TEST(TEST_INI, TestSaving)
{
    ini::Ini conf;
    conf.CreateSection("Graphics");
    ini::Section &graphics = conf["Graphics"];

    graphics.SetValue("ResolutionX", 3200);
    graphics.SetValue("ResolutionY", 3200);
    conf["Graphics"]["Gamma"] = 35.5;

    conf["Controls"]["MovementForward"] = "W";
    conf["Controls"]["MovementBackwards"] = "S";
    auto t = conf.Serialize();
    CreateFile(fs::temp_directory_path() / "test/test_ini/TestFile.ini",
               t.c_str(), t.size());
}
TEST(TEST_INI, TestLoading)
{
    std::ifstream ifs(
        fs::temp_directory_path() / "test/test_ini/TestFile.ini",
        std::ios::in);
    std::string value;
    char *const buffer = new char[1];
    ifs.read(buffer, 1);
    while (ifs.good())
    {
        value += buffer[0];
        ifs.read(buffer, 1);
    }
    delete[] buffer;
    ifs.close();

    auto conf = ini::Ini::Deserialize(value);
    ASSERT_EQ(conf["Graphics"].GetInt("ResolutionX"), 3200);
    ASSERT_EQ(conf["Graphics"].GetInt("ResolutionY"), 3200);
    ASSERT_EQ(conf["Controls"]["MovementForward"], "W");
    ASSERT_EQ(conf["Controls"].GetString("MovementBackwards"), "S");
    ASSERT_EQ(conf["Graphics"]["Gamma"], 35.5);
}
TEST(TEST_INI, TestTrimming)
{
    auto conf = ini::Ini::Deserialize(R"(
[A]
  B = 0  #comments test  23 
   C    =      1  ; test   
D   =           5)");
    ASSERT_EQ(conf["A"]["B"], 0);
    ASSERT_EQ(conf["A"]["C"], 1);
    ASSERT_EQ(conf["A"]["D"], 5);
    ASSERT_EQ(conf["A"]["B"], "0");
    ASSERT_EQ(conf["A"]["C"], "1");
    ASSERT_EQ(conf["A"]["D"], "5");
}
static const size_t kRandomSectionsSize = 32;
static const size_t kRandomIntegerKeysSize = 64;
static const size_t kRandomDoubleKeysSize = 64;
static const size_t kRandomStringKeysSize = 64;
static const size_t kSectionKeySize = 64;
static const size_t kKeySize = 32;
static const size_t kStringValueSize = 32;

static std::string kSectionKeyCharacters = ExcludeString(kAsciiCharacters, "[]#;\\");
static std::string kKeyCharacters = ExcludeString(kAsciiCharacters, "[]=#;\\");

TEST(TEST_INI, RandomTest)
{
    auto conf = ini::Ini();
    for (size_t i = 0; i < kRandomSectionsSize; i++)
    {
        auto t = trim(RandomString(kSectionKeySize, kSectionKeyCharacters));
        ini::Section &section = conf[t];
        for (size_t j = 0; j < kRandomIntegerKeysSize; j++)
        {
            section[trim(RandomString(kKeySize, kKeyCharacters))] = Random<int64_t>();
        }
        for (size_t j = 0; j < kRandomDoubleKeysSize; j++)
        {
            section[trim(RandomString(kKeySize, kKeyCharacters))] = Random<long double>();
        }
        for (size_t j = 0; j < kRandomStringKeysSize; j++)
        {
            section[trim(RandomString(kKeySize, kKeyCharacters))] = trim(RandomString(kStringValueSize, kSectionKeyCharacters));
        }
    }
    auto conf2 = ini::Ini::Deserialize(conf.Serialize());
    ASSERT_EQ(conf2.size(), conf.size());
    for (auto const &[section_key, section] : conf)
    {
        ASSERT_TRUE(conf2.SectionExists(section_key)) << section_key;
        ini::Section const &section2 = conf2[section_key];

        ASSERT_EQ(section.size(), section2.size());

        for (auto const &[key, entry] : section)
        {
            ASSERT_TRUE(section2.Contains(key))
                << section_key << "    " << key << "    " << entry.str();
            ASSERT_EQ(section2.at(key), entry);
        }
    }
}

TEST(TEST_INI, TestExceptions) {}
