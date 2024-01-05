#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos;    // ������ ��ǥ
    Vec3 vNormal; // ������ ��ǥ
    Vec4 vColor;  // ������ ���� ����
    Vec2 vUV;     // UV ��ǥ�� of Texture Coordinate
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

// ==================
// ������� ���� ����ü
// ==================
struct tTransform
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
    Vec4 vAmb;  // ǥ���� ���� �󸶳� �����Ŀ� ���� ���� �����ϴ� ��
    Vec4 vDiff; // ����� ������� ��ü ��ü�� �����ִ� ����
    Vec4 vSpec; // �������κ��� �ݻ�Ǵ� ���� �󸶳� �޴��Ŀ� ���� �����Ǵ� ��
    Vec4 vEmv;
};

struct tMtrlConst
{
    tMtrlData mtrl; // float shininess; // ���� �󸶳� ���� ������ ����ϴ� ��

    int arrInt[4];
    float arrFloat[4];
    Vec2 arrVec2[4];
    Vec4 arrVec4[4];
    Matrix arrMat[4];

    int bTex[TEX_PARAM::END - TEX_PARAM::TEX_0];

    int iPadding[2];
};

struct tLightData
{
    Vec3 color; // Color
    float fallOffStart;
    Vec3 direction;
    float fallOffEnd;
    Vec3 position;
    float spotPower;

    // ��ü�� �������κ��� �Ÿ������� ���� �޴¾��� �޶���
    // fallOffStart, fallOffEnd �ɼǰ����� ǥ��

    UINT LightType;
    Vec3 padd;
};

struct tGlobal
{
    tLightData Lights[MAX_LIGHTS];

    Vec3 eyeWorld;
    bool UseTexture;

    Vec3 rimColor;
    float rimPower;

    Vec2 resolution;
    float NormalLineScale;
    bool UseRim;

    float Bloom_Threshold;
    float Bloom_Strength;
    bool DrawAsWireFrame;
    float CurTime;

    Vec4 pickingColor;
};

struct tAnimData2D
{
    Vec2 vLeftTop;
    Vec2 vSlizeSize;
    Vec2 vOffset;
    int UseAnim2D;
    int iPadding;
};

struct tMeshData
{
    std::vector<Vtx> vertices;
    std::vector<UINT> indices;

    std::string AmbientTextureFilename;
    std::string AoTextureFilename; // Ambient Occlusion
    std::string NormalTextureFilename;
    std::string HeightTextureFilename;
    std::string MetallicTextureFilename;
    std::string RoughnessTextureFilename;
    std::string EmissiveTextureFilename;

    std::string RelativeTextureFilePath; // ��ü��� = ������/�����/�����̸� - ����ο� �ش�Ǵ� ���ڿ�
};

extern tTransform g_Transform;
extern tGlobal g_Global;
