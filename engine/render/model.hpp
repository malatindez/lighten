#pragma once
#include "direct3d11/direct3d11.hpp"
#include "core/math.hpp"
#include "include/library-pch.hpp"
#include "core/triangle-octree.hpp"
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
            core::math::Box bounding_box;
        };
        Mesh() = default;
        Mesh(uint32_t loaded_material_id, core::math::mat4 mesh_to_model, MeshRange &&mesh_range, std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices)
            : loaded_material_id(loaded_material_id), mesh_to_model(mesh_to_model), inv_mesh_to_model(core::math::inverse(mesh_to_model)), mesh_range(std::move(mesh_range)), vertices(std::move(vertices)), indices(std::move(indices)) {}
        Mesh(Mesh &&) = default;
        Mesh &operator=(Mesh &&) = default;
        // copy semantic is disabled because of TriangleOctree
        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;

        uint32_t loaded_material_id = std::numeric_limits<uint32_t>::max();
        core::math::mat4 mesh_to_model;
        core::math::mat4 inv_mesh_to_model;
        MeshRange mesh_range;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        core::TriangleOctree triangle_octree;
    };

    struct Model
    {
        core::math::Box bounding_box;
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
        direct3d::ImmutableVertexBuffer<Vertex> vertices;
        direct3d::ImmutableIndexBuffer<uint32_t> indices;
    };
}