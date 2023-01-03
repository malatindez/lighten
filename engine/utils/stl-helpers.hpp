#pragma once
#include "include/library-pch.hpp"
namespace engine::utils
{
    template <typename T>
    void SortedInsert(std::vector<T> &range, T &&value)
    {
        auto lower = std::lower_bound(range.begin(), range.end(), value);
        range.emplace(lower, std::forward<T>(value));
    }
    template <typename T>
    void SortedInsert(std::vector<T> &range, T &&value, std::function<bool(T const &, T const &)> const &comparator)
    {
        auto lower = std::lower_bound(range.begin(), range.end(), value, comparator);
        range.emplace(lower, std::forward<T>(value));
    }
    // More efficient way to erase elements from a sorted vector
    template <typename T>
    void SortedErase(std::vector<T> &range, T &&value)
    {
        auto t = std::equal_range(range.begin(), range.end(), value);
        range.erase(t.first, t.second);
    }
    template <typename T>
    void SortedErase(std::vector<T> &range, T &&value, std::function<bool(T const &, T const &)> const &comparator)
    {
        auto t = std::equal_range(range.begin(), range.end(), value, comparator);
        range.erase(t.first, t.second);
    }
    template<typename T>
    void hash_combine(size_t &seed, T const &hash)
    {
        seed ^= std::hash<T>{}(hash)+0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
} // namespace engine::utils