#ifndef _STD_DEFERED
#define _STD_DEFERED

#include "global.hlsli"
#include "func.hlsli"

#define MtrlAlbedo g_vAlbedo
#define MtrlDiffuse g_vDiffuse
#define MtrlSpecular g_vSpecular
 
PS_IN VS_Std3D_Deferred(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;
    
    float4 PrevPos = float4(_in.vPos, 1.f);
    if (g_iAnim)
    {
        Skinning(_in.vPos, PrevPos.xyz, _in.vTangent, _in.vBitangent, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosProj = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    // Motion Blur
    if (g_bMotionBlur)
    {
        PrevPos = mul(PrevPos, g_matPrevWV);
        PrevPos = mul(PrevPos, g_matProj);

        float2 velocity = (output.vPosProj.xy / output.vPosProj.w) - (PrevPos.xy / PrevPos.w);
        output.vMotionVector.xy = velocity * 0.5f;
        output.vMotionVector.y *= -1.f;
        output.vMotionVector.z = output.vPosProj.z;
        output.vMotionVector.w = output.vPosProj.w;
    }
    else
    {
        output.vMotionVector = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    output.vUV0 = _in.vUV0;
    
    output.vPosWorld = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vTangentWorld = normalize(mul(float4(_in.vTangent, 0.f), g_matWorldInvTranspose).xyz);
    output.vBitangentWorld = normalize(mul(float4(_in.vBitangent, 0.f), g_matWorldInvTranspose).xyz);
    output.vNormalWorld = normalize(mul(float4(_in.vNormal, 0.f), g_matWorldInvTranspose).xyz);
    
    return output;
}

PS_IN VS_Std3D_Deferred_Inst(VS_IN _in)
{
    PS_IN output = (PS_IN) 0.f;

    float4 PrevPos = float4(_in.vPos, 1.f);
    if (g_iAnim)
    {
        Skinning(_in.vPos, PrevPos.xyz, _in.vTangent, _in.vBitangent, _in.vNormal
              , _in.vWeights, _in.vIndices, _in.iRowIndex);
    }
        
    output.vPosProj = mul(float4(_in.vPos, 1.f), _in.matWorld);
    output.vPosProj = mul(output.vPosProj, _in.matViewProj);
    
    // Motion Blur
    if (_in.iMotionBlur)
    {
        PrevPos = mul(PrevPos, _in.matPrevTransform);

        float2 velocity = (output.vPosProj.xy / output.vPosProj.w) - (PrevPos.xy / PrevPos.w);
        output.vMotionVector.xy = velocity * 0.5f;
        output.vMotionVector.y *= -1.f;
        output.vMotionVector.z = output.vPosProj.z;
        output.vMotionVector.w = output.vPosProj.w;
    }
    else
    {
        output.vMotionVector = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    output.vUV0 = _in.vUV0;
    
    output.vPosWorld = mul(float4(_in.vPos, 1.f), _in.matWorld).xyz;
    output.vTangentWorld = normalize(mul(float4(_in.vTangent, 0.f), _in.matWorldInvTranspose).xyz);
    output.vBitangentWorld = normalize(mul(float4(_in.vBitangent, 0.f), _in.matWorldInvTranspose).xyz);
    output.vNormalWorld = normalize(mul(float4(_in.vNormal, 0.f), _in.matWorldInvTranspose).xyz);
    
    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vPosition : SV_Target1;
    float4 vNormal : SV_Target2;
    float4 vTangent : SV_Target3;
    float4 vBitangent : SV_Target4;
    float4 vEmissive : SV_Target5;
    float4 vDiffuse : SV_Target6;
    float4 vSpecular : SV_Target7;
};

PS_OUT PS_Std3D_Deferred(PS_IN _in) : SV_Target
{
    PS_OUT output = (PS_OUT) 0.f;

    // Color
    float4 vOutColor = MtrlAlbedo;
    if (g_btex_0)
    {
        vOutColor *= g_tex_0.Sample(g_LinearWrapSampler, _in.vUV0);
    }
    
    // Normal 
    float3 vWorldNormal = _in.vNormalWorld;
    if (g_btex_1)
    {
        float3 vNormal = g_tex_1.Sample(g_LinearWrapSampler, _in.vUV0).rgb;
        vNormal = vNormal * 2.f - 1.f;
                        
        float3 N = _in.vNormalWorld;
        float3 T = normalize(_in.vTangentWorld - dot(_in.vTangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        
        vWorldNormal = normalize(mul(vNormal.xyz, TBN));
    }
        
    output.vColor = vOutColor;
    output.vPosition = float4(_in.vPosWorld, 1.f);
    output.vNormal = float4(vWorldNormal, 1.f);
    output.vTangent = float4(_in.vTangentWorld, 1.f);
    output.vBitangent = float4(normalize(cross(_in.vNormalWorld.xyz, _in.vTangentWorld.xyz)), 1.f);
    output.vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    output.vDiffuse = MtrlDiffuse;
    output.vSpecular = MtrlSpecular;
        
    return output;
}

#endif