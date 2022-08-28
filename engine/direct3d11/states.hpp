#pragma once
#include "api.hpp"
namespace engine::direct3d::states
{
    namespace _state_detail
    {
        class StateInitializer
        {
        private:
            friend class api;
            static void Init();
            static void Deinit();
        };

        // class protected from accidental pointer overwriting
        template <typename T>
        class state_readonly_wrapper final : public readonly_d3d_resource_wrapper<T>
        {
            friend class StateInitializer;
            using readonly_d3d_resource_wrapper<T>::operator=;
            using readonly_d3d_resource_wrapper<T>::readonly_d3d_resource_wrapper;
        };

    } // namespace _state_detail

    using ReadOnlyRasterizerState = _state_detail::state_readonly_wrapper<ID3D11RasterizerState>;
    using ReadOnlySamplerState = _state_detail::state_readonly_wrapper<ID3D11SamplerState>;
    using ReadOnlyDepthStencilState = _state_detail::state_readonly_wrapper<ID3D11DepthStencilState>;

    extern ReadOnlyRasterizerState cull_none;
    extern ReadOnlyRasterizerState cull_back;


    extern ReadOnlyDepthStencilState geq_depth;


    extern ReadOnlySamplerState linear_wrap_sampler;
}