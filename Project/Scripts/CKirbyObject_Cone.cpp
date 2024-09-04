#include "pch.h"
#include "CKirbyObject_Cone.h"
#include "CKirbyUnitScript.h"
#include "CMomentaryObjScript.h"

CKirbyObject_Cone::CKirbyObject_Cone()
    : m_bFrmEnter(true)
    , m_PrevSpeed(0.f)
    , m_PrevJumpPower(0.f)
    , m_PrevGravity(0.f)
{
    m_OriginObject = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Cone.pref", L"prefab\\Cone.pref");
    m_Mesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyCone.mdat", L"meshdata\\KirbyCone.mdat");
    m_DemoMesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyConeDemo.mdat", L"meshdata\\KirbyConeDemo.mdat");
    m_Crater = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Crater.pref", L"prefab\\Crater.pref");

    ParseDemoMesh(m_DemoMesh);

    // add waiting animation
    m_WaitingAnim.push_back(L"WaitVariation1");
}

CKirbyObject_Cone::CKirbyObject_Cone(const CKirbyObject_Cone& _Origin)
    : CKirbyObject(_Origin)
    , m_bFrmEnter(true)
    , m_PrevSpeed(0.f)
    , m_PrevJumpPower(0.f)
    , m_PrevGravity(0.f)
{
    m_OriginObject = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Cone.pref", L"prefab\\Cone.pref");
    m_Mesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyCone.mdat", L"meshdata\\KirbyCone.mdat");
    m_DemoMesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyConeDemo.mdat", L"meshdata\\KirbyConeDemo.mdat");
    m_Crater = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Crater.pref", L"prefab\\Crater.pref");

    ParseDemoMesh(m_DemoMesh);

    // add waiting animation
    m_WaitingAnim.push_back(L"WaitVariation1");
}

CKirbyObject_Cone::~CKirbyObject_Cone()
{
}

// ===============
// Attack
// ===============
// ������� - ����

// (�������� ����)
void CKirbyObject_Cone::Attack()
{
}

void CKirbyObject_Cone::AttackEnter()
{
    GamePlayStatic::Play2DSound(L"sound\\wav\\HeroDeformCone\\0006.wav", 1, KIRBY_EFFECTSOUND);

    PLAYER->Animator()->Play(ANIMPREFIX("TurnEnd"), false, false, 2.f);

    PLAYERCTRL->LockDirection();
}

void CKirbyObject_Cone::AttackExit()
{
    PLAYERCTRL->UnlockDirection();
}

// Start (���� �ָ���)
void CKirbyObject_Cone::AttackStart()
{
}

void CKirbyObject_Cone::AttackStartEnter()
{
    GamePlayStatic::Play2DSound(L"sound\\wav\\HeroDeformCone\\Cone_UpWhizz.wav", 1, KIRBY_EFFECTSOUND);

    PLAYER->Animator()->Play(ANIMPREFIX("Turn"), false, false, 2.f);

    // �ӵ�����
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->ClearVelocityY();
    PLAYERCTRL->AddVelocity(Vec3(0.f, 15.f, 0.f));
}

void CKirbyObject_Cone::AttackStartExit()
{
    PLAYERCTRL->UnlockDirection();
}

// End (�������� ��)
void CKirbyObject_Cone::AttackEnd()
{
}

void CKirbyObject_Cone::AttackEndEnter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("TurnEnd"), true, false, 2.f);
    //@Effect �ӵ���

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->SetLimitFallSpeed(false);
    PLAYERCTRL->ClearVelocityY();
    PLAYERCTRL->AddVelocity(Vec3(0.f, -30.f, 0.f));

    PLAYERFSM->SetInvincible(true);
}

void CKirbyObject_Cone::AttackEndExit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->SetLimitFallSpeed(true);

    PLAYERFSM->SetInvincible(false);
}

// ===============
// Attack
// ===============
// ������� - ��

// (�浹)
void CKirbyObject_Cone::AttackCombo1()
{
}

void CKirbyObject_Cone::AttackCombo1Enter()
{
    CAMERACTRL->Shake(0.5f, 30.f, 30.f);

    GamePlayStatic::Play2DSound(L"sound\\wav\\HeroDeformCone\\0007.wav", 1, KIRBY_EFFECTSOUND);

    PLAYER->Animator()->Play(ANIMPREFIX("TurnLanding"), false, false, 2.f);
    //@Effect �浹����Ʈ

    // Crater ��ȯ
    {
        CGameObject* pCrater = m_Crater->Instantiate();
        CMomentaryObjScript* pBulletScript = pCrater->GetScript<CMomentaryObjScript>();
        Vec3 InitPos = PLAYER->CharacterController()->GetFootPos();
        
        pCrater->Transform()->SetLocalPos(InitPos);
        pBulletScript->SetInitVelocity(Vec3());
        pBulletScript->SetPlayTime(10.f);
        pBulletScript->SetEndAnim(ANIMPREFIX("End"));
        pCrater->Animator()->Play(ANIMPREFIX("Start"), false);

        GamePlayStatic::SpawnGameObject(pCrater, LAYER_PLAYERATK);
    }

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();

    PLAYERFSM->SetInvincible(true);
}

void CKirbyObject_Cone::AttackCombo1Exit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();

    PLAYERFSM->SetInvincible(false);
}

// End (�Ͼ)
void CKirbyObject_Cone::AttackCombo1End()
{
}

void CKirbyObject_Cone::AttackCombo1EndEnter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("TurnEndEnd"), false, false, 2.f);
    //@Effect �浹����Ʈ

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();
}

void CKirbyObject_Cone::AttackCombo1EndExit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
}

// ===============
// Drop Object
// ===============

void CKirbyObject_Cone::DropObjectEnter()
{
    CKirbyObject::DropObjectEnter();

    // �ݶ��̴� & �ٵ��ݶ��̴� ũ�� ����
    CPlayerMgr::ResetBodyColliderSetting();

    PLAYERCTRL->SetSpeed(m_PrevSpeed);
    PLAYERCTRL->SetJumpPower(m_PrevJumpPower);
    PLAYERCTRL->SetGravity(m_PrevGravity);
}

// ===============
// Change Object
// ===============

void CKirbyObject_Cone::ChangeObjectEnter()
{
    CKirbyObject::ChangeObjectEnter();

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    m_PrevJumpPower = PLAYERCTRL->GetJumpPower();
    m_PrevGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetJumpPower(15.f);
    PLAYERCTRL->SetGravity(-40.f);
    PLAYERCTRL->SetSpeed(5.f);

    // �ݶ��̴� & �ٵ��ݶ��̴� ũ�� ����
    PLAYER->CharacterController()->SetCenter(Vec3(0.f, 1.49f, 0.f));
    PLAYER->CharacterController()->SetRadius(1.f);
    PLAYER->CharacterController()->SetHeight(3.f);

    CCapsuleCollider* BodyCol = PLAYER->GetChildObject(L"Body Collider")->CapsuleCollider();
    BodyCol->SetCenter(Vec3(0.f, 1.2f, 0.f));
    BodyCol->SetRadius(1.f);
    BodyCol->SetHeight(3.f);
}

void CKirbyObject_Cone::LandingEnter()
{
    CKirbyObject::LandingEnter();

    GamePlayStatic::Play2DSound(L"sound\\wav\\HeroDeformCone\\Cone_Landing.wav", 1, KIRBY_EFFECTSOUND);
}
