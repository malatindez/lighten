#if 0
#include "object-editor.hpp"

using namespace lighten;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace object_editor
{
    std::shared_ptr<lighten::core::Scene> selected_scene = nullptr;
    entt::entity selected_entity = entt::null;
    render::Mesh const *selected_mesh = nullptr;
    float selected_distance = 0.0f;
    ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    bool useSnap(false);
    glm::vec4 snap;

    void UpdateInstances()
    {
        auto *emissive = Engine::scene()->registry.try_get<components::EmissiveComponent>(selected_entity);
        auto *directional = Engine::scene()->registry.try_get<components::DirectionalLight>(selected_entity);
        auto *point = Engine::scene()->registry.try_get<components::PointLight>(selected_entity);
        auto *spot = Engine::scene()->registry.try_get<components::SpotLight>(selected_entity);
        auto *opaque = Engine::scene()->registry.try_get<components::OpaqueComponent>(selected_entity);
        auto *dissolution = Engine::scene()->registry.try_get<components::DissolutionComponent>(selected_entity);
        auto *decal = Engine::scene()->registry.try_get<components::DecalComponent>(selected_entity);
        if (emissive != nullptr)
        {
            Engine::scene()->renderer->emissive_render_system().ScheduleInstanceUpdate();
        }
        if (directional != nullptr || point != nullptr || spot != nullptr)
        {
            Engine::scene()->renderer->light_render_system().ScheduleInstanceUpdate();
        }
        if (opaque != nullptr)
        {
            Engine::scene()->renderer->opaque_render_system().ScheduleInstanceUpdate();
        }
        if (dissolution != nullptr)
        {
            Engine::scene()->renderer->dissolution_render_system().ScheduleInstanceUpdate();
        }
        if (decal != nullptr)
        {
            Engine::scene()->renderer->decal_render_system().ScheduleInstanceUpdate();
        }
    }

    void EditTransform([[maybe_unused]] CameraController const &camera, Transform &transform)
    {
        glm::mat4 &matrix = transform.model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            bool changed = false;
            changed |= ImGui::InputFloat3("Tr", matrixTranslation, "%.3f", 3);
            changed |= ImGui::InputFloat3("Rt", matrixRotation, "%.3f", 3);
            changed |= ImGui::InputFloat3("Sc", matrixScale, "%.3f", 3);
            ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float *>(&matrix));
            if (mCurrentGizmoOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                    mCurrentGizmoMode = ImGuizmo::LOCAL;
                ImGui::SameLine();
                if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                    mCurrentGizmoMode = ImGuizmo::WORLD;
            }
            if (ImGui::IsKeyPressed(83))
                useSnap = !useSnap;
            ImGui::Checkbox("checkbox", &useSnap);
            ImGui::SameLine();
            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", &snap.x);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", &snap.x);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &snap.x);
                break;
            default:
                mal_toolkit::AlwaysAssert(false);
                break;
            }
            if (ImGui::Button("Reset"))
            {
                changed = true;
                matrix = glm::mat4{1.0f};
                ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
            }
            if (ImGui::Button("Reset scale"))
            {
                changed = true;
                matrixScale[0] = matrixScale[1] = matrixScale[2] = 1;
                ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset rotation"))
            {
                changed = true;
                matrixRotation[0] = matrixRotation[1] = matrixRotation[2] = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset rotation and scale"))
            {
                changed = true;
                matrixScale[0] = matrixScale[1] = matrixScale[2] = 1;
                matrixRotation[0] = matrixRotation[1] = matrixRotation[2] = 0;
            }
            if (changed)
            {
                ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
                transform.position = glm::vec3{matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]};
                transform.scale = glm::vec3{matrixScale[0], matrixScale[1], matrixScale[2]};
                transform.rotation = glm::quat_cast(glm::mat3(scale(matrix, 1.0f / transform.scale)));
                transform.UpdateMatrices();
                UpdateInstances();
            }
        }
    }

    void RenderGizmo(CameraController const &camera, Transform &transform, glm::ivec2 const &window_pos, glm::ivec2 const &window_size, Box const &bounding_box)
    {
        glm::mat4 &matrix = transform.model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
        ImGuizmo::SetRect((float)window_pos.x, (float)window_pos.y, (float)window_size.x, (float)window_size.y);
        static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
        if (ImGuizmo::Manipulate(reinterpret_cast<const float *>(&camera.camera().view),
                                 reinterpret_cast<const float *>(&camera.camera().projection),
                                 mCurrentGizmoOperation,
                                 mCurrentGizmoMode,
                                 reinterpret_cast<float *>(&matrix),
                                 nullptr,
                                 useSnap ? &snap.x : nullptr, reinterpret_cast<const float *>(&bounding_box), boundsSnap))
        {
            ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float *>(&matrix), matrixTranslation, matrixRotation, matrixScale);
            transform.position = glm::vec3{matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]};
            transform.scale = glm::vec3{matrixScale[0], matrixScale[1], matrixScale[2]};
            transform.rotation = glm::quat_cast(glm::mat3(scale(matrix, 1.0f / transform.scale)));
            transform.UpdateMatrices();
            UpdateInstances();
        }
    }
    void EditTransform()
    {
        static Transform empty;
        if (selected_entity != entt::null && selected_scene == Engine::scene())
        {
            // check if has transform else return
            auto *transform_ptr = Engine::scene()->registry.try_get<Transform>(selected_entity);
            if (transform_ptr == nullptr)
            {
                return;
            }
            Transform &transform = *transform_ptr;
            EditTransform(*Engine::scene()->main_camera, transform);
        }
        else
        {
            ImGui::BeginDisabled();
            EditTransform(*Engine::scene()->main_camera, empty);
            ImGui::EndDisabled();
        }
    }
    void OnRender(glm::ivec2 const &window_pos, glm::ivec2 const &window_size)
    {
        if (selected_entity != entt::null && selected_scene == Engine::scene() && InputLayer::instance()->key_state(lighten::core::Key::KEY_CONTROL))
        {
            auto *transform_ptr = Engine::scene()->registry.try_get<Transform>(selected_entity);
            if (transform_ptr == nullptr)
            {
                return;
            }
            Transform &transform = *transform_ptr;
            uint64_t id = 0;

            if (auto const *opaque = Engine::scene()->registry.try_get<OpaqueComponent>(selected_entity);
                opaque != nullptr)
            {
                id = opaque->model_id;
            }
            else if (auto const *emissive = Engine::scene()->registry.try_get<EmissiveComponent>(selected_entity);
                     emissive != nullptr)
            {
                id = emissive->model_id;
            }
            auto const &model = render::ModelSystem::GetModel(id);
            RenderGizmo(*Engine::scene()->main_camera, transform, window_pos, window_size, model.bounding_box);
        }
    }
    void EditMaterialEmpty()
    {
        ImGui::BeginDisabled();
        ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::EndDisabled();
    }

    void EditMaterialOpaque(render::OpaqueMaterial *material)
    {
        if (ImGui::CollapsingHeader("Opaque material", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            static entt::entity entity;
            static uint64_t albedo_map_texture_id = 0;
            static uint64_t saved_albedo_map_texture_id = 0;
            static bool albedo_map_texture_enabled = false;
            static uint64_t normal_map_texture_id = 0;
            static uint64_t saved_normal_map_texture_id = 0;
            static bool normal_map_texture_enabled = false;
            static uint64_t roughness_map_texture_id = 0;
            static uint64_t saved_roughness_map_texture_id = 0;
            static bool roughness_map_texture_enabled = false;
            static uint64_t metallic_map_texture_id = 0;
            static uint64_t saved_metallic_map_texture_id = 0;
            static bool metallic_map_texture_enabled = false;
            static uint64_t sheen_map_texture_id = 0;
            static uint64_t saved_sheen_map_texture_id = 0;
            static bool sheen_map_texture_enabled = false;
            if (entity != selected_entity)
            {
                entity = selected_entity;
                saved_albedo_map_texture_id = albedo_map_texture_id = TextureManager::GetTextureIdByPointer(material->albedo_map);
                saved_normal_map_texture_id = normal_map_texture_id = TextureManager::GetTextureIdByPointer(material->normal_map);
                saved_roughness_map_texture_id = roughness_map_texture_id = TextureManager::GetTextureIdByPointer(material->roughness_map);
                saved_metallic_map_texture_id = metallic_map_texture_id = TextureManager::GetTextureIdByPointer(material->metalness_map);
                saved_sheen_map_texture_id = sheen_map_texture_id = TextureManager::GetTextureIdByPointer(material->sheen_map);
                albedo_map_texture_enabled = albedo_map_texture_id != kInvalidTextureId;
                normal_map_texture_enabled = normal_map_texture_id != kInvalidTextureId;
                roughness_map_texture_enabled = roughness_map_texture_id != kInvalidTextureId;
                metallic_map_texture_enabled = metallic_map_texture_id != kInvalidTextureId;
                sheen_map_texture_enabled = sheen_map_texture_id != kInvalidTextureId;
            }
            ImGui::Checkbox("##albedo_map_enabled", &albedo_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!albedo_map_texture_enabled);
            ImGui::InputScalar("Albedo map texture ID", ImGuiDataType_U64, &albedo_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##normal_map_enabled", &normal_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!normal_map_texture_enabled);
            ImGui::InputScalar("Normal map texture ID", ImGuiDataType_U64, &normal_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##roughness_map_enabled", &roughness_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!roughness_map_texture_enabled);
            ImGui::InputScalar("Roughness map texture ID", ImGuiDataType_U64, &roughness_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##metallic_map_enabled", &metallic_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!metallic_map_texture_enabled);
            ImGui::InputScalar("Metallic map texture ID", ImGuiDataType_U64, &metallic_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();

            ImGui::Checkbox("##sheen_map_enabled", &metallic_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!metallic_map_texture_enabled);
            ImGui::InputScalar("Sheen map texture ID", ImGuiDataType_U64, &sheen_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();

            ImGui::ColorEdit3("Albedo color", reinterpret_cast<float *>(&material->albedo_color));
            ImGui::SliderFloat("Metalness", &material->metalness_value, 0.001f, 1.0f);
            ImGui::SliderFloat("Roughness", &material->roughness_value, 0.001f, 1.0f);
            ImGui::ColorEdit3("Sheen color", reinterpret_cast<float *>(&material->sheen_color));
            ImGui::SliderFloat("Sheen Roughness", &material->sheen_roughness, 0.001f, 1.0f);
            auto width = ::ImGui::GetContentRegionMax().x - 200;
            ImGui::PushItemWidth(width / 2);
            ImGui::SliderFloat("X", &material->uv_multiplier.x, 1, 100);
            ImGui::SameLine();
            ImGui::PushItemWidth(width / 2);
            ImGui::SliderFloat("Y", &material->uv_multiplier.y, 1, 100);
            ImGui::SameLine();
            ImGui::Text("UV multiplier");
            ImGui::Checkbox("Reverse normal y", &material->reverse_normal_y);
            if (!albedo_map_texture_enabled)
                albedo_map_texture_id = kInvalidTextureId;
            if (!normal_map_texture_enabled)
                normal_map_texture_id = kInvalidTextureId;
            if (!roughness_map_texture_enabled)
                roughness_map_texture_id = kInvalidTextureId;
            if (!metallic_map_texture_enabled)
                metallic_map_texture_id = kInvalidTextureId;
            if (!sheen_map_texture_enabled)
                sheen_map_texture_id = kInvalidTextureId;
            if (saved_albedo_map_texture_id != albedo_map_texture_id)
            {
                material->albedo_map = TextureManager::GetTextureView(albedo_map_texture_id);
                saved_albedo_map_texture_id = albedo_map_texture_id;
            }
            if (saved_normal_map_texture_id != normal_map_texture_id)
            {
                material->normal_map = TextureManager::GetTextureView(normal_map_texture_id);
                saved_normal_map_texture_id = normal_map_texture_id;
            }
            if (saved_roughness_map_texture_id != roughness_map_texture_id)
            {
                material->roughness_map = TextureManager::GetTextureView(roughness_map_texture_id);
                saved_roughness_map_texture_id = roughness_map_texture_id;
            }
            if (saved_metallic_map_texture_id != metallic_map_texture_id)
            {
                material->metalness_map = TextureManager::GetTextureView(metallic_map_texture_id);
                saved_metallic_map_texture_id = metallic_map_texture_id;
            }
            if (saved_sheen_map_texture_id != sheen_map_texture_id)
            {
                material->metalness_map = TextureManager::GetTextureView(sheen_map_texture_id);
                saved_sheen_map_texture_id = sheen_map_texture_id;
            }
            material->UpdateTextureFlags();
        }
    }

    void EditMaterial()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            EditMaterialEmpty();
            return;
        }
        bool flag = false;
        if (auto *opaque_component = registry.try_get<components::OpaqueComponent>(selected_entity); opaque_component)
        {
            auto *model_instance = Engine::scene()->renderer->opaque_render_system().GetInstancePtr(opaque_component->model_id);
            if (model_instance)
            {
                uint32_t mesh_id = std::numeric_limits<uint32_t>::max();
                for (size_t i = 0; i < model_instance->model.meshes.size(); i++)
                {
                    if (&model_instance->model.meshes[i].mesh == selected_mesh)
                    {
                        mesh_id = (uint32_t)i;
                    }
                }
                if (mesh_id != std::numeric_limits<uint32_t>::max())
                {
                    for (auto &material_instance : model_instance->mesh_instances[mesh_id].material_instances)
                    {
                        if (auto it = std::ranges::find(material_instance.instances, selected_entity); it != material_instance.instances.end())
                        {
                            EditMaterialOpaque(&material_instance.material);
                            flag = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!flag)
        {
            EditMaterialEmpty();
        }
    }
 
    void ShowTextureList()
    {
        if (ImGui::BeginTable("TextureList", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (auto const &[texture_id, ptr] : core::TextureManager::GetTextures())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%llu", texture_id);
                ImGui::TableNextColumn();
                auto &texture_paths = core::TextureManager::GetTexturePaths();
                if (texture_paths.contains(texture_id))
                {
                    ImGui::Text("%s", core::TextureManager::GetTexturePaths().at(texture_id).string().c_str());
                }
                else
                {
                    ImGui::Text("Unknown");
                }
            }
            ImGui::EndTable();
        }
    }
    
    void EditGrassField()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            ImGui::BeginDisabled();
            ImGui::CollapsingHeader("Grass field", ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::EndDisabled();
            return;
        }
        auto &grs = scene.renderer->grass_render_system();
        if (auto *grass_field_ptr = registry.try_get<GrassComponent>(selected_entity); grass_field_ptr)
        {
            auto &grass_field = *grass_field_ptr;
            if (ImGui::CollapsingHeader("Grass field"))
            {
                auto &material = grs.GetMaterial(grass_field.material_id);
                ImGui::Text("Grass material: ");
                ImGui::InputScalar("planes count ##planes-count", ImGuiDataType_U32, &material.planes_count);
                ImGui::InputScalar("sections per plane ##section-count", ImGuiDataType_U32, &material.section_count);
                ImGui::ColorEdit3("albedo color ##albedo-color", reinterpret_cast<float *>(&material.albedo_color));
                ImGui::DragFloat("ambient occlusion value ##ao-value", &material.ao_value, 0.01f, 0.01f, 1.0f);
                ImGui::DragFloat("roughness value ##roughness-value", &material.roughness_value, 0.01f, 0.01f, 1.0f);
                ImGui::DragFloat("metalness value ##metalness-value", &material.metalness_value, 0.01f, 0.01f, 1.0f);
                ImGui::InputFloat3("wind direction ##wind-direction", &material.wind_vector.x);
                ImGui::SliderAngle("wind amplitude ##wind-amplitude", &material.wind_amplitude, 0.0f, 180.0f);
                ImGui::SliderFloat("wind wavenumber ##wind-wavenumber", &material.wind_wavenumber, 0.0f, 50.0f);
                ImGui::SliderFloat("wind frequency ##wind-frequency", &material.wind_frequency, 0.0f, 50.0f);

                ImGui::NewLine();
                ImGui::Text("Grass field: ");
                ImGui::InputScalar("material id ##material-id", ImGuiDataType_U64, &grass_field.material_id);
                ImGui::InputScalar("x spawn range ##width", ImGuiDataType_Float, &grass_field.spawn_range.x);
                ImGui::InputScalar("z spawn range ##height", ImGuiDataType_Float, &grass_field.spawn_range.y);
                ImGui::InputScalar("min scale ##x-scale-range", ImGuiDataType_Float, &grass_field.grass_size_range.x);
                ImGui::InputScalar("max scale ##y-scale-range", ImGuiDataType_Float, &grass_field.grass_size_range.y);
                ImGui::InputFloat3("initial offset", reinterpret_cast<float *>(&grass_field.initial_offset), "%.3f", 3);
                ImGui::InputScalar("min distance between instances", ImGuiDataType_Float, &grass_field.min_distance);
                ImGui::InputScalar("max attempts", ImGuiDataType_U32, &grass_field.max_attempts);
                if (ImGui::Button("Initialize"))
                {
                    try
                    {
                        grass_field.Initialize(material.atlas_data);
                    }
                    catch (std::exception const &e)
                    {
                        spdlog::error("Failed to initialize grass field using the given parameters. Exception: {}", e.what());
                    }
                    catch (...)
                    {
                        spdlog::error("Failed to initialize grass field using the given parameters. Exception: Unknown");
                    }
                    scene.renderer->grass_render_system().ScheduleInstanceUpdate();
                }
            }
        }
    }

    void EditSkyboxComponent()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            ImGui::BeginDisabled();
            ImGui::CollapsingHeader("Skybox", ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::EndDisabled();
            return;
        }
        if (auto *skybox_ptr = registry.try_get<SkyboxComponent>(selected_entity); skybox_ptr)
        {
            auto &skybox = *skybox_ptr;
            if (ImGui::CollapsingHeader("Skybox"))
            {
                static entt::entity entity;
                static uint64_t texture_id = 0;
                static uint64_t saved_texture_id = 0;
                static bool texture_enabled = false;
                if (entity != selected_entity)
                {
                    entity = selected_entity;
                    saved_texture_id = texture_id = skybox.texture_id;
                    texture_enabled = texture_id != kInvalidTextureId;
                }
                ImGui::Text("Skybox texture id: ");
                ImGui::SameLine();
                ImGui::Checkbox("##texture_enabled", &texture_enabled);
                ImGui::SameLine();
                ImGui::BeginDisabled(!texture_enabled);
                ImGui::InputScalar("Skybox texture ID", ImGuiDataType_U64, &texture_id, nullptr, nullptr, "%llu");
                ImGui::EndDisabled();
                if (!texture_enabled)
                    texture_id = kInvalidTextureId;
                if (texture_id != saved_texture_id)
                {
                    skybox.texture_id = texture_id;
                    saved_texture_id = texture_id;
                }
            }
        }
    }

    // TODO:
    // Move this information to components:: namespace
    // Attaching / removing transform / opaque / emissive components will cause a crash
    // Because we need to call custom AddInstance method
    // We shall change the behaviour of systems so that they will overwrite on_construct / on_destroy

    const auto kComponentNames = std::to_array<std::string>({"Camera",
                                                             //       "Transform",
                                                             //       "Opaque material",
                                                             //       "Emissive material",
                                                             "Point light",
                                                             "Spot light",
                                                             "Directional light",
                                                             "Particle emitter",
                                                             "Skybox"});

    using kComponentTypes = mal_toolkit::parameter_pack_info<
        CameraComponent,
        //      Transform,
        //      render::OpaqueMaterial,
        //     render::EmissiveMaterial,
        PointLight,
        SpotLight,
        DirectionalLight,
        ParticleEmitter,
        SkyboxComponent>;

    void AttachComponent()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            return;
        }
        static int selected_component = 0;
        std::string component_list = "";
        for (auto &name : kComponentNames)
        {
            component_list += name;
            component_list += '\0';
        }
        component_list += '\0';

        ImGui::Combo("##attach_component", &selected_component, component_list.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Attach"))
        {
            mal_toolkit::constexpr_for<0, kComponentNames.size(), 1>([&](auto i) constexpr -> bool
                                                                     {
                                                                   if (i != selected_component)
                                                                   {
                                                                       return true;
                                                                   }
                                                                   if (registry.try_get<typename kComponentTypes::type_at<i>::type>(selected_entity))
                                                                   {
                                                                       ImGui::OpenPopup("Component already attached");
                                                                       return false;
                                                                   }
                                                                   registry.emplace<typename kComponentTypes::type_at<i>::type>(selected_entity);
                                                                   UpdateInstances();
                                                                   return false; });
        }
    }
    void RemoveComponent()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            return;
        }
        static int selected_component = 0;
        std::string component_list = "";
        for (auto &name : kComponentNames)
        {
            component_list += name;
            component_list += '\0';
        }
        component_list += '\0';
        ImGui::Combo("##remove_component", &selected_component, component_list.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Remove"))
        {
            mal_toolkit::constexpr_for<0, kComponentNames.size(), 1>([&](auto i) constexpr -> bool
                                                                     {
                                                                   if (i != selected_component)
                                                                   {
                                                                       return true;
                                                                   }
                                                                   if (registry.try_get<typename kComponentTypes::type_at<i>::type>(selected_entity))
                                                                   {
                                                                       registry.erase<typename kComponentTypes::type_at<i>::type>(selected_entity);
                                                                       Engine::scene()->renderer->ScheduleInstanceUpdate();
                                                                       return false;
                                                                   }
                                                                   ImGui::OpenPopup("Entity does not have this component");
                                                                   return false; });
        }
    }

    void OnGuiRender()
    {
        if (selected_entity != entt::null && !Engine::scene()->registry.valid(selected_entity))
        {
            selected_entity = entt::null;
        }
        ImGui::Begin("Component editor");
        EditGameObject();
        ImGui::Spacing();
        EditCamera();
        ImGui::Spacing();
        EditTransform();
        ImGui::Spacing();
        EditMaterial();
        ImGui::Spacing();
        EditLight();
        ImGui::Spacing();
        EditParticleEmitter();
        ImGui::Spacing();
        EditGrassField();
        ImGui::Spacing();
        EditSkyboxComponent();
        ImGui::Spacing();
        AttachComponent();
        ImGui::Spacing();
        RemoveComponent();
        ImGui::End();
        ImGui::Begin("Texture list");
        ShowTextureList();
        ImGui::End();
    }

    void RegisterKeyCallbacks()
    {
        auto &input = *InputLayer::instance();
        auto scene = Engine::scene();
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{lighten::core::Key::KEY_CONTROL, lighten::core::Key::KEY_LBUTTON},
            [&](InputLayer::KeySeq const &, uint32_t)
            {
                auto &input = *InputLayer::instance();
                auto scene = Engine::scene();
                Ray ray = scene->main_camera->PixelRaycast(glm::vec2{input.mouse_position()});
                MeshIntersection nearest;
                nearest.reset();
                std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
                if (entity.has_value())
                {
                    selected_entity = entity.value();
                    selected_scene = scene;
                    selected_distance = nearest.t;
                    selected_mesh = nearest.mesh_ptr;
                }
                else
                {
                    selected_entity = entt::null;
                    selected_scene = nullptr;
                    selected_distance = 0;
                    selected_mesh = nullptr;
                }
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{lighten::core::Key::KEY_R},
            [&](InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{lighten::core::Key::KEY_T},
            [&](InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{lighten::core::Key::KEY_Y},
            [&](InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            },
            false);

        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{lighten::core::Key::KEY_C},
            [&](InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                if (ImGuizmo::LOCAL == mCurrentGizmoMode)
                {
                    mCurrentGizmoMode = ImGuizmo::WORLD;
                }
                else
                {
                    mCurrentGizmoMode = ImGuizmo::LOCAL;
                }
            },
            false);
    }
}
#endif