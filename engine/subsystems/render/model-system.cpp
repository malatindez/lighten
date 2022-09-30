#include "model-system.hpp"
#include "../core/model-loader.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_;
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
                mesh_local.origin() = (inv_mat * core::math::vec4{ mesh_local.origin(), 1 }).xyz;
                mesh_local.SetDirection((inv_mat * core::math::vec4{ mesh_local.direction(), 0 }).xyz);
                bool t = mesh.triangle_octree.intersect(mesh_local, nearest);
                if (t)
                {
                    nearest.point = (mat * core::math::vec4{ nearest.point, 1 }).xyz;
                    nearest.normal = (mat * core::math::vec4{ nearest.normal, 0 }).xyz;
                    rv = t;
                }
            }
            return rv;
        }
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              core::math::Ray const &ray,
                                                              core::MeshIntersection &nearest)
    {
        auto group1 = registry.group<components::OpaqueComponent>(entt::get<components::TransformComponent>);
        auto group2 = registry.group<components::EmissiveComponent>(entt::get<components::TransformComponent>);
        std::optional<entt::entity> rv = std::nullopt;
        group1.each([&rv, &ray, &nearest] (auto const entity, auto const &model_instance, auto const &transform)
                    {
                        auto const &model = GetModel(model_instance.model_id);
                        if (CheckForIntersection(model, transform, ray, nearest))
                        {
                            rv = entity;
                        }
                    });
        group2.each([&rv, &ray, &nearest] (auto const entity, auto const &model_instance, auto const &transform)
                    {
                        auto const &model = GetModel(model_instance.model_id);
                        if (CheckForIntersection(model, transform, ray, nearest))
                        {
                            rv = entity;
                        }
                    });
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
        core::math::Box box = core::math::Box::empty();
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

                    setPos(side, vertex[0], quad[0]);
                    setPos(side, vertex[1], quad[1]);
                    setPos(side, vertex[2], quad[2]);

                    {
                        vec3 AB = vertex[1].position - vertex[0].position;
                        vec3 AC = vertex[2].position - vertex[0].position;
                        vertex[0].normal = vertex[1].normal = vertex[2].normal = normalize(cross(AC, AB));
                    }

                    vertex += 3;

                    setPos(side, vertex[0], quad[1]);
                    setPos(side, vertex[1], quad[3]);
                    setPos(side, vertex[2], quad[2]);

                    {
                        vec3 AB = vertex[1].position - vertex[0].position;
                        vec3 AC = vertex[2].position - vertex[0].position;
                        vertex[0].normal = vertex[1].normal = vertex[2].normal = normalize(cross(AC, AB));
                    }

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
    uint64_t ModelSystem::GetUnitCube()
    {
        static uint64_t model_id = std::numeric_limits<uint64_t>::max();
        if (model_id != std::numeric_limits<uint64_t>::max())
        {
            return model_id;
        }
        model_id = core::ModelLoader::Load(std::filesystem::current_path() / "assets/models/cube/cube.fbx").value();
        return model_id;
    }
} // namespace engine::render