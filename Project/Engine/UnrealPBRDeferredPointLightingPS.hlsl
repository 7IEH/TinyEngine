#include "struct.hlsli"
#include "global.hlsli"
#include "UnrealPBRCommon.hlsli"

#define LIGHT_INDEX g_int_0

float4 main(PS_IN input) : SV_Target
{
     // ȣ��� �ȼ��� ��ġ�� UV ������ ȯ��
    float2 vScreenUV = input.vPosProj.xy / g_RenderResolution;
        
    float3 vWorldPos = g_tex_1.Sample(g_LinearWrapSampler, vScreenUV).xyz;
    
    // x,y,z ���� 0�̶�� discard
    if (!any(vWorldPos))
    {
        discard;
    }
        
    // Sphere �����޽��� ���� �������� ��ȯ
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_mat_0).xyz;
    
    // ���ð������� ��(Sphere) ���ο� �ִ��� üũ
    if (1.f < length(vLocalPos))
    {
        discard;
    }
    
    float3 albedo = g_tex_0.Sample(g_LinearWrapSampler, vScreenUV).rgb;
    float3 normalWorld = g_tex_2.Sample(g_LinearWrapSampler, vScreenUV).rgb;
    float metallic = g_tex_3.Sample(g_LinearWrapSampler, vScreenUV).r;
    float roughness = g_tex_3.Sample(g_LinearWrapSampler, vScreenUV).g;
    float ao = g_tex_3.Sample(g_LinearWrapSampler, vScreenUV).b;
    if (ao >= 1.f)
    {
        ao = SSAOTex.Sample(g_LinearWrapSampler, vScreenUV).r;
    }

    float3 directLighting = float3(0, 0, 0);

    DirectLighting(LIGHT_INDEX, vWorldPos, normalWorld, albedo, ao, metallic, roughness, directLighting);
    
    return float4(directLighting, 1.f);
}

