#pragma once
#include "misc/bitmap-window.hpp"
#include "pch.hpp"

namespace engine {

class Engine final {
 public:
  INT Join() const {
    window_->StartMainLoop();
    return 0;
  }

  static INT Start(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                   int cmd_show);
  // Start should be called before retrieving an engine otherwise we will
  // dereference nullptr and get undefined behaviour
  [[nodiscard]] static inline Engine &engine() noexcept { return *engine_; }

 private:
  Engine(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
         int cmd_show, std::unique_ptr<BitmapWindow> window);
  // delete move & copy semantics
  Engine(Engine &&Engine) = delete;
  Engine(Engine const &Engine) = delete;
  Engine &operator=(Engine &&Engine) = delete;
  Engine &operator=(Engine const &Engine) = delete;

  std::unique_ptr<BitmapWindow> window_;
  HINSTANCE instance_;
  HINSTANCE prev_instance_;
  static std::unique_ptr<Engine> engine_;
};
[[nodiscard]] inline Engine &GetEngine() { return Engine::engine(); }
}  // namespace engine