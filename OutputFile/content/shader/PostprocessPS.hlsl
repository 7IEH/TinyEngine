#include "global.hlsli"
#include "struct.hlsli"

float4 main(PS_IN input) : SV_TARGET
{
    float4 texColor = g_Missing_tex.Sample(g_LinearSampler, input.vUV);
    
    if (g_btex_1)
    {
        int width = 0;
        int height = 0;
        int numMips = 0;
        
        // 2�� ����ó��
        // ���ε��� �ؽ��簡 ������ ������ ��쿡�� ���ø� 
        g_tex_1.GetDimensions(0, width, height, numMips);
        if (!(0 == width || 0 == height))
            texColor = g_tex_1.Sample(g_PointSampler, input.vUV);
    }
    
    return texColor;
}