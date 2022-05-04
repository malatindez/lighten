#pragma once

#include "interfaces/updatable.hpp"
#include "math/sphere.hpp"
#include "misc/bitmap-window.hpp"
namespace engine
{
    class Scene : public interfaces::Updatable
    {
    public:
        Scene(std::shared_ptr<BitmapWindow> window, math::Sphere sphere) : sphere_(sphere), window_(window) {}

        void SetBitmapWindow(std::shared_ptr<BitmapWindow> window);

        [[nodiscard]] math::Sphere &sphere() noexcept;

        void UpdateScene() noexcept;

        bool Update(float) override;

    private:
        void Draw();
        math::Sphere sphere_;
        bool update_scene_{true};
        std::shared_ptr<BitmapWindow> window_ = nullptr;
    };
}