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
    PLAYER->Animator()->Play(KIRBYANIM(L"FireBreathStart"), false, false, 2.f);

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
    PLAYER->Animator()->Play(KIRBYANIM(L"FireBreath"), true, false, 2.f);

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
    PLAYER->Animator()->Play(KIRBYANIM(L"FireBreathEnd"), false, false , 2.f);

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
    PLAYER->Animator()->Play(KIRBYANIM(L"FireWalk"), true, false, 2.f);

    // Movement ����
    PLAYERCTRL->LockJump();
    m_SavedSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(2.f);

    // Kirby ǥ�� �ٲ��ֱ�
    CPlayerMgr::SetPlayerFace(FaceType::UpTail);

    // Kirby ���͸��� ����
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyBig));

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
    const vector<tBoneSocket*>& vecBoneSocket = PLAYER->Animator()->GetSkeletalMesh()->GetvecBoneSocket();

    for (tBoneSocket* BoneSocket : vecBoneSocket)
    {
        CGameObject* pInstObj = nullptr;

        if (BoneSocket->SoketName == L"HatLSocket_0")
        {
            pInstObj = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KiryDragonHat.pref", L"prefab\\KiryDragonHat.pref")->Instantiate();

            if (nullptr != pInstObj)
            {
                // �ִϸ��̼� ����
                pInstObj->Animator()->Play(KIRBYANIM(L"Deform"));

                // ���� ����
                pInstObj->Transform()->SetRelativePos(PLAYER->Transform()->GetWorldPos());
                pInstObj->Transform()->SetRelativeRotation(PLAYER->Transform()->GetWorldRotation());

                pInstObj->Transform()->SetAbsolute(false);


                GamePlayStatic::AddChildObject(PLAYER, pInstObj, BoneSocket);
            }
        }
    }
}

void CKirbyAbility_Fire::ChangeAbilityExit()
{
}
