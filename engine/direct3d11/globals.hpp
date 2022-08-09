#pragma once
#include "common.hpp"
namespace engine::direct3d
{
    // global pointers to most used D3D11 objects for convenience:
    extern Factory factory;
    extern Factory5 factory5;
    extern Device device;
    extern Device5 device5;
    extern DeviceContext devcon;
    extern DeviceContext4 devcon4;
    extern Debug debug;
    void Init();
} // namespace engine::direct3d