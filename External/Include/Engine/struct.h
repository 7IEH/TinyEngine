#pragma once

// 3���� ������ ��ġ�Ǵ� ����
struct Vtx
{
    Vec3 vPos;   // ������ ��ǥ
    Vec4 vColor; // ������ ���� ����
    Vec2 vUV;    // UV ��ǥ�� of Texture Coordinate
};

// ==================
// ������� ���� ����ü
// ==================
struct tTransform
{
    Matrix matWorld;
    Matrix matView;
    Matrix matProj;
};

extern tTransform g_Transform;

struct FTask
{
    TASK_TYPE Type;
    UINT_PTR Param_1;
    UINT_PTR Param_2;
};
