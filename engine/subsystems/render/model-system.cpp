#include "model-system.hpp"
#include "../core/model-loader.hpp"
#include "../core/texture-manager.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_ = nullptr;
    namespace
    {
        bool CheckForIntersection(Model const &model,
                                  components::TransformComponent transform,
                                  Ray const &ray,
                                  core::MeshIntersection &nearest)
        {
            bool rv = false;
            for (auto const &mesh : model.meshes)
            {
                mat4 mat = transform.model * mesh.mesh_to_model;
                mat4 inv_mat = mesh.inv_mesh_to_model * transform.inv_model;
                Ray mesh_local = ray;
                mesh_local.origin() = (inv_mat * vec4{ mesh_local.origin(), 1 }).xyz;
                mesh_local.SetDirection((inv_mat * vec4{ mesh_local.direction(), 0 }).xyz);
                bool t = mesh.triangle_octree.intersect(mesh_local, nearest);
                if (t)
                {
                    nearest.point = (mat * vec4{ nearest.point, 1 }).xyz;
                    nearest.normal = (mat * vec4{ nearest.normal, 0 }).xyz;
                    rv = t;
                }
            }
            return rv;
        }
    } // namespace
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              Ray const &ray,
                                                              core::MeshIntersection &nearest)
    {
        auto group1 = registry.group<components::OpaqueComponent>(entt::get<components::TransformComponent>);
        auto group2 = registry.group<components::EmissiveComponent>(entt::get<components::TransformComponent>);
        auto group3 = registry.group<components::DissolutionComponent>(entt::get<components::TransformComponent>);
        std::optional<entt::entity> rv = std::nullopt;
        auto const &func = [&rv, &ray, &nearest] (auto const entity, auto const &model_instance, auto const &transform) -> void
                    {
                        auto const &model = GetModel(model_instance.model_id);
                        if (CheckForIntersection(model, transform, ray, nearest))
                        {
                            rv = entity;
                        }
                    };
        group1.each(func);
        group2.each(func);
        group3.each(func);
        return rv;
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              Ray const &ray,
                                                              Intersection &nearest)
    {
        core::MeshIntersection mesh_intersection;
        mesh_intersection.normal = nearest.normal;
        mesh_intersection.t = nearest.t;
        mesh_intersection.point = nearest.point;
        mesh_intersection.near = std::numeric_limits<float>::max();
        auto rv = FindIntersection(registry, ray, mesh_intersection);
        nearest.normal = mesh_intersection.normal;
        nearest.t = mesh_intersection.t;
        nearest.point = mesh_intersection.point;
        return rv;
    }
    uint64_t ModelSystem::AddModel(Model &&model_)
    {
        instance_->models_.emplace(current_index, std::move(model_));
        return current_index++;
    }
    uint64_t ModelSystem::GetUnitSphereFlat()
    {
        static uint64_t model_id = std::numeric_limits<uint64_t>::max();
        if (model_id != std::numeric_limits<uint64_t>::max())
        {
            return model_id;
        }
        const uint32_t SIDES = 6;
        const uint32_t GRID_SIZE = 12;
        const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
        const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

        std::string name = "UNIT_SPHERE_FLAT";
        Box box = Box::empty();
        Mesh mesh;
        mesh.vertices.resize(VERT_PER_SIZE * SIDES);
        Vertex *vertex = mesh.vertices.data();
        int sideMasks[6][3] =
        {
            { 2, 1, 0 },
            { 0, 1, 2 },
            { 2, 1, 0 },
            { 0, 1, 2 },
            { 0, 2, 1 },
            { 0, 2, 1 }
        };
        float sideSigns[6][3] =
        {
            { +1, +1, +1 },
            { -1, +1, +1 },
            { -1, +1, -1 },
            { +1, +1, -1 },
            { +1, -1, -1 },
            { +1, +1, +1 }
        };

        for (uint32_t side = 0; side < SIDES; ++side)
        {
            for (uint32_t row = 0; row < GRID_SIZE; ++row)
            {
                for (uint32_t col = 0; col < GRID_SIZE; ++col)
                {
                    float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
                    float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
                    float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
                    float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

                    vec3 quad[4] =
                    {
                        { left, bottom, 1.f },
                        { right, bottom, 1.f },
                        { left, top, 1.f },
                        { right, top, 1.f }
                    };

                    vertex[0] = vertex[1] = vertex[2] = vertex[3] = Vertex::empty();

                    auto setPos = [sideMasks, sideSigns] (int side, Vertex &dst, const vec3 &position)
                    {
                        dst.position[sideMasks[side][0]] = position.x * sideSigns[side][0];
                        dst.position[sideMasks[side][1]] = position.y * sideSigns[side][1];
                        dst.position[sideMasks[side][2]] = position.z * sideSigns[side][2];
                        dst.position = normalize(dst.position);
                    };
                    auto setTexCoord = [] ([[maybe_unused]] Vertex &dst)
                    {
                        // todo
                        // dst.tex_coord.u = ((-dst.position.z / core::math::abs(dst.position.x)) + 1) / 2;
                        // dst.tex_coord.v = ((-dst.position.y / core::math::abs(dst.position.x)) + 1) / 2;
                    };
                    auto calculateTangents = [] ([[maybe_unused]] Vertex &dst)
                    {
                        // todo
                        // vec3 tangent = cross(dst.normal, vec3{ 0, 1, 0 });
                        // if (length(tangent) < 0.0001f)
                        // {
                        //     tangent = cross(dst.normal, vec3{ 1, 0, 0 });
                        // }
                        // tangent = normalize(tangent);
                        // dst.tangent = tangent;
                        // dst.bitangent = cross(dst.normal, tangent);
                    };

                    setPos(side, vertex[0], quad[0]);
                    setPos(side, vertex[1], quad[1]);
                    setPos(side, vertex[2], quad[2]);
                    setTexCoord(vertex[0]);
                    setTexCoord(vertex[1]);
                    setTexCoord(vertex[2]);
                    {
                        vec3 AB = vertex[1].position - vertex[0].position;
                        vec3 AC = vertex[2].position - vertex[0].position;
                        vertex[0].normal = vertex[1].normal = vertex[2].normal = normalize(cross(AC, AB));
                        vertex[0].normal = vertex[0].position;
                        vertex[1].normal = vertex[1].position;
                        vertex[2].normal = vertex[2].position;
                    }
                    calculateTangents(vertex[0]);
                    calculateTangents(vertex[1]);
                    calculateTangents(vertex[2]);
                    vertex += 3;

                    setPos(side, vertex[0], quad[1]);
                    setPos(side, vertex[1], quad[3]);
                    setPos(side, vertex[2], quad[2]);
                    setTexCoord(vertex[0]);
                    setTexCoord(vertex[1]);
                    setTexCoord(vertex[2]);

                    {
                        vec3 AB = vertex[1].position - vertex[0].position;
                        vec3 AC = vertex[2].position - vertex[0].position;
                        vertex[0].normal = vertex[1].normal = vertex[2].normal = normalize(cross(AC, AB));
                        vertex[0].normal = vertex[0].position;
                        vertex[1].normal = vertex[1].position;
                        vertex[2].normal = vertex[2].position;
                    }
                    calculateTangents(vertex[0]);
                    calculateTangents(vertex[1]);
                    calculateTangents(vertex[2]);

                    vertex += 3;
                }
            }
        }
        for (uint32_t i = 0; i < (int)mesh.vertices.size(); i += 3)
        {
            mesh.indices.push_back(i + 2);
            mesh.indices.push_back(i + 1);
            mesh.indices.push_back(i);
            rmin(box.min, mesh.vertices[i].position);
            rmax(box.max, mesh.vertices[i].position);
        }

        ModelMesh model_mesh(0, mat4::identity(),
                             MeshRange{ .vertex_offset = 0, .index_offset = 0,
                             .vertex_count = (uint32_t)mesh.vertices.size(),
                             .index_count = (uint32_t)mesh.indices.size(),
                             .bounding_box = box },
                             std::move(mesh));
        Model model{
            .name = name,
            .bounding_box = box,
            .meshes = {},
            .materials = {render::Material{}},
            .vertices = direct3d::ImmutableVertexBuffer<Vertex>{model_mesh.mesh.vertices},
            .indices = direct3d::ImmutableIndexBuffer<uint32_t>{model_mesh.mesh.indices}
        };
        model.meshes.push_back(std::move(model_mesh));
        model.meshes[0].triangle_octree.initialize(model.meshes[0]);

        model_id = instance().AddModel(std::move(model));
        return model_id;
    }
    uint64_t ModelSystem::GetUnitCube()
    {
        static uint64_t model_id = std::numeric_limits<uint64_t>::max();
        if (model_id != std::numeric_limits<uint64_t>::max())
        {
            return model_id;
        }
        std::string name = "UNIT_CUBE_FLAT";
        Box box = Box{ {-0.5f}, {0.5f} };
        Mesh mesh;
        mesh.vertices.resize(24);
        Vertex *vertex = mesh.vertices.data();
        vertex[0].position = { -0.5f, -0.5f, -0.5f };
        vertex[0].tex_coord = { 0, 1 };
        vertex[0].normal = { 0,0,-1 };
        vertex[0].tangent = { 1,0,0 };
        vertex[0].bitangent = { 0,-1,0 };
        vertex[1].position = { 0.5f, -0.5f, -0.5f };
        vertex[1].tex_coord = { 1, 1 };
        vertex[1].normal = { 0,0,-1 };
        vertex[1].tangent = { 1,0,0 };
        vertex[1].bitangent = { 0,-1,0 };
        vertex[2].position = { -0.5f, 0.5f, -0.5f };
        vertex[2].tex_coord = { 0, 0 };
        vertex[2].normal = { 0,0,-1 };
        vertex[2].tangent = { 1,0,0 };
        vertex[2].bitangent = { 0,-1,0 };
        vertex[3].position = { 0.5f, 0.5f, -0.5f };
        vertex[3].tex_coord = { 1, 0 };
        vertex[3].normal = { 0,0,-1 };
        vertex[3].tangent = { 1,0,0 };
        vertex[3].bitangent = { 0,-1,0 };
        vertex[4].position = { -0.5f, -0.5f, 0.5f };
        vertex[4].tex_coord = { 0, 1 };
        vertex[4].normal = { 0,0,1 };
        vertex[4].tangent = { -1,0,0 };
        vertex[4].bitangent = { 0,-1,0 };
        vertex[5].position = { 0.5f, -0.5f, 0.5f };
        vertex[5].tex_coord = { 1, 1 };
        vertex[5].normal = { 0,0,1 };
        vertex[5].tangent = { -1,0,0 };
        vertex[5].bitangent = { 0,-1,0 };
        vertex[6].position = { -0.5f, 0.5f, 0.5f };
        vertex[6].tex_coord = { 0, 0 };
        vertex[6].normal = { 0,0,1 };
        vertex[6].tangent = { -1,0,0 };
        vertex[6].bitangent = { 0,-1,0 };
        vertex[7].position = { 0.5f, 0.5f, 0.5f };
        vertex[7].tex_coord = { 1, 0 };
        vertex[7].normal = { 0,0,1 };
        vertex[7].tangent = { -1,0,0 };
        vertex[7].bitangent = { 0,-1,0 };
        vertex[8].position = { -0.5f, -0.5f, -0.5f };
        vertex[8].tex_coord = { 0, 1 };
        vertex[8].normal = { -1,0,0 };
        vertex[8].tangent = { 0,0,-1 };
        vertex[8].bitangent = { 0,-1,0 };
        vertex[9].position = { -0.5f, 0.5f, -0.5f };
        vertex[9].tex_coord = { 1, 1 };
        vertex[9].normal = { -1,0,0 };
        vertex[9].tangent = { 0,0,-1 };
        vertex[9].bitangent = { 0,-1,0 };
        vertex[10].position = { -0.5f, -0.5f, 0.5f };
        vertex[10].tex_coord = { 0, 0 };
        vertex[10].normal = { -1,0,0 };
        vertex[10].tangent = { 0,0,-1 };
        vertex[10].bitangent = { 0,-1,0 };
        vertex[11].position = { -0.5f, 0.5f, 0.5f };
        vertex[11].tex_coord = { 1, 0 };
        vertex[11].normal = { -1,0,0 };
        vertex[11].tangent = { 0,0,-1 };
        vertex[11].bitangent = { 0,-1,0 };
        vertex[12].position = { 0.5f, -0.5f, -0.5f };
        vertex[12].tex_coord = { 0, 1 };
        vertex[12].normal = { 1,0,0 };
        vertex[12].tangent = { 0,0,1 };
        vertex[12].bitangent = { 0,-1,0 };
        vertex[13].position = { 0.5f, 0.5f, -0.5f };
        vertex[13].tex_coord = { 1, 1 };
        vertex[13].normal = { 1,0,0 };
        vertex[13].tangent = { 0,0,1 };
        vertex[13].bitangent = { 0,-1,0 };
        vertex[14].position = { 0.5f, -0.5f, 0.5f };
        vertex[14].tex_coord = { 0, 0 };
        vertex[14].normal = { 1,0,0 };
        vertex[14].tangent = { 0,0,1 };
        vertex[14].bitangent = { 0,-1,0 };
        vertex[15].position = { 0.5f, 0.5f, 0.5f };
        vertex[15].tex_coord = { 1, 0 };
        vertex[15].normal = { 1,0,0 };
        vertex[15].tangent = { 0,0,1 };
        vertex[15].bitangent = { 0,-1,0 };
        vertex[16].position = { -0.5f, -0.5f, -0.5f };
        vertex[16].tex_coord = { 0, 1 };
        vertex[16].normal = { 0,-1,0 };
        vertex[16].tangent = { 1,0,0 };
        vertex[16].bitangent = { 0,0,-1 };
        vertex[17].position = { 0.5f, -0.5f, -0.5f };
        vertex[17].tex_coord = { 1, 1 };
        vertex[17].normal = { 0,-1,0 };
        vertex[17].tangent = { 1,0,0 };
        vertex[17].bitangent = { 0,0,-1 };
        vertex[18].position = { -0.5f, -0.5f, 0.5f };
        vertex[18].tex_coord = { 0, 0 };
        vertex[18].normal = { 0,-1,0 };
        vertex[18].tangent = { 1,0,0 };
        vertex[18].bitangent = { 0,0,-1 };
        vertex[19].position = { 0.5f, -0.5f, 0.5f };
        vertex[19].tex_coord = { 1, 0 };
        vertex[19].normal = { 0,-1,0 };
        vertex[19].tangent = { 1,0,0 };
        vertex[19].bitangent = { 0,0,-1 };
        vertex[20].position = { -0.5f, 0.5f, -0.5f };
        vertex[20].tex_coord = { 0, 1 };
        vertex[20].normal = { 0,1,0 };
        vertex[20].tangent = { 1,0,0 };
        vertex[20].bitangent = { 0,0,1 };
        vertex[21].position = { 0.5f, 0.5f, -0.5f };
        vertex[21].tex_coord = { 1, 1 };
        vertex[21].normal = { 0,1,0 };
        vertex[21].tangent = { 1,0,0 };
        vertex[21].bitangent = { 0,0,1 };
        vertex[22].position = { -0.5f, 0.5f, 0.5f };
        vertex[22].tex_coord = { 0, 0 };
        vertex[22].normal = { 0,1,0 };
        vertex[22].tangent = { 1,0,0 };
        vertex[22].bitangent = { 0,0,1 };
        vertex[23].position = { 0.5f, 0.5f, 0.5f };
        vertex[23].tex_coord = { 1, 0 };
        vertex[23].normal = { 0,1,0 };
        vertex[23].tangent = { 1,0,0 };
        vertex[23].bitangent = { 0,0,1 };
        mesh.indices = { 2,1,0, 2,3,1, 4,5,6, 5,7,6, 10,9,8, 10,11,9, 12,13,14, 13,15,14, 16,17,18, 17,19,18, 22,21,20, 22,23,21 };

        ModelMesh model_mesh(0, mat4::identity(),
                             MeshRange{ .vertex_offset = 0, .index_offset = 0, .vertex_count = (uint32_t)mesh.vertices.size(), .index_count = (uint32_t)mesh.indices.size(), .bounding_box = box },
                             std::move(mesh));
        Model model{
            .name = name,
            .bounding_box = box,
            .meshes = {},
            .materials = {render::Material{}},
            .vertices = direct3d::ImmutableVertexBuffer<Vertex>{model_mesh.mesh.vertices},
            .indices = direct3d::ImmutableIndexBuffer<uint32_t>{model_mesh.mesh.indices}
        };
        model.meshes.push_back(std::move(model_mesh));
        model.meshes[0].triangle_octree.initialize(model.meshes[0]);

        model_id = instance().AddModel(std::move(model));
        return model_id;
    }
} // namespace engine::render