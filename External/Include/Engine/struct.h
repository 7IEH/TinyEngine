#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos; // ������ ��ǥ

    Vec3 vNormal;    // ���� ����
    Vec3 vTangent;   // ���� ����
    Vec3 vBiTangent; // ������ ����

    Vec4 vColor; // ������ ���� ����

    // UV ��ǥ�� of Texture Coordinate
    Vec2 vUV0;
    Vec2 vUV1;
    Vec2 vUV2;
    Vec2 vUV3;

    Vec4 vIndices; // Bone �ε���
    Vec4 vWeights; // Bone ����ġ
};

__declspec(align(16)) struct tLightInfo
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

    int ShadowIndex;
    float HaloRadius;
    float HaloStrength;
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
    Vec4 vTranslate;
    Vec4 vScale;
    Vec4 qRot;
};

struct tMTKeyFrame
{
    double dTime;
    int iFrame;
    Vec3 vTranslate;
    Vec3 vScale;
    Vec4 qRot;
};

struct tBoneSocket
{
    wstring SoketName;
    int BoneIndex;
    Vec3 RelativeLocation;
    Vec3 RelativeRotation;
    Vec3 RelativeScale;
    Matrix matSocket; // Bone Socket SRT
};

struct tMTBone
{
    wstring strBoneName;
    int iDepth;
    int iIdx;           // �� �ε���
    int iParentIdx;     // �θ� �� �ε���
    Matrix matOffset;   // Offset ���(�� -> ��Ʈ ������ ���)
    vector<tMTKeyFrame> vecKeyFrame;
    vector<tBoneSocket> vecBoneSocket; // BoneSocket
};

struct tMTAnimClip
{
    wstring strAnimName;
    int iStartFrame;
    int iEndFrame;
    int iFrameLength;

    double dStartTime;
    double dEndTime;
    double dTimeLength;

    FbxTime::EMode eMode;
};

// ===========
// Instancing
// ===========
union uInstID {
    struct
    {
        UINT iMesh;
        WORD iMtrl;
        WORD iMtrlIdx;
    };
    ULONG64 llID;
};

class CGameObject;
struct tInstObj
{
    CGameObject* pObj;
    UINT iMtrlIdx;
};

struct tInstancingData
{
    Matrix matWorld;
    Matrix matWorldInvTranspose;
    Matrix matView;
    Matrix matProj;
    int iRowIdx;
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

    // 3D Animation ����
    int arrAnimData[2];
};

__declspec(align(16)) struct tGlobalData
{
    Vec2 g_RenderResolution;
    Vec2 g_NoiseTexResolution;

    float g_DT;
    float g_Time;
    float g_EngineDT;
    float g_EngineTime;

    Vec3 g_eyeWorld;
    int g_EnableSSAO = false;

    UINT g_Light2DCount;
    UINT g_LightCount;

    int g_DrawAsWireFrame = false;
    int g_RenderOutline = true;
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
};

struct tPixel
{
    BYTE r, g, b, a;
};

// ���� ����ü
struct tRay
{
    Vec3 vStart;
    Vec3 vDir;
};

// Raycast ����� ���� ����ü
struct tRaycastOut
{
    Vec2 vUV0;
    float fDist;
    int bSuccess;
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

#include "CMesh.h"

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

    Ptr<CMesh> pMesh;
};
