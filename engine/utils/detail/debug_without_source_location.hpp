#pragma once
#include "include/spdlog.hpp"
#include "../debug.hpp"
namespace engine::utils
{
    inline std::string CurrentSourceLocation() { return ""; }
    // asserts only in debug
    inline void Assert(bool value, std::string_view message = "Assert failed")
    {
        if constexpr (!DEBUG_UTILS_DEBUG_ENABLED) 
        {
            return;
        }

        if (value) [[likely]]
        {
            return;
        }

        if constexpr (DEBUG_UTILS_ASSERT_LOGS)
        {
            spdlog::critical(std::basic_string(message));
        }
        
        assert(value, message);
    }

    inline void AlwaysAssert(bool value, std::string_view message = "Assert failed")
    {
        if (value) [[likely]]
        {
            return;
        }

        if constexpr (DEBUG_UTILS_ASSERT_LOGS)
        {
            spdlog::critical(std::basic_string(message));
        }

        if constexpr (DEBUG_UTILS_DEBUG_ENABLED)
        {
            assert(value, message);
        }
        else
        {
            if constexpr (DEBUG_UTILS_ASSERT_ABORTS)
            {
                std::abort();
            }
            else
            {
                throw std::runtime_error(std::basic_string(message));
            }
        }
    }
}