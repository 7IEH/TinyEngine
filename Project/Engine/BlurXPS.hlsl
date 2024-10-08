#include "global.hlsli"
#include "struct.hlsli"

// Gaussian blur
static const float weights[5] = { 0.0545, 0.2442, 0.4026, 0.2442, 0.0545 };

float4 main(PS_IN input) : SV_TARGET
{
    float3 color = float3(0.f, 0.f, 0.f);
    float width;
    float height;
    g_tex_0.GetDimensions(width, height);
    float dx = 1.0 / width;
    float dy = 1.0 / height;
    
    //float dx = 1.0 / g_RenderResolution.x;
    //float dy = 1.0 / g_RenderResolution.y;

    for (int i = 0; i < 5; i++)
    {
        color += weights[i] * g_tex_0.Sample(g_LinearClampSampler, input.vUV0 + (float2(dx, 0.0) * float(i - 2))).rgb;
    }
        
    return float4(color, 1.f);
}