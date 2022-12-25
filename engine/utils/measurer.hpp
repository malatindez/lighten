#pragma once
#include "debug.hpp"
#include "timer.hpp"
namespace engine::utils
{
    template <class clock>
    struct Measurer
    {
    public:
        struct Entry
        {
            float entry_time;
            float elapsed;
        };
#ifndef ENGINE_NO_SOURCE_LOCATION
        Measurer(std::string_view s = "Measurer",
                 bool log_automatically = true,
                 float time_to_flush = 30,
                 std::source_location location = std::source_location::current()) :
            log_automatically(log_automatically), time_to_flush(time_to_flush)
        {
            output = CurrentSourceLocation(location) + std::basic_string(s) + " ";
        }
#else
        Measurer(std::string_view s = "Measurer", bool log_automatically = true, float time_to_flush = 30) :
            log_automatically(log_automatically), time_to_flush(time_to_flush)
        {
            output = s;
        }
#endif
        void begin()
        {
            measure.reset_to_now();
        }
        float end()
        {
            float t = measure.elapsed();
            entries.push_back({ entry_time.elapsed(), measure.elapsed() });
            if (log_automatically && flush.elapsed() > time_to_flush)
            {
                log();
                flush.reset();
            }
            return t;
        }
        void log()
        {
            size_t entries_amount = entries.size() - index;
            index = entries.size() - 1;

            float average = avg();
            float avg_over_the_flush = avg(entries_amount);

            std::stringstream out;
            out << output << std::endl;
            out << "Amount of calls over the last " << std::setprecision(3) << flush.elapsed() << " seconds: ";
            out << entries_amount << std::endl;
            out << "Average % of time the function took over the last ";
            out << std::setprecision(3) << flush.elapsed() << " seconds: ";
            out << std::setprecision(7) << avg_over_the_flush * entries_amount / flush.elapsed() * 100;
            out << std::endl;
            out << "Average time the function took over " << entries.size() << " calls: ";
            out << std::setprecision(7) << average * 1000 << " milliseconds" << std::endl;
            out << "Average time the function took over the last " << entries_amount << " calls: ";
            out << std::setprecision(7) << avg_over_the_flush * 1000 << " milliseconds" << std::endl;

            spdlog::info(out.str());
        }

        float avg(size_t last_n_entries = std::numeric_limits<size_t>::max())
        {
            float rv = 0;
            size_t counter = 0;
            for (int64_t i = int64_t(entries.size()) - 1;
                 counter < last_n_entries && i >= 0;
                 ++counter, --i)
            {
                rv += entries[i].elapsed;
            }
            return rv / counter;
        }

        float avg_over_the_last(float seconds)
        {
            float rv = 0;
            int64_t i = int64_t(entries.size()) - 1;
            for (;
                 i >= 0;
                 --i)
            {
                rv += entries[i].elapsed;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            return rv / seconds;
        }

        float avg_over_the_last_limited(float seconds)
        {
            float rv = 0;
            int64_t i = int64_t(entries.size()) - 1;
            for (;
                 i >= 0;
                 --i)
            {
                rv += entries[i].elapsed;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            if (i < 0 && entries.size() > 0)
            {
                return rv / entries.back().entry_time;
            }
            return rv / seconds;
        }

        uint64_t amount_of_calls(float seconds)
        {
            uint64_t rv = 0;
            int64_t i = int64_t(entries.size()) - 1;
            for (;
                 i >= 0;
                 --i)
            {
                ++rv;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            return rv;
        }

        float elapsed()
        {
            return entry_time.elapsed();
        }

        float time_to_flush = 30; // every second
        size_t maximum_entries = std::numeric_limits<size_t>::max();
        bool log_automatically = true;

    private:
        std::string output;
        std::vector<Entry> entries;
        size_t index = 0;
        Timer<clock> flush;
        Timer<clock> measure;
        Timer<clock> entry_time;
    };
}