#include "pch.h"
#include "CKirbyFinalCutterEndAfter.h"



CKirbyFinalCutterEndAfter::CKirbyFinalCutterEndAfter()
{
}

CKirbyFinalCutterEndAfter::~CKirbyFinalCutterEndAfter()
{
}

void CKirbyFinalCutterEndAfter::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"IDLE");
    }
}

void CKirbyFinalCutterEndAfter::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("FinalCutterEndAfter"), false, false, 1.5f, 0);

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    // ���� ����
    PLAYERFSM->SetInvincible(true);

}

void CKirbyFinalCutterEndAfter::Exit()
{
    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    // ���� ����
    PLAYERFSM->SetInvincible(false);
}
