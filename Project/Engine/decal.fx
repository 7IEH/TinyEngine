#ifndef _DECAL
#define _DECAL

#include "struct.hlsli"
#include "global.hlsli"

// ==========================
// Decal Shader
// Domain   : DOMAIN_DECAL
// MRT      : Decal
// RS_TYPE  : CULL_FRONT
// DS_TYPE  : NoTest NoWrite
// BS_TYPE  : DECAL 

// Parameter
// g_int_0  : AsEmissive
// g_int_1  : Invert Normal Y
// g_mat_0  : WorldInv

// g_tex_0  : PositionTex
// g_tex_1  : NormalTargetCopyTex
// g_tex_2  : TangentTex
// g_tex_3  : BitangentTex

// g_tex_4  : Decal Color Texture
// g_tex_5  : Decal Normal Texture
// ===========================

PS_IN VS_Decal(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vEmissive : SV_Target2;
};

PS_OUT PS_Decal(PS_IN _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // ȣ��� �ȼ��� ��ġ�� UV ������ ȯ��
    float2 vScreenUV = _in.vPosProj.xy / g_RenderResolution;
    
    // PositionTarget ���� ���� ȣ��� �ȼ����̴��� ������ ������ �����ؼ� ��ǥ���� Ȯ��
    float4 vWorldPos = g_tex_0.Sample(g_LinearWrapSampler, vScreenUV);
    
    // x,y,z ���� 0�̶�� discard
    if (!any(vWorldPos))
    {
        discard;
    }
    
    // Cube �����޽��� ���� �������� ��������.
    float3 vLocal = mul(float4(vWorldPos.xyz, 1.f), g_mat_0).xyz;
    if (abs(vLocal.x) > 1.f || abs(vLocal.y) > 1.f || abs(vLocal.z) > 1.f)
    {
        discard;
    }
    
    // -1 ~ 1 �� 0 ~ 1 ���� ���� 
    vLocal = (vLocal + 1.f) * 0.5f;
    
    output.vColor = float4(0.f, 0.f, 0.f, 0.f);
    output.vNormal = g_tex_1.Sample(g_LinearWrapSampler, vScreenUV);
    output.vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    
    // �����޽� ���� ���� ���� ��
    if (g_btex_4)
    {
        output.vColor = g_tex_4.Sample(g_LinearWrapSampler, vLocal.xz);
    }
    
    if (g_btex_5)
    {
        // TBN ��� ����
        float3 Tangent = g_tex_2.Sample(g_LinearWrapSampler, vScreenUV).xyz;
        float3 Bitangent = g_tex_3.Sample(g_LinearWrapSampler, vScreenUV).xyz;
        float3 normal = normalize(cross(Tangent, Bitangent));
        float3x3 TBN = float3x3(Tangent, Bitangent, normal);
        
        float3 TexNormal = g_tex_5.Sample(g_LinearWrapSampler, vLocal.xz).xyz;
        TexNormal = 2.0 * TexNormal - 1.0; // ���� ���� [-1.0, 1.0]
        
        TexNormal.y = g_int_1 ? -TexNormal.y : TexNormal.y;
    
        output.vNormal.xyz = normalize(mul(TexNormal, TBN));
    }
    
    if (g_int_0)
    {
        output.vEmissive.rgb = output.vColor.rgb * output.vColor.a;
    }
    
    return output;
}

#endif