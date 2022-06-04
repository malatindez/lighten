#pragma once
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"
#include "transform.hpp"
#include <array>
#include <memory>
namespace engine::components
{
    class MeshComponent
    {
    public:
        MeshComponent(std::shared_ptr<const render::Mesh> value);
        bool CheckIntersection(Transform const &transform, core::math::Intersection &i, core::math::Ray const &ray) const;
        void SetMesh(std::shared_ptr<const render::Mesh> mesh);
        [[nodiscard]] std::shared_ptr<const render::Mesh> mesh_data() const noexcept { return mesh_; }
        [[nodiscard]] render::Material &material() noexcept { return material_; }
        [[nodiscard]] render::Material const &material() const noexcept { return material_; }

    private:
        static inline bool CheckTriangleIntersection(std::vector<render::Vertex>::const_iterator &it,
                                                     core::math::Intersection &i,
                                                     core::math::Ray const &ray) noexcept;
        std::shared_ptr<const render::Mesh> mesh_;
        render::Material material_;
    };
} // namespace engine::components
#include "mesh-component.inl"