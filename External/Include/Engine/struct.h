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

struct tLightInfo
{
    Vec4 vColor;    // ���� ����	- ������ ���� ����
    Vec4 vSpecular; // �ݻ籤	- ������ ��ü ǥ���� �ݻ�� ����
    Vec4 vAmbient;  // ȯ�汤(�ֺ���) - ������ ���ؼ� ����Ǵ� �ּ����� ��

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

    int padding[2];
};

struct tGlobalData
{
    Vec2 g_RenderResolution;
    float g_dt;
    float g_time;

    UINT Light2DCount;
    UINT Light3DCount;
    float Bloom_Threshold;
    float Bloom_Strength;

    Vec4 pickingColor;

    Vec3 eyeWorld;
    int UseTexture;

    int UseRim;
    Vec3 rimColor;
    float rimPower;

    float NormalLineScale;
    int DrawAsWireFrame;

    float padding;
};

struct tAnimData2D
{
    Vec2 vLeftTop;
    Vec2 vSliceSize;
    Vec2 vBackGround;
    Vec2 vOffset;
    int UseAnim2D;
    int UseBackGround;
    Vec2 padding;
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

extern tTransform g_Transform;
extern tGlobalData g_Global;
