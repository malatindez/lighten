#pragma once
#include "interfaces/updatable.hpp"
#include "render/scene.hpp"
#include "core/window.hpp"

#include <memory>
#include <unordered_map>

class Controller : public engine::interfaces::Updatable
{
public:
    Controller(engine::BitmapWindow &window, std::shared_ptr<engine::Scene> scene);

    [[nodiscard]] inline bool key_state(uint64_t key) const noexcept
    {
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

    bool Update(float delta_time) override;

private:
    LRESULT OnKeyDown(engine::Window &, HWND, UINT, WPARAM, LPARAM);
    LRESULT OnKeyRelease(engine::Window &, HWND, UINT, WPARAM, LPARAM);

    LRESULT OnRButtonDown(engine::Window &, HWND, UINT, WPARAM, LPARAM);
    LRESULT OnRButtonUp(engine::Window &, HWND, UINT, WPARAM, LPARAM);

    LRESULT OnLButtonDown(engine::Window &, HWND, UINT, WPARAM, LPARAM);
    LRESULT OnLButtonUp(engine::Window &, HWND, UINT, WPARAM, LPARAM);

    LRESULT OnExitSizeMove(engine::Window &, HWND, UINT, WPARAM, LPARAM) const;

    LRESULT OnDestroy(engine::Window &, HWND, UINT, WPARAM, LPARAM) const;

    std::unordered_map<uint64_t, bool> key_states_;
    bool lbuttonstate_ = false;
    bool rbuttonstate_ = false;
    std::shared_ptr<engine::Scene> scene_;
    engine::BitmapWindow &window_;
};