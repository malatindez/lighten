#pragma once
#include "buffer-wrapper.hpp"
namespace engine::direct3d
{
    namespace _detail
    {
        constexpr D3D11_BUFFER_DESC CreateBasicBufferDesc(uint32_t bind_flags, uint32_t cpu_access_flags, uint32_t misc_flags, uint32_t width = 0, uint32_t stride = 0)
        {
            D3D11_BUFFER_DESC rv;
            rv.BindFlags = bind_flags;
            rv.CPUAccessFlags = cpu_access_flags;
            rv.MiscFlags = misc_flags;
            rv.ByteWidth = width;
            rv.StructureByteStride = stride;
            return rv;
        }
    }
    template <typename T>
    class BufferBase : private BufferWrapper
    {
    public:
        D3D11_USAGE kUsage;
        using BufferWrapper::Map;
        using BufferWrapper::Unmap;
        using BufferWrapper::buffer;
        using BufferWrapper::description;
        [[nodiscard]] inline uint32_t size() { return description_.ByteWidth / sizeof(T); }

    protected:
        BufferBase(D3D11_USAGE usage) : kUsage{ usage } { }
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
            BufferWrapper::Init(std::move(desc), data);
        }
        inline void Init(D3D11_BUFFER_DESC &&desc)
        {
            utils::Assert(desc.Usage == kUsage);
            BufferWrapper::Init(std::move(desc));
        }
        inline void Init(void *ptr, uint32_t width)
        {
            Init(description(), ptr, width);
        }
        inline void Init(std::span<const T> const &span)
        {
            Init(description(), span);
        }
        inline void Init(uint32_t width)
        {
            D3D11_BUFFER_DESC temp = description();
            temp.ByteWidth = width * sizeof(T);
            Init(std::move(temp));
        }

        inline void Init(T const &value)
        {
            Init(&value, sizeof(T));
        }
        using BufferWrapper::description_;
    };
}