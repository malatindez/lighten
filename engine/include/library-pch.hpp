#pragma once

#include "entt/entt.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <istream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <shared_mutex>
#include <span>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>

#include "common-typedefs.hpp"
#include "core/math.hpp"
#include "core/layers/layer.hpp"

#ifdef __clang__
#define __lambda_force_inline __attribute__((always_inline))
#elif defined(__GNUC__)
#define __lambda_force_inline __attribute__((always_inline))
#elif _MSC_VER
#define __lambda_force_inline [[msvc::forceinline]]
#endif

#ifndef ENGINE_NO_SOURCE_LOCATION
#if !defined(__clang__) && __has_include(<source_location>)
#include <source_location>
#elif __has_include(<experimental/source_location>)
#include <experimental/source_location>
namespace std
{
    using source_location = std::experimental::source_location;
}
#else
#define ENGINE_NO_SOURCE_LOCATION
#endif
#endif