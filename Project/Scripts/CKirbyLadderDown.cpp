#include "pch.h"
#include "CKirbyLadderDown.h"

CKirbyLadderDown::CKirbyLadderDown()
{
}

CKirbyLadderDown::~CKirbyLadderDown()
{
}

void CKirbyLadderDown::tick()
{
    // ������ �����ӿ� �����ִ� VeloCity�� �ʱ�ȭ
    PLAYERCTRL->VelocityCut(0.f);

    Vec3 InputWorld = PLAYERCTRL->GetInputWorld();
    Vec3 DownDir = PLAYERFSM->GetLadderDownSightDir();

    switch (m_DownState)
    {
    case DOWN_STATE::SLOW: {
        // �̵�
        PLAYERCTRL->AddVelocity(Vec3(0.f, -5.f, 0.f));

        // �Է��� �ִ� ���
        if (InputWorld.Length() != 0.f)
        {
            // �Է� ������ Down���� ���� �ƴ� ��� State�� UP���·� �ٲ۴�.
            if (InputWorld.Dot(DownDir) < 0.f)
            {
                ChangeState(L"LADDER_UP");
            }
        }
        // �Է��� ���� ��� WAIT ���·� �ٲ۴�.
        else
        {
            ChangeState(L"LADDER_WAIT");
        }

        if (PLAYER->Animator()->IsFinish())
        {
            m_DownState = DOWN_STATE::FAST;
            PLAYER->Animator()->Play(ANIMPREFIX("LadderDown"), true, false, 1.5f);
        }
    }
    break;

    case DOWN_STATE::FAST: {
        // �̵�
        PLAYERCTRL->AddVelocity(Vec3(0.f, -10.f, 0.f));

        // �Է��� �ִ� ���
        if (InputWorld.Length() != 0.f)
        {
            // �Է� ������ Down���� ���� �ƴ� ��� State�� UP���·� �ٲ۴�.
            if (InputWorld.Dot(DownDir) < 0.f)
            {
                ChangeState(L"LADDER_UP");
            }
        }
        // �Է��� ���� ��� WAITSTART ���·� �ٲ۴�.
        else
        {
            ChangeState(L"LADDER_WAITSTART");
        }
    }
    break;
    }

    // ���� ��´ٸ� Idle ���·� �����Ѵ�
    if (PLAYERCTRL->IsGround())
    {
        ChangeState(L"IDLE");
    }

}

void CKirbyLadderDown::Enter()
{
    PLAYERFSM->SetCollisionLadder(true);

    m_DownState = DOWN_STATE::SLOW;

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("LadderUp"), false, true, 1.5f);

    // �߷� ����
    m_SaveGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(0.f);

    // Lock
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();
}

void CKirbyLadderDown::Exit()
{
    PLAYERFSM->SetCollisionLadder(false);

    PLAYERCTRL->SetGravity(m_SaveGravity);

    // Unlock
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
}
