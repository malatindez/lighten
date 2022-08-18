#pragma once
#include "include/pch.hpp"
#include "core/math.hpp"
namespace engine::utils
{

    inline std::string as_lowercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv] (char const &c)
                              { rv += static_cast<char>(std::tolower(c)); });
        return rv;
    }
    inline std::string as_uppercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s, [&rv] (char const &c)
                              { rv += static_cast<char>(std::toupper(c)); });
        return rv;
    }
    // trim from start (in place)
    constexpr std::string_view ltrimview(std::string_view const s) noexcept
    {
        return std::string_view(std::ranges::find_if(s, [] (unsigned char ch)
                                                     { return !std::isspace(ch); }),
                                s.end());
    }

    // trim from end (in place)
    constexpr std::string_view rtrimview(std::string_view const s) noexcept
    {
        return std::string_view(s.begin(), std::find_if(s.rbegin(), s.rend(), [] (unsigned char ch)
                                                        { return !std::isspace(ch); })
                                .base());
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

    template<core::math::AnyVec Vector>
    inline std::string FormatToString(Vector vec, uint32_t precision = 3)
    {
        std::stringstream ss;
        typename Vector::type max;
        for (int i = 0; i < Vector::size; i++)
        {
            max = std::max(max, std::abs(std::log10f(vec[i])) + (max < 0 ? 1 : 0));
        }
        uint32_t size = max + 1 + precision;
        for (int i = 0; i < Vector::size; i++)
        {
            ss << std::setw(size) << std::setprecision(precision) << vec[i];
            ss << " ";
        }
        return ss.str();
    }

    template<core::math::AnyMat Matrix>
    inline std::string FormatToString(Matrix mat, uint32_t precision = 3)
    {
        std::stringstream ss;
        typename Matrix::type max = std::numeric_limits<typename Matrix::type>::min();
        for (int i = 0; i < Matrix::size.x * Matrix::size.y; i++)
        {
            max = std::max((float)max, std::abs(std::ceil(std::log10f((float)mat.arr[i]))) + (max < 0 ? 1 : 0));
        }
        uint32_t size = (uint32_t)max + 1 + precision;
        for (int column = 0; column < Matrix::size.y; column++)
        {
            for (int row = 0; row < Matrix::size.x; row++)
            {
                ss << std::setw(size) << std::setprecision(precision) << mat[column][row];
                ss << " ";
            }
            ss << std::endl;
        }
        return ss.str();
    }
}