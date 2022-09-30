#pragma once
#include "shaders/compute-shader.hpp"
#include "shaders/domain-shader.hpp"
#include "shaders/geometry-shader.hpp"
#include "shaders/hull-shader.hpp"
#include "shaders/input-layout.hpp"
#include "shaders/pixel-shader.hpp"
#include "shaders/vertex-shader.hpp"

namespace engine::render
{
    struct ShaderProgram
    {
        virtual ~ShaderProgram() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };

    class GraphicsShaderProgram final : public ShaderProgram
    {
        enum ShaderSlot
        {
            VS = 0,
            PS = 1,
            HS = 3,
            DS = 2,
            GS = 4,
            ShaderCount = 5
        };

    public:
        GraphicsShaderProgram() = default;
        GraphicsShaderProgram &SetVertexShader(std::shared_ptr<VertexShader> vs)
        {
            shaders_[VS] = vs;
            return *this;
        }
        GraphicsShaderProgram &SetPixelShader(std::shared_ptr<PixelShader> ps)
        {
            shaders_[PS] = ps;
            return *this;
        }
        GraphicsShaderProgram &SetDomainShader(std::shared_ptr<DomainShader> ds)
        {
            shaders_[DS] = ds;
            return *this;
        }
        GraphicsShaderProgram &SetHullShader(std::shared_ptr<HullShader> hs)
        {
            shaders_[HS] = hs;
            return *this;
        }
        GraphicsShaderProgram &SetGeometryShader(std::shared_ptr<GeometryShader> gs)
        {
            shaders_[GS] = gs;
            return *this;
        }
        GraphicsShaderProgram &SetInputLayout(std::shared_ptr<InputLayout> il)
        {
            input_layout_ = il;
            return *this;
        }

        void Bind() override
        {
            if (input_layout_) [[likely]]
                input_layout_->Bind();
            for (size_t i = 0; i < ShaderCount; ++i)
                if (shaders_[i])
                    shaders_[i]->Bind();
        }
        void Unbind() override
        {
            if (input_layout_) [[likely]]
                input_layout_->Unbind();
            for (size_t i = 0; i < ShaderCount; ++i)
                if (shaders_[i])
                    shaders_[i]->Unbind();
        }

    private:
        std::shared_ptr<Shader> shaders_[ShaderCount] = { nullptr };
        std::shared_ptr<InputLayout> input_layout_ = nullptr;
    };
    class ComputeShaderProgram final : public ShaderProgram
    {
    public:
        ComputeShaderProgram() = default;

        void SetComputeShader(std::shared_ptr<ComputeShader> cs)
        {
            shader_ = cs;
        }
        void Bind() override
        {
            if (shader_)
                shader_->Bind();
        }
        void Unbind() override
        {
            if (shader_)
                shader_->Unbind();
        }

    private:
        std::shared_ptr<ComputeShader> shader_ = nullptr;
    };
}