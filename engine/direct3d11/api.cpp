#include "api.hpp"
#include "utils/utils.hpp"
#include "utils/win-utils.hpp"
#include "d3d-debug.hpp"

extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine::direct3d
{
    // global pointers to most used D3D11 objects for convenience:
    Factory api::factory;
    Factory5 api::factory5;
    Device api::device;
    Device5 api::device5;
    DeviceContext api::devcon;
    DeviceContext4 api::devcon4;
#if defined(_DEBUG)
    Debug api::debug;
    DebugInfoQueue api::debug_info_queue;
#endif

    void api::Init()
    {
        AlwaysAssert(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory.reset()), "Failed to create DXGI factory");
        AlwaysAssert(factory->QueryInterface(__uuidof(IDXGIFactory5), (void **)(&factory5.reset())), " Failed to create IDXGI factory 5");
        {
            uint32_t index = 0;
            IDXGIAdapter1 *adapter;
            while (factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                std::wstring temp = desc.Description;
                std::stringstream output;
                output << "GPU #";
                output << index << " ";
                output << utils::wstring_to_string(temp);
                spdlog::info(output.str());
            }
        }

        // Init D3D Device & Context

        const D3D_FEATURE_LEVEL kFeatureLevelRequested = D3D_FEATURE_LEVEL_11_0;
        D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
#if !defined(_DEBUG)
        D3D11_CREATE_DEVICE_FLAG flag = D3D11_CREATE_DEVICE_SINGLETHREADED;
#else
        D3D11_CREATE_DEVICE_FLAG flag = D3D11_CREATE_DEVICE_DEBUG;
#endif
        AlwaysAssert(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag,
                                       &kFeatureLevelRequested, 1, D3D11_SDK_VERSION, &device.reset(), &featureLevelInitialized, &devcon.reset()),
                     "Failed to create device and devcon");

        utils::AlwaysAssert(kFeatureLevelRequested == featureLevelInitialized, "featureLevelInitialized != D3D_FEATURE_LEVEL_11_0");

        AlwaysAssert(device->QueryInterface(__uuidof(ID3D11Device5), (void **)&device5.reset()),
                     "Failed to query ID3D11Device5");

        AlwaysAssert(devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void **)&devcon4.reset()),
                     "Failed to query ID3D11DeviceContext4");
#if defined(_DEBUG)
        AlwaysAssert(device->QueryInterface(__uuidof(ID3D11Debug), (void **)&debug.reset()),
                     "Failed to query ID3D11Debug");

        AlwaysAssert(device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&debug_info_queue.reset()),
                     "Failed to query ID3D11InfoQueue");
#endif
    }
    void api::Deinit()
    {
        devcon4 = nullptr;
        device5 = nullptr;
        factory5 = nullptr;
        factory = nullptr;
        devcon->ClearState();
        devcon->Flush();
        devcon = nullptr;
        device = nullptr;
#if defined(_DEBUG)
        debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
        LogDebugInfoQueue();
        debug = nullptr;
        debug_info_queue = nullptr;
#endif
    }
} // namespace engine::direct3d