#pragma once
#include "include/library-pch.hpp"
#include "direct3d11/direct3d11.hpp"
#include "render/shader-program.hpp"

namespace engine::render::misc
{
    class SortLib
    {
    public:
        SortLib() = default;
        SortLib(SortLib const &) = delete;
        SortLib(SortLib &&) = delete;
        SortLib &operator=(SortLib const &) = delete;
        SortLib &operator=(SortLib &&) = delete;
        ~SortLib() = default;

        void Init();
        void Shutdown();
        void Run(uint32_t max_size, direct3d::UnorderedAccessView sort_buffer_uav, direct3d::Buffer item_count_buffer);
    private:

        bool SortInitial(uint32_t max_size);
        bool SortIncremental(uint32_t presorted, uint32_t max_size);
#ifdef _DEBUG
        void DebugCheck(uint32_t size, ID3D11UnorderedAccessView *pUAV);
#endif

        direct3d::DynamicUniformBuffer<core::math::ivec4> pcb_dispatch_info_;

        direct3d::DefaultBuffer<core::math::uivec4> indirect_sort_args_buffer_;
        direct3d::UnorderedAccessView indirect_sort_args_uav_;
        // InitSortArgsCS.hlsl
        // m_pCSInitArgs
        static ComputeShaderProgram init_sort_args_compute_shader_;
        // SortStepCS2.hlsl
        // m_pCSSortStep
        static ComputeShaderProgram sort_step_compute_shader_;
        // SortinnerCS.hlsl
        // m_pCSSortInner512
        static ComputeShaderProgram inner_compute_shader_;
        // SortCS.hlsl
        // m_pCSSort512
        static ComputeShaderProgram lds_compute_shader_;

        static bool shaders_initialized_;

        static auto constexpr kBitonicSortSortStepComputeShaderPath = "assets/shaders/misc/sort/bitonic-32-sort-step-cs.hlsl";
        static auto constexpr kBitonicSortInnerComputeShaderPath = "assets/shaders/misc/sort/bitonic-32-inner-cs.hlsl";
        static auto constexpr kBitonicSortLDSComputeShaderPath = "assets/shaders/misc/sort/bitonic-32-LDS-cs.hlsl";
        static auto constexpr kBitonicInitSortArgsComputeShaderPath = "assets/shaders/misc/sort/bitonic-32-init-sort-args-cs.hlsl";
    };
}