#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <stdint.h>
#include <thread>
#include <assert.h>
namespace engine {
class ParallelExecutor
{
public:
    static const uint32_t kMaxThreads;
    static const uint32_t kHalfThreads;
    using Func = std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)
    ParallelExecutor(uint32_t thread_num)
    {
        is_looping_ = true;
        assert(thread_num > 0);
        finished_thread_num = 0;
        threads_.reserve(thread_num);
        for (uint32_t i = 0; i < thread_num; ++i)
            threads_.emplace_back([this, i]()
                                   { loop(i); });
    }
    ~ParallelExecutor()
    {
        wait();
        is_looping_ = false;
        awake();
    }
    uint32_t thread_num() const { return threads_.size(); }
	bool is_working() const { return finished_thread_num < threads_.size(); }

	void wait()
	{
		if (!is_working()) return;

		std::unique_lock lock(mutex_);
		if (!is_working()) return; // re-check for a case when threads finished and wait_cv_ is notified before the lock is acquired

		wait_cv_.wait(lock);
	}
    
	// Executes a function in parallel blocking the caller thread.
	void execute(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch)
	{
		if (numTasks == 0) return;
		executeAsync(func, numTasks, tasksPerBatch);
		wait();
	}
    
	// Executes a function in parallel asynchronously.
	void executeAsync(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch)
	{
		if (numTasks == 0) return;
		assert(tasksPerBatch > 0);
        
		wait(); // enforced waiting for completion of the previous parallel dispatch
        
		finished_thread_num = 0;
		completed_batch_num = 0;
        
		uint32_t numBatches = (numTasks + tasksPerBatch - 1) / tasksPerBatch;
        
		tasks_to_execute = [this, func, numTasks, numBatches, tasksPerBatch](uint32_t threadIndex)
		{
			while (true)
			{
				uint32_t batchIndex = completed_batch_num.fetch_add(1);
				if (batchIndex >= numBatches) return;

				uint32_t begin = (batchIndex + 0) * tasksPerBatch;
				uint32_t end = (batchIndex + 1) * tasksPerBatch;
				if (end > numTasks) end = numTasks;

				for (uint32_t taskIndex = begin; taskIndex < end; ++taskIndex)
					func(threadIndex, taskIndex);
			}
		};
		awake();
	}

private:
    void awake()
	{
		work_cv_.notify_all();
	}

	void loop(uint32_t threadIndex)
	{
		while (true)
		{
			{
				std::shared_lock lock(mutex_);
				
				if ((finished_thread_num.fetch_add(1) + 1) == threads_.size())
				{
					tasks_to_execute = {};
					wait_cv_.notify_all(); // If an outer thread waits on wait_cv_, it will remain blocked until this thread enters work_cv_.wait(),
										   // because both CVs wait with the same mutex_. This is needed to avoid this thread missing
										   // a notification on work_cv_ in a situation when an outer thread unblocks after this line and before this thread
										   // enters work_cv_.wait(), which would result in this thread being blocked until a next notification.
				}

				// Calling this unlocks mutex_ until work_cv_ is notified, then re-locks it again until the end of the scope.
				work_cv_.wait(lock);
			}

			if (!is_looping_) return;

			assert(tasks_to_execute);
			tasks_to_execute(threadIndex);
		}
	}

	bool is_looping_;

	std::atomic<uint32_t> finished_thread_num;
	std::atomic<uint32_t> completed_batch_num;
	std::function<void(uint32_t)> tasks_to_execute;

	std::shared_mutex mutex_;
	std::condition_variable_any wait_cv_;
	std::condition_variable_any work_cv_;

	std::vector<std::jthread> threads_;
};
} // namespace engine