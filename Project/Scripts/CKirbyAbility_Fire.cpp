#include "pch.h"
#include "CKirbyAbility_Fire.h"

#include "CPlayerMgr.h"

#include <Engine/CMesh.h>

CKirbyAbility_Fire::CKirbyAbility_Fire()
{
}

CKirbyAbility_Fire::~CKirbyAbility_Fire()
{
}

void CKirbyAbility_Fire::Attack()
{
}

void CKirbyAbility_Fire::AttackEnter()
{
}

void CKirbyAbility_Fire::AttackExit()
{
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
