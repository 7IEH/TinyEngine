#include "pch.h"
#include "CKirbyDodgeStart.h"



CKirbyDodgeStart::CKirbyDodgeStart()
{
}

CKirbyDodgeStart::~CKirbyDodgeStart()
{
}

void CKirbyDodgeStart::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"DODGE1");
    }
}

void CKirbyDodgeStart::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"DodgeStart"), false, false, 2.f,  0);

    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyDodgeStart::Exit()
{
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}
