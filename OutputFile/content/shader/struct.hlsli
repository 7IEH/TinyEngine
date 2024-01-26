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

struct tLightColor
{
    float4 vColor; // ���� ����	- ������ ���� ����
    float4 vSpecular; // �ݻ籤	- ������ ��ü ǥ���� �ݻ�� ����
    float4 vAmbient; // ȯ�汤(�ֺ���) - ������ ���ؼ� ����Ǵ� �ּ����� ��
};

struct tLightInfo
{
    tLightColor ColorInfo;
    
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

struct tTileInfo
{
    float2 vLeftTopUV;
    int bRender;
    int padding;
};

struct tParticle
{
    float4 vWorldPos; // ��ġ
    float4 vWorldScale; // ũ��
    float4 vWorldRotation; // ȸ����
    float4 vVelocity; // �ӵ�
    float4 vColor; // ����

    float Mass; // ����
    float Age; // ���� ����
    float Life; // ����
    int Active; // Ȱ��ȭ, ��Ȱ��ȭ ����
};

struct tParticleModule
{
	// Sapwn ���
    float4 vSpawnColor; // �ʱ� �÷�
    float4 vSpawnMinScale; // �ʱ� �ּ� ũ��
    float4 vSpawnMaxScale; // �ʱ� �ִ� ũ��

    float MinLife; // �ּ� ����
    float MaxLife; // �ִ� ����
    int SpawnRate; // �ʴ� ���� ����
    int SpaceType; // ��ǥ��(0 : LocalSpace, 1 : WorldSpace)
};

#endif