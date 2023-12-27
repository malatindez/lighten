#pragma once
#include "buffer-wrapper.hpp"
namespace engine::direct3d
{
    namespace _detail
    {
        /**
         * @brief Create a Basic Buffer Desc object
         * 
         * @note This function is constexpr and can be used in compile time
         * This is done to avoid the need to create a D3D11_BUFFER_DESC object in runtime and to avoid the need to create a temporary object
         * 
         * @param bind_flags Identify how the buffer will be bound to the pipeline. Flags (see D3D11_BIND_FLAG) can be combined with a bitwise OR.
         * @param cpu_access_flags CPU access flags (see D3D11_CPU_ACCESS_FLAG) or 0 if no CPU access is necessary. Flags can be combined with a bitwise OR.
         * @param misc_flags Miscellaneous flags (see D3D11_RESOURCE_MISC_FLAG) or 0 if unused. Flags can be combined with a bitwise OR.
         * @param width Size of the buffer in bytes.
         * @param stride The size of each element in the buffer structure (in bytes) when the buffer represents a structured buffer. For more info about structured buffers, see Structured Buffer.
         * The size value in StructureByteStride must match the size of the format that you use for views of the buffer. For example, if you use a shader resource view (SRV) to read a buffer in a pixel shader, the SRV format size must match the size value in StructureByteStride.
         * @return constexpr D3D11_BUFFER_DESC 
         */
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
    /**
     * @brief Buffer Base class that contains the common functions for all the buffers
     * 
     * @details This class is used to avoid code duplication and to make the code more readable
     * 
     * @tparam T The type of underlying data
     */
    template <typename T>
    class BufferBase : private BufferWrapper
    {
    public:
        /// @brief The usage type of the buffer 
        /// @details This is used to make sure that the buffer is initialized with the correct usage type
        /// @note This is a constant value and can only be initialized as the object is constructed
        const D3D11_USAGE kUsage;
        using BufferWrapper::Map;
        using BufferWrapper::Unmap;
        using BufferWrapper::buffer;
        using BufferWrapper::description;
        /// @brief Get the size of the buffer
        /// @warning This function is only valid for buffers that are initialized with a span or a single value
        /// @warning If the buffer was not initialized or was deinitialized, the return value is undefined
        /// @return uint32_t The size of the buffer
        [[nodiscard]] inline uint32_t size() { return description_.ByteWidth / sizeof(T); }

    protected:
        /// @brief Construct a new Buffer Base object
        /// @param usage The usage type of the buffer
        BufferBase(D3D11_USAGE usage) : kUsage{ usage } { }
        /// @brief Initialize the buffer
        /// @param desc The description of the buffer
        /// @param span The span of the data to initialize the buffer with
        inline void Init(D3D11_BUFFER_DESC &&desc, std::span<const T> const &span)
        {
            Init(std::move(desc), (void *)span.begin()._Myptr, static_cast<uint32_t>(sizeof(T) * span.size()));
        }
        /// @brief Initialize the buffer
        /// @param desc The description of the buffer
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param width The size of the data to initialize the buffer with, in bytes
        inline void Init(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t width)
        {
            mal_toolkit::Assert(desc.Usage == kUsage);
            desc.ByteWidth = width;
            D3D11_SUBRESOURCE_DATA data;
            data.pSysMem = ptr;
            BufferWrapper::Init(std::move(desc), data);
        }
        /// @brief Initialize the buffer
        /// @param desc The description of the buffer
        inline void Init(D3D11_BUFFER_DESC &&desc)
        {
            mal_toolkit::Assert(desc.Usage == kUsage);
            BufferWrapper::Init(std::move(desc));
        }
        /// @brief Initialize the buffer
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param width The size of the data to initialize the buffer with, in bytes
        inline void Init(void *ptr, uint32_t width)
        {
            Init(D3D11_BUFFER_DESC(description()), ptr, width);
        }
        /// @brief Initialize the buffer
        /// @param span The span of the data to initialize the buffer with
        /// @note This function will calll Init(D3D11_BUFFER_DESC &&desc, std::span<const T> const &span) with the current description of the buffer(the bytewidth will be set to the byte size of the span)
        inline void Init(std::span<const T> const &span)
        {
            Init(D3D11_BUFFER_DESC(description()), span);
        }
        /// @brief Initialize the buffer
        /// @param width The size of the buffer in elements
        inline void Init(uint32_t width)
        {
            D3D11_BUFFER_DESC temp = description();
            temp.ByteWidth = width * sizeof(T);
            Init(std::move(temp));
        }
        /// @brief Initialize the buffer
        /// @param value The single value to initialize the buffer with
        inline void Init(T const &value)
        {
            Init(&value, sizeof(T));
        } 
        using BufferWrapper::description_;
    };
}