#include <Windows.h>

#include "engine.hpp"
using namespace engine;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                    int cmd_show) {
  Engine::Start(instance, prev_instance, cmd_line, cmd_show);
  return 0;
}