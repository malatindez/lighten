#pragma once
#include <stdexcept>
#include <string>

namespace engine::core::utils
{
    // trim from start (in place)
    constexpr std::string_view ltrimview(std::string_view const &s)
    {
        return std::string_view(
            std::find_if(s.begin(), s.end(),
                         [](unsigned char ch)
                         { return !std::isspace(ch); }),
            s.end());
    }

    // trim from end (in place)
    constexpr std::string_view rtrimview(std::string_view const &s)
    {
        return std::string_view(
            s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                                    { return !std::isspace(ch); })
                           .base());
    }

    // trim from both ends (in place)
    constexpr std::string_view trimview(std::string_view const &s)
    {
        return ltrimview(rtrimview(s));
    }
    // trim from start (in place)
    inline std::string ltrim(std::string const &s)
    {
        return std::string(
            std::find_if(s.begin(), s.end(),
                         [](unsigned char ch)
                         { return !std::isspace(ch); }),
            s.end());
    }

    // trim from end (in place)
    inline std::string rtrim(std::string const &s)
    {
        return std::string(s.begin(),
                           std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                                        { return !std::isspace(ch); })
                               .base());
    }

    // trim from both ends (in place)
    inline std::string trim(std::string const &s) { return ltrim(rtrim(s)); }
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