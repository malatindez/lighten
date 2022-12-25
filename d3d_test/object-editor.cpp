#include "object-editor.hpp"

using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace object_editor
{
    std::shared_ptr<engine::core::Scene> selected_scene = nullptr;
    entt::entity selected_entity = entt::null;
    render::Mesh const *selected_mesh = nullptr;
    float selected_distance = 0.0f;
    ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    bool useSnap(false);
    vec4 snap;

    void OnInstancesUpdated()
    {
        auto *emissive = Engine::scene()->registry.try_get<components::EmissiveComponent>(selected_entity);
        auto *directional = Engine::scene()->registry.try_get<components::DirectionalLight>(selected_entity);
        auto *point = Engine::scene()->registry.try_get<components::PointLight>(selected_entity);
        auto *spot = Engine::scene()->registry.try_get<components::SpotLight>(selected_entity);
        auto *opaque = Engine::scene()->registry.try_get<components::OpaqueComponent>(selected_entity);
        if (emissive != nullptr)
        {
            Engine::scene()->renderer->emissive_render_system().OnInstancesUpdated(Engine::scene()->registry);
        }
        if (directional != nullptr || point != nullptr || spot != nullptr)
        {
            Engine::scene()->renderer->light_render_system().OnInstancesUpdated(Engine::scene().get());
        }
        if (opaque != nullptr)
        {
            Engine::scene()->renderer->opaque_render_system().OnInstancesUpdated(Engine::scene()->registry);
        }
    }

    void EditTransform([[maybe_unused]] CameraController const &camera, TransformComponent &transform)
    {
        mat4 &matrix = transform.model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
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
            ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.arr.data());
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
                utils::AlwaysAssert(false);
                break;
            }
            if (ImGui::Button("Reset"))
            {
                changed = true;
                matrix = mat4::identity();
                ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
            }
            if (ImGui::Button("Reset scale"))
            {
                changed = true;
                matrixScale[0] = matrixScale[1] = matrixScale[2] = 1;
                ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
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
                ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
                transform.position = vec3{ matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
                transform.scale = vec3{ matrixScale[0], matrixScale[1], matrixScale[2] };
                transform.rotation = QuaternionFromRotationMatrix(scale(matrix, 1.0f / transform.scale).as_rmat<3, 3>());
                transform.UpdateMatrices();
                OnInstancesUpdated();
            }
        }
    }

    void RenderGizmo(CameraController const &camera, TransformComponent &transform, ivec2 const &window_pos, ivec2 const &window_size, Box const &bounding_box)
    {
        mat4 &matrix = transform.model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
        ImGuizmo::SetRect((float)window_pos.x, (float)window_pos.y, (float)window_size.x, (float)window_size.y);
        static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
        if (ImGuizmo::Manipulate(camera.camera().view.arr.data(),
                                 camera.camera().projection.arr.data(),
                                 mCurrentGizmoOperation,
                                 mCurrentGizmoMode,
                                 matrix.arr.data(),
                                 nullptr,
                                 useSnap ? &snap.x : nullptr, Box{ .min = -bounding_box.min, .max = -bounding_box.max }.min.data.data(), boundsSnap))
        {
            ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
            transform.position = vec3{ matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
            transform.scale = vec3{ matrixScale[0], matrixScale[1], matrixScale[2] };
            transform.rotation = QuaternionFromRotationMatrix(scale(matrix, 1.0f / transform.scale).as_rmat<3, 3>());
            transform.UpdateMatrices();
            OnInstancesUpdated();
        }
    }
    void EditTransform()
    {
        static TransformComponent empty;
        if (selected_entity != entt::null && selected_scene == Engine::scene())
        {
            // check if has transform else return
            auto *transform_ptr = Engine::scene()->registry.try_get<TransformComponent>(selected_entity);
            if (transform_ptr == nullptr)
            {
                return;
            }
            TransformComponent &transform = *transform_ptr;
            EditTransform(*Engine::scene()->main_camera, transform);
        }
        else
        {
            ImGui::BeginDisabled();
            EditTransform(*Engine::scene()->main_camera, empty);
            ImGui::EndDisabled();
        }
    }
    void OnRender(ivec2 const &window_pos, ivec2 const &window_size)
    {
        if (selected_entity != entt::null && selected_scene == Engine::scene() && InputLayer::instance()->key_state(engine::core::Key::KEY_CONTROL))
        {
            auto *transform_ptr = Engine::scene()->registry.try_get<TransformComponent>(selected_entity);
            if (transform_ptr == nullptr)
            {
                return;
            }
            TransformComponent &transform = *transform_ptr;
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
            if (entity != selected_entity)
            {
                entity = selected_entity;
                saved_albedo_map_texture_id = albedo_map_texture_id = TextureManager::GetTextureIdByPointer(material->albedo_map);
                saved_normal_map_texture_id = normal_map_texture_id = TextureManager::GetTextureIdByPointer(material->normal_map);
                saved_roughness_map_texture_id = roughness_map_texture_id = TextureManager::GetTextureIdByPointer(material->roughness_map);
                saved_metallic_map_texture_id = metallic_map_texture_id = TextureManager::GetTextureIdByPointer(material->metalness_map);
                albedo_map_texture_enabled = albedo_map_texture_id != kInvalidTextureId;
                normal_map_texture_enabled = normal_map_texture_id != kInvalidTextureId;
                roughness_map_texture_enabled = roughness_map_texture_id != kInvalidTextureId;
                metallic_map_texture_enabled = metallic_map_texture_id != kInvalidTextureId;
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
            ImGui::ColorEdit3("Albedo color", material->albedo_color.data.data());
            ImGui::SliderFloat("Metalness", &material->metalness_value, 0.001f, 1.0f);
            ImGui::SliderFloat("Roughness", &material->roughness_value, 0.001f, 1.0f);
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
            material->UpdateTextureFlags();
        }
    }

    void EditMaterialEmissive(render::EmissiveMaterial *material)
    {
        if (ImGui::CollapsingHeader("Emissive material", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            static entt::entity entity;
            static uint64_t texture_id = 0;
            static uint64_t saved_texture_id = 0;
            static bool texture_enabled = false;
            if (entity != selected_entity)
            {
                entity = selected_entity;
                texture_id = TextureManager::GetTextureIdByPointer(material->emissive_texture);
                saved_texture_id = TextureManager::GetTextureIdByPointer(material->emissive_texture);
                texture_enabled = texture_id == kInvalidTextureId;
            }

            ImGui::Checkbox("##ambient_enabled", &texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!texture_enabled);
            ImGui::InputScalar("Texture ID", ImGuiDataType_U64, &texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::ColorEdit3("Emissive color", material->emissive_color.data.data());
            ImGui::SliderFloat("Power", &material->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
            if (!texture_enabled)
                texture_id = kInvalidTextureId;
            if (saved_texture_id != texture_id)
            {
                material->emissive_texture = TextureManager::GetTextureView(texture_id);
                saved_texture_id = texture_id;
            }
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

        if (auto *emissive_component = registry.try_get<components::EmissiveComponent>(selected_entity); emissive_component)
        {
            auto *model_instance = Engine::scene()->renderer->emissive_render_system().GetInstancePtr(emissive_component->model_id);
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
                            EditMaterialEmissive(&material_instance.material);
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
    void EditLightEmpty()
    {
        ImGui::BeginDisabled();
        ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::EndDisabled();
    }
    void EditLightPoint(components::PointLight *point_light)
    {
        if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3("Color", point_light->color.data.data());
            ImGui::SliderFloat("Power", &point_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
        }
    }
    void EditLightSpot(components::SpotLight *spot_light)
    {
        if (ImGui::CollapsingHeader("Spot light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3("Color", spot_light->color.data.data());
            ImGui::SliderFloat("Power", &spot_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderAngle("Inner cutoff", &spot_light->inner_cutoff, 0, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderAngle("Outer cutoff", &spot_light->outer_cutoff, 0, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            if (spot_light->inner_cutoff >= spot_light->outer_cutoff)
            {
                spot_light->inner_cutoff = spot_light->outer_cutoff * 0.95;
            }
        }
    }
    void EditLightDirectional(components::DirectionalLight *directional_light)
    {
        if (ImGui::CollapsingHeader("Directional light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::ColorEdit3("Color", directional_light->color.data.data());
            ImGui::SliderFloat("Power", &directional_light->power, 0, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::InputFloat("Solid angle", &directional_light->solid_angle);
        }
    }
    void EditLight()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            EditLightEmpty();
            return;
        }
        if (auto *light = registry.try_get<components::PointLight>(selected_entity); light)
        {
            EditLightPoint(light);
        }
        if (auto *light = registry.try_get<components::SpotLight>(selected_entity); light)
        {
            EditLightSpot(light);
        }
        if (auto *light = registry.try_get<components::DirectionalLight>(selected_entity); light)
        {
            EditLightDirectional(light);
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

    void EditParticleEmitter()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            ImGui::BeginDisabled();
            ImGui::CollapsingHeader("Particle emitter", ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::EndDisabled();
            return;
        }
        if (auto *emitter = registry.try_get<components::ParticleEmitter>(selected_entity); emitter)
        {
            if (ImGui::CollapsingHeader("Particle emitter", ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                ImGui::Checkbox("Freeze: ", &emitter->freeze);

                ImGui::Text("Position: ");

                ImGui::Text("Min spawn angles:");
                ImGui::SameLine();
                ImGui::Text("yaw: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##yaw_position_min", &emitter->position_yaw_pitch_range.x, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
                ImGui::SameLine();
                ImGui::Text("pitch: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##pitch_position_min", &emitter->position_yaw_pitch_range.z, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Text("Max spawn angles: ");
                ImGui::SameLine();
                ImGui::Text("yaw: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##yaw_position_max", &emitter->position_yaw_pitch_range.y, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
                ImGui::SameLine();
                ImGui::Text("pitch: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##pitch_position_max", &emitter->position_yaw_pitch_range.w, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Text("Distance: ");
                ImGui::Text("min: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderFloat("##min-spawn-distance", &emitter->position_radius.x, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::Text("max: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderFloat("##max-spawn-distance", &emitter->position_radius.y, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);

                if (emitter->position_yaw_pitch_range.x > emitter->position_yaw_pitch_range.z) { emitter->position_yaw_pitch_range.x = emitter->position_yaw_pitch_range.z; }
                if (emitter->position_yaw_pitch_range.z < emitter->position_yaw_pitch_range.x) { emitter->position_yaw_pitch_range.z = emitter->position_yaw_pitch_range.x; }
                if (emitter->position_yaw_pitch_range.y > emitter->position_yaw_pitch_range.w) { emitter->position_yaw_pitch_range.y = emitter->position_yaw_pitch_range.w; }
                if (emitter->position_yaw_pitch_range.w < emitter->position_yaw_pitch_range.y) { emitter->position_yaw_pitch_range.w = emitter->position_yaw_pitch_range.y; }
                if (emitter->position_radius.x > emitter->position_radius.y) { emitter->position_radius.x = emitter->position_radius.y; }
                if (emitter->position_radius.y < emitter->position_radius.x) { emitter->position_radius.y = emitter->position_radius.x; }
                ImGui::NewLine();
                ImGui::Text("Velocity: ");

                ImGui::Text("Min spawn angles: ");
                ImGui::SameLine();
                ImGui::Text("yaw: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##yaw_velocity_min", &emitter->velocity_yaw_pitch_range.x, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
                ImGui::SameLine();
                ImGui::Text("pitch: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##pitch_velocity_min", &emitter->velocity_yaw_pitch_range.z, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Text("Max spawn angles: ");
                ImGui::SameLine();
                ImGui::Text("yaw: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##yaw_velocity_max", &emitter->velocity_yaw_pitch_range.y, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
                ImGui::SameLine();
                ImGui::Text("pitch: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderAngle("##pitch_velocity_max", &emitter->velocity_yaw_pitch_range.w, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Text("Distance: ");

                ImGui::Text("min: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderFloat("##min-velocity-distance", &emitter->velocity_radius.x, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Text("max: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
                ImGui::SliderFloat("##max-velocity-distance", &emitter->velocity_radius.y, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);

                if (emitter->velocity_yaw_pitch_range.x > emitter->velocity_yaw_pitch_range.z) { emitter->velocity_yaw_pitch_range.x = emitter->velocity_yaw_pitch_range.z; }
                if (emitter->velocity_yaw_pitch_range.z < emitter->velocity_yaw_pitch_range.x) { emitter->velocity_yaw_pitch_range.z = emitter->velocity_yaw_pitch_range.x; }
                if (emitter->velocity_yaw_pitch_range.y > emitter->velocity_yaw_pitch_range.w) { emitter->velocity_yaw_pitch_range.y = emitter->velocity_yaw_pitch_range.w; }
                if (emitter->velocity_yaw_pitch_range.w < emitter->velocity_yaw_pitch_range.y) { emitter->velocity_yaw_pitch_range.w = emitter->velocity_yaw_pitch_range.y; }
                if (emitter->velocity_radius.x > emitter->velocity_radius.y) { emitter->velocity_radius.x = emitter->velocity_radius.y; }
                if (emitter->velocity_radius.y < emitter->velocity_radius.x) { emitter->velocity_radius.y = emitter->velocity_radius.x; }

                ImGui::ColorEdit4("Base diffuse color", &emitter->base_diffuse_color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
                ImGui::SliderFloat4("Diffuse color variation", &emitter->diffuse_variation.x, -2, 2, "%.3f");

                ImGui::Text("Lifespan ");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##min-lifespan", &emitter->particle_lifespan_range.x, 0, 100, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##max-lifespan", &emitter->particle_lifespan_range.y, 0, 100, "%.3f");

                if (emitter->particle_lifespan_range.x > emitter->particle_lifespan_range.y) { emitter->particle_lifespan_range.x = emitter->particle_lifespan_range.y; }
                if (emitter->particle_lifespan_range.y < emitter->particle_lifespan_range.x) { emitter->particle_lifespan_range.y = emitter->particle_lifespan_range.x; }

                ImGui::Text("Begin size range");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##begin-min-size", &emitter->begin_size_range.x, 0, 100, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##begin-max-size", &emitter->begin_size_range.y, 0, 100, "%.3f");

                if (emitter->begin_size_range.x > emitter->begin_size_range.y) { emitter->begin_size_range.x = emitter->begin_size_range.y; }
                if (emitter->begin_size_range.y < emitter->begin_size_range.x) { emitter->begin_size_range.y = emitter->begin_size_range.x; }

                ImGui::Text("end size range");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##end-min-size", &emitter->end_size_range.x, 0, 100, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##end-max-size", &emitter->end_size_range.y, 0, 100, "%.3f");

                if (emitter->end_size_range.x > emitter->end_size_range.y) { emitter->end_size_range.x = emitter->end_size_range.y; }
                if (emitter->end_size_range.y < emitter->end_size_range.x) { emitter->end_size_range.y = emitter->end_size_range.x; }

                ImGui::Text("Emit rate: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##emit-rate", &emitter->emit_rate, 0, 100, "%.3f");

                // rotation
                ImGui::Text("Rotation ");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##min-rotation", &emitter->rotation_range.x, 0, 360, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##max-rotation", &emitter->rotation_range.y, 0, 360, "%.3f");

                if (emitter->rotation_range.x > emitter->rotation_range.y) { emitter->rotation_range.x = emitter->rotation_range.y; }
                if (emitter->rotation_range.y < emitter->rotation_range.x) { emitter->rotation_range.y = emitter->rotation_range.x; }

                ImGui::Text("Rotation speed ");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##min-rotation-speed", &emitter->rotation_speed_range.x, 0, 360, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##max-rotation-speed", &emitter->rotation_speed_range.y, 0, 360, "%.3f");

                if (emitter->rotation_speed_range.x > emitter->rotation_speed_range.y) { emitter->rotation_speed_range.x = emitter->rotation_speed_range.y; }
                if (emitter->rotation_speed_range.y < emitter->rotation_speed_range.x) { emitter->rotation_speed_range.y = emitter->rotation_speed_range.x; }

                ImGui::Text("Thickness ");
                ImGui::Text("Min: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##min-thickness", &emitter->thickness_range.x, 0, 100, "%.3f");
                ImGui::Text("Max: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##max-thickness", &emitter->thickness_range.y, 0, 100, "%.3f");

                if (emitter->thickness_range.x > emitter->thickness_range.y) { emitter->thickness_range.x = emitter->thickness_range.y; }
                if (emitter->thickness_range.y < emitter->thickness_range.x) { emitter->thickness_range.y = emitter->thickness_range.x; }

                // acceleration

                ImGui::Text("Particle acceleration ");
                ImGui::SameLine();
                ImGui::InputFloat3("##particle-acceleration", &emitter->particle_acceleration.x, "%.3f");

                ImGui::Text("Maximum amount of particles: ");
                ImGui::SameLine();
                static const uint32_t min_particles = 0;
                static const uint32_t max_particles = 10000;
                ImGui::SliderScalar("##max-particles", ImGuiDataType_U32, &emitter->maximum_amount_of_particles, &min_particles, &max_particles, "%u", ImGuiSliderFlags_AlwaysClamp);
            }
        }
    }

    void EditCameraComponent()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            ImGui::BeginDisabled();
            ImGui::CollapsingHeader("Particle emitter", ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::EndDisabled();
            return;
        }

        if (auto *camera_ptr = registry.try_get<CameraComponent>(selected_entity); camera_ptr)
        {
            auto &camera = *camera_ptr;
            if (ImGui::CollapsingHeader("Camera"))
            {
                ImGui::Text("Field of view: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##fov", &camera.fovy_, 0, 100, "%.3f");
                ImGui::Text("Near plane: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##near-plane", &camera.z_near_, 0, 2000, "%.3f");
                ImGui::Text("Far plane: ");
                ImGui::SameLine();
                ImGui::SliderFloat("##far-plane", &camera.z_far_, 0, 2000, "%.3f");
            }
            if (ImGui::CollapsingHeader("Camera data: "))
            {
                ImGui::Text("View matrix: ");
                ImGui::Text(utils::FormatToString(camera.view).c_str());
                ImGui::Text("Projection matrix: ");
                ImGui::Text(utils::FormatToString(camera.projection).c_str());
                ImGui::Text("View projection matrix: ");
                ImGui::Text(utils::FormatToString(camera.view_projection).c_str());
                ImGui::Text("Inv view matrix: ");
                ImGui::Text(utils::FormatToString(camera.inv_view).c_str());
                ImGui::Text("Inv projection matrix: ");
                ImGui::Text(utils::FormatToString(camera.inv_projection).c_str());
                ImGui::Text("Inv view projection matrix: ");
                ImGui::Text(utils::FormatToString(camera.inv_view_projection).c_str());
            }
        }
    }
    void EditGameObject()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            ImGui::BeginDisabled();
            ImGui::CollapsingHeader("Game Object", ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::EndDisabled();
            return;
        }
        if (auto *game_object_ptr = registry.try_get<GameObject>(selected_entity); game_object_ptr)
        {
            auto &game_object = *game_object_ptr;
            if (ImGui::CollapsingHeader("Game Object"))
            {
                ImGui::Text("Name: ");
                game_object.name.reserve(game_object.name.size() + 1);
                ImGui::SameLine();
                ImGui::InputText("##name", game_object.name.data(), game_object.name.capacity());
                ImGui::Text("Tag: ");
                ImGui::SameLine();
                ImGui::InputText("##tag", game_object.tag.data(), game_object.tag.capacity());
                auto constexpr convert_entity_id = [] (entt::entity id) -> uint64_t
                {
                    return static_cast<uint64_t>(*reinterpret_cast<entt::id_type *>(&id));
                };
                ImGui::Text("Entity id: ");
                ImGui::SameLine();
                ImGui::Text("%ull", convert_entity_id(selected_entity));
                ImGui::Text("Parent entity id: ");
                ImGui::SameLine();
                ImGui::Text("%ull", convert_entity_id(game_object.parent));
                ImGui::Text("Amount of children: ");
                ImGui::SameLine();
                ImGui::Text("%ull", game_object.children.size());
                ImGui::TreeNode("Children: ");
                for (auto child : game_object.children)
                {
                    auto *ptr = registry.try_get<GameObject>(child);
                    char const *name = "None";
                    if (ptr != nullptr)
                    {
                        name = ptr->name.data();
                    }
                    ImGui::Text("Name: ");
                    ImGui::SameLine();
                    ImGui::Text(name);
                    ImGui::SameLine();
                    ImGui::Text(". Id: ");
                    ImGui::SameLine();
                    ImGui::Text("%ull", convert_entity_id(child));
                }
                ImGui::TreePop();
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

    const auto kComponentNames = std::to_array<std::string>({
        "Camera",
        //       "Transform",
        //       "Opaque material",
        //       "Emissive material",
               "Point light",
               "Spot light",
               "Directional light",
               "Particle emitter",
               "Skybox" });

    using kComponentTypes = utils::parameter_pack_info<
        CameraComponent,
        //      TransformComponent,
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
            utils::constexpr_for<0, kComponentNames.size(), 1>([&] (auto i) constexpr -> bool
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
                                                                   OnInstancesUpdated();
                                                                   return false;
                                                               });
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
            utils::constexpr_for<0, kComponentNames.size(), 1>([&] (auto i) constexpr  -> bool
                                                               {
                                                                   if (i != selected_component)
                                                                   {
                                                                       return true;
                                                                   }
                                                                   if (registry.try_get<typename kComponentTypes::type_at<i>::type>(selected_entity))
                                                                   {
                                                                       registry.erase<typename kComponentTypes::type_at<i>::type>(selected_entity);
                                                                       Engine::scene()->renderer->OnInstancesUpdated(Engine::scene().get());
                                                                       return false;
                                                                   }
                                                                   ImGui::OpenPopup("Entity does not have this component");
                                                                   return false;
                                                               });
        }
    }

    void OnGuiRender()
    {
        ImGui::Begin("Component editor");
        EditGameObject();
        ImGui::Spacing();
        EditCameraComponent();
        ImGui::Spacing();
        EditTransform();
        ImGui::Spacing();
        EditMaterial();
        ImGui::Spacing();
        EditLight();
        ImGui::Spacing();
        EditParticleEmitter();
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
            InputLayer::KeySeq{ engine::core::Key::KEY_CONTROL, engine::core::Key::KEY_LBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t)
            {
                if (ImGui::GetIO().WantCaptureMouse)
                {
                    return;
                }
                auto &input = *InputLayer::instance();
                auto scene = Engine::scene();
                Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
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
            InputLayer::KeySeq{ engine::core::Key::KEY_R },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_T },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_F },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            },
            false);

        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_C },
            [&] (InputLayer::KeySeq const &, uint32_t count)
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