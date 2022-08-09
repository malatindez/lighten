#pragma once
#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4002)
#pragma warning(disable:4189)
#pragma warning(disable:4245)
#pragma warning(disable:6285)
#pragma warning(disable:26495)
#endif
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ansicolor_sink.h"
#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif