#include "model-loader.hpp"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "texture-manager.hpp"
#include "../render/model-system.hpp"

namespace engine::core
{
    using namespace render;
    namespace
    {
        ModelMesh SetupMesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices, uint32_t material_id, aiMatrix4x4 const &transformation, MeshRange &&mesh_range)
        {
            auto temp = math::mat4(transformation.a1, transformation.a2, transformation.a3, transformation.a4,
                                   transformation.b1, transformation.b2, transformation.b3, transformation.b4,
                                   transformation.c1, transformation.c2, transformation.c3, transformation.c4,
                                   transformation.d1, transformation.b2, transformation.d3, transformation.d4);
            return ModelMesh(material_id, temp, std::move(mesh_range), Mesh{ .vertices = std::move(vertices), .indices = std::move(indices) });
        }

        void processMesh(std::vector<Vertex> &vertices_dest,
                         std::vector<uint32_t> &indices_dest,
                         std::vector<ModelMesh> &meshes,
                         aiMesh *mesh,
                         aiMatrix4x4 const &transformation)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                vertices.emplace_back(Vertex{
                    .position = math::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                    .tex_coord = math::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y},
                    .normal = math::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                    .tangent = math::vec3{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z},
                    .bitangent = math::vec3{mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z} });
            }
            for (uint32_t i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace const &face = mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }
            vertices_dest.insert(vertices_dest.end(), vertices.begin(), vertices.end());
            indices_dest.insert(indices_dest.end(), indices.begin(), indices.end());
            ModelMesh mesh__ = SetupMesh(std::move(vertices), std::move(indices), mesh->mMaterialIndex, transformation, MeshRange{
                // we are gonna fill offsets later
                .vertex_offset = std::numeric_limits<uint32_t>::max(),
                .index_offset = std::numeric_limits<uint32_t>::max(),
                .vertex_count = mesh->mNumVertices,
                .index_count = mesh->mNumFaces * 3,
                .bounding_box = math::Box {
                    .min = math::vec3{mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
                    .max = math::vec3{mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}
                } }
            );
            meshes.push_back(std::move(mesh__));
        }

        void processNode(std::vector<Vertex> &vertices,
                         std::vector<uint32_t> &indices,
                         std::vector<ModelMesh> &meshes,
                         aiNode *node,
                         aiScene const *scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                processMesh(vertices, indices, meshes, mesh, node->mTransformation);
            }

            for (UINT i = 0; i < node->mNumChildren; i++)
            {
                processNode(vertices, indices, meshes, node->mChildren[i], scene);
            }
        }

        [[nodiscard]] static constexpr Material::ShadingMode ShadingModeFromAssimp(uint32_t shading_mode)
        {
            switch (shading_mode)
            {
            case aiShadingMode_Flat: return Material::ShadingMode::Flat;
            case aiShadingMode_Gouraud: return Material::ShadingMode::Gouraud;
            case aiShadingMode_Phong: return Material::ShadingMode::Phong;
            case aiShadingMode_Blinn: return Material::ShadingMode::Blinn;
            case aiShadingMode_Toon: return Material::ShadingMode::Toon;
            case aiShadingMode_OrenNayar: return Material::ShadingMode::OrenNayar;
            case aiShadingMode_Minnaert: return Material::ShadingMode::Minnaert;
            case aiShadingMode_CookTorrance: return Material::ShadingMode::CookTorrance;
            case aiShadingMode_Fresnel: return Material::ShadingMode::Fresnel;
            case aiShadingMode_PBR_BRDF: return Material::ShadingMode::PBR_BRDF;
            default: return Material::ShadingMode::None;
            }
        }
        void ProcessMaterials(std::vector<Material> &materials, aiScene const *scene_ptr, std::filesystem::path const &model_folder);
    } // namespace

    std::optional<uint64_t> ModelLoader::Load(std::filesystem::path const &path)
    {
        if (auto it = instance_->models_.find(std::filesystem::hash_value(path));
            it != instance_->models_.end())
        {
            return it->second;
        }

        Assimp::Importer importer;
        aiScene const *scene_ptr = importer.ReadFile(path.string().c_str(),
                                                     aiProcess_Triangulate |
                                                     aiProcess_MakeLeftHanded |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_FlipWindingOrder |
                                                     aiProcess_JoinIdenticalVertices |
                                                     (uint32_t)aiProcess_GenBoundingBoxes |
                                                     aiProcess_CalcTangentSpace);

        if (scene_ptr == nullptr)
        {
            utils::AlwaysAssert(false, "Failed to load model @ " + path.string());
            return std::nullopt;
        }
        auto model_folder = std::filesystem::absolute(path.parent_path());

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<ModelMesh> meshes;
        std::vector<Material> materials;
        ProcessMaterials(materials, scene_ptr, model_folder);
        processNode(vertices, indices, meshes, scene_ptr->mRootNode, scene_ptr);
        uint32_t index_offset = 0;
        uint32_t vertex_offset = 0;
        math::vec3 min{ std::numeric_limits<float>::max() };
        math::vec3 max{ std::numeric_limits<float>::min() };

        for (auto &mesh : meshes)
        {
            auto &mesh_range = mesh.mesh_range;
            auto mesh_min = (math::vec4(mesh_range.bounding_box.min, 1) * mesh.mesh_to_model).xyz;
            auto mesh_max = (math::vec4(mesh_range.bounding_box.max, 1) * mesh.mesh_to_model).xyz;
            math::rmin(min, mesh_min);
            math::rmin(min, mesh_max);
            math::rmax(max, mesh_min);
            math::rmax(max, mesh_max);

            mesh_range.index_offset = index_offset;
            mesh_range.vertex_offset = vertex_offset;
            index_offset += mesh_range.index_count;
            vertex_offset += mesh_range.vertex_count;
        }
        for (auto &mesh : meshes)
        {
            mesh.triangle_octree.initialize(mesh);
        }

        uint64_t rv = ModelSystem::instance().AddModel(Model{
                .name = scene_ptr->mName.C_Str(),
                .bounding_box = math::Box{.min = min, .max = max},
                .meshes = std::move(meshes),
                .materials = std::move(materials),
                .vertices = direct3d::ImmutableVertexBuffer<Vertex>(vertices),
                .indices = direct3d::ImmutableIndexBuffer<uint32_t>(indices) });
        instance_->models_.emplace(std::pair<size_t, uint64_t>{std::filesystem::hash_value(path), rv});
        return rv;
    }

    namespace
    {
        void ProcessMaterials(std::vector<Material> &materials, aiScene const *scene_ptr, std::filesystem::path const &model_folder)
        {
            aiString str;
            ai_real value;
            aiColor3D color;
            int integer;

            for (size_t material_index = 0; material_index < scene_ptr->mNumMaterials; material_index++)
            {
                aiMaterial *material = scene_ptr->mMaterials[material_index];
                Material result;
                if (AI_SUCCESS != material->Get(AI_MATKEY_NAME, str)) { str = aiString("n/a"); }
                result.name = str.C_Str();

#pragma region Load properties
                if (AI_SUCCESS != material->Get(AI_MATKEY_SHADING_MODEL, integer)) { integer = aiShadingMode_Blinn; }
                result.shading_mode = ShadingModeFromAssimp(integer);
                if (AI_SUCCESS != material->Get(AI_MATKEY_BLEND_FUNC, integer)) { integer = 0; }
                result.blend_mode = integer == aiBlendMode_Default ? Material::BlendMode::Default : Material::BlendMode::Add;

                if (AI_SUCCESS != material->Get(AI_MATKEY_BUMPSCALING, value)) { value = 1.0f; }
                result.bump_scaling = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.diffuse_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.specular_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.ambient_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.emissive_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_TRANSPARENT, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.transparent_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_REFLECTIVE, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.reflective_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, value)) { value = 0.0f; }
                result.shininess = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS_STRENGTH, value)) { value = 1.0f; }
                result.shininess_strength = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, value)) { value = 1.0f; }
                result.opacity = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_REFRACTI, value)) { value = 1.0f; }
                result.refraction = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_REFLECTIVITY, value)) { value = 0.0f; }
                result.reflectivity = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_ENABLE_WIREFRAME, integer)) { integer = 0; }
                result.enable_wireframe = integer != 0;
                if (AI_SUCCESS != material->Get(AI_MATKEY_TWOSIDED, integer)) { integer = 0; }
                result.twosided = integer != 0;

                // load use_color_map
                if (AI_SUCCESS != material->Get(AI_MATKEY_USE_COLOR_MAP, integer)) { integer = 0; }
                result.use_color_map = integer != 0;
                if (AI_SUCCESS != material->Get(AI_MATKEY_USE_METALLIC_MAP, integer)) { integer = 0; }
                result.use_metallic_map = integer != 0;
                if (AI_SUCCESS != material->Get(AI_MATKEY_USE_ROUGHNESS_MAP, integer)) { integer = 0; }
                result.use_normal_map = integer != 0;
                if (AI_SUCCESS != material->Get(AI_MATKEY_USE_EMISSIVE_MAP, integer)) { integer = 0; }
                result.use_emissive_map = integer != 0;
                if (AI_SUCCESS != material->Get(AI_MATKEY_USE_AO_MAP, integer)) { integer = 0; }
                result.use_ambient_occlusion_map = integer != 0;

                if (AI_SUCCESS != material->Get(AI_MATKEY_BASE_COLOR, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.base_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_METALLIC_FACTOR, value)) { value = 0.0f; }
                result.metalness = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value)) { value = 0.0f; }
                result.roughness = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_ANISOTROPY_FACTOR, value)) { value = 0.0f; }
                result.anisotropy = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_SPECULAR_FACTOR, value)) { value = 0.0f; }
                result.specular = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_GLOSSINESS_FACTOR, value)) { value = 0.0f; }
                result.glossiness = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_SHEEN_COLOR_FACTOR, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.sheen_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_CLEARCOAT_FACTOR, value)) { value = 0.0f; }
                result.clearcoat = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, value)) { value = 0.0f; }
                result.clearcoat_roughness = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_TRANSMISSION_FACTOR, value)) { value = 0.0f; }
                result.transmission = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_VOLUME_THICKNESS_FACTOR, value)) { value = 0.0f; }
                result.volume_thickness = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_VOLUME_ATTENUATION_DISTANCE, value)) { value = 0.0f; }
                result.volume_attenuation_distance = value;
                if (AI_SUCCESS != material->Get(AI_MATKEY_VOLUME_ATTENUATION_COLOR, color)) { color = aiColor3D(0.0f, 0.0f, 0.0f); }
                result.volume_attenuation_color = core::math::vec3{ color.r, color.g, color.b };
                if (AI_SUCCESS != material->Get(AI_MATKEY_EMISSIVE_INTENSITY, value)) { value = 0.0f; }
                result.emissive_intensity = value;

