#include "struct.hlsli"
#include "global.hlsli"
#include "func.hlsli"


#define FarZ 50000

float4 main(PS_IN _in) : SV_Target
{
    float3 color = g_postprocess_Tex.Sample(g_LinearClampSampler, _in.vUV0).rgb;

    //float MaskingColor = g_btex_0 ? KirbyMasking.Sample(g_LinearClampSampler, _in.vUV0).r : 0.f;
    
    float DepthMaskingViewZ = g_btex_0 ? TexcoordToView(DepthMaskingTex, _in.vUV0).z : 0.f;
    DepthMaskingViewZ /= FarZ;
    
    float bias = 0.1f;
    
    // ����ŷ �Ǿ��������� �κ��� ���� �������� ������
    if (1.f - DepthMaskingViewZ < bias)
    {
        color = color * 0.3f;
    }
   
    return float4(color, 1.f);
}