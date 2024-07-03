#include "pch.h"
#include "CKirbyRunStart.h"

CKirbyRunStart::CKirbyRunStart()
{
}

CKirbyRunStart::~CKirbyRunStart()
{
}

void CKirbyRunStart::tick()
{
    PLAY_CURSTATE(RunStart)

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
            // ���º��� �ñ׳� �߻���
            if (KEY_TAP(KEY_ATK))
            {
                ChangeState(L"ATTACK_CHARGE1_START");
            }
            else if (KEY_RELEASED_ARROW || KEY_NONE_ARROW)
            {
                ChangeState(L"IDLE");
            }
            else if (KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP)))
            {
                ChangeState(L"JUMP_START");
            }

            // ���º��� �ñ׳� �߻� ���� ��
            else
            {
                if (GetOwner()->Animator()->IsFinish())
                    ChangeState(L"RUN");
            }
        }
        break;

        case AbilityCopyType::FIRE:
            break;
        case AbilityCopyType::RANGER:
            break;
        case AbilityCopyType::SWORD:
            break;
        }
    }
}

void CKirbyRunStart::Enter()
{
    PLAY_CURSTATE(RunStartEnter)
}

void CKirbyRunStart::Exit()
{
    PLAY_CURSTATE(RunStartExit)
}