#pragma once

#include <sstream>
#include "utils.hpp"

#if !defined(__clang__) && __has_include(<source_location>)               
#  include <source_location>
#elif __has_include(<experimental/source_location>)
#  include <experimental/source_location>
namespace std
{
    using source_location = std::experimental::source_location;
}
#else
#define DEBUG_UTILS_NO_SOURCE_LOCATION
#endif

namespace engine::core::debug_utils
{
#ifdef DEBUG_UTILS_NO_SOURCE_LOCATION
    inline std::string CurrentSourceLocation() { return ""; }
#else
    inline std::string CurrentSourceLocation(std::source_location location = std::source_location::current())
    {
        std::stringstream ss;
        ss << "[" << location.file_name() << "] ";
        ss << location.function_name() << "(line " << location.line() << ", column " << location.column() << ") ";
        return ss.str();
    }
#endif
} // namespace engine::core::debug_utils
