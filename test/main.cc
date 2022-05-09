#include "pch.hpp"
#include <iostream>
using namespace engine::math;
template<size_t a, size_t b, Primitive T>
void random_fill(mat<a, b, T> & matrix) 
{
    for (int i = 0; i < matrix.size.x; i++)
    {
        for (int j = 0; j < matrix.size.y; j++)
        {
            matrix[i][j] = rand() %2 ;
        }
    }
}
int main()
{
    mat<5, 100, float> matrix1;
    mat<100, 3, double> matrix2;
    mat<3, 1242, int> matrix3;
    mat<1242, 16, unsigned int> matrix4;
    random_fill(matrix1);
    random_fill(matrix2);
    random_fill(matrix3);
    random_fill(matrix4);
    auto matrix5 = matrix1 * matrix2 * matrix3 * matrix4;
    std::cout << matrix5;
}