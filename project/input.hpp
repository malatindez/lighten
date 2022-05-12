#pragma once
#include "core/events.hpp"
#include "core/layer.hpp"
#include "math.hpp"

class Input : public engine::Layer
{
public:
    void OnEvent(engine::Event &event) override;
    void flush() noexcept { key_states_.clear(); }

    [[nodiscard]] inline bool key_state(uint32_t key) const noexcept
    {
        if (!key_states_.contains(key)) 
        {
            return false;
        }
        return key_states_.at(key);
    }
    [[nodiscard]] constexpr bool lbutton_down() const noexcept
    {
        return lbuttonstate_;
    }
    [[nodiscard]] constexpr bool rbutton_down() const noexcept
    {
        return rbuttonstate_;
    }
    [[nodiscard]] constexpr engine::math::ivec2 const &mouse_position() const noexcept
    {
        return mouse_position_;
    }
private:
    std::unordered_map<uint32_t, bool> key_states_;
    bool lbuttonstate_ = false;
    bool rbuttonstate_ = false;
    engine::math::ivec2 mouse_position_ {0, 0};
};