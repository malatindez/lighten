#pragma once
#include <chrono>
namespace engine::core
{
  template <class clock>
  class Timer
  {
  public:
    Timer() {
        current_ = clock::now();
        reset(); 
    }
    void reset() { start_ = current_; }
    constexpr float elapsed() noexcept
    {
      current_ = clock::now();
      return std::chrono::duration_cast<std::chrono::duration<float>>(current_ - start_).count();
    }

  private:
    std::chrono::time_point<clock> start_;
    std::chrono::time_point<clock> current_;
  };

  using SteadyTimer = Timer<std::chrono::steady_clock>;
  using HighResolutionTimer = Timer<std::chrono::high_resolution_clock>;
} // namespace engine::core