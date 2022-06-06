#pragma once

#include <array>
#include <assert.h>
#include <functional>
#include <memory>
#include <vector>

#ifdef __clang__
#define __lambda_force_inline __attribute__((always_inline))
#elif defined(__GNUC__)
#define __lambda_force_inline __attribute__((always_inline))
#elif _MSC_VER
#define __lambda_force_inline [[msvc::forceinline]]
#endif