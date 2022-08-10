#include "shader.hpp"
namespace engine::direct3d
{
    ShaderProgram::ShaderProgram(std::filesystem::path const &vertex_path,
                                 std::filesystem::path const &pixel_path,
                                 std::vector<D3D11_INPUT_ELEMENT_DESC> const &ied) DEBUG_UTILS_ASSERT_NOEXCEPT
    {
        try
        {
            auto [vertex_shader, layout] = CompileVertexShader(vertex_path, ied);

            vertex_shader_ = direct3d::VertexShader(vertex_shader);
            input_layout_ = direct3d::InputLayout(layout);

            pixel_shader_ = direct3d::PixelShader(CompilePixelShader(pixel_path));
        }
        catch (CompilationError const &e)
        {
            utils::AlwaysAssert(false, e.what());
        }
        catch (RuntimeError const &e)
        {
            utils::AlwaysAssert(false, e.what());
        }
    }

    void ShaderProgram::apply_shader()
    {
        direct3d::devcon4->VSSetShader(vertex_shader_, nullptr, 0);
        direct3d::devcon4->PSSetShader(pixel_shader_, nullptr, 0);
        direct3d::devcon4->IASetInputLayout(input_layout_);
    }

    inline std::pair<ID3D11VertexShader *, ID3D11InputLayout *> ShaderProgram::CompileVertexShader(std::filesystem::path const &vertex_path,
                                                                                                   std::vector<D3D11_INPUT_ELEMENT_DESC> const &ied) const
    {
        // return_values
        ID3D11VertexShader *vertex_shader;

        ID3DBlob *vertex_shader_blob = nullptr;
        ID3DBlob *errors = nullptr;

        try
        {
            D3DCompileFromFile(vertex_path.wstring().c_str(), nullptr, nullptr, "main",
                               "vs_5_0", kD3DShaderCompileFlags, 0, &vertex_shader_blob, &errors);

            // Create the vertex shader from the buffer.
            direct3d::device5->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader);

        }
        catch (...)
        {
            const char *err_str = "NO INFO";
            if (errors != nullptr)
            {
                err_str = (const char *)errors->GetBufferPointer();
            }
            std::stringstream ss;
            ss << utils::CurrentSourceLocation() << "[" << vertex_path << "] Vertex shader compilation error: " << std::string_view(err_str);

            throw CompilationError(ss.str());
        }
        ID3D11InputLayout *layout;


        direct3d::device5->CreateInputLayout(ied.data(), static_cast<uint32_t>(ied.size()), vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &layout);

        return std::pair { vertex_shader, layout };
    }
    inline ID3D11PixelShader *ShaderProgram::CompilePixelShader(std::filesystem::path const &pixel_path)
    {
        ID3D11PixelShader *pixel_shader;

        ID3DBlob *pixel_shader_blob = nullptr;
        ID3DBlob *errors = nullptr;
        try
        {
            D3DCompileFromFile(pixel_path.wstring().c_str(), nullptr, nullptr, "main",
                               "ps_5_0", kD3DShaderCompileFlags, 0, &pixel_shader_blob, &errors);
            if (pixel_shader_blob == nullptr)
            {
                throw 1;
            }
            direct3d::device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);
        }
        catch (...)
        {
            const char *err_str = "NO INFO";
            if (errors != nullptr)
            {
                err_str = (const char *)errors->GetBufferPointer();
            }
            std::stringstream ss;
            ss << utils::CurrentSourceLocation() << "[" << pixel_path << "] Pixel shader compilation error: " << std::string_view(err_str);

            throw CompilationError(ss.str());
        }

        return pixel_shader;
    }
    direct3d::Buffer ShaderProgram::InitializeUniformBuffer(uint32_t uniform_buffer_size)
    {
        constexpr auto RoundBufferSizeTo16Boundary = [] (uint32_t x) -> uint32_t { return x + 0xf & 0xfffffff0; };
        uniform_buffer_size = RoundBufferSizeTo16Boundary(uniform_buffer_size);

        // Setup the description of the dynamic matrix constant buffer that is in the
        // vertex shader.
        D3D11_BUFFER_DESC uniform_buffer_desc;
        uniform_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        uniform_buffer_desc.ByteWidth = uniform_buffer_size;
        uniform_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        uniform_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        uniform_buffer_desc.MiscFlags = 0;
        uniform_buffer_desc.StructureByteStride = 0;

        // Create the constant buffer pointer so we can access the vertex shader
        // constant buffer from within this class.
        ID3D11Buffer *constant_buffer = nullptr;

        if (direct3d::device5->CreateBuffer(&uniform_buffer_desc, nullptr, &constant_buffer) < 0)
        {
            throw RuntimeError("Error: Cannot initalize uniform buffer");
        }


        return direct3d::Buffer { constant_buffer };
    }

}