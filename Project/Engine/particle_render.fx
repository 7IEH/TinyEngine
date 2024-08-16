#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "global.hlsli"
#include "struct.hlsli"

StructuredBuffer<tParticle> g_ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule> g_ParticleModule : register(t21);

#define Particle g_ParticleBuffer[_in.iInstID]

struct VS_Input
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

struct VS_Output
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

VS_Output VS_ParticleRender(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.iInstID = _in.iInstID;
    
    return output;
}

// Geometry Shader
// ������ ������ų �� �ִ� ���̴�
// 1. ��� ��ƼŬ�� ��Ȱ��ȭ ������ ���, �������� �������� �ܰ迡�� �ߴܽ�Ű��
// 2. ������ ������ ���Ǽ�
struct GS_Output
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

[maxvertexcount(12)]
void GS_ParticleRender(point VS_Output _in[1], inout TriangleStream<GS_Output> _OutStream)
{
    GS_Output output[4] = { (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f };
    GS_Output output_cross[4] = { (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f };
        
    // GS �� ����ϴ� ��ƼŬ ������ �����´�.    
    tParticle particle = g_ParticleBuffer[_in[0].iInstID];
    if (0 == particle.Active)
    {
        return;
    }
    
    // ��ƼŬ�� �߽����� ����� �̵�    
    float3 vWorldPos = particle.vWorldPos.xyz;
    
    // ��ƼŬ�� ViewSpace �󿡼��� �߽� ������ ���ϱ�
    float4 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
    
    // 0 -- 1	     
	// | \  |	     
	// 3 -- 2
    output[0].vPosition = float4((particle.vWorldScale.x * -0.5f), (particle.vWorldScale.y * 0.5f), 0.f, 1.f);
    output[1].vPosition = float4((particle.vWorldScale.x * 0.5f), (particle.vWorldScale.y * 0.5f), 0.f, 1.f);
    output[2].vPosition = float4((particle.vWorldScale.x * 0.5f), (particle.vWorldScale.y * -0.5f), 0.f, 1.f);
    output[3].vPosition = float4((particle.vWorldScale.x * -0.5f), (particle.vWorldScale.y * -0.5f), 0.f, 1.f);
          
    output_cross[0].vPosition = float4((particle.vWorldScale.x * -0.5f), 0.f, (particle.vWorldScale.y * 0.5f), 1.f);
    output_cross[1].vPosition = float4((particle.vWorldScale.x * 0.5f), 0.f, (particle.vWorldScale.y * 0.5f), 1.f);
    output_cross[2].vPosition = float4((particle.vWorldScale.x * 0.5f), 0.f, (particle.vWorldScale.y * -0.5f), 1.f);
    output_cross[3].vPosition = float4((particle.vWorldScale.x * -0.5f), 0.f, (particle.vWorldScale.y * -0.5f), 1.f);
    
    output_cross[0].vUV = output[0].vUV = float2(0.f, 0.f);
    output_cross[1].vUV = output[1].vUV = float2(1.f, 0.f);
    output_cross[2].vUV = output[2].vUV = float2(1.f, 1.f);
    output_cross[3].vUV = output[3].vUV = float2(0.f, 1.f);
       
    
    // ������� ���
    if (g_ParticleModule[0].arrModuleCheck[6])
    {
        // �ӵ��� ���� ���� ���
        if (g_ParticleModule[0].VelocityAlignment)
        {
            float3 vR = normalize(mul(float4(particle.vVelocity.xyz, 0.f), g_matView).xyz);
            float3 vF = normalize(cross(vR, float3(0.f, 1.f, 0.f)));
            float3 vU = normalize(cross(vF, vR));
            
            float3x3 vRot =
            {
                vR,
                vU,
                vF
            };
            
            for (int i = 0; i < 4; ++i)
            {
                output[i].vPosition.xyz = mul(output[i].vPosition.xyz, vRot);
                output_cross[i].vPosition.xyz = mul(output_cross[i].vPosition.xyz, vRot);
            }
        }
    }
    
    // View ��ǥ�� �̵�, ������� ����
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos.xyz;
        output[i].vPosition = mul(output[i].vPosition, g_matProj);
        
        output_cross[i].vPosition.xyz += vViewPos.xyz;
        output_cross[i].vPosition = mul(output_cross[i].vPosition, g_matProj);
        
        output[i].iInstID = _in[0].iInstID;
        output_cross[i].iInstID = _in[0].iInstID;
    }
      
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
        
    if (g_ParticleModule[0].arrModuleCheck[6])
    {
        // �ӵ��� ���� ���� ���
        if (g_ParticleModule[0].VelocityAlignment)
        {
            _OutStream.Append(output_cross[0]);
            _OutStream.Append(output_cross[2]);
            _OutStream.Append(output_cross[3]);
            _OutStream.RestartStrip();
    
            _OutStream.Append(output_cross[0]);
            _OutStream.Append(output_cross[1]);
            _OutStream.Append(output_cross[2]);
            _OutStream.RestartStrip();
        }
    }
}

// ==================================================
// Render
// ==================================================

float4 PS_ParticleRender(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // ��� ����
    float4 vOutColor = particle.vColor;
    vOutColor.a = 1.f;
    
    if (g_btex_0)
    {
        float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
        vOutColor.rgb *= vSampleColor.rgb;
        vOutColor.a = vSampleColor.a;
    }
    
    // ��������� ���� ������
    if (module.arrModuleCheck[6])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vOutColor;
}

#define GlowEnable g_int_0
#define GlowThreshold g_float_0
#define GlowColor g_vec4_0

struct PS_Std2D_Glow_Output
{
    float4 RenderTargetColor : SV_Target0;
    float4 BloomTextureColor : SV_Target1;
};

PS_Std2D_Glow_Output PS_ParticleRender_Glow(GS_Output _in)
{
    PS_Std2D_Glow_Output output;
    
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // ��� ����
    float4 vOutColor = particle.vColor;
    vOutColor.a = 1.f;
    
    if (g_UseAnim2D)
    {
        float2 vUV = float2(0.0, 0.0);
        
        if (g_UseBackGround)
        {
            float2 vBackgroundLeftTop = g_vLeftTop + (g_vSliceSize / 2.f) - (g_vBackGround / 2.f);
            vBackgroundLeftTop -= g_vOffset;
            vUV = vBackgroundLeftTop + (g_vBackGround * _in.vUV);
        }
        else
        {
            float2 LT = g_vLeftTop - g_vOffset;
            vUV = LT + (g_vSliceSize * _in.vUV);
        }
        
        if (vUV.x < g_vLeftTop.x || (g_vLeftTop.x + g_vSliceSize.x) < vUV.x
            || vUV.y < g_vLeftTop.y || (g_vLeftTop.y + g_vSliceSize.y) < vUV.y)
        {
            discard;
        }
        else
        {
            float4 vSampleColor = g_anim2d_tex.Sample(g_LinearWrapSampler, vUV);
            vOutColor.rgb *= vSampleColor.rgb;
            vOutColor.a = vSampleColor.a;
        }
    }
    else
    {
        if (g_btex_0)
        {
            float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
            vOutColor.rgb *= vSampleColor.rgb;
            vOutColor.a = vSampleColor.a;
        }
        else
        {
            discard;
        }
    }
        
    if (0.1f >= vOutColor.a)
        discard;
    
    // Relative Luminance : �ȼ��� ���� ������ ��ο����� ���ذ�
    float RelativeLuminance = dot(vOutColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
        
    // ���� GlowThreshold �̻��� ��� ������ �������� ����ŷ
    if (GlowEnable && RelativeLuminance > GlowThreshold)
    {
        output.BloomTextureColor = GlowColor;
    }
    else
    {
        output.BloomTextureColor = float4(0.f, 0.f, 0.f, 1.f);
    }
  
    // ��������� ���� ������
    if (module.arrModuleCheck[6])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    output.RenderTargetColor = vOutColor;
    
    return output;
}

#define LerpRatio g_float_1
#define InitFireColor g_vec4_1
#define EndFireColor g_vec4_2

float4 PS_ParticleRender_Fire(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // Age �� ���� �� ���� �����Ͽ� ���� ����
    float4 vOutColor = (float4) 0.f;
    vOutColor.rgb = lerp(InitFireColor.rgb, EndFireColor.rgb, pow(abs(particle.NormalizeAge), 1.f / LerpRatio));
    vOutColor.a = 1.f;
    
    // Texture ����
    if (g_btex_0)
    {
        // �¿� ����
        if (0 == _in.iInstID % 2)
        {
            _in.vUV.x = 1.f - _in.vUV.x;
        }
        
        float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
        vOutColor.rgb *= vSampleColor.rgb;
        vOutColor.a = vSampleColor.a;
    }

    // ��������� ���� ������
    if (module.arrModuleCheck[6])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vOutColor;
}

#endif