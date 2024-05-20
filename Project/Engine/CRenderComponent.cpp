#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"

#include "CLevel.h"
#include "CTransform.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : CComponent(_Type)
    , m_Mesh(nullptr)
    , m_SharedMtrl(nullptr)
    , m_DynamicMtrl(nullptr)
    , m_CurMtrl(nullptr)
    , m_bFrustumCheck(true)
    , m_BoundingRadius(1.f)
    , m_bCastShadow(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& origin)
    : CComponent(origin)
    , m_Mesh(origin.m_Mesh)
    , m_SharedMtrl(origin.m_SharedMtrl)
    , m_bFrustumCheck(origin.m_bFrustumCheck)
    , m_BoundingRadius(origin.m_BoundingRadius)
    , m_bCastShadow(origin.m_bCastShadow)
{
    // ���� ������Ʈ�� ���������� �����ϰ� �ְ�, ���� ��������� ���������� �ƴѰ��
    if (origin.m_SharedMtrl != origin.m_CurMtrl)
    {
        assert(origin.m_DynamicMtrl.Get());

        m_CurMtrl = m_DynamicMtrl = origin.m_DynamicMtrl->Clone();
    }
    else
    {
        m_CurMtrl = m_SharedMtrl;
    }
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
    // ������ ����Ǹ� ������ ���纻 �޾Ƶ� DynamicMaterial �� �����Ѵ�.
    m_CurMtrl = m_SharedMtrl = _Mtrl;
    m_DynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::CreateDynamicMaterial()
{
    //// ��Ÿ�� ���� ���� ���� ����
    // CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    // if (!(pCurLevel->GetState() == LEVEL_STATE::PLAY || pCurLevel->GetState() == LEVEL_STATE::SIMULATE))
    //     return nullptr;

    // �̹� ���������� �����Ѱ�� ����
    if (nullptr != m_DynamicMtrl)
        return m_DynamicMtrl;

    // ���������� �ִ� ��쿡�� ����
    if (nullptr != m_SharedMtrl)
    {
        // ���������� �����ؼ� ���������� ����� �װ� ���� ��������� �����Ѵ�.
        m_CurMtrl = m_DynamicMtrl = m_SharedMtrl->Clone();
        m_DynamicMtrl->SetName(m_DynamicMtrl->GetName() + L"_Inst");
        return m_DynamicMtrl;
    }

    return nullptr;
}

void CRenderComponent::RestoreMaterial()
{
    m_CurMtrl = m_SharedMtrl;
    m_DynamicMtrl = nullptr;
}

void CRenderComponent::finaltick()
{
    if (m_bFrustumCheck)
    {
        GamePlayStatic::DrawDebugSphere(Transform()->GetWorldPos(), m_BoundingRadius, Vec3(1.f, 1.f, 0.f), true);
    }
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef<CMesh>(m_Mesh.Get(), _File);
    SaveAssetRef<CMaterial>(m_SharedMtrl.Get(), _File);
    fwrite(&m_bFrustumCheck, 1, sizeof(bool), _File);
    fwrite(&m_BoundingRadius, 1, sizeof(float), _File);
    fwrite(&m_bCastShadow, 1, sizeof(bool), _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    LoadAssetRef<CMesh>(m_Mesh, _File);
    LoadAssetRef<CMaterial>(m_SharedMtrl, _File);
    fread(&m_bFrustumCheck, 1, sizeof(bool), _File);
    fread(&m_BoundingRadius, 1, sizeof(float), _File);
    fread(&m_bCastShadow, 1, sizeof(bool), _File);

    SetMaterial(m_SharedMtrl);
}
