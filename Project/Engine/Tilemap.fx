#ifndef _TILEMAP
#define _TILEMAP

#include "Global.hlsli"
#include "struct.hlsli"

// ==============
// TileMap Shader
// Parameter
#define COUNT_X      g_int_0
#define COUNT_Y      g_int_1

#define vSliceUV    g_vec2_0

#define TileAtlas   g_tex_0

// ==============
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

VS_Output VS_TileMap(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_TileMap(VS_Output _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
        
    if (g_btex_0)
    {
        // �� ������ŭ _in.vUV �� ������ �ø�
        float2 vUV = _in.vUV * float2(COUNT_X, COUNT_Y);
        int2 Integer = (int2) floor(vUV);
        vUV = vUV - Integer; // �Ҽ��� �κи� ���Ͽ� 0 ~ 1 ������ ����
                
        int bufferidx = Integer.y * COUNT_X + Integer.x; // �����κ��� ����Ͽ� �ε��� ���
        
        if (!g_TileInfo[bufferidx].bRender)
            discard;
        
        vUV = g_TileInfo[bufferidx].vLeftTopUV + (vSliceUV * vUV);
        vColor = TileAtlas.Sample(g_LinearWrapSampler, vUV);
    }
    
    return vColor;
}

#endif