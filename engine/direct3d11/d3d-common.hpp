#pragma once
#include "include/d3d.hpp"
#include "include/library-pch.hpp"

namespace engine::direct3d
{
    template <typename T>
    class d3d_resource_wrapper
    {
    public:
        using type = T;
        constexpr operator T const *() const noexcept { return ptr_; }
        constexpr operator T *() noexcept { return ptr_; }
        constexpr T const *operator->() const noexcept { return ptr_; }
        constexpr T *operator->() noexcept { return ptr_; }

        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return ptr_; }
        [[nodiscard]] constexpr T *&ptr() noexcept { return ptr_; }

        [[nodiscard]] constexpr bool valid() { return ptr_ != nullptr; }

        inline T *&reset(T *ptr = nullptr)
        {
            if (ptr_)
            {
                ptr_->Release();
            }
            ptr_ = ptr;
            if (ptr_)
            {
                ptr_->AddRef();
            }
            return ptr_;
        }

        constexpr d3d_resource_wrapper<T> &operator=(T *ptr)
        {
            this->reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
            return *this;
        }

        ~d3d_resource_wrapper()
        {
            reset();
        }

        d3d_resource_wrapper(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }

        constexpr d3d_resource_wrapper() = default;
        constexpr d3d_resource_wrapper(T *ptr)
        {
            reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
        }

    private:
        T *ptr_ = nullptr;

    public:
        template <class... Types>
        constexpr auto operator()(Types &&...args) const
            noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
            -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
        {
            return std::invoke(*ptr_, static_cast<Types &&>(args)...);
        }
    };

    enum class ShaderType : uint8_t
    {
        VertexShader = 0,
        PixelShader = 1,
        HullShader = 2,
        DomainShader = 3,
        GeometryShader = 4,
        ComputeShader = 5,
        None = 6
    };

    using SwapChain1 = d3d_resource_wrapper<IDXGISwapChain1>;
    using RenderTargetView1 = d3d_resource_wrapper<ID3D11RenderTargetView1>;
    using RenderTargetView = d3d_resource_wrapper<ID3D11RenderTargetView>;
    using DepthStencilView = d3d_resource_wrapper<ID3D11DepthStencilView>;
    using Texture2D = d3d_resource_wrapper<ID3D11Texture2D>;
    using Buffer = d3d_resource_wrapper<ID3D11Buffer>;
    using InputLayout = d3d_resource_wrapper<ID3D11InputLayout>;
    using RasterizerState1 = d3d_resource_wrapper<ID3D11RasterizerState1>;
    using SamplerState = d3d_resource_wrapper<ID3D11SamplerState>;
    using DepthStencilState = d3d_resource_wrapper<ID3D11DepthStencilState>;
    using Resource = d3d_resource_wrapper<ID3D11Resource>;
    using VertexShader = d3d_resource_wrapper<ID3D11VertexShader>;
    using PixelShader = d3d_resource_wrapper<ID3D11PixelShader>;
    using HullShader = d3d_resource_wrapper<ID3D11HullShader>;
    using DomainShader = d3d_resource_wrapper<ID3D11DomainShader>;
    using GeometryShader = d3d_resource_wrapper<ID3D11GeometryShader>;
    using ComputeShader = d3d_resource_wrapper<ID3D11ComputeShader>;
    using Blob = d3d_resource_wrapper<ID3D10Blob>;
    using Buffer = d3d_resource_wrapper<ID3D11Buffer>;
    using ShaderResourceView = d3d_resource_wrapper<ID3D11ShaderResourceView>;
    using Resource = d3d_resource_wrapper<ID3D11Resource>;
} // namespace engine::direct3d