#pragma once
#include "shader-common.hpp"
namespace engine::render
{
    class Shader
    {
    public:
        Shader(ShaderBlob const &blob, direct3d::ShaderType type)
            : blob_(blob), kType(type)
        {}

        ShaderBlob &blob() { return blob_; }
        direct3d::ShaderType type() const { return kType; }

        virtual ~Shader() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Recreate(ShaderBlob const &blob) = 0;

    private:
        ShaderBlob blob_;
        direct3d::ShaderType const kType;
    };
}