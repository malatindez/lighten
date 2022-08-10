#pragma once
#include "core/math.hpp"
#include "direct3d11/direct3d11.hpp"

namespace engine::direct3d
{
    class TriangleMesh
    {
    public:
        struct Vertex
        {
            core::math::vec3 position;
            core::math::vec4 color;
        };
        TriangleMesh()
        { // create a triangle using the VERTEX struct
            static constexpr Vertex vertices[] =
            {
                {core::math::vec3{0.0f, 0.5f, 1.0f}, core::math::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
                {core::math::vec3{0.45f, -0.5, 1.0f}, core::math::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
                {core::math::vec3{-0.45f, -0.5f, 1.0f}, core::math::vec4{0.0f, 0.0f, 1.0f, 1.0f}} };

            D3D11_BUFFER_DESC vertex_buffer_desc = {};
            vertex_buffer_desc.ByteWidth = sizeof(vertices);
            vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
            vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertex_data = { vertices };

            ID3D11Buffer *vertex_buffer;

            device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
            buffer = vertex_buffer;
        }

        void render()
        {
            // do 3D rendering on the back buffer here
            // select which vertex buffer to display
            UINT stride = sizeof(Vertex);
            UINT offset = 0;

            engine::direct3d::devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            ID3D11Buffer *p_buffer = buffer.ptr();
            engine::direct3d::devcon4->IASetVertexBuffers(0, 1, &p_buffer, &stride, &offset);

            // draw the vertex buffer to the back buffer
            devcon4->Draw(3, 0);
        }

        Buffer buffer;
    };
}