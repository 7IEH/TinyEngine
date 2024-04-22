#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"

#include "CLevel.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : CComponent(_Type)
    , m_Mesh(nullptr)
    , m_SharedMtrl(nullptr)
    , m_DynamicMtrl(nullptr)
    , m_CurMtrl(nullptr)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& origin)
    : CComponent(origin)
    , m_Mesh(origin.m_Mesh)
    , m_SharedMtrl(origin.m_SharedMtrl)
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
    //CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    //if (!(pCurLevel->GetState() == LEVEL_STATE::PLAY || pCurLevel->GetState() == LEVEL_STATE::SIMULATE))
    //    return nullptr;

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

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef<CMesh>(m_Mesh.Get(), _File);
    SaveAssetRef<CMaterial>(m_SharedMtrl.Get(), _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    LoadAssetRef<CMesh>(m_Mesh, _File);
    LoadAssetRef<CMaterial>(m_SharedMtrl, _File);

    SetMaterial(m_SharedMtrl);
}
