#include "pch.hpp"
#include "math/math.hpp"
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
    imat4 matrix(1);
    matrix = imat4(12, 34, 12, 5, 102, 3, 451, 2, 52, 5, 12, 5, 2, 1, 2, 415);
    mat4 inv_ = inv(mat4(matrix));
    std::cout << matrix << std::endl;
    std::cout << imat4(adj(mat4(matrix))) << std::endl;
    std::cout << inv_ << std::endl;
    mat4 a = mat4(matrix) * inv_;
    std::cout << a << std::endl;
    std::cout << imat4(a) << std::endl;
}