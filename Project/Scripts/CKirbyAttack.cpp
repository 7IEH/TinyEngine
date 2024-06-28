#include "pch.h"
#include "CKirbyAttack.h"

#include "CKirbyFSM.h"
#include "CPlayerMgr.h"
#include "CKirbyObject.h"
#include "CKirbyAbility.h"

CKirbyAttack::CKirbyAttack()
{
}

CKirbyAttack::~CKirbyAttack()
{
}

void CKirbyAttack::tick()
{
    wstring NextState = L"IDLE";

    CKirbyFSM* KirbyFSM = CPlayerMgr::GetPlayerFSM();

    if (KirbyFSM->GetCurObject())
    {
        KirbyFSM->GetCurObject()->Attack();
    }
    else
    {
        KirbyFSM->GetCurAbility()->Attack();
    }

    // �⺻������ �����ؾ� �ϴ� ����

    // Change State
    //if (KEY_TAP(KEY::Q))
    //    NextState = L"ATTACK_COMBO";

    //ChangeState(NextState);
}

void CKirbyAttack::Enter()
{
    PLAY_CURSTATE(AttackEnter)
}

void CKirbyAttack::Exit()
{
    PLAY_CURSTATE(AttackExit)
}