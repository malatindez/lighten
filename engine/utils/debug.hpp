#pragma once
#include "include/library-pch.hpp"
#include "include/spdlog.hpp"

#ifndef DEBUG_UTILS_ASSERT_LOGS
#define DEBUG_UTILS_ASSERT_LOGS 1
#endif

// this option is ignored in _DEBUG (implicitly calls builtin assert)
// if false, it will check for ASSERT_THROWS

#if !defined(DEBUG_UTILS_ASSERT_ABORTS)
#if defined(_DEBUG)
#define DEBUG_UTILS_ASSERT_ABORTS 1
#define DEBUG_UTILS_ASSERT_THROWS 0
#else
#define DEBUG_UTILS_ASSERT_ABORTS 0
#endif
#endif

#ifndef DEBUG_UTILS_ASSERT_THROWS
#define DEBUG_UTILS_ASSERT_THROWS 1
#endif

#ifdef _DEBUG
#define DEBUG_UTILS_DEBUG_ENABLED 1
#else
#define DEBUG_UTILS_DEBUG_ENABLED 0
#endif

#if DEBUG_UTILS_ASSERT_THROWS
#define DEBUG_UTILS_ASSERT_NOEXCEPT
#else
#define DEBUG_UTILS_ASSERT_NOEXCEPT noexcept
#endif

#ifdef ENGINE_NO_SOURCE_LOCATION
#include "detail/debug_without_source_location.hpp"
#else
#include "detail/debug_with_source_location.hpp"
#endif