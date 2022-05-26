#pragma once
#include "components/triangle.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
    class Cube
    {
    public:
        Cube() : cube_vertices_{
                     math::vec3{-0.5f, -0.5f, 0.5f},
                     math::vec3{0.5f, -0.5f, 0.5f},
                     math::vec3{-0.5f, 0.5f, 0.5f},
                     math::vec3{0.5f, 0.5f, 0.5f},
                     math::vec3{-0.5f, 0.5f, -0.5f},
                     math::vec3{0.5f, 0.5f, -0.5f},
                     math::vec3{-0.5f, -0.5f, -0.5f},
                     math::vec3{0.5f, -0.5f, -0.5f}},
                 triangles_{
                     Triangle{cube_vertices_[1], cube_vertices_[2], cube_vertices_[3]},
                     Triangle{cube_vertices_[7], cube_vertices_[6], cube_vertices_[5]},
                     Triangle{cube_vertices_[4], cube_vertices_[5], cube_vertices_[1]},
                     Triangle{cube_vertices_[5], cube_vertices_[6], cube_vertices_[2]},
                     Triangle{cube_vertices_[2], cube_vertices_[6], cube_vertices_[7]},
                     Triangle{cube_vertices_[0], cube_vertices_[3], cube_vertices_[7]},
                     Triangle{cube_vertices_[0], cube_vertices_[1], cube_vertices_[3]},
                     Triangle{cube_vertices_[4], cube_vertices_[7], cube_vertices_[5]},
                     Triangle{cube_vertices_[0], cube_vertices_[4], cube_vertices_[1]},
                     Triangle{cube_vertices_[1], cube_vertices_[5], cube_vertices_[2]},
                     Triangle{cube_vertices_[3], cube_vertices_[2], cube_vertices_[7]},
                     Triangle{cube_vertices_[4], cube_vertices_[0], cube_vertices_[7]}}
        {
            TransformUpdated(Transform{});
        }
        bool CheckIntersection(Transform const &transform, math::Intersection &i, math::Ray const &ray) const
        {
            bool rv = false;
            for (auto const &triangle : triangles_)
            {
                rv = triangle.CheckIntersection(transform, i, ray);
            }
            return rv;
        }
        void TransformUpdated(Transform const &new_transform)
        {
            static std::array<math::vec4, 8> const kCubeVertices{
                math::vec4{-0.5f, -0.5f, 0.5f, 1},
                math::vec4{0.5f, -0.5f, 0.5f, 1},
                math::vec4{-0.5f, 0.5f, 0.5f, 1},
                math::vec4{0.5f, 0.5f, 0.5f, 1},
                math::vec4{-0.5f, 0.5f, -0.5f, 1},
                math::vec4{0.5f, 0.5f, -0.5f, 1},
                math::vec4{-0.5f, -0.5f, -0.5f, 1},
                math::vec4{0.5f, -0.5f, -0.5f, 1},
            };
            auto model = new_transform.model();
            for (int i = 0; i < cube_vertices_.size(); i++)
            {
                cube_vertices_[i] = (kCubeVertices[i] * model).as_vec<3>();
            }
            for (auto &triangle : triangles_)
            {
                triangle.UpdateNormal();
            }
        }

    private:
        std::array<math::vec3, 8> cube_vertices_{};
        std::array<Triangle, 12> triangles_;
    };
}