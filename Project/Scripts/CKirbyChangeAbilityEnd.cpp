#include "pch.h"
#include "CKirbyChangeAbilityEnd.h"

CKirbyChangeAbilityEnd::CKirbyChangeAbilityEnd()
{
}

CKirbyChangeAbilityEnd::~CKirbyChangeAbilityEnd()
{
}

void CKirbyChangeAbilityEnd::tick()
{
    // State Change

    switch (PLAYERFSM->GetCurAbilityIdx())
    {
    case AbilityCopyType::FIRE:
    case AbilityCopyType::NORMAL:
    case AbilityCopyType::CUTTER:
    case AbilityCopyType::SWORD: {
        if (PLAYER->Animator()->IsFinish())
        {
            PLAYERFSM->SetGlobalState(false);
            ChangeState(L"IDLE");
        }
    }
    break;
    case AbilityCopyType::SLEEP: {
        if (PLAYER->Animator()->IsFinish())
        {
            PLAYERFSM->SetGlobalState(false);
            ChangeState(L"IDLE_START");
        }
    }
    break;
    }
}

void CKirbyChangeAbilityEnd::Enter()
{
    PLAYERFSM->SetGlobalState(true);
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("EvolutionCopyEnd"), false, false, 1.5f);
}

void CKirbyChangeAbilityEnd::Exit()
{
    // Ŀ�� ǥ�� ����
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));

    //@TODO
    // ī�޶� ���� ����
    // ��� �� ȿ�� ����
    // Ŀ�� ������ ���� ����
    // Ŀ�� �ǿ��� ������Ʈ �ٽ� DT�޵��� ����
}
