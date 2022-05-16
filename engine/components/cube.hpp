#pragma once
#include "intersectable.hpp"
namespace engine::components
{
    struct Intersectable : entt::type_list<bool(math::Intersection &, math::Ray&)>
    {
        template <typename Base>
        struct type : Base
        {
            bool check(Intersection &i, math::Ray & ray) { return entt::poly_call<0>(*this, i, ray);}
        };
    };
}