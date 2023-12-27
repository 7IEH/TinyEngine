#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos;    // ������ ��ǥ
    Vec3 vNormal; // ������ ��ǥ
    Vec4 vColor;  // ������ ���� ����
    Vec2 vUV;     // UV ��ǥ�� of Texture Coordinate
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
    tMtrlData mtrl;

    int iArr[4];
    float fArr[4]; // float shininess; // ���� �󸶳� ���� ������ ����ϴ� ��
    Vec2 v2Arr[4];
    Vec4 v4Arr[4];
    Matrix matArr[4];

    int arrTex[(UINT)TEX_PARAM::END];

    int arrAnimData[2]; // 3D Animation ����
};

struct tLightData
{
    Vec3 strength; // Color
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
    int UseTexture;

    Vec3 rimColor;
    float rimPower;

    float NormalLineScale;

    int UseRim;
    bool DrawAsWireFrame;
    int padd;
};

struct tMeshData
{
    std::vector<Vtx> vertices;
    std::vector<UINT> indices;

    std::string AlbedoTextureFilename;
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
