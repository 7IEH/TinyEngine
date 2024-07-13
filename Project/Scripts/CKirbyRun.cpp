#include "pch.h"
#include "CKirbyRun.h"

#define RUN_SPEED 10.f

CKirbyRun::CKirbyRun()
{
}

CKirbyRun::~CKirbyRun()
{
}

void CKirbyRun::tick()
{
    PLAY_CURSTATE(Run)

    // State Change
    if (PLAYERFSM->GetCurObjectIdx() != ObjectCopyType::NONE)
    {
    }
    else
    {
        switch (PLAYERFSM->GetCurAbilityIdx())
        {
        case AbilityCopyType::NORMAL: 
        {
            if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
            {
                ChangeState(L"ATTACK_CHARGE1_START");
            }
            else if (KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP)))
            {
                ChangeState(L"JUMP_START");
            }
            else if (PLAYERCTRL->GetInput().Length() == 0.f)
            {
                ChangeState(L"IDLE");
            }
            else if (KEY_TAP(KEY_GUARD) || KEY_PRESSED(KEY_GUARD))
            {
                ChangeState(L"GUARD");
            }
        }
        break;

        case AbilityCopyType::FIRE:
        case AbilityCopyType::RANGER:
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
            else if (KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP)))
            {
                ChangeState(L"JUMP_START");
            }
            else if (PLAYERCTRL->GetInput().Length() == 0.f)
            {
                ChangeState(L"IDLE");
            }
            else if (KEY_TAP(KEY_GUARD) || KEY_PRESSED(KEY_GUARD))
            {
                ChangeState(L"GUARD");
            }
        }
            break;
        }
    }
}

void CKirbyRun::Enter()
{
    PLAY_CURSTATE(RunEnter)
}

void CKirbyRun::Exit()
{
    PLAY_CURSTATE(RunExit)
}