#include "pch.h"
#include "CKirbyBodyCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyVacuumCollider.h"
#include "CKirbyUnitScript.h"

CKirbyBodyCollider::CKirbyBodyCollider()
    : CScript(KIRBYBODYCOLLIDER)
{
}

CKirbyBodyCollider::~CKirbyBodyCollider()
{
}

void CKirbyBodyCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    int Layeridx = _OtherCollider->GetOwner()->GetLayerIdx();

    // ������ ������ ����(Ÿ���� ������ ���Ƶ��̱� ������ ����)���� �浹�� ���
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
    }
}

void CKirbyBodyCollider::OnTriggerStay(CCollider* _OtherCollider)
{
}

void CKirbyBodyCollider::OnTriggerExit(CCollider* _OtherCollider)
{
}

UINT CKirbyBodyCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CKirbyBodyCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}
