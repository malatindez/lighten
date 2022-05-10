#include "misc/scene.hpp"
using namespace engine::math;

namespace engine
{
    void Scene::UpdateScene() noexcept
    {
        update_scene_ = true;
    }

    vec3 color(Sphere const &sphere, Ray const &r)
    {
        if (sphere.Hit(r) >= 0)
        {
            return vec3{1, 0, 0};
        }
        vec3 unit_direction = normalize(r.direction());

        float t = 0.5f * (unit_direction.y + 1);

        return (1.0f - t) * vec3{1} + t * vec3{0.5f, 0.7f, 1.0f};
    }

    void Scene::Draw(BitmapWindow &window)
    {
        if (!update_scene_)
        {
            //return;
        }
        vec3 lower_left_corner{-2, -1, -1};
        vec3 horizontal{4, 0, 0};
        vec3 vertical{0, 2, 0};
        vec3 origin{0};
        ivec2 window_size = window.size();
        uint32_t *bitmap = window.bitmap().data();
        for (int j = window_size.y - 1; j >= 0; j--)
        {
            for (int i = 0; i < window_size.x; i++)
            {
                float u = float(i) / float(window_size.x);
                float v = float(j) / float(window_size.y);

                Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
                vec3 col = color(sphere, r);

                auto ir = int(255.99 * col.r) << 16;
                auto ig = int(255.99 * col.g) << 8;
                auto ib = int(255.99 * col.b);

                bitmap[size_t(j) * window_size.x + i] = ir | ig | ib;
            }
        }
        update_scene_ = false;
    }
}