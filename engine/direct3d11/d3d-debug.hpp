#pragma once
#include "utils/debug.hpp"
#include <system_error>
namespace engine::direct3d
{
    
#ifdef ENGINE_NO_SOURCE_LOCATION
inline void Assert(HRESULT hr, std::string_view message = "")
{
    if constexpr(!DEBUG_UTILS_DEBUG_ENABLED)
    {
        return;
    }
    if(SUCCEEDED(hr)) [[likely]]
    {
        return;
    }
    utils::Assert(SUCCEEDED(hr), message + " " + std::system_category().message(hr));
}

inline void AlwaysAssert(HRESULT hr, std::string_view message = "")
{
    if(SUCCEEDED(hr)) [[likely]]
    {
        return;
    }
    utils::Assert(SUCCEEDED(hr), message + " " + std::system_category().message(hr));
}
#else
inline void Assert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
{
    if constexpr(!DEBUG_UTILS_DEBUG_ENABLED)
    {
        return;
    }
    if(SUCCEEDED(hr)) [[likely]]
    {
        return;
    }
    utils::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + std::system_category().message(hr), location);
}

inline void AlwaysAssert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
{
    if(SUCCEEDED(hr)) [[likely]]
    {
        return;
    }
    utils::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + std::system_category().message(hr), location);
}

#endif

}