#ifndef _LIGHT
#define _LIGHT

#include "struct.hlsli"
#include "global.hlsli"

#define MtrlAlbedo g_vAlbedo
#define MtrlDiffuse g_vDiffuse
#define MtrlSpecular g_vSpecular

// =======================================================================================
// 3D LIGHT
// =======================================================================================

// BlinnPhong : Phong �𵨿��� halfway�� �̿��ؼ� �ӵ��� �ø� ��
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float shininess = 1.f;
    float3 specular = MtrlSpecular.rgb * pow(max(hdotn, 0.0f), shininess * 2.0);

    return MtrlAlbedo.rgb + (MtrlDiffuse.rgb + specular) * lightStrength;
}

// Ambient + Lambert's law ����� diffuse + ������ ������ ���� ���� Specular
float3 Phong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
    float shininess = 1.f;
    float3 r = -reflect(lightVec, normal);
    float3 specular = MtrlSpecular.rgb * pow(max(dot(toEye, r), 0.0f), shininess);
    return MtrlAlbedo.rgb + (MtrlDiffuse.rgb + specular) * lightStrength;
}

// �¾�� ���� ���� �ָ��ִ� ����
float3 ComputeDirectionalLight(tLightInfo L, float3 normal, float3 toEye)
{
    float3 lightVec = -L.vWorldDir;

    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.vRadiance.rgb * ndotl;

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
        float3 lightStrength = L.vRadiance.rgb * ndotl;

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
        float3 lightStrength = L.vRadiance.rgb * ndotl;

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
    rim = pow(abs(rim), RimPower);
    float3 strength = float3(1, 1, 1); // strength option
    return rim * RimColor * strength;
}


void CalculateLight3D(int _LightIdx, float3 _vWorldPos, float3 _vWorldNormal, float3 _Diffuse, float3 _Specular, inout tLightInfo _LightColor)
{
    // ������ ������ Ȯ��
    tLightInfo info = g_Light3D[_LightIdx];
    
    float3 lightVec = (float3) 0.f;
    float ndotl = (float) 0.f;
    float3 lightStrength = (float3) 0.f;
    
    if (LIGHT_DIRECTIONAL == info.LightType)
    {
        lightVec = -info.vWorldDir;
        ndotl = max(dot(lightVec, _vWorldNormal), 0.0f);
        lightStrength = info.vRadiance.rgb * ndotl;
    }
    else if (LIGHT_POINT == info.LightType)
    {
        float3 lightVec = info.vWorldPos - _vWorldPos;

        float d = length(lightVec);
        lightVec /= d;

        if (d < info.fallOffEnd)
        {
            float ndotl = max(dot(lightVec, _vWorldNormal), 0.0f);
            float3 lightStrength = info.vRadiance.rgb * ndotl;

            float att = CalcAttenuation(d, info.fallOffStart, info.fallOffEnd);
            lightStrength *= att;
        }
    }
    else if (LIGHT_SPOT == info.LightType)
    {
        float3 lightVec = info.vWorldPos - _vWorldPos;

        float d = length(lightVec);
        lightVec /= d;

        if (d < info.fallOffEnd)
        {
            float ndotl = max(dot(lightVec, _vWorldNormal), 0.0f);
            float3 lightStrength = info.vRadiance.rgb * ndotl;

            float att = CalcAttenuation(d, info.fallOffStart, info.fallOffEnd);
            lightStrength *= att;

            float spotFactor = pow(max(-dot(lightVec, info.vWorldDir), 0.0f), info.spotPower);
            lightStrength *= spotFactor;
        }
    }
    
    // BlinnPhong
    float3 toEye = normalize(g_eyeWorld - _vWorldPos);
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, _vWorldNormal);
    float shininess = 1.f;
    float3 specular = _Specular * pow(max(hdotn, 0.0f), shininess * 2.0);
    
    _LightColor.vRadiance.rgb += (_Diffuse + specular) * lightStrength;
}

// =======================================================================================
// 2D LIGHT
// =======================================================================================

void CalLight2D(float3 _WorldPos, uint _LightIdx, inout tLightInfo _output)
{
    // ���� �����ų ������ ����
    tLightInfo info = g_Light2D[_LightIdx];
    
    if (LIGHT_DIRECTIONAL == info.LightType)
    {
        _output.vRadiance += info.vRadiance;
    }
    else if (LIGHT_POINT == info.LightType)
    {
        float fAttenu = 1.f;
        
        float fDist = distance(info.vWorldPos.xy, _WorldPos.xy);
        if (fDist < info.fRadius)
        {
            float fTheta = (fDist / info.fRadius) * (PI / 2.f);
            fAttenu = saturate(cos(fTheta));
            
            //if (g_int_0)
            //{
            //    float fTheta = (fDist / info.fRadius) * (PI / 2.f);
            //    fAttenu = saturate(cos(fTheta));
            //}
            //else
            //{
            //    fAttenu = saturate(1.f - fDist / g_Light2D[_LightIdx].fRadius);
            //}
            
            _output.vRadiance += info.vRadiance * fAttenu;
        }
    }
    else if (LIGHT_SPOT == info.LightType)
    {
        float2 LightToPixel = normalize(_WorldPos.xy - info.vWorldPos.xy);
        float Theta = dot(info.vWorldDir.xy, LightToPixel);
        float Degree = acos(Theta);
        
        if (Degree < radians(info.fAngle / 2.f))
        {
            float fAttenu = 1.f;
        
            float fDist = distance(info.vWorldPos.xy, _WorldPos.xy);
            if (fDist < info.fRadius)
            {
                float fTheta = (fDist / info.fRadius) * (PI / 2.f);
                fAttenu = saturate(cos(fTheta));
            
                _output.vRadiance += info.vRadiance * fAttenu;
            }
        }
    }
}

#endif