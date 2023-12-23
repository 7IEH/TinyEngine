
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

struct tMaterial
{
    Vec3 ambient;
    float shininess;
    Vec3 diffuse;   
    float dummy1;                     
    Vec3 specular;
    float dummy2;                     
};

struct tLight
{
    Vec3 strength;
    float fallOffStart;
    Vec3 direction;
    float fallOffEnd;
    Vec3 position;
    float spotPower;
};

struct tGlobal
{
    tLight DirLight;
    tLight PointLight;
    tLight SpotLight;

    Vec4 eyeWorld;
};


extern tTransform g_Transform;
extern tGlobal g_Global;
