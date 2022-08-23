#include "model-loader.hpp"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "texture-manager.hpp"
#include "direct3d11/direct3d11.hpp"

namespace engine::core
{
    using namespace render;
    namespace
    {
        Mesh SetupMesh(std::vector<Vertex> const &vertices, std::vector<uint32_t> const &indices, std::vector<Material> &materials, aiMatrix4x4 const &transformation)
        {
            direct3d::VertexBuffer<Vertex> vb(vertices);
            direct3d::IndexBuffer<uint32_t> ib(indices);
            return Mesh {
                .vertices = vb,
                .indices = ib,
                .indices_size = (uint32_t)indices.size(),
                .materials = materials,
                .mesh_to_model = math::mat4(transformation.a1, transformation.a2, transformation.a3, transformation.a4,
                                            transformation.b1, transformation.b2, transformation.b3, transformation.b4,
                                            transformation.c1, transformation.c2, transformation.c3, transformation.c4,
                                            transformation.d1, transformation.b2, transformation.d3, transformation.d4) };
        }

        void processMesh(std::filesystem::path const &folder,
                         aiMesh *mesh,
                         aiMatrix4x4 const &transformation,
                         const aiScene *scene,
                         std::shared_ptr<Model> rv)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<Material> materials;
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                vertices.emplace_back(Vertex {
                    .coords = engine::core::math::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                    .uv = engine::core::math::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} });
            }
            for (uint32_t i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
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
                    materials.emplace_back(Material { .texture = TextureManager::instance()->GetTexture(folder / str.C_Str()) });
                }
            }
            rv->meshes.emplace_back(SetupMesh(vertices, indices, materials, transformation));

            rv->mesh_ranges.emplace_back(Model::MeshRange {
                // we are gonna fill offsets later
                .vertex_offset = std::numeric_limits<uint32_t>::max(),
                .index_offset = std::numeric_limits<uint32_t>::max(),
                .vertex_num = mesh->mNumVertices,
                .index_num = mesh->mNumFaces * 3 });
        }

        void processNode(std::filesystem::path const &folder, std::shared_ptr<Model> rv, aiNode *node, aiScene const *scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                processMesh(folder, mesh, node->mTransformation, scene, rv);
            }

            for (UINT i = 0; i < node->mNumChildren; i++)
            {
                processNode(folder, rv, node->mChildren[i], scene);
            }
        }
    } // namespace

    std::shared_ptr<Model> ModelLoader::Load(std::filesystem::path const &path)
    {
        Assimp::Importer importer;
        aiScene const *scene_ptr = importer.ReadFile(path.string().c_str(),
                                                     aiProcess_Triangulate |
                                                     aiProcess_MakeLeftHanded |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_FlipWindingOrder |
                                                     aiProcess_JoinIdenticalVertices |
                                                     aiProcess_GenBoundingBoxes);
        if (scene_ptr == nullptr)
        {
            utils::AlwaysAssert(false, "Failed to load model @ " + path.string());
            return nullptr;
        }
        auto rv = std::make_shared<Model>();

        processNode(std::filesystem::absolute(path.parent_path()), rv, scene_ptr->mRootNode, scene_ptr);
        uint32_t index_offset = 0;
        uint32_t vertex_offset = 0;
        for (auto &mesh_range : rv->mesh_ranges)
        {
            mesh_range.index_offset = index_offset;
            mesh_range.vertex_offset = vertex_offset;
            index_offset += mesh_range.index_num;
            vertex_offset += mesh_range.vertex_num;
        }
        return rv;
    }

    std::shared_ptr<ModelLoader> ModelLoader::instance_;
} // namespace engine::core