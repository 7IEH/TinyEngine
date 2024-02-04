#include "struct.hlsli"
#include "global.hlsli"

#define Exposure g_float_0 // 렌즈를 오래 열어두면 빛을 많이 받아 들이는 것을 수치적으로 따라한 것
#define Gamma g_float_1    // 어떤 영역의 색을 더 넓게 보여줄지 의미함

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / Gamma;

    color = clamp(Exposure * color, 0., 1.);
    color = pow(color, invGamma);
    return color;
}

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    
    color *= Exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, float3(1.0, 1.0, 1.0) / Gamma);
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / Gamma;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
    return color;
}

float4 main(PS_IN input) : SV_TARGET
{
    float3 color = float3(0.f, 0.f, 0.f);
    
    if (g_btex_0)
    {
        color = g_tex_0.Sample(g_LinearWrapSampler, input.vUV).rgb;
    }
    
    color = LinearToneMapping(color);
    
    return float4(color, 1.0f);
}
