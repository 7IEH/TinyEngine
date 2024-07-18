#include "pch.h"
#include "CKirbyIdle.h"

CKirbyIdle::CKirbyIdle()
{
}

CKirbyIdle::~CKirbyIdle()
{
}

void CKirbyIdle::tick()
{
    // 각 State별로 수행할 작업 (상태전환 제외)
    PLAY_CURSTATE(Idle)

    // State Change
    if (PLAYERFSM->GetCurObjectIdx() != ObjectCopyType::NONE)
    {
        switch (PLAYERFSM->GetCurObjectIdx())
        {
        case ObjectCopyType::CONE: {
            if (KEY_TAP(KEY_ATK))
            {
                ChangeState(L"ATTACK_START");
            }
            else if ((KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP))) && (KEY_NONE(KEY_GUARD) || KEY_RELEASED(KEY_GUARD)))
            {
                ChangeState(L"JUMP_START");
            }
            else if (PLAYERFSM->GetYPressedTime() >= PLAYERFSM->GetDropCopyTime())
            {
                ChangeState(L"DROP_OBJECT_START");
            }
            else if (PLAYERCTRL->GetInput().Length() != 0.f)
            {
                ChangeState(L"RUN");
            }
        }
        break;
        case ObjectCopyType::STAIR:
        case ObjectCopyType::LIGHT:
            break;
        }
    }
    else
    {
        switch (PLAYERFSM->GetCurAbilityIdx())
        {
        case AbilityCopyType::FIRE:
        case AbilityCopyType::NORMAL: {
            if (KEY_TAP(KEY_ATK))
            {
                ChangeState(L"ATTACK_CHARGE1_START");
            }
            else if ((KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP))) && (KEY_NONE(KEY_GUARD) || KEY_RELEASED(KEY_GUARD)))
            {
                ChangeState(L"JUMP_START");
            }
            else if (KEY_TAP(KEY_GUARD) || KEY_PRESSED(KEY_GUARD))
            {
                ChangeState(L"GUARD");
            }
            else if (PLAYERFSM->GetYPressedTime() >= PLAYERFSM->GetDropCopyTime())
            {
                ChangeState(L"DROP_ABILITY");
            }
            else if (PLAYERCTRL->GetInput().Length() != 0.f)
            {
                ChangeState(L"RUN_START");
            }
        }
        break;

        case AbilityCopyType::RANGER:
            break;
        case AbilityCopyType::SWORD: {
            if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
            {
                UINT Combo = PLAYERFSM->GetComboLevel();

                if (Combo == 0)
                    ChangeState(L"ATTACK");
                else if (Combo == 1)
                    ChangeState(L"ATTACK_COMBO1");
                else if (Combo == 2)
                    ChangeState(L"ATTACK_COMBO2");
            }
            // else if (KEY_PRESSED(KEY_ATK))
            //{
            //     ChangeState(L"ATTACK_CHARGE1_START");
            // }
            else if ((KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP))) && (KEY_NONE(KEY_GUARD) || KEY_RELEASED(KEY_GUARD)))
            {
                ChangeState(L"JUMP_START");
            }
            else if (KEY_TAP(KEY_GUARD) || KEY_PRESSED(KEY_GUARD))
            {
                ChangeState(L"GUARD");
            }
            else if (PLAYERFSM->GetYPressedTime() >= PLAYERFSM->GetDropCopyTime())
            {
                ChangeState(L"DROP_ABILITY");
            }
            else if (PLAYERCTRL->GetInput().Length() != 0.f)
            {
                ChangeState(L"RUN_START");
            }
        }
        break;
        }
    }
}

void CKirbyIdle::Enter()
{
    PLAY_CURSTATE(IdleEnter)

    PLAYERCTRL->LockMove();
    PLAYERFSM->SetDroppable(true);
}

void CKirbyIdle::Exit()
{
    PLAY_CURSTATE(IdleExit)

    PLAYERCTRL->UnlockMove();
    PLAYERFSM->SetDroppable(false);
}