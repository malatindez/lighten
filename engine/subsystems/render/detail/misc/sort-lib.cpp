#include "sort-lib.hpp"

#include "subsystems/core/shader-manager.hpp"

namespace engine::render::misc
{
    ComputeShaderProgram SortLib::init_sort_args_compute_shader_;
    ComputeShaderProgram SortLib::sort_step_compute_shader_;
    ComputeShaderProgram SortLib::inner_compute_shader_;
    ComputeShaderProgram SortLib::lds_compute_shader_;

    bool SortLib::shaders_initialized_ = false;

    void SortLib::Init()
    {
        if (!shaders_initialized_)
        {
            auto path = std::filesystem::current_path();
            auto sort_step = core::ShaderManager::instance()->CompileComputeShader(
                {
                    direct3d::ShaderType::ComputeShader,
                    path / kBitonicSortSortStepComputeShaderPath,
                    "BitonicSortStep",
                });
            sort_step_compute_shader_.SetComputeShader(sort_step);

            const std::vector<core::ShaderMacro> sort_size_shader_macro{ core::ShaderMacro{"SORT_SIZE", "512"} };
            auto inner = core::ShaderManager::instance()->CompileComputeShader(
                { direct3d::ShaderType::ComputeShader,
                 path / kBitonicSortInnerComputeShaderPath,
                 "BitonicInnerSort",
                 sort_size_shader_macro });
            inner_compute_shader_.SetComputeShader(inner);

            auto outer = core::ShaderManager::instance()->CompileComputeShader(
                { direct3d::ShaderType::ComputeShader,
                 path / kBitonicSortLDSComputeShaderPath,
                 "BitonicSortLDS",
                 sort_size_shader_macro });

            lds_compute_shader_.SetComputeShader(outer);

            auto init_sort_args = core::ShaderManager::instance()->CompileComputeShader(
                {
                    direct3d::ShaderType::ComputeShader,
                    path / kBitonicInitSortArgsComputeShaderPath,
                    "InitDispatchArgs",
                });

            init_sort_args_compute_shader_.SetComputeShader(init_sort_args);
        }

        D3D11_BUFFER_DESC buffer_desc;
        ZeroMemory(&buffer_desc, sizeof(buffer_desc));
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        buffer_desc.ByteWidth = 4 * sizeof(UINT);
        buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        indirect_sort_args_buffer_.Init(std::move(D3D11_BUFFER_DESC(buffer_desc)));

        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_R32_UINT;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = 4;
        uav_desc.Buffer.Flags = 0;

        direct3d::api().device->CreateUnorderedAccessView(indirect_sort_args_buffer_.buffer(), &uav_desc, &indirect_sort_args_uav_.reset());
    }
    void SortLib::Shutdown()
    {
        indirect_sort_args_uav_.reset();
    }

    void SortLib::Run(uint32_t max_size, direct3d::UnorderedAccessView sort_buffer_uav, direct3d::Buffer item_count_buffer)
    {
        direct3d::UnorderedAccessView prevUAV;
        direct3d::api().devcon4->CSGetUnorderedAccessViews(0, 1, &prevUAV.ptr());

        ID3D11Buffer *prevCBs[] = { nullptr, nullptr };
        direct3d::api().devcon4->CSGetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

        ID3D11Buffer *cbs[] = { item_count_buffer, pcb_dispatch_info_.buffer() };
        direct3d::api().devcon4->CSSetConstantBuffers(0, ARRAYSIZE(cbs), cbs);

        direct3d::api().devcon4->CSSetUnorderedAccessViews(0, 1, &indirect_sort_args_uav_.ptr(), nullptr);

        init_sort_args_compute_shader_.Bind();
        direct3d::api().devcon4->Dispatch(1, 1, 1);

        direct3d::api().devcon4->CSSetUnorderedAccessViews(0, 1, &sort_buffer_uav.ptr(), nullptr);

        bool bDone = SortInitial(max_size);
        int presorted = 512;
        while (!bDone)
        {
            bDone = SortIncremental(presorted, max_size);
            presorted *= 2;
        }

#ifdef _DEBUG
        DebugCheck(max_size, prevUAV);
#endif

        direct3d::api().devcon4->CSSetUnorderedAccessViews(0, 1, &prevUAV.ptr(), nullptr);
        direct3d::api().devcon4->CSSetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

        if (prevUAV)
        {
            prevUAV->Release();
        }

        for (size_t i = 0; i < ARRAYSIZE(prevCBs); i++)
        {
            if (prevCBs[i])
            {
                prevCBs[i]->Release();
            }
        }
    }

