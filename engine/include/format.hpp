#pragma once
#if __has_include(<format>)
#include <format>
#elif __has_include(<experimental/format>)
#include <experimental/format>
namespace std
{
    namespace format = std::experimental::format;
} // namespace std
#else
#error "Missing format library"
#endif