#include "shader-common.hpp"
namespace engine::render
{
    class InputLayout
    {
    public:
        InputLayout() {}

        InputLayout(ShaderBlob const &vs_blob, std::vector<D3D11_INPUT_ELEMENT_DESC> const &desc = {})
        {
            direct3d::api::device->CreateInputLayout(desc.data(), (UINT)desc.size(), vs_blob.ptr(), vs_blob.size(),
                                                &layout.ptr());
        }

        void Bind()
        {
            direct3d::api::devcon->IASetInputLayout(layout.ptr());
        }
        void Unbind()
        {
            direct3d::api::devcon->IASetInputLayout(nullptr);
        }

    private:
        direct3d::InputLayout layout;
    };
}