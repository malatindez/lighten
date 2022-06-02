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
        Mesh(std::shared_ptr<const render::MeshObj> value);
        bool CheckIntersection(Transform const &transform, math::Intersection &i, math::Ray const &ray) const;
        void SetMesh(std::shared_ptr<const render::MeshObj> mesh);
        [[nodiscard]] std::shared_ptr<const render::MeshObj> mesh_data() const noexcept { return mesh_; }

    private:
        static inline bool CheckTriangleIntersection(std::vector<render::Vertex>::const_iterator &it,
                                                     math::Intersection &i,
                                                     math::Ray const &ray) noexcept;
        std::shared_ptr<const render::MeshObj> mesh_;
    };
} // namespace engine::components
#include "mesh.inl"