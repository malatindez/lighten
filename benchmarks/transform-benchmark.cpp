#include <array>
#include "benchmark/benchmark.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <math.hpp>
#define TEST_ARRAY_SIZE 10000


#define FILL_ARRAY(position, scale, quat)               \
state.PauseTiming();                                    \
for (int i = 0; i < TEST_ARRAY_SIZE; i++)               \
{                                                       \
    position.x = static_cast<float>(rand()) / rand();   \
    position.y = static_cast<float>(rand()) / rand();   \
    position.z = static_cast<float>(rand()) / rand();   \
    scale.x = static_cast<float>(rand()) / rand();      \
    scale.y = static_cast<float>(rand()) / rand();      \
    scale.z = static_cast<float>(rand()) / rand();      \
    quat.x = static_cast<float>(rand()) / rand();       \
    quat.y = static_cast<float>(rand()) / rand();       \
    quat.z = static_cast<float>(rand()) / rand();       \
    quat.w = static_cast<float>(rand()) / rand();       \
}   										            \
state.ResumeTiming();                                    \

#define FILL_ARRAY_FLOAT_SCALE(position, scale, quat)   \
state.PauseTiming();                                    \
for (int i = 0; i < TEST_ARRAY_SIZE; i++)               \
{                                                       \
    position.x = static_cast<float>(rand()) / rand();   \
    position.y = static_cast<float>(rand()) / rand();   \
    position.z = static_cast<float>(rand()) / rand();   \
    scale = static_cast<float>(rand()) / rand();      \
    quat.x = static_cast<float>(rand()) / rand();       \
    quat.y = static_cast<float>(rand()) / rand();       \
    quat.z = static_cast<float>(rand()) / rand();       \
    quat.w = static_cast<float>(rand()) / rand();       \
}   										            \
state.ResumeTiming();                                    \

void ComputeWorldTransform(glm::vec3 const& position, glm::vec3 const& scale, glm::quat const& quat, glm::mat4& model, glm::mat4& inv_model)
{
    model = glm::translate(glm::mat4{ 1.0f }, position);
    model = model * glm::mat4_cast(quat);
    model = glm::scale(model, scale);
    inv_model = glm::inverse(model);
}

namespace test1
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat quat;
    };

    struct WorldTransform
    {
        glm::mat4 model;
        glm::mat4 inv_model;
    };

    static void GLM_Transform_SeparateMatrix_AoS(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat)
                for (int i = 0; i < TEST_ARRAY_SIZE; i++)
                {
                    ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                    benchmark::DoNotOptimize(world_transforms[i]);
                }
        }
        delete[] transforms;
        delete[] world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test1::GLM_Transform_SeparateMatrix_AoS);


namespace test2
{
    struct Transform
    {
        glm::vec3 position;
        float padding0;
        glm::vec3 scale;
        float padding1;
        glm::quat quat;
    };

    struct WorldTransform
    {
        glm::mat4 model;
        glm::mat4 inv_model;
    };
    static void GLM_Transform_SeparateMatrix_AoS_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                benchmark::DoNotOptimize(world_transforms[i]);
            }
        }
        delete[] transforms;
        delete[] world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test2::GLM_Transform_SeparateMatrix_AoS_Aligned);

namespace test3
{
    static void GLM_Transform_SeparateMatrix_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        glm::vec3* positions = new glm::vec3[TEST_ARRAY_SIZE];
        glm::vec3* scales = new glm::vec3[TEST_ARRAY_SIZE];
        glm::quat* quats = new glm::quat[TEST_ARRAY_SIZE];
        glm::mat4* world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        glm::mat4* inv_world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(positions[i], scales[i], quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test3::GLM_Transform_SeparateMatrix_SoA);

namespace test4
{
    static void GLM_Transform_SeparateMatrix_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        glm::vec4* positions = new glm::vec4[TEST_ARRAY_SIZE];
        glm::vec4* scales = new glm::vec4[TEST_ARRAY_SIZE];
        glm::quat* quats = new glm::quat[TEST_ARRAY_SIZE];
        glm::mat4* world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        glm::mat4* inv_world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                glm::vec3& position = reinterpret_cast<glm::vec3&>(positions[i]);
                glm::vec3& scale = reinterpret_cast<glm::vec3&>(scales[i]);
                ComputeWorldTransform(position, scale, quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test4::GLM_Transform_SeparateMatrix_SoA_Aligned);

namespace test5
{
    struct Transform
    {
        glm::vec3 position;
        float padding0;
        glm::vec3 scale;
        float padding1;
        glm::quat quat;
        glm::mat4 model;
        glm::mat4 inv_model;
    };


