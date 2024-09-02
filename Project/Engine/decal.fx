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
// g_int_0  : Invert Normal Y
// g_vec4_0 : Emissive
// g_mat_0  : WorldInv
// ===========================

#define PositionTex g_tex_0
#define TangentTex g_tex_1
#define BitangentTex g_tex_2

#define DecalAlbedoTexture g_tex_4
#define DecalMRATexture g_tex_5
#define DecalNormalTexture g_tex_6
#define DecalEmissiveTexture g_tex_7

#define InvertNormalY g_int_0
#define Emissive g_vec4_0

PS_IN VS_Decal(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vMRA : SV_Target1;
    float4 vNormal : SV_Target2;
    float4 vEmissive : SV_Target3;
};

PS_OUT PS_Decal(PS_IN _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // ȣ��� �ȼ��� ��ġ�� UV ������ ȯ��
    float2 vScreenUV = _in.vPosProj.xy / g_RenderResolution;
    
    // PositionTarget ���� ���� ȣ��� �ȼ����̴��� ������ ������ �����ؼ� ��ǥ���� Ȯ��
    float4 vWorldPos = PositionTex.Sample(g_LinearWrapSampler, vScreenUV);
    
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
    output.vMRA = float4(0.f, 0.f, 0.f, 0.f);
    output.vNormal = float4(0.f, 0.f, 0.f, 0.f);
    output.vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    
    // �����޽� ���� ���� ���� ��
    if (g_btex_4)
    {
        output.vColor = DecalAlbedoTexture.Sample(g_LinearWrapSampler, vLocal.xz);
    }
    
    if (0.1f > output.vColor.a)
        return output;

    // Emissive �߰� 
    output.vColor.rgb += output.vColor.rgb * Emissive.rgb;
    
    if (g_btex_5)
    {
        output.vMRA = DecalMRATexture.Sample(g_LinearWrapSampler, vLocal.xz);
        output.vMRA.a = 1.f;
    }
    
    if (g_btex_6)
    {
        float3 normal = DecalNormalTexture.Sample(g_LinearWrapSampler, vLocal.xz).xyz;
        
        // DXT5 Normal Map ����ó��
        bool bDXT5Normal = 0.f >= normal.b ? true : false;
        if (bDXT5Normal)
        {
            normal.b = 1.f;
        }
        
        normal = 2.f * normal - 1.f; // ���� ���� [-1.0, 1.0]
        
        normal.y = InvertNormalY ? -normal.y : normal.y;
    
        // TBN ��� ����
        float3 T = TangentTex.Sample(g_LinearWrapSampler, vScreenUV).xyz;
        float3 B = BitangentTex.Sample(g_LinearWrapSampler, vScreenUV).xyz;
        float3 N = normalize(cross(T, B));

        // ����Ǿ��ִ� �븻�� Bitangent ��� X
        float3x3 TBN = bDXT5Normal ? float3x3(T, T, N) : float3x3(T, B, N);
        
        output.vNormal.xyz = normalize(mul(normal, TBN));
        output.vNormal.a = 1.f;
    }
    
    if (g_btex_7)
    {
        output.vEmissive = DecalEmissiveTexture.Sample(g_LinearWrapSampler, vLocal.xz);
    }
    
    //output.vEmissive += Emissive;
    
    return output;
}

#endif