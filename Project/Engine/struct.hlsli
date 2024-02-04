#ifndef _STRUCT
#define _STRUCT

struct VS_IN
{
    float3 vPos : POSITION; // Sementic
    float3 vNormal : NORMAL;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    float3 vTangent : TANGENT;
};

struct PS_IN
{
    float4 vPosProj : SV_Position;
    float3 vPosWorld : POSITION;
    float3 normalWorld : NORMAL;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    float3 vTangentWorld : TANGENT;
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
    
    float4 vRadiance;
};

struct tTileInfo
{
    float2 vLeftTopUV;
    int bRender;
    int ImageIndex;
};

struct tParticle
{
    float4 vLocalPos; // ���� ��ġ
    float4 vWorldPos; // ���� ��ġ
    float4 vWorldRotation; // ȸ����
    float4 vWorldInitScale; // �ʱ� ũ��
    float4 vWorldScale; // ũ��
    float4 vVelocity; // �ӵ�
    float4 vColor; // ����
    float4 vForce; // ���ڿ� ����� ���� �� �ѷ�
    float4 vNoiseForce; // NoiseForce ���� ���� ���� ��

    float NoiseForceTime; // NoiseForce �� ���ù��� �ð�
    float NormalizeAge; // Age �� Life �������� ����ȭ�� ��
    float Mass; // ����
    float Age; // ���� ����
    float Life; // ����
    int Active; // Ȱ��ȭ, ��Ȱ��ȭ ����
    
    int2 particlepadding;
};

struct tParticleModule
{
    // Sapwn ���
    float4 vSpawnColor; // �ʱ� �÷�
    float4 vSpawnMinScale; // �ʱ� �ּ� ũ��
    float4 vSpawnMaxScale; // �ʱ� �ִ� ũ��

    float MinLife; // �ּ� ����
    float MaxLife; // �ִ� ����
    float MinMass; // �ּ� ����
    float MaxMass; // �ִ� ����
    int SpawnRate; // �ʴ� ���� ����
    int SpaceType; // ��ǥ��(0 : LocalSpace, 1 : WorldSpace
    int SpawnShape; // ���� ����(0 : Sphere, 1 : Box)
    float Radius; // SpawnShape �� Sphere �� ���, ������ ����
    float4 vSpawnBoxScale; // SpawnShape �� Box �� ���, Box �� ũ��
    
    // Scale
    float4 vScaleRatio;

    // Add Velocity
    int AddVelocityType; // 0 : From Center, 1: To Center, 2: Fix Direction
    float MinSpeed;
    float MaxSpeed;
    float FixedAngle; // �ش� ���⿡�� �������� ����
    float4 vFixedDirection; // ���� ����

    // Noise Force
    float NoiseForceScale;
    float NoiseForceTerm;
    
	// Render
    int VelocityAlignment; // 1 : On, 0 : Off
    int AlphaBasedLife; // 0 : off, 1 : NomrlizedAge, 2: Age
    float AlphaMaxAge;

    // Module On / Off
    int arrModuleCheck[7];
};

#endif