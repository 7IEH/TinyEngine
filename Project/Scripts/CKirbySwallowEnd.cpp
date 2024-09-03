#include "pch.h"
#include "CKirbySwallowEnd.h"

#include "CCameraController.h"
#include "CLevelFlowMgr.h"

CKirbySwallowEnd::CKirbySwallowEnd()
{
}

CKirbySwallowEnd::~CKirbySwallowEnd()
{
}

void CKirbySwallowEnd::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"CHANGE_ABILITY");
    }

}

void CKirbySwallowEnd::Enter()
{
    // ���� ���� �� Ŀ�� �̹̽ú� ȿ���� �����ʵ��� �Ѵ�.
    PLAYERFSM->SetSkrr(true);
    PLAYERFSM->SetGlobalState(true);
    PLAYERFSM->SetInvincible(true);

    // Ŀ�� SwallowEnd �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("SwallowStart"), false, false, 2.f);

    // MoveController Lock
    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->LockMove();

    // Ŀ�� ������ ��� ������Ʈ�� ���ߵ��� Ÿ�� �������� ����
    CTimeMgr::GetInst()->SetTimeScale(0.f);
    PLAYERCTRL->Animator()->SetAnimatorUpdateMode(AnimatorUpdateMode::UnscaledTime);

    // ��� �� ȿ��
    CGameObject* Manager = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager");
    CLevelFlowMgr* FLowMgrScript = Manager->GetScript<CLevelFlowMgr>();

    if (FLowMgrScript != nullptr)
    {
        FLowMgrScript->OnDimensionFade(0.3f);
    }

}

void CKirbySwallowEnd::Exit()
{
    // MoveController Lock
    PLAYERCTRL->UnlockInput();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->UnlockMove();

    // ��� �� ȿ�� ����
    CGameObject* Manager = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager");
    CLevelFlowMgr* FLowMgrScript = Manager->GetScript<CLevelFlowMgr>();

    if (FLowMgrScript != nullptr)
    {
        FLowMgrScript->OffDimensionFade();
    }

    // Ÿ�� ������ ����
    CTimeMgr::GetInst()->SetTimeScale(1.f);
    PLAYERCTRL->Animator()->SetAnimatorUpdateMode(AnimatorUpdateMode::Normal);

    // Emissive�� �ٽ� �޵��� ����
    PLAYERFSM->SetSkrr(false);
    PLAYERFSM->SetInvincible(false);

}