    static void GLM_Transform_MatrixTogether_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test5::GLM_Transform_MatrixTogether_SoA_Aligned);

namespace test6
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat quat;
        glm::mat4 model;
        glm::mat4 inv_model;
    };


    static void GLM_Transform_MatrixTogether_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}

// Register the function as a benchmark
BENCHMARK(test6::GLM_Transform_MatrixTogether_SoA);


void ComputeWorldTransform(glm::vec3 const& position, float const& scale, glm::quat const& quat, glm::mat4& model, glm::mat4& inv_model)
{
    model = glm::translate(glm::mat4{ 1.0f }, position);
    model = model * glm::mat4_cast(quat);
    model = glm::scale(model, glm::vec3{ scale });
    inv_model = glm::inverse(model);
}

namespace test7
{
    struct Transform
    {
        glm::vec3 position;
        float scale;
        glm::quat quat;
    };

    struct WorldTransform
    {
        glm::mat4 model;
        glm::mat4 inv_model;
    };

    static void GLM_TransformFloatScale_SeparateMatrix_AoS(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(transforms[i].position, transforms[i].scale, transforms[i].quat)
                for (int i = 0; i < TEST_ARRAY_SIZE; i++)
                {
                    ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                    benchmark::DoNotOptimize(world_transforms[i]);
                }
        }
        delete[] transforms;
        delete[] world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test7::GLM_TransformFloatScale_SeparateMatrix_AoS);

namespace test8
{
    static void GLM_TransformFloatScale_SeparateMatrix_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        glm::vec3* positions = new glm::vec3[TEST_ARRAY_SIZE];
        float* scales = new float[TEST_ARRAY_SIZE];
        glm::quat* quats = new glm::quat[TEST_ARRAY_SIZE];
        glm::mat4* world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        glm::mat4* inv_world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(positions[i], scales[i], quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test8::GLM_TransformFloatScale_SeparateMatrix_SoA);

namespace test9
{
    static void GLM_TransformFloatScale_SeparateMatrix_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        glm::vec4* positions = new glm::vec4[TEST_ARRAY_SIZE];
        float* scales = new float[TEST_ARRAY_SIZE];
        glm::quat* quats = new glm::quat[TEST_ARRAY_SIZE];
        glm::mat4* world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        glm::mat4* inv_world_transforms = new glm::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                glm::vec3& position = reinterpret_cast<glm::vec3&>(positions[i]);
                glm::vec3& scale = reinterpret_cast<glm::vec3&>(scales[i]);
                ComputeWorldTransform(position, scale, quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test9::GLM_TransformFloatScale_SeparateMatrix_SoA_Aligned);

namespace test10
{
    struct Transform
    {
        glm::vec3 position;
        float scale;
        glm::quat quat;
        glm::mat4 model;
        glm::mat4 inv_model;
    };


    static void GLM_TransformFloatScale_MatrixTogether_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test10::GLM_TransformFloatScale_MatrixTogether_SoA_Aligned);



void ComputeWorldTransform(mal_math::vec3 const& position, mal_math::vec3 const& scale, mal_math::quat const& quat, mal_math::mat4& model, mal_math::mat4& inv_model)
{
    model = mal_math::translate(mal_math::mat4{ 1.0f }, position);
    model = model * quat.as_mat4();
    model = mal_math::scale(model, scale);
    inv_model = mal_math::inverse(model);
}

namespace test11
{
    struct Transform
    {
        mal_math::vec3 position;
        mal_math::vec3 scale;
        mal_math::quat quat;
    };

