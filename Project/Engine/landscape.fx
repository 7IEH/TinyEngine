#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "struct.hlsli"
#include "global.hlsli"

// =======================================
// LandScape Shader
// MRT      : Deferred
// RS_TYPE  : CULL_BACK
// DS_TYPE  : LESS
// BS_TYPE  : DEFAULT

// Parameter
// g_int_0  : Face X
// g_int_1  : Face Z
// g_tex_0  : HeightMap Texture
// =======================================

struct VS_Input
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
};

struct VS_Output
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float3 vNormal : NORMAL;
};

VS_Output VS_LandScape(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
    output.vNormal = _in.vNormal;
    output.vTangent = _in.vTangent;
    output.vBinormal = cross(output.vNormal, output.vTangent);
    
    return output;
}

// Hull Shader
struct PatchLevel
{
    float arrEdge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

PatchLevel PatchConstFunc(InputPatch<VS_Output, 3> _in, uint patchID : SV_PrimitiveID)
{
    PatchLevel output = (PatchLevel) 0.f;
        
    output.arrEdge[0] = g_vec4_0.x;
    output.arrEdge[1] = g_vec4_0.y;
    output.arrEdge[2] = g_vec4_0.z;
    output.Inside = g_vec4_0.w;
    
    return output;
}

struct HS_Output
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float3 vNormal : NORMAL;
};

[patchconstantfunc("PatchConstFunc")]
[outputtopology("triangle_cw")]
[domain("tri")]
[maxtessfactor(64)]
[partitioning("integer")] //[partitioning("fractional_odd")]
[outputcontrolpoints(3)]
HS_Output HS_LandScape(InputPatch<VS_Output, 3> _in, uint _idx : SV_OutputControlPointID)
{
    HS_Output output = (HS_Output) 0.f;
    
    output.vPos = _in[_idx].vPos;
    output.vUV = _in[_idx].vUV;
    
    output.vTangent = _in[_idx].vTangent;
    output.vBinormal = _in[_idx].vBinormal;
    output.vNormal = _in[_idx].vNormal;
    
    return output;
}

struct DS_Output
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
        
    float3 vWorldPos : POSITION;
    float3 vWorldTangent : TANGENT;
    float3 vWorldBinormal : BINORMAL;
    float3 vWorldNormal : NORMAL;
};

[domain("tri")]
DS_Output DS_LandScape(PatchLevel _pathlevel // �� ������ �� ���� ����
             , const OutputPatch<HS_Output, 3> _Origin // ��ġ ���� ����
             , float3 _Weight : SV_DomainLocation)   // �� ���� ������ ���� ����ġ)
{
    DS_Output output = (DS_Output) 0.f;
        
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    
    float3 vTangent = (float3) 0.f;
    float3 vBinormal = (float3) 0.f;
    float3 vNormal = (float3) 0.f;
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Origin[i].vPos * _Weight[i];
        vUV += _Origin[i].vUV * _Weight[i];
        
        vTangent += _Origin[i].vTangent * _Weight[i];
        vBinormal += _Origin[i].vBinormal * _Weight[i];
        vNormal += _Origin[i].vNormal * _Weight[i];
    }
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    output.vWorldPos = mul(float4(vLocalPos, 1.f), g_matWorld).xyz;
    output.vWorldTangent = mul(float4(vTangent, 1.f), g_matWorld).xyz;
    output.vWorldBinormal = mul(float4(vBinormal, 1.f), g_matWorld).xyz;
    output.vWorldNormal = mul(float4(vNormal, 1.f), g_matWorld).xyz;
    
    // ���̸� �ؽ��簡 �����ϴ� ���
    if (g_btex_0)
    {
        float2 FullUV = vUV / float2(g_int_0, g_int_1);
        vLocalPos.y = g_tex_0.SampleLevel(g_LinearClampSampler, FullUV, 0).x;
                       
        // �ֺ� ����(��, �Ʒ�, ��, ��) �� �����Ҷ��� ���ý����̽��󿡼��� ����
        float LocalStep = 1.f / _pathlevel.Inside;
        
        // �ֺ� ����(��, �Ʒ�, ��, ��) �� ���̸� ���̸ʿ��� �����ö� �߽�UV ���� �ֺ�UV �� �����Ҷ��� UV ��ȭ��
        float2 vUVStep = LocalStep / float2(g_int_0, g_int_1);
        
        // ��
        float3 vUp = float3(vLocalPos.x
                            , g_tex_0.SampleLevel(g_LinearClampSampler, float2(FullUV.x, FullUV.y - vUVStep.y), 0).x
                            , vLocalPos.z + LocalStep);
        
        // �Ʒ�
        float3 vDown = float3(vLocalPos.x
                             , g_tex_0.SampleLevel(g_LinearClampSampler, float2(FullUV.x, FullUV.y + vUVStep.y), 0).x
                             , vLocalPos.z - LocalStep);
        
        // ��
        float3 vLeft = float3(vLocalPos.x - LocalStep
                             , g_tex_0.SampleLevel(g_LinearClampSampler, float2(FullUV.x - vUVStep.x, FullUV.y), 0).x
                             , vLocalPos.z);
        
        // ��
        float3 vRight = float3(vLocalPos.x + LocalStep
                            , g_tex_0.SampleLevel(g_LinearClampSampler, float2(FullUV.x + vUVStep.x, FullUV.y), 0).x
                            , vLocalPos.z);
        
        output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
        output.vWorldPos = mul(float4(vLocalPos, 1.f), g_matWorld).xyz;
        output.vWorldTangent = normalize(mul(float4(vRight, 1.f), g_matWorld).xyz - mul(float4(vLeft, 1.f), g_matWorld).xyz);
        output.vWorldBinormal = normalize(mul(float4(vDown, 1.f), g_matWorld).xyz - mul(float4(vUp, 1.f), g_matWorld).xyz);
        output.vWorldNormal = normalize(cross(output.vWorldTangent, output.vWorldBinormal));
    }
        
    return output;
}

struct PS_Output
{
    float4 vColor : SV_Target0;
    float4 vPosition : SV_Target1;
    float4 vNormal : SV_Target2;
    float4 vTangent : SV_Target3;
    float4 vBitangent : SV_Target4;
    float4 vEmissive : SV_Target5;
    float4 vMetallicRoughness : SV_Target6;
    float4 vAmbientOcclusion : SV_Target7;
};

PS_Output PS_LandScape(DS_Output _in)
{
    PS_Output output = (PS_Output) 0.f;
    
    output.vColor = float4(0.4f, 0.4f, 0.4f, 1.f);
    output.vPosition = float4(_in.vWorldPos, 1.f);
    output.vTangent = float4(_in.vWorldTangent, 1.f);
    output.vBitangent = float4(_in.vWorldBinormal, 1.f);
    output.vNormal = float4(_in.vWorldNormal, 1.f);
    output.vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    output.vMetallicRoughness = float4(0.f, 0.f, 0.f, 1.f);
    output.vAmbientOcclusion = float4(0.f, 0.f, 0.f, 1.f);
    
    return output;
}

#endif