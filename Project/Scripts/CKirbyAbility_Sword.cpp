#include "pch.h"
#include "CKirbyAbility_Sword.h"
#include "CKirbyMoveController.h"
#include "CState.h"

CKirbyAbility_Sword::CKirbyAbility_Sword()
    : m_bFrmEnter(true)
    , m_PrevSpeed(0.f)
    , m_PrevRotSpeed(0.f)
    , m_PrevGravity(0.f)
{
    m_ComboSuccessTime = 0.5f;
    m_Charge1Time = 1.f;
    m_Charge2Time = 1.f;
}

CKirbyAbility_Sword::~CKirbyAbility_Sword()
{
}

// ===============
// Idle
// ===============

void CKirbyAbility_Sword::IdleEnter()
{
    wstring prev = PLAYERFSM->GetPrevState()->GetName();

    if (prev == L"ATTACK" || prev == L"ATTACK_COMBO1" || prev == L"ATTACK_COMBO2")
    {
        PLAYER->Animator()->Play(KIRBYANIM(L"Wait"), true, false, 2.5f, 0.3f);
    }
    else
    {
        PLAYER->Animator()->Play(KIRBYANIM(L"Wait"));
    }
}

// ===============
// Attack
// ===============
// Į�ֵθ��� (Lv0)

void CKirbyAbility_Sword::Attack()
{
    if (PLAYER->Animator()->GetClipFrameIndex() == 7 && m_bFrmEnter)
    {
        m_bFrmEnter = false;

        //@TODO �˱� ������
        //Ptr<CPrefab> BulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KirbyBullet.pref", L"prefab\\KirbyBullet.pref");
        //if (nullptr != BulletPref)
        //{
        //    CGameObject* BulletInst = BulletPref->Instantiate();
        //}
    }
}

void CKirbyAbility_Sword::AttackEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"Attack1"), false, false, 1.5);

    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    m_bFrmEnter = true;

    //@Effect Effect ���
}

void CKirbyAbility_Sword::AttackExit()
{
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    PLAYERFSM->SetComboLevel(1);
}

// ===============
// Attack Combo 1
// ===============
// Į�ֵθ��� (Lv1)

void CKirbyAbility_Sword::AttackCombo1()
{
    if (PLAYER->Animator()->GetClipFrameIndex() == 9 && m_bFrmEnter)
    {
        m_bFrmEnter = false;

        //@TODO �˱� ������
        // Ptr<CPrefab> BulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KirbyBullet.pref", L"prefab\\KirbyBullet.pref");
        // if (nullptr != BulletPref)
        //{
        //    CGameObject* BulletInst = BulletPref->Instantiate();
        //}
    }
}

void CKirbyAbility_Sword::AttackCombo1Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"Attack2"), false, false, 1.5f);
    //@TODO Effect ���

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    m_bFrmEnter = true;
}

void CKirbyAbility_Sword::AttackCombo1Exit()
{
    PLAYERFSM->SetComboLevel(2);

    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Attack Combo 2
// ===============
// Į�ֵθ��� (Lv2)

void CKirbyAbility_Sword::AttackCombo2()
{
    if (PLAYER->Animator()->GetClipFrameIndex() == 17 && m_bFrmEnter)
    {
        m_bFrmEnter = false;

        //@TODO �˱� ������
        // Ptr<CPrefab> BulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KirbyBullet.pref", L"prefab\\KirbyBullet.pref");
        // if (nullptr != BulletPref)
        //{
        //    CGameObject* BulletInst = BulletPref->Instantiate();
        //}
    }
}

void CKirbyAbility_Sword::AttackCombo2Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"Attack3"), false, false, 1.5f);

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    //@Effect Effect ���
    m_bFrmEnter = true;
}

