#ifndef _STRUCT
#define _STRUCT

struct VS_IN
{
    float3 vPos : POSITION; // Sementic
    float3 vNormal : NORMAL;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

struct PS_IN
{
    float4 vPosProj : SV_Position;
    float3 vPosWorld : POSITION;
    float3 normalWorld : NORMAL;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

struct tLightInfo
{
    float4 vColor; // ���� ����	- ������ ���� ����
    float4 vSpecular; // �ݻ籤	- ������ ��ü ǥ���� �ݻ�� ����
    float4 vAmbient; // ȯ�汤(�ֺ���) - ������ ���ؼ� ����Ǵ� �ּ����� ��

    float3 vWorldPos; // ������ ��ġ
    float3 vWorldDir; // ������ ���ϴ� ����
    float fRadius; // ������ ���� �ݰ�, �Ÿ� ����
    float fAngle; // ������ ���� ����

    int LightType; // ���� Ÿ��

    // ��ü�� �������κ��� �Ÿ������� ���� �޴¾��� �޶���
    // fallOffStart, fallOffEnd �ɼǰ����� ǥ��
    float fallOffStart;
    float fallOffEnd;
    float spotPower;
};

#endif