#include "pch.h"
#include "CKirbyAttackCharge2Slash.h"

CKirbyAttackCharge2Slash::CKirbyAttackCharge2Slash()
    : m_Speed(13.f)
    , m_PrevSpeed(0.f)
    , m_PlayTime(1.5f)
    , m_bEnableFireParticle(false)
{
}

CKirbyAttackCharge2Slash::~CKirbyAttackCharge2Slash()
{
}

void CKirbyAttackCharge2Slash::tick()
{
    m_PlayTime -= DT;

    if (m_bEnableFireParticle && m_PlayTime < 0.5f)
    {
        m_bEnableFireParticle = false;
        PLAYERFSM->GetCurWeapon()->GetChildObject(L"KirbySwordFireParticle")->ParticleSystem()->EnableModule(PARTICLE_MODULE::SPAWN, true);
    }

    if (m_PlayTime < 0.f)
    {
        ChangeState(L"ATTACK_CHARGE2_SLASH_END");
    }
}

void CKirbyAttackCharge2Slash::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("SuperSpinSlashLoop"), true, false, 1.5f);

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(m_Speed);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    PLAYERFSM->SetInvincible(true);
    PLAYERFSM->GetCurWeapon()->BoxCollider()->SetEnabled(true);

    m_PlayTime = 1.5f;
    m_bEnableFireParticle = true;

    // sound
    wstring Sound = L"sound\\wav\\HeroSwordBasic\\Spin_Long.wav";
    GamePlayStatic::Play2DSound(Sound, 1, SOUND_SWORDKIRBY);
}

void CKirbyAttackCharge2Slash::Exit()
{
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    PLAYERFSM->SetInvincible(false);
    PLAYERFSM->GetCurWeapon()->BoxCollider()->SetEnabled(false);
}
