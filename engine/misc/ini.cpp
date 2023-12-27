#include "ini.hpp"
namespace
{
    using namespace mal_toolkit;
}
namespace engine::ini
{
    template <typename T>
    T const &Section::GetValue(std::string const &key) const
    {
        return operator[]<T>(key);
    }
    Entry &Section::operator[](std::string_view key)
    {
        if (mal_toolkit::trimview(key) != key)
        {
            throw std::invalid_argument("The input string should be trimmed!");
        }
        std::string t{ key };
        dict_.try_emplace(t);
        return dict_.at(t);
    }

    [[nodiscard]] Entry const &Section::at(std::string_view const key) const
    {
        return dict_.at(std::string{ key });
    }

    /* Always returns the string value, even of the object of integer or double type
     */
    std::string_view Section::GetString(std::string const &key)
    {
        if (!dict_.contains(key))
        {
            throw KeyErrorException("Invalid key: " + key);
        }
        return dict_.at(key).str();
    }

    long double Section::GetDouble(std::string const &key)
    {
        if (!dict_.contains(key))
        {
            throw KeyErrorException("Invalid key: " + key);
        }
        return dict_.at(key).to_double();
    }

    int64_t Section::GetInt(std::string const &key)
    {
        if (!dict_.contains(key))
        {
            throw KeyErrorException("Invalid key: " + key);
        }
        return dict_.at(key).to_int();
    }

    [[nodiscard]] bool Section::GetBool(std::string const &key)
    {
        if (!dict_.contains(key))
        {
            throw KeyErrorException("Invalid key: " + key);
        }
        return dict_.at(key).to_bool();
    }

    std::string Section::Serialize() const noexcept
    {
        static constexpr auto format = [] (std::string_view const s)
        {
            std::string str(s);
            std::erase(str, '\n');
            std::erase(str, '\r');
            return str;
        };
        std::string return_value;
        for (auto const &[key, value] : dict_)
        {
            return_value += format(key) + "=" + format(value.str()) + "\n";
        }
        return return_value;
    }

    Section &Ini::operator[](std::string_view const key)
    {
        return CreateSection(std::string{ key });
    }

    Section &Ini::CreateSection(std::string const &key)
    {
        if (mal_toolkit::trimview(key) != key)
        {
            throw std::invalid_argument("The input string should be trimmed!");
        } /* creating temporary object because Section() constructor is private */
        dict_.try_emplace(key, Section());
        return dict_.at(key);
    }

    std::string Ini::Serialize() const noexcept
    {
        std::string return_value;
        for (auto const &[key, value] : dict_)
        {
            return_value += "[" + key + "]\n" + value.Serialize();
        }
        return return_value;
    }

    Ini::Ini(std::string_view const str)
    {
        uint64_t i = 0;
        bool skip = false;
        std::string line;
        std::string current_section = "";
        while (i < str.size())
        {
            if (str[i] == '\n')
            {
                if (line.size() >= 3 && line[0] == '[' && line[line.size() - 1] == ']')
                {
                    current_section = std::string(line.begin() + 1, line.end() - 1);
                }
                else
                {
                    DeserializeLine(current_section, line);
                }
                skip = false;
                line.clear();
            }
            else if ((str[i] == ';' || str[i] == '#') &&
                     (i == 0 || str[i - 1] != '\\'))
            {
                skip = true;
            }
            else if (!skip)
            {
                line += str[i];
            }
            i++;
        }
        if (!line.empty())
        {
            DeserializeLine(current_section, line);
        }
    }

    Ini Ini::Deserialize(std::string_view const str) { return Ini(str); }

    inline void Ini::DeserializeLine(std::string const &section,
                                     std::string &line)
    { /* creating temporary object because
         Section() constructor is private */
        dict_.try_emplace(section, Section());
        size_t pos = line.find('=');
        while (pos != std::string::npos && pos != 0 && line[pos - 1] == '\\')
        {
            pos = line.find('=', pos + 1);
        }
        if (pos == std::string::npos)
        {
            return;
        }
        std::string key{ mal_toolkit::trimview(std::string_view{line.begin(), line.begin() + pos}) };
        std::string value{ mal_toolkit::trimview(std::string_view{line.begin() + pos + 1, line.end()}) };
        char *pEnd;
        int64_t ll = strtoll(value.c_str(), &pEnd, 10);
        if (value.c_str() != pEnd && *pEnd == '\0')
        {
            dict_.at(section)[key] = ll;
            return;
        }
        long double ld = strtold(value.c_str(), &pEnd);
        if (value.c_str() != pEnd && *pEnd == '\0')
        {
            dict_.at(section)[key] = ld;
            return;
        }
        dict_.at(section)[key] = value;
    }
} // namespace engine::ini