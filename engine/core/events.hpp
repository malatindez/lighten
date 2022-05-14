#pragma once
#include "events/application-events.hpp"
#include "events/key-events.hpp"
#include "events/mouse-events.hpp"
#include "events/window-events.hpp"

#include <functional>

namespace engine
{
    using EventCallbackFn = std::function<void(Event &)>;
} // namespace engine