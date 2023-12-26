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

struct tMaterialData
{
    Vec3 ambient;    // ����� ������� ��ü ��ü�� �����ִ� ����
    float shininess; // ���� �󸶳� ���� ������ ����ϴ� ��
    Vec3 diffuse;    // ǥ���� ���� �󸶳� �����Ŀ� ���� ���� �����ϴ� ��
    float dummy1;
    Vec3 specular; // �������κ��� �ݻ�Ǵ� ���� �󸶳� �޴��Ŀ� ���� �����Ǵ� ��
    float dummy2;
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
    float fov;

    bool UseRim;
    Vec3 rimColor;
    float rimPower;

    float NormalLineScale;
    bool UseTexture;

    int width;

    // �ȼ������� �ܰ��� �β�
    Vec3 outlineColor;
    float thickness;
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
