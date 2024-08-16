#include "struct.hlsli"
#include "global.hlsli"

#define DistortionForce g_float_0 // �ְ� ȿ�� ����
#define CircleSize g_float_1 // �� ��ü ������
#define Thickness g_float_2 // �� �β�

float4 main(PS_IN input) : SV_TARGET
{
    float offsetSize = 0.125f; // �⺻ �� ������
    float size = offsetSize + CircleSize; // �� ��ü ������
    float4 color = (float4) 0.f;

    float ratio = g_RenderResolution.x / g_RenderResolution.y;
    float2 vScreenUV = input.vPosProj.xy / g_RenderResolution.xy;
    float2 centerUV = float2(0.5f, 0.5f);
    
    float2 scaledUV = (input.vUV0 - float2(centerUV.x, 0.0)) * float2(ratio, 1.0) + float2(centerUV.x, 0.0f); // aspect ratio ���� ����

    float OuterCircle = 1.f - smoothstep(size - 0.1f, size, length(scaledUV - centerUV));
    
    float InnerCircle = smoothstep(size - Thickness - 0.1f, size - Thickness, length(scaledUV - centerUV));
    
    float mask = OuterCircle * InnerCircle; // �ش� ������ �ְ� ó��
    float2 disp = normalize(scaledUV - centerUV) * DistortionForce * mask;
    
    color = g_postprocess_Tex.Sample(g_LinearClampSampler, vScreenUV - disp);
    //color.rgb = mask;  ����ŷ Ȯ�ο�
    
    return color;
}
