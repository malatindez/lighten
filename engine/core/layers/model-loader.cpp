#include "model-loader.hpp"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "texture-manager.hpp"
#include "render/model-system.hpp"

namespace engine::core
{
    using namespace render;
    namespace
    {
        Mesh SetupMesh(std::vector<Material> &&materials, aiMatrix4x4 const &transformation, Mesh::MeshRange &&mesh_range)
        {
            auto temp = math::mat4(transformation.a1, transformation.a2, transformation.a3, transformation.a4,
                transformation.b1, transformation.b2, transformation.b3, transformation.b4,
                transformation.c1, transformation.c2, transformation.c3, transformation.c4,
                transformation.d1, transformation.b2, transformation.d3, transformation.d4);
            return Mesh{
                .materials = std::move(materials),
                .mesh_to_model = temp,
                .inv_mesh_to_model = math::inverse(temp),
                .mesh_range = std::move(mesh_range)
            };
        }

        void processMesh(std::vector<Vertex> &vertices,
            std::vector<uint32_t> &indices,
            std::vector<Mesh> &meshes,
            std::filesystem::path const &folder,
            aiMesh *mesh,
            aiMatrix4x4 const &transformation,
            const aiScene *scene)
        {
            std::vector<Material> materials;
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

            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                for (UINT i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
                {
                    aiString str;
                    material->GetTexture(aiTextureType_DIFFUSE, i, &str);
                    materials.emplace_back(Material{ .texture = TextureManager::instance()->GetTexture(folder / str.C_Str()) });
                }
            }

            meshes.emplace_back(SetupMesh(std::move(materials), transformation, Mesh::MeshRange{
                // we are gonna fill offsets later
                .vertex_offset = std::numeric_limits<uint32_t>::max(),
                .index_offset = std::numeric_limits<uint32_t>::max(),
                .vertex_num = mesh->mNumVertices,
                .index_num = mesh->mNumFaces * 3,
                .bounding_box = math::AABB {
                    .min = math::vec3{mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
                    .max = math::vec3{mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}
                }
                }
            ));
        }

        void processNode(std::vector<Vertex> &vertices,
            std::vector<uint32_t> &indices,
            std::vector<Mesh> &meshes,
            std::filesystem::path const &folder, aiNode *node, aiScene const *scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                processMesh(vertices, indices, meshes, folder, mesh, node->mTransformation, scene);
            }

            for (UINT i = 0; i < node->mNumChildren; i++)
            {
                processNode(vertices, indices, meshes, folder, node->mChildren[i], scene);
            }
        }
    } // namespace

    std::optional<uint32_t> ModelLoader::Load(std::filesystem::path const &path)
    {
        if (auto it = models_.find(std::filesystem::hash_value(path));
            it != models_.end())
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
            (uint32_t)aiProcess_GenBoundingBoxes);

        if (scene_ptr == nullptr)
        {
            utils::AlwaysAssert(false, "Failed to load model @ " + path.string());
            return std::nullopt;
        }

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Mesh> meshes;

        processNode(vertices, indices, meshes, std::filesystem::absolute(path.parent_path()), scene_ptr->mRootNode, scene_ptr);
        uint32_t index_offset = 0;
        uint32_t vertex_offset = 0;
        for (auto &mesh : meshes)
        {
            auto &mesh_range = mesh.mesh_range;

            mesh_range.index_offset = index_offset;
            mesh_range.vertex_offset = vertex_offset;
            index_offset += mesh_range.index_num;
            vertex_offset += mesh_range.vertex_num;
        }

        uint32_t rv = ModelSystem::AddModel(Model{ .meshes = meshes,
                .vertices = direct3d::ImmutableVertexBuffer<Vertex>(vertices),
                .indices = direct3d::ImmutableIndexBuffer<uint32_t>(indices)
            });
        models_.emplace(std::pair<size_t, uint32_t>{std::filesystem::hash_value(path), rv});
        return rv;
    }

    std::shared_ptr<ModelLoader> ModelLoader::instance_;
} // namespace engine::core