    struct WorldTransform
    {
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };

    static void MAL_MATH_Transform_SeparateMatrix_AoS(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                benchmark::DoNotOptimize(world_transforms[i]);
            }
        }
        delete[] transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test11::MAL_MATH_Transform_SeparateMatrix_AoS);

namespace test12
{
    struct Transform
    {
        mal_math::vec3 position;
        float padding0;
        mal_math::vec3 scale;
        float padding1;
        mal_math::quat quat;
    };

    struct WorldTransform
    {
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };

    static void MAL_MATH_Transform_SeparateMatrix_AoS_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                benchmark::DoNotOptimize(world_transforms[i]);
            }
        }
        delete[] transforms;
        delete[] world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test12::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned);

namespace test13
{
    static void MAL_MATH_Transform_SeparateMatrix_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        mal_math::vec3* positions = new mal_math::vec3[TEST_ARRAY_SIZE];
        mal_math::vec3* scales = new mal_math::vec3[TEST_ARRAY_SIZE];
        mal_math::quat* quats = new mal_math::quat[TEST_ARRAY_SIZE];
        mal_math::mat4* world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        mal_math::mat4* inv_world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(positions[i], scales[i], quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}

// Register the function as a benchmark
BENCHMARK(test13::MAL_MATH_Transform_SeparateMatrix_SoA);

namespace test14
{
    static void MAL_MATH_Transform_SeparateMatrix_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        mal_math::vec4* positions = new mal_math::vec4[TEST_ARRAY_SIZE];
        mal_math::vec4* scales = new mal_math::vec4[TEST_ARRAY_SIZE];
        mal_math::quat* quats = new mal_math::quat[TEST_ARRAY_SIZE];
        mal_math::mat4* world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        mal_math::mat4* inv_world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                mal_math::vec3& position = reinterpret_cast<mal_math::vec3&>(positions[i]);
                mal_math::vec3& scale = reinterpret_cast<mal_math::vec3&>(scales[i]);
                ComputeWorldTransform(position, scale, quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}

// Register the function as a benchmark
BENCHMARK(test14::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned);
namespace test15
{
    struct Transform
    {
        mal_math::vec3 position;
        float padding0;
        mal_math::vec3 scale;
        float padding1;
        mal_math::quat quat;
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };


    static void MAL_MATH_Transform_MatrixTogether_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test15::MAL_MATH_Transform_MatrixTogether_SoA_Aligned);

namespace test16
{
    struct Transform
    {
        mal_math::vec3 position;
        mal_math::vec3 scale;
        mal_math::quat quat;
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };

    static void MAL_MATH_Transform_MatrixTogether_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}

// Register the function as a benchmark
BENCHMARK(test16::MAL_MATH_Transform_MatrixTogether_SoA);


void ComputeWorldTransform(mal_math::vec3 const& position, float const& scale, mal_math::quat const& quat, mal_math::mat4& model, mal_math::mat4& inv_model)
{
    model = mal_math::translate(mal_math::mat4{ 1.0f }, position);
    model = model * quat.as_mat4();
    model = mal_math::scale(model, mal_math::vec3{ scale });
    inv_model = mal_math::inverse(model);
}

namespace test17
{
    struct Transform
    {
        mal_math::vec3 position;
        float scale;
        mal_math::quat quat;
    };

    struct WorldTransform
    {
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };

