#include "pch.h"
#include "CKirbyHoveringFall.h"

CKirbyHoveringFall::CKirbyHoveringFall()
    : m_SavedGravity(0.f)
    , m_SavedSpeed(0.f)
{
}

CKirbyHoveringFall::~CKirbyHoveringFall()
{
}

void CKirbyHoveringFall::tick()
{
    if (PLAYERFSM->GetYPressedTime() >= PLAYERFSM->GetDropCopyTime())
    {
        ChangeState(L"DROP_ABILITY");
    }
    else if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
    {
        ChangeState(L"HOVERING_SPIT");
    }
    else if (PLAYERCTRL->IsGround())
    {
        ChangeState(L"HOVERING_LANDING");
    }
    else if (KEY_TAP(KEY_JUMP) || (KEY_PRESSED(KEY_JUMP)))
    {
        ChangeState(L"HOVERING");
    }
    else if (PLAYERFSM->GetHoveringAccTime() >= PLAYERFSM->GetHoveringLimitTime())
    {
        ChangeState(L"HOVERING_FALL_LIMIT");
    }
}

void CKirbyHoveringFall::Enter()
{
    GetOwner()->Animator()->Play(ANIMPREFIX("FlightFall"));
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyBig));
    
    if (PLAYERCTRL->GetVelocity().y > 0.f)
    {
        PLAYERCTRL->ClearVelocityY();
    }

    m_SavedGravity = PLAYERCTRL->GetGravity();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetGravity(-3.f);
    PLAYERCTRL->SetSpeed(PLAYERUNIT->GetInitInfo().Speed / 3.f);

    PLAYERFSM->SetDroppable(true);
    PLAYERFSM->SetHovering(true);
}

void CKirbyHoveringFall::Exit()
{
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));

    PLAYERCTRL->SetGravity(m_SavedGravity);
    PLAYERCTRL->SetSpeed(m_SavedSpeed);

    PLAYERFSM->SetDroppable(false);
    PLAYERFSM->SetHovering(false);
}