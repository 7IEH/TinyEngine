#include "struct.hlsli"
#include "global.hlsli"
#include "disksample.hlsli"
#include "Light.hlsli"

#define BRDFTex g_BRDFTex // SpecularIBL Look-up Table
#define SpecularIBLTex g_SpecularCube
#define IrradianceIBLTex g_DiffuseCube 

#define AmbientTex g_tex_0
#define AOTex g_tex_1
#define NormalTex g_tex_2
#define MetallicRoughnessTex g_tex_4
#define EmissiveTex g_tex_5

#define MtrlAlbedo g_vAlbedo
#define MtrlMetallic g_vMetallic
#define MtrlRoughness g_vRoughness
#define MtrlEmission g_vEmission

#define InvertNormalMapY g_int_0

#define RIM_POWER g_float_1
#define RIM_COLOR g_vec4_0

static const float3 Fdielectric = 0.04; // ��ݼ�(Dielectric) ������ F0
static int ShadowLightCount = 3; // �׸��ڰ� ����� ������ �ִ밹��
static float LightRadiusScale = 0.01f;

// ���� ������ ���� ���̳� ��Ⱑ �޶� ��
float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
    //return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
}

float3 GetNormal(PS_IN input)
{
    float3 normalWorld = normalize(input.normalWorld);
    
    if (g_btex_2) // NormalWorld�� ��ü
    {
        float3 normal = NormalTex.Sample(g_LinearWrapSampler, input.vUV).rgb;
        normal = 2.0 * normal - 1.0; // ���� ���� [-1.0, 1.0]

        // OpenGL �� ��ָ��� ��쿡�� y ���� ����
        normal.y = InvertNormalMapY ? -normal.y : normal.y;
        
        float3 N = normalWorld;
        float3 T = normalize(input.vTangentWorld - dot(input.vTangentWorld, N) * N);
        float3 B = cross(N, T);
        
        // matrix�� float4x4, ���⼭�� ���� ��ȯ���̶� 3x3 ���
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normal, TBN));
    }
    
    return normalWorld;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic)
{
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic); // metallic�� Ŀ������ Diffuse�� ���ҵ� - ������ ���� ��Ģ
    float3 irradiance = IrradianceIBLTex.SampleLevel(g_LinearWrapSampler, normalWorld, 0).rgb;
    
    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness)
{
    float IBLRange = 5.f; // SpecularIBLTex.GetDimensions() Lod
    float IBLBias = 0.f;
    float lod = roughness * IBLRange + IBLBias;

    float2 specularBRDF = BRDFTex.SampleLevel(g_LinearClampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness), 0.0f).rg;
    float3 specularIrradiance = SpecularIBLTex.SampleLevel(g_LinearWrapSampler, reflect(-pixelToEye, normalWorld), lod).rgb;

    float3 F0 = lerp(Fdielectric, albedo, metallic); // metallic�� Ŀ������ albedo���� ������� - ������ ���� ��Ģ

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
                            float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    
    return (diffuseIBL + specularIBL) * ao;
}

