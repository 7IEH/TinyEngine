#include "pch.h"
#include "CMeshCollider.h"

#include "CTransform.h"
#include "CMeshRender.h"

CMeshCollider::CMeshCollider()
    : CCollider(COMPONENT_TYPE::MESHCOLLIDER)
    , m_bConvex(false)
    , m_Mesh(nullptr)
{
}

CMeshCollider::~CMeshCollider()
{
}

void CMeshCollider::begin()
{
    CCollider::begin();
}

void CMeshCollider::finaltick()
{
    CCollider::finaltick();

    // �ݶ��̴� ��Ȱ��ȭ ���¿����� ������ X
    if (!m_bEnabled)
        return;

    if (nullptr != m_Mesh)
    {
        GamePlayStatic::DrawDebugMesh(Transform()->GetWorldMat(), m_Mesh, Vec3(0.f, 1.f, 0.f), true);
    }
}

void CMeshCollider::SetConvex(bool _bEnable)
{
    if (m_bConvex == _bEnable)
        return;

    m_bConvex = _bEnable;
    GamePlayStatic::Physics_Event(GetOwner(), Physics_EVENT_TYPE::RESPAWN);
}

void CMeshCollider::SetMesh(Ptr<CMesh> _pMesh)
{
    if (m_Mesh == _pMesh)
        return;

    m_Mesh = _pMesh;
    GamePlayStatic::Physics_Event(GetOwner(), Physics_EVENT_TYPE::RESPAWN);
}

UINT CMeshCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider::SaveToLevelFile(_File);
    fwrite(&m_bConvex, 1, sizeof(bool), _File);
    MemoryByte += SaveAssetRef(m_Mesh, _File);

    MemoryByte += sizeof(bool);

    return MemoryByte;
}

UINT CMeshCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider::LoadFromLevelFile(_File);
    fread(&m_bConvex, 1, sizeof(bool), _File);
    MemoryByte += LoadAssetRef(m_Mesh, _File);

    MemoryByte += sizeof(bool);

    return MemoryByte;
}
