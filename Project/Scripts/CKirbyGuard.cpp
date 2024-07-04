#include "pch.h"
#include "CKirbyGuard.h"


CKirbyGuard::CKirbyGuard()
{
}

CKirbyGuard::~CKirbyGuard()
{
}
void CKirbyGuard::tick()
{
    if (KEY_TAP_ARROW || KEY_PRESSED_ARROW)
    {
        ChangeState(L"DODGE_START");
    }

    else if (KEY_TAP(KEY_JUMP) || KEY_PRESSED(KEY_JUMP))
    {
        ChangeState(L"SLIDE_START");
    }

    else if (KEY_RELEASED(KEY_GUARD) || KEY_NONE(KEY_GUARD))
    {
        ChangeState(L"IDLE");
    }
}

void CKirbyGuard::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"Guard"), true);

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyGuard::Exit()
{
    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

}
