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
    int LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    // ������ ������ ����(Ÿ���� ������ ���Ƶ��̱� ������ ����)���� �浹�� ���
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
        return;
    }

    // monster : ������ ����
    if (LayerIdx == LAYER_MONSTER)
    {
        Vec3 HitDir = (_OtherCollider->Transform()->GetWorldPos() - Transform()->GetWorldPos()).Normalize();
        UnitHit HitInfo = {DAMAGE_TYPE::NORMAL, HitDir, 5.f, 0.f, 0.f};
        CUnitScript* pMonster = _OtherCollider->GetOwner()->GetScript<CUnitScript>();

        if (!pMonster)
            return;
        pMonster->GetDamage(HitInfo);
        ((CUnitScript*)PLAYERUNIT)->AttackReward();
    }
}

void CKirbyBodyCollider::OnTriggerStay(CCollider* _OtherCollider)
{
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
        return;
    }
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
