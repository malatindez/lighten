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

void ComputeWorldTransform(glm::vec3 position, glm::vec3 scale, glm::quat quat, glm::mat4& model, glm::mat4& inv_model)
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
    Transform *transforms = new Transform[TEST_ARRAY_SIZE];
    WorldTransform *world_transforms = new WorldTransform[TEST_ARRAY_SIZE];
    // Code inside this loop is measured repeatedly
    for (auto _ : state) {
        FILL_ARRAY(transforms[i].position, transforms[i].scale, transforms[i].quat)
      for(int i = 0; i < TEST_ARRAY_SIZE; i++)
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
      for(int i = 0; i < TEST_ARRAY_SIZE; i++)
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
                glm::vec3 &position = reinterpret_cast<glm::vec3&>(positions[i]);
                glm::vec3 &scale = reinterpret_cast<glm::vec3&>(scales[i]);
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

void ComputeWorldTransform(mal_math::vec3 const& position, mal_math::vec3 const& scale, mal_math::quat const& quat, mal_math::mat4& model, mal_math::mat4& inv_model)
{
    model = mal_math::translate(mal_math::mat4{ 1.0f }, position);
    model = model * quat.as_mat4();
    model = mal_math::scale(model, scale);
	inv_model = mal_math::inverse(model);
}

namespace test7
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
BENCHMARK(test7::MAL_MATH_Transform_SeparateMatrix_AoS);

namespace test8
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
BENCHMARK(test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned);

namespace test9
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
BENCHMARK(test9::MAL_MATH_Transform_SeparateMatrix_SoA);

namespace test10
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
BENCHMARK(test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned);
namespace test11
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
BENCHMARK(test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned);

namespace test12
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
BENCHMARK(test12::MAL_MATH_Transform_MatrixTogether_SoA);

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
            /MP /GS /W4 /Ox /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column 
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
            /MP /GS /W4 /Ox /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
            /GL /Gm- /Zc:forScope /Zc:inline /external:W0 /errorReport:prompt
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                   762968 ns       258452 ns         3446       76.2968 ns      25.8452 ns <-- 2
test2::GLM_Transform_SeparateMatrix_AoS_Aligned           757219 ns       288809 ns         3733       75.7219 ns      28.8809 ns <-- 1
test3::GLM_Transform_SeparateMatrix_SoA                   765070 ns       235411 ns         4978       76.5070 ns      23.5411 ns <-- 3
test4::GLM_Transform_SeparateMatrix_SoA_Aligned           772879 ns       272067 ns         3733       77.2879 ns      27.2067 ns
test5::GLM_Transform_MatrixTogether_SoA_Aligned           771798 ns       230164 ns         2987       77.1798 ns      23.0164 ns
test6::GLM_Transform_MatrixTogether_SoA                   768268 ns       249051 ns         2635       76.8268 ns      24.9051 ns

test7::MAL_MATH_Transform_SeparateMatrix_AoS             2609016 ns       781250 ns          640      260.9016 ns      78.1250 ns
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned     2600442 ns      1150949 ns          896      260.0442 ns     115.0949 ns <-- 3
test9::MAL_MATH_Transform_SeparateMatrix_SoA             2604089 ns      1000000 ns         1000      260.4089 ns     100.0000 ns
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned    2572922 ns       983099 ns          747      257.2922 ns      98.3099 ns <-- 1
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned    2592539 ns       575474 ns          896      259.2539 ns      57.5474 ns <-- 2
test12::MAL_MATH_Transform_MatrixTogether_SoA            2620281 ns       784738 ns          896      262.0281 ns      78.4738 ns

Compiled with Clang 16, 
            /MP /GS /W4 /Ox /Ob2 /fp:fast /WX- /arch:AVX2 /Gd /Oi /MD /std:c++20 /EHsc /nologo /Ot /diagnostics:column
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
test1::GLM_Transform_SeparateMatrix_AoS                   480152 ns       137765 ns         8960       48.0152 ns      13.7765 ns <-- 2
test2::GLM_Transform_SeparateMatrix_AoS_Aligned           480470 ns       138126 ns         3733       48.0470 ns      13.8126 ns <-- 3
test3::GLM_Transform_SeparateMatrix_SoA                   479526 ns       178912 ns         4978       47.9526 ns      17.8912 ns <-- 1 
test4::GLM_Transform_SeparateMatrix_SoA_Aligned           483852 ns       167411 ns         4480       48.3852 ns      16.7411 ns 
test5::GLM_Transform_MatrixTogether_SoA_Aligned           489989 ns       122760 ns         4073       48.9989 ns      12.2760 ns
test6::GLM_Transform_MatrixTogether_SoA                   484260 ns       130432 ns         4073       48.4260 ns      13.0432 ns

test7::MAL_MATH_Transform_SeparateMatrix_AoS             1398324 ns       460379 ns         1120      139.8324 ns      46.0379 ns <-- 2
test8::MAL_MATH_Transform_SeparateMatrix_AoS_Aligned     1412610 ns       500801 ns         2028      141.2610 ns      50.0801 ns 
test9::MAL_MATH_Transform_SeparateMatrix_SoA             1400012 ns       448229 ns         2231      140.0012 ns      44.8229 ns <-- 3
test10::MAL_MATH_Transform_SeparateMatrix_SoA_Aligned    1401329 ns       500000 ns         1000      140.1329 ns      50.0000 ns 
test11::MAL_MATH_Transform_MatrixTogether_SoA_Aligned    1389399 ns       404531 ns         1545      138.9399 ns      40.4531 ns <-- 1
test12::MAL_MATH_Transform_MatrixTogether_SoA            1400085 ns       474330 ns         2240      140.0085 ns      47.4330 ns 

Compiled with Intel 2024,
            /MP /GS /W4 /Zc:wchar_t /Ox /Ob2 /fp:fast /Zc:forScope /arch:CORE-AVX2 /Oi /MD /std:c++20 /EHsc /nologo /Ot
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations   Time/Transform   CPU/Transform
----------------------------------------------------------------------------------------------------------------------------------
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