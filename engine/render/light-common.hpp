#pragma once
#include "core/math.hpp"
namespace engine::render
{
    // Schlick's approximation of Fresnel reflectance,
    inline core::math::vec3 Fresnel(float NdotL, const core::math::vec3 &F0)
    {
        return F0 + (1 - F0) * powf(1 - NdotL, 5);
    }

    // Height-correlated Smith G2 for GGX,
    // Filament, 4.4.2 Geometric shadowing
    inline float Smith(float rough2, float NoV, float NoL)
    {
        NoV *= NoV;
        NoL *= NoL;
        return 2.0f / (sqrtf(1.0f + rough2 * (1.0f - NoV) / NoV) + sqrtf(1 + rough2 * (1.0f - NoL) / NoL));
    }

    // GGX normal distribution,
    // Real-Time Rendering 4th Edition, page 340, equation 9.41
    inline float GGX(float rough2, float NoH)
    {
        float denom = NoH * NoH * (rough2 - 1.0f) + 1.0f;
        denom = static_cast<float>(std::numbers::pi) * denom * denom;
        return rough2 / denom;
    }
    inline core::math::vec3 AcesHdr2Ldr(const core::math::vec3 &hdr)
    {
        static core::math::mat3 const m1{
            0.59719f, 0.07600f, 0.02840f,
            0.35458f, 0.90834f, 0.13383f,
            0.04823f, 0.01566f, 0.83777f};
        static core::math::mat3 const m2{
            1.60475f, -0.10208, -0.00327f,
            -0.53108f, 1.10813, -0.07276f,
            -0.07367f, -0.00605, 1.07602f};
        core::math::vec3 v = hdr * m1;
        core::math::vec3 a = v * (v + 0.0245786f) - 0.000090537f;
        core::math::vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
        core::math::vec3 ldr = clamp((a / b) * m2, 0.0f, 1.0f);
        return ldr;
    }
    inline core::math::vec3 AdjustExposure(const core::math::vec3 &color, float EV100)
    {
        float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
        return color * (1.0f / LMax);
    }

    inline core::math::vec3 Illuminate(core::math::vec3 const &L,
                                       render::LightData &light_data,
                                       render::Material const &mat,
                                       float const solid_angle,
                                       core::math::vec3 const &light_energy)
    {
        core::math::vec3 const H = core::math::normalize(L + normalize(light_data.view_dir));
        float ndotl = dot(light_data.normal, L);

        float const rough2 = mat.roughness * mat.roughness;
        core::math::vec3 diffuse = (1 - render::Fresnel(1 - ndotl, mat.F0));
        diffuse *= (1 - mat.metalness);
        diffuse *= (mat.albedo / static_cast<float>(std::numbers::pi));
        
        core::math::vec3 const albedo_metallic = core::math::lerp(core::math::vec3{1}, mat.albedo, mat.metalness);

        core::math::vec3 const F = core::math::clamp(render::Fresnel(dot(L, H), mat.F0), 0.0f, 1.0f);
        float const G = render::Smith(rough2, dot(light_data.normal, normalize(light_data.view_dir)), ndotl);
        float const D = render::GGX(rough2, dot(light_data.normal, H));

        float const ndotv = dot(light_data.normal, normalize(light_data.view_dir));

        core::math::vec3 spec = albedo_metallic * F * G * std::min(1.0f, solid_angle * D / (4 * ndotl * ndotv));
        spec = core::math::clamp(spec, 0.0f, 1.0f);
        ndotl = std::max(ndotl, 0.0f);
        return max((diffuse * solid_angle * ndotl + spec) * light_energy * ndotl, 0);
    }

    inline core::math::vec4 UIntToRGBA(uint32_t value)
    {
        return core::math::vec4{
            (value >> 24) % 256,
            (value >> 16) % 256,
            (value >> 8) % 256,
            value % 256
        };
    }
    inline core::math::vec3 UIntToRGB(uint32_t value)
    {
        return core::math::vec3{
            (value >> 16) % 256,
            (value >> 8) % 256,
            value % 256
        };
    }
    
    inline void branchlessONB(core::math::vec3 const &n, core::math::vec3 &b1, core::math::vec3 &b2)
    {
        float sign = copysignf(1.0f, n.z);
        float const a = -1.0f / (sign + n.z);
        float const b = n.x * n.y * a;
        b1 = core::math::vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
        b2 = core::math::vec3(b, sign + n.y * n.y * a, -n.y);
    }

} // namespace engine::render