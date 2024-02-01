#ifndef _PARTICLE
#define _PARTICLe

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

[maxvertexcount(6)]
void GS_ParticleRender(point VS_Output _in[1], inout TriangleStream<GS_Output> _OutStream)
{
    GS_Output output[4] = { (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f };
    
    // GS �� ����ϴ� ��ƼŬ ������ �����´�.
    tParticle particle = g_ParticleBuffer[(int) _in[0].iInstID];
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
        
    // ������� ���
    if (g_ParticleModule[0].arrModuleCheck[6])
    {
        // �ӵ��� ���� ���� ���
        if (g_ParticleModule[0].VelocityAlignment)
        {
            // ��ƼŬ ���� ���� �ӵ��� View �������� ��ȯ
            float3 vVelocity = normalize(particle.vVelocity);
            vVelocity = mul(float4(vVelocity, 0.f), g_matView).xyz;
                       
            // ��ƼŬ Right ����� �̵� ������ �����ؼ� �� ������ ������ ���Ѵ�.
            float3 vRight = float3(1.f, 0.f, 0.f);
            float fTheta = acos(dot(vRight, vVelocity));
            
            // ������ ����� �ڻ��� ������ �������� �ϱ� ������, 2���� ���� �ݴ�� ������ �ش�.
            if (vVelocity.y < vRight.y)
            {
                fTheta = (2.f * PI) - fTheta;
            }
            
            // ���� ������ Z �� ȸ�� ����� �����.
            float3x3 matRotZ =
            {
                cos(fTheta), sin(fTheta),  0,
                -sin(fTheta), cos(fTheta), 0,
                      0,           0,      1.f,
            };
            
            // 4���� ������ ȸ����Ų��.
            for (int i = 0; i < 4; ++i)
            {
                output[i].vPosition.xyz = mul(output[i].vPosition.xyz, matRotZ);
            }
        }
    }
        
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos.xyz;
    }
    
    // ������� ����
    output[0].vPosition = mul(output[0].vPosition, g_matProj);
    output[1].vPosition = mul(output[1].vPosition, g_matProj);
    output[2].vPosition = mul(output[2].vPosition, g_matProj);
    output[3].vPosition = mul(output[3].vPosition, g_matProj);
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
}

float4 PS_ParticleRender(GS_Output _in) : SV_Target
{
    float4 vOutColor = g_ParticleBuffer[(uint) _in.iInstID].vColor;
    
    if (g_btex_0)
    {
        vOutColor *= g_tex_0.Sample(g_LinearSampler, _in.vUV);
    }
    
    return vOutColor;
}

#endif