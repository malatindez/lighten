#include "cube.hpp"
namespace engine::components
{
    static inline std::array<math::vec3, 8> kCubeVertices{
        math::vec3{0.5f ,0.5f  ,-0.5f},
        math::vec3{0.5f ,-0.5f ,-0.5f},
        math::vec3{0.5f ,0.5f  ,0.5f},
        math::vec3{0.5f ,-0.5f ,0.5f},
        math::vec3{-0.5f, 0.5f ,-0.5f},
        math::vec3{-0.5f, -0.5f, -0.5f},
        math::vec3{-0.5f, 0.5f ,0.5f},
        math::vec3{-0.5f, -0.5f, 0.5f},
    };
    std::array<Triangle, 12> Cube::triangles_{
        Triangle{kCubeVertices[4], kCubeVertices[2], kCubeVertices[0]},
        Triangle{kCubeVertices[2], kCubeVertices[7], kCubeVertices[3]},
        Triangle{kCubeVertices[6], kCubeVertices[5], kCubeVertices[7]},
        Triangle{kCubeVertices[1], kCubeVertices[7], kCubeVertices[5]},
        Triangle{kCubeVertices[0], kCubeVertices[3], kCubeVertices[1]},
        Triangle{kCubeVertices[4], kCubeVertices[1], kCubeVertices[5]},
        Triangle{kCubeVertices[4], kCubeVertices[6], kCubeVertices[2]},
        Triangle{kCubeVertices[2], kCubeVertices[6], kCubeVertices[7]},
        Triangle{kCubeVertices[6], kCubeVertices[4], kCubeVertices[5]},
        Triangle{kCubeVertices[1], kCubeVertices[3], kCubeVertices[7]},
        Triangle{kCubeVertices[0], kCubeVertices[2], kCubeVertices[3]},
        Triangle{kCubeVertices[4], kCubeVertices[0], kCubeVertices[1]}};
} // namespace engine::components
/*

*/