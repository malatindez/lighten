#pragma once

#ifdef __clang__

#elif defined(__GNUC__)

#elif _MSC_VER

#pragma warning(push)
#pragma warning(disable : 5054) // operator '|': deprecated between enumerations of different types 

#endif

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/json.hpp>

#ifdef __clang__

#elif defined(__GNUC__)

#elif _MSC_VER

#pragma warning(pop)

#endif
