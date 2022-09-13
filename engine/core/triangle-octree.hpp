#pragma once

#include "core/math.hpp"
namespace engine::render
{
    struct Mesh;
}

namespace engine::core
{
    struct MeshIntersection : math::Intersection
    {
        MeshIntersection() = default;

        float near;
        uint32_t triangle;

        constexpr void reset(float near_v, float far = std::numeric_limits<float>::infinity())
        {
            near = near_v;
            t = far;
        }
        bool valid() const { return std::isfinite(t); }
    };

    class TriangleOctree
    {
    public:
        TriangleOctree() = default;
        TriangleOctree(TriangleOctree const &) = delete;
        TriangleOctree &operator=(TriangleOctree const &) = delete;
        // default move
        TriangleOctree(TriangleOctree &&) = default;
        TriangleOctree &operator=(TriangleOctree &&) = default;

        const static int PREFFERED_TRIANGLE_COUNT;
        const static float MAX_STRETCHING_RATIO;

        void clear() { m_mesh = nullptr; }
        bool inited() const { return m_mesh != nullptr; }

        void initialize(render::Mesh const &mesh);

        bool intersect(const math::Ray &ray, MeshIntersection &nearest) const;

    protected:
        const render::Mesh *m_mesh = nullptr;
        std::vector<uint32_t> m_triangles;

        math::Box m_box = math::Box::empty();
        math::Box m_initialBox = math::Box::empty();
        std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

        void initialize(const render::Mesh &mesh, const math::Box &parentBox, const math::vec3 &parentCenter, int octetIndex);

        bool addTriangle(uint32_t triangleIndex, const math::vec3 &V1, const math::vec3 &V2, const math::vec3 &V3, const math::vec3 &center);

        bool intersectInternal(const math::Ray &ray, MeshIntersection &nearest) const;
    };
}