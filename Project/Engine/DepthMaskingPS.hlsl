#include "global.hlsli"
#include "struct.hlsli"
#include "func.hlsli"

#define RenderTex g_tex_0
#define DepthOnlyTex g_tex_1
#define DepthMaskingTex g_tex_2

#define FarZ g_float_0
#define PowPower g_float_1

float4 main(PS_IN input) : SV_TARGET
{
    float3 color = g_btex_0 ? RenderTex.Sample(g_LinearClampSampler, input.vUV0).rgb : (float3) 0.f;
    float DepthMaskingViewZ = g_btex_2 ? TexcoordToView(DepthMaskingTex, input.vUV0).z : 0.f;
    DepthMaskingViewZ /= FarZ;
    
    float bias = 0.1f;
    
    // ����ŷ �Ǿ��������� �κ��� ���� �������� ������
    if (1.f - DepthMaskingViewZ < bias)
    {
        return float4(color, 1.f);
    }
    
    float DepthOnlyViewZ = g_btex_1 ? TexcoordToView(DepthOnlyTex, input.vUV0).z : 0.f;
    DepthOnlyViewZ /= FarZ;
    
    // �������� ���� �κ��� ���� �������� ������
    if (DepthMaskingViewZ == DepthOnlyViewZ)
    {
        return float4(color, 1.f);
    }
    
    // �������� �Ÿ��� �������� ���ϰ� ǥ��
    float weight = DepthMaskingViewZ;
    color *= pow(abs(weight), PowPower);
    
    return float4(color, 1.f);
}