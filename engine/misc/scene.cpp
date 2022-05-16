#include "misc/scene.hpp"
#include "components/directional-light.hpp"
#include "components/point-light.hpp"
#include "components/spot-light.hpp"

using namespace engine::math;

namespace engine
{
  void Scene::UpdateScene() noexcept { update_scene = true; }

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
    if (!update_scene)
    {
      // return;
    }
    update_scene = false;

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
        
        float u = static_cast<float>(i) / static_cast<float>(window_size.x);
        float v = static_cast<float>(j) / static_cast<float>(window_size.y);

        Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
        
        float t = sphere.Hit(r);
        if(t <= 0)
        {
          bitmap[size_t(j) * window_size.x + i] = 0;
          continue;
        }

        math::vec3 point = r.PointAtParameter(t);

        components::LightData ld {
          .color = math::vec3{0,0,0},
          .ray = r,
          .point = point,
          .normal = normalize(point - sphere.center()),
          .view_dir = normalize(point - r.origin())
        };
        for(auto i : spot_lights) 
        {
          i.UpdateColor(ld);
        }
        for(auto i : point_lights) 
        {
          i.UpdateColor(ld);
        }
        for(auto i : directional_lights) 
        {
          i.UpdateColor(ld);
        }

        auto ir = static_cast<int>(255.99 * ld.color.r) << 16;
        auto ig = static_cast<int>(255.99 * ld.color.g) << 8;
        auto ib = static_cast<int>(255.99 * ld.color.b);

        bitmap[size_t(j) * window_size.x + i] = ir | ig | ib;
      }
    }
  }
} // namespace engine