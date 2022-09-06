#pragma once
#include "d3d-common.hpp"
#include "utils/utils.hpp"
namespace engine::core
{
    class Engine;
}
namespace engine::direct3d
{
    class Api;

    namespace _api_detail
    {
        // class protected from accidental pointer overwriting
        template <typename T>
        class api_readonly_wrapper final : public readonly_d3d_resource_wrapper<T>
        {
            friend class engine::direct3d::Api;
            using readonly_d3d_resource_wrapper<T>::operator=;
            using readonly_d3d_resource_wrapper<T>::readonly_d3d_resource_wrapper;
        };
    } // namespace _api_detail

    using Factory = _api_detail::api_readonly_wrapper<IDXGIFactory>;
    using Factory5 = _api_detail::api_readonly_wrapper<IDXGIFactory5>;
    using Device = _api_detail::api_readonly_wrapper<ID3D11Device>;
    using Device5 = _api_detail::api_readonly_wrapper<ID3D11Device5>;
    using DeviceContext = _api_detail::api_readonly_wrapper<ID3D11DeviceContext>;
    using DeviceContext4 = _api_detail::api_readonly_wrapper<ID3D11DeviceContext4>;
#if defined(_DEBUG)
    using Debug = _api_detail::api_readonly_wrapper<ID3D11Debug>;
    using DebugInfoQueue = _api_detail::api_readonly_wrapper<ID3D11InfoQueue>;
#endif

    class Api final
    {
    public:
        // global pointers to most used D3D11 objects for convenience:
        Factory factory;
        Factory5 factory5;
        Device device;
        Device5 device5;
        DeviceContext devcon;
        DeviceContext4 devcon4;
#if defined(_DEBUG)
        Debug debug;
        DebugInfoQueue debug_info_queue;
#endif

#if defined(ENGINE_TEST)
        static void public_init()
        {
            Init();
        }
        static void public_deinit() { Deinit(); }
#endif
        [[nodiscard]] static inline Api &instance() { utils::Assert(instance_ != nullptr); return *instance_; }
    private:

        Api() = default;
        // delete move & copy semantics
        Api(Api &&) = delete;
        Api(Api const &) = delete;
        Api &operator=(Api &&) = delete;
        Api &operator=(Api const &) = delete;
    private:
        friend class ::engine::core::Engine;
        static void Init();
        static void Deinit();
        static std::unique_ptr<Api> instance_;
    };
    [[nodiscard]] inline Api &api() { return Api::instance(); }
} // namespace engine::direct3d