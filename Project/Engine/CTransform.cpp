#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
    : CComponent(COMPONENT_TYPE::TRANSFORM)
    , m_vRelativeScale(Vec3(1.f, 1.f, 1.f))
    , m_bAbsolute(true)
{
    m_BoundingSphere = BoundingSphere();
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
    m_matWorld = XMMatrixIdentity();

    Matrix matScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

    Matrix matRotX = XMMatrixRotationX(m_vRelativeRotation.x);
    Matrix matRotY = XMMatrixRotationY(m_vRelativeRotation.y);
    Matrix matRotZ = XMMatrixRotationZ(m_vRelativeRotation.z);

    Matrix matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

    m_matWorld = matScale * matRotX * matRotY * matRotZ * matTranslation;

    // ��ü�� ���Ⱚ�� �ٽ� ����Ѵ�.
    m_arrLocalDir[(UINT)DIR_TYPE::RIGHT] = Vec3(1.f, 0.f, 0.f);
    m_arrLocalDir[(UINT)DIR_TYPE::UP] = Vec3(0.f, 1.f, 0.f);
    m_arrLocalDir[(UINT)DIR_TYPE::FRONT] = Vec3(0.f, 0.f, 1.f);

    // Vec3 �� Vec4 Ÿ������ Ȯ���ؼ� ����� ������Ѿ� ��
    // XMVector3TransformCoord	- w �� 1�� Ȯ��
    // XMVector3TransformNormal - w �� 0���� Ȯ��
    // mul(float4(_in.vPos, 1 or 0), g_matWorld);
    // ���� ���� ��������� �̵��� �������� ���� ����
    for (int i = 0; i < 3; ++i)
    {
        // m_matWorld ��Ŀ� ũ�������� ���� �� �ֱ� ������ �ٽ� ���̸� 1�� ����ȭ ��Ų��.
        m_arrLocalDir[i] = XMVector3TransformNormal(m_arrLocalDir[i], m_matWorld);
        m_arrWorldDir[i] = m_arrLocalDir[i].Normalize();
    }

    // �θ� ������Ʈ�� �ִٸ�
    if (GetOwner()->GetParent())
    {
        m_matParentMat = GetOwner()->GetParent()->Transform()->GetWorldMat();

        if (m_bAbsolute)
        {
            Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetRelativeScale();
            Matrix matParentScaleInv =
                XMMatrixScaling(1.f / vParentScale.x, 1.f / vParentScale.y, 1.f / vParentScale.z);
            m_matWorld = m_matWorld * matParentScaleInv * m_matParentMat;
        }
        else
        {
            m_matWorld *= m_matParentMat;
        }

        for (int i = 0; i < 3; ++i)
        {
            // m_matWorld ��Ŀ� ũ�������� ���� �� �ֱ� ������ �ٽ� ���̸� 1�� ����ȭ ��Ų��.
            m_arrWorldDir[i] = XMVector3TransformNormal(m_arrWorldDir[i], m_matWorld);
            m_arrWorldDir[i].Normalize();
        }
    }

    m_BoundingSphere.Center = Vec3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
    m_BoundingSphere.Radius = m_matWorld._22;
}

void CTransform::UpdateData()
{
    g_Transform.matWorld = m_matWorld;
    g_Transform.matWorldInvTranspose = m_matWorld;
    g_Transform.matWorldInvTranspose.Translation(Vec3(0.0f));
    g_Transform.matWorldInvTranspose = g_Transform.matWorldInvTranspose.Transpose().Invert();

    g_Transform.matWV = g_Transform.matWorld * g_Transform.matView;
    g_Transform.matWVP = g_Transform.matWV * g_Transform.matProj;

    // ��ġ������ Transform ������ۿ� ������, B0 �������Ϳ� ���ε� �ص�
    CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
    pCB->SetData(&g_Transform);
    pCB->UpdateData();
}