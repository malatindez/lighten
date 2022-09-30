#pragma once
#include "core/math.hpp"
#include "core/triangle-octree.hpp"
#include "direct3d11/direct3d11.hpp"
#include "include/library-pch.hpp"
namespace engine::render
{
    // Global material properties loaded from assimp
    // Rendering mode will use these properties to determine how to render the model
    // Some of this properties will not be used depending on the renderer
    struct Material
    {
        enum class BlendMode : uint8_t
        {
            Default = 0,
            Add = 1,
            Multiply = 2,
            Alpha = 3,
            AddAlpha = 4,
            Modulate = 5,
            Modulate2x = 6,
            Off = 7,
            Subtract = 8,
            Dot3 = 9,
            Dot4 = 10,
            Modulate4x = 11,
            AddSigned = 12,
            AddSigned2x = 13,
            AddSmooth = 14,
            BlendDiffuseAlpha = 15,
            BlendTextureAlpha = 16,
            BlendFactorAlpha = 17,
            BlendTextureAlphaPM = 18,
            BlendCurrentAlpha = 19,
            PreMultiplied = 20,
            ModulateAlphaAddColor = 21,
            ModulateColorAddAlpha = 22,
            ModulateInvAlphaAddColor = 23,
            ModulateInvColorAddAlpha = 24,
            BumpEnvMap = 25,
            BumpEnvMapLuminance = 26,
            Decal = 27,
            AddMult = 28,
            AddMult2x = 29,
            AddMultSigned = 30,
            AddMultSigned2x = 31,
            AddMultSmooth = 32,
            AddMultSmooth2x = 33,
            AddMultSmoothSigned = 34,
            AddMultSmoothSigned2x = 35,
            AddMultSmoothSignedAlpha = 36,
            AddMultSmoothSignedAlpha2x = 37,
        };
        enum class ShadingMode : uint8_t
        {
            None = 0,
            Flat = 1,
            Gouraud = 2,
            Phong = 3,
            Blinn = 4,
            Toon = 5,
            OrenNayar = 6,
            Minnaert = 7,
            CookTorrance = 8,
            Fresnel = 9,
            PBR_BRDF = 10
        };

        std::string name = "";
        ShadingMode shading_mode;
        BlendMode blend_mode;

        float bump_scaling;
        core::math::vec3 diffuse_color;
        core::math::vec3 specular_color;
        core::math::vec3 ambient_color;
        core::math::vec3 emissive_color;
        core::math::vec3 transparent_color;
        core::math::vec3 reflective_color;
        bool enable_wireframe;
        bool twosided;
        float opacity;
        float shininess;
        float shininess_strength;
        float refraction;
        float reflectivity;

        bool use_color_map;
        bool use_metallic_map;
        bool use_normal_map;
        bool use_emissive_map;
        bool use_ambient_occlusion_map;

        core::math::vec3 base_color;
        float metalness;
        float roughness;
        float anisotropy;
        float specular;
        float glossiness;
        core::math::vec3 sheen_color;
        float clearcoat;
        float clearcoat_roughness;
        float transmission;
        float volume_thickness;
        float volume_attenuation_distance;
        core::math::vec3 volume_attenuation_color;
        float emissive_intensity;

        std::vector<ID3D11ShaderResourceView *> none_textures;
        std::vector<ID3D11ShaderResourceView *> diffuse_textures;
        std::vector<ID3D11ShaderResourceView *> specular_textures;
        std::vector<ID3D11ShaderResourceView *> ambient_textures;
        std::vector<ID3D11ShaderResourceView *> emissive_textures;
        std::vector<ID3D11ShaderResourceView *> height_textures;
        std::vector<ID3D11ShaderResourceView *> normal_textures;
        std::vector<ID3D11ShaderResourceView *> shininess_textures;
        std::vector<ID3D11ShaderResourceView *> opacity_textures;
        std::vector<ID3D11ShaderResourceView *> displacement_textures;
        std::vector<ID3D11ShaderResourceView *> lightmap_textures;
        std::vector<ID3D11ShaderResourceView *> reflection_textures;
        std::vector<ID3D11ShaderResourceView *> base_color_textures;
        std::vector<ID3D11ShaderResourceView *> normal_camera_textures;
        std::vector<ID3D11ShaderResourceView *> emission_color_textures;
        std::vector<ID3D11ShaderResourceView *> metalness_textures;
        std::vector<ID3D11ShaderResourceView *> diffuse_roughness_textures;
        std::vector<ID3D11ShaderResourceView *> ambient_occlusion_textures;
        std::vector<ID3D11ShaderResourceView *> sheen_textures;
        std::vector<ID3D11ShaderResourceView *> clearcoat_textures;
        std::vector<ID3D11ShaderResourceView *> transmission_textures;

        // TODO load these properties from assimp
        // Holds bytecode of shaders if available
    /*
        std::string vertex_shader = "";
        std::string pixel_shader = "";
        std::string geometry_shader = "";
        std::string tesselation_shader = "";
        std::string primitive_shader = "";
        std::string compute_shader = "";
    */
    };
    struct Vertex
    {
        core::math::vec3 position;
        core::math::vec2 tex_coord;
        core::math::vec3 normal;
        core::math::vec3 tangent;
        core::math::vec3 bitangent;
        static constexpr Vertex empty()
        {
            return Vertex{ core::math::vec3::zero(), core::math::vec2::zero(), core::math::vec3::zero(), core::math::vec3::zero(), core::math::vec3::zero() };
        }
    };
    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
    struct MeshRange
    {
        uint32_t vertex_offset;
        uint32_t index_offset;
        uint32_t vertex_count;
        uint32_t index_count;
        core::math::Box bounding_box;
    };

    struct ModelMesh
    {
        ModelMesh() = default;
        ModelMesh(uint32_t loaded_material_id, core::math::mat4 mesh_to_model, MeshRange &&mesh_range, Mesh &&mesh)
            : loaded_material_id(loaded_material_id),
            mesh_to_model(mesh_to_model),
            inv_mesh_to_model(core::math::inverse(mesh_to_model)),
            mesh_range(std::move(mesh_range)),
            mesh(std::move(mesh)) {}
        ModelMesh(ModelMesh &&) = default;
        ModelMesh &operator=(ModelMesh &&) = default;
        ModelMesh(ModelMesh const &) = delete;
        ModelMesh &operator=(ModelMesh const &) = delete;

        uint32_t loaded_material_id = std::numeric_limits<uint32_t>::max();
        core::math::mat4 mesh_to_model;
        core::math::mat4 inv_mesh_to_model;
        MeshRange mesh_range;
        Mesh mesh;
        core::TriangleOctree triangle_octree;
    };

    struct Model
    {
        std::string name;
        core::math::Box bounding_box;
        std::vector<ModelMesh> meshes;
        std::vector<Material> materials;
        direct3d::ImmutableVertexBuffer<Vertex> vertices;
        direct3d::ImmutableIndexBuffer<uint32_t> indices;
    };
}