void CKirbyAbility_Sword::AttackCombo2Exit()
{
    PLAYERFSM->SetComboLevel(-1);

    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Charge Attack 1
// ===============
// ���ɰ��� (Lv1)

// (charge attack)
void CKirbyAbility_Sword::AttackCharge1()
{
    //@TODO Decal
}

void CKirbyAbility_Sword::AttackCharge1Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SpinSlash"), false, false, 1.5f);
    //@Effect ��������Ʈ

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge1Exit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// Start (charge)
void CKirbyAbility_Sword::AttackCharge1Start()
{
}

void CKirbyAbility_Sword::AttackCharge1StartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SpinSlashCharge"), false, false, 1.5f);
    //@Effect �����Ϸ�

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge1StartExit()
{
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// End (charge attack end)
void CKirbyAbility_Sword::AttackCharge1End()
{
}

void CKirbyAbility_Sword::AttackCharge1EndEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SpinSlashEnd"), false, false, 1.5f);
    //@Effect ������ ��� ��ƼŬ ���ڵ�

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge1EndExit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Charge Attack 2
// ===============
// ���ɰ���-���� (Lv2)

// (charge)
void CKirbyAbility_Sword::AttackCharge2()
{
}

void CKirbyAbility_Sword::AttackCharge2Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SuperSpinSlashCharge"), true, false, 1.5f);

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge2Exit()
{
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// start (charge start)
void CKirbyAbility_Sword::AttackCharge2Start()
{
}

void CKirbyAbility_Sword::AttackCharge2StartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SuperSpinSlashChargeStart"), false, false, 1.5f);
    //@Effect �����Ϸ�

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge2StartExit()
{
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Charge Attack 2
// ===============
// ���ɰ���-���� (Lv2)
// State ���� �ļ� ����

// ===============
// Charge Attack 3
// ===============
// ��˰��� (Lv3)

// (charge)
void CKirbyAbility_Sword::AttackCharge3()
{
    if (PLAYER->Animator()->GetClipFrameIndex() == 25 && !PLAYER->Animator()->IsReverse())
    {
        PLAYER->Animator()->SetReverse(true);
    }
    else if (PLAYER->Animator()->GetClipFrameIndex() == 19 && PLAYER->Animator()->IsReverse())
    {
        PLAYER->Animator()->SetReverse(false);
    }
}

void CKirbyAbility_Sword::AttackCharge3Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"GigantChargeAttack"), false, false, 1.5f);

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    m_PrevRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->SetRotSpeed(5.f);
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge3Exit()
{
    PLAYER->Animator()->SetReverse(false);
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->SetRotSpeed(m_PrevRotSpeed);
    PLAYERCTRL->UnlockJump();
}

// Start (charge start)
void CKirbyAbility_Sword::AttackCharge3Start()
{
}

void CKirbyAbility_Sword::AttackCharge3StartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"GigantChargeAttackStart"), false, false, 1.5f);
    CPlayerMgr::SetPlayerFace(FaceType::Frown);
    //@Effect �����Ϸ�

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    m_PrevRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetSpeed(3.f);
    PLAYERCTRL->SetRotSpeed(5.f);
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge3StartExit()
{
    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->SetRotSpeed(m_PrevRotSpeed);
    PLAYERCTRL->UnlockJump();
}

// End (attack)
void CKirbyAbility_Sword::AttackCharge3End()
{
}

void CKirbyAbility_Sword::AttackCharge3EndEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"GigantChargeAttack"), false, false, 1.5f, 0.f);
    PLAYER->Animator()->SetClipFrameIndex(25);
    //@Effect ����

    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::AttackCharge3EndExit()
{
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Jump Attack
// ===============
// ���� 
void CKirbyAbility_Sword::JumpFallEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"JumpFall"), false, false, 2.5f, 0.3f);
}

// ===============
// Jump Attack
// ===============
// ���� ���ɰ���

void CKirbyAbility_Sword::JumpAttack()
{
    if (m_bFrmEnter && PLAYER->Animator()->GetClipFrameIndex() == 10)
    {
        if (PLAYERFSM->GetSlideComboLevel())
        {
            PLAYERCTRL->SetGravity(-80.f);
        }
        else
        {
            PLAYERCTRL->SetGravity(m_PrevGravity);
        }
        m_bFrmEnter = false;
    }
}

void CKirbyAbility_Sword::JumpAttackEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SwordSpin"), false, false, 2.f);
    //@Effect ���� ����Ʈ

    m_PrevGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(0.f);
    PLAYERCTRL->ClearVelocityY();
    PLAYERCTRL->LockDirection();

    m_bFrmEnter = true;
}

