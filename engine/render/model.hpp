#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/math.hpp"
#include "include/library-pch.hpp"
namespace engine::render
{
    struct Vertex
    {
        core::math::vec3 coords;
        core::math::vec2 uv;
    };

    struct Material
    {
        ID3D11ShaderResourceView *texture;
    };

    struct Mesh
    {
        struct MeshRange
        {
            uint32_t vertex_offset;
            uint32_t index_offset;
            uint32_t vertex_num;
            uint32_t index_num;
            core::math::AABB bounding_box;
        };

        std::vector<Material> materials;
        core::math::mat4 mesh_to_model;
        core::math::mat4 inv_mesh_to_model;
        MeshRange mesh_range;
    };

    struct Model
    {
        core::math::AABB bounding_box;
        std::vector<Mesh> meshes;
        direct3d::ImmutableVertexBuffer<Vertex> vertices;
        direct3d::ImmutableIndexBuffer<uint32_t> indices;
    };
}