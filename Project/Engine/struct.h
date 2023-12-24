
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

struct FTask
{
    TASK_TYPE Type;
    UINT_PTR Param_1;
    UINT_PTR Param_2;
};

struct tMaterialData
{
    Vec3 ambient;       // ����� ������� ��ü ��ü�� �����ִ� ����
    float shininess;    // ���� �󸶳� ���� ������ ����ϴ� ��
    Vec3 diffuse;       // ǥ���� ���� �󸶳� �����Ŀ� ���� ���� �����ϴ� ��
    float dummy1;                     
    Vec3 specular;      // �������κ��� �ݻ�Ǵ� ���� �󸶳� �޴��Ŀ� ���� �����Ǵ� ��
    float dummy2;                     
};

struct tLightData
{
    Vec3 strength;
    float fallOffStart;    
    Vec3 direction;
    float fallOffEnd;
    Vec3 position;
    float spotPower;

    // ��ü�� �������κ��� �Ÿ������� ���� �޴¾��� �޶���
    // fallOffStart, fallOffEnd �ɼǰ����� ǥ��
};

struct tGlobal
{   
    tLightData DirLight;       // �¾�� ���� ���� �ָ��ִ� ����
    tLightData PointLight;     // �� ������ ���� ������������ ���� ������ ����
    tLightData SpotLight;      // ���� ������ �����־� ���� �߽����κ��� �����ڸ��� ������ ��ο����� ����

    Vec4 eyeWorld;

    float NormalLineScale;
    Vec3 padd;
};


extern tTransform g_Transform;
extern tGlobal g_Global;
