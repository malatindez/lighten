#pragma once
#include <assert.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <stdint.h>
#include <thread>
namespace engine::core
{
    class ParallelExecutor
    {
    public:
        static const uint32_t kMaxThreads;
        static const uint32_t kHalfThreads;
        using Func =
            std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)
        explicit ParallelExecutor(uint32_t thread_num);
        ~ParallelExecutor();
        [[nodiscard]] constexpr size_t thread_num() const { return threads_.size(); }
        [[nodiscard]] bool is_working() const { return finished_thread_num < threads_.size(); }

        void wait();
        // Executes a function in parallel blocking the caller thread.
        void execute(const Func &func, uint32_t numTasks, uint32_t tasksPerBatch);

        // Executes a function in parallel asynchronously.
        void executeAsync(const Func &func, uint32_t numTasks,
                          uint32_t tasksPerBatch);

    private:
        void awake() { work_cv_.notify_all(); }

        void loop(uint32_t threadIndex);

        bool is_looping_ = true;

        std::atomic<uint32_t> finished_thread_num = 0;
        std::atomic<uint32_t> completed_batch_num = 0;
        std::function<void(uint32_t)> tasks_to_execute;

        std::shared_mutex mutex_;
        std::condition_variable_any wait_cv_;
        std::condition_variable_any work_cv_;

        std::vector<std::jthread> threads_;
    };
} // namespace engine::core