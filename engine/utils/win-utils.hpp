#pragma once
#include "include/pch.hpp"
#include "include/win.hpp"
namespace utils
{
    // UTF16 to UTF8
    inline std::string wstring_to_string(std::wstring_view const wstr)
    {
        if (wstr.empty())
            return std::string();
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string rv(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), rv.data(), size, NULL, NULL);
        return rv;
    }

    // UTF8 to UTF16
    inline std::wstring string_to_wstring(std::string_view const str)
    {
        if (str.empty())
            return std::wstring();
        int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
        std::wstring rv(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), rv.data(), size);
        return rv;
    }
}