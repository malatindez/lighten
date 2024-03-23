#pragma once
#include "../../components/particle-emitter.hpp"
#include "../gui/ui-helpers.hpp"
namespace lighten::gui
{
    template<>
    void DrawComponent<components::ParticleEmitter>(entt::registry &registry, entt::entity entity, std::string &entity_id, components::ParticleEmitter& light)
    {
        if (ImGui::CollapsingHeader(("Particle emitter##" + entity_id).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::Checkbox("Freeze: ", &emitter.freeze);
        
            ImGui::Text("Position: ");
        
            ImGui::Text("Min spawn angles:");
            ImGui::SameLine();
            ImGui::Text("yaw: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##yaw_position_min##" + entity_id).c_str(), &emitter.position_yaw_pitch_range.x, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::Text("pitch: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##pitch_position_min##" + entity_id).c_str(), &emitter.position_yaw_pitch_range.z, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
        
            ImGui::Text("Max spawn angles: ");
            ImGui::SameLine();
            ImGui::Text("yaw: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##yaw_position_max##" + entity_id).c_str(), &emitter.position_yaw_pitch_range.y, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::Text("pitch: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##pitch_position_max##" + entity_id).c_str(), &emitter.position_yaw_pitch_range.w, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
        
            ImGui::Text("Distance: ");
            ImGui::Text("min: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderFloat(("##min-spawn-distance##" + entity_id).c_str(), &emitter.position_radius.x, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Text("max: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderFloat(("##max-spawn-distance##" + entity_id).c_str(), &emitter.position_radius.y, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        
            if (emitter.position_yaw_pitch_range.x > emitter.position_yaw_pitch_range.z)
            {
                emitter.position_yaw_pitch_range.x = emitter.position_yaw_pitch_range.z;
            }
            if (emitter.position_yaw_pitch_range.z < emitter.position_yaw_pitch_range.x)
            {
                emitter.position_yaw_pitch_range.z = emitter.position_yaw_pitch_range.x;
            }
            if (emitter.position_yaw_pitch_range.y > emitter.position_yaw_pitch_range.w)
            {
                emitter.position_yaw_pitch_range.y = emitter.position_yaw_pitch_range.w;
            }
            if (emitter.position_yaw_pitch_range.w < emitter.position_yaw_pitch_range.y)
            {
                emitter.position_yaw_pitch_range.w = emitter.position_yaw_pitch_range.y;
            }
            if (emitter.position_radius.x > emitter.position_radius.y)
            {
                emitter.position_radius.x = emitter.position_radius.y;
            }
            if (emitter.position_radius.y < emitter.position_radius.x)
            {
                emitter.position_radius.y = emitter.position_radius.x;
            }
            ImGui::NewLine();
            ImGui::Text("Velocity: ");
        
            ImGui::Text("Min spawn angles: ");
            ImGui::SameLine();
            ImGui::Text("yaw: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##yaw_velocity_min" + entity_id).c_str(), &emitter.velocity_yaw_pitch_range.x, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::Text("pitch: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##pitch_velocity_min" + entity_id).c_str(), &emitter.velocity_yaw_pitch_range.z, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
        
            ImGui::Text("Max spawn angles: ");
            ImGui::SameLine();
            ImGui::Text("yaw: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##yaw_velocity_max" + entity_id).c_str(), &emitter.velocity_yaw_pitch_range.y, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::Text("pitch: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderAngle(("##pitch_velocity_max" + entity_id).c_str(), &emitter.velocity_yaw_pitch_range.w, -180, 180, "%.3f degrees", ImGuiSliderFlags_AlwaysClamp);
        
            ImGui::Text("Distance: ");
        
            ImGui::Text("min: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderFloat(("##min-velocity-distance" + entity_id).c_str(), &emitter.velocity_radius.x, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        
            ImGui::Text("max: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
            ImGui::SliderFloat(("##max-velocity-distance" + entity_id).c_str(), &emitter.velocity_radius.y, -100, 100, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        
            if (emitter.velocity_yaw_pitch_range.x > emitter.velocity_yaw_pitch_range.z)
            {
                emitter.velocity_yaw_pitch_range.x = emitter.velocity_yaw_pitch_range.z;
            }
            if (emitter.velocity_yaw_pitch_range.z < emitter.velocity_yaw_pitch_range.x)
            {
                emitter.velocity_yaw_pitch_range.z = emitter.velocity_yaw_pitch_range.x;
            }
            if (emitter.velocity_yaw_pitch_range.y > emitter.velocity_yaw_pitch_range.w)
            {
                emitter.velocity_yaw_pitch_range.y = emitter.velocity_yaw_pitch_range.w;
            }
            if (emitter.velocity_yaw_pitch_range.w < emitter.velocity_yaw_pitch_range.y)
            {
                emitter.velocity_yaw_pitch_range.w = emitter.velocity_yaw_pitch_range.y;
            }
            if (emitter.velocity_radius.x > emitter.velocity_radius.y)
            {
                emitter.velocity_radius.x = emitter.velocity_radius.y;
            }
            if (emitter.velocity_radius.y < emitter.velocity_radius.x)
            {
                emitter.velocity_radius.y = emitter.velocity_radius.x;
            }
        
            ImGui::ColorEdit4("Base diffuse color", &emitter.base_diffuse_color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
            ImGui::SliderFloat4("Diffuse color variation", &emitter.diffuse_variation.x, -2, 2, "%.3f");
        
            ImGui::Text("Lifespan ");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##min-lifespan" + entity_id).c_str(), &emitter.particle_lifespan_range.x, 0, 100, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##max-lifespan" + entity_id).c_str(), &emitter.particle_lifespan_range.y, 0, 100, "%.3f");
        
            if (emitter.particle_lifespan_range.x > emitter.particle_lifespan_range.y)
            {
                emitter.particle_lifespan_range.x = emitter.particle_lifespan_range.y;
            }
            if (emitter.particle_lifespan_range.y < emitter.particle_lifespan_range.x)
            {
                emitter.particle_lifespan_range.y = emitter.particle_lifespan_range.x;
            }
        
            ImGui::Text("Begin size range");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##begin-min-size" + entity_id).c_str(), &emitter.begin_size_range.x, 0, 100, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##begin-max-size" + entity_id).c_str(), &emitter.begin_size_range.y, 0, 100, "%.3f");
        
            if (emitter.begin_size_range.x > emitter.begin_size_range.y)
            {
                emitter.begin_size_range.x = emitter.begin_size_range.y;
            }
            if (emitter.begin_size_range.y < emitter.begin_size_range.x)
            {
                emitter.begin_size_range.y = emitter.begin_size_range.x;
            }
        
            ImGui::Text("end size range");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##end-min-size" + entity_id).c_str(), &emitter.end_size_range.x, 0, 100, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##end-max-size" + entity_id).c_str(), &emitter.end_size_range.y, 0, 100, "%.3f");
        
            if (emitter.end_size_range.x > emitter.end_size_range.y)
            {
                emitter.end_size_range.x = emitter.end_size_range.y;
            }
            if (emitter.end_size_range.y < emitter.end_size_range.x)
            {
                emitter.end_size_range.y = emitter.end_size_range.x;
            }
        
            ImGui::Text("Emit rate: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##emit-rate" + entity_id).c_str(), &emitter.emit_rate, 0, 100, "%.3f");
        
            // rotation
            ImGui::Text("Rotation ");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##min-rotation" + entity_id).c_str(), &emitter.rotation_range.x, 0, 360, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##max-rotation" + entity_id).c_str(), &emitter.rotation_range.y, 0, 360, "%.3f");
        
            if (emitter.rotation_range.x > emitter.rotation_range.y)
            {
                emitter.rotation_range.x = emitter.rotation_range.y;
            }
            if (emitter.rotation_range.y < emitter.rotation_range.x)
            {
                emitter.rotation_range.y = emitter.rotation_range.x;
            }
        
            ImGui::Text("Rotation speed ");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##min-rotation-speed" + entity_id).c_str(), &emitter.rotation_speed_range.x, 0, 360, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##max-rotation-speed" + entity_id).c_str(), &emitter.rotation_speed_range.y, 0, 360, "%.3f");
        
            if (emitter.rotation_speed_range.x > emitter.rotation_speed_range.y)
            {
                emitter.rotation_speed_range.x = emitter.rotation_speed_range.y;
            }
            if (emitter.rotation_speed_range.y < emitter.rotation_speed_range.x)
            {
                emitter.rotation_speed_range.y = emitter.rotation_speed_range.x;
            }
        
            ImGui::Text("Thickness ");
            ImGui::Text("Min: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##min-thickness" + entity_id).c_str(), &emitter.thickness_range.x, 0, 100, "%.3f");
            ImGui::Text("Max: ");
            ImGui::SameLine();
            ImGui::SliderFloat(("##max-thickness" + entity_id).c_str(), &emitter.thickness_range.y, 0, 100, "%.3f");
        
            if (emitter.thickness_range.x > emitter.thickness_range.y)
            {
                emitter.thickness_range.x = emitter.thickness_range.y;
            }
            if (emitter.thickness_range.y < emitter.thickness_range.x)
            {
                emitter.thickness_range.y = emitter.thickness_range.x;
            }
        
            // acceleration
        
            ImGui::Text("Particle acceleration ");
            ImGui::SameLine();
            ImGui::InputFloat3(("##particle-acceleration" + entity_id).c_str(), &emitter.particle_acceleration.x, "%.3f");
        
            ImGui::Text("Maximum amount of particles: ");
            ImGui::SameLine();
            static const uint32_t min_particles = 0;
            static const uint32_t max_particles = 10000;
            ImGui::SliderScalar(("##max-particles" + entity_id).c_str(), ImGuiDataType_U32, &emitter.maximum_amount_of_particles, &min_particles, &max_particles, "%u", ImGuiSliderFlags_AlwaysClamp);
        }
    }
}