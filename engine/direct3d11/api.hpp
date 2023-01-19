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
    /// @brief The _api_detail namespace contains all the classes and functions related to Direct3D11 Api classs that are not meant to be used by the user
    namespace _api_detail
    {
        /// @brief The readonly_d3d_resource_wrapper class is a wrapper for readonly d3d resources
        /// @tparam T The type of the resource
        /// @details This class is used to wrap readonly d3d resources, so that they can be used in the Api class.
        /// These resources are protected from accidental modification, and can only be modified by the Api class.
        template <typename T>
        class api_readonly_wrapper final : public readonly_d3d_resource_wrapper<T>
        {
            friend class engine::direct3d::Api;
            using readonly_d3d_resource_wrapper<T>::operator=;
            using readonly_d3d_resource_wrapper<T>::readonly_d3d_resource_wrapper;
        };
    } // namespace _api_detail

    using ReadonlyFactory = _api_detail::api_readonly_wrapper<IDXGIFactory>;
    using ReadonlyFactory5 = _api_detail::api_readonly_wrapper<IDXGIFactory5>;
    using ReadonlyDevice = _api_detail::api_readonly_wrapper<ID3D11Device>;
    using ReadonlyDevice5 = _api_detail::api_readonly_wrapper<ID3D11Device5>;
    using ReadonlyDeviceContext = _api_detail::api_readonly_wrapper<ID3D11DeviceContext>;
    using ReadonlyDeviceContext4 = _api_detail::api_readonly_wrapper<ID3D11DeviceContext4>;
#if defined(_DEBUG)
    using ReadonlyDebug = _api_detail::api_readonly_wrapper<ID3D11Debug>;
    using ReadonlyDebugInfoQueue = _api_detail::api_readonly_wrapper<ID3D11InfoQueue>;
#endif
    /// @brief The Api class is the main class for the Direct3D11 API
    /// @details This class is used to initialize and deinitialize the Direct3D11 API, and to provide global pointers to the most used Direct3D11 objects.
    /// This class is a singleton, and can be accessed using the api() function.
    /// @warning You need to call the ::engine::core::Engine::Init() function if you want to initialize this class.
    /// @warning You need to call the ::engine::core::Engine::Deinit() function if you want to destroy this class.
    class Api final
    {
    public:
        /// @brief The factory member is a pointer to the IDXGIFactory object
        ReadonlyFactory factory;
        /// @brief The factory5 member is a pointer to the IDXGIFactory5 object
        ReadonlyFactory5 factory5;
        /// @brief The device member is a pointer to the ID3D11Device object
        ReadonlyDevice device;
        /// @brief The device5 member is a pointer to the ID3D11Device5 object
        ReadonlyDevice5 device5;
        /// @brief The devcon member is a pointer to the ID3D11DeviceContext object
        ReadonlyDeviceContext devcon;
        /// @brief The devcon4 member is a pointer to the ID3D11DeviceContext4 object
        ReadonlyDeviceContext4 devcon4;
#if defined(_DEBUG)
        /// @brief The debug member is a pointer to the ID3D11Debug object
        ReadonlyDebug debug;
        /// @brief The debug_info_queue member is a pointer to the ID3D11InfoQueue object
        ReadonlyDebugInfoQueue debug_info_queue;
#endif

#if defined(ENGINE_TEST)
        /// @brief The public_init function is used to initialize the Api class in tests
        static void public_init()
        {
            Init();
        }
        /// @brief The public_deinit function is used to deinitialize the Api class in tests
        static void public_deinit() { Deinit(); }
#endif
        /// @brief The instance function returns a reference to the instance of the Api class
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
        /// @brief The Init function initializes the Api class
        /// @details This function is called by the ::engine::core::Engine::Init() function.
        static void Init();
        /// @brief The Deinit function deinitializes the Api class
        /// @details This function is called by the ::engine::core::Engine::Deinit() function.
        static void Deinit();
        /// @brief The instance_ member is a pointer to the instance of the Api class
        static std::unique_ptr<Api> instance_;
    };
    /// @brief The api function returns a reference to the instance of the Api class
    [[nodiscard]] inline Api &api() { return Api::instance(); }
} // namespace engine::direct3d