#include "pch.h"
#include "CKirbySlide.h"

CKirbySlide::CKirbySlide()
{
}

CKirbySlide::~CKirbySlide()
{
}

void CKirbySlide::tick()
{
    // �̵��� MoveController����

    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"SLIDE_END");
    }
}

void CKirbySlide::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"Slide"), false);

    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbySlide::Exit()
{
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}
