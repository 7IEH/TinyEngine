#include "pch.h"
#include "CKirbyBurning.h"

CKirbyBurning::CKirbyBurning()
    : m_SaveRotSpeed(0.f)
    , m_Duration(0.5f)
    , m_Acc(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurning::~CKirbyBurning()
{
}

void CKirbyBurning::tick()
{
    m_Acc += DT;

    // ��ü �۶��̵� �ð��� ���� FSM�� Gliding �ð��� �����ش�.
    PLAYERFSM->AddGlidingTime(DT);

    // �߷°� ����
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 1.f)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }

    // ���罺����Ʈ�� ���ѽð��� �Ѿ�ų�, ���� ����� ��� BurningEnd
    if (m_Acc > m_Duration)
    {
        if (KEY_TAP(KEY_ATK))
        {
            ChangeState(L"JUMP_FALL");
        }
        else
        {
            ChangeState(L"BURNING_END");
        }
    }
}

void CKirbyBurning::Enter()
{
    m_Acc = 0.f;

    // �ִϸ��̼� ���
    //PLAYER->Animator()->Play(ANIMPREFIX("Burning"), false, false, 1.5f);

    PLAYER->Animator()->Play(ANIMPREFIX("Wait"), true, false, 1.5f);

    // Movement
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetForwardMode(true);
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(0.5f);

    m_SaveSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(13.f);
}

void CKirbyBurning::Exit()
{
    // Movement
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);

    PLAYERCTRL->SetSpeed(m_SaveSpeed);
}
