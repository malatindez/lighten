#pragma once
#include "events/key-events.hpp"
#include "events/mouse-events.hpp"
#include "events/util-events.hpp"
#include "events/window-events.hpp"
#include "include/library-pch.hpp"

namespace lighten::core
{
    using EventCallbackFn = std::function<void(core::events::Event &)>;
} // namespace lighten::core