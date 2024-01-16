#include "struct.hlsli"
#include "global.hlsli"

struct PS_Input
{
    float4 vPosProj : SV_POSITION0;
    float4 vCenterProj : POSITION;
    float2 vUV : TEXCOORD;
};

float4 main(PS_Input input) : SV_TARGET
{
    float force = 0.125f; // �ְ� ȿ�� ����
    float size = 0.5; // �� ��ü ������
    float thickness = 0.125; // �� �β�
    float4 color = (float4) 0.f;
    
    size = g_TestSize;

    float ratio = g_RenderResolution.x / g_RenderResolution.y;
    float2 vScreenUV = input.vPosProj.xy / g_RenderResolution;
    float2 centerUV = input.vCenterProj.xy / g_RenderResolution;
    
    float2 scaledUV = lerp(0.f, 1.f, (vScreenUV - float2(0.5f, 0.0)) * float2(ratio, 1.0) + float2(0.5f, 0.0f)); // aspect ratio ����
    
    float OuterCircle = 1.f - smoothstep(size - 0.1f, size, length(scaledUV - centerUV));
    
    float InnerCircle = smoothstep(size - thickness - 0.1f, size - thickness, length(scaledUV - centerUV));
    float mask = OuterCircle * InnerCircle; // �ش� ������ �ְ� ó��
    float2 disp = normalize(scaledUV - centerUV) * force * mask;
    
    color = g_postprocess_Tex.Sample(g_LinearSampler, input.vUV - disp);
    //color.rgb = mask;
    
    return color;
}
