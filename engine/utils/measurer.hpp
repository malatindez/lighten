#pragma once
#include "debug.hpp"
#include "timer.hpp"
namespace engine::utils
{
    template <class clock>
    struct Measurer
    {
    public:
#ifndef ENGINE_NO_SOURCE_LOCATION
        Measurer(std::string_view s = "Measurer", std::source_location location = std::source_location::current())
        {
            output = CurrentSourceLocation(location) + std::basic_string(s) + " ";
        }
#else
        Measurer(std::string_view s = "Measurer")
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
            entries.push_back(t);
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

            float avg = avg();
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
            out << std::setprecision(7) << avg * 1000 << " milliseconds" << std::endl;
            out << "Average time the function took over the last " << entries_amount.size() << " calls: ";
            out << std::setprecision(7) << avg_over_the_flush * 1000 << " milliseconds" << std::endl;
        }

        float avg(size_t last_n_entries = std::numeric_limits<size_t>::max())
        {
            float rv = 0;
            size_t counter = 0;
            for (size_t i = entries.size() - 1;
                counter < last_n_entries && i >= 0;
                ++counter, --i)
            {
                rv += entries[i];
            }
            return rv / counter;
        }

        float time_to_flush = 1; // every second
        size_t maximum_entries = std::numeric_limits<size_t>::max();
        bool log_automatically = true;

    private:
        std::string output;
        std::vector<float> entries;
        size_t index = 0;
        Timer<clock> flush;
        Timer<clock> measure;
    };
}