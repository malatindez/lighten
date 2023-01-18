#pragma once
#include "../api.hpp"
#include "../d3d-debug.hpp"
namespace engine::direct3d
{
    class BufferWrapper
    {
    public:
        [[nodiscard]] inline ID3D11Buffer const *const &buffer() const { return buffer_.ptr(); }
        [[nodiscard]] inline ID3D11Buffer *const &buffer() { return buffer_.ptr(); }
        [[nodiscard]] inline D3D11_BUFFER_DESC const &description() const { return description_; }
        inline D3D11_MAPPED_SUBRESOURCE Map(D3D11_MAP map_type, uint32_t map_flags, uint32_t subresource = 0)
        {
            D3D11_MAPPED_SUBRESOURCE rv;
            direct3d::AlwaysAssert(direct3d::api().devcon4->Map(buffer_.ptr(), subresource, map_type, map_flags, &rv),
                                   "Failed to map resource");
            return rv;
        }
        inline void Unmap(uint32_t subresource = 0)
        {
            direct3d::api().devcon4->Unmap(buffer_.ptr(), subresource);
        }
    protected:
        inline void Init(D3D11_BUFFER_DESC &&desc, D3D11_SUBRESOURCE_DATA &initial_data)
        {
            direct3d::AlwaysAssert(direct3d::api().device5->CreateBuffer(&desc, &initial_data, &buffer_.reset()),
                                   "Failed to create buffer");
            description_ = desc;
        }
        inline void Init(D3D11_BUFFER_DESC &&desc)
        {
            direct3d::AlwaysAssert(direct3d::api().device5->CreateBuffer(&desc, nullptr, &buffer_.reset()),
                                   "Failed to create buffer");
            description_ = desc;
        }
#pragma warning(push)
#pragma warning(disable:26495)
        D3D11_BUFFER_DESC description_
        {
            0, D3D11_USAGE_DEFAULT, 0, 0, 0, 0
        };
#pragma warning(pop)
    private:
        Buffer buffer_;
    };
} // namespace engine::direct3d