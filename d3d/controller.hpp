#pragma once
#include "core/layer.hpp"
#include "core/parallel-executor.hpp"
#include "core/window.hpp"

#include <functional>
#include <memory>
#include <vector>

class Controller : public engine::core::Layer
{
public:
    using UpdateCallback = std::function<void(float)>;
    Controller(std::shared_ptr<engine::core::Window> window);
    void OnEvent(engine::core::events::Event &event) override;

    std::vector<std::function<void(float)>> &update_callbacks() { return update_callbacks_; }

private:
    void Tick(float delta_time);

    std::shared_ptr<engine::core::Window> window_;
    engine::core::ParallelExecutor executor{
        static_cast<uint32_t>(std::max(1, std::max(int32_t(engine::core::ParallelExecutor::kMaxThreads) - 4, int32_t(engine::core::ParallelExecutor::kHalfThreads))))};
    std::vector<UpdateCallback> update_callbacks_;
    long double time_from_start_ = 0;
};