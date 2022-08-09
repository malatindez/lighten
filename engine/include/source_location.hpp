#pragma once
#include <sstream>
#ifndef ENGINE_NO_SOURCE_LOCATION
#  if !defined(__clang__) && __has_include(<source_location>)               
#    include <source_location>
#  elif __has_include(<experimental/source_location>)
#    include <experimental/source_location>
namespace std
{
    using source_location = std::experimental::source_location;
}
#  else
#    define ENGINE_NO_SOURCE_LOCATION
#  endif
#endif