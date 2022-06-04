#pragma once
#include "core/events.hpp"
#include "core/layer.hpp"
#include "math.hpp"

class Input : public engine::core::Layer
{
public:
    void OnEvent(engine::core::events::Event &event) override;
    void flush() noexcept
    {
        mouse_scrolled_ = false;
        scroll_delta_ = 0;
    }

    [[nodiscard]] inline bool key_state(uint32_t key) const noexcept
    {
        if (!key_states_.contains(key))
        {
            return false;
        }
        return key_states_.at(key);
    }
    [[nodiscard]] constexpr bool lbutton_down() const noexcept { return lbuttonstate_; }
    [[nodiscard]] constexpr bool rbutton_down() const noexcept { return rbuttonstate_; }
    [[nodiscard]] constexpr engine::core::math::ivec2 const &mouse_position() const noexcept { return mouse_position_; }
    [[nodiscard]] constexpr bool mouse_scrolled() const noexcept { return mouse_scrolled_; }
    [[nodiscard]] constexpr int16_t scroll_delta() const noexcept { return scroll_delta_; }

private:
    std::unordered_map<uint32_t, bool> key_states_;
    bool lbuttonstate_ = false;
    bool rbuttonstate_ = false;
    bool mouse_scrolled_ = false;
    int16_t scroll_delta_ = 0;
    engine::core::math::ivec2 mouse_position_{0};
};