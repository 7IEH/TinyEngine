#include "struct.hlsli"
#include "global.hlsli"

float4 main(VS_OUT input) : SV_TARGET
{
    // 235, 162, 0 - ���� ��Ȳ��
    return float4(235.0 / 255, 162.0 / 255, 10.0 / 255, 1.0);
}