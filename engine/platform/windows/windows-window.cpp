#include "windows-window.hpp"
#include "include/imgui.hpp"
#include "utils/utils.hpp"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::platform::windows
{
    using namespace core::events;
    Window::Window(Props const &props) : core::Window(props)
    {
        WNDCLASSEXW wc;
        // clear out the window class for use
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        // fill in the struct with the needed information
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = L"WindowClass1";
        wc.lpfnWndProc = Window::StaticWindowProc;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.cbWndExtra = sizeof(void *);

        RegisterClassExW(&wc);

        handle_ = CreateWindowExW(NULL, wc.lpszClassName, title_.c_str(),
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  position_.x, position_.y, size_.x, size_.y,
                                  nullptr, nullptr, GetModuleHandle(NULL), nullptr);

        // save window handle in the window class
        SetWindowLongPtrW(handle_, 0, reinterpret_cast<LONG_PTR>(this));

        if (!handle_)
        {
            MessageBoxW(nullptr, L"CreateWindowEx failed", nullptr, 0);
            UnregisterClassW(wc.lpszClassName, GetModuleHandle(NULL));
            throw GetLastError();
        }

        // display the window on the screen
        ShowWindow(handle(), 10);

        initialize_d3d();
    }
    bool Window::OnUpdate()
    {
        if (!alive_)
        {
            return false;
        }
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) // doesnt work
            {
                alive_ = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!IsWindow(handle())) // temporary workaround
        {
            alive_ = false;
            core::events::WindowCloseEvent wce { handle() };
            event_callback_(wce);
        }
        return true;
    }
    LRESULT CALLBACK Window::WindowProcCallback(HWND handle, UINT message,
                                                WPARAM w_param, LPARAM l_param)
    {
        if (ImGui_ImplWin32_WndProcHandler(handle, message, w_param, l_param))
            return true;
        if (message == WM_KEYDOWN)
        {
            KeyPressedEvent event { uint32_t(w_param), LOWORD(l_param) };
            event_callback_(event);
        }
        else if (message == WM_KEYUP)
        {
            KeyReleasedEvent event { uint32_t(w_param) };
            event_callback_(event);
        }
        else if (message == WM_MOUSEMOVE)
        {
            MouseMovedEvent event { core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_LBUTTONDOWN)
        {
            MouseButtonPressedEvent event { 0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_LBUTTONUP)
        {
            MouseButtonReleasedEvent event { 0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_RBUTTONDOWN)
        {
            MouseButtonPressedEvent event { 1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_RBUTTONUP)
        {
            MouseButtonReleasedEvent event { 1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_MOUSEWHEEL)
        {
            MouseScrollEvent event { GET_WHEEL_DELTA_WPARAM(w_param), core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_SIZE) // update the window size if it has changed
        {
            WindowResizeEvent event { LOWORD(l_param), HIWORD(l_param) };
            event_callback_(event);
            size_ = core::math::ivec2 { LOWORD(l_param), HIWORD(l_param) };
        }
        else if (message == WM_EXITSIZEMOVE || message == WM_PAINT)
        {
            OnSizeChangeEnd();
        }
        else if (message == WM_WINDOWPOSCHANGED) // update window position if it has changed
        {
            auto window_pos = reinterpret_cast<LPWINDOWPOS>(l_param);
            position_ = core::math::ivec2 { window_pos->x, window_pos->y };
        }
        else if (message == WM_DESTROY)
        {
            WindowCloseEvent event { handle_ };
            event_callback_(event);
        }
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    LRESULT CALLBACK Window::StaticWindowProc(HWND handle, UINT message,
                                              WPARAM w_param, LPARAM l_param)
    {
        if (auto ptr = reinterpret_cast<Window *>(GetWindowLongPtrW(handle, 0));
            ptr != nullptr)
        {
            return ptr->WindowProcCallback(handle, message, w_param, l_param);
        }
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    void Window::OnSizeChangeEnd()
    {
        if (frame_buffer_.valid())
        {
            direct3d::api::devcon4->OMSetRenderTargets(0, nullptr, nullptr);
            frame_buffer_.reset();
            frame_buffer_view_.reset();
            depth_buffer_.reset();
            depth_buffer_view_.reset();
            swapchain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        }
        initializeFramebuffer();
        initializeDepthbuffer();

        // Set up the viewport.
        D3D11_VIEWPORT vp;
        vp.Width = (float)size().x;
        vp.Height = (float)size().y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        direct3d::api::devcon4->RSSetViewports(1, &vp);
    }
    direct3d::SwapChain1 initializeSwapchain(HWND hWnd, core::math::ivec2 const &window_size)
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        desc.Width = window_size.x;
        desc.Height = window_size.y;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_NONE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = 0;

        IDXGISwapChain1 *swapchain = nullptr;
        spdlog::info(std::to_string(reinterpret_cast<uint64_t>(&direct3d::api::device)));
        spdlog::info(std::to_string(reinterpret_cast<uint64_t>(hWnd)));
        SetLastError(0);
        direct3d::AlwaysAssert(direct3d::api::factory5->CreateSwapChainForHwnd(direct3d::api::device, hWnd, &desc, nullptr, nullptr, &swapchain),
                               "Failed to create the swapchain");
        return direct3d::SwapChain1 { swapchain };
    }

    void Window::initializeFramebuffer()
    {
        ID3D11Texture2D *frame_buffer = nullptr;

        direct3d::AlwaysAssert(swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frame_buffer)),
                               "Failed to get frame buffer");

        ID3D11RenderTargetView *frame_buffer_view = nullptr;

        direct3d::AlwaysAssert(direct3d::api::device->CreateRenderTargetView(frame_buffer, nullptr, &frame_buffer_view),
                               "Failed to initialize framebuffer");
        frame_buffer_ = frame_buffer;
        frame_buffer_view_ = frame_buffer_view;
    }

    void Window::initializeDepthbuffer()
    {
        frame_buffer_->GetDesc(&depth_buffer_desc_); // base on frame_buffer properties

        depth_buffer_desc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_buffer_desc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D *depth_buffer;

        direct3d::AlwaysAssert(direct3d::api::device->CreateTexture2D(&depth_buffer_desc_, nullptr, &depth_buffer),
                               "Failed to initialize depthbuffer");

        ID3D11DepthStencilView *depth_buffer_view;

        direct3d::AlwaysAssert(direct3d::api::device->CreateDepthStencilView(depth_buffer, nullptr, &depth_buffer_view),
                               "Failed to initialize depthbuffer");

        depth_buffer_ = depth_buffer;
        depth_buffer_view_ = depth_buffer_view;
    }

    void Window::initialize_d3d()
    {
        swapchain_ = initializeSwapchain(handle(), size_);
        OnSizeChangeEnd();
    }
} // namespace engine::platform::windows