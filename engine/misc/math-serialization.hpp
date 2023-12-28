#pragma once
#include "core/math.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/serialization.hpp>
namespace boost {
    namespace serialization {

        template<class Archive, ::engine::core::math::AnyVec Vector>
        inline void serialize(Archive& ar, Vector const& vec, const unsigned int version)
        {
            for (size_t i = 0; i < Vector::size; i++)
            {
                ar& vec[i];
            }
        }
        template<class Archive, ::engine::core::math::AnyMat Matrix>
        inline void serialize(Archive& ar, Matrix const& mat, const unsigned int version)
        {
            for (size_t i = 0; i < Matrix::size.x * Matrix::size.y; i++)
            {
                ar& mat[i];
            }
        }
        template<class Archive, ::engine::core::math::Primitive T>
        inline void serialize(Archive& ar, ::engine::core::math::qua<T> const& quat, const unsigned int version)
        {
            ar& quat.x;
            ar& quat.y;
            ar& quat.z;
            ar& quat.w;
        }

    } // namespace serialization
} // namespace boost