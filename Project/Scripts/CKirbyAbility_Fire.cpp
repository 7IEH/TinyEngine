#include "pch.h"
#include "CKirbyAbility_Fire.h"

#include "CPlayerMgr.h"
#include "CKirbyMoveController.h"
#include "CKirbyFSM.h"

#include <Engine/CMesh.h>

CKirbyAbility_Fire::CKirbyAbility_Fire()
    : m_bFrmEnter(true)
    , m_SavedSpeed(0.f)
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
    PLAYERCTRL->SetSpeed(2.f);
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
}

void CKirbyAbility_Fire::AttackCharge1Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("FireBreath"), true, false, 2.f);

    // Ŀ�� ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);

    // Ŀ�� ���͸��� ����
    //CPlayerMgr::ClearBodyMtrl();
    //CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyBig));

    // @TODO ������ ���� -> ȸ������ �ٲ���� ��
    // Movement ����
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(4.f);

    // @TODO ���� �ֱ⸶�� Fire Projectile ����
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
}

// ========
// ���� ����
// ========

void CKirbyAbility_Fire::AttackCharge1End()
{
}

void CKirbyAbility_Fire::AttackCharge1EndEnter()
{
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

    // @TODO ���� �ֱ⸶�� Fire Projectile ����

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
    PLAYERCTRL->SetSpeed(m_SavedSpeed);
}

void CKirbyAbility_Fire::JumpAttack()
{
}

void CKirbyAbility_Fire::JumpAttackEnter()
{
}

void CKirbyAbility_Fire::JumpAttackExit()
{
}

void CKirbyAbility_Fire::ChangeAbility()
{
}

void CKirbyAbility_Fire::ChangeAbilityEnter()
{
    // ���Ͽ� ���� �����ֱ�
    CGameObject* pInstObj = m_Hat->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Hat");

    PLAYERFSM->SetCurHat(pInstObj);

    // �ִϸ��̼� ���
    pInstObj->Animator()->Play(ANIMPREFIX("Deform"));
}

void CKirbyAbility_Fire::ChangeAbilityExit()
{
}
