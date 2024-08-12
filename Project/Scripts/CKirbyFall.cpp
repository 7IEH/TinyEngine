#include "pch.h"
#include "CKirbyFall.h"

#include "CCameraController.h"
#include "CKirbyUnitScript.h"

CKirbyFall::CKirbyFall()
{
}

CKirbyFall::~CKirbyFall()
{
}

void CKirbyFall::tick()
{
    m_Acc += DT;

    if (!m_TimeStopExit && m_Acc > m_TimeStopDuration)
    {
        // @TODO DT ����ȭ

        m_TimeStopExit = true;
    }

    // State�� �ð��� ��� ������
    if (m_Acc > m_Duration)
    {
        // HP�� 0������ ���
        if (PLAYERUNIT->GetCurInfo().HP <= 0.f)
        {
            // FLOW �Ŵ����� ���� ���� �Լ��� ȣ��

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

        }
    }

}

void CKirbyFall::Enter()
{
    // @TODO DT 0���� �ٲ��ֱ�


    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;

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
    m_TimeStopDuration = 0.5f;
    m_Duration = 2.f;
    m_TimeStopExit = false;
}

void CKirbyFall::Exit()
{
    PLAYERCTRL->UnlockInput();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetLimitFallSpeed(false);
}