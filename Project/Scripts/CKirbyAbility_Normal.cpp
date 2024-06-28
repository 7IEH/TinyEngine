#include "pch.h"
#include "CKirbyAbility_Normal.h"
#include "CKirbyBulletScript.h"

#define BULLET_SPEED 5.f


// �̰� Ability�ʿ� �ھƵ� �ǳ�?
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"

CKirbyAbility_Normal::CKirbyAbility_Normal()
{
	// ��� �޽� ����
	// ��� �ִϸ��̼� ����
}

CKirbyAbility_Normal::~CKirbyAbility_Normal()
{
}

// ===============
// Attack
// ===============
// �ӱ��� ��ü ���
void CKirbyAbility_Normal::Attack()
{
}

void CKirbyAbility_Normal::AttackEnter()
{
    // ==============
    // case: Vacuum
    // ==============
    if (!PLAYERFSM->GetVacuum())
    {
        ChangeState(L"ATTACK_CHARGE1");
        return;
    }

    // ==============
    // case: Shoot
    // ==============
    CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(1, 1, 1));
    PLAYERFSM->SetVacuum(false);

    // fire bullet
    Ptr<CPrefab> BulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\BulletStar.pref", L"prefab\\BulletStar.pref");
    if (nullptr != BulletPref)
    {
        CGameObject* BulletInst = BulletPref->Instantiate();
        Vec3 InitPos = PLAYER->Transform()->GetWorldPos() + PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT) * 2.f;
        
        BulletInst->Transform()->SetRelativePos(InitPos);
        GamePlayStatic::SpawnGameObject(BulletInst, 0);

        CKirbyBulletScript* bulletScript = BulletInst->GetScript<CKirbyBulletScript>();
        if (nullptr != bulletScript)
        {
            bulletScript->SetInitVelocity(PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT) * BULLET_SPEED);
        }
    }

    ChangeState(L"IDLE");
}

void CKirbyAbility_Normal::AttackExit()
{
}

// ===============
// Charge Attack 1
// ===============
// �����ϱ�1
void CKirbyAbility_Normal::AttackCharge1()
{
}

void CKirbyAbility_Normal::AttackCharge1Enter()
{
    PLAYERFSM->SetVacuum(true);
    CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(2, 2, 2));
}

void CKirbyAbility_Normal::AttackCharge1Exit()
{
    if (!PLAYERFSM->GetVacuum())
        CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(1, 1, 1));
}

// ===============
// Charge Attack 2
// ===============
// �����ϱ�2
void CKirbyAbility_Normal::AttackCharge2()
{
}

void CKirbyAbility_Normal::AttackCharge2Enter()
{
}

void CKirbyAbility_Normal::AttackCharge2Exit()
{
}