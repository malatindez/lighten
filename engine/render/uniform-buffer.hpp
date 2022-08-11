#pragma once
#include "shader-program.hpp"
#include "utils/utils.hpp"
namespace engine::render
{
    template <typename... Args>
    class UniformBuffer
    {
        static constexpr auto RoundBufferSizeTo16Boundary(size_t x) { return x + 0xf & 0xfffffff0; };

    public:
        static constexpr size_t kSize = RoundBufferSizeTo16Boundary(utils::parameter_pack_info<Args...>::size);
        UniformBuffer(bool dynamic = true);
        UniformBuffer(Args const &... initial_data, bool dynamic = true);

        void Update(void const *data, uint32_t data_size);
        void Update(Args const &...args);

        void Bind(ShaderType type, uint32_t slot);

        direct3d::Buffer buffer() const noexcept { return buffer_; }

    private:
        direct3d::Buffer buffer_ = nullptr;
        bool kDynamic;
    };
}
#include "uniform-buffer.inl"