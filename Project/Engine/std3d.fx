#ifndef _STD3D
#define _STD3D

#include "global.hlsli"
#include "struct.hlsli"
#include "Light.hlsli"

#define SpecularIBLTex g_SpecularCube
#define IrradianceIBLTex g_DiffuseCube 

#define MtrlAlbedo g_vAlbedo
#define MtrlDiffuse g_vDiffuse
#define MtrlSpecular g_vSpecular

PS_IN VS_Std3D(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vPosWorld = mul(float4(_in.vPos, 1.f), g_matWorld).rgb;
    
    output.vUV0 = _in.vUV0;
    output.vColor = _in.vColor;
    
    output.vTangentWorld = normalize(mul(float4(_in.vTangent, 0.f), g_matWorldInvTranspose).rgb);
    output.vBitangentWorld = normalize(mul(float4(_in.vBitangent, 0.f), g_matWorldInvTranspose).rgb);
    output.vNormalWorld = normalize(mul(float4(_in.vNormal, 0.f), g_matWorldInvTranspose).rgb);
        
    return output;
}


float4 PS_Std3D(PS_IN _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // 물체 색상
    float4 ObjectColor = MtrlAlbedo;
    
    // 출력 텍스쳐가 바인딩 되어있다면, 텍스쳐의 색상을 사용한다.
    if (g_btex_0)
    {
        ObjectColor *= g_tex_0.Sample(g_LinearWrapSampler, _in.vUV0);
    }
   
    float3 vWorldNormal = _in.vNormalWorld;
    
    // 노말 텍스쳐가 바인딩 되어있다면, 노말맵핑을 진행한다.
    if (g_btex_1)
    {
        // 색상의 범위는 0~1 이지만, 저장된 값은 방향벡터를 뜻하기 때문에 원래 의도한 값으로 바꾸기 위해서
        // 값의 0 ~ 1 범위를 -1.f ~ 1.f 로 변경한다.
        float3 vNormal = g_tex_1.Sample(g_LinearWrapSampler, _in.vUV0).rgb;
        vNormal = vNormal * 2.f - 1.f;
                        
        float3 N = _in.vNormalWorld;
        float3 T = normalize(_in.vTangentWorld - dot(_in.vTangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        
        vWorldNormal = normalize(mul(vNormal.xyz, TBN));
    }
    
    tLightInfo LightColor = (tLightInfo) 0.f;
    
    for (uint i = 0; i < g_LightCount; ++i)
    {
        CalculateLight(i, _in.vPosWorld, vWorldNormal, MtrlDiffuse.rgb, MtrlSpecular.rgb, LightColor);
    }
        
    vOutColor = ObjectColor + LightColor.vRadiance;
    
    // IBL
    float3 toEye = normalize(g_eyeWorld - _in.vPosWorld);
    
    float4 diffuse = float4(0.0, 0.0, 0.0, 0.0);
    float4 specular = float4(0.0, 0.0, 0.0, 0.0);
    
    diffuse = IrradianceIBLTex.Sample(g_LinearWrapSampler, vWorldNormal);
    diffuse.xyz *= MtrlDiffuse.xyz;
    
    specular = SpecularIBLTex.Sample(g_LinearWrapSampler, reflect(-toEye, vWorldNormal));
    specular.xyz *= MtrlSpecular.xyz;
    
    vOutColor += diffuse + specular;

    vOutColor.a = 1.f;
    
    return vOutColor;
}

#endif