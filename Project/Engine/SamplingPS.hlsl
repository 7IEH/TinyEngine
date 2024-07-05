#include "global.hlsli"

#define Threshold g_float_0

float4 main(PS_IN input) : SV_TARGET
{
    float3 color = g_tex_0.Sample(g_LinearClampSampler, input.vUV0).rgb;
    
    // Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    float RelativeLuminance = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    
    // �ش� �ȼ��� ������ ������ Threshold���� �۴ٸ�(��ο� ���̶��) ���������� ó��
    if (RelativeLuminance < Threshold)
        color = float3(0.f, 0.f, 0.f);
    
    return float4(color, 1.f);
}