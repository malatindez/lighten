#pragma once
#include "../api.hpp"
#include "../d3d-debug.hpp"
namespace lighten::direct3d
{
    /**
     * @brief A wrapper for a D3D11 buffer.
     * @details This class is used to wrap a D3D11 buffer and provide a common interface for all buffers.
     * @todo IMPORTANT! Add `Deinit()` method and update systems so if there's none instances, the buffer is deinitialized. Right now systems keep the buffer alive even if there's no instances. This is not a memory leak, but it's a waste of memory.
     * This is a base class for all buffers located in direct3d11/buffers
     */
    class BufferWrapper
    {
    public:
        /// @brief Returns the const & to underlying ID3D11Buffer const *
        [[nodiscard]] inline ID3D11Buffer const *const &buffer() const { return buffer_.ptr(); }
        /// @brief Returns the const & to underlying ID3D11Buffer *
        [[nodiscard]] inline ID3D11Buffer *const &buffer() { return buffer_.ptr(); }
        /// @brief Returns the const & to underlying Buffer const &
        [[nodiscard]] inline D3D11_BUFFER_DESC const &description() const { return description_; }
        /// @brief Maps the buffer to a D3D11_MAPPED_SUBRESOURCE
        /// @param map_type Map type
        /// Can be one of:
        /// * D3D11_MAP_READ
        /// * D3D11_MAP_WRITE
        /// * D3D11_MAP_READ_WRITE
        /// * D3D11_MAP_WRITE_DISCARD
        /// * D3D11_MAP_WRITE_NO_OVERWRITE
        /// @param map_flags Map flag
        /// The only available flag is D3D11_MAP_FLAG_DO_NOT_WAIT
        /// @param subresource index number of the subresource
        /// @return D3D11_MAPPED_SUBRESOURCE that provides access to subresource data
        inline D3D11_MAPPED_SUBRESOURCE Map(D3D11_MAP map_type, uint32_t map_flags, uint32_t subresource = 0)
        {
            D3D11_MAPPED_SUBRESOURCE rv;
            direct3d::AlwaysAssert(direct3d::api().devcon4->Map(buffer_.ptr(), subresource, map_type, map_flags, &rv),
                                   "Failed to map resource");
            return rv;
        }
        /// @brief Unmaps the mapped buffer
        /// @param subresource index number of the subresource
        inline void Unmap(uint32_t subresource = 0)
        {
            direct3d::api().devcon4->Unmap(buffer_.ptr(), subresource);
        }

    protected:
        /**
         * @brief Initializes the buffer
         *
         * @param desc Buffer description
         * @param initial_data Initial data
         */
        inline void Init(D3D11_BUFFER_DESC &&desc, D3D11_SUBRESOURCE_DATA &initial_data)
        {
            direct3d::AlwaysAssert(direct3d::api().device5->CreateBuffer(&desc, &initial_data, &buffer_.reset()),
                                   "Failed to create buffer");
            description_ = desc;
        }
        /**
         * @brief Initializes the buffer
         *
         * @param desc Buffer description
         */
        inline void Init(D3D11_BUFFER_DESC &&desc)
        {
            direct3d::AlwaysAssert(direct3d::api().device5->CreateBuffer(&desc, nullptr, &buffer_.reset()),
                                   "Failed to create buffer");
            description_ = desc;
        }
#pragma warning(push)
#pragma warning(disable : 26495)
        /**
         * @brief Buffer description
         *
         */
        D3D11_BUFFER_DESC description_{
            0, D3D11_USAGE_DEFAULT, 0, 0, 0, 0};
#pragma warning(pop)
    private:
        /// @brief Underlying buffer
        Buffer buffer_;
    };
} // namespace lighten::direct3d