#include "pch.h"
#include "CKirbyAttackAirGuardEnd.h"

CKirbyAttackAirGuardEnd::CKirbyAttackAirGuardEnd()
    : m_PrevGravity(0.f)
{
}

CKirbyAttackAirGuardEnd::~CKirbyAttackAirGuardEnd()
{
}

void CKirbyAttackAirGuardEnd::tick()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        if (PLAYERCTRL->IsGround())
        {
            ChangeState(L"IDLE");
        }
        else
        {
            ChangeState(L"JUMP_FALL");
        }
    }
}

void CKirbyAttackAirGuardEnd::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("ButterflyScalesAttackEnd"), false, false, 1.5f);
    //@Effect 차지 파티클입자

    PLAYERCTRL->ClearVelocityY();
    m_PrevGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(0.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->LockMove();
}

void CKirbyAttackAirGuardEnd::Exit()
{
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    PLAYERCTRL->SetGravity(m_PrevGravity);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->UnlockMove();
}