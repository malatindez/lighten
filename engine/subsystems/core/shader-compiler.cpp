#include "shader-compiler.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
#include "mal-toolkit/win-utils.hpp"
namespace engine::core
{
    using namespace render;
    namespace
    {
        class CShaderInclude : public ID3DInclude
        {
        public:
            CShaderInclude(char const *shader_dir) : shader_dir_(shader_dir)
            {}

            HRESULT __stdcall Open(
                D3D_INCLUDE_TYPE include_type,
                LPCSTR file_name,
                [[maybe_unused]] LPCVOID parent_data,
                LPCVOID *data_ptr,
                UINT *bytes) noexcept
            {
                std::filesystem::path final_path;
                std::filesystem::path shader_dir;
                if (parent_data == nullptr) [[likely]]
                {
                    shader_dir = shader_dir_;
                }
                else
                {
                    shader_dir = shader_files_[parent_data].parent_path();
                }
                switch (include_type)
                {
                case D3D_INCLUDE_LOCAL:
                    final_path = shader_dir / std::filesystem::path(file_name);
                    break;
                case D3D_INCLUDE_SYSTEM:
                    final_path = file_name;
                    break;
                default:
                    mal_toolkit::AlwaysAssert(false);
                }
                includes_.push_back(std::filesystem::absolute(final_path));
                std::ifstream file_stream(final_path.string());
                if (file_stream)
                {
                    std::string contents;
                    contents.assign(std::istreambuf_iterator<char>(file_stream),
                                    std::istreambuf_iterator<char>());

                    char *buf = new char[contents.size()];
                    contents.copy(buf, contents.size());
                    *data_ptr = buf;
                    *bytes = (UINT)contents.size();
                    shader_files_[buf] = final_path;
                }
                else
                {
                    *data_ptr = nullptr;
                    *bytes = 0;
                }
                return S_OK;
            }

            HRESULT __stdcall Close(LPCVOID pData) noexcept
            {
                char *buf = (char *)pData;
                delete[] buf;
                return S_OK;
            }

            std::vector<std::filesystem::path> const &includes() const { return includes_; }

        private:
            std::string shader_dir_;
            std::vector<std::filesystem::path> includes_;
            std::map<LPCVOID, std::filesystem::path> shader_files_;
        };
    }

    namespace ShaderCompiler
    {
        void GetBlobFromCompiledShader(std::filesystem::path const &filename, ShaderBlob &blob)
        {
            direct3d::Blob pBytecodeBlob = nullptr;

            mal_toolkit::AlwaysAssert(D3DReadFileToBlob(filename.wstring().c_str(), &pBytecodeBlob.ptr()) >= 0, "Failed to read file to blob");

            blob.bytecode.resize(pBytecodeBlob->GetBufferSize());
            std::memcpy(blob.ptr(), pBytecodeBlob->GetBufferPointer(), blob.size());
        }

        void CompileShader(ShaderCompileInput const &input, ShaderCompileOutput &output)
        {
            output = ShaderCompileOutput{};
            std::string entrypoint, model;
            switch (input.type)
            {
            case direct3d::ShaderType::VertexShader:
                entrypoint = "vs_main";
                model = "vs_5_0";
                break;
            case direct3d::ShaderType::PixelShader:
                entrypoint = "ps_main";
                model = "ps_5_0";
                break;
            case direct3d::ShaderType::HullShader:
                entrypoint = "hs_main";
                model = "hs_5_0";
                break;
            case direct3d::ShaderType::DomainShader:
                entrypoint = "ds_main";
                model = "ds_5_0";
                break;
            case direct3d::ShaderType::GeometryShader:
                entrypoint = "gs_main";
                model = "gs_5_0";
                break;
            case direct3d::ShaderType::ComputeShader:
                entrypoint = "cs_main";
                model = "cs_5_0";
                break;
            default:
                mal_toolkit::Assert(false, "Unknown shader type");
            }

            entrypoint = input.entrypoint.empty() ? entrypoint : input.entrypoint;

            std::vector<D3D_SHADER_MACRO> defines{};
            defines.resize(input.macros.size());

            for (uint32_t i = 0; i < input.macros.size(); ++i)
            {
                defines[i].Name = input.macros[i].name.c_str();
                defines[i].Definition = input.macros[i].definition.c_str();
            }
            defines.push_back({ NULL, NULL });

            direct3d::Blob bytecode_blob = nullptr;
            direct3d::Blob error_blob = nullptr;

            CShaderInclude includer(input.source_file.parent_path().string().c_str());
            HRESULT hr = D3DCompileFromFile(input.source_file.wstring().c_str(),
                                            defines.data(),
                                            &includer, entrypoint.c_str(), model.c_str(),
                                            input.flags, 0, &bytecode_blob.ptr(), &error_blob.ptr());
            auto const &includes = includer.includes();

            if (FAILED(hr))
            {
                if (error_blob != nullptr)
                {
                    std::string err(reinterpret_cast<const char *>(error_blob->GetBufferPointer()));
                    throw CompilerException(err.substr(0, err.size() - 1 /* Remove \n */));
                }
                else
                {
                    throw CompilerException(std::system_category().message(hr));
                }
            }
            output.blob.bytecode.resize(bytecode_blob->GetBufferSize());
            std::memcpy(output.blob.ptr(), bytecode_blob->GetBufferPointer(), bytecode_blob->GetBufferSize());
            output.dependent_files = includes;
            output.dependent_files.push_back(input.source_file);
        }
        std::shared_ptr<VertexShader> CompileVertexShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::VertexShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<VertexShader>(out.blob);
        }
        std::shared_ptr<PixelShader> CompilePixelShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::PixelShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<PixelShader>(out.blob);
        }
        std::shared_ptr<HullShader> CompileHullShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::HullShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<HullShader>(out.blob);
        }
        std::shared_ptr<DomainShader> CompileDomainShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::DomainShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<DomainShader>(out.blob);
        }
        std::shared_ptr<GeometryShader> CompileGeometryShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::GeometryShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<GeometryShader>(out.blob);
        }
        std::shared_ptr<ComputeShader> CompileComputeShader(std::filesystem::path const &input)
        {
            ShaderCompileInput compile_input;
            compile_input.type = direct3d::ShaderType::ComputeShader;
            compile_input.source_file = input;
            ShaderCompileOutput out;
            CompileShader(compile_input, out);
            return std::make_shared<ComputeShader>(out.blob);
        }
    }
}