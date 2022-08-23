#pragma once
#include "core/math.hpp"
#include "direct3d11/direct3d11.hpp"
#include "include/pch.hpp"
namespace engine::render
{
    struct Vertex
    {
        core::math::vec3 coords;
        core::math::vec2 uv;
    };

    struct Material
    {
        direct3d::ShaderResourceView texture;
    };

    struct Mesh
    {
        direct3d::VertexBuffer<Vertex> vertices;
        direct3d::IndexBuffer<uint32_t> indices;
        uint32_t indices_size;
        std::vector<Material> materials;
        core::math::mat4 mesh_to_model;
    };

    struct Model
    {
        struct MeshRange
        {
            uint32_t vertex_offset;
            uint32_t index_offset;
            uint32_t vertex_num;
            uint32_t index_num;
            core::math::AABB bounding_box;
        };
        std::vector<MeshRange> mesh_ranges;
        std::vector<Mesh> meshes;
    };
}