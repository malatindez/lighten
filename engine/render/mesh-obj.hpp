#pragma once
#include <vector>
#include <filesystem>
#include <fstream>
#include "math.hpp"

namespace engine::render
{
    struct Vertex
    {
        math::vec3 position;
        math::vec3 normal;
        math::vec2 tex_coords;
    };
    class MeshObj
    {
    public:
        std::vector<Vertex> vertices;
        MeshObj() = default;
    };
    std::shared_ptr<const MeshObj> LoadMeshFromObj(std::filesystem::path const &path);
}