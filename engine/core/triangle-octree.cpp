#include "triangle-octree.hpp"
#include "render/model.hpp"
namespace engine::core
{
    const int TriangleOctree::PREFFERED_TRIANGLE_COUNT = 32;
    const float TriangleOctree::MAX_STRETCHING_RATIO = 1.05f;

    void TriangleOctree::initialize(render::ModelMesh const &model_mesh)
    {
        render::Mesh const &mesh = model_mesh.mesh;
        m_triangles.clear();
        m_triangles.shrink_to_fit();

        m_mesh = &mesh;
        m_children = nullptr;

        const glm::vec3 eps{1e-5f, 1e-5f, 1e-5f};
        m_box = m_initialBox = math::Box{model_mesh.mesh_range.bounding_box.min - eps, model_mesh.mesh_range.bounding_box.max + eps};

        for (size_t i = 0; i < mesh.indices.size() - 2;)
        {
            auto const &V1 = mesh.vertices[mesh.indices[i++]].position;
            auto const &V2 = mesh.vertices[mesh.indices[i++]].position;
            auto const &V3 = mesh.vertices[mesh.indices[i++]].position;

            glm::vec3 center = (V1 + V2 + V3) / 3.f;

            bool inserted = addTriangle((uint32_t)(i / 3 - 1), V1, V2, V3, center);
            if (!inserted)
            {
                mal_toolkit::Assert(inserted, "Failed to insert triangle");
            }
        }
    }

    void TriangleOctree::initialize(render::Mesh const &mesh, const math::Box &parentBox, const glm::vec3 &parentCenter, int octetIndex)
    {
        m_mesh = &mesh;
        m_children = nullptr;

        if (octetIndex % 2 == 0)
        {
            m_initialBox.min[0] = parentBox.min[0];
            m_initialBox.max[0] = parentCenter[0];
        }
        else
        {
            m_initialBox.min[0] = parentCenter[0];
            m_initialBox.max[0] = parentBox.max[0];
        }

        if (octetIndex % 4 < 2)
        {
            m_initialBox.min[1] = parentBox.min[1];
            m_initialBox.max[1] = parentCenter[1];
        }
        else
        {
            m_initialBox.min[1] = parentCenter[1];
            m_initialBox.max[1] = parentBox.max[1];
        }

        if (octetIndex < 4)
        {
            m_initialBox.min[2] = parentBox.min[2];
            m_initialBox.max[2] = parentCenter[2];
        }
        else
        {
            m_initialBox.min[2] = parentCenter[2];
            m_initialBox.max[2] = parentBox.max[2];
        }

        m_box = m_initialBox;
        glm::vec3 elongation = (MAX_STRETCHING_RATIO - 1.f) * m_box.size();

        if (octetIndex % 2 == 0)
            m_box.max[0] += elongation[0];
        else
            m_box.min[0] -= elongation[0];

        if (octetIndex % 4 < 2)
            m_box.max[1] += elongation[1];
        else
            m_box.min[1] -= elongation[1];

        if (octetIndex < 4)
            m_box.max[2] += elongation[2];
        else
            m_box.min[2] -= elongation[2];
    }

    bool TriangleOctree::addTriangle(uint32_t triangleIndex, const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3, const glm::vec3 &center, uint32_t depth)
    {
        if (depth > maximum_depth)
        {
            m_triangles.emplace_back(triangleIndex);
            return true;
        }
        if (!m_initialBox.contains(center) ||
            !m_box.contains(V1) ||
            !m_box.contains(V2) ||
            !m_box.contains(V3))
        {
            return false;
        }

        if (m_children == nullptr)
        {
            if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT)
            {
                m_triangles.emplace_back(triangleIndex);
                return true;
            }
            else
            {
                glm::vec3 C = (m_initialBox.min + m_initialBox.max) / 2.f;

                m_children.reset(new std::array<TriangleOctree, 8>());
                for (int i = 0; i < 8; ++i)
                {
                    (*m_children)[i].initialize(*m_mesh, m_initialBox, C, i);
                }

                std::vector<uint32_t> newTriangles;

                for (uint32_t index : m_triangles)
                {
                    auto const &P1 = m_mesh->vertices[m_mesh->indices[index * 3]].position;
                    auto const &P2 = m_mesh->vertices[m_mesh->indices[index * 3 + 1]].position;
                    auto const &P3 = m_mesh->vertices[m_mesh->indices[index * 3 + 2]].position;

                    glm::vec3 P = (P1 + P2 + P3) / 3.f;

                    int i = 0;
                    for (; i < 8; ++i)
                    {
                        if ((*m_children)[i].addTriangle(index, P1, P2, P3, P))
                            break;
                    }

                    if (i == 8)
                        newTriangles.emplace_back(index);
                }

                m_triangles = std::move(newTriangles);
            }
        }

        int i = 0;
        for (; i < 8; ++i)
        {
            if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center, depth + 1))
                break;
        }

        if (i == 8)
            m_triangles.emplace_back(triangleIndex);

        return true;
    }

    bool TriangleOctree::intersect(const math::Ray &ray, MeshIntersection &nearest) const
    {
        float boxT = nearest.t;
        if (!m_box.Intersect(ray, boxT))
            return false;

        return intersectInternal(ray, nearest);
    }
    bool TriangleOctree::intersectInternal(const math::Ray &ray, MeshIntersection &nearest) const
    {
        {
            float boxT = nearest.t;
            if (!m_box.Intersect(ray, boxT))
                return false;
        }

        bool found = false;

        for (uint32_t i = 0; i < m_triangles.size(); ++i)
        {
            auto const &V1 = m_mesh->vertices[m_mesh->indices[m_triangles[i] * 3]].position;
            auto const &V2 = m_mesh->vertices[m_mesh->indices[m_triangles[i] * 3 + 1]].position;
            auto const &V3 = m_mesh->vertices[m_mesh->indices[m_triangles[i] * 3 + 2]].position;

            if (math::Triangle::Intersect(V1, V2, V3, nearest, ray))
            {
                nearest.triangle = i;
                nearest.mesh_ptr = m_mesh;
                found = true;
            }
        }

        if (!m_children)
            return found;

        struct OctantIntersection
        {
            int index;
            float t;
        };

        std::array<OctantIntersection, 8> boxIntersections;

        for (int i = 0; i < 8; ++i)
        {
            if ((*m_children)[i].m_box.contains(ray.origin()))
            {
                boxIntersections[i].index = i;
                boxIntersections[i].t = 0.f;
            }
            else
            {
                float boxT = nearest.t;
                if ((*m_children)[i].m_box.Intersect(ray, boxT))
                {
                    boxIntersections[i].index = i;
                    boxIntersections[i].t = boxT;
                }
                else
                {
                    boxIntersections[i].index = -1;
                }
            }
        }
        std::sort(boxIntersections.begin(), boxIntersections.end(),
                  [](const OctantIntersection &A, const OctantIntersection &B) -> bool
                  { return A.t < B.t; });

        for (int i = 0; i < 8; ++i)
        {
            if (boxIntersections[i].index < 0 || boxIntersections[i].t > nearest.t)
                continue;

            if ((*m_children)[boxIntersections[i].index].intersectInternal(ray, nearest))
            {
                nearest.mesh_ptr = m_mesh;
                found = true;
            }
        }

        return found;
    }
}