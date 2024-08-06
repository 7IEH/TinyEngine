#include "pch.h"
#include "CKirbyLadderUp.h"


CKirbyLadderUp::CKirbyLadderUp()
{
}

CKirbyLadderUp::~CKirbyLadderUp()
{
}

void CKirbyLadderUp::tick()
{
    // ������ �����ӿ� �����ִ� VeloCity�� �ʱ�ȭ
    PLAYERCTRL->VelocityCut(0.f);

    // �̵�
    PLAYERCTRL->AddVelocity(Vec3(0.f, 5.f, 0.f));

    Vec3 InputWorld = PLAYERCTRL->GetInputWorld();
    Vec3 UpDir = PLAYERFSM->GetLadderUpSightDir();

    // �Է��� �ִ� ���
    if (InputWorld.Length() != 0.f)
    {
        if (KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP)))
        {
            ChangeState(L"JUMP_START");
        }
        // �Է� ������ Up���� ���� �ƴ� ��� State�� Down���·� �ٲ۴�.
        else if(InputWorld.Dot(UpDir) < 0.f)
        {
            ChangeState(L"LADDER_DOWN");
        }
    }
     
    // �Է��� ���� ��� WAIT ���·� �ٲ۴�.
    else
    {
        ChangeState(L"LADDER_WAIT");
    }

}

void CKirbyLadderUp::Enter()
{
    PLAYERFSM->SetCollisionLadder(true);

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("LadderUp"), true, false, 1.5f);

    // �߷� ����
    m_SaveGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(0.f);

    // Lock
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();

    /*Vec3 LadderPos = PLAYERFSM->GetLadderBottom();
    LadderPos.y = PLAYER->Transform()->GetWorldPos().y;

    PLAYER->Transform()->SetWorldPos(LadderPos);*/
}

void CKirbyLadderUp::Exit()
{
    PLAYERFSM->SetCollisionLadder(false);

    PLAYERCTRL->SetGravity(m_SaveGravity);

    // Unlock
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
}
