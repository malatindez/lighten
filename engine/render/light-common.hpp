#pragma once
#include "core/math.hpp"
#include "light-data.hpp"
#include "material.hpp"
namespace engine::render
{
    // Schlick's approximation of Fresnel reflectance,
    inline core::math::vec3 F_Schlick(float NdotL, const core::math::vec3 &F0)
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
        denom = float(std::numbers::pi) * denom * denom;
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
    // May return direction pointing beneath surface horizon (dot(N, dir) < 0), use clampDirToHorizon to fix it.
    // sphereCos is cosine of light sphere solid angle.
    // sphereRelPos is position of a sphere relative to surface:
    // 'sphereDir == normalize(sphereRelPos)' and 'sphereDir * sphereDist == sphereRelPos'
    inline core::math::vec3 approximateClosestSphereDir(bool &intersects, core::math::vec3 const &reflectionDir, float sphereCos,
                                                        core::math::vec3 const &sphereRelPos, core::math::vec3 const &sphereDir, float sphereDist, float sphereRadius)
    {
        float RoS = dot(reflectionDir, sphereDir);

        intersects = RoS >= sphereCos;
        if (intersects)
            return reflectionDir;
        if (RoS < 0.0)
            return sphereDir;

        core::math::vec3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
        return normalize(sphereRelPos + sphereRadius * closestPointDir);
    }

    // Input dir and NoD is N and NoL in a case of lighting computation
    inline void clampDirToHorizon(core::math::vec3 &dir, float &NoD, core::math::vec3 const &normal, float minNoD)
    {
        if (NoD < minNoD)
        {
            dir = normalize(dir + (minNoD - NoD) * normal);
            NoD = minNoD;
        }
    }

    inline core::math::vec3 Illuminate(core::math::vec3 const &L,
                                       render::LightData &light_data,
                                       render::Material const &mat,
                                       float const attenuation,
                                       core::math::vec3 const &light_energy,
                                       float const power)
    {
        core::math::vec3 const &N = light_data.normal;
        core::math::vec3 const V = normalize(light_data.view_dir);
        core::math::vec3 const H = core::math::normalize(L + V);
        float ndotl = dot(N, L);
        float ndotv = dot(N, V);
        float ndoth = dot(N, H);

        core::math::vec3 diffuse = clamp(render::F_Schlick(ndotl, mat.F0), 0.0f, 1.0f);
        diffuse = 1 - diffuse;
        diffuse *= (1 - mat.metalness);
        diffuse *= (mat.albedo / float(std::numbers::pi));

        float const rough2 = mat.roughness * mat.roughness;

        core::math::vec3 const F = render::F_Schlick(dot(L, H), mat.F0);
        float const G = render::Smith(rough2, ndotv, ndotl);
        float const D = render::GGX(rough2, ndoth);
        core::math::vec3 spec = F * G * core::math::clamp(D * attenuation / (4 * ndotv * ndotl), 0.0f, 1.0f);

        return (diffuse * attenuation + spec) * light_energy * power * ndotl;
    }

    inline core::math::vec4 UIntToRGBA(uint32_t value)
    {
        return core::math::vec4{
            (value >> 24) % 256,
            (value >> 16) % 256,
            (value >> 8) % 256,
            value % 256};
    }
    inline core::math::vec3 UIntToRGB(uint32_t value)
    {
        return core::math::vec3{
            (value >> 16) % 256,
            (value >> 8) % 256,
            value % 256};
    }

    template <core::math::AnyVec T>
    inline void branchlessONB(T const &n, T &b1, T &b2)
    {
        float sign = copysignf(1.0f, n.z);
        float const a = -1.0f / (sign + n.z);
        float const b = n.x * n.y * a;
        b1 = T(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
        b2 = T(b, sign + n.y * n.y * a, -n.y);
    }

} // namespace engine::render