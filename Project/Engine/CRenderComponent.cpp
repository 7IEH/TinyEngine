#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"

#include "CLevel.h"
#include "CTransform.h"
#include "CAnimator.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : CComponent(_Type)
    , m_Mesh(nullptr)
    , m_vecMtrls{}
    , m_bEnabled(true)
    , m_bFrustumCheck(true)
    , m_BoundingRadius(1.f)
    , m_bCastShadow(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& origin)
    : CComponent(origin)
    , m_Mesh(origin.m_Mesh)
    , m_vecMtrls{}
    , m_bEnabled(origin.m_bEnabled)
    , m_bFrustumCheck(origin.m_bFrustumCheck)
    , m_BoundingRadius(origin.m_BoundingRadius)
    , m_bCastShadow(origin.m_bCastShadow)
{
    m_vecMtrls.resize(origin.m_vecMtrls.size());

    for (UINT i = 0; i < (UINT)origin.m_vecMtrls.size(); ++i)
    {
        m_vecMtrls[i].pCurMtrl = origin.m_vecMtrls[i].pCurMtrl;
        m_vecMtrls[i].pSharedMtrl = origin.m_vecMtrls[i].pSharedMtrl;

        // ���� ������Ʈ�� ���������� �����ϰ� �ְ�, ���� ��������� ���������� �ƴѰ��
        if (origin.m_vecMtrls[i].pSharedMtrl != origin.m_vecMtrls[i].pCurMtrl)
        {
            assert(origin.m_vecMtrls[i].pDynamicMtrl.Get());

            // ���� ���� ������Ʈ�� ������ ���������� �����Ѵ�.
            GetDynamicMaterial(i);

            // ���� ����������Ʈ�� �������� ���� ���� ������ ���������� �����Ѵ�.
            m_vecMtrls[i].pDynamicMtrl = origin.m_vecMtrls[i].pDynamicMtrl->Clone();
        }
        else
        {
            m_vecMtrls[i].pCurMtrl = m_vecMtrls[i].pSharedMtrl;
        }
    }
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMesh(Ptr<CMesh> _Mesh)
{
    m_Mesh = _Mesh;

    if (!m_vecMtrls.empty())
    {
        m_vecMtrls.clear();
        vector<tMtrlSet> vecMtrls;
        m_vecMtrls.swap(vecMtrls);
    }

    if (nullptr != m_Mesh)
    {
        m_vecMtrls.resize(m_Mesh->GetSubsetCount());
    }

    if (nullptr != GetOwner())
    {
        GetOwner()->DetachBoneSockets();

        if (nullptr != Animator())
        {
            Animator()->SetSkeletalMesh(m_Mesh);
        }
    }
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT _idx)
{
    return m_vecMtrls[_idx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _idx)
{
    // ���������� �������°����� ���� ��������� ������������ ȸ���ϵ��� �Ѵ�
    m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pSharedMtrl;

    if (m_vecMtrls[_idx].pDynamicMtrl.Get())
    {
        m_vecMtrls[_idx].pDynamicMtrl = nullptr;
    }

    return m_vecMtrls[_idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _idx)
{
    // ���� ������ ���� -> Nullptr ��ȯ
    if (nullptr == m_vecMtrls[_idx].pSharedMtrl)
    {
        m_vecMtrls[_idx].pCurMtrl = nullptr;
        m_vecMtrls[_idx].pDynamicMtrl = nullptr;
        return m_vecMtrls[_idx].pCurMtrl;
    }

    if (nullptr == m_vecMtrls[_idx].pDynamicMtrl)
    {
        m_vecMtrls[_idx].pDynamicMtrl = m_vecMtrls[_idx].pSharedMtrl->Clone();
        m_vecMtrls[_idx].pDynamicMtrl->SetName(m_vecMtrls[_idx].pSharedMtrl->GetName() + L"_Clone");
        m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pDynamicMtrl;
    }

    return m_vecMtrls[_idx].pCurMtrl;
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx)
{
    // ������ ����Ǹ� ������ ���纻 �޾Ƶ� DynamicMaterial �� �����Ѵ�.
    m_vecMtrls[_idx].pSharedMtrl = _Mtrl;
    m_vecMtrls[_idx].pCurMtrl = _Mtrl;
    m_vecMtrls[_idx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::GetMaterial(wstring _Name)
{
    for (UINT i = 0; i < m_Mesh->GetSubsetCount(); ++i)
    {
        if (_Name == m_Mesh->GetIBName(i))
        {
            return GetMaterial(i);
        }
    }

    return nullptr;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(wstring _Name)
{
    for (UINT i = 0; i < m_Mesh->GetSubsetCount(); ++i)
    {
        if (_Name == m_Mesh->GetIBName(i))
        {
            return GetSharedMaterial(i);
        }
    }

    return nullptr;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(wstring _Name)
{
    for (UINT i = 0; i < m_Mesh->GetSubsetCount(); ++i)
    {
        if (_Name == m_Mesh->GetIBName(i))
        {
            return GetDynamicMaterial(i);
        }
    }

    return nullptr;
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl, wstring _Name)
{
    for (UINT i = 0; i < m_Mesh->GetSubsetCount(); ++i)
    {
        if (_Name == m_Mesh->GetIBName(i))
        {
            SetMaterial(_Mtrl, i);
            return;
        }
    }
}

void CRenderComponent::SetMeshData(Ptr<CMeshData> _MeshData)
{
    SetMesh(_MeshData->GetMesh());

    const vector<Ptr<CMaterial>>& vecMtrl = _MeshData->GetVecMaterial();
    for (UINT i = 0; i < vecMtrl.size(); ++i)
    {
        SetMaterial(vecMtrl[i], i);
    }
}

ULONG64 CRenderComponent::GetInstID(UINT _iMtrlIdx)
{
    if (m_Mesh == nullptr || m_vecMtrls[_iMtrlIdx].pCurMtrl == nullptr)
        return 0;

    uInstID id{(UINT)m_Mesh->GetID(), (WORD)m_vecMtrls[_iMtrlIdx].pCurMtrl->GetID(), (WORD)_iMtrlIdx};
    return id.llID;
}

void CRenderComponent::finaltick()
{
    if (m_bFrustumCheck)
    {
        GamePlayStatic::DrawDebugSphere(Transform()->GetWorldPos(), m_BoundingRadius, Vec3(1.f, 1.f, 0.f), true);
    }
}

UINT CRenderComponent::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += SaveAssetRef<CMesh>(m_Mesh.Get(), _File);

    // ���� �������� ����
    UINT iMtrlCount = GetMtrlCount();
    fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        MemoryByte += SaveAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
    }

    fwrite(&m_bEnabled, 1, sizeof(bool), _File);
    fwrite(&m_bFrustumCheck, 1, sizeof(bool), _File);
    fwrite(&m_BoundingRadius, 1, sizeof(float), _File);
    fwrite(&m_bCastShadow, 1, sizeof(bool), _File);

    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(bool);
    MemoryByte += sizeof(bool);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(bool);

    return MemoryByte;
}

UINT CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += LoadAssetRef<CMesh>(m_Mesh, _File);

    // ���� �������� �ҷ�����
    UINT iMtrlCount = GetMtrlCount();
    fread(&iMtrlCount, sizeof(UINT), 1, _File);
    m_vecMtrls.resize(iMtrlCount);
    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        Ptr<CMaterial> pMtrl;
        MemoryByte += LoadAssetRef(pMtrl, _File);
        SetMaterial(pMtrl, i);
    }

    fread(&m_bEnabled, 1, sizeof(bool), _File);
    fread(&m_bFrustumCheck, 1, sizeof(bool), _File);
    fread(&m_BoundingRadius, 1, sizeof(float), _File);
    fread(&m_bCastShadow, 1, sizeof(bool), _File);

    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(bool);
    MemoryByte += sizeof(bool);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(bool);

    return MemoryByte;
}
