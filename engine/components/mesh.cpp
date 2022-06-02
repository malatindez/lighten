#include "mesh.hpp"
#include "mesh.inl"
namespace engine::components
{
    Mesh::Mesh(std::shared_ptr<const render::MeshObj> value) : mesh_(value)
    {
        assert(value != nullptr);
    }
    bool Mesh::CheckIntersection(Transform const &transform, math::Intersection &i,
                                 math::Ray const &ray) const
    {
        math::Ray local = ray;

        local.direction() =
            (math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
        local.origin() =
            (math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();

        bool rv = false;
        auto it = mesh_->vertices.begin();
        while (std::distance(it, mesh_->vertices.end()) > 2)
        {
            rv |= CheckTriangleIntersection(it, i, local);
        }

        if (rv)
        {
            i.normal =
                normalize((math::vec4{i.normal, 0} * transform.model).as_vec<3>());
            i.point = (math::vec4{i.point, 1} * transform.model).as_vec<3>();
        }
        return rv;
    }
    void Mesh::SetMesh(std::shared_ptr<const render::MeshObj> mesh)
    {
        assert(mesh != nullptr);
        mesh_ = mesh;
    }
}