#pragma endregion

#pragma region Load textures
                result.none_textures.reserve(material->GetTextureCount(aiTextureType_NONE));
                result.diffuse_textures.reserve(material->GetTextureCount(aiTextureType_DIFFUSE));
                result.specular_textures.reserve(material->GetTextureCount(aiTextureType_SPECULAR));
                result.ambient_textures.reserve(material->GetTextureCount(aiTextureType_AMBIENT));
                result.emissive_textures.reserve(material->GetTextureCount(aiTextureType_EMISSIVE));
                result.height_textures.reserve(material->GetTextureCount(aiTextureType_AMBIENT));
                result.normal_textures.reserve(material->GetTextureCount(aiTextureType_HEIGHT));
                result.shininess_textures.reserve(material->GetTextureCount(aiTextureType_SHININESS));
                result.opacity_textures.reserve(material->GetTextureCount(aiTextureType_OPACITY));
                result.displacement_textures.reserve(material->GetTextureCount(aiTextureType_DISPLACEMENT));
                result.lightmap_textures.reserve(material->GetTextureCount(aiTextureType_LIGHTMAP));
                result.reflection_textures.reserve(material->GetTextureCount(aiTextureType_REFLECTION));
                result.base_color_textures.reserve(material->GetTextureCount(aiTextureType_BASE_COLOR));
                result.normal_camera_textures.reserve(material->GetTextureCount(aiTextureType_NORMAL_CAMERA));
                result.emission_color_textures.reserve(material->GetTextureCount(aiTextureType_EMISSION_COLOR));
                result.metalness_textures.reserve(material->GetTextureCount(aiTextureType_METALNESS));
                result.diffuse_roughness_textures.reserve(material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS));
                result.ambient_occlusion_textures.reserve(material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION));
                result.sheen_textures.reserve(material->GetTextureCount(aiTextureType_SHEEN));
                result.clearcoat_textures.reserve(material->GetTextureCount(aiTextureType_CLEARCOAT));
                result.transmission_textures.reserve(material->GetTextureCount(aiTextureType_TRANSMISSION));

                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_NONE); i++)
                {
                    material->GetTexture(aiTextureType_NONE, i, &str);
                    result.none_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
                {
                    material->GetTexture(aiTextureType_DIFFUSE, i, &str);
                    result.diffuse_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
                {
                    material->GetTexture(aiTextureType_SPECULAR, i, &str);
                    result.specular_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_AMBIENT); i++)
                {
                    material->GetTexture(aiTextureType_AMBIENT, i, &str);
                    result.ambient_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_EMISSIVE); i++)
                {
                    material->GetTexture(aiTextureType_EMISSIVE, i, &str);
                    result.emissive_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_HEIGHT); i++)
                {
                    material->GetTexture(aiTextureType_HEIGHT, i, &str);
                    result.height_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
                {
                    material->GetTexture(aiTextureType_NORMALS, i, &str);
                    result.normal_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_SHININESS); i++)
                {
                    material->GetTexture(aiTextureType_SHININESS, i, &str);
                    result.shininess_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_OPACITY); i++)
                {
                    material->GetTexture(aiTextureType_OPACITY, i, &str);
                    result.opacity_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_DISPLACEMENT); i++)
                {
                    material->GetTexture(aiTextureType_DISPLACEMENT, i, &str);
                    result.displacement_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_LIGHTMAP); i++)
                {
                    material->GetTexture(aiTextureType_LIGHTMAP, i, &str);
                    result.lightmap_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_REFLECTION); i++)
                {
                    material->GetTexture(aiTextureType_REFLECTION, i, &str);
                    result.reflection_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; result.use_color_map && i < material->GetTextureCount(aiTextureType_BASE_COLOR); i++)
                {
                    material->GetTexture(aiTextureType_BASE_COLOR, i, &str);
                    result.base_color_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_NORMAL_CAMERA); i++)
                {
                    material->GetTexture(aiTextureType_NORMAL_CAMERA, i, &str);
                    result.normal_camera_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_EMISSION_COLOR); i++)
                {
                    material->GetTexture(aiTextureType_EMISSION_COLOR, i, &str);
                    result.emission_color_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_METALNESS); i++)
                {
                    material->GetTexture(aiTextureType_METALNESS, i, &str);
                    result.metalness_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); i++)
                {
                    material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, i, &str);
                    result.diffuse_roughness_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); i++)
                {
                    material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, i, &str);
                    result.ambient_occlusion_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_SHEEN); i++)
                {
                    material->GetTexture(aiTextureType_SHEEN, i, &str);
                    result.sheen_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_CLEARCOAT); i++)
                {
                    material->GetTexture(aiTextureType_CLEARCOAT, i, &str);
                    result.clearcoat_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_TRANSMISSION); i++)
                {
                    material->GetTexture(aiTextureType_TRANSMISSION, i, &str);
                    result.transmission_textures.emplace_back(TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())));
                }
