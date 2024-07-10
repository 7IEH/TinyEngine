#include "pch.h"
#include "CKirbySlideStart.h"

CKirbySlideStart::CKirbySlideStart()
{
}

CKirbySlideStart::~CKirbySlideStart()
{
}

void CKirbySlideStart::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"SLIDE");
    }
}

void CKirbySlideStart::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"SlideStart"), false);
    
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbySlideStart::Exit()
{
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}
