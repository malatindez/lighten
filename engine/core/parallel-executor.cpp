#include "parallel-executor.hpp"
namespace engine{
    const uint32_t ParallelExecutor::kMaxThreads = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t ParallelExecutor::kHalfThreads = std::max(1u, std::thread::hardware_concurrency() / 2);
}