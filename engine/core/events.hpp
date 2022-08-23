#pragma once
#include "events/key-events.hpp"
#include "events/mouse-events.hpp"
#include "events/util-events.hpp"
#include "events/window-events.hpp"
#include "include/pch.hpp"

namespace engine::core
{
    using EventCallbackFn = std::function<void(core::events::Event &)>;
} // namespace engine::core