#include "pch.h"
#include "CKirbyChangeAbility.h"

#include "CCameraController.h"

CKirbyChangeAbility::CKirbyChangeAbility()
{
}

CKirbyChangeAbility::~CKirbyChangeAbility()
{
}

void CKirbyChangeAbility::tick()
{
    if (!m_bFrameEnter && CHECK_ANIMFRM(GetOwner(), 0))
    {
        PLAYER->Animator()->SetPlay(false);
        m_bFrameEnter = true;
    }
    else
    {
        PLAYER->Animator()->SetPlay(true);
    }

    // �ִϸ��̼� ����� �ʿ��� �۾� 
    PLAYERFSM->GetNextAbility()->ChangeAbility();

    if (PLAYER->Animator()->IsFinish())
    {
        switch (PLAYERFSM->GetCurAbilityIdx())
        {
        case AbilityCopyType::FIRE:
        case AbilityCopyType::NORMAL:
        case AbilityCopyType::CUTTER:
        case AbilityCopyType::SWORD: {
            if (PLAYER->Animator()->IsFinish())
            {
                PLAYERFSM->SetGlobalState(false);
                ChangeState(L"IDLE");
            }
        }
        break;
        case AbilityCopyType::SLEEP: {
            if (PLAYER->Animator()->IsFinish())
            {
                PLAYERFSM->SetGlobalState(false);
                ChangeState(L"IDLE_START");
            }
        }
        break;
        }
    }

}

void CKirbyChangeAbility::Enter()
{
    m_bFrameEnter = false;

    PLAYERFSM->SetGlobalState(true);

    // ���� �Ⱦ��ֱ�
    PLAYERFSM->GetNextAbility()->ChangeAbilityEnter();
    
    // Ŀ�� ���� �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("CopyFirst"), false, false, 1.5f);

    // Ŀ�� ���͸��� �ٽ� ����
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthSmileOpen));
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    // MoveController Lock
    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->LockMove();

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;
    CamCtrl->SaveSetting();

    CamCtrl->SetOffset(Vec3(0.f, 15.f, 0));
    CamCtrl->SetLookDir(Vec3(0.f, -0.35f, 0.937f));
    CamCtrl->SetLookDist(170.f);

    // @TODO Ÿ�� ������ ���� 

    // @TODO ī�޶� ���� ����
    Vec3 CamPos = CamCtrl->GetOwner()->Transform()->GetWorldPos();

    // @TODO Ŀ�� ���� ����
    Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();
    Vec3 Dir = CamPos - PlayerPos;
    Dir.y = 0.f;
    Dir.Normalize();

    ForceDirInfo DirInfo = {ForceDirType::STAGEEVENT, Dir, false};
    PLAYERCTRL->ForceDir(DirInfo);

    // @TODO
    // ��� �� ȿ��
    // Ŀ�� ������ ��� ������Ʈ�� ���߾�� ��
}

void CKirbyChangeAbility::Exit()
{
    // MoveController Lock
    PLAYERCTRL->UnlockInput();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->UnlockMove();

    PLAYERFSM->GetNextAbility()->ChangeAbilityExit();

    // Ŀ�� ǥ�� ����
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;
    CamCtrl->LoadSetting();

    //@TODO
    // ��� �� ȿ�� ����
    // Ŀ�� �ǿ��� ������Ʈ �ٽ� DT�޵��� ����
}
