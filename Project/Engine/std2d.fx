#ifndef _STD2D
#define _STD2D

#include "global.hlsli"
#include "struct.hlsli"

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Std2D(VS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_LinearSampler, _in.vUV);
    
    // ���İ����и� ���� ����
    //if (vColor.a <= 0.1f)
    //{
    //    vColor.rgba = float4(1.f, 0.f, 0.f, 1.f);
    //}
    
    // ȸ�� ���̴�
    //float Aver = (vColor.r + vColor.g + vColor.b) / 3.f;
    //vColor.rgb = float3(Aver, Aver, Aver);
    //vColor.a = 1.f;
    
    //// ����Ÿ ���� ����
    //if (vColor.r >= 1.f && vColor.b >= 1.f)
    //{
    //    clip(-1);
    //}
    
    //return _in.vColor;
    
    return vColor;
}

#endif