// GGX/Towbridge-Reitz normal distribution function. // �̼� ǥ���� ���� ������ �븻�� ǥ���� ����
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float NdotH, float roughness, float alphaPrime)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaPrime * alphaPrime / (3.141592 * denom * denom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
// �̼�ǥ���� ���������� �κ��� ��� ���� �������� �����ٰ� ������ Masking �� ǥ���ϴ� ���������� ������ �Լ�
float SchlickGGX(float NdotI, float NdotO, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

#define LIGHT_NEAR_PLANE 1.f
#define LIGHT_FRUSTUM_WIDTH 3.4614f // Near 1.f Far 10000.f ���� 

// NdcDepthToViewDepth
float N2V(float ndcDepth, matrix invProj)
{
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}

float PCF_Filter(float2 uv, float zReceiverNdc, float filterRadiusUV, Texture2D shadowMap)
{
    float sum = 0.0f;
    for (int i = 0; i < 64; ++i)
    {
        float2 offset = diskSamples64[i] * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(
            g_ShadowCompareSampler, uv + offset, zReceiverNdc);
    }
    
    return sum / 64;
}

void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv,
                 float zReceiverView, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;
    
    // ���� Near Plane �������� Ž�� ����
    // UV ��ǥ ����
    float searchRadius = lightRadiusUV * (zReceiverView - LIGHT_NEAR_PLANE) / zReceiverView;

    float blockerSum = 0;
    numBlockers = 0;
    for (int i = 0; i < 64; ++i)
    {
        float shadowMapDepth =
            shadowMap.SampleLevel(g_ShadowPointSampler, float2(uv + diskSamples64[i] * searchRadius), 0).r;

        shadowMapDepth = N2V(shadowMapDepth, invProj);
        
        if (shadowMapDepth < zReceiverView)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    
    avgBlockerDepthView = blockerSum / numBlockers;
}

float PCSS(float2 uv, float zReceiverNdc, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float zReceiverView = N2V(zReceiverNdc, invProj); // Light �������� View ��ǥ�迡���� Z��
    
    // STEP 1: blocker search
    // �׸��ڰ� ���� �������� �ƴ��� �Ǻ�
    float avgBlockerDepthView = 0;
    float numBlockers = 0;

    FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap, invProj, lightRadiusWorld);

    if (numBlockers < 1)
    {
        // �׸��ڰ� �����ʴ� �����̴�
        return 1.f;
    }
    else
    {
        // STEP 2: penumbra size
        float penumbraRatio = (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView; // FindBlocker���� ã�� penumbra�� ����
        float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH; // Near Plane �������� Light�� ������
        float filterRadiusUV = penumbraRatio * lightRadiusUV * LIGHT_NEAR_PLANE / zReceiverView;

        // STEP 3: filtering
        return PCF_Filter(uv, zReceiverNdc, filterRadiusUV, shadowMap);
    }
}

float3 LightRadiance(tLightInfo light, float3 representativePoint, float3 posWorld, float3 normalWorld)
{
    // Default Point Light
    
    // Directional light
    float3 lightVec = light.LightType == LIGHT_DIRECTIONAL
                      ? -light.vWorldDir
                      : representativePoint - posWorld;

    float lightDist = length(lightVec);
    lightVec /= lightDist;

    // Spot light
    float spotFator = light.LightType == LIGHT_SPOT
                      ? pow(max(-dot(lightVec, light.vWorldDir), 0.0f), light.spotPower)
                      : 1.0f;
        
    // Distance attenuation
    float att = saturate((light.fallOffEnd - lightDist)
                         / (light.fallOffEnd - light.fallOffStart));
    
    // Shadow map
    float shadowFactor = 1.0;

    if (0 < ShadowLightCount && 1 == light.ShadowType)
    {
        const float nearZ = 1.f; // ī�޶� ������ ����
        
        // Project posWorld to light screen
        // light.viewProj ���
        float4 pos = float4(posWorld, 1.0);
        float4 lightScreen = mul(pos, light.viewMat);
        lightScreen = mul(lightScreen, light.projMat);
        lightScreen.xyz /= lightScreen.w;
        
        // ī�޶�(����)���� �� ���� �ؽ��� ��ǥ ���
        // [-1, 1]x[-1, 1] -> [0, 1]x[0, 1]
        float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y); // �ؽ��� ��ǥ�� NDC�� y�� �ݴ�
        lightTexcoord += 1.0;
        lightTexcoord *= 0.5;
        
        // PCSS
        float bias = 0.001f;
        if (3 == ShadowLightCount)
            shadowFactor = PCSS(lightTexcoord, lightScreen.z - bias, g_LightDepthMapTex1, light.invProj, light.fRadius * LightRadiusScale);
        else if (2 == ShadowLightCount)
            shadowFactor = PCSS(lightTexcoord, lightScreen.z - bias, g_LightDepthMapTex2, light.invProj, light.fRadius * LightRadiusScale);
        else if (1 == ShadowLightCount)
            shadowFactor = PCSS(lightTexcoord, lightScreen.z - bias, g_LightDepthMapTex3, light.invProj, light.fRadius * LightRadiusScale);
        
        ShadowLightCount -= 1;
    }

    float3 radiance = light.vRadiance.rgb * spotFator * att * shadowFactor;

    return radiance;
}

// PBR ��Ģ
// 1. microfacet ǥ�� ���� ������� �Ѵ�
// 2. �������� �����Ѵ�.
// 3. ������ ��� BRDF�� ����Ѵ�. // BRDF - Bidirectional reflectance distribution function
float4 main(PS_IN input) : SV_TARGET
{
    float3 pixelToEye = normalize(g_eyeWorld - input.vPosWorld);
    float3 normalWorld = GetNormal(input);
    
    float3 albedo = g_btex_0 ? AmbientTex.Sample(g_LinearWrapSampler, input.vUV).rgb 
                                 : MtrlAlbedo.rgb;
    float ao = g_btex_1 ? AOTex.Sample(g_LinearWrapSampler, input.vUV).r : 1.f - g_SSAOTex.Sample(g_LinearWrapSampler, input.vUV).r;
    float metallic = g_btex_4 ? MetallicRoughnessTex.Sample(g_LinearWrapSampler, input.vUV).b
                                    : MtrlMetallic;
    float roughness = g_btex_4 ? MetallicRoughnessTex.Sample(g_LinearWrapSampler, input.vUV).g 
                                      : MtrlRoughness;
    float3 emission = g_btex_5 ? EmissiveTex.Sample(g_LinearWrapSampler, input.vUV).rgb
                                     : MtrlEmission.rgb;

    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness);
    
    float3 directLighting = float3(0, 0, 0);

    for (uint i = 0; i < g_Light3DCount; ++i)
    {
        float3 lightVec = float3(0.f, 0.f, 0.f);
        float3 representativePoint = float3(0.f, 0.f, 0.f);
        
        if (g_Light3D[i].LightType == LIGHT_DIRECTIONAL)
        {
            lightVec = -g_Light3D[i].vWorldDir;
            representativePoint = g_Light3D[i].vWorldPos;
        }
        else if (g_Light3D[i].LightType == LIGHT_POINT)
        {
            // SphereLight 
            float3 L = g_Light3D[i].vWorldPos - input.vPosWorld;
            float3 r = normalize(reflect(g_eyeWorld - input.vPosWorld, normalWorld));
            float3 centerToRay = dot(L, r) * r - L;
            float LightRadius = g_Light3D[i].fRadius * LightRadiusScale;
            
            representativePoint = L + centerToRay * clamp(LightRadius / length(centerToRay), 0.0, 1.0);
            representativePoint += input.vPosWorld;
            lightVec = representativePoint - input.vPosWorld;
        }
        else if (g_Light3D[i].LightType == LIGHT_SPOT)
        {
            lightVec = g_Light3D[i].vWorldPos - input.vPosWorld;
            representativePoint = g_Light3D[i].vWorldPos;
        }
        
        float lightDist = length(lightVec);
        lightVec /= lightDist;
        float3 halfway = normalize(pixelToEye + lightVec);
        
        float NdotI = max(0.0, dot(normalWorld, lightVec));
        float NdotH = max(0.0, dot(normalWorld, halfway));
        float NdotO = max(0.0, dot(normalWorld, pixelToEye));

        float3 F0 = lerp(Fdielectric, albedo, metallic);
        float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
        float3 diffuseBRDF = kd * albedo;

        // Sphere Normalization
        float alpha = roughness * roughness;
        float alphaPrime = roughness;
        if (g_Light3D[i].LightType == LIGHT_POINT) // Point Light�� ��쿡�� Sphere Light ����
        {
            alphaPrime = saturate(alpha + g_Light3D[i].fRadius / (2.0 * lightDist));
        }
                
        float D = NdfGGX(NdotH, roughness, alphaPrime);
        float3 G = SchlickGGX(NdotI, NdotO, roughness);
        float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);

        float3 radiance = LightRadiance(g_Light3D[i], representativePoint, input.vPosWorld, normalWorld);
            
        directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
    }
    
    // Rim
    float3 toEye = normalize(g_eyeWorld - input.vPosWorld);
    float3 RimColor = RimLight(input.normalWorld, toEye, RIM_COLOR.rgb, RIM_POWER);
    
    float4 output = float4(0.f, 0.f, 0.f, 1.f);
    output = float4(ambientLighting + directLighting + emission + RimColor, 1.0);
    output = clamp(output, 0.0, 1000.0);

    return output;
}