#pragma endregion
                materials.emplace_back(std::move(result));
            }
        }
    }
    // create a unit sphere using
    render::Mesh create_unit_sphere(uint32_t precision)
    {
        render::Mesh return_value;
        return_value.vertices.resize(precision * precision);
        return_value.indices.resize(precision * precision * 6);
        for (uint32_t i = 0; i < precision; i++)
        {
            for (uint32_t j = 0; j < precision; j++)
            {
                float x = (float)i / (float)precision;
                float y = (float)j / (float)precision;
                float theta = x * 2.0f * (float)core::math::numbers::pi;
                float phi = y * (float)core::math::numbers::pi;
                float sin_theta = sin(theta);
                float cos_theta = cos(theta);
                float sin_phi = sin(phi);
                float cos_phi = cos(phi);
                float u = x;
                float v = y;
                return_value.vertices[i * precision + j].position = core::math::vec3{ sin_phi * cos_theta, cos_phi, sin_phi * sin_theta };
                return_value.vertices[i * precision + j].tex_coord = core::math::vec2{ u, v };
            }
        }
        for (uint32_t i = 0; i < precision - 1; i++)
        {
            for (uint32_t j = 0; j < precision - 1; j++)
            {
                return_value.indices[(i * precision + j) * 6 + 0] = i * precision + j;
                return_value.indices[(i * precision + j) * 6 + 1] = i * precision + j + 1;
                return_value.indices[(i * precision + j) * 6 + 2] = (i + 1) * precision + j;
                return_value.indices[(i * precision + j) * 6 + 3] = (i + 1) * precision + j;
                return_value.indices[(i * precision + j) * 6 + 4] = i * precision + j + 1;
                return_value.indices[(i * precision + j) * 6 + 5] = (i + 1) * precision + j + 1;
            }
        }
        return return_value;
    }
    std::shared_ptr<ModelLoader> ModelLoader::instance_;
} // namespace engine::core