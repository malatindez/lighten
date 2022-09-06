#pragma once
#include "api.hpp"
namespace engine::direct3d
{
    class States;
    namespace _state_detail
    {
        // class protected from accidental pointer overwriting
        template <typename T>
        class state_readonly_wrapper final : public readonly_d3d_resource_wrapper<T>
        {
            friend class States;
            using readonly_d3d_resource_wrapper<T>::operator=;
            using readonly_d3d_resource_wrapper<T>::readonly_d3d_resource_wrapper;
        };

    } // namespace _state_detail

    using ReadOnlyRasterizerState = _state_detail::state_readonly_wrapper<ID3D11RasterizerState>;
    using ReadOnlySamplerState = _state_detail::state_readonly_wrapper<ID3D11SamplerState>;
    using ReadOnlyDepthStencilState = _state_detail::state_readonly_wrapper<ID3D11DepthStencilState>;
    class States
    {
    public:
        ReadOnlyRasterizerState cull_none;
        ReadOnlyRasterizerState cull_back;


        ReadOnlyDepthStencilState geq_depth;
        ReadOnlyDepthStencilState geq_depth_no_write;


        ReadOnlySamplerState point_wrap_sampler;
        ReadOnlySamplerState point_clamp_sampler;
        ReadOnlySamplerState linear_wrap_sampler;
        ReadOnlySamplerState linear_clamp_sampler;
        ReadOnlySamplerState anisotropic_wrap_sampler;
        ReadOnlySamplerState anisotropic_clamp_sampler;

        [[nodiscard]] static inline States &instance() { utils::Assert(instance_ != nullptr, "States not initialized"); return *instance_; }
    private:
        friend class Api;
        States();
        States(const States &) = delete;
        States(States &&) = delete;
        States &operator=(const States &) = delete;
        States &operator=(States &&) = delete;
        // make init & deinit static funcitons
        static void Init()
        {
            utils::Assert(instance_ == nullptr, "States already initialized");
            instance_ = std::unique_ptr<States>(new States());
        }
        static void Deinit() { instance_.reset(); }
        static std::unique_ptr<States> instance_;
    };
    [[nodiscard]] inline States &states() { return States::instance(); }
}