    static void MAL_MATH_TransformFloatScale_SeparateMatrix_AoS(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        WorldTransform* world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(transforms[i].position, transforms[i].scale, transforms[i].quat)
                for (int i = 0; i < TEST_ARRAY_SIZE; i++)
                {
                    ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, world_transforms[i].model, world_transforms[i].inv_model);
                    benchmark::DoNotOptimize(world_transforms[i]);
                }
        }
        delete[] transforms;
        delete[] world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test17::MAL_MATH_TransformFloatScale_SeparateMatrix_AoS);

namespace test18
{
    static void MAL_MATH_TransformFloatScale_SeparateMatrix_SoA(benchmark::State& state) {
        srand(0xABCDEF00);
        mal_math::vec3* positions = new mal_math::vec3[TEST_ARRAY_SIZE];
        float* scales = new float[TEST_ARRAY_SIZE];
        mal_math::quat* quats = new mal_math::quat[TEST_ARRAY_SIZE];
        mal_math::mat4* world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        mal_math::mat4* inv_world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(positions[i], scales[i], quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test18::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA);

namespace test19
{
    static void MAL_MATH_TransformFloatScale_SeparateMatrix_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        mal_math::vec4* positions = new mal_math::vec4[TEST_ARRAY_SIZE];
        float* scales = new float[TEST_ARRAY_SIZE];
        mal_math::quat* quats = new mal_math::quat[TEST_ARRAY_SIZE];
        mal_math::mat4* world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        mal_math::mat4* inv_world_transforms = new mal_math::mat4[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(positions[i], scales[i], quats[i]);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                mal_math::vec3& position = reinterpret_cast<mal_math::vec3&>(positions[i]);
                mal_math::vec3& scale = reinterpret_cast<mal_math::vec3&>(scales[i]);
                ComputeWorldTransform(position, scale, quats[i], world_transforms[i], inv_world_transforms[i]);
                benchmark::DoNotOptimize(world_transforms[i]);
                benchmark::DoNotOptimize(inv_world_transforms[i]);
            }
        }
        delete[] positions;
        delete[] scales;
        delete[] quats;
        delete[] world_transforms;
        delete[] inv_world_transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test19::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA_Aligned);

namespace test20
{
    struct Transform
    {
        mal_math::vec3 position;
        float scale;
        mal_math::quat quat;
        mal_math::mat4 model;
        mal_math::mat4 inv_model;
    };


