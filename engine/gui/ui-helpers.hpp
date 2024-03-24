#pragma once
#include "../include/library-pch.hpp"
#include "../include/imgui.hpp"
#include "../include/entt.hpp"
namespace lighten::gui
{

    template <typename Vector>
    inline std::string FormatVectorToString(Vector vec, uint32_t precision = 3)
    {
        std::stringstream ss;
        size_t max = 0;
        for (size_t i = 0; i < Vector::size; i++)
        {
            std::stringstream ss2;
            ss2 << std::setprecision(precision) << vec[i];
            max = std::max(max, ss2.str().size());
        }
        size_t size = max + 1 + precision;
        for (size_t i = 0; i < Vector::size; i++)
        {
            ss << std::left << std::setw(size) << std::setprecision(precision) << vec[i];
            ss << " ";
        }
        return ss.str();
    }

    template <glm::length_t size_x, glm::length_t size_y, typename matrix_type, glm::qualifier Q>
    inline std::string FormatMatrixToString(glm::mat<size_x, size_y, matrix_type, Q> mat, uint32_t precision = 3)
    {
        std::stringstream ss;
        glm::length_t max = 0;
        for (glm::length_t i = 0; i < size_x * size_y; i++)
        {
            std::stringstream ss2;
            ss2 << std::setprecision(precision) << reinterpret_cast<const matrix_type *>(&mat)[i];
            max = std::max(max, glm::length_t(ss2.str().size()));
        }
        uint32_t size = (uint32_t)max;
        for (glm::length_t column = 0; column < size_x; column++)
        {
            for (glm::length_t row = 0; row < size_y; row++)
            {
                ss << std::left << std::setw(size) << std::setprecision(precision) << mat[column][row];
                ss << " ";
            }
            ss << std::endl;
        }
        return ss.str();
    }


    template<typename T>
    inline void DrawComponent(entt::registry & registry, entt::entity entity, std::string &entity_str, T &t);
}