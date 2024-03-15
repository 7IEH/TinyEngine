#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "global.hlsli"

// mesh : RectMesh
struct VS_Input
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_Output
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// ==========
// GrayFilter
// ==========
VS_Output VS_Postprocess(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_GrayFilter(VS_Output _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = g_postprocess_Tex.Sample(g_LinearWrapSampler, _in.vUV);
    
    float aver = (vColor.r + vColor.g + vColor.b) / 3.f;
    
    vColor.rgb = float3(aver, aver, aver);
    
    return vColor;
}

#define BlendColor g_vec4_0

float4 PS_BlendFilter(VS_Output _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = g_postprocess_Tex.Sample(g_LinearWrapSampler, _in.vUV);
    
    float Alpha = g_vec4_0.a;

    float3 color = (vColor.rgb * (1.f - Alpha)) + (BlendColor.rgb * Alpha);
    
    return float4(color, 1.f);
}

// ==========
// Distortion
// ==========
VS_Output VS_Postprocess_World(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Distortion(VS_Output _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
        
    // �ȼ����̴��� SV_Position ���� �Էµ� ���� �ȼ� ���̴��� ȣ���� �ش� �ȼ��� ��ǥ�� ����ִ�.
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;
    
    //vScreenUV.y += cos((vScreenUV.x + (g_time * (�ӵ�) )) * (���ļ�)) * (����);
    //vScreenUV.y += cos((vScreenUV.x + (g_time *  0.1f))   *   40.f)  *  0.1f;
    
    if (g_btex_0)
    {
        float2 vUV = _in.vUV;
        vUV.x += g_time * 0.1f;
        
        float2 vNoise = g_tex_0.Sample(g_LinearWrapSampler, vUV).rg;
        vNoise = (vNoise.xy - 0.5f) * 0.1f;
        
        vScreenUV += vNoise;
    }
        
    vColor = g_postprocess_Tex.Sample(g_LinearWrapSampler, vScreenUV);
    
    return vColor;
}

#endif