#include "pch.h"
#include "CKirbyBurningStart.h"

CKirbyBurningStart::CKirbyBurningStart()
    : m_SaveRotSpeed(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurningStart::~CKirbyBurningStart()
{
}


void CKirbyBurningStart::tick()
{
    // ��ü �۶��̵� �ð��� ���� FSM�� Gliding �ð��� �����ش�.
    PLAYERFSM->AddGlidingTime(DT);

    // �߷°� ����
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 0)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }


    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"BURNING");
    }
}

void CKirbyBurningStart::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"BurningStart"), false, false, 1.5f);

    // �۶��̵� �ð� �ʱ�ȭ
    PLAYERFSM->ClearGlidingTime();

    // Movement
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetForwardMode(true);
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(0.5f);

    m_SaveSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(13.f);

    PLAYERCTRL->ClearVelocityY();

    //  ���� ����
    PLAYERFSM->SetInvincible(true);
}

void CKirbyBurningStart::Exit()
{
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);
    PLAYERCTRL->SetSpeed(m_SaveSpeed);

    //  ���� ����
    PLAYERFSM->SetInvincible(false);
}
