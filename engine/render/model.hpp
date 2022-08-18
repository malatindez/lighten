#pragma once
#include "include/pch.hpp"
#include "direct3d11/direct3d11.hpp"
#include "core/math.hpp"
namespace engine::render
{
    using VertexBuffer = direct3d::Buffer;
    using IndexBuffer = direct3d::Buffer;

    struct Vertex
    {
        core::math::vec3 coords;
        core::math::vec2 uv;
    };

    struct Mesh
    {
        VertexBuffer vertices;
        IndexBuffer indices;
        uint32_t indices_size;
        std::vector<direct3d::ShaderResourceView> textures;
        core::math::mat4 mesh_to_model;
    };

    struct Model
    {
        struct MeshRange
        {
            uint32_t vertexOffset; // offset in vertices
            uint32_t indexOffset; // offset in indices
            uint32_t vertexNum; // num of vertices
            uint32_t indexNum; // num of indices
        };
        std::vector<MeshRange> mesh_ranges;
        std::vector<Mesh> meshes;
    };
}