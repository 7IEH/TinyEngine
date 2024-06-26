#include "pch.h"
#include "CKirbyAbility_Normal.h"
#include "CKirbyBulletScript.h"

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

void CKirbyAbility_Normal::Vacuum()
{
}

void CKirbyAbility_Normal::VacuumEnter()
{
    PLAYERFSM->SetVacuum(true);
    CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(200, 200, 200));

}

void CKirbyAbility_Normal::VacuumExit()
{
    if (!PLAYERFSM->GetVacuum())
        CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(100, 100, 100));
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
    if (!PLAYERFSM->GetVacuum())
        return;

    CPlayerMgr::GetPlayer()->Transform()->SetRelativeScale(Vec3(100, 100, 100));
    PLAYERFSM->SetVacuum(false);

    // fire bullet
    Ptr<CPrefab> BulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\BulletStar.pref", L"prefab\\BulletStar.pref");
    if (nullptr != BulletPref)
    {
        CGameObject* BulletInst = BulletPref->Instantiate();
        Vec3 InitPos = PLAYER->Transform()->GetWorldPos() + PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT) * 10.f;
        
        BulletInst->Transform()->SetRelativePos(InitPos);
        GamePlayStatic::SpawnGameObject(BulletInst, 0);

        CKirbyBulletScript* bulletScript = BulletInst->GetScript<CKirbyBulletScript>();
        if (nullptr != bulletScript)
        {
            bulletScript->SetInitVelocity(PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT) * 50.f);
        }
    }
}

void CKirbyAbility_Normal::AttackExit()
{
}

// ===============
// Charge Attack 1
// ===============
// �����ϱ�1
void CKirbyAbility_Normal::ChargeAttack1()
{
}

void CKirbyAbility_Normal::ChargeAttack1Enter()
{
}

void CKirbyAbility_Normal::ChargeAttack1Exit()
{
}

// ===============
// Charge Attack 2
// ===============
// �����ϱ�2
void CKirbyAbility_Normal::ChargeAttack2()
{
}

void CKirbyAbility_Normal::ChargeAttack2Enter()
{
}

void CKirbyAbility_Normal::ChargeAttack2Exit()
{
}