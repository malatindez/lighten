#pragma once

#include "direct3d11/d3d-common.hpp"
#include "core/math.hpp"

namespace engine::direct3d
{
    /// @brief The render target base class
    class RenderTargetBase
    {
    public:
        /// @brief Construct a new Render Target Base object
        /// @param format The format of the render target
        explicit RenderTargetBase(DXGI_FORMAT format) noexcept;
        /// @brief virtual destructor of the RenderTargetBase class
        virtual ~RenderTargetBase() = default;
        /// @brief Move constructor of the RenderTargetBase class
        /// @param other The other RenderTargetBase object
        RenderTargetBase(RenderTargetBase &&) = default;
        /// @brief Move assignment operator of the RenderTargetBase class
        /// @param other The other RenderTargetBase object
        /// @return A reference to the RenderTargetBase object
        RenderTargetBase &operator=(RenderTargetBase &&) = default;
        /// @brief Deleted copy constructor of the RenderTargetBase class
        RenderTargetBase(RenderTargetBase const &) = delete;
        /// @brief Deleted copy assignment operator of the RenderTargetBase class
        RenderTargetBase &operator=(RenderTargetBase const &) = delete;

        /// @brief Initialize the render target
        /// @param size The size of the render target
        virtual void SizeResources(glm::ivec2 const &size) = 0;

        /// @brief Get the size of the render target
        [[nodiscard]] inline glm::ivec2 const &size() const noexcept { return size_; }
        /// @brief Get the ptr to the const render target
        /// @return The render target
        ID3D11Texture2D const *const &render_target() const noexcept { return render_target_.ptr(); }
        /// @brief Get the ptr to the const render target view
        /// @return The render target view
        ID3D11RenderTargetView const *const &render_target_view() const noexcept { return render_target_view_.ptr(); }
        /// @brief Get the ptr to the const shader resource view
        /// @return The shader resource view
        ID3D11ShaderResourceView const *const &shader_resource_view() const noexcept { return shader_resource_view_.ptr(); }
        /// @brief Get the ptr to the render target texture
        /// @return The render target texture
        ID3D11Texture2D *const &render_target() noexcept { return render_target_.ptr(); }
        /// @brief Get the ptr to the render target view
        /// @return The render target view
        ID3D11RenderTargetView *const &render_target_view() noexcept { return render_target_view_.ptr(); }
        /// @brief Get the ptr to the shader resource view
        /// @return The shader resource view
        ID3D11ShaderResourceView *const &shader_resource_view() noexcept { return shader_resource_view_.ptr(); }
        /// @brief Get the render target description
        /// @return The render target description
        D3D11_TEXTURE2D_DESC const &render_target_description() const noexcept { return render_target_description_; }
        /// @brief Get the format of the render target
        /// @return The format of the render target
        DXGI_FORMAT GetFormat() const noexcept { return format_; }

    protected:
        /// @brief if true then the render target is initialized
        bool initialized = false;
        /// @brief The render target wrapped pointer
        Texture2D render_target_;
        /// @brief The render target view wrapped pointer
        RenderTargetView render_target_view_;
        /// @brief The shader resource view wrapped pointer
        ShaderResourceView shader_resource_view_;
        /// @brief The format of the render target
        DXGI_FORMAT format_;
        /// @brief The description of the render target
        D3D11_TEXTURE2D_DESC render_target_description_;
        /// @brief The size of the render target
        glm::ivec2 size_;
    };
    /// @brief The render target class
    class RenderTarget : public RenderTargetBase
    {
    public:
        /// @brief Construct a new Render Target object
        RenderTarget(DXGI_FORMAT format) : RenderTargetBase(format) {}
        /// @brief virtual destructor of the RenderTarget class
        virtual ~RenderTarget() = default;
        /// @brief Initialize the render target
        void init();
        /// @brief Deinitialize the render target
        void deinit() noexcept;
        /// @brief Get an empty render target
        /// @return An empty render target
        static RenderTarget empty() noexcept { return RenderTarget(DXGI_FORMAT_UNKNOWN); }
        /// @brief Force the resize of the render target
        /// @param size The size of the render target
        /// @note SizeResources is checking if the size is equal to the one stored in the class, this function will force the resize
        void ForceSizeResources(glm::ivec2 const &size);
        /// @brief Resize the render target
        /// @param size The size of the render target
        /// @note This function will check if the size is equal to the one stored in the class and if it is it will do nothing
        void SizeResources(glm::ivec2 const &size) override;
    };
    /// @todo finish docs for RenderTargetMS

    class RenderTargetMS : public RenderTargetBase
    {
    public:
        RenderTargetMS(DXGI_FORMAT format, uint32_t sample_count = 1) : RenderTargetBase(format), sample_count(sample_count) {}
        virtual ~RenderTargetMS() = default;
        void init();
        void deinit() noexcept;
        static RenderTargetMS empty() noexcept { return RenderTargetMS(DXGI_FORMAT_UNKNOWN); }
        void ForceSizeResources(glm::ivec2 const &size);
        void SizeResources(glm::ivec2 const &size) override;
        uint32_t sample_count = 1;
    };
    /// @todo finish docs for SwapchainRenderTarget

    class SwapchainRenderTarget final : public RenderTargetBase
    {
    public:
        using RenderTargetBase::RenderTargetBase;
        virtual ~SwapchainRenderTarget() = default;
        SwapchainRenderTarget() : RenderTargetBase(DXGI_FORMAT_UNKNOWN) {}
        void init(HWND hWnd, glm::ivec2 const &window_size);
        void deinit() noexcept;
        void SizeResources(glm::ivec2 const &size) override;
        [[nodiscard]] SwapChain1 const &swapchain() const noexcept { return swapchain_; }
        [[nodiscard]] SwapChain1 &swapchain() noexcept { return swapchain_; }

    private:
        SwapChain1 swapchain_ = nullptr;
    };
}