    const int kMaxNumTG = 1024; // 128; // max 128 * 512 elements = 64k elements

    bool SortLib::SortInitial(uint32_t max_size)
    {
        bool bDone = true;

        // calculate how many threads we'll require:
        //   we'll sort 512 elements per CU (threadgroupsize 256)
        //     maybe need to optimize this or make it changeable during init
        //     TGS=256 is a good intermediate value

        unsigned int numThreadGroups = ((max_size - 1) >> 9) + 1;
        utils::Assert(numThreadGroups <= kMaxNumTG, "Too many thread groups: " + std::to_string(numThreadGroups) + "! Max is " + std::to_string(kMaxNumTG));
        // To disable std::to_string on release builds
        utils::AlwaysAssert(numThreadGroups <= kMaxNumTG);

        if (numThreadGroups > 1)
            bDone = false;

        // sort all buffers of size 512 (and presort bigger ones)
        lds_compute_shader_.Bind();
        direct3d::api().devcon4->DispatchIndirect(indirect_sort_args_buffer_.buffer(), 0);
        return bDone;
    }

    bool SortLib::SortIncremental(uint32_t presorted, uint32_t max_size)
    {
        bool bDone = true;
        sort_step_compute_shader_.Bind();

        // prepare thread group description data
        unsigned int numThreadGroups = 0;

        if (max_size > presorted)
        {
            if (max_size > presorted * 2)
                bDone = false;

            unsigned int pow2 = presorted;
            while (pow2 < max_size)
                pow2 *= 2;
            numThreadGroups = pow2 >> 9;
        }

        unsigned int nMergeSize = presorted * 2;
        for (unsigned int nMergeSubSize = nMergeSize >> 1; nMergeSubSize > 256; nMergeSubSize = nMergeSubSize >> 1)
            //	for( int nMergeSubSize=nMergeSize>>1; nMergeSubSize>0; nMergeSubSize=nMergeSubSize>>1 )
        {
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            direct3d::api().devcon4->Map(pcb_dispatch_info_.buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            core::math::ivec4 *sc = (core::math::ivec4 *)mapped_resource.pData;
            sc->x = nMergeSubSize;
            if (nMergeSubSize == nMergeSize >> 1)
            {
                sc->y = (2 * nMergeSubSize - 1);
                sc->z = -1;
            }
            else
            {
                sc->y = nMergeSubSize;
                sc->z = 1;
            }
            sc->w = 0;
            direct3d::api().devcon4->Unmap(pcb_dispatch_info_.buffer(), 0);

            direct3d::api().devcon4->Dispatch(numThreadGroups, 1, 1);
        }

        inner_compute_shader_.Bind();
        direct3d::api().devcon4->Dispatch(numThreadGroups, 1, 1);

        return bDone;
    }
#ifdef _DEBUG
    void SortLib::DebugCheck(uint32_t size, ID3D11UnorderedAccessView *pUAV)
    {
        ID3D11Resource *srcResource;
        pUAV->GetResource(&srcResource);
        ID3D11Buffer *srcBuffer;
        srcResource->QueryInterface(IID_ID3D11Buffer, (void **)&srcBuffer);

        D3D11_BUFFER_DESC srcDesc;
        srcBuffer->GetDesc(&srcDesc);

        // create a readbackbuffer for manual verification of correctness
        ID3D11Buffer *readBackBuffer;
        D3D11_BUFFER_DESC bDesc = srcDesc;
        bDesc.Usage = D3D11_USAGE_STAGING;
        bDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
        bDesc.BindFlags = 0;
        direct3d::api().device->CreateBuffer(&bDesc, NULL, &readBackBuffer);

        // Download the data
        D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
        direct3d::api().devcon->CopyResource(readBackBuffer, srcResource);
        direct3d::api().devcon->Map(readBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource);

        bool correct = true;
        typedef struct
        {
            float sortval;
            float data;
        } BufferData;

        BufferData *b = &((BufferData *)(MappedResource.pData))[0];
        for (unsigned int i = 1; i < size; ++i)
        {
            correct &= b[i - 1].sortval <= b[i].sortval;
            if (b[i - 1].sortval > b[i].sortval)
            {
                int _abc = 0;
                (void)_abc;
            }
        }

        direct3d::api().devcon4->Unmap(readBackBuffer, 0);

        srcResource->Release();
        readBackBuffer->Release();
    }
#endif
}