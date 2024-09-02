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
    CKirbyFSM* KirbyFSM = CPlayerMgr::GetPlayerFSM();
    KirbyFSM->GetCurAbility()->SlideStart();

    // State Change
    switch (PLAYERFSM->GetCurAbilityIdx())
    {
    case AbilityCopyType::NORMAL:
    case AbilityCopyType::FIRE:
    case AbilityCopyType::CUTTER: {
        if (PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"SLIDE");
        }
    }
    break;
    case AbilityCopyType::SWORD: {
        if (PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"SLIDE");
        }
    }
    break;
    }
}

void CKirbySlideStart::Enter()
{
    GamePlayStatic::Play2DSound(L"sound\\wav\\HeroBasic\\Sliding.wav", 1, KIRBY_EFFECTSOUND);

    // �ִϸ��̼� ���
    CKirbyFSM* KirbyFSM = CPlayerMgr::GetPlayerFSM();
    KirbyFSM->GetCurAbility()->SlideStartEnter();
    
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    // Ŀ�� ǥ�� �ٲٱ� 
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);
}

void CKirbySlideStart::Exit()
{
    CKirbyFSM* KirbyFSM = CPlayerMgr::GetPlayerFSM();
    KirbyFSM->GetCurAbility()->SlideStartExit();

    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    // Ŀ�� ǥ�� �ٲٱ�
    CPlayerMgr::SetPlayerFace(FaceType::Normal);
}
