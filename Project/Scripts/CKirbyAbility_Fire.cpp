#include "pch.h"
#include "CKirbyAbility_Fire.h"

#include "CPlayerMgr.h"
#include "CKirbyMoveController.h"
#include "CKirbyFSM.h"
#include "CKirbyFireBullet.h"

#include <Engine/CMesh.h>

CKirbyAbility_Fire::CKirbyAbility_Fire()
    : m_bFrmEnter(true)
    , m_SavedSpeed(0.f)
    , m_ProjectileAcc(0.f)
    , m_ProjectileCooltime(0.1f)
{
    m_Hat = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KiryDragonHat.pref", L"prefab\\KiryDragonHat.pref");
}

CKirbyAbility_Fire::CKirbyAbility_Fire(const CKirbyAbility_Fire& _Origin)
    : CKirbyAbility(_Origin)
    , m_bFrmEnter(true)
    , m_SavedSpeed(0.f)
    , m_ProjectileAcc(0.f)
    , m_ProjectileCooltime(_Origin.m_ProjectileCooltime)
{
    m_Hat = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KiryDragonHat.pref", L"prefab\\KiryDragonHat.pref");
}

CKirbyAbility_Fire::~CKirbyAbility_Fire()
{
}

// ===========
// �� ���� ����
// ===========

void CKirbyAbility_Fire::AttackCharge1Start()
{


}

void CKirbyAbility_Fire::AttackCharge1StartEnter()
{
    // Projectile CoolTime, Acc �ʱ�ȭ
    m_ProjectileAcc = 0.f;
    m_ProjectileCooltime = 0.1f;

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("FireBreathStart"), false, false, 2.f);

    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthOpen));
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);

    // Movement ���� (����, ����)
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();

    // �ӵ� ����
    PLAYERCTRL->SetSpeed(m_SavedSpeed/2.f);
}

void CKirbyAbility_Fire::AttackCharge1StartExit()
{
    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));
    CPlayerMgr::SetPlayerFace(FaceType::Normal);
    
    // Movement Unlock
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    // �ӵ� �ǵ����ֱ�
    PLAYERCTRL->SetSpeed(m_SavedSpeed);
}

// ======
// �� ����
// ======

void CKirbyAbility_Fire::AttackCharge1()
{
    // Ű���� �Է��� ������ �ش� ���⿡�� ���ߵ��� ����
    if (PLAYERCTRL->GetInput().Length() == 0.f)
    {
        PLAYERCTRL->LockDirection();
    }
    else
    {
        PLAYERCTRL->UnlockDirection();
    }


    // tick
    m_ProjectileAcc += DT;

    if (m_ProjectileAcc > m_ProjectileCooltime)
    {
        // Fire Projectile ����
        Ptr<CPrefab> FireProjectilePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV0.pref");
        CGameObject* FireProjectileObj = FireProjectilePref->Instantiate();

        // Kirby�� ���ʿ��� FireProjectile�� �����ǵ��� ��ġ�� ����Ѵ�. ������ Ŀ�� �ٶ󺸰� �ִ� �������� �Ѵ�.
        Vec3 KirbyPos = PLAYER->Transform()->GetWorldPos();
        Vec3 KirbyFront = PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Vec3 Offset = Vec3(0.f, 25.f, 0.f) + KirbyFront * 5.f;

        FireProjectileObj->Transform()->SetWorldPos(KirbyPos + Offset);
        FireProjectileObj->Transform()->SetDirection(KirbyFront);
        FireProjectileObj->GetScript<CKirbyFireBullet>()->SetFirst();

        GamePlayStatic::SpawnGameObject(FireProjectileObj, FireProjectileObj->GetLayerIdx());

        m_ProjectileAcc -= m_ProjectileCooltime;
    }
}

void CKirbyAbility_Fire::AttackCharge1Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("FireBreath"), true, false, 2.f);

    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);

    // Movement ����
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(4.f);

    // ȸ�� �ӵ� ����
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed / 4.f);
    
}

void CKirbyAbility_Fire::AttackCharge1Exit()
{
    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    // Ŀ�� ���͸��� ����
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));

    // Movement ����
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetSpeed(m_SavedSpeed);
    PLAYERCTRL->UnlockDirection();

    // ȸ�� �ӵ� ����
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);
}

// ========
// ���� ����
// ========

void CKirbyAbility_Fire::AttackCharge1End()
{
}

void CKirbyAbility_Fire::AttackCharge1EndEnter()
{
    m_ProjectileAcc = 0.f;

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("FireBreathEnd"), false, false , 2.f);

    // Movement ����
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(2.f);
}

void CKirbyAbility_Fire::AttackCharge1EndExit()
{
    // Movement ����
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetSpeed(m_SavedSpeed);
}

void CKirbyAbility_Fire::AttackCharge1Run()
{
    // Ű���� �Է��� ������ �ش� ���⿡�� ���ߵ��� ����
    if (PLAYERCTRL->GetInput().Length() == 0.f)
    {
        PLAYERCTRL->LockDirection();
    }
    else
    {
        PLAYERCTRL->UnlockDirection();
    }

    // tick
    m_ProjectileAcc += DT;

    if (m_ProjectileAcc > m_ProjectileCooltime)
    {
        // Fire Projectile ����
        Ptr<CPrefab> FireProjectilePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV0.pref");
        CGameObject* FireProjectileObj = FireProjectilePref->Instantiate();

        // Kirby�� ���ʿ��� FireProjectile�� �����ǵ��� ��ġ�� ����Ѵ�. ������ Ŀ�� �ٶ󺸰� �ִ� �������� �Ѵ�.
        Vec3 KirbyPos = PLAYER->Transform()->GetWorldPos();
        Vec3 KirbyFront = PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Vec3 Offset = Vec3(0.f, 25.f, 0.f) + KirbyFront * 5.f;

        FireProjectileObj->Transform()->SetWorldPos(KirbyPos + Offset);
        FireProjectileObj->Transform()->SetDirection(KirbyFront);
        FireProjectileObj->GetScript<CKirbyFireBullet>()->SetFirst();

        GamePlayStatic::SpawnGameObject(FireProjectileObj, FireProjectileObj->GetLayerIdx());

        m_ProjectileAcc -= m_ProjectileCooltime;
    }
}

void CKirbyAbility_Fire::AttackCharge1RunEnter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("FireWalk"), true, false, 2.f);

    // Movement ����
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(2.f);

    // Kirby ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);

    
    // ȸ�� �ӵ� ����
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed / 3.f);

}

void CKirbyAbility_Fire::AttackCharge1RunExit()
{
    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    // Kirby ���͸��� ����
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));

    // Movement ����
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->SetSpeed(m_SavedSpeed);

    // ȸ�� �ӵ� ����
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);
}


void CKirbyAbility_Fire::ChangeAbility()
{
}

void CKirbyAbility_Fire::ChangeAbilityEnter()
{
    // ���Ͽ� ���� �����ֱ�
    if (m_Hat.Get())
    {
        CGameObject* pInstObj = m_Hat->Instantiate();
        GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Hat");
        PLAYERFSM->SetCurHat(pInstObj);

        // �ִϸ��̼� ���
        pInstObj->Animator()->Play(ANIMPREFIX("Deform"),false,false,2.5f,0);
    }
}

void CKirbyAbility_Fire::ChangeAbilityExit()
{
}
