#pragma once
#include <stdexcept>
#include <string>
#include <optional>
namespace engine::utils
{
    inline std::optional<int64_t> to_int64(std::string_view str)
    {
        char *end;
        int64_t ll = strtoll(str.data(), &end, static_cast<int>(str.size()));
        if (str.data() != end)
        {
            return ll;
        }
        return std::nullopt;
    }
    inline std::optional<uint64_t> to_uint64(std::string_view str)
    {
        char *end;
        uint64_t ll = strtoull(str.data(), &end, static_cast<int>(str.size()));
        if (str.data() != end)
        {
            return ll;
        }
        return std::nullopt;
    }
    inline std::optional<long double> to_long_double(std::string_view str)
    {
        char *end;
        long double ld = strtold(str.data(), &end);
        if (str.data() != end)
        {
            return ld;
        }
        return std::nullopt;
    }
}