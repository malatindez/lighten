
#include <iostream>

#include "math.hpp"
#include "pch.hpp"

namespace math = engine::core::math;
template <size_t a, size_t b, math::Primitive T>
void random_fill(math::mat<a, b, T> &matrix)
{
    for (int i = 0; i < matrix.size.x; i++)
    {
        for (int j = 0; j < matrix.size.y; j++)
        {
            matrix[i][j] = rand() % 2;
        }
    }
}
#include "core/parallel-executor.hpp"

#include <array>
#include <string>
#include <iostream>
#include <chrono>

// required headers
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>
int main()
{
	namespace t = std::chrono;
    using engine::core::ParallelExecutor;
	uint32_t numTasks = 1000;
	uint32_t tasksPerBatch = 10;

	std::vector<int> indices;
	std::vector<int> values;
	values.resize(numTasks);
	indices.resize(numTasks, -1);

	auto heavyFunc = [&values, &indices](uint32_t, uint32_t taskIndex)
	{
		uint32_t x = 0;
		for (uint32_t i = 0; i < 10000000; ++i)
			x += i;
		values[taskIndex] = x;
		indices[taskIndex] = taskIndex;
	};

	{
		// Set numThreads to the number of logical cores minus 4 if there are more than 8, otherwise just half of them
		uint32_t numThreads = std::max(1u, std::max(ParallelExecutor::kMaxThreads - 4u, ParallelExecutor::kHalfThreads));

		ParallelExecutor executor{ numThreads };

		for (uint32_t i = 0; i < 10; ++i)
		{
			for (uint32_t j = 0; j < indices.size(); ++j)
				indices[j] = -1;

			auto start = t::steady_clock::now();
			executor.execute(heavyFunc, static_cast<uint32_t>(values.size()), tasksPerBatch);
			auto finish = t::steady_clock::now();

			for (uint32_t j = 0; j < indices.size(); ++j)
				assert(static_cast<uint32_t>(indices[j]) == j);

			std::cout << executor.thread_num() << " threads, time : " << t::duration_cast<t::microseconds>(finish - start).count() << " mcs" << std::endl;
		}
	}

	for (uint32_t i = 0; i < 1; ++i)
	{
		auto start = t::steady_clock::now();

		for (uint32_t j = 0; j < numTasks; ++j)
			heavyFunc(0, j);

		auto finish = t::steady_clock::now();

		std::cout << "single thread, time : " << t::duration_cast<t::microseconds>(finish - start).count() << " ms" << std::endl;
	}
}