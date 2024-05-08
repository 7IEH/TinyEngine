#include "struct.hlsli"
#include "global.hlsli"
#include "UnrealPBRCommon.hlsli"

#define AmbientTex g_tex_0
#define PositionTex g_tex_1
#define NormalTex g_tex_2
#define MetallicRoughnessTex g_tex_3
#define AOTex g_tex_4

#define LIGHT_INDEX g_int_0

float4 main(PS_IN input) : SV_Target
{
     // ȣ��� �ȼ��� ��ġ�� UV ������ ȯ��
    float2 vScreenUV = input.vPosProj.xy / g_RenderResolution;
        
    float3 vWorldPos = PositionTex.Sample(g_LinearWrapSampler, vScreenUV).xyz;
    
    // x,y,z ���� 0�̶�� discard
    if (!any(vWorldPos))
    {
        return float4(1, 0.f, 0.f, 1.f);
        // discard;
    }
        
    // Sphere �����޽��� ���� �������� ��ȯ
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_mat_0).xyz;
    
    //// ���ð������� ��(Sphere) ���ο� �ִ��� üũ
    //if (1.f < length(vLocalPos))
    //{
    //    discard;
    //}
    
    float3 albedo = AmbientTex.Sample(g_LinearWrapSampler, vScreenUV).rgb;
    float3 normalWorld = NormalTex.Sample(g_LinearWrapSampler, vScreenUV).rgb;
    float metallic = MetallicRoughnessTex.Sample(g_LinearWrapSampler, vScreenUV).b;
    float roughness = MetallicRoughnessTex.Sample(g_LinearWrapSampler, vScreenUV).g;
    float ao = AOTex.Sample(g_LinearWrapSampler, vScreenUV).r;
    if (ao >= 1.f)
    {
        ao = SSAOTex.Sample(g_LinearWrapSampler, vScreenUV).r;
    }

    float3 directLighting = float3(0, 0, 0);

    DirectLighting(LIGHT_INDEX, vWorldPos, normalWorld, albedo, ao, metallic, roughness, directLighting);
    
    return float4(0.f, 0.f, 1.f, 1.f);
    //return float4(directLighting, 1.f);
}

