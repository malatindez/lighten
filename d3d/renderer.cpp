#include "renderer.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;

void Renderer::OnEvent(events::Event &event)
    {
        if (event.type() == events::EventType::AppRender)
        {
            float time = Application::TimeFromStart();

            static vec4 const kSkyColor{vec3{0.25f}, 0.0f};

            ID3D11RenderTargetView *view = window_->frame_buffer_view();
            devcon4->OMSetRenderTargets(1, &view, nullptr);

            devcon4->RSSetState(rasterizer_state_);
            ID3D11SamplerState* sampler_state = sampler_state_;
            devcon4->PSSetSamplers(0, 1, &sampler_state);
            ID3D11RenderTargetView* frame_buffer_view = window_->frame_buffer_view();

            devcon4->OMSetRenderTargets(1, &frame_buffer_view, window_->depth_buffer_view());
            devcon4->OMSetDepthStencilState(depth_stencil_state_, 0);
            devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

            engine::core::Application::logger().info(core::debug_utils::CurrentSourceLocation() + "Failed to initialize framebuffer");

            // clear the back buffer to a deep blue
            devcon4->ClearRenderTargetView(window_->frame_buffer_view(), kSkyColor.data.data());
            devcon4->ClearDepthStencilView(window_->depth_buffer_view(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            triangle_.shader.apply_shader();
            triangle_.mesh.render();

            // switch the back buffer and the front buffer
            window_->swapchain()->Present(1, 0);
        }
    }
    
    Renderer::Triangle Renderer::create_triangle()
    {
        auto path = std::filesystem::current_path();
        auto vertex_path = path / "assets/shaders/triangle/vertex.hlsl";
        auto pixel_path = path / "assets/shaders/triangle/pixel.hlsl";

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        return Triangle{.mesh = TriangleMesh(),
                        .shader = ShaderProgram(vertex_path, pixel_path, sizeof(Triangle::ShaderInput), d3d_input_desc)};
    }

    void Renderer::initialize()
    {
        D3D11_RASTERIZER_DESC1 rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;

        ID3D11RasterizerState1 *rasterizer_state;

        device5->CreateRasterizerState1(&rasterizer_desc,
                                                          &rasterizer_state);

        D3D11_SAMPLER_DESC sampler_desc = {};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        ID3D11SamplerState *sampler_state;

        device5->CreateSamplerState(&sampler_desc, &sampler_state);

        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

        ID3D11DepthStencilState *depth_stencil_state;

        device->CreateDepthStencilState(&depth_stencil_desc,
                                                          &depth_stencil_state);

        rasterizer_state_ = rasterizer_state;
        sampler_state_ = sampler_state;
        depth_stencil_state_ = depth_stencil_state;
    }