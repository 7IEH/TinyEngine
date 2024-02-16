#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"
#include "CScript.h"

#include "CRenderMgr.h"

CCollider2D::CCollider2D()
    : CComponent(COMPONENT_TYPE::COLLIDER2D)
    , m_vOffsetPos(Vec3())
    , m_vOffsetScale(Vec3(1.f, 1.f, 1.f))
    , m_CollisionCount(0)
    , m_bAbsolute(false)
    , m_Type(COLLIDER2D_TYPE::RECT)
    , m_fRadius(0.f)
{
    SetColliderType(m_Type);
}

CCollider2D::CCollider2D(const CCollider2D& origin)
    : CComponent(origin)
    , m_vOffsetPos(origin.m_vOffsetPos)
    , m_vOffsetScale(origin.m_vOffsetScale)
    , m_bAbsolute(origin.m_bAbsolute)
    , m_CollisionCount(0)
    , m_Type(origin.m_Type)
    , m_fRadius(origin.m_fRadius)
    , m_BoundingBox(origin.m_BoundingBox)
    , m_BoundingSphere(origin.m_BoundingSphere)
{
}


CCollider2D::~CCollider2D()
{
}

void CCollider2D::finaltick()
{
    // �浹ü�� ���� ��� ���
    m_matColWorld = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
    m_matColWorld *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);

    // �浹ü�� �����ų GameObject �� �������
    const Matrix& matObjWorld = Transform()->GetWorldMat();

    // GameObject �� Scale �� ������ ���� �������� ���
    if (m_bAbsolute)
    {
        Vec3 vWorldScale = Transform()->GetWorldScale();
        Matrix matScaleInv = XMMatrixScaling(vWorldScale.x, vWorldScale.y, vWorldScale.z);
        matScaleInv = XMMatrixInverse(nullptr, matScaleInv);

        // Object �� Scale �� ������� ���̿� ��ġ�ؼ� ��ȿȭ ��Ŵ
        m_matColWorld = m_matColWorld * matScaleInv * matObjWorld;
    }
    else
    {
        m_matColWorld *= matObjWorld;
    }

    if (CRenderMgr::GetInst()->IsShowCollider())
    {
        // �浹���̸� Red, �浹�ϰ� ���� ������ Green
        if (m_Type == COLLIDER2D_TYPE::RECT)
        {
            if (0 == m_CollisionCount)
                GamePlayStatic::DrawDebugRect(m_matColWorld, Vec3(0.f, 1.f, 0.f), false);
            else
                GamePlayStatic::DrawDebugRect(m_matColWorld, Vec3(1.f, 0.f, 0.f), false);

            // 3D
            // if (0 == m_CollisionCount)
            //     GamePlayStatic::DrawDebugBox(m_matColWorld, Vec3(0.f, 1.f, 0.f), false);
            // else
            //     GamePlayStatic::DrawDebugBox(m_matColWorld, Vec3(1.f, 0.f, 0.f), false);
        }
        else if (m_Type == COLLIDER2D_TYPE::CIRCLE)
        {
            if (0 == m_CollisionCount)
                GamePlayStatic::DrawDebugCircle(m_matColWorld.Translation(), m_fRadius, Vec3(0.f, 1.f, 0.f), false);
            else
                GamePlayStatic::DrawDebugCircle(m_matColWorld.Translation(), m_fRadius, Vec3(1.f, 0.f, 0.f), false);

            // 3D
            // if (0 == m_CollisionCount)
            //     GamePlayStatic::DrawDebugSphere(m_matColWorld.Translation(), m_fRadius, Vec3(0.f, 1.f, 0.f), false);
            // else
            //     GamePlayStatic::DrawDebugSphere(m_matColWorld.Translation(), m_fRadius, Vec3(1.f, 0.f, 0.f), false);
        }
    }

    // Bounding Area
    m_BoundingSphere.Center = m_matColWorld.Translation();
    if (m_Type == COLLIDER2D_TYPE::RECT)
    {
        // ��� ����
        XMVECTOR XMscale;
        XMVECTOR XMrot;
        XMVECTOR XMTr;

        XMMatrixDecompose(&XMscale, &XMrot, &XMTr, m_matColWorld);

        m_BoundingBox.Extents = Vec3(XMscale);
    }
    else if (m_Type == COLLIDER2D_TYPE::CIRCLE)
    {
        m_BoundingSphere.Radius = m_fRadius;
    }
}

void CCollider2D::SetColliderType(COLLIDER2D_TYPE _Type)
{
    m_Type = _Type;

    if (m_Type == COLLIDER2D_TYPE::RECT)
    {
        // ��� ����
        XMVECTOR XMscale;
        XMVECTOR XMrot;
        XMVECTOR XMTr;

        XMMatrixDecompose(&XMscale, &XMrot, &XMTr, m_matColWorld);

        m_BoundingBox = BoundingBox(Vec3(XMTr), Vec3(XMscale));
    }
    else if (m_Type == COLLIDER2D_TYPE::CIRCLE)
    {
        m_BoundingSphere = BoundingSphere(m_matColWorld.Translation(), m_fRadius);
    }
}

void CCollider2D::BeginOverlap(CCollider2D* _OtherCollider)
{
    ++m_CollisionCount;

    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->BeginOverlap(this, _OtherCollider->GetOwner(), _OtherCollider);
    }
}

void CCollider2D::Overlap(CCollider2D* _OtherCollider)
{
    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->Overlap(this, _OtherCollider->GetOwner(), _OtherCollider);
    }
}

void CCollider2D::EndOverlap(CCollider2D* _OtherCollider)
{
    --m_CollisionCount;

    const vector<CScript*>& vecScript = GetOwner()->GetScripts();
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        vecScript[i]->EndOverlap(this, _OtherCollider->GetOwner(), _OtherCollider);
    }
}

void CCollider2D::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _File);
    fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _File);
    fwrite(&m_bAbsolute, sizeof(bool), 1, _File);
    fwrite(&m_Type, sizeof(COLLIDER2D_TYPE), 1, _File);
    fwrite(&m_fRadius, sizeof(float), 1, _File);
    fwrite(&m_BoundingBox, sizeof(BoundingBox), 1, _File);
    fwrite(&m_BoundingSphere, sizeof(BoundingSphere), 1, _File);
}

void CCollider2D::LoadFromLevelFile(FILE* _File)
{
    fread(&m_vOffsetPos, sizeof(Vec3), 1, _File);
    fread(&m_vOffsetScale, sizeof(Vec3), 1, _File);
    fread(&m_bAbsolute, sizeof(bool), 1, _File);
    fread(&m_Type, sizeof(COLLIDER2D_TYPE), 1, _File);
    fread(&m_fRadius, sizeof(float), 1, _File);
    fread(&m_BoundingBox, sizeof(BoundingBox), 1, _File);
    fread(&m_BoundingSphere, sizeof(BoundingSphere), 1, _File);
}
