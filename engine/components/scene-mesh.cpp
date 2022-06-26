#include "scene-mesh.hpp"
namespace engine::components
{
    SceneMesh::SceneMesh(std::shared_ptr<const render::Mesh> value) : mesh_(value)
    {
        assert(value != nullptr);
    }
    bool SceneMesh::CheckIntersection(Transform const &transform, core::math::Intersection &i,
                                         core::math::Ray const &ray) const
    {
        core::math::Ray local = ray;
        local.direction() =
            (core::math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
        local.origin() =
            (core::math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();

        bool rv = false;
        auto it = mesh_->vertices.begin();
        while (it != mesh_->vertices.end())
        {
            auto const &normal = it->normal;
            auto const &p0 = (it++)->position;
            auto const &p1 = (it++)->position;
            auto const &p2 = (it++)->position;
            rv |= CheckTriangleIntersection(p0, p1, p2, normal, i, local);
        }

        if (rv)
        {
            i.normal =
                normalize((core::math::vec4{i.normal, 0} * transform.model).as_vec<3>());
            i.point = (core::math::vec4{i.point, 1} * transform.model).as_vec<3>();
        }
        return rv;
    }
    void SceneMesh::SetMesh(std::shared_ptr<const render::Mesh> mesh)
    {
        assert(mesh != nullptr);
        mesh_ = mesh;
    }

    bool SceneMesh::CheckTriangleIntersection(
        core::math::vec3 const &p0,
        core::math::vec3 const &p1,
        core::math::vec3 const &p2,
        core::math::vec3 const &normal,
        core::math::Intersection &i,
        core::math::Ray const &ray) noexcept
    {

        float ndotdir = core::math::dot(normal, ray.direction());
        if (fabs(ndotdir) < 1e-6f)
        {
            return false;
        }
        float d = -dot(normal, p0);
        float t = -(core::math::dot(ray.origin(), normal) + d) / ndotdir;
        if (t > i.t || t < 0)
        {
            return false;
        }
        core::math::vec3 P = ray.PointAtParameter(t);
        core::math::vec3 c;
        core::math::vec3 edge0 = p1 - p0;
        core::math::vec3 vp0 = P - p0;
        c = core::math::cross(edge0, vp0);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        core::math::vec3 edge1 = p2 - p1;
        core::math::vec3 vp1 = P - p1;
        c = core::math::cross(edge1, vp1);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        core::math::vec3 edge2 = p0 - p2;
        core::math::vec3 vp2 = P - p2;
        c = core::math::cross(edge2, vp2);
        if (core::math::dot(normal, c) < 0.0f)
        {
            return false;
        }
        i.point = P;
        // reverse normal if the ray is on the opposite side of the triangle
        i.normal = normal * (ndotdir > 0 ? -1 : 1);
        i.t = t;
        return true;
    }
} // namespace engine::components