#include "struct.hlsli"
#include "global.hlsli"

float4 main(PS_IN input) : SV_TARGET
{
    if (!g_btex_0)
        discard;
    
    float4 color = g_tex_0.Sample(g_LinearSampler, input.vUV);

    float pixelX = 1.f / g_RenderResolution.x;
    float pixelY = 1.f / g_RenderResolution.y;
    
    float totalAlpha = 0.f;
    
    // 8���� Ž�� * Thickness
    for (int i = 1; i <= g_OutLineThickness_2D; ++i)
    {
        float4 pixelTop = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(0.f, pixelY * i));
        float4 pixelBottom = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(0.f, -pixelY * i));
        float4 pixelRight = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(pixelX * i, 0.f));
        float4 pixelLeft = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(-pixelX * i, 0.f));
        
        float4 pixelLT = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(-pixelX * i, pixelY * i));
        float4 pixelLB = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(-pixelX * i, -pixelY * i));
        float4 pixelRT = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(pixelX * i, pixelY * i));
        float4 pixelRB = g_tex_0.Sample(g_LinearSampler, input.vUV + float2(-pixelX * i, -pixelY * i));
        
        totalAlpha += pixelTop.a + pixelBottom.a + pixelRight.a + pixelLeft.a + pixelLT.a + pixelLB.a + pixelRT.a + pixelRB.a;
    }
    
    // 8���� Ž�� �� ����ִ� �����̶�� �ش� �ȼ��� ���ó��
    if (0.f >= totalAlpha)
        discard;
    
    // ���ø��ؿ� ������ ������ �־��ٸ� �ش� �������� ������ �� �ܿ��� �ܰ��� �������� ������
    if (0.f >= color.a)
        return g_OutLineColor_2D;
    else
        return color;
}