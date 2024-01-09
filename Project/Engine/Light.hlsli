#ifndef _LIGHT
#define _LIGHT

#include "struct.hlsli"
#include "global.hlsli"

// BlinnPhong : Phong �𵨿��� halfway�� �̿��ؼ� �ӵ��� �ø� ��
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float shininess = 1.f;
    float3 specular = g_vSpec.rgb * pow(max(hdotn, 0.0f), shininess * 2.0);

    return g_vAmb.rgb + (g_vDiff.rgb + specular) * lightStrength;
}

// Ambient + Lambert's law ����� diffuse + ������ ������ ���� ���� Specular
float3 Phong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
    float shininess = 1.f;
    float3 r = -reflect(lightVec, normal);
    float3 specular = g_vSpec.rgb * pow(max(dot(toEye, r), 0.0f), shininess);
    return g_vAmb.rgb + (g_vDiff.rgb + specular) * lightStrength;
}

// �¾�� ���� ���� �ָ��ִ� ����
float3 ComputeDirectionalLight(tLightInfo L, float3 normal, float3 toEye)
{
    float3 lightVec = -L.vWorldDir;

    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.vColor.rgb * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye);
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

 // �� ������ ���� ������������ ���� ������ ����
float3 ComputePointLight(tLightInfo L, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.vWorldPos - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.vColor.rgb * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        return BlinnPhong(lightStrength, lightVec, normal, toEye);
    }
}

// ���� ������ �����־� ���� �߽����κ��� �����ڸ��� ������ ��ο����� ����
float3 ComputeSpotLight(tLightInfo L, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.vWorldPos - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= d;

        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.vColor.rgb * ndotl;

        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        float spotFactor = pow(max(-dot(lightVec, L.vWorldDir), 0.0f), L.spotPower);
        lightStrength *= spotFactor;

        return BlinnPhong(lightStrength, lightVec, normal, toEye);
    }
}

float3 RimLight(float3 NormalWorld, float3 toEye, float3 RimColor, float RimPower)
{
    float rim = 1.0 - dot(NormalWorld, toEye); // Fresnel's formulas
    rim = saturate(smoothstep(0.0, 1.0, rim)); // saturate() 0 ~ 1 Climp 
    rim = pow(abs(rim), g_rimPower);
    float3 strength = float3(1, 1, 1); // strength option
    return rim * g_rimColor * strength;
}

#endif