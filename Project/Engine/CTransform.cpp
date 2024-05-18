#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
    : CComponent(COMPONENT_TYPE::TRANSFORM)
    , m_vRelativePos(Vec3())
    , m_vRelativeScale(Vec3(1.f, 1.f, 1.f))
    , m_vRelativeRotation(Vec3())
    , m_arrLocalDir{}
    , m_arrWorldDir{}
    , m_matWorld()
    , m_Mobility(MOBILITY_TYPE::MOVABLE)
    , m_bAbsolute(true)
    , m_matTransformation()
{
}

CTransform::CTransform(const CTransform& origin)
    : CComponent(origin)
    , m_vRelativePos(origin.m_vRelativePos)
    , m_vRelativeScale(origin.m_vRelativeScale)
    , m_vRelativeRotation(origin.m_vRelativeRotation)
    , m_arrLocalDir{origin.m_arrLocalDir[0], origin.m_arrLocalDir[1], origin.m_arrLocalDir[2]}
    , m_arrWorldDir{origin.m_arrWorldDir[0], origin.m_arrWorldDir[1], origin.m_arrWorldDir[2]}
    , m_matWorld()
    , m_Mobility(origin.m_Mobility)
    , m_bAbsolute(origin.m_bAbsolute)
    , m_matTransformation()
{
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
    static const Vec3 BasisVector[3] = {Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 1.f)};

    // Vec3 �� Vec4 Ÿ������ Ȯ���ؼ� ����� ������Ѿ� ��
    // XMVector3TransformCoord	- w �� 1�� Ȯ��
    // XMVector3TransformNormal - w �� 0���� Ȯ��
    // mul(float4(_in.vPos, 1 or 0), g_matWorld);
    // ���� ���� ��������� �̵��� �������� ���� ����
    for (int i = 0; i < 3; ++i)
    {
        // m_matWorld ��Ŀ� ũ�������� ���� �� �ֱ� ������ �ٽ� ���̸� 1�� ����ȭ ��Ų��.
        m_arrLocalDir[i] = XMVector3TransformNormal(BasisVector[i], m_matWorld);
        m_arrWorldDir[i] = m_arrLocalDir[i].Normalize();
    }

    // �θ� ������Ʈ�� �ִٸ�
    if (GetOwner()->GetParent())
    {
        m_matTransformation = GetOwner()->GetParent()->Transform()->GetWorldMat();

        if (m_bAbsolute)
        {
            Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetRelativeScale();
            Matrix matParentScaleInv = XMMatrixScaling(1.f / vParentScale.x, 1.f / vParentScale.y, 1.f / vParentScale.z);

            // �θ��� ũ�� ��� ���
            m_matTransformation = matParentScaleInv * m_matTransformation;

            m_matWorld *= m_matTransformation;
        }
        else
        {
            m_matWorld *= m_matTransformation;
        }

        for (int i = 0; i < 3; ++i)
        {
            // �θ� ����� ����� ���� ��ķ� ���� ���
            m_arrWorldDir[i] = XMVector3TransformNormal(BasisVector[i], m_matWorld);
            m_arrWorldDir[i].Normalize();
        }
    }
}

void CTransform::UpdateData()
{
    g_Transform.matWorld = m_matWorld;

    g_Transform.matWorldInvTranspose = g_Transform.matWorld;
    g_Transform.matWorldInvTranspose.Translation(Vec3(0.0f));
    g_Transform.matWorldInvTranspose = g_Transform.matWorldInvTranspose.Transpose().Invert();

    g_Transform.matWV = g_Transform.matWorld * g_Transform.matView;
    g_Transform.matWVP = g_Transform.matWV * g_Transform.matProj;

    // ��ġ������ Transform ������ۿ� ������, B0 �������Ϳ� ���ε�
    CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
    pCB->SetData(&g_Transform);
    pCB->UpdateData();
}

Vec3 CTransform::GetWorldScale() const
{
    CGameObject* pParent = GetOwner()->GetParent();
    Vec3 vWorldScale = m_vRelativeScale;

    bool bAbsolute = m_bAbsolute;

    while (pParent)
    {
        if (!bAbsolute)
            vWorldScale *= pParent->Transform()->GetRelativeScale();

        bAbsolute = pParent->Transform()->IsAbsolute();
        pParent = pParent->GetParent();
    }

    return vWorldScale;
}

Vec3 CTransform::GetWorldRotation() const
{
    CGameObject* pParent = GetOwner()->GetParent();
    Vec3 vWorldRot = m_vRelativeRotation;

    while (pParent)
    {
        vWorldRot += pParent->Transform()->GetRelativeRotation();
        pParent = pParent->GetParent();
    }

    return vWorldRot;
}

void CTransform::SetDirection(Vec3 _Dir)
{
    _Dir.Normalize();
    Vec3 vRight = Vec3(0.f, 1.f, 0.f).Cross(_Dir);
    if (0.f >= vRight.Length())
        vRight = Vec3(1.f, 0.f, 0.f);

    vRight.Normalize();
    Vec3 vUp = _Dir.Cross(vRight);

    Matrix matRot = XMMatrixIdentity();

    matRot._11 = vRight.x;
    matRot._12 = vRight.y;
    matRot._13 = vRight.z;
    matRot._21 = vUp.x;
    matRot._22 = vUp.y;
    matRot._23 = vUp.z;
    matRot._31 = _Dir.x;
    matRot._32 = _Dir.y;
    matRot._33 = _Dir.z;

    Vec3 vRot = DecomposeRotMat(matRot);
    SetRelativeRotation(vRot);
}

void CTransform::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_vRelativePos, sizeof(Vec3), 1, _File);
    fwrite(&m_vRelativeScale, sizeof(Vec3), 1, _File);
    fwrite(&m_vRelativeRotation, sizeof(Vec3), 1, _File);
    fwrite(&m_Mobility, sizeof(MOBILITY_TYPE), 1, _File);
    fwrite(&m_bAbsolute, sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
    fread(&m_vRelativePos, sizeof(Vec3), 1, _File);
    fread(&m_vRelativeScale, sizeof(Vec3), 1, _File);
    fread(&m_vRelativeRotation, sizeof(Vec3), 1, _File);
    fread(&m_Mobility, sizeof(MOBILITY_TYPE), 1, _File);
    fread(&m_bAbsolute, sizeof(bool), 1, _File);
}
