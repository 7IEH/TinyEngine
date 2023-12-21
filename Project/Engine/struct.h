#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos;      // ������ ��ǥ
    Vec3 vNormal;      // ������ ��ǥ
    Vec4 vColor;    // ������ ���� ����
    Vec2 vUV;       // UV ��ǥ�� of Texture Coordinate
};

// ==================
// ������� ���� ����ü
// ==================
struct tTransform
{
    Matrix matWorld;
    Matrix matWorldInv;

    Matrix matView;
    Matrix matViewInv;

    Matrix matProj;
    Matrix matProjInv;

    Matrix matWV;
    Matrix matWVP;
};

extern tTransform g_Transform;

struct FTask
{
    TASK_TYPE Type;
    UINT_PTR Param_1;
    UINT_PTR Param_2;
};
