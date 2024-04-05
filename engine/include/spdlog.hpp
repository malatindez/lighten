#pragma once

#include "win-def.hpp"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(__clang__)
#ifndef _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define KEEP_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING 0
#else
#define KEEP_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING 1
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4002)
#pragma warning(disable : 4189)
#pragma warning(disable : 4245)
#pragma warning(disable : 4616)
#pragma warning(disable : 6285)
#pragma warning(disable : 6385)
#pragma warning(disable : 26437)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 26498)
#pragma warning(disable : 26800)
#endif
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/async.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#if defined(__clang__)

#pragma clang diagnostic pop

#if !KEEP_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#undef _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#endif
#undef KEEP_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "win-undef.hpp"