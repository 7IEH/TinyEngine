#include "global.hlsli"
#include "struct.hlsli"
#include "func.hlsli"

#define MotionVectorTex g_tex_0
#define Thresholud g_float_0

#define NumBlurSample 5

float4 main(PS_IN input) : SV_TARGET
{
    // ����� ������ �̹����� �����ɴϴ�. a ���п� Z ���� ����ȴ�.
    float4 Out = g_postprocess_Tex.Sample(g_LinearClampSampler, input.vUV0);

    // �ӵ� �ʿ��� �ӵ� ���Ϳ� Z ���� �����ɴϴ�.
    float4 Velocity = MotionVectorTex.Sample(g_LinearClampSampler, input.vUV0);
    if (dot(Velocity.xy, Velocity.xy) < Thresholud)
    {
        return float4(Out.rgb, 1.f);
    }
    
    Velocity.xy /= (float) NumBlurSample;

    int cnt = 1;
    float4 BColor = (float4) 0.f;

    [unroll]
    for (int i = cnt; i < NumBlurSample; i++)
    {
        float DepthZ = TexcoordToView(DepthOnlyTex, input.vUV0 + -Velocity.xy * (float) i).z;
    
        // �ӵ� ���� Z ���� �ӵ� ���� ������ �ؼ� ��ġ�� ���ø� �� ����� ������ �̹����� Z ���� ���մϴ�. (���� 1)
        if (Velocity.a < DepthZ + 0.04f)
        {
            // �ӵ� ���� ������ �ؼ� ��ġ�� �����Ͽ� ��� ������ �̹����� �� ������ ����ϴ�.
            BColor = g_postprocess_Tex.Sample(g_LinearClampSampler, input.vUV0 + -Velocity.xy * (float) i);
            
            cnt++;
            Out += BColor;
        }
        cnt++;
        Out += BColor;
    }

    Out /= (float) cnt;
      
    return float4(Out.rgb, 1.f);
}