#pragma once

#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 6001)
#pragma warning(disable : 6287)
#pragma warning(disable : 26495)
#endif

#include "math/mat.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "math/triangle.hpp"
#include "math/plane.hpp"
#include "math/box.hpp"
#include "math/quaternion.hpp"
#include "math/sphere.hpp"

namespace engine::core::math
{
    namespace numbers = ::std::numbers;
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> reflect(Vector const &normal, Vector const &incident) noexcept
    {
        return incident - 2.0f * dot(normal, incident) * normal;
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> refract(Vector const &normal, Vector const &incident, float eta) noexcept
    {
        auto const cos_theta = dot(normal, incident);
        auto const r_out_perp = eta * (incident - cos_theta * normal);
        auto const r_out_parallel = -std::sqrt(std::abs(1.0f - squared_length(r_out_perp))) * normal;
        return r_out_perp + r_out_parallel;
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> refract(Vector const &normal, Vector const &incident, float eta, float &cos_theta) noexcept
    {
        cos_theta = dot(normal, incident);
        auto const r_out_perp = eta * (incident - cos_theta * normal);
        auto const r_out_parallel = -std::sqrt(std::abs(1.0f - squared_length(r_out_perp))) * normal;
        return r_out_perp + r_out_parallel;
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> refract(Vector const &normal, Vector const &incident, float eta, float &cos_theta, float &cos_theta_t) noexcept
    {
        cos_theta = dot(normal, incident);
        auto const r_out_perp = eta * (incident - cos_theta * normal);
        cos_theta_t = std::sqrt(std::abs(1.0f - squared_length(r_out_perp)));
        auto const r_out_parallel = -cos_theta_t * normal;
        return r_out_perp + r_out_parallel;
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> refract(Vector const &normal, Vector const &incident, float eta, float &cos_theta, float &cos_theta_t, float &reflectance) noexcept
    {
        cos_theta = dot(normal, incident);
        auto const r_out_perp = eta * (incident - cos_theta * normal);
        cos_theta_t = std::sqrt(std::abs(1.0f - squared_length(r_out_perp)));
        auto const r_out_parallel = -cos_theta_t * normal;
        auto const reflectance_perp = (eta * cos_theta - cos_theta_t) / (eta * cos_theta + cos_theta_t);
        auto const reflectance_parallel = (cos_theta - eta * cos_theta_t) / (cos_theta + eta * cos_theta_t);
        reflectance = (reflectance_perp * reflectance_perp + reflectance_parallel * reflectance_parallel) / 2.0f;
        return r_out_perp + r_out_parallel;
    }
    template <AnyVec Vector>
    [[nodiscard]] constexpr vec<Vector::size, std::remove_const_t<typename Vector::type>> refract(Vector const &normal, Vector const &incident, float eta, float &cos_theta, float &cos_theta_t, float &reflectance, float &reflectance_perp, float &reflectance_parallel) noexcept
    {
        cos_theta = dot(normal, incident);
        auto const r_out_perp = eta * (incident - cos_theta * normal);
        cos_theta_t = std::sqrt(std::abs(1.0f - squared_length(r_out_perp)));
        auto const r_out_parallel = -cos_theta_t * normal;
        reflectance_perp = (eta * cos_theta - cos_theta_t) / (eta * cos_theta + cos_theta_t);
        reflectance_parallel = (cos_theta - eta * cos_theta_t) / (cos_theta + eta * cos_theta_t);
        reflectance = (reflectance_perp * reflectance_perp + reflectance_parallel * reflectance_parallel) / 2.0f;
        return r_out_perp + r_out_parallel;
    }
} // namespace engine::core::math

namespace std {
    template <size_t size_x, typename vector_type>
    struct hash<engine::core::math::vec<size_x, vector_type>> {
        [[nodiscard]] constexpr size_t operator()(engine::core::math::vec<size_x, vector_type> const &v) const noexcept {
            size_t seed = 0;
            for (auto const &e : v.data) {
                seed ^= std::hash<vector_type>{}(e)+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    template <size_t size_x, size_t size_y, typename matrix_type>
    struct hash<engine::core::math::mat<size_x, size_y, matrix_type>> {
        [[nodiscard]] constexpr size_t operator()(engine::core::math::mat<size_x, size_y, matrix_type> const &m) const noexcept {
            size_t seed = 0;
            for (auto const &e : m.data) {
                seed ^= std::hash<matrix_type>{}(e)+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    template<typename quaternion_type>
    struct hash<engine::core::math::qua<quaternion_type>> {
        [[nodiscard]] constexpr size_t operator()(engine::core::math::qua<quaternion_type> const &q) const noexcept {
            size_t seed = 0;
            for (auto const &e : q.data) {
                seed ^= std::hash<quaternion_type>{}(e)+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
} // namespace std
#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif