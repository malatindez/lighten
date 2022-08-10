#pragma once
#include "include/pch.hpp"
namespace engine::utils
{

    inline std::string as_lowercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv] (char const &c) { rv += static_cast<char>(std::tolower(c)); });
        return rv;
    }
    inline std::string as_uppercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv] (char const &c) { rv += static_cast<char>(std::toupper(c)); });
        return rv;
    }
    // trim from start (in place)
    constexpr std::string_view ltrimview(std::string_view const s) noexcept
    {
        return std::string_view(std::ranges::find_if(s, [] (unsigned char ch) { return !std::isspace(ch); }), s.end());
    }

    // trim from end (in place)
    constexpr std::string_view rtrimview(std::string_view const s) noexcept
    {
        return std::string_view(s.begin(), std::find_if(s.rbegin(), s.rend(), [] (unsigned char ch) { return !std::isspace(ch); }).base());
    }

    // trim from both ends (in place)
    constexpr std::string_view trimview(std::string_view const s) noexcept
    {
        return ltrimview(rtrimview(s));
    }
    // trim from start (in place)
    inline std::string ltrim(std::string const &s) noexcept
    {
        return std::string(ltrimview(s));
    }

    // trim from end (in place)
    inline std::string rtrim(std::string const &s) noexcept
    {
        return std::string(rtrimview(s));
    }

    // trim from both ends (in place)
    inline std::string trim(std::string const &s) noexcept { return std::string(trimview(s)); }

}