#include "pch.hpp"
#include <iostream>

using namespace engine::math;
template <size_t a, size_t b, Primitive T>
void random_fill(mat<a, b, T> &matrix)
{
    for (int i = 0; i < matrix.size.x; i++)
    {
        for (int j = 0; j < matrix.size.y; j++)
        {
            matrix[i][j] = rand() % 2;
        }
    }
}
constexpr float pi = 3.141592653589897932384626433;
             
int main()
{
    mat4 matrix(1);
    std::cout << matrix << std::endl;
    matrix = translate(matrix, vec3{1, 2, 3});
    std::cout << matrix << std::endl;
    matrix = rotate(matrix, pi / 2, vec3{1, 1, 1});


    matrix = mat4(1, 2, vec2{5, 4},
                dvec3{1, 2, 3}, 4,
                1, vec<6, unsigned long long>{ vec2{5, 7}, 3,vec3{6, 7, 7}},
                   4);
    std::cout << matrix << std::endl;
}