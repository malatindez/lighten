#pragma once
#include "../include/d3d.hpp"

#include <functional>
#include <memory>

namespace engine::direct3d
{
    template <typename T>
    class d3d_resource_wrapper
    {
    public:
        using Type = T;
        constexpr operator T const *() const noexcept { return ptr_; }
        constexpr operator T *() noexcept { return ptr_; }
        constexpr T const *operator->() const noexcept { return ptr_; }
        constexpr T *operator->() noexcept { return ptr_; }

        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return ptr_; }
        [[nodiscard]] constexpr T *&ptr() noexcept { return ptr_; }

        [[nodiscard]] constexpr bool valid() { return ptr_ != nullptr; }

        inline void reset(T *ptr = nullptr)
        {
            int ref_amount = 0;
            if (ptr_)
            {
                ref_amount = ptr_->Release();
            }
            ptr_ = ptr;
            if (ptr_)
            {
                ptr_->AddRef();
            }
        }

        constexpr d3d_resource_wrapper<T> &operator=(T *ptr)
        {
            this->reset(ptr);
            ptr->Release();
            return *this;
        }

        ~d3d_resource_wrapper()
        {
            ptr_->Release();
        }

        d3d_resource_wrapper(d3d_resource_wrapper<T> &&other)
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper(d3d_resource_wrapper<T> const &other)
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> &&other)
        {
            reset(other.ptr_);
            return *this;
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> const &other)
        {
            reset(other.ptr_);
            return *this;
        }

        constexpr d3d_resource_wrapper() = default;
        explicit constexpr d3d_resource_wrapper(T *ptr)
        {
            reset(ptr);
            ptr->Release();
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

    using Factory = d3d_resource_wrapper<IDXGIFactory>;
    using Factory5 = d3d_resource_wrapper<IDXGIFactory5>;
    using Device = d3d_resource_wrapper<ID3D11Device>;
    using Device5 = d3d_resource_wrapper<ID3D11Device5>;
    using DeviceContext = d3d_resource_wrapper<ID3D11DeviceContext>;
    using DeviceContext4 = d3d_resource_wrapper<ID3D11DeviceContext4>;
    using Debug = d3d_resource_wrapper<ID3D11Debug>;
    using SwapChain1 = d3d_resource_wrapper<IDXGISwapChain1>;
    using RenderTargetView1 = d3d_resource_wrapper<ID3D11RenderTargetView1>;
    using RenderTargetView = d3d_resource_wrapper<ID3D11RenderTargetView>;
    using DepthStencilView = d3d_resource_wrapper<ID3D11DepthStencilView>;
    using Texture2D = d3d_resource_wrapper<ID3D11Texture2D>;
    using Buffer = d3d_resource_wrapper<ID3D11Buffer>;
    using GeometryShader = d3d_resource_wrapper<ID3D11GeometryShader>;
    using VertexShader = d3d_resource_wrapper<ID3D11VertexShader>;
    using PixelShader = d3d_resource_wrapper<ID3D11PixelShader>;
    using InputLayout = d3d_resource_wrapper<ID3D11InputLayout>;
    using RasterizerState1 = d3d_resource_wrapper<ID3D11RasterizerState1>;
    using SamplerState = d3d_resource_wrapper<ID3D11SamplerState>;
    using DepthStencilState = d3d_resource_wrapper<ID3D11DepthStencilState>;
    using Resource = d3d_resource_wrapper<ID3D11Resource>;
} // namespace engine::direct3d