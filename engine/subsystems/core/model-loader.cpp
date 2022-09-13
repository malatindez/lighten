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
        Mesh SetupMesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices, uint32_t material_id, aiMatrix4x4 const &transformation, Mesh::MeshRange &&mesh_range)
        {
            auto temp = math::mat4(transformation.a1, transformation.a2, transformation.a3, transformation.a4,
                                   transformation.b1, transformation.b2, transformation.b3, transformation.b4,
                                   transformation.c1, transformation.c2, transformation.c3, transformation.c4,
                                   transformation.d1, transformation.b2, transformation.d3, transformation.d4);
            return Mesh(material_id, temp, std::move(mesh_range), std::move(vertices), std::move(indices));
        }

        void processMesh(std::vector<Vertex> &vertices_dest,
                         std::vector<uint32_t> &indices_dest,
                         std::vector<Mesh> &meshes,
                         aiMesh *mesh,
                         aiMatrix4x4 const &transformation)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                vertices.emplace_back(Vertex{
                    .coords = math::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                    .uv = math::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} });
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
            Mesh mesh__ = SetupMesh(std::move(vertices), std::move(indices), mesh->mMaterialIndex, transformation, Mesh::MeshRange{
                // we are gonna fill offsets later
                .vertex_offset = std::numeric_limits<uint32_t>::max(),
                .index_offset = std::numeric_limits<uint32_t>::max(),
                .vertex_num = mesh->mNumVertices,
                .index_num = mesh->mNumFaces * 3,
                .bounding_box = math::Box {
                    .min = math::vec3{mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
                    .max = math::vec3{mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}
                } }
            );
            meshes.push_back(std::move(mesh__));
        }

        void processNode(std::vector<Vertex> &vertices,
                         std::vector<uint32_t> &indices,
                         std::vector<Mesh> &meshes,
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
        std::vector<Mesh> meshes;
        std::vector<Material> materials;

        for (size_t material_index = 0; material_index < scene_ptr->mNumMaterials; material_index++)
        {
            aiMaterial *material = scene_ptr->mMaterials[material_index];
            for (UINT i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, i, &str);
                materials.emplace_back(Material{ .texture = TextureManager::GetTextureView(TextureManager::LoadTexture(model_folder / str.C_Str())) });
            }
        }


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
            index_offset += mesh_range.index_num;
            vertex_offset += mesh_range.vertex_num;
        }
        for (auto &mesh : meshes)
        {
            mesh.triangle_octree.initialize(mesh);
        }

        uint64_t rv = ModelSystem::instance().AddModel(Model{
                .bounding_box = math::Box{.min = min, .max = max},
                .meshes = std::move(meshes),
                .materials = std::move(materials),
                .vertices = direct3d::ImmutableVertexBuffer<Vertex>(vertices),
                .indices = direct3d::ImmutableIndexBuffer<uint32_t>(indices)
                                                       });
        instance_->models_.emplace(std::pair<size_t, uint64_t>{std::filesystem::hash_value(path), rv});
        return rv;
    }

    std::shared_ptr<ModelLoader> ModelLoader::instance_;
} // namespace engine::core