#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos;     // ������ ��ǥ
    Vec3 vNormal;  // ������ ��ǥ
    Vec4 vColor;   // ������ ���� ����
    Vec2 vUV;      // UV ��ǥ�� of Texture Coordinate
    Vec3 vTangent; // ������ ��ǥ
};

struct tDebugShapeInfo
{
    DEBUG_SHAPE eShape;

    Vec3 vWorldPos;
    Vec3 vWorldScale;
    Vec3 vWorldRot;
    Matrix matWorld;

    Vec3 vColor;
    float fLifeTime;
    float fDuration;
    bool bDepthTest;
};

struct tLightInfo
{
    Vec4 vRadiance; // ���� ������ ����

    Vec3 vWorldPos; // ������ ��ġ
    Vec3 vWorldDir; // ������ ���ϴ� ����
    float fRadius;  // ������ ���� �ݰ�, �Ÿ� ����
    float fAngle;   // ������ ���� ����

    int LightType; // ���� Ÿ��

    // ��ü�� �������κ��� �Ÿ������� ���� �޴¾��� �޶���
    // fallOffStart, fallOffEnd �ɼǰ����� ǥ��
    float fallOffStart;
    float fallOffEnd;
    float spotPower;

    // �׸��� ������ 
    Matrix viewMat; 
    Matrix projMat; 
    Matrix invProj; // �׸��� ������ ������

    int ShadowType; // 0 : Static , 1 : Dynamic
    float HaloRadius;
    float HaloStrength;
    float Lightpadd;
};

// ==================
// ������� ���� ����ü
// ==================
__declspec(align(16)) struct tTransform
{
    Matrix matWorld;
    Matrix matWorldInv;
    Matrix matWorldInvTranspose;

    Matrix matView;
    Matrix matViewInv;

    Matrix matProj;
    Matrix matProjInv;

    Matrix matWV;
    Matrix matWVP;
};

// Material ���
struct tMtrlData
{
    Vec4 vAlbedo;
    Vec4 vDiffuse;
    Vec4 vSpecular;
    float vMetallic;
    float vRoughness;
    Vec2 mtrlpadd;
    Vec4 vEmission;
};

__declspec(align(16)) struct tMtrlConst
{
    tMtrlData mtrl;

    int arrInt[4];
    float arrFloat[4];
    Vec2 arrVec2[4];
    Vec4 arrVec4[4];
    Matrix arrMat[4];

    int bTex[TEX_PARAM::TEX_END];
};

__declspec(align(16)) struct tGlobalData
{
    Vec2 g_RenderResolution;
    Vec2 g_NoiseTexResolution;

    float g_dt;
    float g_time;
    UINT Light2DCount;
    UINT Light3DCount;

    Vec3 eyeWorld;
    int DrawAsWireFrame;
};

__declspec(align(16)) struct tAnimData2D
{
    Vec2 vLeftTop;
    Vec2 vSliceSize;
    Vec2 vBackGround;
    Vec2 vOffset;
    int UseAnim2D;
    int UseBackGround;
};

struct tMeshData
{
    std::vector<Vtx> vertices;
    std::vector<UINT> indices;

    string AmbientTextureFilename;
    string AoTextureFilename; // Ambient Occlusion
    string NormalTextureFilename;
    string HeightTextureFilename;
    string MetallicTextureFilename;
    string RoughnessTextureFilename;
    string EmissiveTextureFilename;

    string RelativeTextureFilePath; // ��ü��� = ������/�����/�����̸� - ����ο� �ش�Ǵ� ���ڿ�
};

struct tPixel
{
    BYTE r, g, b, a;
};

__declspec(align(16)) struct tParticle
{
    Vec4 vLocalPos;       // ���� ��ġ
    Vec4 vWorldPos;       // ���� ��ġ
    Vec4 vWorldRotation;  // ȸ����
    Vec4 vWorldInitScale; // �ʱ� ũ��
    Vec4 vWorldScale;     // ũ��
    Vec4 vVelocity;       // �ӵ�
    Vec4 vColor;          // ����
    Vec4 vForce;          // ���ڿ� ����� ���� �� �ѷ�
    Vec4 vNoiseForce;     // NoiseForce ���� ���� ���� ��

    float NoiseForceTime; // NoiseForce �� ���ù��� �ð�
    float NormalizeAge;   // Age �� Life �������� ����ȭ�� ��
    float Mass;           // ����
    float Age;            // ���� ����
    float Life;           // ����
    int Active;           // Ȱ��ȭ, ��Ȱ��ȭ ����
};

__declspec(align(16)) struct tParticleModule
{
    // Sapwn ���
    Vec4 vSpawnColor;    // �ʱ� �÷�
    Vec4 vSpawnMinScale; // �ʱ� �ּ� ũ��
    Vec4 vSpawnMaxScale; // �ʱ� �ִ� ũ��

    float MinLife;       // �ּ� ����
    float MaxLife;       // �ִ� ����
    float MinMass;       // �ּ� ����
    float MaxMass;       // �ִ� ����
    int SpawnRate;       // �ʴ� ���� ����
    int SpaceType;       // ��ǥ��(0 : LocalSpace, 1 : WorldSpace)
    int SpawnShape;      // ���� ����(0 : Sphere, 1 : Box)
    float Radius;        // SpawnShape �� Sphere �� ���, ������ ����
    Vec4 vSpawnBoxScale; // SpawnShape �� Box �� ���, Box �� ũ��

    // Drag
    float DragTime; // ���ӽ�Ű�µ� �ɸ��� �ð�

    // Scale
    Vec4 vScaleRatio;

    // Add Velocity
    int AddVelocityType; // 0 : From Center, 1: To Center, 2: Fix Direction
    float MinSpeed;
    float MaxSpeed;
    float FixedAngle;     // �ش� ���⿡�� �������� ����
    Vec4 vFixedDirection; // ���� ����

    // Noise Force
    float NoiseForceScale;
    float NoiseForceTerm;

    // Render
    int VelocityAlignment; // 1 : On, 0 : Off
    int AlphaBasedLife;    // 0 : off, 1 : NormalizedAge, 2: Max Age
    float AlphaMaxAge;

    // Module On / Off
    int arrModuleCheck[(UINT)PARTICLE_MODULE::END];
};

__declspec(align(16)) struct tSpawnCount
{
    int SpawnCount;
};

extern tTransform g_Transform;
extern tGlobalData g_Global;
