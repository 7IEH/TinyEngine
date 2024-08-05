#include "pch.h"
#include "CKirbyLadderWaitStart.h"


CKirbyLadderWaitStart::CKirbyLadderWaitStart()
{
}

CKirbyLadderWaitStart::~CKirbyLadderWaitStart()
{
}


void CKirbyLadderWaitStart::tick()
{
    // ������ �����ӿ� �����ִ� VeloCity�� �ʱ�ȭ
    PLAYERCTRL->VelocityCut(0.f);

    Vec3 InputWorld = PLAYERCTRL->GetInputWorld();
    Vec3 UpDir = PLAYERFSM->GetLadderUpSightDir();

    // �Է��� �ִ� ���
    if (InputWorld.Length() != 0.f)
    {
        // �Է� ������ Up���� ���� ��� State�� ��ٸ��� �ö󰡴� ���·� �ٲ۴�.
        if (InputWorld.Dot(UpDir) > 0.f)
        {
            ChangeState(L"LADDER_UP");
        }
        else
        {
            ChangeState(L"LADDER_DOWN");
        }
    }
    else if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"LADDER_WAIT");
    }
}

void CKirbyLadderWaitStart::Enter()
{
    PLAYERFSM->SetCollisionLadder(true);

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("LadderWaitStart"), false, false, 1.5f);

    // �߷� ����
    m_SaveGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(0.f);

    // Lock
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();
}

void CKirbyLadderWaitStart::Exit()
{
    PLAYERFSM->SetCollisionLadder(false);

    PLAYERCTRL->SetGravity(m_SaveGravity);

    // Unlock
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
}
