#include "mat.hpp"
namespace engine::core::math
{
    template struct mat<4, 4, float>;
    template struct mat<4, 4, double>;
    template struct mat<4, 4, int>;

    template struct mat<3, 3, float>;
    template struct mat<3, 3, double>;
    template struct mat<3, 3, int>;

    template struct mat<2, 2, float>;
    template struct mat<2, 2, double>;
    template struct mat<2, 2, int>;

    template struct mat<2, 3, float>;
    template struct mat<2, 3, double>;
    template struct mat<2, 3, int>;

    template struct mat<3, 2, float>;
    template struct mat<3, 2, double>;
    template struct mat<3, 2, int>;

    template struct mat<2, 4, float>;
    template struct mat<2, 4, double>;
    template struct mat<2, 4, int>;

    template struct mat<4, 2, float>;
    template struct mat<4, 2, double>;
    template struct mat<4, 2, int>;

    template struct mat<3, 4, float>;
    template struct mat<3, 4, double>;
    template struct mat<3, 4, int>;

    template struct mat<4, 3, float>;
    template struct mat<4, 3, double>;
    template struct mat<4, 3, int>;

        template struct rmat<4, 4, float>;
    template struct rmat<4, 4, double>;
    template struct rmat<4, 4, int>;

    template struct rmat<3, 3, float>;
    template struct rmat<3, 3, double>;
    template struct rmat<3, 3, int>;

    template struct rmat<2, 2, float>;
    template struct rmat<2, 2, double>;
    template struct rmat<2, 2, int>;

    template struct rmat<2, 3, float>;
    template struct rmat<2, 3, double>;
    template struct rmat<2, 3, int>;

    template struct rmat<3, 2, float>;
    template struct rmat<3, 2, double>;
    template struct rmat<3, 2, int>;

    template struct rmat<2, 4, float>;
    template struct rmat<2, 4, double>;
    template struct rmat<2, 4, int>;

    template struct rmat<4, 2, float>;
    template struct rmat<4, 2, double>;
    template struct rmat<4, 2, int>;

    template struct rmat<3, 4, float>;
    template struct rmat<3, 4, double>;
    template struct rmat<3, 4, int>;

    template struct rmat<4, 3, float>;
    template struct rmat<4, 3, double>;
    template struct rmat<4, 3, int>;
} // namespace engine::core::math