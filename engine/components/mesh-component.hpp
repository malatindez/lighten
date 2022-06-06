#pragma once
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
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
        static bool CheckTriangleIntersection(core::math::vec3 const &p0,
                                                     core::math::vec3 const &p1,
                                                     core::math::vec3 const &p2,
                                                     core::math::vec3 const &normal,
                                                     core::math::Intersection &i,
                                                     core::math::Ray const &ray) noexcept;
        std::shared_ptr<const render::Mesh> mesh_;
        render::Material material_;
    };
} // namespace engine::components