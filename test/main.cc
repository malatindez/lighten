#include "pch.hpp"
#include <iostream>
int main()
{
    using namespace engine::math;
    mat<2, 4, float> matrix1;
    mat<4, 3, int> matrix2;
    for (int i = 0; i < matrix1.size.x; i++)
    {
        for (int j = 0; j < matrix1.size.y; j++)
        {
            matrix1[i][j] = rand() % 100;
        }
    }
    for (int i = 0; i < matrix2.size.x; i++)
    {
        for (int j = 0; j < matrix2.size.y; j++)
        {
            matrix2[i][j] = rand() % 100;
        }
    }
    auto matrix3 = matrix1 * matrix2;
    std::cout << matrix1 << std::endl;
    std::cout << matrix2 << std::endl;
    std::cout << matrix3 << std::endl;
}