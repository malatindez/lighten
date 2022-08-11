#pragma once
#include "d3d-common.hpp"
namespace engine::core
{
    class Engine;
}
namespace engine::direct3d
{
    class api;

    namespace _detail
    {
        // class protected from accidental pointer overwriting
        template<typename T>
        class global_d3d_resource_wrapper : protected d3d_resource_wrapper<T>
        {
        public:
            constexpr operator T const *() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
            constexpr operator T *() noexcept { return d3d_resource_wrapper<T>::ptr(); }
            constexpr T const *operator->() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
            constexpr T *operator->() noexcept { return d3d_resource_wrapper<T>::ptr(); }

            [[nodiscard]] constexpr T const *const &ptr() const noexcept { return d3d_resource_wrapper<T>::ptr(); }

            using d3d_resource_wrapper<T>::valid;
            using d3d_resource_wrapper<T>::operator();

        private:
            friend class engine::direct3d::api;

            using d3d_resource_wrapper<T>::d3d_resource_wrapper;
            using d3d_resource_wrapper<T>::operator=;
        };
    } // namespace _detail

    using Factory = _detail::global_d3d_resource_wrapper<IDXGIFactory>;
    using Factory5 = _detail::global_d3d_resource_wrapper<IDXGIFactory5>;
    using Device = _detail::global_d3d_resource_wrapper<ID3D11Device>;
    using Device5 = _detail::global_d3d_resource_wrapper<ID3D11Device5>;
    using DeviceContext = _detail::global_d3d_resource_wrapper<ID3D11DeviceContext>;
    using DeviceContext4 = _detail::global_d3d_resource_wrapper<ID3D11DeviceContext4>;
    using Debug = _detail::global_d3d_resource_wrapper<ID3D11Debug>;

    class api final
    {
    public:
        // global pointers to most used D3D11 objects for convenience:
        static Factory factory;
        static Factory5 factory5;
        static Device device;
        static Device5 device5;
        static DeviceContext devcon;
        static DeviceContext4 devcon4;
        static Debug debug;

#if defined(ENGINE_TEST)
        static void public_init() {Init();}
        static void public_deinit(){Deinit();}
#endif

    private:
        friend class ::engine::core::Engine;
        static void Init();
        static void Deinit();
    };
} // namespace engine::direct3d