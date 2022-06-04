#pragma once
#include "events/application-events.hpp"
#include "events/key-events.hpp"
#include "events/mouse-events.hpp"
#include "events/window-events.hpp"

#include <functional>

namespace engine::core
{
    using EventCallbackFn = std::function<void(core::events::Event &)>;
} // namespace engine::core