#include "pch.h"
#include "CKirbyFallDetectTrigger.h"

#include "CPlayerMgr.h"
#include "CKirbyFSM.h"

CKirbyFallDetectTrigger::CKirbyFallDetectTrigger()
    : CScript(KIRBYFALLDETECTTRIGGER)
{
}

CKirbyFallDetectTrigger::CKirbyFallDetectTrigger(const CKirbyFallDetectTrigger& _Origin)
    : CScript(KIRBYFALLDETECTTRIGGER)
{
}

CKirbyFallDetectTrigger::~CKirbyFallDetectTrigger()
{
}


void CKirbyFallDetectTrigger::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetLayerIdx() == LAYER_PLAYER)
    {
        // Player�� �浹�ߴٸ� FAll���·� �ٲ۴�.
        PLAYERFSM->ChangeState(L"FALL");
    }
}


UINT CKirbyFallDetectTrigger::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CKirbyFallDetectTrigger::LoadFromLevelFile(FILE* _File)
{
    return 0;
}