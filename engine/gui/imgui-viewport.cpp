#include "imgui-viewport.hpp" 

namespace lighten::gui
{
        
            void UI_ViewportRenderer::OnGuiRender()
            {
    			DisplayViewport();
                core::LayerStack::OnGuiRender();
    		}
            void UI_ViewportRenderer::DisplayViewport()
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
    
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
                ImGui::Begin("Main Dockspace", nullptr, window_flags);
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
                ImGui::End();
    
                ImGui::Begin(rendering_viewport_->name().data());
                auto framebuffer_size = ImGui::GetContentRegionAvail();
                
                ImGui::Image(rendering_viewport_->GetResourceView(), convert_to_imvec(rendering_viewport_->framebuffer_size()));
    
                framebuffer_pos_ = glm::ivec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
                framebuffer_size_ = glm::ivec2{ framebuffer_size.x, framebuffer_size.y };
                framebuffer_size_ = glm::max(framebuffer_size_, glm::ivec2{16});
    
                framebuffer_pos_ += glm::ivec2{ ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y };
    
                glm::vec2 framebuffer_end = glm::vec2{ framebuffer_pos_.x, framebuffer_pos_.y } + glm::vec2{ framebuffer_size_.x, framebuffer_size_.y };
    
                ImVec2 mousePos = ImGui::GetMousePos();
        
                glm::ivec2 relative_mouse_pos{ -1 };
                // Check if the mouse is within the framebuffer area
                if (mousePos.x >= framebuffer_pos_.x && mousePos.x <= (framebuffer_pos_.x + framebuffer_size_.x) &&
                    mousePos.y >= framebuffer_pos_.y && mousePos.y <= (framebuffer_pos_.y + framebuffer_size_.y))
                {
                    relative_mouse_pos = glm::ivec2(mousePos.x - framebuffer_pos_.x, mousePos.y - framebuffer_pos_.y);
                }
    
                bool isWindowFocused = ImGui::IsWindowFocused();
                // Handling keyboard when the window is focused
                if (isWindowFocused)
                {
                    ImGui::GetForegroundDrawList()->AddRect(
                        ImVec2(static_cast<float>(framebuffer_pos_.x), static_cast<float>(framebuffer_pos_.y)),
                        ImVec2(static_cast<float>(framebuffer_end.x), static_cast<float>(framebuffer_end.y)),
                        IM_COL32(255, 255, 0, 255));
                    for (int key = 0; key < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++key)
                    {
                        if (ImGui::IsKeyPressed(key))
                        {
                            lighten::core::events::KeyPressedEvent event(key);
                            OnEvent(event);
                        }
                    }
                }
    
                for (int key = 0; key < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++key)
                {
                    if (ImGui::IsKeyReleased(key))
                    {
                        lighten::core::events::KeyReleasedEvent event(key);
                        OnEvent(event);
                    }
                }
                // Handling mouse clicks when the window is focused
                if (isWindowFocused && (relative_mouse_pos != glm::ivec2{ -1 }))
                {
                    lighten::core::events::MouseMovedEvent mouseMovedEvent(relative_mouse_pos);
                    OnEvent(mouseMovedEvent);
                    for (int button = 0; button < ImGuiMouseButton_COUNT; ++button)
                    {
                        if (ImGui::IsMouseClicked(button))
                        {
                            lighten::core::events::MouseButtonPressedEvent event(static_cast<uint16_t>(button), relative_mouse_pos);
                            OnEvent(event);
                        }
    
                        if (ImGui::IsMouseReleased(button))
                        {
                            lighten::core::events::MouseButtonReleasedEvent event(static_cast<uint16_t>(button), relative_mouse_pos);
                            OnEvent(event);
                        }
                    }
                }
    
                if (ImGui::GetIO().MouseWheel != 0)
                {
                    lighten::core::events::MouseScrollEvent event(static_cast<int16_t>(ImGui::GetIO().MouseWheel * 100), relative_mouse_pos);
                    OnEvent(event);
                }
                ImGui::End();
            }
}