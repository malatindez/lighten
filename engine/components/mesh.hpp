#pragma once
#include "components/triangle.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "render/mesh-obj.hpp"
#include "transform.hpp"
#include <array>
#include <memory>
namespace engine::components
{
    class Mesh
    {
    public:
        Mesh(std::shared_ptr<const render::MeshObj> value) : mesh_(value)
        {
            assert(value != nullptr);
        }
        bool CheckIntersection(Transform const &transform, math::Intersection &i, math::Ray const &ray) const
        {
            math::Ray local = ray;

            local.direction() = (math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
            local.origin() = (math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();

            bool rv = false;
            auto it = mesh_->vertices.begin();
            while (std::distance(it, mesh_->vertices.end()) > 2)
            {
                rv |= CheckTriangleIntersection(it, i, local);
            }

            if (rv)
            {
                i.normal = normalize((math::vec4{i.normal, 0} * transform.model).as_vec<3>());
                i.point = (math::vec4{i.point, 1} * transform.model).as_vec<3>();
            }
            return rv;
        }
        void SetMesh(std::shared_ptr<const render::MeshObj> mesh)
        {
            assert(mesh != nullptr);
            mesh_ = mesh;
        }
        [[nodiscard]] std::shared_ptr<const render::MeshObj> mesh_data() const noexcept { return mesh_; }

    private:
        static inline bool CheckTriangleIntersection(std::vector<render::Vertex>::const_iterator &it, math::Intersection &i, math::Ray const &ray) noexcept
        {
            math::vec3 const& p0 = (it++)->position;
            math::vec3 const& p1 = (it++)->position;
            math::vec3 const& p2 = (it)->position;
            math::vec3 const& normal = (it++)->normal;

            float ndotdir = math::dot(normal, ray.direction());
            if (fabs(ndotdir) < 1e-6f)
            {
                return false;
            }
            float t = math::dot(p0 - ray.origin(), normal) / ndotdir;
            if (t > i.t || t < 0)
            {
                return false;
            }
            math::vec3 P = ray.PointAtParameter(t);
            math::vec3 c;
            math::vec3 edge0 = p1 - p0;
            math::vec3 vp0 = P - p0;
            c = math::cross(edge0, vp0);
            if (math::dot(normal, c) < 0.0f)
            {
                return false;
            }
            math::vec3 edge1 = p2 - p1;
            math::vec3 vp1 = P - p1;
            c = math::cross(edge1, vp1);
            if (math::dot(normal, c) < 0.0f)
            {
                return false;
            }
            math::vec3 edge2 = p0 - p2;
            math::vec3 vp2 = P - p2;
            c = math::cross(edge2, vp2);
            if (math::dot(normal, c) < 0.0f)
            {
                return false;
            }
            i.point = P;
            i.normal = normal;
            i.t = t;
            return true;
        }
        std::shared_ptr<const render::MeshObj> mesh_;
    };
}