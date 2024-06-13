#include "struct.hlsli"
#include "global.hlsli"
#include "func.hlsli"

PS_IN main(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
   
    if (g_iAnim)
    {
        Skinning(input.vPos, input.vTangent, input.vBitangent, input.vNormal
              , input.vWeights, input.vIndices, 0);
    }
    
    float4 normal = float4(input.vNormal, 0.0);
    output.normalWorld = mul(normal, g_matWorldInvTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    float4 pos = float4(input.vPos, 1.0);
    output.vPosProj = mul(pos, g_matWorld);
    output.vPosWorld = mul(pos, g_matWorld).xyz; // ���� ��ġ ���� ����
    
    // ī�޶���� �Ÿ��� thickness ������ �ܰ��� ���� ��ŭ ũ�� Ȯ��
    float dist = length(output.vPosWorld - g_eyeWorld);
    float thickness = 0.005f;
    
    output.vPosProj += float4(output.normalWorld * dist * thickness, 0.0);
    output.vPosProj = mul(output.vPosProj, g_matView);
    output.vPosProj = mul(output.vPosProj, g_matProj);

    return output;
}