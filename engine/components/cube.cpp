#include "cube.hpp"
namespace engine::components
{
    static inline std::array<math::vec3, 8> kCubeVertices{
        math::vec3{-0.5f, -0.5f, 0.5f},
        math::vec3{-0.5f, 0.5f, 0.5f},
        math::vec3{-0.5f, -0.5f, -0.5f},
        math::vec3{-0.5f, 0.5f, -0.5f},
        math::vec3{0.5f, -0.5f, 0.5f},
        math::vec3{0.5f, 0.5f, 0.5f},
        math::vec3{0.5f, -0.5f, -0.5f},
        math::vec3{0.5f, 0.5f, -0.5f},
    };
    static inline std::array<math::vec3, 6> kCubeNormals{
        math::vec3{0, 1, 0}, math::vec3{0, 0, 1}, math::vec3{-1, 0, 0},
        math::vec3{0, -1, 0}, math::vec3{1, 0, 0}, math::vec3{0, 0, -1}};
    std::array<Triangle, 12> Cube::triangles_{
        Triangle{kCubeVertices[4], kCubeVertices[2], kCubeVertices[0],
                 kCubeNormals[3]},
        Triangle{kCubeVertices[2], kCubeVertices[7], kCubeVertices[3],
                 kCubeNormals[1]},
        Triangle{kCubeVertices[6], kCubeVertices[5], kCubeVertices[7],
                 kCubeNormals[2]},
        Triangle{kCubeVertices[1], kCubeVertices[7], kCubeVertices[5],
                 kCubeNormals[0]},
        Triangle{kCubeVertices[0], kCubeVertices[3], kCubeVertices[1],
                 kCubeNormals[4]},
        Triangle{kCubeVertices[4], kCubeVertices[1], kCubeVertices[5],
                 kCubeNormals[5]},
        Triangle{kCubeVertices[4], kCubeVertices[6], kCubeVertices[2],
                 kCubeNormals[3]},
        Triangle{kCubeVertices[2], kCubeVertices[6], kCubeVertices[7],
                 kCubeNormals[1]},
        Triangle{kCubeVertices[6], kCubeVertices[4], kCubeVertices[5],
                 kCubeNormals[2]},
        Triangle{kCubeVertices[1], kCubeVertices[3], kCubeVertices[7],
                 kCubeNormals[0]},
        Triangle{kCubeVertices[0], kCubeVertices[2], kCubeVertices[3],
                 kCubeNormals[4]},
        Triangle{kCubeVertices[4], kCubeVertices[0], kCubeVertices[1],
                 kCubeNormals[5]}};

    bool Cube::CheckIntersection(Transform const &transform,
                                 math::Intersection &i, math::Ray const &ray)
    {
        math::Ray local = ray;
        local.direction() =
            (math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
        local.origin() =
            (math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();
        bool rv = false;
        for (auto const &triangle : triangles_)
        {
            rv |= triangle.CheckIntersection(transform, i, local);
        }
        if (rv)
        {
            i.normal =
                normalize((math::vec4{i.normal, 0} * transform.model).as_vec<3>());
            i.point = (math::vec4{i.point, 1} * transform.model).as_vec<3>();
        }
        return rv;
    }
} // namespace engine::components
