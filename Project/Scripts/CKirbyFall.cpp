#include "pch.h"
#include "CKirbyFall.h"

#include "CCameraController.h"
#include "CKirbyUnitScript.h"

#include "CLevelFlowMgr.h"

CKirbyFall::CKirbyFall()
{
}

CKirbyFall::~CKirbyFall()
{
}

void CKirbyFall::tick()
{
    m_Acc += DT;

    // State�� �ð��� ��� ������
    if (m_Acc > m_Duration)
    {
        // HP�� 0������ ���
        if (PLAYERUNIT->GetCurInfo().HP <= 0.f)
        {
            // FLOW �Ŵ����� ���� ���� �Լ��� ȣ��
            CGameObject* ManagerObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager");
            
            if (nullptr != ManagerObj)
            {
                CLevelFlowMgr* FlowMgrScript = ManagerObj->GetScript<CLevelFlowMgr>();

                if (nullptr != FlowMgrScript)
                {
                    // �ѹ��� ȣ��ǵ��� State�� �����Ѵ�
                    ChangeState(L"IDLE");
                    FlowMgrScript->LevelRestart();
                }
            }
        }
        // HP�� �����ִ� ��� Respawn�Ѵ�
        else
        {
            Vec3 RespawnPos = PLAYERCTRL->GetCheckPoint();
            RespawnPos.y += 30.f;

            // Player�� Respawn�Ѵ�
            PLAYER->Transform()->SetWorldPos(RespawnPos);

            // Emissive, ���� ó��
            PLAYERFSM->SetEmissive(true, 2.f);
            PLAYERFSM->SetInvincible(true, 2.f);

            // State Change
            ChangeState(L"IDLE");

            PLAYER->CharacterController()->Move(Vec3(0.f, 0.1f, 0.f));

            // ī�޶� Lock�� Ǯ���� �� �÷��̾���ġ�� �ٷ� ��ȯ
            CAMERACTRL->SetLock(false);
            CAMERACTRL->SetLock(true, 0.05f);

            //CAMERACTRL->ResetCamera();

        }
    }

}

void CKirbyFall::Enter()
{
    // TimeScale 0���� �ٲ� �Ͻ�����
    CTimeMgr::GetInst()->SetTimeScale(0.5f, 0.f);

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;
    CamCtrl->SetLock(true);

    // Camera Shake
    CamCtrl->Shake(0.5f, 30.f, 30.f);

    GetOwner()->Animator()->Play(ANIMPREFIX("Damage"), true);

    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetLimitFallSpeed(true);

    // Kirby���� ������ �ֱ�
    PLAYERUNIT->GetDamage({DAMAGE_TYPE::NORMAL, Vec3(0.f, 0.f, 0.f), 10.f, 0.f, 0.f});

    m_Acc = 0.f;
    m_Duration = 2.f;
}

void CKirbyFall::Exit()
{
    PLAYERCTRL->UnlockInput();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetLimitFallSpeed(false);
}