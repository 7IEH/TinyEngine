#include "global.hlsli"
#include "struct.hlsli"

float4 main(PS_IN input) : SV_TARGET
{
    //float3 RTCopyTex = float3(0.f, 0.f, 0.f);
    //float3 PostProcessColor = float3(0.f, 0.f, 0.f);
    
    //RTCopyTex = g_RTCopy_Tex.Sample(g_PointSampler, input.vUV).rgb;
    //PostProcessColor = g_postprocess_Tex.Sample(g_PointSampler, input.vUV).rgb;
    
    //// Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    //float RelativeLuminance = dot(PostProcessColor, float3(0.2126f, 0.7152f, 0.0722f));
    
    //// �ش� �ȼ��� ������ ������ Threshold���� �۴ٸ�(��ο� ���̶��) ���������� ó��
    //if (RelativeLuminance < g_Bloom_Threshold)
    //    PostProcessColor = float3(0.f, 0.f, 0.f);
    
    //// ���� �ؽ���� ��ó�� �ؽ��縦 Combine
    //return float4(RTCopyTex + PostProcessColor * g_Bloom_Strength, 1.f);
    
    return float4(0.f, 0.f, 0.f, 1.f); // Temp
}