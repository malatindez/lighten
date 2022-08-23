#pragma once
#include "utils/utils.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#undef CreateFile
namespace utils
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    namespace fs = std::filesystem;
    namespace _detail
    {
        // https://www.youtube.com/watch?v=rpn_5Mrrxf8
        constexpr auto seed()
        {
            std::uint64_t shifted = 0;

            for (const auto c : __TIME__)
            {
                shifted <<= 8;
                shifted |= c;
            }

            return shifted;
        }

        struct PCG
        {
            struct pcg32_random_t
            {
                std::uint64_t state = 0;
                std::uint64_t inc = seed();
            };
            pcg32_random_t rng;
            typedef std::uint32_t result_type;

            constexpr result_type operator()()
            {
                return pcg32_random_r();
            }

            static result_type constexpr min()
            {
                return std::numeric_limits<result_type>::min();
            }

            static result_type constexpr max()
            {
                return std::numeric_limits<result_type>::min();
            }

        private:
            constexpr std::uint32_t pcg32_random_r()
            {
                std::uint64_t oldstate = rng.state;
                // Advance internal state
                rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
                // Calculate output function (XSH RR), uses old state for max ILP
                auto xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
                std::uint32_t rot = oldstate >> 59u;
                return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
            }
        };
    } // namespace _detail
    constexpr auto RandomConstexpr(int count, int min = 0, int max = 0xffffffff)
    {
        count = count + ((min ^ 0xff) & 0xff) + ((max ^ 0xff) & 0xff);
        _detail::PCG pcg;
        while (count > 0)
        {
            pcg();
            --count;
        }
        return min + pcg() % (max - min);
    }

    template <std::integral T>
    [[nodiscard]] T Random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
    {
        std::uniform_int_distribution dis { min, max };
        return dis(gen);
    }

    template <std::floating_point T>
    [[nodiscard]] T Random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
    {
        std::uniform_real_distribution dis { min, max };
        return dis(gen);
    }
    const std::string kAsciiCharacters =
        "' "
        "!\"#$%&'()*+,-./"
        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~";

    [[nodiscard]] inline std::string ExcludeString(std::string const &a, std::string_view const b)
    {
        std::string return_value { a };
        std::erase_if(return_value, [&b] (char const &c)
                      { return std::ranges::find(b, c) != b.end(); });
        return return_value;
    }

    [[nodiscard]] std::string Uint32ToUTF8(uint32_t const &value);
    [[nodiscard]] inline std::string RandomUTF8Char(uint32_t const &from = 0, uint32_t const &to = 0xA000);
    [[nodiscard]] std::string RandomUTF8String(size_t const &size = 32);
    [[nodiscard]] std::string RandomUTF8Filename(size_t const &size = 32);
    [[nodiscard]] std::string RandomBinaryString(size_t const &size = 32);
    [[nodiscard]] std::string RandomString(size_t const &size = 32, std::string_view const including = kAsciiCharacters);
    [[nodiscard]] std::string RandomFilename(size_t const &size = 32);

    std::vector<fs::path> CreateRandomFiles(fs::path const &path, size_t file_size = 1024, size_t amount = 16);
    std::vector<fs::path> CreateRandomFilesRecursive(fs::path const &path, int depth = 3, size_t folder_amount = 4, size_t file_amount_per_folder = 4);

    void CreateFile(fs::path const &path, char const *data, const size_t size);

    [[nodiscard]] constexpr double time_diff(std::chrono::high_resolution_clock::time_point begin, std::chrono::high_resolution_clock::time_point end)
    {
        return std::chrono::duration<double, std::milli>(end - begin).count();
    }
}