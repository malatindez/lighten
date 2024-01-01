#pragma once
#include "../math.hpp"
#include <random>
/**
 * @file random.hpp
 * @brief Provides functionality for random number generation and manipulation in vector spaces.
 *
 * This file contains utility functions for generating random vectors in 2D, 3D, and 4D spaces.
 * It also provides Poisson Disk sampling for generating a random, yet evenly distributed set of points.
 */
namespace engine::core::math::random
{
    namespace _detail
    {
        /**
         * @brief Generates a unit vector with random direction.
         * @tparam T Any vector type.
         * @param gen Reference to a random number generator.
         * @return A normalized random vector.
         */
        template <typename T>
        inline T UnitRandom(std::mt19937 &gen) noexcept
        {
            std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
            T p;
            for (typename T::length_type i = 0; i < T::length(); i++)
                p[i] = dis(gen);
            return normalize(p);
        }
        /**
         * @brief Generates a random vector between two other vectors.
         * @tparam T Resulting vector type.
         * @tparam U Input vector types.
         * @param gen Reference to a random number generator.
         * @param from Start vector.
         * @param to End vector.
         * @return A random vector between `from` and `to`.
         */
        template <glm::length_t size, typename Type, glm::qualifier Q>
        inline glm::vec<size, Type, Q> RandomVector(std::mt19937 &gen, glm::vec<size, Type, Q> const &from, glm::vec<size, Type, Q> const &to) noexcept
        {
            std::uniform_real_distribution<float> dis(0.0f, 1.0f);
            glm::vec<size, Type, Q> p;
            for (glm::length_t i = 0; i < size; i++)
                p[i] = static_cast<Type>(from[i] + dis(gen) * (to[i] - from[i]));
            return p;
        }
    } // namespace _detail
    /**
     * @brief Generates a random 2D float direction.
     * @param gen Reference to a random number generator.
     * @return A normalized random 2D vector.
     */
    inline glm::vec2 RandomDirection2(std::mt19937 &gen) noexcept { return _detail::UnitRandom<glm::vec2>(gen); }
    /**
     * @brief Generates a random 3D float direction.
     * @param gen Reference to a random number generator.
     * @return A normalized random 3D vector.
     */
    inline glm::vec3 RandomDirection3(std::mt19937 &gen) noexcept { return _detail::UnitRandom<glm::vec3>(gen); }
    /**
     * @brief Generates a random 4D float direction.
     * @param gen Reference to a random number generator.
     * @return A normalized random 4D vector.
     */
    inline glm::vec4 RandomDirection4(std::mt19937 &gen) noexcept { return _detail::UnitRandom<glm::vec4>(gen); }

    template <typename U>
    U RandomVector(std::mt19937 &gen, U const &from, U const &to) noexcept { return _detail::RandomVector(gen, from, to); }

    namespace poisson_disc
    {
        namespace _detail
        {
            /**
             * @brief Generates a set of points distributed according to Poisson Disk Sampling.
             * @tparam U Vector type.
             * @param gen Reference to a random number generator.
             * @param from Start vector.
             * @param to End vector.
             * @param min_distance Minimum distance between points.
             * @param max_attempts Maximum number of attempts before giving up on finding a new point.
             * @return A vector containing the generated points.
             */
            template <typename U>
            inline std::vector<U> PoissonDisc(std::mt19937 &gen, U const &from, U const &to, float min_distance, size_t max_attempts = 30) noexcept
            {
                std::vector<U> result;
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                std::vector<U> active;
                result.push_back(::engine::core::math::random::_detail::RandomVector(gen, from, to));
                active.push_back(result.back());
                while (!active.empty())
                {
                    std::uniform_int_distribution<size_t> dist2(0, active.size() - 1);
                    size_t index = dist2(gen);
                    U point = active[index];
                    bool found = false;
                    for (size_t i = 0; i < max_attempts; ++i)
                    {
                        U new_point = point + ::engine::core::math::random::_detail::RandomVector(gen, U{-min_distance}, U{min_distance});
                        if (new_point.x < from.x || new_point.x > to.x || new_point.y < from.y || new_point.y > to.y)
                            continue;
                        bool ok = true;
                        for (auto const &p : result)
                        {
                            if (distance(p, new_point) < min_distance)
                            {
                                ok = false;
                                break;
                            }
                        }
                        if (ok)
                        {
                            found = true;
                            result.push_back(new_point);
                            active.push_back(new_point);
                            break;
                        }
                    }
                    if (!found)
                    {
                        active.erase(active.begin() + index);
                    }
                }
                return result;
            }
        } // namespace _detail
        /**
         * @brief Wrapper function for generating Poisson Disk sampled 2D points.
         * @param gen Reference to a random number generator.
         * @param from Start vector.
         * @param to End vector.
         * @param min_distance Minimum distance between points.
         * @param max_attempts Maximum number of attempts before giving up on finding a new point.
         * @return A vector containing the generated points.
         */
        inline std::vector<glm::vec2> Generate(std::mt19937 &gen, glm::vec2 const &from, glm::vec2 const &to, float min_distance, size_t max_attempts = 30) noexcept
        {
            return _detail::PoissonDisc<glm::vec2>(gen, from, to, min_distance, max_attempts);
        }
    } // namespace poisson_disc
} // namespace mal_math::random