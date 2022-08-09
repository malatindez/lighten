#pragma once
#include "../include/d3d.hpp"

#include <functional>
#include <memory>

namespace engine::direct3d
{
    namespace _detail
    {
        template <typename T>
        class ptr_wrapper
        {
        public:
            using type = T;
            explicit constexpr ptr_wrapper(std::shared_ptr<T> ptr) : ptr_ { ptr } {}
            explicit constexpr ptr_wrapper(T *ptr, std::function<void(T *)> const &deleter) : ptr_ { std::shared_ptr<T>(ptr, deleter) } {}

            constexpr operator T const *() const noexcept { return ptr_.get(); }
            constexpr operator T *() noexcept { return ptr_.get(); }
            constexpr T const *operator->() const noexcept { return ptr_.get(); }
            constexpr T *operator->() noexcept { return ptr_.get(); }

            [[nodiscard]] constexpr T const *ptr() const noexcept { return ptr_.get(); }
            [[nodiscard]] constexpr T *ptr() noexcept { return ptr_.get(); }

            [[nodiscard]] constexpr bool valid() { return ptr_ != nullptr; }

            void reset(std::shared_ptr<T> ptr = std::shared_ptr<T> { nullptr }) { ptr_ = ptr; }

        private:
            std::shared_ptr<T> ptr_;

        public:
            template <class... Types>
            constexpr auto operator()(Types &&...args) const
                noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
                -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
            {
                return std::invoke(*ptr_, static_cast<Types &&>(args)...);
            }
        };
    } // namespace

    template <typename T>
    class d3d_resource_wrapper : public _detail::ptr_wrapper<T>
    {
    public:
        constexpr operator T const *() const noexcept { return this->ptr(); }
        constexpr operator T *() noexcept { return this->ptr(); }
        constexpr T const *operator->() const noexcept { return this->ptr(); }
        constexpr T *operator->() noexcept { return this->ptr(); }

        constexpr d3d_resource_wrapper<T> &operator=(T *ptr)
        {
            this->reset(std::shared_ptr<T>(ptr, [] (T *p) { p->Release(); }));
            return *this;
        }

        constexpr d3d_resource_wrapper() : _detail::ptr_wrapper<T> { std::shared_ptr<T>{nullptr} } {}
        explicit constexpr d3d_resource_wrapper(std::shared_ptr<T> ptr) : _detail::ptr_wrapper<T> { ptr } {}
        explicit constexpr d3d_resource_wrapper(T *ptr) : _detail::ptr_wrapper<T> { ptr, [] (T *p)
                                                                                  { p->Release(); } } {}
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
} // namespace engine::direct3d