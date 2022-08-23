#pragma once
#include "buffer-wrapper.hpp"
namespace engine::direct3d
{
    namespace _detail
    {
        inline D3D11_BUFFER_DESC CreateBasicBufferDesc(uint32_t bind_flags, uint32_t cpu_access_flags, uint32_t misc_flags)
        {
            D3D11_BUFFER_DESC rv;
            rv.BindFlags = bind_flags;
            rv.CPUAccessFlags = cpu_access_flags;
            rv.MiscFlags = misc_flags;
            return rv;
        }
    }
    template <typename T>
    class BufferBase : public BufferWrapper
    {
    public:
        D3D11_USAGE kUsage;
    protected:

        BufferBase(D3D11_USAGE usage) : kUsage { usage } {}
        inline void Init(D3D11_BUFFER_DESC &&desc, std::span<const T> const &span)
        {
            Init(std::move(desc), (void *)span.begin()._Myptr, static_cast<uint32_t>(sizeof(T) * span.size()));
        }

        inline void Init(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t width)
        {
            utils::Assert(desc.Usage == kUsage);
            desc.ByteWidth = width;
            D3D11_SUBRESOURCE_DATA data;
            data.pSysMem = ptr;
            Init(std::move(desc), data);
        }
        inline void Init(D3D11_BUFFER_DESC &&desc)
        {
            utils::Assert(desc.Usage == kUsage);
            Init(std::move(desc));
        }
    private: // Hide base Init functions
        using BufferWrapper::Init;
    };
}