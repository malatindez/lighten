#include "mesh-component.hpp"
#include "mesh-component.inl"
namespace engine::components
{
    MeshComponent::MeshComponent(std::shared_ptr<const render::Mesh> value) : mesh_(value)
    {
        assert(value != nullptr);
    }
    bool MeshComponent::CheckIntersection(Transform const &transform, core::math::Intersection &i,
                                 core::math::Ray const &ray) const
    {
        core::math::Ray local = ray;

        local.direction() =
            (core::math::vec4{local.direction(), 0} * transform.inv_model).as_vec<3>();
        local.origin() =
            (core::math::vec4{local.origin(), 1} * transform.inv_model).as_vec<3>();

        bool rv = false;
        auto it = mesh_->vertices.begin();
        while (std::distance(it, mesh_->vertices.end()) > 2)
        {
            rv |= CheckTriangleIntersection(it, i, local);
        }

        if (rv)
        {
            i.normal =
                normalize((core::math::vec4{i.normal, 0} * transform.model).as_vec<3>());
            i.point = (core::math::vec4{i.point, 1} * transform.model).as_vec<3>();
        }
        return rv;
    }
    void MeshComponent::SetMesh(std::shared_ptr<const render::Mesh> mesh)
    {
        assert(mesh != nullptr);
        mesh_ = mesh;
    }
}