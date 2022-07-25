#pragma once
#include <stdexcept>
#include <algorithm>
#include <string>
#include <optional>
namespace engine::core::utils
{
    inline std::string as_lowercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv](char const& c) { rv += static_cast<char>(std::tolower(c)); });
        return rv;
    }
    inline std::string as_uppercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv](char const& c) { rv += static_cast<char>(std::toupper(c)); });
        return rv;
    }
    // trim from start (in place)
    constexpr std::string_view ltrimview(std::string_view const s) noexcept
    {
        return std::string_view( std::ranges::find_if(s, [](unsigned char ch) { return !std::isspace(ch); }), s.end());
    }

    // trim from end (in place)
    constexpr std::string_view rtrimview(std::string_view const s) noexcept
    {
        return std::string_view(s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base());
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

    inline std::optional<int64_t> to_int64(std::string_view str)
    {
        char *end;
        int64_t ll = strtoll(str.data(), &end, static_cast<int>(str.size()));
        if(str.data() != end)
        {
            return ll;
        }
        return std::nullopt;
    }
    inline std::optional<long double> to_long_double(std::string_view str)
    {
        char *end;
        long double ld = strtold(str.data(), &end);
        if(str.data() != end)
        {
            return ld;
        }
        return std::nullopt;
    }

    template <typename T>
    struct return_type;

    template <typename R, typename... Args>
    struct return_type<R(Args...)>
    {
        using type = R;
    };

    template <typename R, typename... Args>
    struct return_type<R (*)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) volatile>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) volatile &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) volatile &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const volatile>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const volatile &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R (C::*)(Args...) const volatile &&>
    {
        using type = R;
    };

    template <typename T>
    using return_type_t = typename return_type<T>::type;
} // namespace engine::core::utils