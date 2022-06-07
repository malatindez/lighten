#pragma once
#include "math.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

namespace engine::render
{
    struct Vertex
    {
        core::math::vec3 position;
        core::math::vec3 normal;
        core::math::vec2 tex_coords;
    };
    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        constexpr Mesh() = default;
        explicit constexpr Mesh(std::vector<Vertex> const &v) : vertices{v} {}
    };
    std::shared_ptr<const Mesh> LoadMeshFromObj(std::filesystem::path const &path);
} // namespace engine::render