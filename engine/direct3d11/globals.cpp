#include "globals.hpp"
#include "core/application.hpp"
#include <assert.h>
namespace engine::direct3d
{
    // global pointers to most used D3D11 objects for convenience:
    Factory factory;
    Factory5 factory5;
    Device device;
    Device5 device5;
    DeviceContext devcon;
    DeviceContext4 devcon4;
    Debug debug;

    void Init()
    {
        HRESULT result;

        IDXGIFactory *f = nullptr;
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **) (&f));
        factory = Factory { f };

        assert(result >= 0 && "CreateDXGIFactory");

        IDXGIFactory5 *f5 = nullptr;
        result = factory->QueryInterface(__uuidof(IDXGIFactory5), (void **) (&f5));
        factory5 = Factory5 { f5 };
        assert(result >= 0 && "Query IDXGIFactory5");
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
                output << std::string(temp.begin(), temp.end());
                core::Application::logger().info(output.str());
            }
        }

        // Init D3D Device & Context

        const D3D_FEATURE_LEVEL kFeatureLevelRequested = D3D_FEATURE_LEVEL_11_0;
        D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
        ID3D11Device *dvc = nullptr;
        ID3D11DeviceContext *dvccn = nullptr;
        result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
                                   &kFeatureLevelRequested, 1, D3D11_SDK_VERSION, &dvc, &featureLevelInitialized, &dvccn);
        assert(result >= 0 && "D3D11CreateDevice");
        assert(kFeatureLevelRequested == featureLevelInitialized && "D3D_FEATURE_LEVEL_11_0");
        device = Device { dvc };
        devcon = DeviceContext { dvccn };

        ID3D11Device5 *dvc5 = nullptr;
        result = device->QueryInterface(__uuidof(ID3D11Device5), (void **) &dvc5);
        assert(result >= 0 && "Query ID3D11Device5");
        device5 = Device5 { dvc5 };

        ID3D11DeviceContext4 *dvccn4 = nullptr;
        result = devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void **) &dvccn4);
        assert(result >= 0 && "Query ID3D11DeviceContext4");
        devcon4 = DeviceContext4 { dvccn4 };

        ID3D11Debug *dbg = nullptr;
        result = device->QueryInterface(__uuidof(ID3D11Debug), (void **) &dbg);
        assert(result >= 0 && "Query ID3D11Debug");
        debug = Debug { dbg };
    }
} // namespace engine::direct3d