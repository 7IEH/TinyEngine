#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : CComponent(_Type)
    , m_Mesh(nullptr)
    , m_SharedMtrl(nullptr)
    , m_DynamicMtrl(nullptr)
    , m_CurMtrl(nullptr)
{
}

CRenderComponent::~CRenderComponent()
{
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
    // �̹� ���������� �����ϰ� ������ �װ� �ش�.
    if (nullptr != m_DynamicMtrl)
        return m_DynamicMtrl;

    // ���������� �ִٸ�
    if (nullptr != m_SharedMtrl)
    {
        // ���������� �����ؼ� ���������� ����� �װ� ���� ��������� �����Ѵ�.
        m_CurMtrl = m_DynamicMtrl = m_SharedMtrl->Clone();
        return m_DynamicMtrl;
    }

    return nullptr;
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
    // ������ ����Ǹ� ������ ���纻 �޾Ƶ� DynamicMaterial �� �����Ѵ�.
    m_CurMtrl = m_SharedMtrl = _Mtrl;
    m_DynamicMtrl = nullptr;
}

void CRenderComponent::RestoreMaterial()
{
    m_CurMtrl = m_SharedMtrl;
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef<CMesh>(m_Mesh.Get(), _File);
    SaveAssetRef<CMaterial>(m_SharedMtrl.Get(), _File);
    SaveAssetRef<CMaterial>(m_DynamicMtrl.Get(), _File);
    SaveAssetRef<CMaterial>(m_CurMtrl.Get(), _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    LoadAssetRef<CMesh>(m_Mesh, _File);
    LoadAssetRef<CMaterial>(m_SharedMtrl, _File);
    LoadAssetRef<CMaterial>(m_DynamicMtrl, _File);
    LoadAssetRef<CMaterial>(m_CurMtrl, _File);
}
