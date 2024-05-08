#ifndef _LIGHT_DEFERRED
#define _LIGHT_DEFERRED

#include "global.hlsli"
#include "Light.hlsli"

#define MtrlAlbedo g_vAlbedo
#define MtrlDiffuse g_vDiffuse
#define MtrlSpecular g_vSpecular

// ========================
// Directional Light Shader
// MRT      : LIGHT
// Mesh     : RectMesh
// DS_TYPE  : NO_TEST_NO_WIRTE
// BS_TYPE  : ONE_ONE , �������� ���� ������ �� �ְ�

// Parameter
// g_int_0 : Light Idex
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// g_tex_2 : DiffuseTargetTex
// g_tex_3 : SpecularTargetTex
// ========================

PS_IN VS_DirLight(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_DirLight(PS_IN _in) : SV_Target
{
    float4 vWorldPos = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
    
    // x,y,z ���� 0�̶�� discard
    if (!any(vWorldPos.xyz))
    {
        discard;
    }
    
    float3 vWorldNormal = normalize(g_tex_1.Sample(g_LinearWrapSampler, _in.vUV).xyz);
    float3 vDiffuse = g_tex_2.Sample(g_LinearWrapSampler, _in.vUV).xyz;
    float3 vSpecular = g_tex_3.Sample(g_LinearWrapSampler, _in.vUV).xyz;
       
    tLightInfo LightColor = (tLightInfo) 0.f;
    CalculateLight3D(g_int_0, vWorldPos.xyz, vWorldNormal, vDiffuse, vSpecular, LightColor);
        
    LightColor.vRadiance.a = 1.f;
    
    return LightColor.vRadiance;
}

// ========================
// Point Light Shader
// MRT      : LIGHT
// Mesh     : SphereMesh
// DS_TYPE  : NO_TEST_NO_WIRTE
// BS_TYPE  : ONE_ONE , �������� ���� ������ �� �ְ�

// Parameter
// g_int_0 : Light Idex
// g_mat_0 : WorldInv
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// g_tex_2 : DiffuseTargetTex
// g_tex_3 : SpecularTargetTex
// ========================
PS_IN VS_PointLight(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_PointLight(PS_IN _in) : SV_Target
{
    // ȣ��� �ȼ��� ��ġ�� UV ������ ȯ��
    float2 vScreenUV = _in.vPosProj.xy / g_RenderResolution;
        
    float3 vWorldPos = g_tex_0.Sample(g_LinearWrapSampler, vScreenUV).xyz;
    
    // x,y,z ���� 0�̶�� discard
    if (!any(vWorldPos))
    {
        discard;
    }
        
    // Sphere �����޽��� ���� �������� ��ȯ
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_mat_0).xyz;
    
    // ���ð������� ��(Sphere) ���ο� �ִ��� üũ�Ѵ�.
    if (1.f < length(vLocalPos))
    {
        discard;
    }
    
    float3 vWorldNormal = normalize(g_tex_1.Sample(g_LinearWrapSampler, vScreenUV).xyz);
    float3 vDiffuse = g_tex_2.Sample(g_LinearWrapSampler, vScreenUV).xyz;
    float3 vSpecular = g_tex_3.Sample(g_LinearWrapSampler, vScreenUV).xyz;
       
    tLightInfo LightColor = (tLightInfo) 0.f;
    CalculateLight3D(g_int_0, vWorldPos.xyz, vWorldNormal, vDiffuse, vSpecular, LightColor);
        
    LightColor.vRadiance.a = 1.f;
    
    return LightColor.vRadiance;
}

#endif