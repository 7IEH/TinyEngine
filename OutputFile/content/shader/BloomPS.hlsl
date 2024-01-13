#include "global.hlsli"
#include "struct.hlsli"

float4 main(PS_IN input) : SV_TARGET
{
    float3 RTCopyTex = float3(0.f, 0.f, 0.f);
    float3 PostProcessColor = float3(0.f, 0.f, 0.f);
    
    if (g_btex_0)
        RTCopyTex = g_tex_0.Sample(g_PointSampler, input.vUV).rgb;
    
    PostProcessColor = g_postprocess_Tex.Sample(g_PointSampler, input.vUV).rgb;
    
    // Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    float RelativeLuminance = ((0.2126f * PostProcessColor.r) + (0.7152f * PostProcessColor.g) + (0.0722f * PostProcessColor.b));
    
    // �ش� �ȼ��� ������ ������ Threshold���� �۴ٸ�(��ο� ���̶��) ���������� ó��
    if (RelativeLuminance < g_Bloom_Threshold)
        PostProcessColor = float3(0.f, 0.f, 0.f);
    
    // ���� �ؽ���� ��ó�� �ؽ��縦 Combine
    return float4(RTCopyTex + PostProcessColor * g_Bloom_Strength, 1.f);
}