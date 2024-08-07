#include "pch.h"
#include "CKirbyAbility_Cutter.h"

#include "CPlayerMgr.h"
#include "CKirbyMoveController.h"
#include "CKirbyHatBlade.h"

CKirbyAbility_Cutter::CKirbyAbility_Cutter()
    : m_IsNextCombo(false)
{
    m_CutterBulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalCutter.pref", L"prefab\\MetalCutter.pref");
    m_Hat = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalHat.pref", L"prefab\\MetalHat.pref");
    m_Weapon = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\KirbyMetalCutterWeapon.pref", L"prefab\\KirbyMetalCutterWeapon.pref");

}

CKirbyAbility_Cutter::CKirbyAbility_Cutter(const CKirbyAbility_Cutter& _Origin)
    : m_IsNextCombo(_Origin.m_IsNextCombo)
{
    m_CutterBulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalCutter.pref", L"prefab\\MetalCutter.pref");
}

CKirbyAbility_Cutter::~CKirbyAbility_Cutter()
{
}

void CKirbyAbility_Cutter::Attack()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"IDLE");
    }
}

void CKirbyAbility_Cutter::AttackEnter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("MetalCutterThrow"), false, false, 1.5f);

    // �����ʿ� Blade ��������ϱ�
    CKirbyHatBlade* HatBladeScript = dynamic_cast<CKirbyHatBlade*>(PLAYERFSM->GetCurHatBlade()->GetScripts()[0]);

    if (HatBladeScript != nullptr)
    {
        HatBladeScript->Attack();
    }

    // Bullet �߻�
    // Prefab ��������
    if (m_CutterBulletPrefab == nullptr)
    {
        m_CutterBulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalCutter.pref", L"prefab\\MetalCutter.pref");
    }

    // �ʱⰪ ����
    CGameObject* InstObj = m_CutterBulletPrefab->Instantiate();
    Vec3 KirbyPos = PLAYER->Transform()->GetWorldPos();
    Vec3 CurInputWorld = PLAYERCTRL->GetInputWorld();

    Vec3 Offset = Vec3(0.f, 20.f, 0.f);

    if (CurInputWorld == Vec3(0.f, 0.f, 0.f))
    {
        CurInputWorld = PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    }


    // Transform
    InstObj->Transform()->SetWorldPos(KirbyPos + Offset);
    InstObj->Transform()->SetDirection(CurInputWorld);

    GamePlayStatic::SpawnGameObject(InstObj, InstObj->GetLayerIdx());
}

void CKirbyAbility_Cutter::AttackExit()
{
}

void CKirbyAbility_Cutter::AttackCombo1()
{
    if (KEY_TAP(KEY_ATK))
    {
        m_IsNextCombo = true;
    }

    if (PLAYER->Animator()->IsFinish())
    {
        if (m_IsNextCombo)
        {
            ChangeState(L"ATTACK_COMBO2");
        }
        else
        {
            ChangeState(L"IDLE");
        }
    }
}

void CKirbyAbility_Cutter::AttackCombo1Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("CutterSlashChain1"), false, false, 2.f);

    m_IsNextCombo = false;

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetFriction(5.f);
    PLAYERCTRL->SetFrictionMode(true);

    // Material ���
    Ptr<CMaterial> CutterMaterial = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalCutter_BladeC.mtrl", L"material\\MetalCutter_BladeC.mtrl");

    PLAYERFSM->GetCurWeapon()->SetActive(true);
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(CutterMaterial, 0);

    // �Ӹ��� Blade�� �Ⱥ��̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(nullptr, 0);
}

void CKirbyAbility_Cutter::AttackCombo1Exit()
{
    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetFriction(0.f);
    PLAYERCTRL->SetFrictionMode(false);

    // Material ����
    PLAYERFSM->GetCurWeapon()->SetActive(false);
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // �Ӹ��� Blade�� ���̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(
        CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalHat_BladeC.mtrl", L"material\\MetalHat_BladeC.mtrl"), 0);
}

void CKirbyAbility_Cutter::AttackCombo2()
{
    if (KEY_TAP(KEY_ATK))
    {
        m_IsNextCombo = true;
    }

    if (PLAYER->Animator()->IsFinish())
    {
        if (m_IsNextCombo)
        {
            ChangeState(L"FINALCUTTERRISE");
        }
        else
        {
            ChangeState(L"IDLE");
        }
    }
}

void CKirbyAbility_Cutter::AttackCombo2Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("CutterSlashChain2"), false, false, 2.f);
    m_IsNextCombo = false;

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetFriction(5.f);
    PLAYERCTRL->SetFrictionMode(true);

    // Material ���
    PLAYERFSM->GetCurWeapon()->SetActive(true);
    Ptr<CMaterial> CutterMaterial = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalCutter_BladeC.mtrl", L"material\\MetalCutter_BladeC.mtrl");
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(CutterMaterial, 0);

    // �Ӹ��� Blade�� �Ⱥ��̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(nullptr, 0);
}

void CKirbyAbility_Cutter::AttackCombo2Exit()
{
    // Material ����
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetFriction(0.f);
    PLAYERCTRL->SetFrictionMode(false);

    // Material ����
    PLAYERFSM->GetCurWeapon()->SetActive(false);
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // �Ӹ��� Blade�� ���̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(
        CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalHat_BladeC.mtrl", L"material\\MetalHat_BladeC.mtrl"), 0);
}

void CKirbyAbility_Cutter::ChangeAbility()
{
}

void CKirbyAbility_Cutter::ChangeAbilityEnter()
{
    // Prefab ��������
    if (m_CutterBulletPrefab == nullptr)
    {
        m_CutterBulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalCutter.pref", L"prefab\\MetalCutter.pref");
    }

    // ���Ͽ� ���� �����ֱ�
    CGameObject* pInstObj = m_Hat->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Hat");

    // FSM�� ���� ���
    PLAYERFSM->SetCurHat(pInstObj);

    // HatBlade ���� ������ �����ֱ�
    pInstObj = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\MetalHatBlade.pref", L"prefab\\MetalHatBlade.pref")->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Hat");

    // FSM�� HatBlade ����
    PLAYERFSM->SetCurHatBlade(pInstObj);
    PLAYERFSM->SetBladeAttack(true);

    // ���� ����
    pInstObj = m_Weapon->Instantiate();
    GamePlayStatic::AddChildObject(PLAYER, pInstObj, L"Weapon");

    // �ִϸ��̼� ���
    pInstObj->Animator()->Play(ANIMPREFIX("Hold"), true, false, 1.f, 0);

    // FSM�� ���� ���
    PLAYERFSM->SetCurWeapon(pInstObj);

    // ���Ⱑ �Ⱥ��̵��� ����
    pInstObj->SetActive(false);
    pInstObj->MeshRender()->SetMaterial(nullptr, 0);

    // �ִϸ��̼� ���
    pInstObj->Animator()->Play(ANIMPREFIX("BladeWait"));
}

void CKirbyAbility_Cutter::ChangeAbilityExit()
{
}
