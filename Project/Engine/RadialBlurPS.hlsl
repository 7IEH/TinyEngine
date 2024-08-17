#include "global.hlsli"
#include "struct.hlsli"
#include "func.hlsli"

#define Radius g_float_0    // �� ���� �����ʴ� ����
#define BlurPower g_float_1 // �� ����
#define FocusUV g_vec2_0    // �߽� UV ��ġ

#define NumBlurSample 6     

float4 main(PS_IN input) : SV_TARGET
{
    float2 Direction = input.vUV0 - FocusUV;
    float3 color = (float3) 0.f;
    
    [unroll]
    for (int i = 0; i < NumBlurSample; i++)
    {
        float scale = 1 - BlurPower * (i / (float) NumBlurSample) * (saturate(length(Direction) / Radius));
        color += g_postprocess_Tex.Sample(g_LinearClampSampler, FocusUV + Direction * scale).rgb;
    }
    
    color /= (float) NumBlurSample;
    
    return float4(color, 1.f);
}