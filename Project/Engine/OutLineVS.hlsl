#include "struct.hlsli"
#include "global.hlsli"

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    float4 normal = float4(input.vNormal, 0.0);
    output.normalWorld = mul(normal, g_matWorldInvTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);    
    
    float4 pos = float4(input.vPos, 1.0);
    output.vPosProj = mul(pos, g_matWorld);
    output.vPosWorld = mul(pos, g_matWorld).xyz; // ���� ��ġ ���� ����
    
    // ī�޶���� �Ÿ��� thickness ������ �ܰ��� ���� ��ŭ ũ�� Ȯ��
    float dist = length(output.vPosWorld - g_eyeWorld);
    float thickness = 0.005;
    
    output.vPosProj += float4(output.normalWorld * dist * thickness, 0.0);
    output.vPosProj = mul(output.vPosProj, g_matView);
    output.vPosProj = mul(output.vPosProj, g_matProj);

    return output;
}