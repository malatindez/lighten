#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace engine::core::math
{
	using namespace ::glm;
    
    template <typename T>
    void invert_orthonormal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
    {
        assert(&src != &dst);
        dst[0][0] = src[0][0];
        dst[1][1] = src[1][1];
        dst[2][2] = src[2][2];
        dst[0][1] = src[1][0];
        dst[1][0] = src[0][1];
        dst[0][2] = src[2][0];
        dst[2][0] = src[0][2];
        dst[1][2] = src[2][1];
        dst[2][1] = src[1][2];
        glm::vec3 vec = - src[3].x * glm::vec3{dst[0][0], dst[0][1], dst[0][2]} 
                        - src[3].y * glm::vec3{dst[1][0], dst[1][1], dst[1][2]} 
                        - src[3].z * glm::vec3{dst[2][0], dst[2][1], dst[2][2]};
        dst[3][0] = vec.x;
        dst[3][1] = vec.y;
        dst[3][2] = vec.z;
        dst[0][3] = 0;
        dst[1][3] = 0;
        dst[2][3] = 0;
        dst[3][3] = 1;
    }
    template <typename T>
    void invert_orthogonal(mat<4, 4, T> const &src, mat<4, 4, T> &dst)
    {
        assert(&src != &dst);
        dst[0][0] = src[0][0];
        dst[1][1] = src[1][1];
        dst[2][2] = src[2][2];
        dst[0][1] = src[1][0];
        dst[1][0] = src[0][1];
        dst[0][2] = src[2][0];
        dst[2][0] = src[0][2];
        dst[1][2] = src[2][1];
        dst[2][1] = src[1][2];

        vec3 lengths{
            sqrt(dst[0][0] * dst[0][0] + 
                 dst[0][1] * dst[0][1] +
                 dst[0][2] * dst[0][2]),
            sqrt(dst[1][0] * dst[1][0] + 
                 dst[1][1] * dst[1][1] +
                 dst[1][2] * dst[1][2]),
            sqrt(dst[2][0] * dst[2][0] + 
                 dst[2][1] * dst[2][1] +
                 dst[2][2] * dst[2][2]),
        };

        dst[0][0] = 1.0f / (dst[0][0] * lengths[0]);
        dst[0][1] = 1.0f / (dst[0][1] * lengths[0]);
        dst[0][2] = 1.0f / (dst[0][2] * lengths[0]);
        dst[1][0] = 1.0f / (dst[1][0] * lengths[1]);
        dst[1][1] = 1.0f / (dst[1][1] * lengths[1]);
        dst[1][2] = 1.0f / (dst[1][2] * lengths[1]);
        dst[2][0] = 1.0f / (dst[2][0] * lengths[2]);
        dst[2][1] = 1.0f / (dst[2][1] * lengths[2]);
        dst[2][2] = 1.0f / (dst[2][2] * lengths[2]);

        dst[3][0] = 
            -src[3].x * dst[0][0] / lengths[0]
            -src[3].x * dst[0][1] / lengths[0]
            -src[3].x * dst[0][2] / lengths[0];
        dst[3][1] = 
            -src[3].y * dst[1][0] / lengths[1]
            -src[3].y * dst[1][1] / lengths[1]
            -src[3].y * dst[1][2] / lengths[1];
        dst[3][2] = 
            -src[3].z * dst[2][0] / lengths[2] 
            -src[3].z * dst[2][1] / lengths[2]
            -src[3].z * dst[2][2] / lengths[2];
        dst[0][3] = 0;
        dst[1][3] = 0;
        dst[2][3] = 0;
        dst[3][3] = 1;
    }
    template <typename T>
    mat<4, 4, T> invert_orthonormal(mat<4, 4, T> const &src)
    {
        mat<4, 4, T> return_value;
        invert_orthonormal(src, return_value);
        return return_value;
    }
    template <typename T>
    mat<4, 4, T> invert_orthogonal(mat<4, 4, T> const &src)
    {
        mat<4, 4, T> return_value;
        invert_orthogonal(src, return_value);
        return return_value;
    }
}

/**
 * @namespace std
 * @brief Standard namespace, extended with hash specializations for custom math types.
 */
namespace std {
    /**
     * @brief Hash specialization for glm::vec.
     * 
     * @tparam size_x Vector dimension.
     * @tparam vector_type Type of vector's elements.
     * @tparam Q Qualifier.
     */
    template <size_t size_x, typename vector_type, glm::qualifier Q>
    struct hash<glm::vec<size_x, vector_type, Q>> {
        [[nodiscard]] constexpr size_t operator()(glm::vec<size_x, vector_type, Q> const &v) const noexcept {
            size_t seed = 0;
            vector_type const *ptr = reinterpret_cast<vector_type const *>(&v);
            for (size_t i = 0; i < size_x; i++) {
                seed ^= std::hash<vector_type>{}(ptr[i])+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    /**
     * @brief Hash specialization for glm::mat.
     * 
     * @tparam size_x Matrix X dimension.
     * @tparam size_y Matrix Y dimension.
     * @tparam vector_type Type of matrix elements.
     * @tparam Q Qualifier.
     */
    template <size_t size_x, size_t size_y, typename matrix_type, glm::qualifier Q>
    struct hash<glm::mat<size_x, size_y, matrix_type, Q>> {
        [[nodiscard]] constexpr size_t operator()(glm::mat<size_x, size_y, matrix_type, Q> const &m) const noexcept {
            size_t seed = 0;
            matrix_type const *ptr = reinterpret_cast<matrix_type const *>(&m);
            for (size_t i = 0; i < size_x * size_y; i++) {
                seed ^= std::hash<matrix_type>{}(ptr[i])+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    /**
     * @brief Hash specialization for glm::qua.
     * 
     * @tparam vector_type Type of matrix elements.
     * @tparam Q Qualifier.
     */
    template<typename quaternion_type, glm::qualifier Q>
    struct hash<glm::qua<quaternion_type, Q>> {
        [[nodiscard]] constexpr size_t operator()(glm::qua<quaternion_type, Q> const &q) const noexcept {
            size_t seed = 0;
            quaternion_type const *ptr = reinterpret_cast<quaternion_type const *>(&q);
            for (size_t i = 0; i < 4; i++) {
                seed ^= std::hash<quaternion_type>{}(ptr[i])+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
} // namespace std
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "math/triangle.hpp"
#include "math/plane.hpp"
#include "math/box.hpp"
#include "math/sphere.hpp"
#include "math/random.hpp"