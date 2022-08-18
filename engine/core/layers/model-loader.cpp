#include "model-loader.hpp"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "texture-manager.hpp"

namespace engine::core
{
    using namespace render;
    namespace
    {
        Mesh SetupMesh(std::vector<Vertex> const &vertices, std::vector<uint32_t> const &indices, std::vector<direct3d::ShaderResourceView> &textures, aiMatrix4x4 const &transformation)
        {
            VertexBuffer vb;
            IndexBuffer ib;

            D3D11_BUFFER_DESC vbd;
            vbd.Usage = D3D11_USAGE_IMMUTABLE;
            vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vbd.CPUAccessFlags = 0;
            vbd.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA initData;
            initData.pSysMem = &vertices[0];

            utils::AlwaysAssert(SUCCEEDED(direct3d::api::device5->CreateBuffer(&vbd, &initData, &vb.ptr())), "Failed to create vertex buffer");

            D3D11_BUFFER_DESC ibd;
            ibd.Usage = D3D11_USAGE_IMMUTABLE;
            ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
            ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            ibd.CPUAccessFlags = 0;
            ibd.MiscFlags = 0;

            initData.pSysMem = &indices[0];

            utils::AlwaysAssert(SUCCEEDED(direct3d::api::device5->CreateBuffer(&ibd, &initData, &ib.ptr())), "Failed to create index buffer");
            return Mesh
            {
                .vertices = vb,
                .indices = ib,
                .textures = textures,
                .mesh_to_model = math::mat4(transformation.a1,transformation.a2,transformation.a3,transformation.a4,
                                            transformation.b1,transformation.b2,transformation.b3,transformation.b4,
                                            transformation.c1,transformation.c2,transformation.c3,transformation.c4,
                                            transformation.d1,transformation.b2,transformation.d3,transformation.d4)
            };
        }

        Mesh processMesh(std::filesystem::path const &folder, aiMesh *mesh, aiMatrix4x4 const &transformation, const aiScene *scene)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<direct3d::ShaderResourceView> textures;
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                vertices.emplace_back(Vertex
                                      {
                                          .coords = engine::core::math::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                                          .uv = engine::core::math::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                                      });
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
                    textures.emplace_back(TextureManager::instance()->GetTexture(folder / str.C_Str()));
                }
            }
            return SetupMesh(vertices, indices, textures, transformation);
        }

        void processNode(std::filesystem::path const &folder, std::shared_ptr<Model> rv, aiNode *node, aiScene const *scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                rv->meshes.push_back(processMesh(folder, mesh, node->mTransformation, scene));
            }

            for (UINT i = 0; i < node->mNumChildren; i++)
            {
                processNode(folder, rv, node->mChildren[i], scene);
            }
        }
    } // namespace 

    std::shared_ptr<const Model> ModelLoader::Load(std::filesystem::path const &path)
    {
        Assimp::Importer importer;
        aiScene const *scene_ptr = importer.ReadFile(path.string().c_str(),
                                                     aiProcess_Triangulate |
                                                     aiProcess_MakeLeftHanded |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_FlipWindingOrder |
                                                     aiProcess_JoinIdenticalVertices);
        if (scene_ptr == nullptr)
        {
            utils::AlwaysAssert(false, "Failed to load model @ " + path.string());
            return nullptr;
        }
        auto rv = std::make_shared<Model>();
        rv->meshes.resize(scene_ptr->mNumMeshes);
        rv->mesh_ranges.resize(scene_ptr->mNumMeshes);

        processNode(std::filesystem::absolute(path.parent_path()), rv, scene_ptr->mRootNode, scene_ptr);
        return std::static_pointer_cast<const Model>(rv);
    }

    std::shared_ptr<ModelLoader> ModelLoader::instance_;
} // namespace engine::core