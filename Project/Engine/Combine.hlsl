#include "global.hlsli"
#include "struct.hlsli"

#define Strength g_float_0
#define Threshold g_float_1

// =========================
// LDR Bloom
// =========================
float4 main(PS_IN input) : SV_TARGET
{
    float3 color0 = g_tex_0.Sample(g_PointSampler, input.vUV).rgb;
    float3 color1 = g_tex_1.Sample(g_PointSampler, input.vUV).rgb;
    
    // Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    float RelativeLuminance = dot(color1, float3(0.2126f, 0.7152f, 0.0722f));
    
    // �ش� �ȼ��� ������ ������ Threshold���� �۴ٸ�(��ο� ���̶��) ���������� ó��
    if (RelativeLuminance < Threshold)
        color1 = float3(0.f, 0.f, 0.f);
    
    // ���� �ؽ���� ��ó�� �ؽ��縦 Combine
    return float4(color0 + color1 * Strength, 1.f);
}