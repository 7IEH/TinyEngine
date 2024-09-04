#include "pch.h"
#include "CKirbyBurningEnd.h"

#include "CCameraController.h"

CKirbyBurningEnd::CKirbyBurningEnd()
    : m_SaveRotSpeed(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurningEnd::~CKirbyBurningEnd()
{
}
void CKirbyBurningEnd::tick()
{
    // ��ü �۶��̵� �ð��� ���� FSM�� Gliding �ð��� �����ش�.
    PLAYERFSM->AddGlidingTime(DT);

    // �߷°� ����
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 10.f)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }

    float CurGlidingTime = PLAYERFSM->GetCurGlidingTime();

    // �۶��̵� �ð��� �����ų� ����Ű�� ������ ������
    if (CurGlidingTime > PLAYERFSM->GetGlidingDuration() || (KEY_RELEASED(KEY_ATK) || KEY_NONE(KEY_ATK)))
    {
        ChangeState(L"JUMP_FALL");
    }
}

void CKirbyBurningEnd::Enter()
{
    CGameObject* Spawner = PLAYER->GetChildObject(L"FireSmokeSpawner");
    if (Spawner != nullptr)
    {
        Spawner->SetActive(true);
    }

    PLAYERFSM->SetBurningParticleSpawn(true);

    CGameObject* Wing = PLAYER->GetChildObject(L"KirbyDragon");

    if (Wing != nullptr)
    {
        Wing->SetActive(true);
    }

    Wing->Animator()->Play(ANIMPREFIX("BurningEnd"), false, false, 1.5f);
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("BurningEnd"), false, false, 1.5f);

    // Movement
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetForwardMode(true);
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(0.5f);

    m_SaveSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(13.f);

    PLAYERCTRL->SetGravity(PLAYERFSM->GetGlidingGravity());

    //  ���� ����
    PLAYERFSM->SetInvincible(true);

    // Camera Setting
    CAMERACTRL->SaveSetting();

    CAMERACTRL->SetMinSpeed(100.f);
    CAMERACTRL->SetMaxSpeed(500.f);
    CAMERACTRL->SetThresholdDistance(300.f);
}

void CKirbyBurningEnd::Exit()
{
    CGameObject* Spawner = PLAYER->GetChildObject(L"FireSmokeSpawner");
    if (Spawner != nullptr)
    {
        Spawner->SetActive(false);
    }

    PLAYERFSM->SetBurningParticleSpawn(false);

    CGameObject* Wing = PLAYER->GetChildObject(L"KirbyDragon");

    if (Wing != nullptr)
    {
        Wing->SetActive(false);
    }

    // Movement
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);

    PLAYERCTRL->SetSpeed(m_SaveSpeed);

    PLAYERFSM->SetGlidingGravity(PLAYERCTRL->GetGravity());
    PLAYERCTRL->SetGravity(PLAYERCTRL->GetInitGravity());

    //  ���� ����
    PLAYERFSM->SetInvincible(false);

        // Camera Setting Return
    CAMERACTRL->LoadSetting(true);
}
