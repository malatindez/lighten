#include "object-editor.hpp"
#include "scene-viewer.hpp"
#include "include/win.hpp"

using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

void DrawPathSelector(std::vector<std::filesystem::path> &paths)
{
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * std::max(3u, std::min(14u, (uint32_t)paths.size()))), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& path : paths)
    {
        ImGui::Text(mal_toolkit::wstring_to_string(path.wstring()).c_str());
    }

    ImGui::EndChild();

    if (ImGui::Button("Select file(s)..."))
    {
        OPENFILENAMEW ofn;
        std::vector<WCHAR> pathBufferWCHAR;
        pathBufferWCHAR.resize(8192);
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = pathBufferWCHAR.data();
        ofn.nMaxFile = static_cast<DWORD>(pathBufferWCHAR.size());
        ofn.lpstrFilter = L"All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

        if (GetOpenFileNameW(&ofn))
        {
            WCHAR* p = ofn.lpstrFile;
            std::wstring dir = p;
            p += dir.size() + 1;

            if (*p)
            {
                do
                {
                    std::wstring wPath = dir + L"\\" + p;
                    std::filesystem::path path = std::filesystem::path(wPath);
                    paths.push_back(path);
                    p += wcslen(p) + 1;
                } while (*p);
            }
            else
            { 
                std::filesystem::path path = std::filesystem::path(dir);
                paths.push_back(path);
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Select Folder..."))
    {
        IFileDialog* pfd;

        if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
        {
            DWORD dwOptions;
            if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
            {
                pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
            }

            if (SUCCEEDED(pfd->Show(NULL)))
            {
                IShellItem* psi;
                if (SUCCEEDED(pfd->GetResult(&psi)))
                {
                    PWSTR pszPath;
                    if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
                    {
                        paths.push_back(std::filesystem::path(pszPath));
                        CoTaskMemFree(pszPath);
                    }
                    psi->Release();
                }
            }
            pfd->Release();
        }
    }
}

namespace scene_viewer
{
    
    void ImportModelGUI()
    {
        auto load = [](std::filesystem::path const &path) {
            try
            {
                if (engine::core::ModelLoader::Load(path) != std::nullopt)
                {
                    spdlog::info("Successfully loaded {}", path.string());
                }
            }
            catch (std::exception& e)
            {
                spdlog::error(e.what());
            }
        };

        static std::vector<std::filesystem::path> paths;

        ImGui::Begin("Import model(s)");
        DrawPathSelector(paths);
        ImGui::SameLine();
        if (ImGui::Button("Import"))
        {
            for (const auto& input_path : paths)
            {
                if (std::filesystem::is_directory(input_path))
                {
                    for (const auto& file_path : std::filesystem::recursive_directory_iterator(input_path))
                    {
                        if (!std::filesystem::is_directory(file_path))
                        {
                            load(file_path);
                        }
                    }
                }
                else
                {
                    load(input_path);
                }
            }
            paths.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            paths.clear();
        }
        ImGui::End();
    }

    entt::entity AddModelToWorld(uint64_t model_id)
    {
        auto& registry = Engine::scene()->registry;
        auto& ors = Engine::scene()->renderer->opaque_render_system();
        auto entity = registry.create();
        auto& game_object = registry.emplace<GameObject>(entity);
        auto& transform = registry.emplace<Transform>(entity);
        transform.reset();
        transform.UpdateMatrices();
        auto& model_obj = render::ModelSystem::GetModel(model_id);
        game_object.name = model_obj.name;

        ors.AddInstance(model_id, registry, entity);
        Engine::scene()->renderer->opaque_render_system().ScheduleInstanceUpdate();
        Engine::scene()->renderer->light_render_system().ScheduleShadowMapUpdate();
        if (math::pow(length(model_obj.bounding_box.size()), 1.0f / 3.0f) > cbrtf(100.0f))
        {
            transform.scale = vec3{ 1.0f / cbrtf(100.0f) };
        }
        for (auto &mesh : model_obj.meshes)
        {
            spdlog::info("Added new mesh to the world: vertices: {}, indices: {}", mesh.mesh.vertices.size(), mesh.mesh.indices.size());
        }
        spdlog::info("Added new model to the world: {}", game_object.name);
        return entity;
    }

    void ModelLoaderGUI()
    {
        static ModelId selected_mesh = std::numeric_limits<ModelId>::max();
        ImGui::Begin("Model Loader");
        ImGui::BeginChild("Models", ImVec2(0, 300), true);
        for (auto const &[hash, model_info] : ModelLoader::loaded_models())
        {
            auto& [path, model_id, model_name] = model_info;
            ImGui::PushID(static_cast<int>(model_id));
            if (ImGui::Selectable(model_name.size() > 0 ? model_name.c_str() : path.filename().string().c_str(), selected_mesh == model_id))
            {
                selected_mesh = model_id;
            }
            ImGui::PopID();
        }
        ImGui::EndChild();
        if(ImGui::Button("Unload"))
        {
            try
            {
                if (auto opt_model_id = ModelLoader::get_hash_from_model_id(selected_mesh);
                    opt_model_id.has_value())
                {
                    auto const& model_id = opt_model_id.value();
//                    auto& model = render::ModelSystem::GetModel(model_id);
                    render::ModelSystem::UnloadModel(model_id);
                    
                    if (ModelLoader::loaded_models().size() > 1)
                    {
                        selected_mesh = ModelLoader::loaded_models().cbegin()->second.model_id;
                    }
                    else {
                        selected_mesh = std::numeric_limits<uint64_t>::max();
                    }
                }
            }
            catch (...) {}
        }
        if (ImGui::Button("Select target mesh"))
        {
            if(auto hash = ModelLoader::get_hash_from_model_id(selected_mesh);
                hash != std::nullopt)
            {
                AddModelToWorld(ModelLoader::loaded_models().at(hash.value()).model_id);
            }
        }
        ImGui::SameLine();
        ImGui::End();
    }


    void DrawGuiEntity(entt::registry &registry, entt::entity entity, std::vector<entt::entity> &drawn, bool subnode = false)
    {
        drawn.push_back(entity);
        auto game_object = registry.try_get<GameObject>(entity);
        std::string name = "None";
        if (game_object != nullptr)
        {
            if (game_object->parent != entt::null && !subnode)
            {
                return;
            }
            name = game_object->name;
        }
        else
        {
            game_object = &registry.emplace<GameObject>(entity);
            game_object->name = name;
        }

        if (ImGui::TreeNode((name + " ##" + std::to_string(*reinterpret_cast<entt::id_type *>(&entity))).c_str()))
        {
            for (auto &child : game_object->children)
            {
                DrawGuiEntity(registry, child, drawn, true);
            }
            if (ImGui::Button("Add child Entity"))
            {
                auto new_entity = registry.create();
                auto &t = registry.emplace<GameObject>(new_entity);
                t.name = "New Entity";
                game_object->children.push_back(new_entity);
                t.parent = entity;
            }
            ImGui::SameLine();
            if (ImGui::Button("Select"))
            {
                object_editor::selected_entity = entity;
                object_editor::selected_scene = engine::core::Engine::scene();
            }
            ImGui::TreePop();
        }
    }
    void OnGuiRender()
    {
        ImGui::Begin("Scene Viewer");
        // get scene
        auto scene = engine::core::Engine::scene();
        auto &registry = scene->registry;
        // draw entity tree
        ImGui::BeginChild("##entity_tree", ImVec2(0, ImGui::GetWindowHeight() * 0.8f), true);
        std::vector<entt::entity> drawn;
        registry.each([&drawn, &registry] (auto entity) {
            if (std::find(drawn.begin(), drawn.end(), entity) != drawn.end())
                return;
            DrawGuiEntity(registry, entity, drawn);
                      });
        ImGui::EndChild();
        if (ImGui::Button("Add Entity"))
        {
            auto entity = registry.create();
            registry.emplace<GameObject>(entity).name = "New Entity";
        }
        ImGui::End();
        ImportModelGUI();
        ModelLoaderGUI();
    }
}