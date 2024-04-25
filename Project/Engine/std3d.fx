#ifndef _STD3D
#define _STD3D

#include "global.hlsli"
#include "struct.hlsli"
#include "Light.hlsli"

PS_IN VS_Std3D(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vPosWorld = mul(float4(_in.vPos, 1.f), g_matWorld).rgb;

    output.normalWorld = normalize(mul(float4(_in.vNormal, 0.f), g_matWorld).rgb);
    output.vTangentWorld = normalize(mul(float4(_in.vTangent, 0.f), g_matWorld).rgb);
        
    output.vUV = _in.vUV;
    output.vColor = _in.vColor;
    
    return output;
}


float4 PS_Std3D(PS_IN _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // ��ü ����
    float4 ObjectColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    
    // ��� �ؽ��İ� ���ε� �Ǿ��ִٸ�, �ؽ����� ������ ����Ѵ�.
    if (g_btex_0)
    {
        ObjectColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
    }
   
    float3 vWorldNormal = _in.normalWorld;
    
    // �븻 �ؽ��İ� ���ε� �Ǿ��ִٸ�, �븻������ �����Ѵ�.
    if (g_btex_1 && g_int_0)
    {
        // ������ ������ 0~1 ������, ����� ���� ���⺤�͸� ���ϱ� ������ ���� �ǵ��� ������ �ٲٱ� ���ؼ�
        // ���� 0 ~ 1 ������ -1.f ~ 1.f �� �����Ѵ�.
        float3 vNormal = g_tex_1.Sample(g_LinearWrapSampler, _in.vUV).rgb;
        vNormal = vNormal * 2.f - 1.f;
                        
        float3 N = _in.normalWorld;
        float3 T = normalize(_in.vTangentWorld - dot(_in.vTangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        
        vWorldNormal = normalize(mul(vNormal.xyz, TBN));
    }
    
    tLightInfo LightColor = (tLightInfo) 0.f;
    
    for (uint i = 0; i < g_Light3DCount; ++i)
    {
        CalculateLight3D(i, _in.vPosWorld, vWorldNormal, LightColor);
    }
        
    vOutColor.rgb = ObjectColor.rgb * LightColor.vRadiance.rgb;
    
    return vOutColor;
}

#endif