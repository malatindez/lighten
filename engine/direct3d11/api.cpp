#include "api.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
#include "mal-toolkit/win-utils.hpp"
#include "d3d-debug.hpp"
#include "states.hpp"
#include "core/config.hpp"
extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace lighten::direct3d
{
    std::unique_ptr<Api> Api::instance_ = nullptr;
    void Api::Init()
    {
        instance_ = std::unique_ptr<Api>(new Api());
        AlwaysAssert(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&instance_->factory.reset()), "Failed to create DXGI factory");
        AlwaysAssert(instance_->factory->QueryInterface(__uuidof(IDXGIFactory5), (void **)(&instance_->factory5.reset())), " Failed to create IDXGI factory 5");
        {
            uint32_t index = 0;
            IDXGIAdapter1 *adapter;
            while (instance_->factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                std::wstring temp = desc.Description;
                std::stringstream output;
                output << "GPU #";
                output << index << " ";
                output << mal_toolkit::wstring_to_string(temp);
                spdlog::info(output.str());
            }
        }

        // Init D3D Device & Context

        const D3D_FEATURE_LEVEL kFeatureLevelRequested = D3D_FEATURE_LEVEL_11_1;
        D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_1;
#if !defined(_DEBUG)
        UINT flag = D3D11_CREATE_DEVICE_SINGLETHREADED;
#else
        UINT flag = D3D11_CREATE_DEVICE_DEBUG;
        flag |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif
        flag |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;
        AlwaysAssert(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag,
                                       &kFeatureLevelRequested, 1, D3D11_SDK_VERSION, &instance_->device.reset(), &featureLevelInitialized, &instance_->devcon.reset()),
                     "Failed to create device and devcon");

        mal_toolkit::AlwaysAssert(kFeatureLevelRequested == featureLevelInitialized, "featureLevelInitialized != D3D_FEATURE_LEVEL_11_0");

        AlwaysAssert(instance_->device->QueryInterface(__uuidof(ID3D11Device5), (void **)&instance_->device5.reset()),
                     "Failed to query ID3D11Device5");

        AlwaysAssert(instance_->devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void **)&instance_->devcon4.reset()),
                     "Failed to query ID3D11DeviceContext4");
#if defined(_DEBUG)
        AlwaysAssert(instance_->device->QueryInterface(__uuidof(ID3D11Debug), (void **)&instance_->debug.reset()),
                     "Failed to query ID3D11Debug");

        AlwaysAssert(instance_->device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&instance_->debug_info_queue.reset()),
                     "Failed to query ID3D11InfoQueue");
#endif
        States::Init();
    }
    void Api::Deinit()
    {
        States::Deinit();
#if defined(_DEBUG)
        if (core::config()["Debug"]["show_direct3d_debug_output"].as_boolean())
        {
            api().debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
            api().debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
            LogDebugInfoQueue();
            api().debug = nullptr;
            api().debug_info_queue = nullptr;
        }
#endif
    }
} // namespace lighten::direct3d