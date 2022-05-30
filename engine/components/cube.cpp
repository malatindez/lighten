#include "cube.hpp"
namespace engine::components
{
    static inline std::array<math::vec3, 8> kCubeVertices{
        math::vec3{0.5f, 0.5f, -0.5f},
        math::vec3{0.5f, -0.5f, -0.5f},
        math::vec3{0.5f, 0.5f, 0.5f},
        math::vec3{0.5f, -0.5f, 0.5f},
        math::vec3{-0.5f, 0.5f, -0.5f},
        math::vec3{-0.5f, -0.5f, -0.5f},
        math::vec3{-0.5f, 0.5f, 0.5f},
        math::vec3{-0.5f, -0.5f, 0.5f},
    };
    static inline std::array<math::vec3, 6> kCubeNormals{
        math::vec3{0, 1, 0},
        math::vec3{0, 0, 1},
        math::vec3{-1, 0, 0},
        math::vec3{0, -1, 0},
        math::vec3{1, 0, 0},
        math::vec3{0, 0, -1}};
    std::array<Triangle, 12> Cube::triangles_{
        Triangle{kCubeVertices[4], kCubeVertices[2], kCubeVertices[0], kCubeNormals[3]},
        Triangle{kCubeVertices[2], kCubeVertices[7], kCubeVertices[3], kCubeNormals[1]},
        Triangle{kCubeVertices[6], kCubeVertices[5], kCubeVertices[7], kCubeNormals[2]},
        Triangle{kCubeVertices[1], kCubeVertices[7], kCubeVertices[5], kCubeNormals[0]},
        Triangle{kCubeVertices[0], kCubeVertices[3], kCubeVertices[1], kCubeNormals[4]},
        Triangle{kCubeVertices[4], kCubeVertices[1], kCubeVertices[5], kCubeNormals[5]},
        Triangle{kCubeVertices[4], kCubeVertices[6], kCubeVertices[2], kCubeNormals[3]},
        Triangle{kCubeVertices[2], kCubeVertices[6], kCubeVertices[7], kCubeNormals[1]},
        Triangle{kCubeVertices[6], kCubeVertices[4], kCubeVertices[5], kCubeNormals[2]},
        Triangle{kCubeVertices[1], kCubeVertices[3], kCubeVertices[7], kCubeNormals[0]},
        Triangle{kCubeVertices[0], kCubeVertices[2], kCubeVertices[3], kCubeNormals[4]},
        Triangle{kCubeVertices[4], kCubeVertices[0], kCubeVertices[1], kCubeNormals[5]}};
} // namespace engine::components
  /*
  
  */