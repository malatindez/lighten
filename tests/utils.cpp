#include "utils.hpp"
namespace fs = std::filesystem;
namespace utils
{
    inline std::string ProhibitedCharactersInFilename()
    {
        std::string return_value = "<>:\"/\\|?*";
        for (char i = 0; i < 32; i++)
        {
            return_value += i;
        }
        return return_value;
    }
    const std::string kProhibitedCharacters = ProhibitedCharactersInFilename();

    inline std::string AllBinaryCharacters()
    {
        std::string return_value;
        for (char i = -128; i < 127; i++)
        {
            return_value.push_back(i);
        }
        return return_value;
    }
    const std::string kAllBinaryCharacters = AllBinaryCharacters();

    [[nodiscard]] std::string Uint32ToUTF8(uint32_t const &value)
    {
        if (value > 0x10FFFF)
        {
            throw std::invalid_argument("Invalid value provided.");
        }

        std::string return_value = "";

        if (value < 0x80)
        {
            return_value += static_cast<char>(value);
        }
        else if (value < 0x800)
        {
            return_value += static_cast<char>(((value >> 0x06) & 0b00011111) | 0b11000000);
            return_value += static_cast<char>(((value >> 0x00) & 0b00111111) | 0b10000000);
        }
        else if (value < 0x10000)
        {
            return_value += static_cast<char>(((value >> 0x0C) & 0b00001111) | 0b11100000);
            return_value += static_cast<char>(((value >> 0x06) & 0b00111111) | 0b10000000);
            return_value += static_cast<char>(((value >> 0x00) & 0b00111111) | 0b10000000);
        }
        else
        {
            return_value += static_cast<char>(((value >> 0x12) & 0b00000111) | 0b11110000);
            return_value += static_cast<char>(((value >> 0x0C) & 0b00111111) | 0b10000000);
            return_value += static_cast<char>(((value >> 0x06) & 0b00111111) | 0b10000000);
            return_value += static_cast<char>(((value >> 0x00) & 0b00111111) | 0b10000000);
        }
        return return_value;
    }

    [[nodiscard]] inline std::string RandomUTF8Char(uint32_t const &from, uint32_t const &to)
    {
        return Uint32ToUTF8(Random(from, to));
    }

    [[nodiscard]] std::string RandomUTF8String(size_t const &size)
    {
        std::string return_value;
        for (size_t i = 0; i < size; i++)
        {
            return_value += RandomUTF8Char();
        }
        return return_value;
    }
    [[nodiscard]] std::string RandomUTF8Filename(size_t const &size)
    {
        std::string return_value;
        for (size_t i = 0; i < size - 1; i++)
        {
            std::string temp = RandomUTF8Char(32);

            while (temp.size() == 1 &&
                   std::find_if(kProhibitedCharacters.begin(),
                                kProhibitedCharacters.end(), [&temp] (char const &c)
                                { return c == temp[0]; }) != kProhibitedCharacters.end())
            {
                temp = RandomUTF8Char(32);
            }
            return_value += temp;
        }
        std::string final_char; // files in windows cannot end with space nor with dot
        while (final_char.size() == 1 &&
               std::find_if(kProhibitedCharacters.begin(),
                            kProhibitedCharacters.end(),
                            [&final_char] (char const &c)
                            {
                                return c == final_char[0] || final_char[0] == ' ' ||
                                    final_char[0] == '.';
                            }) != kProhibitedCharacters.end())
        {
            final_char = RandomUTF8Char(32);
        }
                            return return_value + final_char;
    }

    [[nodiscard]] std::string RandomBinaryString(size_t const &size)
    {
        return RandomString(size, kAllBinaryCharacters);
    }

    [[nodiscard]] std::string RandomString(size_t const &size, std::string_view const including)
    {
        std::string return_value;
        return_value.reserve(size);
        std::uniform_int_distribution<size_t> dis{ 0, including.size() - 1 };
        for (size_t i = 0; i < size; i++)
        {
            size_t k = dis(gen);
            return_value += including.at(k);
        }

        return return_value;
    }

    [[nodiscard]] std::string RandomFilename(size_t const &size)
    {
        if (size == 0)
        {
            return "";
        }
        std::string return_value = RandomString(
            size, ExcludeString(kAsciiCharacters, kProhibitedCharacters));
        if (return_value[return_value.size() - 1] == ' ' ||
            return_value[return_value.size() - 1] == '.')
        {
            return_value[return_value.size() - 1] = static_cast<char>(Random<unsigned short>('a', 'z'));
        }
        return return_value;
    }

    void CreateFile(fs::path const &path, char const *data, const size_t size)
    {
        fs::create_directories({ path.parent_path() });
        std::ofstream ofs(path);
        ofs.write(data, size);
        ofs.close();
    }
    std::vector<fs::path> CreateRandomFiles(fs::path const &path,
                                            size_t file_size,
                                            size_t amount)
    {
        std::vector<fs::path> rv;
        fs::create_directories({ path.parent_path() });
        for (int i = 0; i < amount; i++)
        {
            fs::path temp = path / RandomFilename(Random<size_t>(5, 32));
            std::string random_string = RandomBinaryString(file_size);
            CreateFile(temp, random_string.c_str(), random_string.size());
            rv.emplace_back(temp);
        }
        return rv;
    }
    std::vector<fs::path> CreateRandomFilesRecursive(fs::path const &path,
                                                     int depth,
                                                     size_t folder_amount,
                                                     size_t file_amount_per_folder)
    {
        if (depth == -1)
        {
            return {};
        }
        std::vector<fs::path> rv;
        fs::create_directories({ path.parent_path() });
        for (int i = 0; i < folder_amount; i++)
        {
            fs::path temp = path / RandomFilename(Random<size_t>(5, 32)) / "";
            auto t = CreateRandomFilesRecursive(temp, depth - 1, folder_amount);
            rv.insert(rv.begin(), t.begin(), t.end());
        }
        auto t = CreateRandomFiles(path, 1024, file_amount_per_folder);
        rv.insert(rv.begin(), t.begin(), t.end());
        return rv;
    }
}