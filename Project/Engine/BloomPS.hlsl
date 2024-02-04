#include "global.hlsli"
#include "struct.hlsli"

#define Strength g_float_0
#define Threshold g_float_1

float4 main(PS_IN input) : SV_TARGET
{
    float3 BloomTex = float3(0.f, 0.f, 0.f);
    float3 PostProcessColor = float3(0.f, 0.f, 0.f);
    
    BloomTex = g_Bloom_Tex.Sample(g_PointSampler, input.vUV).rgb;
    PostProcessColor = g_postprocess_Tex.Sample(g_PointSampler, input.vUV).rgb;
    
    // Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    float RelativeLuminance = dot(PostProcessColor, float3(0.2126f, 0.7152f, 0.0722f));
    
    // �ش� �ȼ��� ������ ������ Threshold���� �۴ٸ�(��ο� ���̶��) ���������� ó��
    if (RelativeLuminance < Threshold)
        PostProcessColor = float3(0.f, 0.f, 0.f);
    
    // ���� �ؽ���� ��ó�� �ؽ��縦 Combine
    return float4(BloomTex + PostProcessColor * Strength, 1.f);
}