void CKirbyAbility_Sword::JumpAttackExit()
{
    PLAYERCTRL->SetGravity(m_PrevGravity);
    PLAYERCTRL->UnlockDirection();
}

// Start
void CKirbyAbility_Sword::JumpAttackStart()
{
}

void CKirbyAbility_Sword::JumpAttackStartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SwordSpinStart"), false, false, 2.f);
    //@Effect Į �� ���󰡴� ����Ʈ

    PLAYERCTRL->LockDirection();
}

void CKirbyAbility_Sword::JumpAttackStartExit()
{
    PLAYERCTRL->UnlockDirection();
}

// ===============
// Landing
// ===============

void CKirbyAbility_Sword::LandingEnter()
{
    if (PLAYERFSM->GetLastJump() == LastJumpType::LOW)
    {
        PLAYER->Animator()->Play(KIRBYANIM(L"LandingSmall"), false);
    }
    else
    {
        PLAYER->Animator()->Play(KIRBYANIM(L"Landing"), false);
    }

    if (PLAYERFSM->GetSlideComboLevel())
    {
        PLAYERFSM->SetKnockBackDir(-PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT));

        //@Effect ������� ���ȿ��, ����
    }

    PLAYERCTRL->LockJump();
}

void CKirbyAbility_Sword::LandingExit()
{
    PLAYERFSM->SetSlideComboLevel(0);
    PLAYERCTRL->UnlockJump();
}

// ===============
// Slide
// ===============
// �����̵� ���

void CKirbyAbility_Sword::Slide()
{
}

void CKirbyAbility_Sword::SlideEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SwordSlide"));
}

void CKirbyAbility_Sword::SlideExit()
{
}

// Start
void CKirbyAbility_Sword::SlideStart()
{
}

void CKirbyAbility_Sword::SlideStartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SwordSlideStart"), false);
}

void CKirbyAbility_Sword::SlideStartExit()
{
}

// End
void CKirbyAbility_Sword::SlideEnd()
{
}

void CKirbyAbility_Sword::SlideEndEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"SwordSlideEnd"), false);
}

void CKirbyAbility_Sword::SlideEndExit()
{
}

// ===============
// Slide Attack
// ===============
// �����̵� �޺�����

void CKirbyAbility_Sword::SlideAttack()
{
    if (PLAYER->Animator()->IsFinish() && m_bFrmEnter)
    {
        if (PLAYERCTRL->GetVelocity().y > 0.f)
        {
            PLAYERCTRL->ClearVelocityY();
        }

        m_bFrmEnter = false;
    }
}

void CKirbyAbility_Sword::SlideAttackEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"UpwardSlash"), false, false, 2.f);
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);
    //@Effect Į�� ������

    PLAYERCTRL->AddVelocity(Vec3(0, 30, 0));
    m_PrevGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(-100.f);

    m_bFrmEnter = true;
}

void CKirbyAbility_Sword::SlideAttackExit()
{
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    PLAYERCTRL->SetGravity(m_PrevGravity);
    if (PLAYERCTRL->GetVelocity().y > 0.f)
    {
        PLAYERCTRL->ClearVelocityY();
    }
}

// ===============
// Change Ability
// ===============

void CKirbyAbility_Sword::ChangeAbility()
{
}

void CKirbyAbility_Sword::ChangeAbilityEnter()
{
    // create hat
    Ptr<CMeshData> pMeshData = CAssetMgr::GetInst()->FindAsset<CMeshData>(L"meshdata\\MorphoHat.mdat");
    CGameObject* pInstObj = pMeshData->Instantiate();
    //CGameObject* pInstObj = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KirySwordHat.pref", L"prefab\\KirySwordHat.pref")->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Hat");

    // create sword
    pMeshData = CAssetMgr::GetInst()->FindAsset<CMeshData>(L"meshdata\\MorphoSword.mdat");
    pInstObj = pMeshData->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"WeaponSword");
}

void CKirbyAbility_Sword::ChangeAbilityExit()
{
}