    static void MAL_MATH_TransformFloatScale_MatrixTogether_SoA_Aligned(benchmark::State& state) {
        srand(0xABCDEF00);
        Transform* transforms = new Transform[TEST_ARRAY_SIZE];
        // Code inside this loop is measured repeatedly
        for (auto _ : state) {
            FILL_ARRAY_FLOAT_SCALE(transforms[i].position, transforms[i].scale, transforms[i].quat);
            for (int i = 0; i < TEST_ARRAY_SIZE; i++)
            {
                ComputeWorldTransform(transforms[i].position, transforms[i].scale, transforms[i].quat, transforms[i].model, transforms[i].inv_model);
                benchmark::DoNotOptimize(transforms[i].model);
                benchmark::DoNotOptimize(transforms[i].inv_model);
            }
        }
        delete[] transforms;
    }
}
// Register the function as a benchmark
BENCHMARK(test20::MAL_MATH_TransformFloatScale_MatrixTogether_SoA_Aligned);


BENCHMARK_MAIN();

/*

Run on (16 X 3199.12 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)


------------------------------------------------------------------------------------------------
    Array size: 100
------------------------------------------------------------------------------------------------
Compiled with MSVC v143 14.38.33130,
            /MP /GS /W4 /O2 /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
            /GL /Gm- /Zc:forScope /Zc:inline /external:W0 /errorReport:prompt
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                     8445 ns         2825 ns       298667         84.45 ns        28.25 ns <-- 1
test2::GLM_Transform_SeparateMatrix_AoS_Aligned             8457 ns         2699 ns       248889         84.57 ns        26.99 ns <-- 2
test3::GLM_Transform_SeparateMatrix_SoA                     8603 ns         3292 ns       280000         86.03 ns        32.92 ns
test4::GLM_Transform_SeparateMatrix_SoA_Aligned             8557 ns         2490 ns       213333         85.57 ns        24.90 ns <-- 3
test5::GLM_Transform_MatrixTogether_SoA_Aligned             8578 ns         2832 ns       320000         85.78 ns        28.32 ns
test6::GLM_Transform_MatrixTogether_SoA                     8713 ns         2616 ns       298667         87.13 ns        26.16 ns

test7::MAL_MATH_Transform_SeparateMatrix_AoS               29089 ns         6696 ns        74667        290.89 ns        66.96 ns <-- 3
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned       30012 ns         7952 ns       112000        300.12 ns        79.52 ns
test9::MAL_MATH_Transform_SeparateMatrix_SoA               28671 ns         9531 ns       100000        286.71 ns        95.31 ns <-- 2
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned      28370 ns         8161 ns        74667        283.70 ns        81.61 ns <-- 1
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned      29124 ns         8371 ns        89600        291.24 ns        83.71 ns
test12::MAL_MATH_Transform_MatrixTogether_SoA              31564 ns         8022 ns        89600        315.64 ns        80.22 ns

Compiled with Clang 16,
            /MP /GS /W4 /Ox /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                     5660 ns         1758 ns       640000         56.60 ns        17.58 ns <-- 3
test2::GLM_Transform_SeparateMatrix_AoS_Aligned             5607 ns         1779 ns       298667         56.07 ns        17.79 ns <-- 2
test3::GLM_Transform_SeparateMatrix_SoA                     5607 ns         2148 ns       560000         56.07 ns        21.48 ns <-- 1
test4::GLM_Transform_SeparateMatrix_SoA_Aligned             5802 ns         1381 ns       373333         58.02 ns        13.81 ns
test5::GLM_Transform_MatrixTogether_SoA_Aligned             5677 ns         1569 ns       448000         56.77 ns        15.69 ns
test6::GLM_Transform_MatrixTogether_SoA                     5681 ns         1541 ns       456145         56.81 ns        15.41 ns

test7::MAL_MATH_Transform_SeparateMatrix_AoS               15875 ns         7188 ns       100000        158.75 ns        71.88 ns <-- 3
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned       15654 ns         5156 ns       100000        156.54 ns        51.56 ns <-- 1
test9::MAL_MATH_Transform_SeparateMatrix_SoA               16434 ns         6696 ns        74667        164.34 ns        66.96 ns
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned      15967 ns         4604 ns       112000        159.67 ns        46.04 ns
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned      15856 ns         4604 ns       112000        158.56 ns        46.04 ns <-- 2
test12::MAL_MATH_Transform_MatrixTogether_SoA              16708 ns         5138 ns       133803        167.08 ns        51.38 ns


Compiled with Intel 2024,
            /MP /GS /W4 /Zc:wchar_t /Ox /Ob2 /fp:fast /Zc:forScope /arch:CORE-AVX2 /Oi /MD /std:c++20 /EHsc /nologo /Ot
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                     2706 ns          830 ns       640000        27.06 ns          8.30 ns <-- 2
test2::GLM_Transform_SeparateMatrix_AoS_Aligned             2607 ns          854 ns       896000        26.07 ns          8.54 ns
test3::GLM_Transform_SeparateMatrix_SoA                     2572 ns         1036 ns       497778        25.72 ns         10.36 ns
test4::GLM_Transform_SeparateMatrix_SoA_Aligned             2542 ns          691 ns       746667        25.42 ns          6.91 ns <-- 1
test5::GLM_Transform_MatrixTogether_SoA_Aligned             2529 ns          893 ns      1120000        25.29 ns          8.93 ns
test6::GLM_Transform_MatrixTogether_SoA                     2556 ns          844 ns      1000000        25.56 ns          8.44 ns <-- 3

test7::MAL_MATH_Transform_SeparateMatrix_AoS                5920 ns         1500 ns       448000        59.20 ns         15.00 ns <-- 2
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned        5788 ns         1270 ns       640000        57.88 ns         12.70 ns <-- 1
test9::MAL_MATH_Transform_SeparateMatrix_SoA                5809 ns         1758 ns       640000        58.09 ns         17.58 ns
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned       5702 ns         1611 ns       640000        57.02 ns         16.11 ns <-- 3
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned       5732 ns         1883 ns       373333        57.32 ns         18.83 ns
test12::MAL_MATH_Transform_MatrixTogether_SoA               5844 ns         1660 ns       320000        58.44 ns         16.60 ns

------------------------------------------------------------------------------------------------
    Array size: 10000
------------------------------------------------------------------------------------------------

Compiled with MSVC v143 14.38.33130,
            /MP /GS /W4 /O2 /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
            /GL /Gm- /Zc:forScope /Zc:inline /external:W0 /errorReport:prompt
--------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations   Time/Transform   CPU/Transform
--------------------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                             796701 ns       171611 ns         3733       79.6701 ns       17.1611 ns  
test2::GLM_Transform_SeparateMatrix_AoS_Aligned                     780208 ns       244827 ns         4978       78.0208 ns       24.4827 ns  
test3::GLM_Transform_SeparateMatrix_SoA                             816678 ns       207456 ns         3088       81.6678 ns       20.7456 ns  
test4::GLM_Transform_SeparateMatrix_SoA_Aligned                     792412 ns       213109 ns         3446       79.2412 ns       21.3109 ns  
test5::GLM_Transform_MatrixTogether_SoA_Aligned                     801012 ns       207162 ns         2489       80.1012 ns       20.7162 ns  
test6::GLM_Transform_MatrixTogether_SoA                             797334 ns       241683 ns         4073       79.7334 ns       24.1683 ns  
test7::GLM_TransformFloatScale_SeparateMatrix_AoS                   775058 ns       296779 ns         1948       77.5058 ns       29.6779 ns  
test8::GLM_TransformFloatScale_SeparateMatrix_SoA                   796040 ns       219544 ns         3345       79.6040 ns       21.9544 ns  
test9::GLM_TransformFloatScale_SeparateMatrix_SoA_Aligned           796974 ns       258789 ns         3200       79.6974 ns       25.8789 ns  
test10::GLM_TransformFloatScale_MatrixTogether_SoA_Aligned          792882 ns       234375 ns         3200       79.2882 ns       23.4375 ns  

test11::MAL_MATH_Transform_SeparateMatrix_AoS                      2582630 ns       767299 ns          896      258.2630 ns       76.7299 ns  
test12::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned              2575747 ns       828125 ns         1000      257.5747 ns       82.8125 ns  
test13::MAL_MATH_Transform_SeparateMatrix_SoA                      2513879 ns       836680 ns          747      251.3879 ns       83.6680 ns  
test14::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned              2529358 ns       711178 ns          747      252.9358 ns       71.1178 ns  
test15::MAL_MATH_Transform_MatrixTogether_SoA_Aligned              2568256 ns       794798 ns          865      256.8256 ns       79.4798 ns  
test16::MAL_MATH_Transform_MatrixTogether_SoA                      2536071 ns       784988 ns          836      253.6071 ns       78.4988 ns  
test17::MAL_MATH_TransformFloatScale_SeparateMatrix_AoS            2591373 ns       751831 ns          956      259.1373 ns       75.1831 ns  
test18::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA            2552267 ns       796875 ns         1000      255.2267 ns       79.6875 ns  
test19::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA_Aligned    2517748 ns       767299 ns         1120      251.7748 ns       76.7299 ns  
test20::MAL_MATH_TransformFloatScale_MatrixTogether_SoA_Aligned    2562456 ns       655692 ns         1120      256.2456 ns       65.5692 ns  

Compiled with Clang 16,
            /MP /GS /W4 /O2 /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
--------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations   Time/Transform   CPU/Transform
--------------------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                             548792 ns       151822 ns         6175       54.8792 ns      15.1822 ns
test2::GLM_Transform_SeparateMatrix_AoS_Aligned                     535130 ns       157286 ns         4073       53.5130 ns      15.7286 ns
test3::GLM_Transform_SeparateMatrix_SoA                             544450 ns       200911 ns         3733       54.4450 ns      20.0911 ns
test4::GLM_Transform_SeparateMatrix_SoA_Aligned                     548370 ns       158698 ns         3446       54.8370 ns      15.8698 ns
test5::GLM_Transform_MatrixTogether_SoA_Aligned                     541375 ns       244849 ns         3446       54.1375 ns      24.4849 ns
test6::GLM_Transform_MatrixTogether_SoA                             535753 ns       150663 ns         4978       53.5753 ns      15.0663 ns
test7::GLM_TransformFloatScale_SeparateMatrix_AoS                   531029 ns       179983 ns         3733       53.1029 ns      17.9983 ns
test8::GLM_TransformFloatScale_SeparateMatrix_SoA                   551916 ns       146484 ns         4480       55.1916 ns      14.6484 ns
test9::GLM_TransformFloatScale_SeparateMatrix_SoA_Aligned           543714 ns       184978 ns         5575       54.3714 ns      18.4978 ns
test10::GLM_TransformFloatScale_MatrixTogether_SoA_Aligned          546605 ns       100446 ns         5600       54.6605 ns      10.0446 ns
test11::MAL_MATH_Transform_SeparateMatrix_AoS                      1337847 ns       409807 ns         1792      133.7847 ns      40.9807 ns
test12::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned              1348613 ns       373594 ns         2133      134.8613 ns      37.3594 ns
test13::MAL_MATH_Transform_SeparateMatrix_SoA                      1338177 ns       384304 ns         1545      133.8177 ns      38.4304 ns
test14::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned              1360157 ns       546875 ns         1000      136.0157 ns      54.6875 ns
test15::MAL_MATH_Transform_MatrixTogether_SoA_Aligned              1388044 ns       388570 ns         2091      138.8044 ns      38.8570 ns
test16::MAL_MATH_Transform_MatrixTogether_SoA                      1473119 ns       329641 ns         1659      147.3119 ns      32.9641 ns
test17::MAL_MATH_TransformFloatScale_SeparateMatrix_AoS            1572608 ns       593750 ns         1000      157.2608 ns      59.3750 ns
test18::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA            1537686 ns       367647 ns         1445      153.7686 ns      36.7647 ns
test19::MAL_MATH_TransformFloatScale_SeparateMatrix_SoA_Aligned    1372687 ns       341797 ns         1600      137.2687 ns      34.1797 ns
test20::MAL_MATH_TransformFloatScale_MatrixTogether_SoA_Aligned    1439617 ns       360939 ns         2987      143.9617 ns      36.0939 ns

Compiled with Intel 2024,
            /MP /GS /W4 /Zc:wchar_t /O3 /Ob2 /fp:fast /Zc:forScope /arch:CORE-AVX2 /Oi /MD /std:c++20 /EHsc /nologo /Ot
--------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time             CPU   Iterations   Time/Transform   CPU/Transform
--------------------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                   181421 ns        48828 ns        11200       18.1421 ns       4.8828 ns
test2::GLM_Transform_SeparateMatrix_AoS_Aligned           184530 ns        62500 ns        10000       18.4530 ns       6.2500 ns
test3::GLM_Transform_SeparateMatrix_SoA                   170901 ns        44643 ns        11200       17.0901 ns       4.4643 ns <-- 2
test4::GLM_Transform_SeparateMatrix_SoA_Aligned           168762 ns        56250 ns        10000       16.8762 ns       5.6250 ns <-- 1
test5::GLM_Transform_MatrixTogether_SoA_Aligned           187867 ns        70843 ns        11469       18.7867 ns       7.0843 ns
test6::GLM_Transform_MatrixTogether_SoA                   180798 ns        61523 ns        16000       18.0798 ns       6.1523 ns <-- 3

test7::MAL_MATH_Transform_SeparateMatrix_AoS              475979 ns       167411 ns         4480       47.5979 ns      16.7411 ns <-- 1
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned      478498 ns       202288 ns         4480       47.8498 ns      20.2288 ns
test9::MAL_MATH_Transform_SeparateMatrix_SoA              475981 ns       153450 ns         4073       47.5981 ns      15.3450 ns <-- 2
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned     476741 ns       153809 ns         6400       47.6741 ns      15.3809 ns <-- 3
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned     487884 ns       167411 ns         4480       48.7884 ns      16.7411 ns
test12::MAL_MATH_Transform_MatrixTogether_SoA             488862 ns       145725 ns         4825       48.8862 ns      14.5725 ns
*/