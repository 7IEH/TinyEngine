#include "pch.h"
#include "CKirbyObject_Cone.h"

CKirbyObject_Cone::CKirbyObject_Cone()
    : m_bFrmEnter(true)
    , m_PrevSpeed(0.f)
{
    m_OriginObject = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Cone.pref", L"prefab\\Cone.pref");
    m_Mesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyCone.mdat", L"meshdata\\KirbyCone.mdat");
    m_DemoMesh = CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\KirbyConeDemo.mdat", L"meshdata\\KirbyConeDemo.mdat");
    m_Crater = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Crater.pref", L"prefab\\Crater.pref");

    ParseDemoMesh(m_DemoMesh);
}

CKirbyObject_Cone::~CKirbyObject_Cone()
{
}

// ===============
// Run
// ===============

// End
void CKirbyObject_Cone::RunEnd()
{
}

void CKirbyObject_Cone::RunEndEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"Stop"), false);
}

void CKirbyObject_Cone::RunEndExit()
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
    PLAYER->Animator()->Play(KIRBYANIM(L"TurnEnd"), false, false, 1.5f);

    PLAYERCTRL->ClearVelocityY();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
}

void CKirbyObject_Cone::AttackExit()
{
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
}

// Start (���� �ָ���)
void CKirbyObject_Cone::AttackStart()
{
}

void CKirbyObject_Cone::AttackStartEnter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"Turn"), false, false, 1.5f);

    // �ӵ�����
    PLAYERCTRL->LockDirection();
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
    PLAYER->Animator()->Play(KIRBYANIM(L"TurnEnd"), true, false, 1.5f);
    //@Effect �ӵ���

    PLAYERCTRL->LockDirection();
    PLAYERCTRL->AddVelocity(Vec3(0.f, -20.f, 0.f));

    PLAYERFSM->SetInvincible(true);
}

void CKirbyObject_Cone::AttackEndExit()
{
    PLAYERCTRL->UnlockDirection();

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

#include "CKirbyBulletScript.h"

void CKirbyObject_Cone::AttackCombo1Enter()
{
    PLAYER->Animator()->Play(KIRBYANIM(L"TurnLanding"), false, false, 1.5f);
    //@Effect �浹����Ʈ

    // Crater ��ȯ
    {
        CGameObject* pCrater = m_Crater->Instantiate();
        CKirbyBulletScript* pBulletScript = pCrater->GetScript<CKirbyBulletScript>();
        Vec3 InitPos = PLAYER->CharacterController()->GetFootPos();
        
        pCrater->Transform()->SetLocalPos(InitPos);
        pBulletScript->SetInitVelocity(Vec3());
        pBulletScript->SetPlayTime(10.f);
        pBulletScript->SetEndAnim(KIRBYANIM(L"End"));
        pCrater->Animator()->Play(KIRBYANIM(L"Start"), false);

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
    PLAYER->Animator()->Play(KIRBYANIM(L"TurnEndEnd"), false, false, 1.5f);
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

    PLAYERCTRL->SetSpeed(m_PrevSpeed);

    // �ݶ��̴� & �ٵ��ݶ��̴� ũ�� ����
    CPlayerMgr::ResetBodyColliderSetting();
}

// ===============
// Change Object
// ===============

void CKirbyObject_Cone::ChangeObjectEnter()
{
    CKirbyObject::ChangeObjectEnter();

    m_PrevSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(7.f);

    // �ݶ��̴� & �ٵ��ݶ��̴� ũ�� ����
    PLAYER->CharacterController()->SetCenter(Vec3(0.f, 140.f, 0.f));
    PLAYER->CharacterController()->SetRadius(1.5f);
    PLAYER->CharacterController()->SetHeight(4.5f);

    CCapsuleCollider* BodyCol = PLAYER->GetChildObject(L"Body Collider")->CapsuleCollider();
    BodyCol->SetCenter(Vec3(0.f, 140.f, 0.f));
    BodyCol->SetRadius(1.5f);
    BodyCol->SetHeight(4.5f);
}
